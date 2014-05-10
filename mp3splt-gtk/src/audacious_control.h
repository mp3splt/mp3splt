/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2005-2014 Alexandru Munteanu - m@ioalex.net
 *
 * http://mp3splt.sourceforge.net/
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

/**********************************************************
 * Filename: audacious_control.h
 *
 * header to audacious_control.c
 *
 * audacious_control.c controls the audacious player
 *********************************************************/

#ifndef audacious_CONTROL_H

#define audacious_CONTROL_H

#include "external_includes.h"
#include "ui_types.h"

void myaudacious_get_song_infos(gchar *total_infos, ui_state *ui);
gchar *myaudacious_get_filename(ui_state *ui);
gint myaudacious_get_playlist_number(ui_state *ui);
gchar *myaudacious_get_title_song(ui_state *ui);
gint myaudacious_get_time_elapsed(ui_state *ui);
void myaudacious_start(ui_state *ui);
void myaudacious_select_last_file(ui_state *ui);
void myaudacious_play_last_file(ui_state *ui);
void myaudacious_add_files(GList *list, ui_state *ui);
void myaudacious_set_volume(gint volume, ui_state *ui);
gint myaudacious_get_volume(ui_state *ui);
void myaudacious_start_with_songs(GList *list, ui_state *ui);
gint myaudacious_is_running(ui_state *ui);
gint myaudacious_is_paused(ui_state *ui);
void myaudacious_play(ui_state *ui);
void myaudacious_stop(ui_state *ui);
void myaudacious_pause(ui_state *ui);
void myaudacious_next(ui_state *ui);
void myaudacious_prev(ui_state *ui);
void myaudacious_jump(gint position, ui_state *ui);
gint myaudacious_get_total_time(ui_state *ui);
gint myaudacious_is_playing(ui_state *ui);

#endif

