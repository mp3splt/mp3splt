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

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include <math.h>

#include "flac_frame_reader.h"
#include "flac_utils.h"
#include "from_flac_library.h"
#include "flac_metadata_utils.h"
#include "silence_processors.h"
#include "flac_silence.h"

static void splt_flac_fr_read_header(splt_flac_frame_reader *fr,
    unsigned min_blocksize, unsigned max_blocksize, unsigned metadata_bits_per_sample, 
    splt_state *state, splt_code *error)
{
  fr->crc8 = 0;
  fr->bytes_between_frame_number_and_crc8 = 0;

  //sync code start
  splt_flac_u_read_next_byte(fr, error);
  if (*error < 0) { return; }
  unsigned char sync_code_end = splt_flac_u_read_next_byte(fr, error);
  if (*error < 0) { return; }
  unsigned char blocksize_and_sample_rate = splt_flac_u_read_next_byte(fr, error);
  if (*error < 0) { return; }

  unsigned previous_blocksize = fr->blocksize;

  unsigned char blocksize_bits = blocksize_and_sample_rate >> 4;
  if (blocksize_bits == 1)
    fr->blocksize = 192;
  else if (blocksize_bits > 1 && blocksize_bits <= 5)
    fr->blocksize = 576 << (blocksize_bits - 2);
  else if (blocksize_bits >= 8 && blocksize_bits <= 15)
    fr->blocksize = 256 << (blocksize_bits - 8);

  unsigned char channel_assignment_and_sample_size = splt_flac_u_read_next_byte(fr, error);
  if (*error < 0) { return; }
  unsigned char blocking_strategy = sync_code_end & 0x01;
  fr->blocking_strategy = blocking_strategy;

  unsigned char sample_size_in_bits = (channel_assignment_and_sample_size & 0x0e) >> 1;
  switch (sample_size_in_bits)
  {
    case 0:
      fr->bits_per_sample = metadata_bits_per_sample;
      break;
    case 1:
      fr->bits_per_sample = 8;
      break;
    case 2:
      fr->bits_per_sample = 12;
      break;
    default:
      fr->bits_per_sample = sample_size_in_bits * 4;
      break;
  }

  unsigned char channel_assignment = (channel_assignment_and_sample_size & 0xf0) >> 4;
  if (channel_assignment & 8)
  {
    fr->channels = 2;
    switch (channel_assignment & 7)
    {
      case 0:
        fr->channel_assignment = SPLT_FLAC_LEFT_SIDE;
        break;
      case 1:
        fr->channel_assignment = SPLT_FLAC_RIGHT_SIDE;
        break;
      case 2:
        fr->channel_assignment = SPLT_FLAC_MID_SIDE;
        break;
    }
  }
  else {
    fr->channels = (unsigned) channel_assignment + 1;
    fr->channel_assignment = SPLT_FLAC_INDEPENDENT;
  }

  fr->blocking_strategy = blocking_strategy || min_blocksize != max_blocksize;
  if (fr->blocking_strategy)
  {
    fr->current_sample_number = splt_flac_l_read_utf8_uint64(fr, error, &fr->sample_number_bytes);
    if (*error < 0) { return; }
  }
  else
  {
    uint32_t frame_number = splt_flac_l_read_utf8_uint32(fr, error, &fr->frame_number_bytes);
    if (*error < 0) { return; }
    fr->current_sample_number = frame_number * previous_blocksize;
  }

  if (blocksize_bits == 6)
  {
    unsigned char byte = splt_flac_u_read_next_byte(fr, error);
    if (*error < 0) { return; }

    fr->blocksize = byte + 1;
    fr->bytes_between_frame_number_and_crc8 = 1;
  }
  else if (blocksize_bits == 7)
  {
    unsigned char byte1 = splt_flac_u_read_next_byte(fr, error);
    if (*error < 0) { return; }
    unsigned char byte2 = splt_flac_u_read_next_byte(fr, error);
    if (*error < 0) { return; }

    unsigned size = (byte1 << 8) | byte2;
    fr->blocksize = size + 1;
    fr->bytes_between_frame_number_and_crc8 = 2;
  }

  if (fr->blocksize < fr->out_streaminfo.min_blocksize || fr->out_streaminfo.min_blocksize == 0)
  {
    fr->out_streaminfo.min_blocksize = fr->blocksize;
  }

  if (fr->blocksize > fr->out_streaminfo.max_blocksize || fr->out_streaminfo.max_blocksize == 0)
  {
    fr->out_streaminfo.max_blocksize = fr->blocksize;
  }

  unsigned char sample_bits = blocksize_and_sample_rate & 0x0f;
  if (sample_bits == 12)
  {
    splt_flac_u_read_next_byte(fr, error);
    if (*error < 0) { return; }
    fr->bytes_between_frame_number_and_crc8 += 1;
  }
  else if (sample_bits == 13 || sample_bits == 14)
  {
    splt_flac_u_read_next_byte(fr, error);
    if (*error < 0) { return; }
    splt_flac_u_read_next_byte(fr, error);
    if (*error < 0) { return; }
    fr->bytes_between_frame_number_and_crc8 += 2;
  }

  unsigned char computed_crc8 = fr->crc8;
  unsigned char crc8 = splt_flac_u_read_next_byte(fr, error);
  if (*error < 0) { return; }

  if (crc8 != computed_crc8)
  {
    splt_e_set_error_data(state, splt_t_get_filename_to_split(state));
    *error = SPLT_ERROR_INVALID;
  }
}

