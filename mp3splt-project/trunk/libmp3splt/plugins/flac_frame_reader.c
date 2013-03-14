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

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

#include <math.h>

#include "flac_frame_reader.h"
#include "flac_utils.h"
#include "from_flac_library.h"
#include "flac_metadata_utils.h"

static void splt_flac_fr_read_header(splt_flac_frame_reader *fr,
    unsigned min_blocksize, unsigned max_blocksize, unsigned metadata_bits_per_sample, 
    splt_flac_code *error)
{
  fr->crc8 = 0;
  fr->bytes_between_frame_number_and_crc8 = 0;

  unsigned char sync_code_start = splt_flac_u_read_next_byte(fr, error);
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

  fprintf(stdout, "COMPUTED crc8 = %x\n", computed_crc8);
  fprintf(stdout, "REAL crc8 = %x\n", crc8);
  fflush(stdout);

  if (crc8 != computed_crc8) { *error = SPLT_FLAC_ERR_BAD_CRC8; }
}

static void splt_flac_fr_read_constant_subframe(splt_flac_frame_reader *fr, unsigned bits_per_sample,
    splt_flac_code *error)
{
  //fprintf(stdout, "reading constant subframe ...\n");
  //fflush(stdout);

  splt_flac_u_read_up_to_total_bits(fr, bits_per_sample, error);
}

static void splt_flac_fr_read_verbatim_subframe(splt_flac_frame_reader *fr, unsigned bits_per_sample,
    splt_flac_code *error)
{
  //fprintf(stdout, "reading verbatim subframe ...\n");
  //fflush(stdout);

  splt_flac_u_read_up_to_total_bits(fr, bits_per_sample * fr->blocksize, error);
}

static void splt_flac_fr_read_rice_residual(splt_flac_frame_reader *fr, unsigned order,
    splt_flac_code *error)
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

  //fprintf(stdout, "rice method %d\n", rice_method);
  //fflush(stdout);

  unsigned char rice_partition_order = splt_flac_u_read_bits(fr, 4, error);
  if (*error < 0) { return; }

  //fprintf(stdout, "RICE ORDER %u\n", rice_partition_order);
  //fflush(stdout);

  unsigned order_partition_number = pow(2, rice_partition_order);

  //fprintf(stdout, "order partition number = %lu\n", order_partition_number);
  //fflush(stdout);

  unsigned partition_number;
  for (partition_number = 1;partition_number <= order_partition_number; partition_number++)
  {
    //fprintf(stdout, "rice partition number = %d\n", partition_number);
    //fflush(stdout);

    unsigned char rice_parameter_or_escape_code = splt_flac_u_read_bits(fr, rice_method, error);
    if (*error < 0) { return; }

    //fprintf(stdout, "bits to read rice param = %u\n", rice_method);
    //fprintf(stdout, "rice parameter = %u\n", rice_parameter_or_escape_code);
    //fflush(stdout);

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

    //fprintf(stdout, "num of samples = %u\n", number_of_samples);
    //fflush(stdout);

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
    unsigned bits_per_sample, splt_flac_code *error)
{
  //fprintf(stdout, "reading fixed subframe ...\n");
  //fflush(stdout);

  splt_flac_u_read_up_to_total_bits(fr, bits_per_sample * order, error);
  if (*error < 0) { return; }

  splt_flac_fr_read_rice_residual(fr, order, error);
}

static void splt_flac_fr_read_lpc_subframe(splt_flac_frame_reader *fr, unsigned order, 
    unsigned bits_per_sample, splt_flac_code *error)
{
  //fprintf(stdout, "reading lpc subframe with %u bps ...\n", bits_per_sample);
  //fflush(stdout);

  splt_flac_u_read_up_to_total_bits(fr, bits_per_sample * order, error);
  if (*error < 0) { return; }

  unsigned char quantized_linear_predictor = splt_flac_u_read_bits(fr, 4, error);
  if (*error < 0) { return; }

  if ((quantized_linear_predictor & 0x0f) == 0x0f)
  {
    fprintf(stderr, "error in lpc subframe quantized predictor\n");
    fflush(stderr);
  }

  char qlp_coeff_precision = ((char) quantized_linear_predictor) + 1;

  //fprintf(stdout, "qlp coeff precision = %d\n", qlp_coeff_precision);
  //fflush(stdout);

  //qlp_coeff_shift
  char qlp_coeff_shift = (char) splt_flac_u_read_bits(fr, 5, error);
  if (*error < 0) { return; }

  //fprintf(stdout, "qlp coeff shift = %d\n", qlp_coeff_shift);
  //fflush(stdout);

  //fprintf(stdout, "ORDER = %d\n", order);
  //fprintf(stdout, "COEFF_PRECISION = %d\n", qlp_coeff_precision);
  //fflush(stdout);

  splt_flac_u_read_up_to_total_bits(fr, qlp_coeff_precision * order, error);
  if (*error < 0) { return; }

  splt_flac_fr_read_rice_residual(fr, order, error);
}

