/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2014 Alexandru Munteanu - <m@ioalex.net>
 *
 * Large parts of this files have been copied from the 'vcut' 1.6
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

/*! \file

The Plug-in that handles ogg vorbis files
*/

#include <time.h>
#include <string.h>
#include <locale.h>

#ifdef __WIN32__
#include <io.h>
#include <fcntl.h>
#endif

#include "splt.h"
#include "ogg.h"
#include "ogg_utils.h"
#include "ogg_silence.h"
#include "ogg_new_stream_handler.h"

#define FIRST_GRANPOS 1

/**
 * Notes:
 * - if ogg_stream_init() returns -1, then the stream was incorrectly
 *   initialized => coding error
 * - some 'vorbis_synthesis_*' functions return OV_EINVAL for bad argument
 *   but we don't check that error code because this is not supposed to
 *   happend; if it happends, it's a coding error
 **/

/****************************/
/* some function prototypes */

splt_ogg_state *splt_ogg_info(FILE *in, splt_state *state, int *error);
static void splt_ogg_clear_sync_in_and_free(splt_ogg_state *oggstate);

/****************************/
/* ogg utils */

FILE *splt_ogg_open_file_read(splt_state *state, const char *filename, int *error)
{
  FILE *file_input = NULL;

  if (strcmp(filename,"o-") == 0)
  {
    file_input = stdin;
#ifdef __WIN32__
    _setmode(fileno(file_input), _O_BINARY);
#endif
  }
  else
  {
    //we open the file
    file_input = splt_io_fopen(filename, "rb");
    if (file_input == NULL)
    {
      splt_e_set_strerror_msg_with_data(state, filename);
      *error = SPLT_ERROR_CANNOT_OPEN_FILE;
    }
  }

  return file_input;
}

//gets the mp3 info and puts it in the state
void splt_ogg_get_info(splt_state *state, FILE *file_input, int *error)
{
  //checks if valid ogg file
  state->codec = splt_ogg_info(file_input, state, error);

  //if error
  if ((*error < 0) || (state->codec == NULL))
  {
    return;
  }
  else
  {
    //put file infos to client 
    if (! splt_o_messages_locked(state))
    {
      splt_ogg_state *oggstate = state->codec;

      char ogg_infos[1024] = { '\0' };
      snprintf(ogg_infos, 1023, 
          _(" info: Ogg Vorbis Stream - %ld - %ld Kb/s - %d channels"),
          oggstate->vd->vi->rate, oggstate->vd->vi->bitrate_nominal/1024,
          oggstate->vd->vi->channels);

      char total_time[256] = { '\0' };
      int total_seconds = (int) splt_t_get_total_time(state) / 100;
      int minutes = total_seconds / 60;
      int seconds = total_seconds % 60;
      snprintf(total_time, 255, _(" - Total time: %dm.%02ds"), minutes, seconds%60);

      splt_c_put_info_message_to_client(state, "%s%s\n", ogg_infos, total_time);
    }
  }
}

/* Full cleanup of internal state and vorbis/ogg structures */
static void splt_ogg_v_free(splt_ogg_state *oggstate)
{
  if (!oggstate)
  {
    return;
  }

  if(oggstate->packets)
  {
    splt_ogg_free_packet(&oggstate->packets[0]);
    splt_ogg_free_packet(&oggstate->packets[1]);
    free(oggstate->packets);
    oggstate->packets = NULL;
  }

  splt_ogg_free_oggstate_headers(oggstate);

  splt_ogg_free_vorbis_comment(&oggstate->vc, oggstate->cloned_vorbis_comment);
  oggstate->cloned_vorbis_comment = 2;

  if(oggstate->vb)
  {
    vorbis_block_clear(oggstate->vb);
    free(oggstate->vb);
    oggstate->vb = NULL;
  }

  if(oggstate->vd)
  {
    vorbis_dsp_clear(oggstate->vd);
    free(oggstate->vd);
    oggstate->vd = NULL;
  }

  //only free the input if different from stdin
  if(oggstate->stream_in && oggstate->in != stdin)
  {
    ogg_stream_clear(oggstate->stream_in);
    free(oggstate->stream_in);
    oggstate->stream_in = NULL;
  }

  splt_ogg_clear_sync_in_and_free(oggstate);

  if (oggstate->vi)
  {
    vorbis_info_clear(oggstate->vi);
    free(oggstate->vi);
    oggstate->vi = NULL;
  }

  free(oggstate);
  oggstate = NULL;
}

static void splt_ogg_clear_sync_in_and_free(splt_ogg_state *oggstate)
{
  if (oggstate->sync_in)
  {
    ogg_sync_clear(oggstate->sync_in);

    free(oggstate->sync_in);
    oggstate->sync_in = NULL;
  }
}

static splt_ogg_state *splt_ogg_v_new(int *error)
{
  splt_ogg_state *oggstate = NULL;

  if ((oggstate = malloc(sizeof(splt_ogg_state)))==NULL)
  {
    goto error;
  }
  memset(oggstate, 0, sizeof(splt_ogg_state));
  if ((oggstate->sync_in = malloc(sizeof(ogg_sync_state)))==NULL)
  {
    goto error;
  }
  if ((oggstate->stream_in = malloc(sizeof(ogg_stream_state)))==NULL)
  {
    goto error;
  }
  if ((oggstate->vd = malloc(sizeof(vorbis_dsp_state)))==NULL)
  {
    goto error;
  }
  if ((oggstate->vi = malloc(sizeof(vorbis_info)))==NULL)
  {
    goto error;
  }
  if ((oggstate->vb = malloc(sizeof(vorbis_block)))==NULL)
  {
    goto error;
  }

  if ((oggstate->headers = malloc(sizeof(splt_v_packet) * TOTAL_HEADER_PACKETS))==NULL)
  {
    goto error;
  }
  memset(oggstate->headers, 0, sizeof(splt_v_packet) * TOTAL_HEADER_PACKETS);

  if ((oggstate->packets = malloc(sizeof(splt_v_packet) * 2))==NULL)
  {
    goto error;
  }
  memset(oggstate->packets, 0, sizeof(splt_v_packet) * 2);

  oggstate->cloned_vorbis_comment = 2;

  return oggstate;

error:
  *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  splt_ogg_v_free(oggstate);
  return NULL;
}