static void splt_flac_fr_read_constant_subframe(splt_flac_frame_reader *fr, unsigned bits_per_sample,
    splt_code *error)
{
  splt_flac_u_read_up_to_total_bits(fr, bits_per_sample, error);
}

static void splt_flac_fr_read_verbatim_subframe(splt_flac_frame_reader *fr, unsigned bits_per_sample,
    splt_code *error)
{
  splt_flac_u_read_up_to_total_bits(fr, bits_per_sample * fr->blocksize, error);
}

static void splt_flac_fr_read_rice_residual(splt_flac_frame_reader *fr, unsigned order,
    splt_code *error)
{
  unsigned char rice_method = 4;

  unsigned char first_bit = splt_flac_u_read_bit(fr, error);
  if (*error < 0) { return; }
  unsigned char second_bit = splt_flac_u_read_bit(fr, error);
  if (*error < 0) { return; }
  if (first_bit == 0 && second_bit == 1)
  {
    rice_method = 5;
  }

  unsigned char rice_partition_order = splt_flac_u_read_bits(fr, 4, error);
  if (*error < 0) { return; }

  unsigned order_partition_number = pow(2, rice_partition_order);

  unsigned partition_number;
  for (partition_number = 1;partition_number <= order_partition_number; partition_number++)
  {
    unsigned char rice_parameter_or_escape_code = splt_flac_u_read_bits(fr, rice_method, error);
    if (*error < 0) { return; }

    register unsigned char rice_parameter = rice_parameter_or_escape_code;
    if ((rice_method == 4 && ((rice_parameter_or_escape_code & 0x0f) == 0x0f)) ||
        (rice_method == 5 && ((rice_parameter_or_escape_code & 0x1f) == 0x1f)))
    {
      rice_parameter = splt_flac_u_read_bits(fr, 5, error);
      if (*error < 0) { return; }
    }

    unsigned number_of_samples = 0;
    if (rice_partition_order == 0)
    {
      number_of_samples = fr->blocksize - order;
    }
    else if (partition_number > 1)
    {
      number_of_samples = fr->blocksize / order_partition_number;
    }
    else
    {
      number_of_samples = (fr->blocksize / order_partition_number) - order;
    }

    unsigned i;
    for (i = 0;i < number_of_samples;i++)
    {
      splt_flac_u_read_zeroes_and_the_next_one(fr, error);
      if (*error < 0) { return; }

      if (rice_parameter)
      {
        splt_flac_u_read_up_to_total_bits(fr, rice_parameter, error);
        if (*error < 0) { return; }
      }
    }
  }
}

static void splt_flac_fr_read_fixed_subframe(splt_flac_frame_reader *fr, unsigned order, 
    unsigned bits_per_sample, splt_code *error)
{
  splt_flac_u_read_up_to_total_bits(fr, bits_per_sample * order, error);
  if (*error < 0) { return; }

  splt_flac_fr_read_rice_residual(fr, order, error);
}

static void splt_flac_fr_read_lpc_subframe(splt_flac_frame_reader *fr, unsigned order, 
    unsigned bits_per_sample, splt_state *state, splt_code *error)
{
  splt_flac_u_read_up_to_total_bits(fr, bits_per_sample * order, error);
  if (*error < 0) { return; }

  unsigned char quantized_linear_predictor = splt_flac_u_read_bits(fr, 4, error);
  if (*error < 0) { return; }

  if ((quantized_linear_predictor & 0x0f) == 0x0f)
  {
    splt_e_set_error_data(state, splt_t_get_filename_to_split(state));
    *error = SPLT_ERROR_INVALID;
  }

  //qlp_coeff_shift
  splt_flac_u_read_bits(fr, 5, error);
  if (*error < 0) { return; }

  char qlp_coeff_precision = ((char) quantized_linear_predictor) + 1;
  splt_flac_u_read_up_to_total_bits(fr, qlp_coeff_precision * order, error);
  if (*error < 0) { return; }

  splt_flac_fr_read_rice_residual(fr, order, error);
}

static void splt_flac_fr_read_subframe_content(splt_flac_frame_reader *fr, int sf_type, unsigned order,
    unsigned bits_per_sample, splt_state *state, splt_code *error)
{
  switch (sf_type)
  {
    case SPLT_FLAC_SUBFRAME_CONSTANT:
      splt_flac_fr_read_constant_subframe(fr, bits_per_sample, error);
      return;
    case SPLT_FLAC_SUBFRAME_VERBATIM:
      splt_flac_fr_read_verbatim_subframe(fr, bits_per_sample, error);
      return;
    case SPLT_FLAC_SUBFRAME_FIXED:
      splt_flac_fr_read_fixed_subframe(fr, order, bits_per_sample, error);
      return;
    case SPLT_FLAC_SUBFRAME_LPC:
      splt_flac_fr_read_lpc_subframe(fr, order, bits_per_sample, state, error);
      return;
  }

  splt_e_set_error_data(state, splt_t_get_filename_to_split(state));
  *error = SPLT_ERROR_INVALID;
}

