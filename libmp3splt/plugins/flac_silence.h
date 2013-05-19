/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2013 Alexandru Munteanu - m@ioalex.net
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

#ifndef MP3SPLT_FLAC_SILENCE_H

#include "flac.h"
#include "silence_processors.h"
#include "splt.h"

int splt_flac_scan_silence(splt_state *state, off_t start_offset, unsigned long length,
    float threshold, float min, int shots, short output, int *error,
    short silence_processor(double time, float level, int silence_was_found, short must_flush,
      splt_scan_silence_data *ssd, int *found, int *error));

typedef struct {
  int error;
  splt_state *state;
  splt_flac_state *flacstate;
  double time;
  int silence_found;
  float threshold;
} splt_flac_silence_data;

#define MP3SPLT_FLAC_SILENCE_H

#endif