//frees the splt_ogg_state structure,
//used in the splt_t_state_free() function
void splt_ogg_state_free(splt_state *state)
{
  splt_ogg_state *oggstate = state->codec;
  if (oggstate)
  {
    ov_clear(&oggstate->vf);
    splt_ogg_v_free(oggstate);
    state->codec = NULL;
  }
}

/****************************/
/* ogg tags */

//macro used only in the following function splt_ogg_get_original_tags
#define OGG_VERIFY_ERROR() \
if (err != SPLT_OK) \
{ \
*tag_error = err; \
return; \
};
//get the original ogg tags and put them in the state
void splt_ogg_get_original_tags(const char *filename,
    splt_state *state, int *tag_error)
{
  splt_ogg_state *oggstate = state->codec;

  vorbis_comment *vc_local = NULL;
  vc_local = ov_comment(&oggstate->vf,-1);
  int err = SPLT_OK;

  char *a = NULL,*t = NULL,*al = NULL,*da = NULL, *g = NULL,*tr = NULL,
       *com = NULL;

  int has_tags = SPLT_FALSE;

  a = vorbis_comment_query(vc_local, SPLT_OGG_ARTIST, 0);
  if (a != NULL)
  {
    err = splt_tu_set_original_tags_field(state, SPLT_TAGS_ARTIST, a);
    has_tags = SPLT_TRUE;
    OGG_VERIFY_ERROR();
  }

  t = vorbis_comment_query(vc_local, SPLT_OGG_TITLE, 0);
  if (t != NULL)
  {
    err = splt_tu_set_original_tags_field(state, SPLT_TAGS_TITLE, t);
    has_tags = SPLT_TRUE;
    OGG_VERIFY_ERROR();
  }

  al = vorbis_comment_query(vc_local, SPLT_OGG_ALBUM, 0);
  if (al != NULL)
  {
    err = splt_tu_set_original_tags_field(state, SPLT_TAGS_ALBUM, al);
    has_tags = SPLT_TRUE;
    OGG_VERIFY_ERROR();
  }

  da = vorbis_comment_query(vc_local, SPLT_OGG_DATE, 0);
  if (da != NULL)
  {
    err = splt_tu_set_original_tags_field(state, SPLT_TAGS_YEAR, da);
    has_tags = SPLT_TRUE;
    OGG_VERIFY_ERROR();
  }

  g = vorbis_comment_query(vc_local, SPLT_OGG_GENRE, 0);
  if (g != NULL)
  {
    err = splt_tu_set_original_tags_field(state, SPLT_TAGS_GENRE, g);
    has_tags = SPLT_TRUE;
    OGG_VERIFY_ERROR();
  }

  tr = vorbis_comment_query(vc_local, SPLT_OGG_TRACKNUMBER, 0);
  if (tr != NULL)
  {
    int track = atoi(tr);
    err = splt_tu_set_original_tags_field(state, SPLT_TAGS_TRACK, &track);
    has_tags = SPLT_TRUE;
    OGG_VERIFY_ERROR();
  }

  com = vorbis_comment_query(vc_local, SPLT_OGG_COMMENT, 0);
  if (com != NULL)
  {
    err = splt_tu_set_original_tags_field(state, SPLT_TAGS_COMMENT, com);
    has_tags = SPLT_TRUE;
    OGG_VERIFY_ERROR();
  }

  splt_tu_set_original_tags_field(state, SPLT_TAGS_VERSION, &has_tags);

  vorbis_comment *cloned_comment = splt_ogg_clone_vorbis_comment(vc_local);
  if (cloned_comment == NULL)
  {
    err = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    OGG_VERIFY_ERROR();
  }

  splt_tu_set_original_tags_data(state, cloned_comment);
}

/****************************/
/* ogg infos */

