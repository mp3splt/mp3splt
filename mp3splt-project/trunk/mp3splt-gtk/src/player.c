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
 * This program is free software; you ca nredistribute it and/or
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

/*!********************************************************
 * \file
 * Functions to access the currently selected player
 *
 * this file is used to play for the appropriate player, 
 * for example if we choose snackamp, the player will use 
 * snackamp
 **********************************************************/

#include "player.h"

//!returns the elapsed time of the player
gint player_get_elapsed_time(ui_state *ui)
{
  if (ui->infos->selected_player == PLAYER_SNACKAMP)
  {
    return snackamp_get_time_elapsed();
  }
  else if (ui->infos->selected_player == PLAYER_AUDACIOUS)
  {
#ifndef __WIN32__
#ifndef NO_AUDACIOUS
    return myxmms_get_time_elapsed();
#endif
#endif
  }
  else
  {
#ifndef NO_GSTREAMER
    return gstreamer_get_time_elapsed(ui);
#endif
  }

  return 0;
}

//!returns total time of the song
gint player_get_total_time(ui_state *ui)
{
  if (ui->infos->selected_player == PLAYER_SNACKAMP)
  {
    return snackamp_get_total_time();
  }
  else if (ui->infos->selected_player == PLAYER_AUDACIOUS)
  {
#ifndef __WIN32__
#ifndef NO_AUDACIOUS
    return myxmms_get_total_time();
#endif
#endif
  }
  else
  {
#ifndef NO_GSTREAMER
    return gstreamer_get_total_time(ui);
#endif
  }

  return 0;
}

//!returns FALSE if the player is not running, else TRUE
gint player_is_running(ui_state *ui)
{
  if (ui->infos->selected_player == PLAYER_SNACKAMP)
  {
    return snackamp_is_running();
  }
  else if (ui->infos->selected_player == PLAYER_AUDACIOUS)
  {
#ifndef __WIN32__
#ifndef NO_AUDACIOUS
    return myxmms_is_running();
#endif
#endif
  }
  else
  {
#ifndef NO_GSTREAMER
    return gstreamer_is_running(ui);
#endif
  }

  return 0;
}

//!starts the player
void player_start(ui_state *ui)
{
  if (ui->infos->selected_player == PLAYER_SNACKAMP)
  {
    snackamp_start();
  }
  else if (ui->infos->selected_player == PLAYER_AUDACIOUS)
  {
#ifndef __WIN32__
#ifndef NO_AUDACIOUS
    myxmms_start();
#endif
#endif
  }
  else
  {
#ifndef NO_GSTREAMER
    gstreamer_start(ui);
#endif
  }
}

//!start player and add files to playlist
void player_start_add_files(GList *list, ui_state *ui)
{
  if (ui->infos->selected_player == PLAYER_SNACKAMP)
  {
    snackamp_start_with_songs(list);
  }
  else if (ui->infos->selected_player == PLAYER_AUDACIOUS)
  {
#ifndef __WIN32__
#ifndef NO_AUDACIOUS
    myxmms_start_with_songs(list);
#endif
#endif
  }
  else
  {
#ifndef NO_GSTREAMER
    gstreamer_start_with_songs(list, ui);
#endif 
  }
}

//!add files to playlist
void player_add_files(GList *list, ui_state *ui)
{
  if (ui->infos->selected_player == PLAYER_SNACKAMP)
  {
    snackamp_add_files(list);
  }
  else if (ui->infos->selected_player == PLAYER_AUDACIOUS)
  {
#ifndef __WIN32__
#ifndef NO_AUDACIOUS
    myxmms_add_files(list);
#endif
#endif
  }
  else
  {
#ifndef NO_GSTREAMER
    gstreamer_add_files(list, ui);
#endif
  }
}

//!add files to playlist
void player_add_files_and_select(GList *list, ui_state *ui)
{
  if (ui->infos->selected_player == PLAYER_SNACKAMP)
  {
    snackamp_add_files(list);
    snackamp_select_last_file();
  }
  else if (ui->infos->selected_player == PLAYER_AUDACIOUS)
  {
#ifndef __WIN32__
#ifndef NO_AUDACIOUS
    myxmms_add_files(list);
    myxmms_select_last_file();
#endif
#endif
  }
  else
  {
#ifndef NO_GSTREAMER
    gstreamer_add_files(list, ui);
    gstreamer_select_last_file(ui);
#endif
  }
}

