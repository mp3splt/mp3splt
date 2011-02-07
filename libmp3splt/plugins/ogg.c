/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2010 Alexandru Munteanu - <io_fx@yahoo.fr>
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307,
 * USA.
 *
 *********************************************************/

/*! \file

The Plug-in that handles ogg vorbis files
*/

#include <time.h>
#include <string.h>
#include <math.h>
#include <locale.h>

#ifdef __WIN32__
#include <io.h>
#include <fcntl.h>
#endif

#include "splt.h"
#include "ogg.h"

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
int splt_ogg_scan_silence(splt_state *state, short seconds, 
    float threshold, float min, short output, ogg_page *page,
    ogg_int64_t granpos, int *error, long first_cut_granpos);

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

static char *splt_ogg_trackstring(int number, int *error)
{
  char *track = NULL;

  if (number > 0)
  {
    int len = 0, i;
    len = ((int) (log10((double) (number)))) + 1;

    if ((track = malloc(len + 1))==NULL)
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      return NULL;
    }
    memset(track, 0, len + 1);
    for (i=len-1; i >= 0; i--)
    {
      track[i] = ((number%10) | 0x30);
      number /= 10;
    }
  }

  return track;
}

//saves a packet
static splt_v_packet *splt_ogg_save_packet(ogg_packet *packet, int *error)
{
  splt_v_packet *p = NULL;

  //if we have no header, we will have bytes < 0
  p = malloc(sizeof(splt_v_packet));
  if (!p)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return p;
  }

  p->length = packet->bytes;
  p->packet = malloc(p->length);
  if (! p->packet)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    free(p);
    p = NULL;
    return p;
  }
  memcpy(p->packet, packet->packet, p->length);

  return p;
}

//frees a packet
static void splt_ogg_free_packet(splt_v_packet **p)
{
  if (p)
  {
    if (*p)
    {
      if((*p)->packet)
      {
        free((*p)->packet);
        (*p)->packet = NULL;
      }
      free(*p);
      *p = NULL;
    }
  }
}

static long splt_ogg_get_blocksize(splt_ogg_state *oggstate, 
    vorbis_info *vi, ogg_packet *op)
{
  //if this < 0, there is a problem
  int this = vorbis_packet_blocksize(vi, op);
  int ret = (this + oggstate->prevW)/4;

  oggstate->prevW = this;

  return ret;
}

static int splt_ogg_update_sync(splt_state *state, ogg_sync_state *sync_in,
    FILE *f, int *error)
{
  char *buffer = ogg_sync_buffer(sync_in, SPLT_OGG_BUFSIZE);
  if (!buffer)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return -1;
  }
  int bytes = fread(buffer,1,SPLT_OGG_BUFSIZE,f);

  if (ogg_sync_wrote(sync_in, bytes) != 0)
  {
    splt_e_set_error_data(state, splt_t_get_filename_to_split(state));
    *error = SPLT_ERROR_INVALID;
    return -1;
  }

  return bytes;
}

/* Returns 0 for success, or -1 on failure. */
static int splt_ogg_write_pages_to_file(splt_state *state, 
    ogg_stream_state *stream, FILE *file, int flush, int *error,
    const char *output_fname)
{
  ogg_page page;

  if (flush)
  {
    while (ogg_stream_flush(stream, &page))
    {
      if (splt_io_fwrite(state, page.header, 1, page.header_len, file) < page.header_len)
      {
        goto write_error;
      }
      if (splt_io_fwrite(state, page.body, 1, page.body_len, file) < page.body_len)
      {
        goto write_error;
      }
    }
  }
  else
  {
    while (ogg_stream_pageout(stream, &page))
    {
      if (splt_io_fwrite(state, page.header,1,page.header_len, file) < page.header_len)
      {
        goto write_error;
      }
      if (splt_io_fwrite(state, page.body,1,page.body_len, file) < page.body_len)
      {
        goto write_error;
      }
    }
  }

  return 0;

write_error:
  splt_e_set_error_data(state, output_fname);
  *error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
  return -1;
}