//Pull out and save the 3 header packets from the input file.
//-returns -1 if error and error is set in '*error'
static int splt_ogg_read_headers_and_save_them(splt_state *state, splt_ogg_state *oggstate, int *error)
{
  ogg_page page;
  ogg_packet packet;
  int bytes = 0;
  int i = 0;
  char *buffer = NULL;

  ogg_sync_init(oggstate->sync_in);
  vorbis_info_init(oggstate->vi);

  int result = 0;
  while ((result = ogg_sync_pageout(oggstate->sync_in, &page))!=1)
  {
    buffer = ogg_sync_buffer(oggstate->sync_in, SPLT_OGG_BUFSIZE);
    if (buffer == NULL)
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      return -1;
    }
    bytes = fread(buffer, 1, SPLT_OGG_BUFSIZE, oggstate->in);
    if (bytes <= 0)
    {
      goto error_invalid_file;
    }
    if (ogg_sync_wrote(oggstate->sync_in, bytes) != 0)
    {
      goto error_invalid_file;
    }
  }

  oggstate->serial = ogg_page_serialno(&page);
  oggstate->saved_serial = ogg_page_serialno(&page);
  //how to handle alloc memory problem ?
  ogg_stream_init(oggstate->stream_in, oggstate->serial);
  if(ogg_stream_pagein(oggstate->stream_in, &page) < 0)
  {
    goto error_invalid_file;
  }
  //ogg doc says 'usually this will not be a fatal error'
  if(ogg_stream_packetout(oggstate->stream_in, &packet)!=1)
  {
    goto error_invalid_file;
  }
  //if bad header
  if(vorbis_synthesis_headerin(oggstate->vi, &oggstate->vc, &packet) < 0)
  {
    goto error_invalid_file;
  }
  oggstate->cloned_vorbis_comment = SPLT_FALSE;

  int packet_err = SPLT_OK;
  oggstate->headers[0] = splt_ogg_clone_packet(&packet, &packet_err);
  if (packet_err < 0)
  { 
    goto error;
  }

  i=0;
  while(i<2)
  {
    while(i<2)
    {
      int res = ogg_sync_pageout(oggstate->sync_in, &page);
      //res == -1 is NOT a fatal error
      if(res == 0)
      {
        break;
      }

      if(res == 1)
      {
        if (ogg_stream_pagein(oggstate->stream_in, &page) < 0)
        {
          goto error_invalid_file;
        }
        while(i<2)
        {
          res = ogg_stream_packetout(oggstate->stream_in, &packet);
          if(res==0)
          {
            break;
          }
          //ogg doc says 'usually this will not be a fatal error'
          if(res<0)
          {
            goto error_invalid_file;
          }

          oggstate->headers[i+1] = splt_ogg_clone_packet(&packet, &packet_err);
          if (packet_err < 0)
          {
            goto error;
          }
          //if bad header
          if (vorbis_synthesis_headerin(oggstate->vi,&oggstate->vc,&packet) < 0)
          {
            goto error_invalid_file;
          }
          oggstate->cloned_vorbis_comment = SPLT_FALSE;
          i++;
        }
      }
    }

    buffer=ogg_sync_buffer(oggstate->sync_in, SPLT_OGG_BUFSIZE);
    if (buffer == NULL)
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      goto error;
    }
    bytes=fread(buffer,1,SPLT_OGG_BUFSIZE,oggstate->in);

    if(bytes == 0 && i < 2)
    {
      goto error_invalid_file;
    }
    if (ogg_sync_wrote(oggstate->sync_in, bytes) != 0)
    {
      goto error_invalid_file;
    }
  }

  return 0;

error_invalid_file:
  *error = SPLT_ERROR_INVALID;
  splt_e_set_error_data(state, splt_t_get_filename_to_split(state));
error:
  return -1;
}

//returns ogg info
splt_ogg_state *splt_ogg_info(FILE *in, splt_state *state, int *error)
{
  splt_ogg_state *oggstate = state->codec;

  oggstate = splt_ogg_v_new(error);
  if (oggstate == NULL) { return NULL; }

  char *filename = splt_t_get_filename_to_split(state);

  oggstate->in = in;
  oggstate->end = 0;

  oggstate->total_blocksize = -1;
  oggstate->first_granpos = 0;

  //open the file
  if (oggstate->in != stdin)
  {
    int ret = ov_open(oggstate->in, &oggstate->vf, NULL, 0);
    if(ret < 0)
    {
      splt_e_set_error_data(state, filename);
      switch (ret)
      {
        case OV_EREAD:
          *error = SPLT_ERROR_WHILE_READING_FILE;
          break;
        default:
          splt_e_set_error_data(state, splt_t_get_filename_to_split(state));
          *error = SPLT_ERROR_INVALID;
          break;
      }
      splt_ogg_v_free(oggstate);
      return NULL;
    }
    //go at the start of the file
    rewind(oggstate->in);
  }

  /* Read headers in, and save them */
  if (splt_ogg_read_headers_and_save_them(state, oggstate, error) == -1)
  {
    splt_ogg_v_free(oggstate);
    return NULL;
  }

  if (oggstate->in != stdin)
  {
    //read total time
    double total_time = ov_time_total(&oggstate->vf, -1) * 100;
    splt_t_set_total_time(state, total_time);
    oggstate->len = (ogg_int64_t) (oggstate->vi->rate * total_time);
  }

  oggstate->cutpoint_begin = 0;
  //what to do if no memory ?
  //- we must free memory from 'vorbis_synthesis_init' ?
  vorbis_synthesis_init(oggstate->vd, oggstate->vi);
  vorbis_block_init(oggstate->vd, oggstate->vb);

  srand(time(NULL));

  return oggstate;
}

/****************************/
/* ogg split */

/* Read stream until we get to the appropriate cut point.
 *
 * We need to do the following:
 *   - Save the final two packets in the stream to temporary buffers.
 *     These two packets then become the first two packets in the 2nd stream
 *     (we need two packets because of the overlap-add nature of vorbis).
 *   - For each packet, buffer it (it could be the 2nd last packet, we don't
 *     know yet (but we could optimise this decision based on known maximum
 *     block sizes, and call get_blocksize(), because this updates internal
 *     state needed for sample-accurate block size calculations.
 */
