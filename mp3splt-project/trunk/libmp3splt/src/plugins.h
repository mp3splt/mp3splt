/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2012 Alexandru Munteanu - io_fx@yahoo.fr
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

#ifndef SPLT_PLUGINS_H

float splt_p_get_version(splt_state *state, int *error);
const char *splt_p_get_name(splt_state *state, int *error);
const char *splt_p_get_extension(splt_state *state, int *error);
const char *splt_p_get_upper_extension(splt_state *state, int *error);
int splt_p_check_plugin_is_for_file(splt_state *state, int *error);
void splt_p_search_syncerrors(splt_state *state, int *error);
void splt_p_dewrap(splt_state *state, int listonly, const char *dir, int *error);
void splt_p_set_total_time(splt_state *state, int *error);
double splt_p_split(splt_state *state, const char *final_fname, double begin_point,
    double end_point, int *error, int save_end_point);
int splt_p_simple_split(splt_state *state, const char *output_fname, off_t begin,
    off_t end);
int splt_p_scan_silence(splt_state *state, int *error);
int splt_p_scan_trim_silence(splt_state *state, int *error);
void splt_p_set_original_tags(splt_state *state, int *error);
void splt_p_clear_original_tags(splt_state *state, int *error);

int splt_p_find_get_plugins_data(splt_state *state);
int splt_p_append_plugin_scan_dir(splt_state *state, const char *dir);

void splt_p_init(splt_state *state, int *error);
void splt_p_end(splt_state *state, int *error);

int splt_p_set_default_values(splt_state *state);
void splt_p_free_plugins(splt_state *state);

void splt_p_set_current_plugin(splt_state *state, int current_plugin);
int splt_p_get_current_plugin(splt_state *state);

int splt_p_file_is_supported_by_plugins(splt_state *state, const char *fname);

#define SPLT_PLUGINS_H

#endif

