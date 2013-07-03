/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2013 Alexandru Munteanu - <m@ioalex.net>
 *
 * Parts of this file have been copied from the 'vcut' 1.6
 * program provided with 'vorbis-tools' :
 *      vcut (c) 2000-2001 Michael Smith <msmith@xiph.org>
 *
 * Some parts from a more recent version of vcut :
 *           (c) 2008 Michael Gold <mgold@ncf.ca>
 *
 * http://mp3splt.sourceforge.net
 *
 *********************************************************/

/**********************************************************
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 *
 *********************************************************/

#include <string.h>
#include <math.h>

#include "ogg.h"
#include "ogg_silence.h"
#include "ogg_utils.h"
#include "silence_processors.h"
#include "ogg_new_stream_handler.h"

static void splt_ogg_scan_silence_and_process(splt_state *state, short seconds,
    float max_threshold, ogg_page *page,  ogg_int64_t granpos, ogg_int64_t first_cut_granpos,
    short process_silence(double time, float level, int silence_was_found, short must_flush,
      splt_scan_silence_data *ssd, int *found, int *error),
    splt_scan_silence_data *ssd, int *error);
static int splt_ogg_silence(splt_ogg_state *oggstate, vorbis_dsp_state *vd, float threshold);

int splt_ogg_scan_silence(splt_state *state, short seconds, float threshold, 
    float min, int shots, short output, ogg_page *page, ogg_int64_t granpos,
    int *error, ogg_int64_t first_cut_granpos,
    short silence_processor(double time, float level, int silence_was_found, short must_flush,
      splt_scan_silence_data *ssd, int *found, int *error))
{
  splt_scan_silence_data *ssd = splt_scan_silence_data_new(state, output, min, shots, SPLT_FALSE);
  if (ssd == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return -1;
  }

  splt_ogg_scan_silence_and_process(state, seconds, threshold, page, granpos, first_cut_granpos, silence_processor, ssd, error);

  int found = ssd->found;

  splt_free_scan_silence_data(&ssd);

  if (*error < 0)
  {
    found = -1;
  }

  return found;
}

