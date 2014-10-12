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

#include "splt.h"

#include "mp3.h"

void splt_mp3_init_stream_frame(splt_mp3_state *mp3state);
void splt_mp3_finish_stream_frame(splt_mp3_state *mp3state);
void splt_mp3_checksync(splt_mp3_state *mp3state);
int splt_mp3_c_bitrate(unsigned long head);
struct splt_header splt_mp3_makehead(unsigned long headword, 
    struct splt_mp3 mp3f, struct splt_header head, off_t ptr);
off_t splt_mp3_findhead(splt_mp3_state *mp3state, off_t start);
off_t splt_mp3_findvalidhead(splt_mp3_state *mp3state, off_t start);
int splt_mp3_get_frame(splt_mp3_state *mp3state);
int splt_mp3_get_valid_frame(splt_state *state, int *error);

void splt_mp3_read_process_side_info_main_data_begin(splt_mp3_state *mp3state, off_t offset);
void splt_mp3_extract_reservoir_and_build_reservoir_frame(splt_mp3_state *mp3state,
    splt_state *state, splt_code *error);
void splt_mp3_build_xing_lame_frame(splt_mp3_state *mp3state, off_t begin, off_t end, 
    unsigned long fbegin, splt_code *error, splt_state *state);

int splt_mp3_get_mpeg_as_int(int mpgid);
int splt_mp3_get_samples_per_frame(struct splt_mp3 *mp3file);
void splt_mp3_parse_xing_lame(splt_mp3_state *mp3state);

unsigned long splt_mp3_find_begin_frame(double fbegin_sec, splt_mp3_state *mp3state,
    splt_state *state, splt_code *error);
unsigned long splt_mp3_find_end_frame(double fend_sec, splt_mp3_state *mp3state, 
    splt_state *state);

void splt_mp3_get_overlapped_frames(long last_frame, splt_mp3_state *mp3state,
    splt_state *state, splt_code *error);
int splt_mp3_handle_bit_reservoir(splt_state *state);