static int splt_ogg_find_begin_cutpoint(splt_state *state, splt_ogg_state *oggstate, 
    FILE *in, ogg_int64_t cutpoint, int *error, const char *filename,
    int save_end_point)
{
  splt_ogg_new_stream_handler *ogg_new_stream_handler = 
    splt_ogg_nsh_new(state, oggstate, NULL, NULL, SPLT_FALSE, NULL);
  if (ogg_new_stream_handler == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return -1;
  }

  int eos=0;
  ogg_page page;
  ogg_packet packet;
  ogg_int64_t granpos, prevgranpos;

  granpos = prevgranpos = 0;

  int packet_err = SPLT_OK;

  cutpoint += oggstate->first_granpos;

  while (!eos)
  {
    while (!eos)
    {
      int result = ogg_sync_pageout(oggstate->sync_in, &page);

      //result == -1 is NOT a fatal error
      if (result==0) 
      {
        break;
      }
      else 
      {
        //result==1 means that we have a good page
        if (result > 0)
        {
          if (ogg_page_bos(&page) &&
              (oggstate->saved_serial != ogg_page_serialno(&page)))
          {
            splt_ogg_initialise_for_new_stream(ogg_new_stream_handler, 
                &page, &cutpoint, prevgranpos);
          }

          granpos = ogg_page_granulepos(&page);

          /*long page_number = ogg_page_pageno(&page);
          fprintf(stdout,"page number = %ld, granule position = %ld, cutpoint = %ld\n",
              page_number, granpos, cutpoint);
          fflush(stdout);*/

          if (ogg_stream_pagein(oggstate->stream_in, &page) == -1)
          {
            splt_e_set_error_data(state, splt_t_get_filename_to_split(state));
            *error = SPLT_ERROR_INVALID;
            goto error;
          }

          //for a broken ogg file with no
          //header, we have granpos > cutpoint the first time
          if (granpos < cutpoint)
          {
            while (1)
            {
              result = ogg_stream_packetout(oggstate->stream_in, &packet);

              if (result == 0)
              {
                break;
              }

              //skip headers for overlap split - already processed once
              if (granpos == 0)
              {
                continue;
              }

              if (result != -1)
              {
                /*fprintf(stdout,"packet number = %ld packet granulepos = %ld\n",
                    packet.packetno, packet.granulepos);
                fflush(stdout);*/

                if (splt_ogg_new_stream_needs_header_packet(ogg_new_stream_handler))
                {
                  splt_ogg_new_stream_handle_header_packet(ogg_new_stream_handler, &packet, error);
                  if (*error < 0) { goto error; }
                  continue;
                }

                int bs = splt_ogg_get_blocksize(oggstate, oggstate->vi, &packet);

                cutpoint += splt_ogg_compute_first_granulepos(state, oggstate, &packet, bs);

                /* We need to save the last packet in the first
                 * stream - but we don't know when we're going
                 * to get there. So we have to keep every packet
                 * just in case.
                 */
                splt_ogg_free_packet(&oggstate->packets[0]);
                oggstate->packets[0] = splt_ogg_clone_packet(&packet, &packet_err);
                if (packet_err < 0) { goto error; }
              }
            }

            prevgranpos = granpos;
          }
          else
          {
            eos = 1;

            while ((result = ogg_stream_packetout(oggstate->stream_in, &packet)) != 0)
            {
              //skip headers for overlap split - already processed once
              if (granpos == 0)
              {
                continue;
              }

              //if == -1, we are out of sync; not a fatal error
              if (result != -1)
              {
                /*fprintf(stdout,"packet number bis = %ld packet granulepos = %ld\n",
                    packet.packetno, packet.granulepos);
                fflush(stdout);*/

                int bs = splt_ogg_get_blocksize(oggstate, oggstate->vi, &packet);
                prevgranpos += bs;

                ogg_int64_t old_first_granpos = oggstate->first_granpos;
                ogg_int64_t first_granpos = splt_ogg_compute_first_granulepos(state, oggstate, &packet, bs);
                cutpoint += first_granpos;
                if (first_granpos != 0)
                {
                  eos = 0;
                }
                if (old_first_granpos == 0)
                {
                  prevgranpos += first_granpos;
                }

                if (prevgranpos > cutpoint)
                {
                  splt_ogg_free_packet(&oggstate->packets[1]);
                  oggstate->packets[1] = splt_ogg_clone_packet(&packet, &packet_err);
                  if (packet_err < 0) { goto error; }
                  eos = 1;
                  break;
                }

                splt_ogg_free_packet(&oggstate->packets[0]);
                oggstate->packets[0] = splt_ogg_clone_packet(&packet, &packet_err);
                if (packet_err < 0) { goto error; }
              }
            }
          }

          if (ogg_page_eos(&page))
          {
            break;
          }
        }
      }
    }

    if (!eos)
    {
      int sync_bytes = splt_ogg_update_sync(state, oggstate->sync_in, in, error);
      if (sync_bytes == 0)
      {
        eos = 1;
      }
      else if (sync_bytes == -1)
      {
        goto error;
      }
    }
  }

  if (granpos < cutpoint)
  {
    *error = SPLT_ERROR_BEGIN_OUT_OF_FILE;
    goto error;
  }

  /* Remaining samples in first packet */
  oggstate->initialgranpos = prevgranpos - cutpoint;
  oggstate->cutpoint_begin = cutpoint;

  splt_ogg_nsh_free(&ogg_new_stream_handler);

  return 0;

error:
  splt_ogg_nsh_free(&ogg_new_stream_handler);

  return -1;
}

/* Process second stream.
 *
 * We need to do more packet manipulation here, because we need to calculate
 * a new granulepos for every packet, since the old ones are now invalid.
 * Start by placing the modified first and second packets into the stream.
 * Then just proceed through the stream modifying packno and granulepos for
 * each packet, using the granulepos which we track block-by-block.
 */
