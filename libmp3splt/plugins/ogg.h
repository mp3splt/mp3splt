/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2006 Munteanu Alexandru - io_alex_2002@yahoo.fr
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

#ifndef MP3SPLT_OGG_H

#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include "splt.h"

/**********************************/
/* Ogg structures                 */

typedef struct {
  int length;
  unsigned char *packet;
} splt_v_packet;

typedef struct {
  ogg_sync_state *sync_in;
  ogg_stream_state *stream_in;
  vorbis_dsp_state *vd;
  vorbis_info *vi;
  vorbis_block *vb;
  int prevW;
  ogg_int64_t initialgranpos;
  ogg_int64_t len;
  ogg_int64_t cutpoint_begin;
  unsigned int serial;
  splt_v_packet **packets; /* 2 */
  splt_v_packet **headers; /* 3 */
  OggVorbis_File vf;
  vorbis_comment vc;
  FILE *in,*out;
  short end;
  float off;
  double avg_level;
  float temp_level;
  unsigned long n_stat;
  //we cound how many pages we have for the headers
  long header_page_number;
  //the granpos at the end of the first page of the stream
  ogg_int64_t stream_granpos;
} splt_ogg_state;

/****************************/
/* ogg utils */

void splt_ogg_state_free (splt_state *state);

/****************************/
/* ogg tags */

void splt_ogg_get_original_tags(char *filename,
    splt_state *state, int *tag_error);

void splt_ogg_put_tags(splt_state *state, int *error);

/****************************/
/* ogg infos */

splt_ogg_state *splt_ogg_info(FILE *in, splt_ogg_state *oggstate, 
    int *error);

/****************************/
/* ogg split */

void splt_ogg_split(char *filename, splt_state *state, double
    sec_begin, double sec_end, short seekable, 
    short adjust, float threshold, int *error);

/****************************/
/* ogg scan for silence */

int splt_ogg_scan_silence(splt_state *sp_state, short seconds, 
    float threshold, float min, 
    short output, ogg_page *page, ogg_int64_t granpos);

#define SPLT_OGG_BUFSIZE 4096

#define MP3SPLT_OGG_H

#endif

