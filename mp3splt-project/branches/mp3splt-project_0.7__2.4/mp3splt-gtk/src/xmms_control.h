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
 * Filename: xmms_control.h
 *
 * header to xmms_control.c
 *
 * xmms_control.c controls the xmms player
 *
 *********************************************************/

void myxmms_get_song_infos(gchar *total_infos);
gchar *myxmms_get_filename();
gint myxmms_get_playlist_number();
gchar *myxmms_get_title_song();
gint myxmms_get_time_elapsed();
void myxmms_start();
void myxmms_select_last_file();
void myxmms_play_last_file();
void myxmms_add_files(GList *list);
void myxmms_set_volume(gint volume);
gint myxmms_get_volume();
void myxmms_start_with_songs(GList *list);
gint myxmms_is_running();
gint myxmms_is_paused();
void myxmms_play();
void myxmms_stop();
void myxmms_pause();
void myxmms_next();
void myxmms_prev();
void myxmms_jump(gint position);
gint myxmms_get_total_time();
gint myxmms_is_playing();
void myxmms_quit();



