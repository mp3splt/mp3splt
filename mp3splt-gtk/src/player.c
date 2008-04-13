/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2006 Munteanu Alexandru
 * Contact: io_alex_2002@yahoo.fr
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

/**********************************************************
 * Filename: player.c
 *
 * this file is used to play for the appropriate player, 
 * for example if we choose snackamp, the player will use snackamp
 *
 *********************************************************/

#include <glib.h>
#include <stdio.h>

#include "player.h"
#include "snackamp_control.h"
#include "xmms_control.h"

extern int selected_player;

//returns the elapsed time of the player
gint player_get_elapsed_time()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      return snackamp_get_time_elapsed();
    }
  else
    {
#ifndef __WIN32__
#ifndef NO_BMP
      return myxmms_get_time_elapsed();
#endif
#endif
    }
  return 0;
}

//returns total time of the song
gint player_get_total_time()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      return snackamp_get_total_time();
    }
  else
    {
#ifndef __WIN32__
#ifndef NO_BMP
      return myxmms_get_total_time();
#endif
#endif
    }
  return 0;
}

//returns FALSE if the player is not running, else TRUE
gint player_is_running()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      return snackamp_is_running();
    }
  else
    {
#ifndef __WIN32__
#ifndef NO_BMP
      return myxmms_is_running();
#endif
#endif
    }
  return 0;
}

//starts the player
void player_start()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      snackamp_start();
    }
  else
    {
#ifndef __WIN32__
#ifndef NO_BMP
      myxmms_start();
#endif
#endif
    }
}

//start player and add files to playlist
void player_start_add_files(GList *list)
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      snackamp_start_with_songs(list);
    }
  else
    {
#ifndef __WIN32__
#ifndef NO_BMP
      myxmms_start_with_songs(list);
#endif
#endif
    }
}

//add files to playlist
void player_add_files(GList *list)
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      snackamp_add_files(list);
    }
  else
    {
#ifndef __WIN32__
#ifndef NO_BMP
      myxmms_add_files(list);
#endif
#endif
    }
}

//add files to playlist
void player_add_files_and_select(GList *list)
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      snackamp_add_files(list);
      snackamp_select_last_file();
    }
  else
    {
#ifndef __WIN32__
#ifndef NO_BMP
      myxmms_add_files(list);
      myxmms_select_last_file();
#endif
#endif
    }
}

//add files to playlist
void player_add_play_files(GList *list)
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      //snackamp adds files just after the current one
      //and not at the end of the playlist
      player_add_files(list);
      snackamp_next();
      //snackamp_play_last_file();
    }
  else
    {
#ifndef __WIN32__
#ifndef NO_BMP
      player_add_files(list);
      myxmms_play_last_file();
#endif
#endif
    }
}

//starts the player
void player_start_play_with_songs(GList *list)
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      snackamp_start_with_songs(list);
      snackamp_play_last_file();
    }
  else
    {
#ifndef __WIN32__
#ifndef NO_BMP
      myxmms_start_with_songs(list);
      myxmms_play_last_file();
#endif
#endif
    }
}

//plays the song
void player_play()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      snackamp_play();
    }
  else
    {
#ifndef __WIN32__
#ifndef NO_BMP
      myxmms_play();
#endif
#endif
    }
}

//stops the song
void player_stop()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      snackamp_stop();
    }
  else
    {
#ifndef __WIN32__
#ifndef NO_BMP
      myxmms_stop();
#endif
#endif
    }
}

//pause the song
void player_pause()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      snackamp_pause();
    }
  else
    {
#ifndef __WIN32__
#ifndef NO_BMP
      myxmms_pause();
#endif
#endif
    }
}

//pass to the next song
void player_next()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      snackamp_next();
    }
  else
    {
#ifndef __WIN32__
#ifndef NO_BMP
      myxmms_next();
#endif
#endif
    }
}

//pass to the previous song
void player_prev()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      snackamp_prev();
    }
  else
    {
#ifndef __WIN32__
#ifndef NO_BMP
      myxmms_prev();
#endif
#endif
    }
}

//jumps to a position in the song
void player_jump(gint position)
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      snackamp_jump(position);
    }
  else
    {
#ifndef __WIN32__
#ifndef NO_BMP
      myxmms_jump(position);
#endif
#endif
    }
}

//get infos about the song
//in the total_infos variable
void player_get_song_infos(gchar *total_infos)
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      snackamp_get_song_infos(total_infos);
    }
  else
    {
#ifndef __WIN32__
#ifndef NO_BMP
      myxmms_get_song_infos(total_infos);
#endif
#endif
    }
}

#include <stdio.h>
//returns TRUE if the player is playing, else FALSE
gint player_is_playing()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      return snackamp_is_playing();
    }
  else
    {
#ifndef __WIN32__
#ifndef NO_BMP
      return myxmms_is_playing();
#endif
#endif
    }
  return 0;
}

//if the player is paused
gint player_is_paused()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      return snackamp_is_paused();
    }
  else
    {
#ifndef __WIN32__
#ifndef NO_BMP
      return myxmms_is_paused();
#endif
#endif
    }
  return 0;
}

//gets the filename of the current song
//must be freed after
gchar *player_get_filename()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      return (gchar *)snackamp_get_filename();
    }
  else
    {
#ifndef __WIN32__
#ifndef NO_BMP
      return (gchar *)myxmms_get_filename();
#endif
#endif
    }
  return 0;
}

//gets the title of the song
//must be freed after
gchar *player_get_title()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      return (gchar *)snackamp_get_title_song();
    }
  else
    {
#ifndef __WIN32__
#ifndef NO_BMP
      return (gchar *)myxmms_get_title_song();
#endif
#endif
    }
  return 0;
}

//gets the volume of the player
gint player_get_volume()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      return snackamp_get_volume();
    }
  else
    {
#ifndef __WIN32__
#ifndef NO_BMP
      return myxmms_get_volume();
#endif
#endif
    }
  return 0;
}

//sets the volume of the player
void player_set_volume(gint volume)
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      snackamp_set_volume(volume);
    }
  else
    {
#ifndef __WIN32__
#ifndef NO_BMP
      myxmms_set_volume(volume);
#endif
#endif
    }
}

//returns the number of songs in the playlist
gint player_get_playlist_number()
{
  if (selected_player == PLAYER_SNACKAMP)
    {
      return snackamp_get_playlist_number();
    }
  else
    {
#ifndef __WIN32__
#ifndef NO_BMP
      return myxmms_get_playlist_number();
#endif
#endif
    }
  return 0;
}

//quits the player
gint player_quit()
{
  /*if (selected_player == PLAYER_SNACKAMP)
    {
      return snackamp_quit();
    }
  else
    {
#ifndef __WIN32__
#ifndef NO_BMP
      return myxmms_quit();
#endif
#endif
}*/
  return 0;
}