static void splt_flac_fr_read_subframe(splt_flac_frame_reader *fr, unsigned bits_per_sample,
    splt_state *state, splt_code *error)
{
  unsigned char bit_padding_subframe_type_and_wasted_bit = splt_flac_u_read_next_byte(fr, error);
  if (*error < 0) { return; }

  unsigned char zero_bit = bit_padding_subframe_type_and_wasted_bit >> 7;
  if (zero_bit != 0)
  {
    splt_e_set_error_data(state, splt_t_get_filename_to_split(state));
    *error = SPLT_ERROR_INVALID;
    return;
  }

  unsigned wasted_bits_per_sample = 0;
  if (bit_padding_subframe_type_and_wasted_bit & 1)
  {
    unsigned k = 0;
    unsigned char bit;
    do {
      bit = splt_flac_u_read_bit(fr, error);
      if (*error < 0) { return; }
      k++;
    } while (bit != 1);

    wasted_bits_per_sample = k;
  }

  bit_padding_subframe_type_and_wasted_bit &= 0xfe;
  unsigned char subframe_type = bit_padding_subframe_type_and_wasted_bit;

  int sf_type = 0;
  unsigned order = 0;

  if (subframe_type & 0x80)
  {
    splt_e_set_error_data(state, splt_t_get_filename_to_split(state));
    *error = SPLT_ERROR_INVALID;
    return;
  }

  if (subframe_type == 0)
  {
    sf_type = SPLT_FLAC_SUBFRAME_CONSTANT;
  }
  else if (subframe_type == 2)
  {
    sf_type = SPLT_FLAC_SUBFRAME_VERBATIM;
  }
  else if (subframe_type < 16)
  {
    splt_e_set_error_data(state, splt_t_get_filename_to_split(state));
    *error = SPLT_ERROR_INVALID;
    return;
  }
  else if (subframe_type <= 24)
  {
    sf_type = SPLT_FLAC_SUBFRAME_FIXED;
    order = ((bit_padding_subframe_type_and_wasted_bit >> 1) & 7);
  }
  else if (subframe_type < 64)
  {
    splt_e_set_error_data(state, splt_t_get_filename_to_split(state));
    *error = SPLT_ERROR_INVALID;
    return;
  }
  else
  {
    sf_type = SPLT_FLAC_SUBFRAME_LPC;
    order = ((bit_padding_subframe_type_and_wasted_bit >> 1) & 31) + 1;
  }

  splt_flac_fr_read_subframe_content(fr, sf_type, order, bits_per_sample - wasted_bits_per_sample,
      state, error);
}

static void splt_flac_fr_read_frame(splt_flac_frame_reader *fr,
    unsigned min_blocksize, unsigned max_blocksize, unsigned metadata_bits_per_sample,
    splt_state *state, splt_code *error)
{
  fr->crc16 = 0;

  splt_flac_fr_read_header(fr, min_blocksize, max_blocksize, metadata_bits_per_sample, state, error);
  if (*error < 0) { return; }

  unsigned channel = 0;
  for (channel = 0; channel < fr->channels; channel++)
  {
    unsigned bits_per_sample = fr->bits_per_sample;
    if ((fr->channel_assignment == SPLT_FLAC_LEFT_SIDE && channel == 1) ||
        (fr->channel_assignment == SPLT_FLAC_RIGHT_SIDE && channel == 0) ||
        (fr->channel_assignment == SPLT_FLAC_MID_SIDE && channel == 1))
      bits_per_sample++;

    splt_flac_fr_read_subframe(fr, bits_per_sample, state, error);
    if (*error < 0) { return; }
  }

  splt_flac_u_read_up_to_total_bits(fr, fr->remaining_bits, error);
  if (*error < 0) { return; }

  unsigned computed_crc16 = fr->crc16;
  unsigned crc16 = splt_flac_u_read_unsigned(fr, error);
  if (*error < 0) { return; }

  if (crc16 != computed_crc16)
  {
    splt_e_set_error_data(state, splt_t_get_filename_to_split(state));
    *error = SPLT_ERROR_INVALID;
  }
}

static void splt_flac_fr_set_next_frame_and_sample_numbers(splt_flac_frame_reader *fr, splt_code *error)
{
  fr->frame_number++;
  fr->sample_number = fr->sample_number + fr->blocksize;

  if (fr->frame_number_as_utf8) { free(fr->frame_number_as_utf8); }
  fr->frame_number_as_utf8 =
    splt_flac_l_convert_to_utf8((FLAC__uint64) fr->frame_number, &fr->frame_number_as_utf8_length);
  if (fr->frame_number_as_utf8 == NULL) { *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY; return; }

  if (fr->sample_number_as_utf8) { free(fr->sample_number_as_utf8); }
  fr->sample_number_as_utf8 =
    splt_flac_l_convert_to_utf8((FLAC__uint64) fr->sample_number, &fr->sample_number_as_utf8_length);
  if (fr->sample_number_as_utf8 == NULL) { *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY; return; }
}

