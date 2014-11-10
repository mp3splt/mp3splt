/**********************************************************
 *
 * libmp3splt flac plugin 
 *
 * Copyright (c) 2014 Alexandru Munteanu - <m@ioalex.net>
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

#ifndef MP3SPLT_FLAC_METADATA_UTILS_H

#include "splt.h"

#include "flac.h"
#include "flac_tags.h"
#include "from_flac_library.h"
#include "flac_metadata.h"

void splt_flac_mu_read(splt_flac_state *flacstate, splt_state *state, FILE *in, splt_code *error);
unsigned char *splt_flac_mu_build_metadata_header(unsigned char type, unsigned char is_last,
    unsigned length);

#define SPLT_FLAC_METADATA_STREAMINFO 0
#define SPLT_FLAC_METADATA_PADDING 1
#define SPLT_FLAC_METADATA_APPLICATION 2
#define SPLT_FLAC_METADATA_SEEKTABLE 3
#define SPLT_FLAC_METADATA_VORBIS_COMMENT 4
#define SPLT_FLAC_METADATA_CUESHEET 5
#define SPLT_FLAC_METADATA_PICTURE 6

#define SPLT_FLAC_METADATA_HEADER_LENGTH 4

#define MP3SPLT_FLAC_METADATA_UTILS_H

#endif

