/**********************************************************
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2013 Alexandru Munteanu - m@ioalex.net
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

#ifndef SPLT_SILENCE_PROCESSORS_H

typedef struct {
  short first;
  short flush;
  double silence_begin;
  double silence_end;
  int len;
  int found;
  int shot;
  int number_of_shots;
  float min;
  splt_state *state;
  short silence_begin_was_found;
  short set_new_length;

  short continue_after_silence;
  double previous_time;
} splt_scan_silence_data;

short splt_scan_silence_processor(double time, float level, int silence_was_found, short must_flush, 
    splt_scan_silence_data *data, int *found_silence_points, int *error);

short splt_trim_silence_processor(double time, float level, int silence_was_found, short must_flush,
    splt_scan_silence_data *data, int *found_silence_points, int *error);

splt_scan_silence_data *splt_scan_silence_data_new(splt_state *state, short first, 
    float min, int shots, short set_new_length);

void splt_free_scan_silence_data(splt_scan_silence_data **ssd);

#define SPLT_SILENCE_PROCESSORS_H

#endif

