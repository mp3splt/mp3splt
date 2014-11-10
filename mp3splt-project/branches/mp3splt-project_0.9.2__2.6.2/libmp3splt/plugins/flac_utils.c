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

#include <string.h>
#include <stdlib.h>

#include "flac_utils.h"
#include "from_flac_library.h"

static const unsigned char splt_flac_u_bit_access_table[] = { 0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f };

static void splt_flac_u_read_byte_skip(splt_flac_frame_reader *fr, splt_code *error);

static void splt_flac_u_read_bits_skip(splt_flac_frame_reader *fr, unsigned char bits_number,
    splt_code *error)
{
  if (bits_number > fr->remaining_bits)
  {
    splt_flac_u_read_byte_skip(fr, error);
    fr->remaining_bits = (fr->remaining_bits + 8) - bits_number;
    return;
  }

  fr->remaining_bits -= bits_number;
}

unsigned char splt_flac_u_read_bits(splt_flac_frame_reader *fr, unsigned char bits_number,
    splt_code *error)
{
  if (bits_number > fr->remaining_bits)
  {
    unsigned char bits = splt_flac_u_read_next_byte(fr, error) >> (8 - bits_number);
    fr->remaining_bits = (fr->remaining_bits + 8) - bits_number;
    return bits;
  }

  unsigned char bits = fr->last_byte << (8 - fr->remaining_bits);
  fr->remaining_bits -= bits_number;
  return bits >> (8 - bits_number);
}

static void splt_flac_u_read_bit_skip(splt_flac_frame_reader *fr, splt_code *error)
{
  if (fr->remaining_bits == 0)
  {
    splt_flac_u_read_next_byte(fr, error);
    fr->remaining_bits = 7;
    return;
  }

  fr->remaining_bits--;
}

unsigned char splt_flac_u_read_bit(splt_flac_frame_reader *fr, splt_code *error)
{
  if (fr->remaining_bits == 0)
  {
    unsigned char byte = splt_flac_u_read_next_byte(fr, error) >> 7;
    fr->remaining_bits = 7;
    return byte;
  }

  fr->remaining_bits--;

  return (fr->last_byte & splt_flac_u_bit_access_table[fr->remaining_bits + 1]) >> fr->remaining_bits;
}

static void splt_flac_u_append_input_buffer_to_output_buffer(splt_flac_frame_reader *fr,
    splt_code *error)
{
  if (fr->buffer == NULL) { return; }

  size_t existing_size = SPLT_FLAC_FR_BUFFER_SIZE * fr->output_buffer_times;
  fr->output_buffer = realloc(fr->output_buffer, existing_size + SPLT_FLAC_FR_BUFFER_SIZE);
  if (fr->output_buffer == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return;
  }

  memcpy(fr->output_buffer + existing_size, fr->buffer, SPLT_FLAC_FR_BUFFER_SIZE);

  fr->output_buffer_times++;
}

void splt_flac_u_process_frame(splt_flac_frame_reader *fr, unsigned frame_byte_buffer_start,
    splt_state *state, splt_code *error,
    void (*frame_processor)(unsigned char *frame, size_t frame_length,
      splt_state *state, splt_code *error, void *user_data),
    void *user_data)
{
  splt_flac_u_append_input_buffer_to_output_buffer(fr, error);
  if (*error < 0) { return; }

  if (frame_processor != NULL)
  {
    unsigned frame_byte_cut_end = 0;
    if (fr->next_byte < SPLT_FLAC_FR_BUFFER_SIZE)
    {
      frame_byte_cut_end = SPLT_FLAC_FR_BUFFER_SIZE - fr->next_byte;
    }

    size_t total_length =
      (fr->output_buffer_times * SPLT_FLAC_FR_BUFFER_SIZE) - frame_byte_buffer_start - frame_byte_cut_end;

    frame_processor(fr->output_buffer + frame_byte_buffer_start, total_length, state, error, user_data);
  }

  free(fr->output_buffer);
  fr->output_buffer = NULL;
  fr->output_buffer_times = 0;
}