static void splt_flac_fr_write_frame_processor(unsigned char *frame, size_t frame_length,
    splt_state *state, splt_code *error, void *user_data)
{
  splt_flac_frame_reader *fr = (splt_flac_frame_reader *) user_data;

  unsigned char *frame_or_sample_utf8_bytes;
  unsigned char frame_or_sample_utf8_length;

  unsigned char frame_or_sample_read_length;

  if (fr->blocking_strategy)
  {
    frame_or_sample_utf8_bytes = fr->sample_number_as_utf8;
    frame_or_sample_utf8_length = fr->sample_number_as_utf8_length;
    frame_or_sample_read_length = fr->sample_number_bytes;
  }
  else
  {
    frame_or_sample_utf8_bytes = fr->frame_number_as_utf8;
    frame_or_sample_utf8_length = fr->frame_number_as_utf8_length;
    frame_or_sample_read_length = fr->frame_number_bytes;
  }

  size_t modified_frame_length =
    frame_length - frame_or_sample_read_length + frame_or_sample_utf8_length;

  if (modified_frame_length < fr->out_streaminfo.min_framesize ||
      fr->out_streaminfo.min_framesize == 0)
  {
    fr->out_streaminfo.min_framesize = modified_frame_length;
  }

  if (modified_frame_length > fr->out_streaminfo.max_framesize ||
      fr->out_streaminfo.max_framesize == 0)
  {
    fr->out_streaminfo.max_framesize = modified_frame_length;
  }

  unsigned char *modified_frame = malloc(sizeof(unsigned char) * modified_frame_length);
  if (modified_frame == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return;
  }

  //sync code & reserved & blocking strategy
  modified_frame[0] = frame[0];
  modified_frame[1] = frame[1];
  //block size & sample rate
  modified_frame[2] = frame[2];
  //channel assignment & sample size & reserved
  modified_frame[3] = frame[3];

  unsigned char *ptr = modified_frame + 3;

  //frame or sample utf8 number
  int i = 0;
  for (;i < frame_or_sample_utf8_length;i++)
  {
    modified_frame[4 + i] = frame_or_sample_utf8_bytes[i];
    ptr = modified_frame + 4 + i;
  }

  //copy everything else
  size_t length_up_to_including_frame_number = 4 + frame_or_sample_read_length;

  memcpy(ptr + 1,
      frame + length_up_to_including_frame_number,
      frame_length - length_up_to_including_frame_number);

  //compute and set new crc8
  size_t j = 0;
  size_t before_crc8_length = 
    4 + frame_or_sample_utf8_length + fr->bytes_between_frame_number_and_crc8;
  unsigned char new_crc8 = 0;
  for (;j < before_crc8_length;j++)
  {
    SPLT_FLAC_UPDATE_CRC8(new_crc8, modified_frame[j]);
  }
  modified_frame[j] = new_crc8;

  //compute and set new crc16
  unsigned new_crc16 = 0;
  for (j = 0;j < modified_frame_length - 2;j++)
  {
    SPLT_FLAC_UPDATE_CRC16(new_crc16, modified_frame[j]);
  }
  unsigned char first_byte_of_new_crc16 = (unsigned char) (new_crc16 >> 8);
  unsigned char last_byte_of_new_crc16 = (unsigned char) ((new_crc16 << 8) >> 8);
  modified_frame[j] = first_byte_of_new_crc16;
  modified_frame[j+1] = last_byte_of_new_crc16;

  splt_flac_md5_decode_frame(modified_frame, modified_frame_length, fr->flac_md5_d, error, state);
  if (*error < 0) { goto end; }

  if (splt_io_fwrite(state, modified_frame, modified_frame_length, 1, fr->out) != 1)
  {
    splt_e_set_error_data(state, fr->output_fname);
    *error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
  }

end:
  free(modified_frame);
}

static void splt_flac_fr_backup_frame_processor(unsigned char *frame, size_t frame_length,
    splt_state *state, splt_code *error, void *user_data)
{
  splt_flac_frame_reader *fr = (splt_flac_frame_reader *) user_data;

  if (fr->previous_frame) { free(fr->previous_frame); }
  fr->previous_frame = malloc(sizeof(unsigned char) * frame_length);
  memcpy(fr->previous_frame, frame, frame_length);
  fr->previous_frame_length = frame_length;
}

