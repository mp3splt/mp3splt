/**********************************************************
 *
 * libmp3splt flac plugin 
 *
 * Copyright (c) 2013 Alexandru Munteanu - <m@ioalex.net>
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

#ifndef MP3SPLT_FLAC_TAGS_H

#include <FLAC/all.h>

#include "splt.h"
#include "from_flac_library.h"

typedef struct {
  char **tags;
  FLAC__uint32 number_of_tags;
  FLAC__uint32 total_bytes;
} splt_flac_vorbis_tags;

typedef struct {
  FLAC__uint32 vendor_length;
  char *vendor_string;
  splt_flac_vorbis_tags *other_tags;
  splt_tags *original_tags;
} splt_flac_tags;

splt_flac_tags *splt_flac_t_new(unsigned char *comments, FLAC__uint32 total_block_length,
    splt_code *error);
void splt_flac_t_free(splt_flac_tags **flac_tags);

splt_flac_vorbis_tags *splt_flac_vorbis_tags_new(splt_code *error);
void splt_flac_vorbis_tags_free(splt_flac_vorbis_tags **vorbis_tags);
void splt_flac_vorbis_tags_append_with_prefix(splt_flac_vorbis_tags *vorbis_tags,
    char *prefix, char *comment, splt_code *error);
void splt_flac_vorbis_tags_append(splt_flac_vorbis_tags *vorbis_tags,
    char *comment, splt_code *error);

#define MP3SPLT_FLAC_TAGS_H

#endif

