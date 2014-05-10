/**********************************************************
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2014 Alexandru Munteanu - m@ioalex.net
 *
 *********************************************************/

/**********************************************************
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
 *********************************************************/

#include "flac_md5_decoder.h"

static FLAC__StreamDecoderReadStatus splt_flac_md5_decoder_read(
    const FLAC__StreamDecoder *decoder, FLAC__byte buffer[], size_t *bytes, void *client_data)
{
  splt_flac_md5_decoder *flac_md5_d = (splt_flac_md5_decoder *) client_data;

  if (*bytes <= 0) { return FLAC__STREAM_DECODER_READ_STATUS_ABORT; }

  unsigned char *start = 
    flac_md5_d->frame + (flac_md5_d->frame_size - flac_md5_d->remaining_size);

  size_t to_read = *bytes;
  if (to_read >= flac_md5_d->remaining_size)
  {
    to_read = flac_md5_d->remaining_size;
    memcpy(buffer, start, to_read);
    flac_md5_d->remaining_size = 0;
    *bytes = to_read;
    return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
  }

  to_read = *bytes;
  memcpy(buffer, start, to_read);
  flac_md5_d->remaining_size -= to_read;
  if (flac_md5_d->remaining_size < 0) { flac_md5_d->remaining_size = 0; }

  return FLAC__STREAM_DECODER_READ_STATUS_CONTINUE;
}

static FLAC__StreamDecoderWriteStatus splt_flac_md5_decoder_write(const FLAC__StreamDecoder *decoder, 
    const FLAC__Frame *frame, const FLAC__int32 * const buffer[], void *client_data)
{
  splt_flac_md5_decoder *flac_md5_d = (splt_flac_md5_decoder *) client_data;

  int bytes_per_sample = (int) (frame->header.bits_per_sample + 7) / 8;

  size_t channel, sample;
  for (sample = 0;sample < frame->header.blocksize; sample++)
  {
    for (channel = 0;channel < frame->header.channels; channel++)
    {
      FLAC__int32 num = buffer[channel][sample];
      MD5_Update(&flac_md5_d->md5_context, &num, bytes_per_sample);
    }
  }

  return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

static void splt_flac_md5_decoder_error(const FLAC__StreamDecoder *decoder,
    FLAC__StreamDecoderErrorStatus status, void *client_data)
{
  splt_flac_md5_decoder *flac_md5_d = (splt_flac_md5_decoder *) client_data;

  flac_md5_d->error = SPLT_ERROR_INVALID;
  splt_e_set_error_data(flac_md5_d->state, splt_t_get_filename_to_split(flac_md5_d->state));

  splt_d_print_debug(flac_md5_d->state, "Error while decoding flac file: %s\n", 
      FLAC__StreamDecoderErrorStatusString[status]);
}

static splt_flac_md5_decoder *splt_flac_md5_decoder_new(splt_code *error, splt_state *state)
{
  splt_flac_md5_decoder *flac_md5_d = malloc(sizeof(splt_flac_md5_decoder));
  if (flac_md5_d == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return NULL;
  }

  memset(flac_md5_d, 0, sizeof(splt_flac_md5_decoder));

  flac_md5_d->error = SPLT_OK;
  flac_md5_d->state = state;
  MD5_Init(&flac_md5_d->md5_context);

  return flac_md5_d;
}

splt_flac_md5_decoder *splt_flac_md5_decoder_new_and_init(splt_state *state, splt_code *error)
{
  if (!splt_o_get_int_option(state, SPLT_OPT_DECODE_AND_WRITE_FLAC_MD5SUM_FOR_CREATED_FILES))
  {
    return NULL;
  }

  splt_flac_md5_decoder *flac_md5_d = splt_flac_md5_decoder_new(error, state);
  if (*error < 0) { return NULL; }

  flac_md5_d->decoder = FLAC__stream_decoder_new();
  if (flac_md5_d->decoder == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return NULL;
  }

  FLAC__StreamDecoderInitStatus status = FLAC__stream_decoder_init_stream(flac_md5_d->decoder,
      splt_flac_md5_decoder_read, 
      NULL /* seek */, NULL /* tell */, NULL /* length */, NULL /* eof */,
      splt_flac_md5_decoder_write, NULL /* metadata */,
      splt_flac_md5_decoder_error, flac_md5_d);

  if (status != FLAC__STREAM_DECODER_INIT_STATUS_OK)
  {
    splt_d_print_debug(state, "Failed to initialize md5 flac decoder with error %d", status);
    splt_e_set_error_data(state, splt_t_get_filename_to_split(state));
    *error = SPLT_ERROR_INVALID;
    unsigned char *md5sum = splt_flac_md5_decoder_free_and_get_md5sum(flac_md5_d);
    if (md5sum) { free(md5sum); }
    return NULL;
  }

  return flac_md5_d;
}

void splt_flac_md5_decode_frame(unsigned char *frame,
    size_t frame_size, splt_flac_md5_decoder *flac_md5_d, splt_code *error, splt_state *state)
{
  if (!splt_o_get_int_option(state, SPLT_OPT_DECODE_AND_WRITE_FLAC_MD5SUM_FOR_CREATED_FILES))
  {
    return;
  }

  flac_md5_d->frame = frame;
  flac_md5_d->frame_size = frame_size;
  flac_md5_d->remaining_size = frame_size;

  if (!FLAC__stream_decoder_process_single(flac_md5_d->decoder))
  {
    splt_d_print_debug(flac_md5_d->state, "Failed to process single frame for md5 computation\n");
    return;
  }

  if (flac_md5_d->error < 0) { *error = flac_md5_d->error; }
}

unsigned char *splt_flac_md5_decoder_free_and_get_md5sum(splt_flac_md5_decoder *flac_md5_d)
{
  if (!flac_md5_d) { return NULL; }

  if (flac_md5_d->decoder)
  {
    FLAC__stream_decoder_delete(flac_md5_d->decoder);
  }

  unsigned char *md5sum = NULL;
  md5sum = malloc(sizeof(unsigned char) * 16);
  MD5_Final(md5sum, &flac_md5_d->md5_context);

  free(flac_md5_d);

  return md5sum;
}