static void splt_flac_fr_finish_and_write_streaminfo(splt_state *state,
    unsigned min_blocksize, unsigned max_blocksize,
    unsigned min_framesize, unsigned max_framesize,
    const splt_flac_metadatas *metadatas, splt_flac_frame_reader *fr, splt_code *error)
{
  if (min_framesize == 0) { fr->out_streaminfo.min_framesize = 0; }
  if (max_framesize == 0) { fr->out_streaminfo.max_framesize = 0; }

  if (min_blocksize == max_blocksize)
  {
    fr->out_streaminfo.min_blocksize = min_blocksize;
    fr->out_streaminfo.max_blocksize = max_blocksize;
  }

  unsigned char *md5sum = splt_flac_md5_decoder_free_and_get_md5sum(fr->flac_md5_d);
  fr->flac_md5_d = NULL;
  if (md5sum)
  {
    int i;
    for (i = 0; i < 16; i++)
    {
      fr->out_streaminfo.md5sum[i] = md5sum[i];
    }
    free(md5sum);
  }

  unsigned char *streaminfo_bytes = splt_flac_l_convert_from_streaminfo(&fr->out_streaminfo);
  if (streaminfo_bytes == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return;
  }

  int last = 1;
  if (metadatas->number_of_datas > 0) { last = 0; }

  unsigned char *metadata_header =
    splt_flac_mu_build_metadata_header(SPLT_FLAC_METADATA_STREAMINFO, last, SPLT_FLAC_STREAMINFO_LENGTH);
  if (metadata_header == NULL)
  {
    free(streaminfo_bytes);
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return;
  }

  //TODO: remove fseek for stdout; how to ?
  if (fr->out != NULL)
  {
    rewind(fr->out);
  }

  unsigned char flac_word[4] = { 0x66, 0x4C, 0x61, 0x43 };
  if (splt_io_fwrite(state, flac_word, 4, 1, fr->out) != 1)
  {
    splt_e_set_error_data(state, fr->output_fname);
    *error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
    goto end;
  }

  if (splt_io_fwrite(state, metadata_header, SPLT_FLAC_METADATA_HEADER_LENGTH, 1, fr->out) != 1)
  {
    splt_e_set_error_data(state, fr->output_fname);
    *error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
    goto end;
  }

  if (splt_io_fwrite(state, streaminfo_bytes, SPLT_FLAC_STREAMINFO_LENGTH, 1, fr->out) != 1)
  {
    splt_e_set_error_data(state, fr->output_fname);
    *error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
  }

end:
  free(metadata_header);
  free(streaminfo_bytes);
}

static splt_flac_frame_reader *splt_flac_fr_reset_for_new_file(splt_flac_frame_reader *fr)
{
  fr->out = NULL;

  fr->out_streaminfo.min_blocksize = 0;
  fr->out_streaminfo.max_blocksize = 0;
  fr->out_streaminfo.min_framesize = 0;
  fr->out_streaminfo.max_framesize = 0;
  fr->out_streaminfo.total_samples = 0;

  int i = 0;
  for (i = 0;i < 16; i++)
  {
    fr->out_streaminfo.md5sum[i] = 0;
  }

  fr->out_streaminfo.sample_rate = 0;
  fr->out_streaminfo.channels = 0;
  fr->out_streaminfo.bits_per_sample = 0;

  fr->frame_number = 0;
  if (fr->frame_number_as_utf8) { free(fr->frame_number_as_utf8); }
  fr->frame_number_as_utf8 =
    splt_flac_l_convert_to_utf8(fr->frame_number, &fr->frame_number_as_utf8_length);
  if (fr->frame_number_as_utf8 == NULL) { free(fr); return NULL; }

  fr->sample_number = 0;
  if (fr->sample_number_as_utf8) { free(fr->sample_number_as_utf8); }
  fr->sample_number_as_utf8 = 
    splt_flac_l_convert_to_utf8(fr->sample_number, &fr->sample_number_as_utf8_length);
  if (fr->sample_number_as_utf8 == NULL)
  {
    free(fr->frame_number_as_utf8);
    free(fr);
    return NULL;
  }

  if (fr->output_fname) { free(fr->output_fname); }
  fr->output_fname = NULL;

  return fr;
}

splt_flac_frame_reader *splt_flac_fr_new(FILE *in, const char *input_filename)
{
  splt_flac_frame_reader *fr = malloc(sizeof(splt_flac_frame_reader));
  if (fr == NULL) { return NULL; }
  memset(fr, 0x0, sizeof(splt_flac_frame_reader));

  fr->in = in;

  fr->crc8 = 0;
  fr->crc16 = 0;

  fr->bits_per_sample = 0;
  fr->blocksize = 0;
  fr->blocking_strategy = 0;

  fr->channels = 0;
  fr->channel_assignment = 0;

  fr->remaining_bits = 0;
  fr->last_byte = 0;
  fr->next_byte = SPLT_FLAC_FR_BUFFER_SIZE;
  fr->read_bytes = 0;

  fr->buffer = NULL;

  fr->output_buffer = NULL;
  fr->output_buffer_times = 0;

  if (splt_flac_fr_reset_for_new_file(fr) == NULL)
  {
    return NULL;
  }

  fr->previous_frame = NULL;
  fr->previous_frame_length = 0;

  fr->end_point = 0;

  return fr;
}

void splt_flac_fr_free(splt_flac_frame_reader *fr)
{
  if (fr == NULL) { return; }

  if (fr->frame_number_as_utf8) { free(fr->frame_number_as_utf8); }
  if (fr->sample_number_as_utf8) { free(fr->sample_number_as_utf8); }
  if (fr->buffer) { free(fr->buffer); }
  if (fr->output_buffer) { free(fr->output_buffer); }
  if (fr->previous_frame) { free(fr->previous_frame); }
  if (fr->output_fname) { free(fr->output_fname); }
  if (fr->flac_md5_d)
  {
    unsigned char *md5sum = splt_flac_md5_decoder_free_and_get_md5sum(fr->flac_md5_d);
    if (md5sum) { free(md5sum); }
  }

  free(fr);
}