static void splt_ogg_submit_headers_to_stream(ogg_stream_state *stream, 
    splt_ogg_state *oggstate)
{
  int i;
  for(i=0;i<3;i++)
  {
    ogg_packet p;
    p.bytes = oggstate->headers[i]->length;
    p.packet = oggstate->headers[i]->packet;
    p.b_o_s = ((i==0)?1:0);
    p.e_o_s = 0;
    p.granulepos=0;

    ogg_stream_packetin(stream, &p);
  }
}

/* Full cleanup of internal state and vorbis/ogg structures */
static void splt_ogg_v_free(splt_ogg_state *oggstate)
{
  if(oggstate)
  {
    if(oggstate->packets)
    {
      splt_ogg_free_packet(&oggstate->packets[0]);
      splt_ogg_free_packet(&oggstate->packets[1]);
      free(oggstate->packets);
      oggstate->packets = NULL;
    }
    if(oggstate->headers)
    {
      int i;
      for(i=0; i < 3; i++)
      {
        splt_ogg_free_packet(&oggstate->headers[i]);
      }
      free(oggstate->headers);
      oggstate->headers = NULL;
    }
    vorbis_comment_clear(&oggstate->vc);
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
    if(oggstate->sync_in)
    {
      ogg_sync_clear(oggstate->sync_in);
      free(oggstate->sync_in);
      oggstate->sync_in = NULL;
    }
    if (oggstate->vi)
    {
      vorbis_info_clear(oggstate->vi);
      free(oggstate->vi);
      oggstate->vi = NULL;
    }
    free(oggstate);
    oggstate = NULL;
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
  if ((oggstate->headers = malloc(sizeof(splt_v_packet)*3))==NULL)
  {
    goto error;
  }
  memset(oggstate->headers, 0, sizeof(splt_v_packet)*3);
  if ((oggstate->packets = malloc(sizeof(splt_v_packet)*2))==NULL)
  {
    goto error;
  }
  memset(oggstate->packets, 0, sizeof(splt_v_packet)*2);

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

//puts tags in vc
//what happens if 'vorbis_comment_add_tag(..)' fails ?
//- ask vorbis developers
static void splt_ogg_v_comment(splt_state *state, vorbis_comment *vc, char *artist,
    char *album, char *title, char *tracknum, char *date, char *genre, char *comment,
    int *error)
{
  if (splt_o_get_int_option(state, SPLT_OPT_TAGS) == SPLT_TAGS_ORIGINAL_FILE &&
      state->original_tags.tags_version == 0)
  {
    return;
  }

  if (title != NULL)
  {
    vorbis_comment_add_tag(vc, "title", title);
  }
  if (artist != NULL)
  {
    vorbis_comment_add_tag(vc, "artist", artist);
  }
  if (album != NULL)
  {
    vorbis_comment_add_tag(vc, "album", album);
  }
  if (date != NULL)
  {
    if (strlen(date) > 0)
    {
      vorbis_comment_add_tag(vc, "date", date);
    }
  }
  if (genre != NULL)
  {
    vorbis_comment_add_tag(vc, "genre", genre);
  }
  if (tracknum != NULL)
  {
    vorbis_comment_add_tag(vc, "tracknumber", tracknum);
  }
  if (comment != NULL)
  {
    vorbis_comment_add_tag(vc, "comment", comment);
  }
}

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

  a = vorbis_comment_query(vc_local, "artist",0);
  if (a != NULL)
  {
    err = splt_tu_set_original_tags_field(state, SPLT_TAGS_ARTIST, a);
    has_tags = SPLT_TRUE;
    OGG_VERIFY_ERROR();
  }

  t = vorbis_comment_query(vc_local, "title",0);
  if (t != NULL)
  {
    err = splt_tu_set_original_tags_field(state, SPLT_TAGS_TITLE, t);
    has_tags = SPLT_TRUE;
    OGG_VERIFY_ERROR();
  }

  al = vorbis_comment_query(vc_local, "album",0);
  if (al != NULL)
  {
    err = splt_tu_set_original_tags_field(state, SPLT_TAGS_ALBUM, al);
    has_tags = SPLT_TRUE;
    OGG_VERIFY_ERROR();
  }

  da = vorbis_comment_query(vc_local, "date",0);
  if (da != NULL)
  {
    err = splt_tu_set_original_tags_field(state, SPLT_TAGS_YEAR, da);
    has_tags = SPLT_TRUE;
    OGG_VERIFY_ERROR();
  }

  g = vorbis_comment_query(vc_local, "genre",0);
  if (g != NULL)
  {
    err = splt_tu_set_original_tags_field(state, SPLT_TAGS_GENRE, g);
    has_tags = SPLT_TRUE;
    OGG_VERIFY_ERROR();
  }

  tr = vorbis_comment_query(vc_local, "tracknumber",0);
  if (tr != NULL)
  {
    int track = atoi(tr);
    err = splt_tu_set_original_tags_field(state, SPLT_TAGS_TRACK, &track);
    has_tags = SPLT_TRUE;
    OGG_VERIFY_ERROR();
  }

  com = vorbis_comment_query(vc_local, "comment",0);
  if (com != NULL)
  {
    err = splt_tu_set_original_tags_field(state, SPLT_TAGS_COMMENT, com);
    has_tags = SPLT_TRUE;
    OGG_VERIFY_ERROR();
  }

  splt_tu_set_original_tags_field(state, SPLT_TAGS_VERSION, &has_tags);
}

//puts the ogg tags
void splt_ogg_put_tags(splt_state *state, int *error)
{
  splt_d_print_debug(state,"Setting ogg tags ...\n");

  splt_ogg_state *oggstate = state->codec;

  vorbis_comment_clear(&oggstate->vc);

  if (splt_o_get_int_option(state, SPLT_OPT_TAGS) == SPLT_NO_TAGS)
  {
    return;
  }

  splt_tags *tags = splt_tu_get_current_tags(state);
  if (!tags)
  {
    return;
  }

  char *track_string = splt_ogg_trackstring(tags->track, error);
  if (*error < 0) { return; }

  char *artist_or_performer = splt_tu_get_artist_or_performer_ptr(tags);

  vorbis_comment_init(&oggstate->vc);

  splt_ogg_v_comment(state, &oggstate->vc,
      artist_or_performer, tags->album, tags->title, track_string,
      tags->year, tags->genre, tags->comment, error);

  free(track_string);
  track_string = NULL;
}

/****************************/
/* ogg infos */

//Pull out and save the 3 header packets from the input file.
//-returns -1 if error and error is set in '*error'
static int splt_ogg_process_headers(splt_ogg_state *oggstate, int *error)
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
  int packet_err = SPLT_OK;
  oggstate->headers[0] = splt_ogg_save_packet(&packet, &packet_err);
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

          oggstate->headers[i+1] = splt_ogg_save_packet(&packet, &packet_err);
          if (packet_err < 0)
          {
            goto error;
          }
          //if bad header
          if (vorbis_synthesis_headerin(oggstate->vi,&oggstate->vc,&packet) < 0)
          {
            goto error_invalid_file;
          }
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
      *error = SPLT_ERROR_INVALID;
      goto error;
    }
  }

  return 0;

