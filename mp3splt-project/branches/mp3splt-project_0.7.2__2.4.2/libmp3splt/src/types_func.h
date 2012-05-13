/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2012 Munteanu Alexandru - io_fx@yahoo.fr
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307,
 * USA.
 *
 *********************************************************/

#ifndef SPLT_TYPES_FUNC_H

splt_state *splt_t_new_state(splt_state *state, int *error);
void splt_t_free_state(splt_state *state);

void splt_t_set_total_time(splt_state *state, long value);
long splt_t_get_total_time(splt_state *state);
double splt_t_get_total_time_as_double_secs(splt_state *state);

void splt_t_set_new_filename_path(splt_state *state, 
    const char *new_filename_path, int *error);
char *splt_t_get_new_filename_path(splt_state *state);

int splt_t_set_path_of_split(splt_state *state, const char *path);
char *splt_t_get_path_of_split(splt_state *state);

int splt_t_set_m3u_filename(splt_state *state, const char *filename);
char *splt_t_get_m3u_filename(splt_state *state);
char *splt_t_get_m3u_file_with_path(splt_state *state, int *error);

int splt_t_set_input_filename_regex(splt_state *state, const char *regex);
char *splt_t_get_input_filename_regex(splt_state *state);

int splt_t_set_default_comment_tag(splt_state *state, const char *default_comment);
char *splt_t_get_default_comment_tag(splt_state *state);

int splt_t_set_default_genre_tag(splt_state *state, const char *default_genre);
char *splt_t_get_default_genre_tag(splt_state *state);

int splt_t_set_silence_log_fname(splt_state *state, const char *filename);
char *splt_t_get_silence_log_fname(splt_state *state);

int splt_t_set_filename_to_split(splt_state *state, const char *filename);
char *splt_t_get_filename_to_split(splt_state *state);
char *splt_t_get_filename_to_split(splt_state *state);

void splt_t_set_current_split(splt_state *state, int index);
void splt_t_current_split_next(splt_state *state);
int splt_t_get_current_split(splt_state *state);
int splt_t_get_current_split_file_number(splt_state *state);

void splt_t_set_splitnumber(splt_state *state, int number);
int splt_t_get_splitnumber(splt_state *state);

void splt_t_free_splitpoints_tags(splt_state *state);

void splt_t_clean_one_split_data(splt_state *state, int num);
void splt_t_clean_split_data(splt_state *state,int tracks);

int splt_t_split_is_canceled(splt_state *state);
void splt_t_set_stop_split(splt_state *state, int bool_value);

#define SPLT_TYPES_FUNC_H

#endif