static void splt_flac_fr_read_subframe_content(splt_flac_frame_reader *fr, int sf_type, unsigned order,
    unsigned bits_per_sample, splt_flac_code *error)
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
      splt_flac_fr_read_lpc_subframe(fr, order, bits_per_sample, error);
      return;
  }

  *error = SPLT_FLAC_ERR_BAD_SUBFRAME_TYPE;
}

static void splt_flac_fr_read_subframe(splt_flac_frame_reader *fr, unsigned bits_per_sample,
    splt_flac_code *error)
{
  unsigned char bit_padding_subframe_type_and_wasted_bit = splt_flac_u_read_next_byte(fr, error);
  if (*error < 0) { return; }

  //fprintf(stdout, "subframe start = 0x%x\n", bit_padding_subframe_type_and_wasted_bit);
  //fflush(stdout);

  unsigned char zero_bit = bit_padding_subframe_type_and_wasted_bit >> 7;
  if (zero_bit != 0)
  {
    *error = SPLT_FLAC_ERR_ZERO_BIT_OF_SUBFRAME_HEADER_IS_WRONG;
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
    //fprintf(stdout, "wasted bits per sample = %d\n", wasted_bits_per_sample);
    //fflush(stdout);
  }

  bit_padding_subframe_type_and_wasted_bit &= 0xfe;
  unsigned char subframe_type = bit_padding_subframe_type_and_wasted_bit;
  //fprintf(stdout, "subframe type = %u\n", subframe_type);
  //fflush(stdout);

  int sf_type = 0;
  unsigned order = 0;

  if (subframe_type & 0x80)
  {
    *error = SPLT_FLAC_ERR_LOST_SYNC;
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
    *error = SPLT_FLAC_ERR_LOST_SYNC;
    return;
  }
  else if (subframe_type <= 24)
  {
    sf_type = SPLT_FLAC_SUBFRAME_FIXED;
    order = ((bit_padding_subframe_type_and_wasted_bit >> 1) & 7);
  }
  else if (subframe_type < 64)
  {
    *error = SPLT_FLAC_ERR_LOST_SYNC;
    return;
  }
  else
  {
    sf_type = SPLT_FLAC_SUBFRAME_LPC;
    order = ((bit_padding_subframe_type_and_wasted_bit >> 1) & 31) + 1;
  }

  //fprintf(stdout, "ORDER = %u\n", order);
  //fflush(stdout);

  splt_flac_fr_read_subframe_content(fr, sf_type, order, bits_per_sample - wasted_bits_per_sample, error);
}

static void splt_flac_fr_read_frame(splt_flac_frame_reader *fr,
    unsigned min_blocksize, unsigned max_blocksize, unsigned metadata_bits_per_sample,
    splt_flac_code *error)
{
  fr->crc16 = 0;

  splt_flac_fr_read_header(fr, min_blocksize, max_blocksize, metadata_bits_per_sample, error);
  if (*error < 0) { return; }

  //fprintf(stdout, "channels = %d\n", fr->channels);
  //fflush(stdout);

  unsigned channel = 0;
  for (channel = 0; channel < fr->channels; channel++)
  {
    unsigned bits_per_sample = fr->bits_per_sample;
    if ((fr->channel_assignment == SPLT_FLAC_LEFT_SIDE && channel == 1) ||
        (fr->channel_assignment == SPLT_FLAC_RIGHT_SIDE && channel == 0) ||
        (fr->channel_assignment == SPLT_FLAC_MID_SIDE && channel == 1))
      bits_per_sample++;

    //fprintf(stdout, "read subframe ...\n");
    //fflush(stdout);

    splt_flac_fr_read_subframe(fr, bits_per_sample, error);
    if (*error < 0) { return; }
  }

  splt_flac_u_read_up_to_total_bits(fr, fr->remaining_bits, error);
  if (*error < 0) { return; }

  unsigned computed_crc16 = fr->crc16;
  unsigned crc16 = splt_flac_u_read_unsigned(fr, error);
  if (*error < 0) { return; }

  if (crc16 != computed_crc16) { *error = SPLT_FLAC_ERR_BAD_CRC16; }

  fprintf(stdout, "crc16 = 0x%x\n", computed_crc16);
  fflush(stdout);
}

