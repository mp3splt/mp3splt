/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2014 Alexandru Munteanu - m@ioalex.net
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

#ifndef SPLT_SPLIT_POINTS_H

int splt_sp_splitpoint_exists(splt_state *state, int index);
int splt_sp_get_real_splitpoints_number(splt_state *state);

int splt_sp_append_splitpoint(splt_state *state, long split_value,
    const char *name, int type);
splt_points *splt_sp_get_splitpoints(splt_state *state);
void splt_sp_free_splitpoints(splt_state *state);
void splt_sp_free_one_splitpoint(splt_point *point);

int splt_sp_set_splitpoint_value(splt_state *state, int index, long split_value);
int splt_sp_set_splitpoint_name(splt_state *state, int index, const char *name);
int splt_sp_set_splitpoint_type(splt_state *state, int index, int type);


long splt_sp_get_splitpoint_value(splt_state *state, int index, int *error);
const char *splt_sp_get_splitpoint_name(splt_state *state, int index, int *error);
int splt_sp_get_splitpoint_type(splt_state *state, int index, int *error);

int splt_sp_cut_splitpoint_extension(splt_state *state, int index);

void splt_sp_order_splitpoints(splt_state *state, int len);
void splt_sp_skip_minimum_track_length_splitpoints(splt_state *state, int *error);
void splt_sp_join_minimum_tracks_splitpoints(splt_state *state, int *error);

void splt_sp_get_mins_secs_hundr_from_splitpoint(long splitpoint,
    long *mins, long *secs, long *hudr);
void splt_sp_get_mins_secs_frames_from_splitpoint(long splitpoint,
    long *mins, long *secs, long *frames);

long splt_sp_overlap_time(splt_state *state, int splitpoint_index);

splt_array *splt_sp_find_intervals_between_two_consecutive_big_tracks(splt_state *state,
    int min_track_join, int *error);

#define SPLT_SPLIT_POINTS_H

#endif

