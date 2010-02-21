/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2010 Alexandru Munteanu - io_fx@yahoo.fr
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

#ifndef MP3SPLT_PLUGIN_UTILS_H

void splt_tu_free_original_tags(splt_state *state);
void splt_tu_auto_increment_tracknumber(splt_state *state);
int splt_tu_append_original_tags(splt_state *state);
int splt_tu_append_tags(splt_state *state, 
    const char *title, const char *artist,
    const char *album, const char *performer,
    const char *year, const char *comment,
    int track, unsigned char genre);
int splt_tu_append_only_non_null_previous_tags(splt_state *state, 
    const char *title, const char *artist,
    const char *album, const char *performer,
    const char *year, const char *comment,
    int track, unsigned char genre);
void splt_tu_reset_tags(splt_tags *tags);
int splt_tu_new_tags_if_necessary(splt_state *state, int index);
int splt_tu_tags_exists(splt_state *state, int index);
int splt_tu_set_tags_field(splt_state *state, int index,
    int tags_field, const void *data);
int splt_tu_set_like_x_tags_field(splt_state *state,
    int tags_field, const void *data);
int splt_tu_set_original_tags_field(splt_state *state,
    int tags_field, const void *data);
splt_tags *splt_tu_get_tags(splt_state *state, int *tags_number);

int splt_tu_set_tags_in_tags(splt_state *state, int current_tags);
splt_tags *splt_tu_get_tags_at(splt_state *state, int tags_index);
splt_tags splt_tu_get_last_tags(splt_state *state);
void *splt_tu_get_tags_field(splt_state *state, int index, int tags_field);
void splt_tu_free_tags(splt_state *state);
splt_tags *splt_tu_get_tags_like_x(splt_state *state);
splt_tags *splt_tu_get_current_tags(splt_state *state);
void splt_tu_get_original_tags(splt_state *state, int *err);

#define MP3SPLT_PLUGIN_UTILS_H

#endif