//Warning ! cutpoint is not the end cutpoint, but the length between the
//begin and the end
static int splt_ogg_find_end_cutpoint(splt_state *state, ogg_stream_state *stream,
    FILE *in, FILE *f, ogg_int64_t cutpoint, int adjust, float threshold, float min_length,
    int shots, int *error, const char *output_fname, int save_end_point,
    double *sec_split_time_length, double sec_end)
{
  splt_c_put_progress_text(state, SPLT_PROGRESS_CREATE);

  //TODO: eos not set on ripped streams on last split file

  splt_ogg_state *oggstate = state->codec;

  splt_ogg_new_stream_handler *ogg_new_stream_handler = 
    splt_ogg_nsh_new(state, oggstate, stream, output_fname, SPLT_TRUE, NULL);
  if (ogg_new_stream_handler == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return -1;
  }

  ogg_packet packet;
  ogg_page page;
  int eos=0;
  int result = 0;
  int after_adjust = 0;
  ogg_int64_t up_to_adjust_granpos = 0;
  ogg_int64_t page_granpos = 0, current_granpos = 0, prev_granpos = 0;
  ogg_int64_t packetnum = 0; /* Should this start from 0 or 2 ? */

  ogg_int64_t first_cut_granpos = 0;

  if (oggstate->packets[0] && oggstate->packets[1])
  {
    // Check if we have the 2 packet, begin can be 0!
    packet.bytes = oggstate->packets[0]->length;
    packet.packet = oggstate->packets[0]->packet;
    packet.b_o_s = 0;
    packet.e_o_s = 0;
    packet.granulepos = FIRST_GRANPOS;
    packet.packetno = packetnum++;
    ogg_stream_packetin(stream,&packet);

    packet.bytes = oggstate->packets[1]->length;
    packet.packet = oggstate->packets[1]->packet;
    packet.b_o_s = 0;
    packet.e_o_s = 0;
    packet.granulepos = oggstate->initialgranpos;
    packet.packetno = packetnum++;
    ogg_stream_packetin(stream,&packet);

    if (ogg_stream_flush(stream, &page)!=0)
    {
      if (splt_io_fwrite(state, page.header,1,page.header_len,f) < page.header_len)
      {
        goto write_error;
      }
      if (splt_io_fwrite(state, page.body,1,page.body_len,f) < page.body_len)
      {
        goto write_error;
      }
    }

    while (ogg_stream_flush(stream, &page) != 0)
    {
      // Might this happen for _really_ high bitrate modes, if we're
      // spectacularly unlucky? Doubt it, but let's check for it just
      // in case.
      //
      //fprintf(stderr, 'Warning: First audio packet didn't fit into page. File may not decode correctly\n")'
      if (splt_io_fwrite(state, page.header,1,page.header_len,f) < page.header_len)
      {
        goto write_error;
      }
      if (splt_io_fwrite(state, page.body,1,page.body_len,f) < page.body_len)
      {
        goto write_error;
      }
    }
  }
  //added because packetno = 3 was never written
  else
  {
    if (oggstate->packets[1])
    {
      packet.bytes = oggstate->packets[1]->length;
      packet.packet = oggstate->packets[1]->packet;
      packet.b_o_s = 0;
      packet.e_o_s = 0;
      packet.granulepos = FIRST_GRANPOS;
      packet.packetno = packetnum++;
      ogg_stream_packetin(stream, &packet);

      if (ogg_stream_flush(stream, &page)!=0)
      {
        if (splt_io_fwrite(state, page.header,1,page.header_len,f) < page.header_len)
        {
          goto write_error;
        }
        if (splt_io_fwrite(state, page.body,1,page.body_len,f) < page.body_len)
        {
          goto write_error;
        }
      }
    }

    oggstate->initialgranpos = FIRST_GRANPOS;
  }

  current_granpos = oggstate->initialgranpos;

  int packet_err = SPLT_OK;

  while (!eos)
  {
    while (!eos)
    {
      result = ogg_sync_pageout(oggstate->sync_in, &page);

      //result == -1 is NOT a fatal error
      if (result==0)
      {
        break;
      }
      else
      {
        if (result != -1)
        {
          if (ogg_page_bos(&page) &&
              (oggstate->saved_serial != ogg_page_serialno(&page)))
          {
            splt_ogg_initialise_for_new_stream(ogg_new_stream_handler, &page, &cutpoint, prev_granpos);
            oggstate->cutpoint_begin = 0;
          }

          page_granpos = ogg_page_granulepos(&page) - oggstate->cutpoint_begin;

          /*long page_number = ogg_page_pageno(&page);
          fprintf(stdout,"end point page number = %ld, granule position = %ld, cutpoint = %ld, cutpoint_begin = %ld\n",
              page_number, page_granpos, cutpoint, oggstate->cutpoint_begin);
          fflush(stdout);*/

          if (ogg_stream_pagein(oggstate->stream_in, &page) == -1)
          {
            splt_e_set_error_data(state, splt_t_get_filename_to_split(state));
            *error = SPLT_ERROR_INVALID;
            goto error;
          }

          if ((cutpoint == 0) || (page_granpos < cutpoint))
          {
            while(1)
            {
              result = ogg_stream_packetout(oggstate->stream_in, &packet);

              //result == -1 is not a fatal error
              if (result==0)
              {
                break;
              }
              else
              {
                if (result != -1)
                {
                  /*fprintf(stdout,"end point packet number = %ld packet granulepos = %ld, b_o_s = %ld, e_o_s = %ld\n",
                      packet.packetno, packet.granulepos, packet.b_o_s, packet.e_o_s);
                  fflush(stdout);*/

                  if (splt_ogg_new_stream_needs_header_packet(ogg_new_stream_handler))
                  {
                    splt_ogg_new_stream_handle_header_packet(ogg_new_stream_handler, &packet, error);
                    if (*error < 0) { goto error; }
                    continue;
                  }
 
                  int bs = splt_ogg_get_blocksize(oggstate, oggstate->vi, &packet);
                  ogg_int64_t first_granpos = splt_ogg_compute_first_granulepos(state, oggstate, &packet, bs);
                  if (first_cut_granpos == 0)
                  {
                    first_cut_granpos = first_granpos;
                    if (cutpoint != 0)
                    {
                      cutpoint += first_granpos;
                    }
                  }
                  current_granpos += bs;

                  //we need to save the last packet, so save the current packet each time
                  splt_ogg_free_packet(&oggstate->packets[0]);
                  oggstate->packets[0] = splt_ogg_clone_packet(&packet, &packet_err);

                  if (packet_err < 0) { goto error; }
                  if (current_granpos > page_granpos)
                  {
                    current_granpos = page_granpos;
                  }
                  packet.granulepos = current_granpos;
                  packet.packetno = packetnum++;

                  //progress
                  if ((splt_o_get_int_option(state,SPLT_OPT_SPLIT_MODE) == SPLT_OPTION_SILENCE_MODE) ||
                      (splt_o_get_int_option(state,SPLT_OPT_SPLIT_MODE) == SPLT_OPTION_TRIM_SILENCE_MODE) ||
                      (!splt_o_get_int_option(state,SPLT_OPT_AUTO_ADJUST)))
                  {
                    /*fprintf(stdout, "%lf\t%lf\tx\n", (double)page_granpos, (double)cutpoint);
                    fflush(stdout);*/
                    splt_c_update_progress(state, (double)page_granpos,
                        (double)cutpoint,
                        1,0,SPLT_DEFAULT_PROGRESS_RATE);
                  }
                  else
                  {
                    /*fprintf(stdout, "%lf\t%lf\to\n", (double)page_granpos, (double)cutpoint);
                    fflush(stdout);*/
                    int progress_stage = 2;
                    float progress_start = 0;
                    if (after_adjust)
                    {
                      progress_stage = 4;
                      progress_start = 0.75;
                    }
                    splt_c_update_progress(state, (double)(page_granpos - up_to_adjust_granpos),
                        (double)(cutpoint - up_to_adjust_granpos),
                        progress_stage, progress_start, SPLT_DEFAULT_PROGRESS_RATE);
                  }

                  ogg_stream_packetin(stream, &packet);

                  if (packet.packetno == 4 && packet.granulepos != -1)
                  {
                    if (splt_ogg_write_pages_to_file(state, stream,f, 1,
                          error, output_fname)) { goto error; }
                  }
                  else
                  {
                    if (splt_ogg_write_pages_to_file(state, stream,f, 0,
                          error, output_fname)) { goto error; }
                  }
                }
              }
            }

            prev_granpos = page_granpos;
          }
          else 
          {
            if (adjust)
            {
              if (splt_ogg_scan_silence(state,
                    (2 * adjust), threshold, min_length, shots, 0, &page, current_granpos, error, first_cut_granpos,
                    splt_scan_silence_processor) > 0)
              {
                cutpoint = (splt_siu_silence_position(state->silence_list, 
                      oggstate->off) * oggstate->vi->rate);
              }
              else
              {
                cutpoint = (cutpoint + (adjust * oggstate->vi->rate));

                *error = splt_u_process_no_auto_adjust_found(state, sec_end + adjust);
                if (*error < 0) { goto error; }
              }

              if (first_cut_granpos == 0 && oggstate->first_granpos != 0)
              {
                first_cut_granpos = oggstate->first_granpos;
                cutpoint += first_cut_granpos;
                prev_granpos += first_cut_granpos;
              }

              *sec_split_time_length = cutpoint / oggstate->vi->rate;

              splt_siu_ssplit_free(&state->silence_list);
              adjust = 0;
              after_adjust = 1;
              up_to_adjust_granpos = page_granpos;
              splt_c_put_progress_text(state, SPLT_PROGRESS_CREATE);

              if (*error < 0) { goto error; }
            }
            else 
            {
              eos = 1; /* We reached the second cutpoint */
            }

            while ((result = ogg_stream_packetout(oggstate->stream_in, &packet)) != 0)
            {
              //if == -1, we are out of sync; not a fatal error
              if (result != -1)
              {
                /*fprintf(stdout,"end point packet bis number = %ld packet granulepos = %ld\n",
                    packet.packetno, packet.granulepos);
                fflush(stdout);*/

                if (splt_ogg_new_stream_needs_header_packet(ogg_new_stream_handler))
                {
                  splt_ogg_new_stream_handle_header_packet(ogg_new_stream_handler, &packet, error);
                  if (*error < 0) { goto error; }
                  continue;
                }

                int bs;
                bs = splt_ogg_get_blocksize(oggstate, oggstate->vi, &packet);

                ogg_int64_t first_granpos = splt_ogg_compute_first_granulepos(state, oggstate, &packet, bs);
                if (first_cut_granpos == 0 && first_granpos != 0)
                {
                  first_cut_granpos = first_granpos;
                  cutpoint += first_granpos;
                  prev_granpos += first_granpos;
                  eos = 0;
                }

                if (prev_granpos == -1)
                {
                  prev_granpos = 0;
                }
                else if (prev_granpos == 0 && !packet.e_o_s)
                {
                  prev_granpos = bs + first_cut_granpos;
                  if (prev_granpos > current_granpos + first_cut_granpos)
                  {
                    prev_granpos = current_granpos + first_cut_granpos;
                  }
                }
                else
                {
                  prev_granpos += bs;
                }

                current_granpos += bs;

                if (prev_granpos >= cutpoint)
                {
                  splt_ogg_free_packet(&oggstate->packets[1]);
                  //don't save the last packet if exact split
                  if (prev_granpos != cutpoint)
                  {
                    oggstate->packets[1] = splt_ogg_clone_packet(&packet, &packet_err);
                  }
                  if (packet_err < 0) { goto error; }
                  if (first_cut_granpos != 0)
                  {
                    packet.granulepos = current_granpos;
                  }
                  else
                  {
                    packet.granulepos = cutpoint; /* Set it! This 'truncates' the final packet, as needed. */
                  }
                  //fprintf(stdout,"granpos 2 = %ld\n", packet.granulepos);
                  //fflush(stdout);
                  packet.e_o_s = 1;
                  ogg_stream_packetin(stream, &packet);
                  break;
                }
                else
                {
                  if (first_cut_granpos != 0)
                  {
                    packet.granulepos = current_granpos;
                  }
                  else
                  {
                    packet.granulepos = prev_granpos - first_cut_granpos;
                  }
                  //fprintf(stdout,"granpos 3 = %ld\n", packet.granulepos);
                  //fflush(stdout);
                  packet.packetno = packetnum++;
                }

                splt_ogg_free_packet(&oggstate->packets[0]);
                oggstate->packets[0] = splt_ogg_clone_packet(&packet, &packet_err);
                if (packet_err < 0) { goto error; }

                ogg_stream_packetin(stream, &packet);
                if (splt_ogg_write_pages_to_file(state, stream, f, 0, error, output_fname))
                {
                  goto error;
                }
              }
            }
          }

          if (ogg_page_eos(&page))
          {
            break;
          }
        }
      }
    }

    if (!eos)
    {
      int sync_bytes = splt_ogg_update_sync(state, oggstate->sync_in, in, error);
      if (sync_bytes == 0)
      {
        eos = 1;
      }
      else if (sync_bytes == -1)
      {
        goto error;
      }
    }
  }

  if ((cutpoint == 0) || (page_granpos < cutpoint)) // End of file. We stop here
  {
    if (splt_ogg_write_pages_to_file(state, stream, f, 1, error, output_fname))
    {
      goto error;
    }

    oggstate->end = -1; // No more data available. Next processes aborted

    splt_ogg_nsh_free(&ogg_new_stream_handler);

    return 0;
  }

  if (splt_ogg_write_pages_to_file(state, stream, f, 0, error, output_fname))
  {
    goto error;
  }

  oggstate->initialgranpos = prev_granpos - cutpoint;
  oggstate->cutpoint_begin += cutpoint;

//  fprintf(stdout,"prev_granpos = %ld\n",prev_granpos);
//  fprintf(stdout,"initial granpos = %ld\n",prev_granpos - cutpoint);
//  fprintf(stdout,"cutpoint begin = %ld\n", oggstate->cutpoint_begin);
//  fflush(stdout);

  if (save_end_point)
  {
    oggstate->end = 1;
  }
  else
  {
    oggstate->end = 0;
    //if we don't save the end point, go at the start of the file
    rewind(oggstate->in);
  }

  splt_ogg_nsh_free(&ogg_new_stream_handler);

  return 0;

write_error:
  splt_e_set_error_data(state, output_fname);
  *error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;

error:
  splt_ogg_nsh_free(&ogg_new_stream_handler);

  return -1;
}