error_invalid_file:
  *error = SPLT_ERROR_INVALID;
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
      splt_e_set_error_data(state,filename);
      switch (ret)
      {
        case OV_EREAD:
          *error = SPLT_ERROR_WHILE_READING_FILE;
          break;
        default:
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
  if (splt_ogg_process_headers(oggstate, error) == -1)
  {
    if (*error == SPLT_ERROR_INVALID)
    {
      splt_e_set_error_data(state,filename);
    }
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
  vorbis_synthesis_init(oggstate->vd,oggstate->vi);
  vorbis_block_init(oggstate->vd,oggstate->vb);

  srand(time(NULL));

  return oggstate;
}

static long splt_ogg_compute_first_granulepos(splt_state *state, splt_ogg_state *oggstate, ogg_packet *packet, int bs)
{
  long first_granpos = 0;

  if (packet->granulepos >= 0)
  {
    /*fprintf(stdout,"oggstate->total_blocksize + bs = %ld\n",oggstate->total_blocksize + bs);
    fprintf(stdout,"packet granulepos = %ld\n",packet->granulepos);
    fflush(stdout);*/

    if ((packet->granulepos > oggstate->total_blocksize + bs) &&
        (oggstate->total_blocksize > 0) &&
        !packet->e_o_s &&
        (oggstate->first_granpos == 0))
    {
      first_granpos = packet->granulepos;
      oggstate->first_granpos = first_granpos;
      splt_c_put_info_message_to_client(state,
          _(" warning: unexpected position in ogg vorbis stream - split from 0.0 to EOF to fix.\n"));
    }

    oggstate->total_blocksize = packet->granulepos;
  }
  else if (oggstate->total_blocksize == -1)
  {
    oggstate->total_blocksize = 0;
  }
  else {
    oggstate->total_blocksize += bs;
    /*fprintf(stdout,"blocksize = %d, total = %ld\n", bs, oggstate->total_blocksize);
    fflush(stdout);*/
  }

  return first_granpos;
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
          granpos = ogg_page_granulepos(&page);

          /*long page_number = ogg_page_pageno(&page);
          fprintf(stdout,"page number = %ld, granule position = %ld, cutpoint = %ld\n",
              page_number, granpos, cutpoint);
          fflush(stdout);*/

          if (ogg_stream_pagein(oggstate->stream_in, &page) == -1)
          {
            *error = SPLT_ERROR_INVALID;
            return -1;
          }

          //for a broken ogg file with no
          //header, we have granpos > cutpoint the first time
          if (granpos < cutpoint)
          {
            while (1)
            {
              result = ogg_stream_packetout(oggstate->stream_in, &packet);

              /*fprintf(stdout,"packet number = %ld packet granulepos = %ld\n",
                  packet.packetno, packet.granulepos);
              fflush(stdout);*/

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
                int bs = splt_ogg_get_blocksize(oggstate, oggstate->vi, &packet);

                cutpoint += splt_ogg_compute_first_granulepos(state, oggstate, &packet, bs);

                /* We need to save the last packet in the first
                 * stream - but we don't know when we're going
                 * to get there. So we have to keep every packet
                 * just in case.
                 */
                splt_ogg_free_packet(&oggstate->packets[0]);
                oggstate->packets[0] = splt_ogg_save_packet(&packet, &packet_err);
                if (packet_err < 0) { return -1; }
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
                int bs = splt_ogg_get_blocksize(oggstate, oggstate->vi, &packet);
                prevgranpos += bs;

                long old_first_granpos = oggstate->first_granpos;
                long first_granpos = splt_ogg_compute_first_granulepos(state, oggstate, &packet, bs);
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
                  oggstate->packets[1] = splt_ogg_save_packet(&packet, &packet_err);
                  if (packet_err < 0) { return -1; }
                  eos = 1;
                  break;
                }

                splt_ogg_free_packet(&oggstate->packets[0]);
                oggstate->packets[0] = splt_ogg_save_packet(&packet, &packet_err);
                if (packet_err < 0) { return -1; }
              }
            }
          }

          if (ogg_page_eos(&page))
          {
            eos = 1;
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
        return -1;
      }
    }
  }

  if (granpos < cutpoint)
  {
    *error = SPLT_ERROR_BEGIN_OUT_OF_FILE;
    return -1;
  }

  /* Remaining samples in first packet */
  oggstate->initialgranpos = prevgranpos - cutpoint;
  oggstate->cutpoint_begin = cutpoint;

  return 0;
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
    FILE *in, FILE *f, ogg_int64_t cutpoint, int adjust, float threshold,
    int *error, const char *output_fname, int save_end_point,
    double *sec_split_time_length)
{
  splt_c_put_progress_text(state,SPLT_PROGRESS_CREATE);

  //TODO: eos not set on ripped streams on last split file

  //for the progress
  int progress_adjust = 1;

  splt_ogg_state *oggstate = state->codec;

  ogg_packet packet;
  ogg_page page;
  int eos=0;
  int result = 0;
  ogg_int64_t page_granpos = 0, current_granpos = 0, prev_granpos = 0;
  ogg_int64_t packetnum = 0; /* Should this start from 0 or 2 ? */

  long first_cut_granpos = 0;

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
          page_granpos = ogg_page_granulepos(&page) - oggstate->cutpoint_begin;

          if (ogg_page_eos(&page)) 
          {
            eos = 1;
          }

          if (ogg_stream_pagein(oggstate->stream_in, &page) == -1)
          {
            *error = SPLT_ERROR_INVALID;
            return -1;
          }

//          fprintf(stdout, "page_granpos = %ld\n", page_granpos - first_cut_granpos);
//          fprintf(stdout, "cutpoint = %ld\n", cutpoint - first_cut_granpos);
//          fflush(stdout);

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
                  int bs = splt_ogg_get_blocksize(oggstate, oggstate->vi, &packet);
                  long first_granpos = splt_ogg_compute_first_granulepos(state, oggstate, &packet, bs);
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
                  oggstate->packets[0] = splt_ogg_save_packet(&packet, &packet_err);

                  if (packet_err < 0) { return -1; }
                  if (current_granpos > page_granpos)
                  {
                    current_granpos = page_granpos;
                  }
                  packet.granulepos = current_granpos;
//                  fprintf(stdout,"granpos 1 = %ld\n", packet.granulepos);
//                  fflush(stdout);
                  packet.packetno = packetnum++;

                  //progress
                  if ((splt_o_get_int_option(state,SPLT_OPT_SPLIT_MODE)
                        == SPLT_OPTION_SILENCE_MODE) ||
                      (!splt_o_get_int_option(state,SPLT_OPT_AUTO_ADJUST)))
                  {
                    splt_c_update_progress(state, (double)page_granpos,
                        (double)cutpoint,
                        1,0,SPLT_DEFAULT_PROGRESS_RATE);
                  }
                  else
                  {
                    splt_c_update_progress(state, (double)page_granpos,
                        (double)cutpoint,
                        2,0,SPLT_DEFAULT_PROGRESS_RATE);
                  }

                  ogg_stream_packetin(stream, &packet);

                  if (packet.packetno == 4 && packet.granulepos != -1)
                  {
                    if (splt_ogg_write_pages_to_file(state, stream,f, 1,
                          error, output_fname)) { return -1; }
                  }
                  else
                  {
                    if (splt_ogg_write_pages_to_file(state, stream,f, 0,
                          error, output_fname)) { return -1; }
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
                    (2 * adjust), threshold, 0.f, 0, &page, current_granpos, error, first_cut_granpos) > 0)
              {
                cutpoint = (splt_siu_silence_position(state->silence_list, 
                      oggstate->off) * oggstate->vi->rate);
              }
              else
              {
                cutpoint = (cutpoint + (adjust * oggstate->vi->rate));
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
              progress_adjust = 0;
              splt_c_put_progress_text(state, SPLT_PROGRESS_CREATE);

              if (*error < 0) { return -1; }
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
                int bs;
                bs = splt_ogg_get_blocksize(oggstate, oggstate->vi, &packet);

                long first_granpos = splt_ogg_compute_first_granulepos(state, oggstate, &packet, bs);
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
                    oggstate->packets[1] = splt_ogg_save_packet(&packet, &packet_err);
                  }
                  if (packet_err < 0) { return -1; }
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
                oggstate->packets[0] = splt_ogg_save_packet(&packet, &packet_err);
                if (packet_err < 0) { return -1; }

                ogg_stream_packetin(stream, &packet);
                if (splt_ogg_write_pages_to_file(state, stream, f, 0, error, output_fname))
                {
                  return -1;
                }
              }
            }
          }

          if(ogg_page_eos(&page))
          {
            eos=1;
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
        return -1;
      }
    }
  }

  if ((cutpoint == 0) || (page_granpos < cutpoint)) // End of file. We stop here
  {
    if (splt_ogg_write_pages_to_file(state, stream, f, 1, error, output_fname))
    {
      return -1;
    }

    oggstate->end = -1; // No more data available. Next processes aborted

    return 0;
  }

  if (splt_ogg_write_pages_to_file(state, stream, f, 0, error, output_fname))
  {
    return -1;
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

  return 0;

write_error:
  splt_e_set_error_data(state, output_fname);
  *error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
  return -1;
}