//!add files to playlist
void player_add_play_files(GList *list, ui_state *ui)
{
  player_add_files(list, ui);

  if (ui->infos->selected_player == PLAYER_SNACKAMP)
  {
    snackamp_next();
  }
  else if (ui->infos->selected_player == PLAYER_AUDACIOUS)
  {
#ifndef __WIN32__
#ifndef NO_AUDACIOUS
    myxmms_play_last_file();
#endif
#endif
  }
  else
  {
#ifndef NO_GSTREAMER
    gstreamer_play_last_file(ui);
#endif
  }
}

//!starts the player
void player_start_play_with_songs(GList *list, ui_state *ui)
{
  if (ui->infos->selected_player == PLAYER_SNACKAMP)
  {
    snackamp_start_with_songs(list);
    snackamp_play_last_file();
  }
  else if (ui->infos->selected_player == PLAYER_AUDACIOUS)
  {
#ifndef __WIN32__
#ifndef NO_AUDACIOUS
    myxmms_start_with_songs(list);
    myxmms_play_last_file();
#endif
#endif
  }
  else
  {
#ifndef NO_GSTREAMER
    gstreamer_start_with_songs(list, ui);
    gstreamer_play_last_file(ui);
#endif
  }
}

//!plays the song
void player_play(ui_state *ui)
{
  if (ui->infos->selected_player == PLAYER_SNACKAMP)
  {
    snackamp_play();
  }
  else if (ui->infos->selected_player == PLAYER_AUDACIOUS)
  {
#ifndef __WIN32__
#ifndef NO_AUDACIOUS
    myxmms_play();
#endif
#endif
  }
  else
  {
#ifndef NO_GSTREAMER
    gstreamer_play(ui);
#endif
  }
}

//!stops the song
void player_stop(ui_state *ui)
{
  if (ui->infos->selected_player == PLAYER_SNACKAMP)
  {
    snackamp_stop();
  }
  else if (ui->infos->selected_player == PLAYER_AUDACIOUS)
  {
#ifndef __WIN32__
#ifndef NO_AUDACIOUS
    myxmms_stop();
#endif
#endif
  }
  else
  {
#ifndef NO_GSTREAMER
    gstreamer_stop(ui);
#endif
  }
}

//!pause the song
void player_pause(ui_state *ui)
{
  if (ui->infos->selected_player == PLAYER_SNACKAMP)
  {
    snackamp_pause();
  }
  else if (ui->infos->selected_player == PLAYER_AUDACIOUS)
  {
#ifndef __WIN32__
#ifndef NO_AUDACIOUS
    myxmms_pause();
#endif
#endif
  }
  else
  {
#ifndef NO_GSTREAMER
    gstreamer_pause(ui);
#endif
  }
}

//!pass to the next song
void player_next(ui_state *ui)
{
  if (ui->infos->selected_player == PLAYER_SNACKAMP)
  {
    snackamp_next();
  }
  else if (ui->infos->selected_player == PLAYER_AUDACIOUS)
  {
#ifndef __WIN32__
#ifndef NO_AUDACIOUS
    myxmms_next();
#endif
#endif
  }
  else
  {
#ifndef NO_GSTREAMER
    gstreamer_next(ui);
#endif
  }
}

//!pass to the previous song
void player_prev(ui_state *ui)
{
  if (ui->infos->selected_player == PLAYER_SNACKAMP)
  {
    snackamp_prev();
  }
  else if (ui->infos->selected_player == PLAYER_AUDACIOUS)
  {
#ifndef __WIN32__
#ifndef NO_AUDACIOUS
    myxmms_prev();
#endif
#endif
  }
  else
  {
#ifndef NO_GSTREAMER
    gstreamer_prev(ui);
#endif
  }
}

//!jumps to a position in the song
void player_seek(gint position, ui_state *ui)
{
  if (ui->infos->selected_player == PLAYER_SNACKAMP)
  {
    snackamp_jump(position);
  }
  else if (ui->infos->selected_player == PLAYER_AUDACIOUS)
  {
#ifndef __WIN32__
#ifndef NO_AUDACIOUS
    myxmms_jump(position);
#endif
#endif
  }
  else
  {
#ifndef NO_GSTREAMER
    gstreamer_jump(position, ui);
#endif
  }
}

/*!get infos about the song

\param total_infos The result of this function call
*/
void player_get_song_infos(gchar *total_infos, ui_state *ui)
{
  if (ui->infos->selected_player == PLAYER_SNACKAMP)
  {
    snackamp_get_song_infos(total_infos);
  }
  else if (ui->infos->selected_player == PLAYER_AUDACIOUS)
  {
#ifndef __WIN32__
#ifndef NO_AUDACIOUS
    myxmms_get_song_infos(total_infos);
#endif
#endif
  }
  else
  {
#ifndef NO_GSTREAMER
    gstreamer_get_song_infos(total_infos, ui);
#endif
  }
}