static splt_code splt_flac_fr_convert_flac_error_to_splt_error(splt_state *state, int frame_reader_error_code)
{
  if (frame_reader_error_code >= -1)
  {
    //TODO: EOF
    return SPLT_OK;
  }

  splt_d_print_debug(state, "flac error code %d\n", frame_reader_error_code);

  //TODO: switch case ...

  return SPLT_ERROR_INVALID;
  //TODO: set filename ?
}

static void splt_flac_fr_set_next_frame_and_sample_numbers(splt_flac_frame_reader *fr, int *error)
{
  fr->frame_number++;
  fr->sample_number = fr->frame_number * fr->blocksize;

  if (fr->frame_number_as_utf8) { free(fr->frame_number_as_utf8); }
  fr->frame_number_as_utf8 =
    splt_flac_l_convert_to_utf8((FLAC__uint64) fr->frame_number, &fr->frame_number_as_utf8_length);
  if (fr->frame_number_as_utf8 == NULL) { *error = SPLT_FLAC_ERR_CANNOT_ALLOCATE_MEMORY; return; }

  if (fr->sample_number_as_utf8) { free(fr->sample_number_as_utf8); }
  fr->sample_number_as_utf8 =
    splt_flac_l_convert_to_utf8((FLAC__uint64) fr->sample_number, &fr->sample_number_as_utf8_length);
  if (fr->sample_number_as_utf8 == NULL) { *error = SPLT_FLAC_ERR_CANNOT_ALLOCATE_MEMORY; return; }
}

static void splt_flac_fr_write_frame_processor(unsigned char *frame, size_t frame_length,
    splt_flac_code *error, void *user_data)
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
    *error = SPLT_FLAC_ERR_CANNOT_ALLOCATE_MEMORY;
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
  /*fprintf(stdout, "frame or sample utf8 length = %d\n", frame_or_sample_utf8_length);
  fflush(stdout);*/
  for (;i < frame_or_sample_utf8_length;i++)
  {
    modified_frame[4 + i] = frame_or_sample_utf8_bytes[i];
    ptr = modified_frame + 4 + i;
  }

  //copy everything else
  size_t length_up_to_including_frame_number = 4 + frame_or_sample_read_length;
  /*fprintf(stdout, "frame number length = %d\n", frame_or_sample_read_length);
  fflush(stdout);*/

  memcpy(ptr + 1,
      frame + length_up_to_including_frame_number,
      frame_length - length_up_to_including_frame_number);

  /*unsigned crc16 = 0;
  crc16 |= ((unsigned)modified_frame[modified_frame_length - 2]) << 8;
  fprintf(stdout, "crc16 from modified frame = 0x%x\n", 
      crc16 | ((unsigned)modified_frame[modified_frame_length - 1]));
  fflush(stdout);*/

  /*fprintf(stdout, "bytes between frame number and crc8 = %d\n",
      fr->bytes_between_frame_number_and_crc8);
  fflush(stdout);*/

  //compute and set new crc8
  size_t j = 0;
  size_t before_crc8_length = 
    4 + frame_or_sample_utf8_length + fr->bytes_between_frame_number_and_crc8;
  /*fprintf(stdout, "bytes between frame number and crc8 = %ld\n", fr->bytes_between_frame_number_and_crc8);
  fprintf(stdout, "before crc8 length = %ld\n", (long int) before_crc8_length);
  fflush(stdout);*/
  unsigned char new_crc8 = 0;
  for (;j < before_crc8_length;j++)
  {
    SPLT_FLAC_UPDATE_CRC8(new_crc8, modified_frame[j]);
  }
  modified_frame[j] = new_crc8;

  /*fprintf(stdout, "new crc8 = %x\n", new_crc8);
  fflush(stdout);*/

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

  /*fprintf(stdout, "modified frame length = %llu\n", (long long unsigned) modified_frame_length);
  fprintf(stdout, "frame length = %llu\n", (long long unsigned) frame_length);
  fflush(stdout);

  for (j = 0;j < frame_length;j++)
  {
    fprintf(stdout, "Ox%x\tNx%x\n", frame[j], modified_frame[j]);
    fflush(stdout);
  }*/

  //TODO: change with splt_io_fwrite
  if (fwrite(modified_frame, modified_frame_length, 1, fr->out) != 1)
  {
    *error = SPLT_FLAC_ERR_FAILED_TO_WRITE_OUTPUT_FILE;
  }

  free(modified_frame);
}