static void splt_flac_u_sync_buffer_to_next_byte(splt_flac_frame_reader *fr, splt_code *error)
{
  if (fr->next_byte >= SPLT_FLAC_FR_BUFFER_SIZE)
  {
    if (feof(fr->in))
    {
      *error = SPLT_OK_SPLIT_EOF;
      return;
    }

    splt_flac_u_append_input_buffer_to_output_buffer(fr, error);
    if (*error < 0) { return; }

    if (fr->buffer) { free(fr->buffer); }
    fr->buffer = calloc(SPLT_FLAC_FR_BUFFER_SIZE, 1);
    if (fr->buffer == NULL)
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      return;
    }

    fr->read_bytes = fread(fr->buffer, 1, SPLT_FLAC_FR_BUFFER_SIZE, fr->in);

    fr->last_byte = fr->buffer[0];
    fr->next_byte = 1;

    SPLT_FLAC_UPDATE_CRC16(fr->crc16, fr->last_byte);
    return;
  }

  fr->last_byte = fr->buffer[fr->next_byte];
  fr->next_byte++;

  SPLT_FLAC_UPDATE_CRC16(fr->crc16, fr->last_byte); 
}

void splt_flac_u_read_zeroes_and_the_next_one(splt_flac_frame_reader *fr, splt_code *error)
{
  if (fr->remaining_bits > 0)
  {
    unsigned char current = fr->last_byte << (8 - fr->remaining_bits);
    if (current > 0)
    {
      fr->remaining_bits -= (splt_flac_l_byte_to_unary_table[current] + 1);
      return;
    }
  }

  fr->remaining_bits = 0;

  while (1)
  {
    splt_flac_u_sync_buffer_to_next_byte(fr, error);
    if (*error < 0) { return; }

    if (fr->last_byte > 0)
    {
      fr->remaining_bits = 8 - (splt_flac_l_byte_to_unary_table[fr->last_byte] + 1);
      return;
    }
  }
}

static void splt_flac_u_read_byte_skip(splt_flac_frame_reader *fr, splt_code *error)
{
  splt_flac_u_sync_buffer_to_next_byte(fr, error);
  SPLT_FLAC_UPDATE_CRC8(fr->crc8, fr->last_byte);
}

static unsigned char splt_flac_u_read_byte(splt_flac_frame_reader *fr, splt_code *error)
{
  splt_flac_u_read_byte_skip(fr, error);
  return fr->last_byte;
}

unsigned char splt_flac_u_read_next_byte_(void *flac_frame_reader, splt_code *error)
{
  return splt_flac_u_read_next_byte((splt_flac_frame_reader *) flac_frame_reader, error);
}

unsigned char splt_flac_u_read_next_byte(splt_flac_frame_reader *fr, splt_code *error)
{
  if (fr->remaining_bits == 0) { return splt_flac_u_read_byte(fr, error); }
  return (fr->last_byte << (8 - fr->remaining_bits)) | (splt_flac_u_read_byte(fr, error) >> fr->remaining_bits);
}

void splt_flac_u_read_up_to_total_bits(splt_flac_frame_reader *fr, unsigned total_bits,
    splt_code *error)
{
  if (total_bits <= fr->remaining_bits)
  {
    fr->remaining_bits -= total_bits;
    return;
  }

  total_bits -= fr->remaining_bits;
  fr->remaining_bits = 0;

  while (total_bits >= 8)
  {
    splt_flac_u_sync_buffer_to_next_byte(fr, error);
    if (*error < 0) { return; }
    total_bits -= 8;
  }

  if (total_bits == 1)
  {
    splt_flac_u_read_bit_skip(fr, error);
    return;
  }

  if (total_bits > 0)
  {
    splt_flac_u_read_bits_skip(fr, total_bits, error);
  }
}

unsigned splt_flac_u_read_unsigned(splt_flac_frame_reader *fr, splt_code *error)
{
  unsigned word = 0;
  word |= ((unsigned) splt_flac_u_read_next_byte(fr, error)) << 8;
  if (*error < 0) { return word; }
  return word | ((unsigned) splt_flac_u_read_next_byte(fr, error));
}

