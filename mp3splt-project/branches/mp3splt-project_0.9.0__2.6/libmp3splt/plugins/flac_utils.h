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

#ifndef MP3SPLT_FLAC_UTILS_H

#include "flac_frame_reader.h"

unsigned char splt_flac_u_read_next_byte(splt_flac_frame_reader *fr, splt_code *error);
unsigned char splt_flac_u_read_next_byte_(void *flac_frame_reader, splt_code *error);
unsigned char splt_flac_u_read_bit(splt_flac_frame_reader *fr, splt_code *error);
unsigned char splt_flac_u_read_bits(splt_flac_frame_reader *fr, unsigned char bits_number,
    splt_code *error);

void splt_flac_u_read_up_to_total_bits(splt_flac_frame_reader *fr, unsigned total_bits,
    splt_code *error);
unsigned splt_flac_u_read_unsigned(splt_flac_frame_reader *fr, splt_code *error);

void splt_flac_u_read_zeroes_and_the_next_one(splt_flac_frame_reader *fr, splt_code *error);

void splt_flac_u_process_frame(splt_flac_frame_reader *fr,
    unsigned frame_byte_buffer_start, splt_state *state, splt_code *error,
    void (*frame_processor)(unsigned char *frame, size_t frame_length, 
      splt_state *state, splt_code *error, void *user_data),
    void *user_data);

#define MP3SPLT_FLAC_UTILS_H

#endif

