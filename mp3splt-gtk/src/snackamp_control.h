/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2005-2011 Alexandru Munteanu - io_fx@yahoo.fr
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
 * Filename: snackamp_control.h
 *
 * header of snackamp_control.c
 *
 *********************************************************/

gint connect_snackamp(gint port);
gchar *cut_begin_end(gchar *result);
gint disconnect_snackamp();
gchar *snackamp_socket_send_message(gchar *message);
gint get_integer_from_string(gchar *result);
gboolean snackamp_is_connected();
void snackamp_get_song_infos(gchar *total_infos);
gchar *snackamp_get_filename();
gint snackamp_get_playlist_pos();
void snackamp_stop();
gint snackamp_get_playlist_number();
gchar *snackamp_get_title_song();
gint snackamp_get_time_elapsed();
void snackamp_start();
void snackamp_set_playlist_pos(gint pos);
void snackamp_select_last_file();
void snackamp_play();
void snackamp_play_last_file();
void snackamp_add_files(GList *list);
void snackamp_set_volume(gint volume);
gint snackamp_get_volume();
void snackamp_start_with_songs(GList *list);
gint snackamp_is_running();
void snackamp_pause();
void snackamp_next();
void snackamp_prev();
void snackamp_jump(gint position);
gint snackamp_get_total_time();
gint snackamp_is_playing();
gint snackamp_is_paused();
void snackamp_quit();


