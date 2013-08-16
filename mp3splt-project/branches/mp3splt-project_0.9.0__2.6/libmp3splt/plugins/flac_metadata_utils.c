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

#include "flac_metadata_utils.h"

static unsigned char *splt_flac_mu_read_metadata(FLAC__uint32 total_block_length, FILE *in, splt_code *error)
{
  unsigned char *bytes = malloc(total_block_length);
  if (bytes == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return NULL;
  }

  fread(bytes, 1, total_block_length, in);

  return bytes;
}

static void splt_flac_mu_skip_metadata(FLAC__uint32 total_block_length, FILE *in, splt_code *error)
{
  unsigned char *bytes = splt_flac_mu_read_metadata(total_block_length, in, error);
  if (*error < 0 || !bytes) { return; }

  free(bytes);
}

static void splt_flac_mu_save_metadata(splt_flac_state *flacstate, 
    unsigned char block_type, FLAC__uint32 total_block_length, FILE *in, splt_code *error)
{
  unsigned char *bytes = splt_flac_mu_read_metadata(total_block_length, in, error);
  if (*error < 0 || !bytes) { return; }

  splt_flac_m_append_metadata(block_type, total_block_length, bytes, flacstate->metadatas, error);
}

static void splt_flac_mu_read_streaminfo(splt_flac_state *flacstate,
    FLAC__uint32 total_block_length, FILE *in, splt_code *error)
{
  unsigned char *bytes = splt_flac_mu_read_metadata(total_block_length, in, error);
  if (*error < 0 || !bytes) { return; }

  splt_flac_l_convert_to_streaminfo(&flacstate->streaminfo, bytes);
  free(bytes);
}

static void splt_flac_mu_read_vorbis_comment(splt_flac_state *flacstate,
    FLAC__uint32 total_block_length, FILE *in, splt_code *error)
{
  unsigned char *comments = splt_flac_mu_read_metadata(total_block_length, in, error);
  if (*error < 0 || !comments) { return; }

  if (flacstate->flac_tags)
  {
    splt_flac_t_free(&flacstate->flac_tags);
  }

  flacstate->flac_tags = splt_flac_t_new(comments, total_block_length, error);

  free(comments);
}

static void splt_flac_mu_read_metadata_of_type(splt_flac_state *flacstate, splt_state *state,
    unsigned char block_type, FLAC__uint32 total_block_length, FILE *in, splt_code *error)
{
  switch (block_type)
  {
    case SPLT_FLAC_METADATA_STREAMINFO:
      splt_flac_mu_read_streaminfo(flacstate, total_block_length, in, error);
      return;
    case SPLT_FLAC_METADATA_PADDING:
      splt_flac_mu_save_metadata(flacstate, block_type, total_block_length, in, error);
      return;
    case SPLT_FLAC_METADATA_APPLICATION:
      splt_flac_mu_save_metadata(flacstate, block_type, total_block_length, in, error);
      return;
    case SPLT_FLAC_METADATA_SEEKTABLE:
      splt_flac_mu_skip_metadata(total_block_length, in, error);
      return;
    case SPLT_FLAC_METADATA_VORBIS_COMMENT:
      splt_flac_mu_read_vorbis_comment(flacstate, total_block_length, in, error);
      if (*error == SPLT_ERROR_INVALID)
      {
        splt_e_set_error_data(state, splt_t_get_filename_to_split(state));
      }
      return;
    case SPLT_FLAC_METADATA_CUESHEET:
      splt_flac_mu_skip_metadata(total_block_length, in, error);
      return;
    case SPLT_FLAC_METADATA_PICTURE:
      splt_flac_mu_save_metadata(flacstate, block_type, total_block_length, in, error);
      return;
    case 127:
      splt_e_set_error_data(state, splt_t_get_filename_to_split(state));
      *error = SPLT_ERROR_INVALID;
      return;
    default:
      splt_flac_mu_skip_metadata(total_block_length, in, error);
      return;
  }
}

void splt_flac_mu_read(splt_flac_state *flacstate, splt_state *state, FILE *in, splt_code *error)
{
  char flac_stream_marker[4] = { '\0' };
  fread(&flac_stream_marker, 1, 4, in);

  if (flac_stream_marker[0] != 'f' || flac_stream_marker[1] != 'L' ||
      flac_stream_marker[2] != 'a' || flac_stream_marker[3] != 'C')
  {
    splt_e_set_error_data(state, splt_t_get_filename_to_split(state));
    *error = SPLT_ERROR_INVALID;
    return;
  }

  unsigned char is_last_block = 0;
  while (!is_last_block)
  {
    unsigned char metadata_block_flag_and_block_type;
    fread(&metadata_block_flag_and_block_type, 1, 1, in);

    is_last_block = metadata_block_flag_and_block_type >> 7;
    unsigned char block_type = metadata_block_flag_and_block_type & 0x7f;

    unsigned char block_length[3];
    fread(block_length, 1, 3, in);
    FLAC__uint32 total_block_length = splt_flac_l_unpack_uint32(block_length, 3);

    splt_flac_mu_read_metadata_of_type(flacstate, state, block_type, total_block_length, in, error);
    if (*error < 0) { return; }
  }
}

unsigned char *splt_flac_mu_build_metadata_header(unsigned char type, unsigned char is_last,
    unsigned length)
{
  unsigned char *metadata_header = malloc(4);
  if (metadata_header == NULL) { return NULL; }

  metadata_header[0] = type | (is_last << 7);
  splt_flac_l_pack_uint32(length, metadata_header + 1, 3); 

  return metadata_header;
}

