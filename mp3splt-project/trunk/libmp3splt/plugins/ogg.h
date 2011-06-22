/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2011 Alexandru Munteanu - io_fx@yahoo.fr
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

#define SPLT_OGGEXT ".ogg"

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
  short cloned_vorbis_comment;
  FILE *in,*out;
  short end;
  float off;
  float temp_level;
  //the granpos at the end of the first page of the stream
  ogg_int64_t stream_granpos;
  ogg_int64_t first_granpos;
  long total_blocksize;
} splt_ogg_state;

#define SPLT_OGG_BUFSIZE 4096

#define SPLT_OGG_ARTIST "ARTIST"
#define SPLT_OGG_TITLE "TITLE"
#define SPLT_OGG_ALBUM "ALBUM"
#define SPLT_OGG_DATE "DATE"
#define SPLT_OGG_GENRE "GENRE"
#define SPLT_OGG_TRACKNUMBER "TRACKNUMBER"
#define SPLT_OGG_COMMENT "COMMENT"

#define MP3SPLT_OGG_H

#endif