double splt_ogg_split(const char *output_fname, splt_state *state,
    double sec_begin, double sec_end, short seekable, 
    int adjust, float threshold, float min_length, int shots, int *error, int save_end_point)
{
  splt_ogg_state *oggstate = state->codec;

  ogg_stream_state stream_out;
  ogg_int64_t begin, end = 0, cutpoint = 0;

  begin = (ogg_int64_t) (sec_begin * oggstate->vi->rate);

  double sec_end_time = sec_end;

  char *filename = splt_t_get_filename_to_split(state);

  short sec_end_is_not_eof =
    !splt_u_fend_sec_is_bigger_than_total_time(state, sec_end);

  if (sec_end_is_not_eof)
  {
    if (adjust)
    {
      if (sec_end_is_not_eof)
      {
        float gap = (float) adjust;
        if (sec_end > gap)
        {
          sec_end -= gap;
        }
        if (sec_end < sec_begin)
        {
          sec_end = sec_begin;
        }
      }
      else 
      {
        adjust = 0;
      }
    }
    end = (ogg_int64_t) (sec_end * oggstate->vi->rate);
    cutpoint = end - begin;
  }

  // First time we run this, no packets already saved.
  if (oggstate->end == 0)
  {
    // We must do this before. If an error occurs, we don't want to create empty files!
    //we find the begin cutpoint
    if (splt_ogg_find_begin_cutpoint(state,
          oggstate, oggstate->in, begin, error, filename, save_end_point) < 0)
    {
      return sec_end_time;
    }
  }

  if (! splt_o_get_int_option(state, SPLT_OPT_PRETEND_TO_SPLIT))
  {
    //- means stdout
    if (strcmp(output_fname, "-")==0)
    {
      oggstate->out = stdout;
#ifdef __WIN32__
      _setmode(fileno(oggstate->out), _O_BINARY);
#endif
    }
    else
    {
      if (!(oggstate->out = splt_io_fopen(output_fname, "wb")))
      {
        splt_e_set_strerror_msg_with_data(state, output_fname);
        *error = SPLT_ERROR_CANNOT_OPEN_DEST_FILE;
        return sec_end_time;
      }
    }
  }

  ogg_stream_init(&stream_out, rand());

  splt_ogg_set_tags_in_headers(oggstate, error);
  if (*error < 0)
  {
    ogg_stream_clear(&stream_out);
    return sec_end_time;
  }

  if (splt_ogg_write_header_packets(state, oggstate, &stream_out, output_fname, error) == -1)
  {
    goto end;
  }

  double sec_split_time_length = sec_end - sec_begin;
  splt_ogg_find_end_cutpoint(state, &stream_out, oggstate->in, 
      oggstate->out, cutpoint, adjust, threshold, min_length, shots, error, output_fname,
      save_end_point, &sec_split_time_length, sec_end);
  sec_end_time = sec_begin + sec_split_time_length;

end:
  ogg_stream_clear(&stream_out);
  if (oggstate->out)
  {
    if (oggstate->out != stdout)
    {
      if (fclose(oggstate->out) != 0)
      {
        splt_e_set_strerror_msg_with_data(state, output_fname);
        *error = SPLT_ERROR_CANNOT_CLOSE_FILE;
      }
    }
    oggstate->out = NULL;
  }

  if (*error >= 0)
  {
    if (oggstate->end == -1) 
    {
      *error = SPLT_OK_SPLIT_EOF;
      return sec_end_time;
    }

    *error = SPLT_OK_SPLIT;
  }

  return sec_end_time;
}