static void splt_flac_fr_write_metadatas(splt_flac_frame_reader *fr,
    const splt_flac_metadatas *metadatas, const int set_last_as_last,
    const char *output_fname, splt_state *state, splt_code *error)
{
  int i = 0;
  for (;i < metadatas->number_of_datas; i++)
  {
    splt_flac_one_metadata *one_metadata = &metadatas->datas[i];

    unsigned char block_flag_and_block_type = one_metadata->block_type;
    if (set_last_as_last && (i == metadatas->number_of_datas - 1))
    {
      block_flag_and_block_type |= 0x80;
    }

    FLAC__uint32 block_length = one_metadata->block_length;
    FLAC__byte block_lengths[3];
    splt_flac_l_pack_uint32(block_length, block_lengths, 3);

    if (splt_io_fwrite(state, &block_flag_and_block_type, 1, 1, fr->out) != 1) { goto error; }
    if (splt_io_fwrite(state, block_lengths, 3, 1, fr->out) != 1) { goto error; }
    if (splt_io_fwrite(state, one_metadata->bytes, block_length, 1, fr->out) != 1) { goto error; }
  }

  return;

error:
  splt_e_set_error_data(state, fr->output_fname);
  *error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
}

static void splt_flac_fr_write_tags(splt_flac_frame_reader *fr, const splt_flac_tags *flac_tags,
    const splt_tags *tags, const char *output_fname, splt_state *state, splt_code *error)
{
  splt_flac_vorbis_tags *vorbis_tags = splt_flac_vorbis_tags_new(error);
  if (*error < 0) { return; }

  splt_flac_vorbis_tags_append_with_prefix(vorbis_tags, "TITLE=", tags->title, error);
  if (*error < 0) { goto end; }
  char *artist_or_performer = splt_tu_get_artist_or_performer_ptr(tags);
  splt_flac_vorbis_tags_append_with_prefix(vorbis_tags, "ARTIST=", artist_or_performer, error);
  if (*error < 0) { goto end; }
  splt_flac_vorbis_tags_append_with_prefix(vorbis_tags, "ALBUM=", tags->album, error);
  if (*error < 0) { goto end; }
  splt_flac_vorbis_tags_append_with_prefix(vorbis_tags, "DATE=", tags->year, error);
  if (*error < 0) { goto end; }
  splt_flac_vorbis_tags_append_with_prefix(vorbis_tags, "GENRE=", tags->genre, error);
  if (*error < 0) { goto end; }
  splt_flac_vorbis_tags_append_with_prefix(vorbis_tags, "COMMENT=", tags->comment, error);
  if (*error < 0) { goto end; }
  char track_str[255] = { '\0' };
  snprintf(track_str, 254, "%d", tags->track);
  splt_flac_vorbis_tags_append_with_prefix(vorbis_tags, "TRACKNUMBER=", track_str, error);
  if (*error < 0) { goto end; }

  if (tags->set_original_tags && flac_tags->other_tags)
  {
    FLAC__uint32 j = 0;
    for (;j < flac_tags->other_tags->number_of_tags; j++)
    {
      splt_flac_vorbis_tags_append(vorbis_tags, flac_tags->other_tags->tags[j], error);
      if (*error < 0) { goto end; }
    }
  }

  //4 is VORBIS_COMMENT metadata block type
  unsigned char block_flag_and_block_type = 4;
  //set as true the last metadata block flag
  block_flag_and_block_type |= 0x80;

  FLAC__uint32 total_bytes = 4 + flac_tags->vendor_length + 4 + vorbis_tags->total_bytes;

  FLAC__byte block_length[3];
  splt_flac_l_pack_uint32(total_bytes, block_length, 3);

  //metadata block header
  if (splt_io_fwrite(state, &block_flag_and_block_type, 1, 1, fr->out) != 1) { goto error; }
  if (splt_io_fwrite(state, block_length, 3, 1, fr->out) != 1) { goto error; }

  //metadata vorbis block
  FLAC__byte uint32_bytes[4];

  //vendor length and string
  splt_flac_l_pack_uint32_little_endian(flac_tags->vendor_length, uint32_bytes, 4);
  if (splt_io_fwrite(state, uint32_bytes, 4, 1, fr->out) != 1) { goto error; }
  if (splt_io_fwrite(state, flac_tags->vendor_string, flac_tags->vendor_length, 1, fr->out) != 1)
  {
    goto error;
  }

  //number of comments
  splt_flac_l_pack_uint32_little_endian(vorbis_tags->number_of_tags, uint32_bytes, 4);
  if (splt_io_fwrite(state, uint32_bytes, 4, 1, fr->out) != 1) { goto error; }

  FLAC__uint32 i = 0;
  for (;i < vorbis_tags->number_of_tags;i++)
  {
    FLAC__uint32 comment_length = (FLAC__uint32) strlen(vorbis_tags->tags[i]);
    splt_flac_l_pack_uint32_little_endian(comment_length, uint32_bytes, 4);
    //comment length and value
    if (splt_io_fwrite(state, uint32_bytes, 4, 1, fr->out) != 1) { goto error; }
    if (splt_io_fwrite(state, vorbis_tags->tags[i], comment_length, 1, fr->out) != 1) { goto error; }
  }

end:
  splt_flac_vorbis_tags_free(&vorbis_tags); 
  return;

error:
  splt_e_set_error_data(state, fr->output_fname);
  *error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
  splt_flac_vorbis_tags_free(&vorbis_tags); 
}

