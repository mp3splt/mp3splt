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

#ifndef MP3SPLT_FLAC_METADATA_H

#include <FLAC/all.h>

#include "splt.h"

typedef struct {
  unsigned char block_type;
  FLAC__uint32 block_length;
  unsigned char *bytes;
} splt_flac_one_metadata;

typedef struct {
  splt_flac_one_metadata *datas; 
  int number_of_datas;
} splt_flac_metadatas;

splt_flac_metadatas *splt_flac_m_new();
void splt_flac_m_free(splt_flac_metadatas *metadatas);
void splt_flac_m_append_metadata(unsigned char block_type, FLAC__uint32 block_length, 
    unsigned char *bytes, splt_flac_metadatas *metadatas, splt_code *error);

#define MP3SPLT_FLAC_METADATA_H

#endif

