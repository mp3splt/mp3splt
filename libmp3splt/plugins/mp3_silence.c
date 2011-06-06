/**********************************************************
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2011 Alexandru Munteanu - io_fx@yahoo.fr
 *
 *********************************************************/

/**********************************************************
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307,
 * USA.
 *********************************************************/

#include <stdio.h>

#include "mp3.h"
#include "mp3_silence.h"
#include "mp3_utils.h"

static short scan_silence_processor(unsigned long time, int silence_was_found, void *data, 
    int *found_silence_points, int *error);

static scan_silence_data *scan_silence_data_new(splt_state *state, short first, unsigned long length, float min);
static void free_scan_silence_data(scan_silence_data **ssd);

static void splt_scan_silence_and_process(splt_state *state, off_t begin_offset, 
    float max_threshold, unsigned long length, 
    short process_silence(unsigned long time, int silence_was_found, void *data, int *found, int *error),
    void *data, int *error);
static int splt_mp3_silence(splt_mp3_state *mp3state, int channels, mad_fixed_t threshold);

/*! scan for silence

\return 
 - the number of silence points found
 - -1 on error
\param state The central structure libmp3splt keeps all of its data in
\param error Contains the error number if an error has occoured
\param length The time length to scan [in seconds]
\param threshold The threshold that tells noise from silence
*/
int splt_mp3_scan_silence(splt_state *state, off_t begin, unsigned long length,
    float threshold, float min, short output, int *error)
{
  scan_silence_data *ssd = scan_silence_data_new(state, output, length, min); 
  if (ssd == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return -1;
  }

  splt_scan_silence_and_process(state, begin, threshold, length, scan_silence_processor, ssd, error);

  int found = ssd->found;

  free_scan_silence_data(&ssd);

  return found;
}

static void splt_scan_silence_and_process(splt_state *state, off_t begin_offset, 
    float max_threshold, unsigned long length, 
    short process_silence(unsigned long time, int silence_was_found, void *data, int *found, int *error),
    void *data, int *error)
{
  int found = 0;
  short stop = SPLT_FALSE;

  mad_fixed_t threshold = mad_f_tofixed(splt_co_convert_from_dB(max_threshold));
  splt_mp3_state *mp3state = state->codec;

  splt_c_put_progress_text(state, SPLT_PROGRESS_SCAN_SILENCE);

  //seek to the begin
  if (fseeko(mp3state->file_input, begin_offset, SEEK_SET) == -1)
  {
    splt_e_set_strerror_msg_with_data(state, splt_t_get_filename_to_split(state));
    *error = SPLT_ERROR_SEEKING_FILE;
    return;
  }

  //initialise mad stuff
  splt_mp3_init_stream_frame(mp3state);
  mad_synth_init(&mp3state->synth);

  mad_timer_reset(&mp3state->timer);

  mp3state->temp_level = 0.0;

  do {
    int mad_err = SPLT_OK;

    switch (splt_mp3_get_valid_frame(state, &mad_err))
    {
      case 1:
        //1 we have a valid frame
        mad_timer_add(&mp3state->timer, mp3state->frame.header.duration);
        mad_synth_frame(&mp3state->synth, &mp3state->frame);
        unsigned long time = (unsigned long) mad_timer_count(mp3state->timer, MAD_UNITS_CENTISECONDS);

        int silence_was_found =
          splt_mp3_silence(mp3state, MAD_NCHANNELS(&mp3state->frame.header), threshold);
 
        int err = SPLT_OK;
        stop = process_silence(time, silence_was_found, data, &found, &err);
        if (stop && (err < 0)) { *error = err; }

        if (mp3state->mp3file.len > 0)
        {
          off_t pos = ftello(mp3state->file_input);

          float level = splt_co_convert_to_dB(mad_f_todouble(mp3state->temp_level));
          if (state->split.get_silence_level)
          {
            state->split.get_silence_level(time, level, state->split.silence_level_client_data);
          }
          state->split.p_bar->silence_db_level = level;
          state->split.p_bar->silence_found_tracks = found;

          //if we don't have silence split,
          //put the 1/4 of progress
          if (splt_o_get_int_option(state, SPLT_OPT_SPLIT_MODE) != SPLT_OPTION_SILENCE_MODE)
          {
            splt_c_update_progress(state,(double)(time),
                (double)(length), 4,1/(float)4, SPLT_DEFAULT_PROGRESS_RATE);
          }
          else
          {
            if (splt_t_split_is_canceled(state))
            {
              //split cancelled
              stop = SPLT_TRUE;
            }
            splt_c_update_progress(state,(double)pos,
                (double)(mp3state->mp3file.len), 1,0,SPLT_DEFAULT_PROGRESS_RATE);
          }
        }
        break;
      case 0:
        //0 do nothing
        break;
      case -1:
        //-1 means eof
        stop = SPLT_TRUE;
        break;
      case -3:
        //error from libmad
        stop = SPLT_TRUE;
        *error = mad_err;
        break;
      default:
        break;
    }

  } while (!stop);

  //only if we have silence mode, we set progress to 100%
  if (splt_o_get_int_option(state, SPLT_OPT_SPLIT_MODE) == SPLT_OPTION_SILENCE_MODE)
  {
    splt_c_update_progress(state,1.0,1.0,1,1,1);
  }

  //finish with mad_*
  splt_mp3_finish_stream_frame(mp3state);
  mad_synth_finish(&mp3state->synth);
}

