/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2005-2013 Alexandru Munteanu - m@ioalex.net
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
 *
 *********************************************************/

#ifndef GSTREAMER_CONTROL_H

#define GSTREAMER_CONTROL_H

#include "all_includes.h"

void gstreamer_get_song_infos(gchar *total_infos, ui_state *ui);
gchar *gstreamer_get_filename(ui_state *ui);
gint gstreamer_get_playlist_number(ui_state *ui);
gchar *gstreamer_get_title_song(ui_state *ui);
gint gstreamer_get_time_elapsed(ui_state *ui);
void gstreamer_start(ui_state *ui);
void gstreamer_select_last_file(ui_state *ui);
void gstreamer_play_last_file(ui_state *ui);
void gstreamer_add_files(GList *list, ui_state *ui);
void gstreamer_set_volume(gint volume, ui_state *ui);
gint gstreamer_get_volume(ui_state *ui);
void gstreamer_start_with_songs(GList *list, ui_state *ui);
gint gstreamer_is_running(ui_state *ui);
gint gstreamer_is_paused(ui_state *ui);
void gstreamer_play(ui_state *ui);
void gstreamer_stop(ui_state *ui);
void gstreamer_pause(ui_state *ui);
void gstreamer_next(ui_state *ui);
void gstreamer_prev(ui_state *ui);
void gstreamer_jump(gint position, ui_state *ui);
gint gstreamer_get_total_time(ui_state *ui);
gint gstreamer_is_playing(ui_state *ui);
void gstreamer_quit(ui_state *ui);

#endif