//splits ogg
double splt_ogg_split(const char *output_fname, splt_state *state,
    double sec_begin, double sec_end, short seekable, 
    int adjust, float threshold, int *error, int save_end_point)
{
  splt_ogg_state *oggstate = state->codec;

  ogg_stream_state stream_out;
  ogg_packet header_comm;
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

  /* gets random serial number*/
  ogg_stream_init(&stream_out, rand());

  //vorbis memory leak ?
  vorbis_commentheader_out(&oggstate->vc, &header_comm);

  int packet_err = SPLT_OK;
  splt_ogg_free_packet(&oggstate->headers[1]);
  oggstate->headers[1] = splt_ogg_save_packet(&header_comm, &packet_err);
  ogg_packet_clear(&header_comm);
  vorbis_comment_clear(&oggstate->vc);
  if (packet_err < 0)
  {
    *error = packet_err;
    ogg_stream_clear(&stream_out);
    return sec_end_time;
  }

  splt_ogg_submit_headers_to_stream(&stream_out, oggstate);

  if (splt_ogg_write_pages_to_file(state, &stream_out, oggstate->out, 1,
        error, output_fname) == -1)
  {
    goto end;
  }

  //find end cutpoint and get error
  double sec_split_time_length = sec_end - sec_begin;
  splt_ogg_find_end_cutpoint(state, &stream_out, oggstate->in, 
      oggstate->out, cutpoint, adjust, threshold, error, output_fname,
      save_end_point, &sec_split_time_length);
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

/****************************/
/* ogg scan for silence */

//used by scan_silence
static int splt_ogg_silence(splt_ogg_state *oggstate, vorbis_dsp_state *vd, float threshold)
{
  float **pcm = NULL, sample;
  int samples, silence = 1;

  while((samples=vorbis_synthesis_pcmout(vd,&pcm))>0)
  {
    if (silence) 
    {
      int i, j;
      for (i=0; i < oggstate->vi->channels; i++)
      {
        float  *mono=pcm[i];
        if (!silence) 
        {
          break;
        }
        for(j=0; j<samples; j++)
        {
          sample = fabs(mono[j]);
          oggstate->temp_level = oggstate->temp_level *0.999 + sample*0.001;
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

int splt_ogg_scan_silence(splt_state *state, short seconds,
    float threshold, float min, short output, 
    ogg_page *page, ogg_int64_t granpos, int *error, long first_cut_granpos)
{
  splt_c_put_progress_text(state,SPLT_PROGRESS_SCAN_SILENCE);
  splt_ogg_state *oggstate = state->codec;

  ogg_page og;
  ogg_packet op;
  ogg_stream_state os;
  vorbis_dsp_state vd;
  vorbis_block vb;
  ogg_int64_t end_position, begin_position, pos, end, begin, page_granpos;
  int eos=0, found = 0, shot, result = 0, len = 0 ;
  short first, flush = 0;
  off_t position = ftello(oggstate->in); // Some backups
  int saveW = oggstate->prevW;
  float th = splt_co_convert_from_dB(threshold);

  ogg_stream_init(&os, oggstate->serial);

  char *filename = splt_t_get_filename_to_split(state);

  // We still have a page to process
  if (page)
  {
    memcpy(&og, page, sizeof(ogg_page));
    result = 1;
  }

  end_position = begin_position = pos = granpos;
  vorbis_synthesis_init(&vd, oggstate->vi);
  vorbis_block_init(&vd, &vb);

  ogg_sync_state oy;
  ogg_sync_init(&oy);

  int split_type = splt_o_get_int_option(state, SPLT_OPT_SPLIT_MODE);
  short option_silence_mode = (split_type == SPLT_OPTION_SILENCE_MODE);
  if (option_silence_mode)
  {
    memcpy(&oy, oggstate->sync_in, sizeof(*oggstate->sync_in));

    size_t storage_to_copy = oggstate->sync_in->storage * sizeof(unsigned char);

    oy.data = malloc(storage_to_copy);
    if (oy.data == NULL)
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      found = -1;
      goto function_end;
    }

    memcpy(oy.data, oggstate->sync_in->data, storage_to_copy);
  }

  if (seconds > 0)
  {
    end = (ogg_int64_t) (seconds * oggstate->vi->rate);
  }
  else 
  {
    end = 0;
  }

  begin = 0;
  first = output;
  shot = SPLT_DEFAULTSHOT;

  oggstate->temp_level = 0.0;

  short is_stream = SPLT_FALSE;
  long stream_time0 = 0;

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
        if (ogg_page_eos(&og)) 
        {
          eos = 1;
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

          /* need more data */
          if (result == 0) 
          {
            break;
          }

          if (result > 0)
          {
            int bs = splt_ogg_get_blocksize(oggstate, oggstate->vi, &op);
            long first_granpos = 
              splt_ogg_compute_first_granulepos(state, oggstate, &op, bs);
            if (first_cut_granpos == 0 && first_granpos != 0)
            {
              is_stream = SPLT_TRUE;
              first_cut_granpos = first_granpos;
              pos += first_cut_granpos;
            }

            pos += bs;                      
            if (pos > page_granpos)
            {
              pos = page_granpos;
            }
            begin += bs;

            if (vorbis_synthesis(&vb, &op) == 0)
            {
              vorbis_synthesis_blockin(&vd, &vb);
              if ((!flush) && (splt_ogg_silence(oggstate, &vd, th))) 
              {
                if (len == 0) 
                {
                  begin_position = pos;
                }
                if (first == 0) 
                {
                  len++;
                }
                if (shot < SPLT_DEFAULTSHOT)
                {
                  shot+=2;
                }
                end_position = pos;
              }
              else 
              {
                if (len > SPLT_DEFAULTSILLEN)
                {
                  if ((flush) || (shot <= 0))
                  {
                    float b_position, e_position;

                    double temp = (double) (begin_position - first_cut_granpos);
                    temp /= oggstate->vi->rate;
                    b_position = (float) temp;

                    temp = (double) (end_position - first_cut_granpos);
                    temp /= oggstate->vi->rate;
                    e_position = (float) temp;

                    if ((e_position - b_position - min) >= 0.f)
                    {
                      int err = SPLT_OK;
                      if (splt_siu_ssplit_new(&state->silence_list, b_position, e_position, len, &err) == -1)
                      {
                        found = -1;
                        goto function_end;
                      }
                      found++;
                    }
                    len = 0;
                    shot = SPLT_DEFAULTSHOT;
                  }
                } 
                else 
                {
                  len = 0;
                }
                if (flush)
                {
                  eos = 1;
                  break;
                }
                if ((first) && (shot <= 0))
                {
                  first = 0;
                }
                if (shot > 0) 
                {
                  shot--;
                }
              }
            }
            else
            {
              *error = SPLT_ERROR_INVALID;
              splt_e_set_error_data(state,filename);
              found = -1;
              goto function_end;
            }
          }
          if (end)
          {
            if (begin > end)
            {
              flush = 1;
            }
          }
          if (found >= SPLT_MAXSILENCE) 
          {
            eos = 1;
          }
        }
      }

      result = ogg_sync_pageout(&oy, &og);
      //result == -1 is NOT a fatal error

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
        found = -1;
        goto function_end;
      }

      result = ogg_sync_pageout(&oy, &og);
      //result == -1 is NOT a fatal error

//      if (result != -1)
//      {
//        long page_number = ogg_page_pageno(&og);
//        fprintf(stdout,"Y page number = %ld\n", page_number);
//        fflush(stdout);
//      }

      float level = splt_co_convert_to_dB(oggstate->temp_level);
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
            (double)(oggstate->len),
            1,0,SPLT_DEFAULT_PROGRESS_RATE2);
      }
      else
      {
        splt_c_update_progress(state,(double)begin,
            (double)end, 2,0.5,SPLT_DEFAULT_PROGRESS_RATE2);
      }
    }
  }

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
  if (fseeko(oggstate->in, position, SEEK_SET) == -1)
  {
    splt_e_set_strerror_msg_with_data(state, filename);
    *error = SPLT_ERROR_SEEKING_FILE;
    found = -1;
  }

  return found;
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
    return splt_ogg_split(final_fname, state,
        begin_point, end_point,
        !state->options.option_input_not_seekable,
        state->options.parameter_gap,
        state->options.parameter_threshold, error, save_end_point);
  }

  return end_point;
}

//! Plugin API: Scan for silence
int splt_pl_scan_silence(splt_state *state, int *error)
{
  float offset = splt_o_get_float_option(state,SPLT_OPT_PARAM_OFFSET);
  float threshold = splt_o_get_float_option(state, SPLT_OPT_PARAM_THRESHOLD);
  float min_length = splt_o_get_float_option(state, SPLT_OPT_PARAM_MIN_LENGTH);
  int found = 0;

  splt_ogg_state *oggstate = state->codec;
  oggstate->off = offset;

  found = splt_ogg_scan_silence(state, 0, threshold, min_length, 1, NULL, 0, error, 0);
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

//@}
