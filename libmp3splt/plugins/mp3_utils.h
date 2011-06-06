/**********************************************************
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2011 Alexandru Munteanu - io_fx@yahoo.fr
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307,
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
int splt_mp3_xing_info_off(splt_mp3_state *mp3state);
int splt_mp3_get_frame(splt_mp3_state *mp3state);
int splt_mp3_get_valid_frame(splt_state *state, int *error);