static short scan_silence_processor(unsigned long time, int silence_was_found, void *data, 
    int *found_silence_points, int *error)
{
  scan_silence_data *ssd = (scan_silence_data *) data;

  short stop = SPLT_FALSE;

  if (ssd->length > 0 && time >= ssd->length)
  {
    ssd->flush = SPLT_TRUE;
    stop = SPLT_TRUE;
  }

  if (!ssd->flush && silence_was_found)
  {
    if (ssd->len == 0)
    {
      ssd->silence_begin = time;
    }

    if (ssd->first == SPLT_FALSE) 
    {
      ssd->len++;
    }

    if (ssd->shot < SPLT_DEFAULTSHOT)
    {
      ssd->shot += 2;
    }

    ssd->silence_end = time;

    *found_silence_points = ssd->found;
    return stop;
  }

  if (ssd->len > SPLT_DEFAULTSILLEN)
  {
    if (ssd->flush || (ssd->shot <= 0))
    {
      double begin_position = (double) (ssd->silence_begin / 100.f);
      double end_position = (double) (ssd->silence_end / 100.f);
      ssd->len = (int) (ssd->silence_end - ssd->silence_begin);

      if ((end_position - begin_position - ssd->min) >= 0.f)
      {
        if (splt_siu_ssplit_new(&ssd->state->silence_list,
              begin_position, end_position, ssd->len, error) == -1)
        {
          ssd->found = -1;
          *found_silence_points = ssd->found;
          return SPLT_TRUE;
        }

        ssd->found++;
      }

      ssd->len = 0;
      ssd->shot = SPLT_DEFAULTSHOT;
    }
  }
  else 
  {
    ssd->len = 0;
  }

  if (ssd->first && (ssd->shot <= 0))
  {
    ssd->first = SPLT_FALSE;
  }

  if (ssd->shot > 0) 
  {
    ssd->shot--;
  }

  *found_silence_points = ssd->found;

  return stop;
}

static scan_silence_data *scan_silence_data_new(splt_state *state, 
    short first, unsigned long length, float min)
{
  scan_silence_data *ssd = malloc(sizeof(scan_silence_data));
  if (!ssd)
  {
    return NULL;
  }

  ssd->first = first;
  ssd->flush = SPLT_FALSE;
  ssd->silence_begin = 0;
  ssd->silence_end = 0;
  ssd->len = 0;
  ssd->found = 0;
  ssd->shot = SPLT_DEFAULTSHOT;

  ssd->length = length;
  ssd->min = min;
  ssd->state = state;

  return ssd;
}

static void free_scan_silence_data(scan_silence_data **ssd)
{
  if (!ssd || !*ssd)
  {
    return;
  }

  free(*ssd);
  *ssd = NULL;
}

/*!  Compare the noise level with threshold

Used by mp3_scan_silence

\return 
 - 0 if silence spot > threshold, 
 - 1 otherwise

Always computes only one frame
*/
static int splt_mp3_silence(splt_mp3_state *mp3state, int channels, mad_fixed_t threshold)
{
  int i, j;
  mad_fixed_t sample;
  int silence = 1;

  for (j = 0; j < channels; j++)
  {
    for(i = 0; i < mp3state->synth.pcm.length; i++)
    {
      //get silence spot ?
      sample = mad_f_abs(mp3state->synth.pcm.samples[j][i]);
      mp3state->temp_level = mp3state->temp_level * 0.999 + sample * 0.001;

      if (sample > threshold)
      {
        silence = 0;
      }
    }
  }

  return silence;
}

