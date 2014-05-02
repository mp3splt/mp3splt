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

#include <string.h>

#include "flac_metadata.h"

splt_flac_metadatas *splt_flac_m_new()
{
  splt_flac_metadatas *metadatas = malloc(sizeof(splt_flac_metadatas));
  if (metadatas == NULL) { return NULL; }

  metadatas->number_of_datas = 0;
  metadatas->datas = NULL;

  return metadatas;
}

void splt_flac_m_free(splt_flac_metadatas *metadatas)
{
  if (!metadatas) { return; }

  int i = 0;
  for (;i < metadatas->number_of_datas;i++)
  {
    if (metadatas->datas[i].bytes)
    {
      free(metadatas->datas[i].bytes);
      metadatas->datas[i].bytes = NULL;
    }
  }

  free(metadatas->datas);
  metadatas->datas = NULL;

  free(metadatas);
}

void splt_flac_m_append_metadata(unsigned char block_type, FLAC__uint32 block_length, 
    unsigned char *bytes, splt_flac_metadatas *metadatas, splt_code *error)
{
  splt_flac_one_metadata *one_metadata;

  if (metadatas->datas == NULL)
  {
    metadatas->datas = malloc(sizeof(splt_flac_one_metadata));
    if (metadatas->datas == NULL) { *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY; return; }

    one_metadata = &metadatas->datas[0];
  }
  else
  {
    metadatas->datas = realloc(metadatas->datas,
        (metadatas->number_of_datas + 1) * sizeof(splt_flac_one_metadata));
    if (metadatas->datas == NULL) { *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY; return; }

    one_metadata = &metadatas->datas[metadatas->number_of_datas];
  }

  metadatas->number_of_datas++;

  one_metadata->block_type = block_type;
  one_metadata->block_length = block_length;
  one_metadata->bytes = bytes;
}

