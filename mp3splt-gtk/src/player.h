/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2012 Alexandru Munteanu
 * Contact: io_fx@yahoo.fr
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 *
 *********************************************************/

/**********************************************************
 * Filename: player.h
 *
 * header of player.c, defines constants, etc..
 *
 *********************************************************/

#define PLAYER_AUDACIOUS 1
#define PLAYER_SNACKAMP 2
#define PLAYER_GSTREAMER 3

gint player_get_elapsed_time();
gint player_get_total_time();
gint player_is_running();
void player_start();
void player_start_add_files(GList *list);
void player_add_files(GList *list);
void player_add_files_and_select(GList *list);
void player_add_play_files(GList *list);
void player_start_play_with_songs(GList *list);
void player_play();
void player_stop();
void player_pause();
void player_next();
void player_prev();
void player_jump(gint position);
void player_get_song_infos(gchar *total_infos);
gint player_is_playing();
gint player_is_paused();
gchar *player_get_filename();
gchar *player_get_title();
gint player_get_volume();
void player_set_volume(gint volume);
gint player_get_playlist_number();
gint player_quit();
