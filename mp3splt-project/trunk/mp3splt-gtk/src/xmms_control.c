/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2006 Munteanu Alexandru
 * Contact: io_alex_2002@yahoo.fr
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
 * Filename: xmms_control.c
 *
 * this file has functions to control the xmms player
 *
 *********************************************************/

#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <time.h>
#include <unistd.h>

#ifndef NO_BMP
#include <bmp/beepctrl.h>
#endif

#include "player.h"

extern int selected_player;

#ifndef NO_BMP
//Gets informations about the song
void myxmms_get_song_infos(gchar *total_infos)
{
  //the frequency
  gint freq;
  //rate kb/s
  gint rate;
  //number of channels( mono/stereo
  gint nch;
  
  gchar rate_str[32];
  gchar freq_str[32];
  gchar *nch_str;
  
  //infos about the song
  xmms_remote_get_info(0, &rate, &freq, &nch);
  
  g_snprintf(rate_str,32,
             "%d", rate/1000);
  g_snprintf(freq_str,32,
             "%d", freq/1000);
  
  if (nch == 2)
    nch_str = (gchar *)_("stereo");
  else
    nch_str = (gchar *)_("mono");

  gchar *_Kbps = (gchar *)_("Kbps");
  gchar *_Khz = (gchar *)_("Khz");

  if (rate != 0)
    g_snprintf(total_infos,512,
               "%s %s     %s %s    %s", 
               rate_str,_Kbps,freq_str, 
               _Khz,nch_str);
  else 
    total_infos[0] = '\0';
}

//returns the filename
//must be freed after
gchar *myxmms_get_filename()
{
  gchar *fname;
  
  //position of the song in the playlist
  gint playlist_position;
  
  playlist_position = xmms_remote_get_playlist_pos(0);
  
  fname = xmms_remote_get_playlist_file(0, playlist_position);
  
  return fname;
}

//returns the number of songs of the playlist
gint myxmms_get_playlist_number()
{
  return xmms_remote_get_playlist_length(0);
}

//returns the title of the song
//must be freed after
gchar *myxmms_get_title_song()
{
  gchar *title;
  
  //position of the song in the playlist
  gint playlist_position;
  
  playlist_position = xmms_remote_get_playlist_pos(0);
  title = xmms_remote_get_playlist_title(0,playlist_position);
  
  return title;
}

//returns elapsed time
gint myxmms_get_time_elapsed()
{
  
  return xmms_remote_get_output_time(0);
}

//starts xmms
void myxmms_start()
{
  gint timer;
  time_t lt;
  
  static gchar *exec_command;
  exec_command = "beep-media-player";
  gchar *exec_this = g_strdup_printf("%s &", exec_command);
  system(exec_this);
  
  timer = time(&lt);
  while (!xmms_remote_is_running(0) 
         && ((time(&lt) - timer) < 4))
    {
      usleep(0);
    }
  
  g_free(exec_this);
}

//selects the last file in the playlist
void myxmms_select_last_file()
{
  gint number;
  number = xmms_remote_get_playlist_length(0);
  xmms_remote_set_playlist_pos(0,(number-1));
}

//plays the last file of the playlist
void myxmms_play_last_file()
{
  myxmms_select_last_file();
  xmms_remote_play(0);
}

//add files to the xmms playlist
void myxmms_add_files(GList *list)
{
  xmms_remote_playlist_add(0, list); 
}

//sets volume
void myxmms_set_volume(gint volume)
{
  xmms_remote_set_main_volume(0, volume);
}

//returns volume
gint myxmms_get_volume()
{
  return xmms_remote_get_main_volume(0);
}

//starts xmms with songs
void myxmms_start_with_songs(GList *list)
{
  myxmms_start();
  myxmms_add_files(list);
}

//returns TRUE if xmms is running; if not, FALSE 
gint myxmms_is_running()
{
  if (!xmms_remote_is_running(0))
        return FALSE;
  else
        return TRUE;
}

//returns TRUE if xmms is paused, if not, FALSE 
gint myxmms_is_paused()
{
  if (!xmms_remote_is_paused(0))
    return FALSE;
  else
    return TRUE;
}

//plays a song
void myxmms_play()
{
  xmms_remote_play(0);
}

//stops a song
void myxmms_stop()
{
  xmms_remote_stop(0);
}

//pause a song
void myxmms_pause()
{
  xmms_remote_pause(0);
}

//changes to next song
void myxmms_next()
{
  xmms_remote_playlist_next(0);
}

//changes to previous song
void myxmms_prev()
{
  xmms_remote_playlist_prev(0);
}

//jump to time
void myxmms_jump(gint position)
{
  xmms_remote_jump_to_time(0, position);
}

//returns total time of the current song
gint myxmms_get_total_time()
{
  gint playlist_position;
  playlist_position = xmms_remote_get_playlist_pos(0);
  return xmms_remote_get_playlist_time(0,playlist_position);
}

//returns TRUE if xmms is playing, else FALSE
gint myxmms_is_playing()
{
  if(xmms_remote_is_playing(0))
    return TRUE;
  else
    return FALSE;
}

//quits player
void myxmms_quit()
{
  xmms_remote_quit(0);
}

#endif