/*! 
\defgroup PluginAPI_OGG The OGG plugin's API

@{
*/

/*! Plugin API: returns the plugin infos (name, version, extension)

alloced data in splt_plugin_info will be freed by splt_t_state_free()
at the end of the program 
*/
void splt_pl_set_plugin_info(splt_plugin_info *info, int *error)
{
  float plugin_version = 1.0;

  //set plugin version
  info->version = plugin_version;

  //set plugin name
  info->name = malloc(sizeof(char) * 40);
  if (info->name != NULL)
  {
    snprintf(info->name, 39, "ogg vorbis (libvorbis)");
  }
  else
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return;
  }

  //set plugin extension
  info->extension = malloc(sizeof(char) * (strlen(SPLT_OGGEXT)+2));
  if (info->extension != NULL)
  {
    snprintf(info->extension, strlen(SPLT_OGGEXT)+1, SPLT_OGGEXT);
  }
  else
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return;
  }

  info->upper_extension = splt_su_convert(info->extension, SPLT_TO_UPPERCASE, error);
}

//! Plugin API: check if file can be handled by this plugin
int splt_pl_check_plugin_is_for_file(splt_state *state, int *error)
{
  char *filename = splt_t_get_filename_to_split(state);

  //o- means stdin ogg format
  if ((filename != NULL) && (strcmp(filename,"o-")) == 0)
  {
    return SPLT_TRUE;
  }

  int is_ogg = SPLT_FALSE;
  OggVorbis_File ogg_file;

  FILE *file_input = NULL;

  if ((file_input = splt_io_fopen(filename, "rb")) == NULL)
  {
    splt_e_set_strerror_msg_with_data(state, filename);
    *error = SPLT_ERROR_CANNOT_OPEN_FILE;
  }
  else
  {
    //check if the file is ogg vorbis
    if ((ov_test(file_input, &ogg_file, NULL, 0) + 1) == 1)
    {
      is_ogg = SPLT_TRUE;
      ov_clear(&ogg_file);
    }
    else
    {
      if (file_input != stdin)
      {
        if (fclose(file_input) != 0)
        {
          splt_e_set_strerror_msg_with_data(state, filename);
          *error = SPLT_ERROR_CANNOT_CLOSE_FILE;
        }
      }
      file_input = NULL;
    }
  }

  return is_ogg;
}