static void splt_flac_fr_backup_frame_processor(unsigned char *frame, size_t frame_length,
    splt_flac_code *error, void *user_data)
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
    splt_flac_frame_reader *fr, splt_flac_code *error)
{
  if (min_framesize == 0) { fr->out_streaminfo.min_framesize = 0; }
  if (max_framesize == 0) { fr->out_streaminfo.max_framesize = 0; }

  if (min_blocksize == max_blocksize)
  {
    fr->out_streaminfo.min_blocksize = min_blocksize;
    fr->out_streaminfo.max_blocksize = max_blocksize;
  }

  unsigned char *streaminfo_bytes = splt_flac_l_convert_from_streaminfo(&fr->out_streaminfo);
  if (streaminfo_bytes == NULL)
  {
    *error = SPLT_FLAC_ERR_CANNOT_ALLOCATE_MEMORY;
    return;
  }

  unsigned char *metadata_header =
    splt_flac_mu_build_metadata_header(SPLT_FLAC_METADATA_STREAMINFO, 1, SPLT_FLAC_STREAMINFO_LENGTH);
  if (metadata_header == NULL)
  {
    free(streaminfo_bytes);
    *error = SPLT_FLAC_ERR_CANNOT_ALLOCATE_MEMORY;
    return;
  }

  //TODO: remove fseek for stdout; how to ?
  rewind(fr->out);

  unsigned char flac_word[4] = { 0x66, 0x4C, 0x61, 0x43 };
  if (fwrite(flac_word, 4, 1, fr->out) != 1)
  {
    *error = SPLT_FLAC_ERR_FAILED_TO_WRITE_OUTPUT_FILE;
    goto end;
  }

  //TODO: change is_last from 1 to 0 when writing other metadata types ...
  if (fwrite(metadata_header, SPLT_FLAC_METADATA_HEADER_LENGTH, 1, fr->out) != 1)
  {
    *error = SPLT_FLAC_ERR_FAILED_TO_WRITE_OUTPUT_FILE;
    goto end;
  }

  if (fwrite(streaminfo_bytes, SPLT_FLAC_STREAMINFO_LENGTH, 1, fr->out) != 1)
  {
    *error = SPLT_FLAC_ERR_FAILED_TO_WRITE_OUTPUT_FILE;
  }

end:
  free(metadata_header);
  free(streaminfo_bytes);

  /*  fprintf(stdout, "min blocksize = %u\n", fr->out_streaminfo.min_blocksize);
      fprintf(stdout, "max blocksize = %u\n", fr->out_streaminfo.max_blocksize);
      fprintf(stdout, "min framesize = %u\n", fr->out_streaminfo.min_framesize);
      fprintf(stdout, "max framesize = %u\n", fr->out_streaminfo.max_framesize);
      fprintf(stdout, "sample rate = %u\n", fr->out_streaminfo.sample_rate);
      fprintf(stdout, "channels = %u\n", fr->out_streaminfo.channels);
      fprintf(stdout, "bits per sample = %u\n", fr->out_streaminfo.bits_per_sample);
      fprintf(stdout, "total_samples = %llu\n", (long long unsigned) fr->out_streaminfo.total_samples);
      fflush(stdout);*/
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

  return fr;
}

splt_flac_frame_reader *splt_flac_fr_new(FILE *in)
{
  splt_flac_frame_reader *fr = malloc(sizeof(splt_flac_frame_reader));
  if (fr == NULL) { return NULL; }

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

  free(fr);
}