//!returns TRUE if the player is playing, else FALSE
gint player_is_playing(ui_state *ui)
{
  if (ui->infos->selected_player == PLAYER_SNACKAMP)
  {
    return snackamp_is_playing();
  }
  else if (ui->infos->selected_player == PLAYER_AUDACIOUS)
  {
#ifndef __WIN32__
#ifndef NO_AUDACIOUS
    return myxmms_is_playing();
#endif
#endif
  }
  else
  {
#ifndef NO_GSTREAMER
    return gstreamer_is_playing(ui);
#endif
  }

  return FALSE;
}

//! Check if the player is paused
gint player_is_paused(ui_state *ui)
{
  if (ui->infos->selected_player == PLAYER_SNACKAMP)
  {
    return snackamp_is_paused();
  }
  else if (ui->infos->selected_player == PLAYER_AUDACIOUS)
  {
#ifndef __WIN32__
#ifndef NO_AUDACIOUS
    return myxmms_is_paused();
#endif
#endif
  }
  else
  {
#ifndef NO_GSTREAMER
    return gstreamer_is_paused(ui);
#endif
  }

  return 0;
}

/*!gets the filename of the current song

The returned string must be g_free'd after use
*/
gchar *player_get_filename(ui_state *ui)
{
  if (ui->infos->selected_player == PLAYER_SNACKAMP)
  {
    return snackamp_get_filename();
  }
  else if (ui->infos->selected_player == PLAYER_AUDACIOUS)
  {
#ifndef __WIN32__
#ifndef NO_AUDACIOUS
    return myxmms_get_filename();
#endif
#endif
  }
  else
  {
#ifndef NO_GSTREAMER
    return gstreamer_get_filename(ui);
#endif
  }

  return 0;
}

/*!Get the title of the song

The returned string must be g_freed after use
*/
gchar *player_get_title(ui_state *ui)
{
  if (ui->infos->selected_player == PLAYER_SNACKAMP)
  {
    return snackamp_get_title_song();
  }
  else if (ui->infos->selected_player == PLAYER_AUDACIOUS)
  {
#ifndef __WIN32__
#ifndef NO_AUDACIOUS
    return myxmms_get_title_song();
#endif
#endif
  }
  else
  {
#ifndef NO_GSTREAMER
    return gstreamer_get_title_song(ui);
#endif
  }

  return 0;
}

//!gets the volume of the player
gint player_get_volume(ui_state *ui)
{
  if (ui->infos->selected_player == PLAYER_SNACKAMP)
  {
    return snackamp_get_volume();
  }
  else if (ui->infos->selected_player == PLAYER_AUDACIOUS)
  {
#ifndef __WIN32__
#ifndef NO_AUDACIOUS
    return myxmms_get_volume();
#endif
#endif
  }
  else
  {
#ifndef NO_GSTREAMER
    return gstreamer_get_volume(ui);
#endif
  }

  return 0;
}

//!sets the volume of the player
void player_set_volume(gint volume, ui_state *ui)
{
  if (ui->infos->selected_player == PLAYER_SNACKAMP)
  {
    snackamp_set_volume(volume);
  }
  else if (ui->infos->selected_player == PLAYER_AUDACIOUS)
  {
#ifndef __WIN32__
#ifndef NO_AUDACIOUS
    myxmms_set_volume(volume);
#endif
#endif
  }
  else
  {
#ifndef NO_GSTREAMER
    gstreamer_set_volume(volume, ui);
#endif
  }
}

//!returns the number of songs in the playlist
gint player_get_playlist_number(ui_state *ui)
{
  if (ui->infos->selected_player == PLAYER_SNACKAMP)
  {
    return snackamp_get_playlist_number();
  }
  else if (ui->infos->selected_player == PLAYER_AUDACIOUS)
  {
#ifndef __WIN32__
#ifndef NO_AUDACIOUS
    return myxmms_get_playlist_number();
#endif
#endif
  }
  else
  {
#ifndef NO_GSTREAMER
    return gstreamer_get_playlist_number(ui);
#endif
  }

  return 0;
}

//!quits the player
gint player_quit(ui_state *ui)
{
  if (ui->infos->selected_player == PLAYER_GSTREAMER)
  {
#ifndef NO_GSTREAMER
    gstreamer_quit(ui);
#endif
  }

  return 0;
}

