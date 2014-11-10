/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2014 Alexandru Munteanu - m@ioalex.net
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

#ifndef MP3SPLT_FLAC_MD5_DECODER_H

#include <FLAC/all.h>

#include <string.h>

#include "splt.h"
#include "md5.h"

typedef struct {
  FLAC__StreamDecoder *decoder;
  unsigned char *frame;
  size_t frame_size;
  size_t remaining_size;
  splt_code error;
  splt_state *state;
  MD5_CTX md5_context;
} splt_flac_md5_decoder;

splt_flac_md5_decoder *splt_flac_md5_decoder_new_and_init(splt_state *state, splt_code *error);
void splt_flac_md5_decode_frame(unsigned char *frame,
    size_t frame_size, splt_flac_md5_decoder *flac_md5_d, splt_code *error, splt_state *state);
unsigned char *splt_flac_md5_decoder_free_and_get_md5sum(splt_flac_md5_decoder *flac_md5_d);

#define MP3SPLT_FLAC_MD5_DECODER_H

#endif