static void splt_flac_fr_open_file_and_reserve_streaminfo_space_if_first_time(splt_flac_frame_reader *fr,
    const char *output_fname, splt_flac_code *error)
{
  if (fr->out_streaminfo.total_samples != 0)
  {
    return;
  }

  fprintf(stdout, "open file\n");
  fflush(stdout);

  fr->out = splt_io_fopen(output_fname, "wb+");
  if (fr->out == NULL)
  {
    *error = SPLT_FLAC_ERR_FAILED_TO_OPEN_FILE;
    return;
  }

  unsigned char space[4+SPLT_FLAC_METADATA_HEADER_LENGTH+SPLT_FLAC_STREAMINFO_LENGTH]={'\0'};
  int space_size= 4 + SPLT_FLAC_METADATA_HEADER_LENGTH + SPLT_FLAC_STREAMINFO_LENGTH;
  if (fwrite(space, space_size, 1, fr->out) != 1)
  {
    *error = SPLT_FLAC_ERR_FAILED_TO_WRITE_OUTPUT_FILE;
  }
}

void splt_flac_fr_read_and_write_frames(splt_state *state, splt_flac_frame_reader *fr, 
    const char *output_fname,
    double begin_point, double end_point, int save_end_point,
    unsigned min_blocksize, unsigned max_blocksize, 
    unsigned bits_per_sample, unsigned sample_rate, unsigned channels, 
    unsigned min_framesize, unsigned max_framesize,
    int *error)
{
  int frame_reader_error_code = SPLT_FLAC_OK;

  if (splt_flac_fr_reset_for_new_file(fr) == NULL)
  {
    frame_reader_error_code = SPLT_FLAC_ERR_CANNOT_ALLOCATE_MEMORY;
    goto end;
  }

  fr->out_streaminfo.sample_rate = sample_rate;
  fr->out_streaminfo.channels = channels;
  fr->out_streaminfo.bits_per_sample = bits_per_sample;

  if (save_end_point && fr->previous_frame)
  {
    splt_flac_fr_open_file_and_reserve_streaminfo_space_if_first_time(fr, output_fname, 
        &frame_reader_error_code);

    splt_flac_fr_write_frame_processor(fr->previous_frame, fr->previous_frame_length,
        &frame_reader_error_code, fr);
    free(fr->previous_frame);
    fr->previous_frame = NULL;
    fr->previous_frame_length = 0;

    splt_flac_fr_set_next_frame_and_sample_numbers(fr, &frame_reader_error_code);
    if (frame_reader_error_code < 0) { goto end; }

    fr->out_streaminfo.total_samples += fr->blocksize;
  }

  int we_continue = 1;
  while (we_continue)
  {
    unsigned frame_byte_buffer_start = 0;
    if (fr->buffer != NULL) { frame_byte_buffer_start = fr->next_byte; }

    splt_flac_fr_read_frame(fr, min_blocksize, max_blocksize, bits_per_sample, 
        &frame_reader_error_code);
    if (frame_reader_error_code < 0) { goto end; }

    double time = (double) fr->current_sample_number / (double) sample_rate;
    if (time >= begin_point && (time < end_point || end_point < 0))
    {
      splt_flac_fr_open_file_and_reserve_streaminfo_space_if_first_time(fr, output_fname, 
          &frame_reader_error_code);

      splt_flac_u_process_frame(fr, frame_byte_buffer_start, &frame_reader_error_code,
          splt_flac_fr_write_frame_processor, fr);
      if (frame_reader_error_code < 0) { goto end; }

      splt_flac_fr_set_next_frame_and_sample_numbers(fr, &frame_reader_error_code);
      if (frame_reader_error_code < 0) { goto end; }

      fr->out_streaminfo.total_samples += fr->blocksize;
    }
    else if (end_point > 0 && time >= end_point)
    {
      splt_flac_u_process_frame(fr, frame_byte_buffer_start, &frame_reader_error_code, 
          splt_flac_fr_backup_frame_processor, fr);
      we_continue = 0;
    }
    else
    {
      splt_flac_u_process_frame(fr, frame_byte_buffer_start, &frame_reader_error_code, NULL, fr);
    }

    if (feof(fr->in) && fr->next_byte >= fr->read_bytes)
    {
      break;
    }
  }

  if (fr->out_streaminfo.total_samples != 0)
  {
    splt_flac_fr_finish_and_write_streaminfo(state, min_blocksize, max_blocksize,
        min_framesize, max_framesize, fr, &frame_reader_error_code);
  }
  else
  {
    frame_reader_error_code = SPLT_FLAC_ERR_BEGIN_OUT_OF_FILE;
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

  splt_code err = splt_flac_fr_convert_flac_error_to_splt_error(state, frame_reader_error_code);
  if (err < 0) { *error = err; }
}

