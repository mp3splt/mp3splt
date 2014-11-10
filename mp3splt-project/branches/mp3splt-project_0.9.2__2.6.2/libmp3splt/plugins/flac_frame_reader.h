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

#ifndef MP3SPLT_FLAC_FRAME_READER_H

#include <stdio.h>
#include <stdint.h>

#include <FLAC/all.h>

#include "splt.h"

#include "flac_metadata.h"
#include "flac_tags.h"
#include "flac_md5_decoder.h"

typedef struct {
  //input file
  FILE *in;
  //input buffer
  unsigned char *buffer;
  char *output_fname;

  //output file
  FILE *out;
  //output buffer
  unsigned char *output_buffer;
  unsigned output_buffer_times;
  FLAC__StreamMetadata_StreamInfo out_streaminfo;

  //computed crc8 and crc16 for checking purposes
  unsigned char crc8;
  unsigned crc16;

  //store infos about the current frame
  unsigned bits_per_sample;
  unsigned blocksize;
  unsigned char blocking_strategy;
  unsigned char channels;
  unsigned char channel_assignment;

  //byte reader informations
  unsigned char remaining_bits;
  unsigned char last_byte;
  unsigned next_byte;
  unsigned read_bytes;

  //for frame modification variables

  //sample and frame number to be written to the modified frame
  FLAC__uint64 frame_number;
  FLAC__uint64 sample_number;

  uint64_t current_sample_number;

  unsigned char *frame_number_as_utf8;
  unsigned char frame_number_as_utf8_length;
  unsigned char *sample_number_as_utf8;
  unsigned char sample_number_as_utf8_length;

  //sample number of bytes read from original frame
  unsigned char sample_number_bytes;
  unsigned char frame_number_bytes;

  int bytes_between_frame_number_and_crc8;

  //we have to read 1 more frame for each file to know where to stop the split
  //and we backup this frame here for the next file split
  unsigned char *previous_frame;
  size_t previous_frame_length;

  double end_point;

  splt_flac_md5_decoder *flac_md5_d;
} splt_flac_frame_reader;

splt_flac_frame_reader *splt_flac_fr_new(FILE *in, const char *input_filename);
void splt_flac_fr_free(splt_flac_frame_reader *fr);

void splt_flac_fr_read_and_write_frames(splt_state *state, splt_flac_frame_reader *fr,
    const splt_flac_metadatas *metadatas, 
    const splt_flac_tags *flac_tags, const splt_tags *tags_to_write,
    const char *output_fname,
    double begin_point, double end_point, int save_end_point,
    unsigned min_blocksize, unsigned max_blocksize, 
    unsigned bits_per_sample, unsigned sample_rate, unsigned channels, 
    unsigned min_framesize, unsigned max_framesize,
    float offset,
    int *error);

#define SPLT_FLAC_FR_BUFFER_SIZE 2048

#define SPLT_FLAC_SUBFRAME_CONSTANT 1
#define SPLT_FLAC_SUBFRAME_FIXED 2
#define SPLT_FLAC_SUBFRAME_LPC 3
#define SPLT_FLAC_SUBFRAME_VERBATIM 4

#define SPLT_FLAC_LEFT_SIDE 1
#define SPLT_FLAC_RIGHT_SIDE 2
#define SPLT_FLAC_MID_SIDE 3
#define SPLT_FLAC_INDEPENDENT 4

#define MP3SPLT_FLAC_FRAME_READER_H

#endif