static void splt_flac_fr_open_file_and_write_metadata_if_first_time(splt_flac_frame_reader *fr,
    const splt_flac_metadatas *metadatas, const splt_flac_tags *flac_tags, 
    const splt_tags *tags_to_write, const char *output_fname, splt_state *state, splt_code *error)
{
  if (fr->out_streaminfo.total_samples != 0)
  {
    return;
  }

  splt_c_put_progress_text(state, SPLT_PROGRESS_CREATE);

  if (! splt_o_get_int_option(state, SPLT_OPT_PRETEND_TO_SPLIT))
  {
    fr->out = splt_io_fopen(output_fname, "wb+");
    if (fr->out == NULL)
    {
      splt_e_set_strerror_msg_with_data(state, output_fname);
      *error = SPLT_ERROR_CANNOT_OPEN_DEST_FILE;
      return;
    }
  }

  unsigned char space[4+SPLT_FLAC_METADATA_HEADER_LENGTH+SPLT_FLAC_STREAMINFO_LENGTH] = {'\0'};
  int space_size= 4 + SPLT_FLAC_METADATA_HEADER_LENGTH + SPLT_FLAC_STREAMINFO_LENGTH;
  if (splt_io_fwrite(state, space, space_size, 1, fr->out) != 1)
  {
    splt_e_set_error_data(state, fr->output_fname);
    *error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
  }

  int need_to_write_tags = SPLT_TRUE;
  if (flac_tags == NULL || tags_to_write == NULL)
  {
    need_to_write_tags = SPLT_FALSE;
  }

  splt_flac_fr_write_metadatas(fr, metadatas, !need_to_write_tags, output_fname, state, error);
  if (*error < 0) { return; }

  if (need_to_write_tags)
  {
    splt_flac_fr_write_tags(fr, flac_tags, tags_to_write, output_fname, state, error);
  }
}

static double splt_flac_fr_back_end_point_according_to_auto_adjust(splt_state *state, 
    double begin_point, double end_point, int *adjust_gap_secs)
{
  if (!*adjust_gap_secs)
  {
    return end_point;
  }

  double new_end_point = end_point;

  double adj = (double) (*adjust_gap_secs);
  double len = (end_point - begin_point);
  if (adj > len)
  {
    adj = len;
  }

  if (end_point > adj)
  {
    new_end_point -= adj;
    *adjust_gap_secs = (int) adj;
  }
  else
  {
    *adjust_gap_secs = 0;
  }

  return new_end_point;
}

static void update_progress(splt_state *state, double first_time, double time, double end_point,
    short before_adjust)
{
  double current = time - first_time;
  double total = end_point - first_time;

  if ((splt_o_get_int_option(state, SPLT_OPT_SPLIT_MODE) == SPLT_OPTION_SILENCE_MODE) ||
      (splt_o_get_int_option(state, SPLT_OPT_SPLIT_MODE) == SPLT_OPTION_TRIM_SILENCE_MODE) ||
      (!splt_o_get_int_option(state, SPLT_OPT_AUTO_ADJUST)))
  {
    splt_c_update_progress(state, current, total, 1, 0, SPLT_DEFAULT_PROGRESS_RATE2);
  }
  else
  {
    if (before_adjust)
    {
      splt_c_update_progress(state, current, total, 2, 0, SPLT_DEFAULT_PROGRESS_RATE2);
    }
    else
    {
      splt_c_update_progress(state, current, total, 4, 0.75, SPLT_DEFAULT_PROGRESS_RATE2);
    }
  }
}

