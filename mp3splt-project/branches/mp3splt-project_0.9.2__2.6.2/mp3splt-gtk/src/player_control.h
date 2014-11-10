/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2014 Alexandru Munteanu
 * Contact: m@ioalex.net
 *
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
 * Filename: player_control.h
 *
 * header of player_control.c, defines constants, etc..
 *********************************************************/

#ifndef PLAYER_CONTROL_H

#define PLAYER_CONTROL_H

#include "external_includes.h"

#include "snackamp_control.h"    
#include "audacious_control.h"   
#include "gstreamer_control.h"

#define PLAYER_AUDACIOUS 1
#define PLAYER_SNACKAMP 2
#define PLAYER_GSTREAMER 3

gint player_get_elapsed_time(ui_state *ui);
gint player_get_total_time(ui_state *ui);
gint player_is_running(ui_state *ui);
void player_start(ui_state *ui);
void player_start_add_files(GList *list, ui_state *ui);
void player_add_files(GList *list, ui_state *ui);
void player_add_files_and_select(GList *list, ui_state *ui);
void player_add_play_files(GList *list, ui_state *ui);
void player_start_play_with_songs(GList *list, ui_state *ui);
void player_play(ui_state *ui);
void player_stop(ui_state *ui);
void player_pause(ui_state *ui);
void player_next(ui_state *ui);
void player_prev(ui_state *ui);
void player_seek(gint position, ui_state *ui);
void player_get_song_infos(gchar *total_infos, ui_state *ui);
gint player_is_playing(ui_state *ui);
gint player_is_paused(ui_state *ui);
gchar *player_get_filename(ui_state *ui);
gchar *player_get_title(ui_state *ui);
gint player_get_volume(ui_state *ui);
void player_set_volume(gint volume, ui_state *ui);
gint player_get_playlist_number(ui_state *ui);
gint player_quit(ui_state *ui);

#endif