static void splt_ogg_scan_silence_and_process(splt_state *state, short seconds,
    float max_threshold, ogg_page *page,  ogg_int64_t granpos, ogg_int64_t first_cut_granpos,
    short process_silence(double time, float level, int silence_was_found, short must_flush,
      splt_scan_silence_data *ssd, int *found, int *error),
    splt_scan_silence_data *ssd, int *error)
{
  splt_c_put_progress_text(state, SPLT_PROGRESS_SCAN_SILENCE);

  splt_ogg_state *oggstate = state->codec;

  ogg_stream_state os;
  ogg_stream_init(&os, oggstate->serial);

  splt_ogg_new_stream_handler *ogg_new_stream_handler = 
    splt_ogg_nsh_new(state, oggstate, NULL, NULL, SPLT_FALSE, &os);
  if (ogg_new_stream_handler == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return;
  }

  ogg_int64_t pos = granpos;

  off_t initial_file_offset = ftello(oggstate->in);
  int saveW = oggstate->prevW;

  ogg_page og;

  float threshold = splt_co_convert_from_db(max_threshold);

  int result = 0;

  vorbis_dsp_state vd;
  vorbis_synthesis_init(&vd, oggstate->vi);

  vorbis_block vb;
  vorbis_block_init(&vd, &vb);

  ogg_sync_state oy;
  ogg_sync_init(&oy);

  int split_type = splt_o_get_int_option(state, SPLT_OPT_SPLIT_MODE);
  short option_silence_mode =
    (split_type == SPLT_OPTION_SILENCE_MODE || split_type == SPLT_OPTION_TRIM_SILENCE_MODE);

  int found = 0;

  //still have a page to process
  if (page)
  {
    memcpy(&og, page, sizeof(ogg_page));
    result = 1;
  }

  if (option_silence_mode)
  {
    memcpy(&oy, oggstate->sync_in, sizeof(*oggstate->sync_in));

    size_t storage_to_copy = oggstate->sync_in->storage * sizeof(unsigned char);

    oy.data = malloc(storage_to_copy);
    if (oy.data == NULL)
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      goto function_end;
    }

    memcpy(oy.data, oggstate->sync_in->data, storage_to_copy);
  }

  ogg_int64_t end = 0, begin = 0;

  if (seconds > 0)
  {
    end = (ogg_int64_t) (seconds * oggstate->vi->rate);
  }

  oggstate->temp_level = 0.0;

  short is_stream = SPLT_FALSE;
  long stream_time0 = 0;

  int eos = 0;
  ogg_int64_t page_granpos;
  ogg_packet op;

  double previous_streams_time_in_double = 0;
  double time_in_double = 0;

  while (!eos)
  {
    while (!eos)
    {
      if (result == 0) 
      {
        break;
      }

      if (result > 0)
      {
        if (ogg_page_bos(&og) &&
            (oggstate->saved_serial != ogg_page_serialno(&og)))
        {
          splt_ogg_initialise_for_new_stream(ogg_new_stream_handler, &og, NULL, 0);
          oggstate->cutpoint_begin = 0;
          previous_streams_time_in_double = time_in_double;
        }

        page_granpos = ogg_page_granulepos(&og) - oggstate->cutpoint_begin;

        if (pos == 0) 
        {
          pos = page_granpos;
        }
        ogg_stream_pagein(&os, &og);

        while (1)
        {
          result = ogg_stream_packetout(&os, &op);

          //need more data
          if (result == 0) 
          {
            break;
          }

          if (result > 0)
          {
            if (splt_ogg_new_stream_needs_header_packet(ogg_new_stream_handler))
            {
              splt_ogg_new_stream_handle_header_packet(ogg_new_stream_handler, &op, error);
              if (*error < 0) { goto function_end; }
              continue;
            }

            int bs = splt_ogg_get_blocksize(oggstate, oggstate->vi, &op);

            //we currently loose the first packet when using the
            //auto adjust option because we are out of sync,
            //so we disable the continuity check
            if (!splt_o_get_int_option(state, SPLT_OPT_AUTO_ADJUST))
            {
              ogg_int64_t first_granpos = splt_ogg_compute_first_granulepos(state, oggstate, &op, bs);
              if (first_cut_granpos == 0 && first_granpos != 0)
              {
                is_stream = SPLT_TRUE;
                first_cut_granpos = first_granpos;
                pos += first_cut_granpos;
              }
            }

            pos += bs;                      
            if (pos > page_granpos)
            {
              pos = page_granpos;
            }
            begin += bs;

            if (vorbis_synthesis(&vb, &op) != 0)
            {
              splt_e_set_error_data(state, splt_t_get_filename_to_split(state));
              *error = SPLT_ERROR_INVALID;
              goto function_end;
            }

            vorbis_synthesis_blockin(&vd, &vb);
            int silence_was_found = splt_ogg_silence(oggstate, &vd, threshold);

            int err = SPLT_OK;
            short must_flush = (end && begin > end);
            time_in_double = (double) (pos - first_cut_granpos);
            time_in_double /= oggstate->vi->rate;
            time_in_double += previous_streams_time_in_double;

            float level = splt_co_convert_to_db(oggstate->temp_level);
            if (level < -96.0) { level = -96.0; }
            if (level > 0) { level = 0; }

            int stop = process_silence(time_in_double, level, silence_was_found, must_flush, ssd, &found, &err);
            if (stop || stop == -1)
            {
              eos = 1;
              if (err < 0) { *error = err; goto function_end; }

              if (stop == -1)
                break;
            }

            //BEGIN silence callbacks
            if (state->split.get_silence_level)
            {
              long time = (long) (((double) pos / oggstate->vi->rate) * 100.0);
              if (is_stream && stream_time0 == 0 && time != 0)
              {
                stream_time0 = time;
                //          fprintf(stdout, "stream_time0 = %ld\n", stream_time0);
                //          fflush(stdout);
              }

              //        fprintf(stdout, "level = %f, time = %ld, time - stream_time0 = %ld\n", 
              //            level, time, (long) (time - stream_time0));
              //        fflush(stdout);

              state->split.get_silence_level(time - stream_time0, level,
                  state->split.silence_level_client_data);
            }
            state->split.p_bar->silence_db_level = level;
            state->split.p_bar->silence_found_tracks = found;

            if (option_silence_mode)
            {
              if (splt_t_split_is_canceled(state))
              {
                eos = 1;
              }
              splt_c_update_progress(state,(double)pos * 100,
                  (double)(oggstate->len), 1,0,SPLT_DEFAULT_PROGRESS_RATE2);
            }
            else
            {
              //fprintf(stdout, "%lf/%lf\n", (double)begin, (double)end);
              //fflush(stdout);
              //TODO: bug
              splt_c_update_progress(state,(double)begin, (double)end, 4, 0.5, SPLT_DEFAULT_PROGRESS_RATE2);
            }
            //END silence callbacks

          }
        }
      }

      result = ogg_sync_pageout(&oy, &og);
//      long page_number = ogg_page_pageno(&og);
//      fprintf(stdout,"X page number = %ld\n", page_number);
//      fflush(stdout);
    }

    if (!eos)
    {
      int sync_bytes = splt_ogg_update_sync(state, &oy, oggstate->in, error);
      if (sync_bytes == 0)
      {
        eos = 1;
      }
      else if (sync_bytes == -1)
      {
        goto function_end;
      }

      result = ogg_sync_pageout(&oy, &og);

//      if (result != -1)
//      {
//        long page_number = ogg_page_pageno(&og);
//        fprintf(stdout,"Y page number = %ld\n", page_number);
//        fflush(stdout);
//      }
    }
  }

  int junk;
  int err = SPLT_OK;
  process_silence(-1, -96, SPLT_FALSE, SPLT_FALSE, ssd, &junk, &err);
  if (err < 0) { *error = err; }

function_end:

  ogg_stream_clear(&os);

  vorbis_block_clear(&vb);
  vorbis_dsp_clear(&vd);

  if (option_silence_mode)
  {
    if (oy.data)
    {
      free(oy.data);
      oy.data = NULL;
    }
  }
  ogg_sync_clear(&oy);

  oggstate->prevW = saveW;
  if (fseeko(oggstate->in, initial_file_offset, SEEK_SET) == -1)
  {
    splt_e_set_strerror_msg_with_data(state, splt_t_get_filename_to_split(state));
    *error = SPLT_ERROR_SEEKING_FILE;
  }

  splt_ogg_nsh_free(&ogg_new_stream_handler);
}

static int splt_ogg_silence(splt_ogg_state *oggstate, vorbis_dsp_state *vd, float threshold)
{
  float **pcm = NULL, sample;
  int samples, silence = 1;

  while ((samples = vorbis_synthesis_pcmout(vd, &pcm)) > 0)
  {
    if (silence) 
    {
      int i, j;
      for (i = 0; i < oggstate->vi->channels; i++)
      {
        float *mono=pcm[i];
        if (!silence) 
        {
          break;
        }
        for(j = 0; j < samples; j++)
        {
          float single_mono = mono[j];
          sample = fabs(single_mono);
          oggstate->temp_level = oggstate->temp_level * 0.999 + sample * 0.001;
          if (sample > threshold)
          {
            silence = 0;
          }
        }
      }
    }

    vorbis_synthesis_read(vd, samples);
  }

  return silence;
}



