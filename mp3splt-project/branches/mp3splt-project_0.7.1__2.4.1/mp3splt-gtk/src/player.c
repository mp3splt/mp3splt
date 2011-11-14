/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2011 Alexandru Munteanu
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

#include <glib.h>
#include <stdio.h>

#include "player.h"
#include "snackamp_control.h"
#include "xmms_control.h"
#include "gstreamer_control.h"

extern int selected_player;

//!returns the elapsed time of the player
gint player_get_elapsed_time()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      return snackamp_get_time_elapsed();
    }
  else if (selected_player == PLAYER_AUDACIOUS)
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
      return gstreamer_get_time_elapsed();
#endif
    }

  return 0;
}

//!returns total time of the song
gint player_get_total_time()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      return snackamp_get_total_time();
    }
  else if (selected_player == PLAYER_AUDACIOUS)
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
      return gstreamer_get_total_time();
#endif
    }

  return 0;
}

//!returns FALSE if the player is not running, else TRUE
gint player_is_running()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      return snackamp_is_running();
    }
  else if (selected_player == PLAYER_AUDACIOUS)
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
      return gstreamer_is_running();
#endif
    }

  return 0;
}

//!starts the player
void player_start()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      snackamp_start();
    }
  else if (selected_player == PLAYER_AUDACIOUS)
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
      gstreamer_start();
#endif
    }
}

//!start player and add files to playlist
void player_start_add_files(GList *list)
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      snackamp_start_with_songs(list);
    }
  else if (selected_player == PLAYER_AUDACIOUS)
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
      gstreamer_start_with_songs(list);
#endif 
    }
}

//!add files to playlist
void player_add_files(GList *list)
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      snackamp_add_files(list);
    }
  else if (selected_player == PLAYER_AUDACIOUS)
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
      gstreamer_add_files(list);
#endif
    }
}

//!add files to playlist
void player_add_files_and_select(GList *list)
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      snackamp_add_files(list);
      snackamp_select_last_file();
    }
  else if (selected_player == PLAYER_AUDACIOUS)
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
      gstreamer_add_files(list);
      gstreamer_select_last_file();
#endif
    }
}

//!add files to playlist
void player_add_play_files(GList *list)
{
  player_add_files(list);

  if (selected_player == PLAYER_SNACKAMP)
    {
      //snackamp adds files just after the current one
      //and not at the end of the playlist
      snackamp_next();
      //snackamp_play_last_file();
    }
  else if (selected_player == PLAYER_AUDACIOUS)
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
      gstreamer_play_last_file();
#endif
    }
}

//!starts the player
void player_start_play_with_songs(GList *list)
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      snackamp_start_with_songs(list);
      snackamp_play_last_file();
    }
  else if (selected_player == PLAYER_AUDACIOUS)
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
      gstreamer_start_with_songs(list);
      gstreamer_play_last_file();
#endif
    }
}

//!plays the song
void player_play()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      snackamp_play();
    }
  else if (selected_player == PLAYER_AUDACIOUS)
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
      gstreamer_play();
#endif
    }
}

//!stops the song
void player_stop()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      snackamp_stop();
    }
  else if (selected_player == PLAYER_AUDACIOUS)
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
      gstreamer_stop();
#endif
    }
}

//!pause the song
void player_pause()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      snackamp_pause();
    }
  else if (selected_player == PLAYER_AUDACIOUS)
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
      gstreamer_pause();
#endif
    }
}

//!pass to the next song
void player_next()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      snackamp_next();
    }
  else if (selected_player == PLAYER_AUDACIOUS)
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
      gstreamer_next();
#endif
    }
}

//!pass to the previous song
void player_prev()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      snackamp_prev();
    }
  else if (selected_player == PLAYER_AUDACIOUS)
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
      gstreamer_prev();
#endif
    }
}

//!jumps to a position in the song
void player_jump(gint position)
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      snackamp_jump(position);
    }
  else if (selected_player == PLAYER_AUDACIOUS)
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
      gstreamer_jump(position);
#endif
    }
}

/*!get infos about the song

\param total_infos The result of this function call
*/
void player_get_song_infos(gchar *total_infos)
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      snackamp_get_song_infos(total_infos);
    }
  else if (selected_player == PLAYER_AUDACIOUS)
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
      gstreamer_get_song_infos(total_infos);
#endif
    }
}

#include <stdio.h>
//!returns TRUE if the player is playing, else FALSE
gint player_is_playing()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      return snackamp_is_playing();
    }
  else if (selected_player == PLAYER_AUDACIOUS)
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
      return gstreamer_is_playing();
#endif
    }

  return 0;
}

//! Check if the player is paused
gint player_is_paused()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      return snackamp_is_paused();
    }
  else if (selected_player == PLAYER_AUDACIOUS)
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
      return gstreamer_is_paused();
#endif
    }

  return 0;
}

/*!gets the filename of the current song

The returned string must be g_free'd after use
*/
gchar *player_get_filename()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      return snackamp_get_filename();
    }
  else if (selected_player == PLAYER_AUDACIOUS)
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
      return gstreamer_get_filename();
#endif
    }

  return 0;
}

/*!Get the title of the song

The returned string must be g_freed after use
*/
gchar *player_get_title()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      return snackamp_get_title_song();
    }
  else if (selected_player == PLAYER_AUDACIOUS)
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
      return gstreamer_get_title_song();
#endif
    }

  return 0;
}

//!gets the volume of the player
gint player_get_volume()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      return snackamp_get_volume();
    }
  else if (selected_player == PLAYER_AUDACIOUS)
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
      return gstreamer_get_volume();
#endif
    }

  return 0;
}

//!sets the volume of the player
void player_set_volume(gint volume)
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      snackamp_set_volume(volume);
    }
  else if (selected_player == PLAYER_AUDACIOUS)
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
      gstreamer_set_volume(volume);
#endif
    }
}

//!returns the number of songs in the playlist
gint player_get_playlist_number()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      return snackamp_get_playlist_number();
    }
  else if (selected_player == PLAYER_AUDACIOUS)
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
      return gstreamer_get_playlist_number();
#endif
    }

  return 0;
}

//!quits the player
gint player_quit()
{
  /*if (selected_player == PLAYER_SNACKAMP)
    {
      return snackamp_quit();
    }
  else
    {
#ifndef __WIN32__
#ifndef NO_AUDACIOUS
      return myxmms_quit();
#endif
#endif
}*/
  if (selected_player == PLAYER_GSTREAMER)
  {
#ifndef NO_GSTREAMER
    gstreamer_quit();
#endif
  }

  return 0;
}