//! Plugin API: Initialize this plugin
void splt_pl_init(splt_state *state, int *error)
{
  FILE *file_input = NULL;
  char *filename = splt_t_get_filename_to_split(state);

  if (splt_io_input_is_stdin(state))
  {
    if (filename[1] == '\0')
    {
      splt_c_put_info_message_to_client(state, 
          _(" warning: stdin 'o-' is supposed to be ogg stream.\n"));
    }
  }

  //if we can open the file
  if ((file_input = splt_ogg_open_file_read(state, filename, error)) != NULL)
  {
    splt_ogg_get_info(state, file_input, error);
    if (*error >= 0)
    {
      splt_ogg_state *oggstate = state->codec;
      oggstate->off = splt_o_get_float_option(state,SPLT_OPT_PARAM_OFFSET);
    }
  }
}

//! Plugin API: Uninitialize this plugin
void splt_pl_end(splt_state *state, int *error)
{
  splt_ogg_state_free(state);
}

//! Plugin API: Output a portion of the file
double splt_pl_split(splt_state *state, const char *final_fname,
    double begin_point, double end_point, int *error, int save_end_point) 
{
  splt_ogg_put_tags(state, error);

  if (*error >= 0)
  {
    int gap = splt_o_get_int_option(state, SPLT_OPT_PARAM_GAP);
    float threshold = splt_o_get_int_option(state, SPLT_OPT_PARAM_THRESHOLD);
    int shots = splt_o_get_int_option(state, SPLT_OPT_PARAM_SHOTS);
    float min_length = splt_o_get_float_option(state, SPLT_OPT_PARAM_MIN_LENGTH);
 
    int input_not_seekable = splt_o_get_int_option(state, SPLT_OPT_INPUT_NOT_SEEKABLE);

    return splt_ogg_split(final_fname, state, begin_point, end_point, 
        input_not_seekable, gap, threshold, min_length, shots, error, save_end_point);
  }

  return end_point;
}

//! Plugin API: Scan for silence
int splt_pl_scan_silence(splt_state *state, int *error)
{
  float offset = splt_o_get_float_option(state,SPLT_OPT_PARAM_OFFSET);
  float threshold = splt_o_get_float_option(state, SPLT_OPT_PARAM_THRESHOLD);
  float min_length = splt_o_get_float_option(state, SPLT_OPT_PARAM_MIN_LENGTH);
  int shots = splt_o_get_int_option(state, SPLT_OPT_PARAM_SHOTS);

  splt_ogg_state *oggstate = state->codec;
  oggstate->off = offset;

  int found = splt_ogg_scan_silence(state, 0, threshold, min_length, shots, 1, NULL, 0, 
      error, 0, splt_scan_silence_processor);
  if (*error < 0) { return -1; }

  return found;
}

//! Plugin API: Scan trim using silence
int splt_pl_scan_trim_silence(splt_state *state, int *error)
{
  float threshold = splt_o_get_float_option(state, SPLT_OPT_PARAM_THRESHOLD);
  int shots = splt_o_get_int_option(state, SPLT_OPT_PARAM_SHOTS);

  int found = splt_ogg_scan_silence(state, 0, threshold, 0, shots, 1, NULL, 0, error, 0,
      splt_trim_silence_processor);
  if (*error < 0) { return -1; }

  return found;
}

//! Plugin API: Read the original Tags from the file
void splt_pl_set_original_tags(splt_state *state, int *error)
{
  splt_d_print_debug(state,"Taking ogg original tags... \n");
  char *filename = splt_t_get_filename_to_split(state);
  splt_ogg_get_original_tags(filename, state, error);
}

void splt_pl_clear_original_tags(splt_original_tags *original_tags)
{
  vorbis_comment *comment = (vorbis_comment *)original_tags->all_original_tags;
  if (!comment)
  {
    return;
  }

  splt_ogg_free_vorbis_comment(comment, SPLT_TRUE);

  free(original_tags->all_original_tags);
  original_tags->all_original_tags = NULL;
}

//@}
