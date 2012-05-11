/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2005-2012 Alexandru Munteanu - io_fx@yahoo.fr
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
 * Filename: xmms_control.h
 *
 * header to xmms_control.c
 *
 * xmms_control.c controls the xmms player
 *
 *********************************************************/

void gstreamer_get_song_infos(gchar *total_infos);
gchar *gstreamer_get_filename();
gint gstreamer_get_playlist_number();
gchar *gstreamer_get_title_song();
gint gstreamer_get_time_elapsed();
void gstreamer_start();
void gstreamer_select_last_file();
void gstreamer_play_last_file();
void gstreamer_add_files(GList *list);
void gstreamer_set_volume(gint volume);
gint gstreamer_get_volume();
void gstreamer_start_with_songs(GList *list);
gint gstreamer_is_running();
gint gstreamer_is_paused();
void gstreamer_play();
void gstreamer_stop();
void gstreamer_pause();
void gstreamer_next();
void gstreamer_prev();
void gstreamer_jump(gint position);
gint gstreamer_get_total_time();
gint gstreamer_is_playing();
void gstreamer_quit();