void splt_flac_fr_read_and_write_frames(splt_state *state, splt_flac_frame_reader *fr,
    const splt_flac_metadatas *metadatas, const splt_flac_tags *flac_tags,
    const splt_tags *tags_to_write,
    const char *output_fname,
    double begin_point, double end_point, int save_end_point,
    unsigned min_blocksize, unsigned max_blocksize, 
    unsigned bits_per_sample, unsigned sample_rate, unsigned channels, 
    unsigned min_framesize, unsigned max_framesize,
    float offset,
    splt_code *error)
{
  if (splt_flac_fr_reset_for_new_file(fr) == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    goto end;
  }

  fr->out_streaminfo.sample_rate = sample_rate;
  fr->out_streaminfo.channels = channels;
  fr->out_streaminfo.bits_per_sample = bits_per_sample;

  splt_su_copy(output_fname, &fr->output_fname);

  if (save_end_point && fr->previous_frame)
  {
    splt_flac_fr_open_file_and_write_metadata_if_first_time(fr, metadatas, flac_tags,
        tags_to_write, output_fname, state, error);

    splt_flac_fr_write_frame_processor(fr->previous_frame, fr->previous_frame_length, state, error, fr);
    free(fr->previous_frame);
    fr->previous_frame = NULL;
    fr->previous_frame_length = 0;

    splt_flac_fr_set_next_frame_and_sample_numbers(fr, error);
    if (*error < 0) { goto end; }

    fr->out_streaminfo.total_samples += fr->blocksize;
  }

  if (save_end_point && fr->end_point > 0)
  {
    begin_point = fr->end_point;
  }

  unsigned long adjust_gap_hundr = 0;
  int adjust_gap_secs = splt_o_get_int_option(state, SPLT_OPT_PARAM_GAP);

  short end_point_is_eof = splt_u_fend_sec_is_bigger_than_total_time(state, end_point);
  double total_time = splt_t_get_total_time_as_double_secs(state);
  if (!end_point_is_eof && end_point < total_time)
  {
    end_point = splt_flac_fr_back_end_point_according_to_auto_adjust(state,
        begin_point, end_point, &adjust_gap_secs);
    adjust_gap_hundr = (unsigned long) adjust_gap_secs * 100;
  }

  off_t previous_offset = 0;

  int we_continue = 1;
  double first_time = -1;
  short before_adjust = SPLT_TRUE;

  while (we_continue)
  {
    unsigned frame_byte_buffer_start = 0;
    if (fr->buffer != NULL) { frame_byte_buffer_start = fr->next_byte; }

    splt_flac_fr_read_frame(fr, min_blocksize, max_blocksize, bits_per_sample, state, error);
    if (*error < 0) { goto end; }

    double time = (double) fr->current_sample_number / (double) sample_rate;
    if (first_time < 0) { first_time = time; }

    update_progress(state, first_time, time, end_point, before_adjust);

    if (time >= begin_point && (time < end_point || end_point < 0))
    {
      splt_flac_fr_open_file_and_write_metadata_if_first_time(fr, metadatas, flac_tags,
          tags_to_write, output_fname, state, error);

      splt_flac_u_process_frame(fr, frame_byte_buffer_start, state, error,
          splt_flac_fr_write_frame_processor, fr);
      if (*error < 0) { goto end; }

      splt_flac_fr_set_next_frame_and_sample_numbers(fr, error);
      if (*error < 0) { goto end; }

      fr->out_streaminfo.total_samples += fr->blocksize;
    }
    else if (end_point > 0 && time >= end_point)
    {
      if (adjust_gap_hundr)
      {
        float threshold = splt_o_get_float_option(state, SPLT_OPT_PARAM_THRESHOLD);
        float min_length = splt_o_get_float_option(state, SPLT_OPT_PARAM_MIN_LENGTH);
        int shots = splt_o_get_int_option(state, SPLT_OPT_PARAM_SHOTS);

        unsigned long length = 2 * adjust_gap_hundr;

        int silence_points_found = splt_flac_scan_silence(state, previous_offset, length,
            threshold, min_length, shots, 0, error, splt_scan_silence_processor);

        if (silence_points_found > 0)
        {
          end_point = (double) splt_siu_silence_position(state->silence_list, offset);
        }
        else
        {
          end_point += (double) adjust_gap_secs;

          *error = splt_u_process_no_auto_adjust_found(state, end_point);
          if (*error < 0) { goto end; }
        }

        splt_siu_ssplit_free(&state->silence_list);
        adjust_gap_hundr = 0;
        splt_c_put_progress_text(state, SPLT_PROGRESS_CREATE);

        before_adjust = SPLT_FALSE;
        first_time = time;
      }

      if (end_point > 0 && time >= end_point)
      {
        splt_flac_u_process_frame(fr, frame_byte_buffer_start, state, error,
            splt_flac_fr_backup_frame_processor, fr);
        we_continue = 0;
      }
      else
      {
        //process frame if auto adjust changed the end point
        splt_flac_u_process_frame(fr, frame_byte_buffer_start, state, error,
            splt_flac_fr_write_frame_processor, fr);
        if (*error < 0) { goto end; }

        splt_flac_fr_set_next_frame_and_sample_numbers(fr, error);
        if (*error < 0) { goto end; }

        fr->out_streaminfo.total_samples += fr->blocksize;
      }
    }
    else
    {
      splt_c_put_progress_text(state, SPLT_PROGRESS_PREPARE);
      splt_flac_u_process_frame(fr, frame_byte_buffer_start, state, error, NULL, fr);
    }

    if (feof(fr->in) && fr->next_byte >= fr->read_bytes)
    {
      *error = SPLT_OK_SPLIT_EOF;
      break;
    }

    previous_offset = ftello(fr->in) - (fr->read_bytes - fr->next_byte);
  }

  if (fr->out_streaminfo.total_samples != 0)
  {
    splt_flac_fr_finish_and_write_streaminfo(state, min_blocksize, max_blocksize,
        min_framesize, max_framesize, metadatas, fr, error);
  }
  else
  {
    *error = SPLT_ERROR_BEGIN_OUT_OF_FILE;
  }

  if (save_end_point)
  {
    fr->end_point = end_point;
  }
  else
  {
    fr->end_point = 0;
  }

end:
  ;

  if (fr->out)
  {
    if (fclose(fr->out) != 0)
    {
      splt_e_set_strerror_msg_with_data(state, output_fname);
      *error = SPLT_ERROR_CANNOT_CLOSE_FILE;
      return;
    }
  }
}

