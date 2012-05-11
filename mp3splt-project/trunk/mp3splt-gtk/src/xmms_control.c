/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2012 Alexandru Munteanu
 * Contact: io_fx@yahoo.fr
 *
 * from BMP to Audacious patch from Roberto Neri - 2007,2008
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

/*!********************************************************
 * \file 
 * xmms control
 *
 * this file contains the functions that control the xmms
 * player
 ********************************************************/

#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#ifndef NO_AUDACIOUS
#include <audacious/audctrl.h>
#include <audacious/dbus.h>
//ugly hack until fix
DBusGProxy *dbus_proxy = NULL;
static DBusGConnection *dbus_connection = NULL;
#endif

#include "player.h"

#ifndef NO_AUDACIOUS
//!Acquires informations about the song
void myxmms_get_song_infos(gchar *total_infos)
{
  //the frequency
  gint freq;
  //rate kb/s
  gint rate;
  //number of channels (mono/stereo)
  gint nch;
  
  gchar rate_str[32] = { '\0' };
  gchar freq_str[32] = { '\0' };
  gchar nch_str[32] = { '\0' };
  
  //infos about the song
  audacious_remote_get_info(dbus_proxy, &rate, &freq, &nch);
  
  g_snprintf(rate_str,32, "%d", rate/1000);
  g_snprintf(freq_str,32, "%d", freq/1000);
  
  if (nch == 2)
  {
    snprintf(nch_str, 32, "%s", _("stereo"));
  }
  else
  {
    snprintf(nch_str, 32, "%s", _("mono"));
  }

  gchar *_Kbps = _("Kbps");
  gchar *_Khz = _("Khz");

  if (rate != 0)
  {
    g_snprintf(total_infos,512,
               "%s %s     %s %s    %s", 
               rate_str,_Kbps,freq_str, _Khz,nch_str);
  }
  else 
  {
    total_infos[0] = '\0';
  }
}

/*!returns the filename

The filename is allocated by this function and must be g_free'ed after use.
*/
gchar *myxmms_get_filename()
{
  gchar *fname;
  
  //position of the song in the playlist
  gint playlist_position;
  
  playlist_position = audacious_remote_get_playlist_pos(dbus_proxy);
  
  fname = audacious_remote_get_playlist_file(dbus_proxy, playlist_position);

  //erase file:// and replace %20 with spaces
  gchar *fname2 = g_filename_from_uri(fname,NULL,NULL);
  g_free(fname);
  fname = NULL;

  return fname2;
}

//!returns the number of songs in the playlist
gint myxmms_get_playlist_number()
{
  return audacious_remote_get_playlist_length(dbus_proxy);
}

/*!returns the title of the song

The filename is allocated by this function and must be g_free'ed after use.
*/
gchar *myxmms_get_title_song()
{
  gchar *title;
  
  //position of the song in the playlist
  gint playlist_position;
  
  playlist_position = audacious_remote_get_playlist_pos(dbus_proxy);
  title = audacious_remote_get_playlist_title(dbus_proxy,playlist_position);
  
  return title;
}

//!returns elapsed time
gint myxmms_get_time_elapsed()
{
  return audacious_remote_get_output_time(dbus_proxy);
}

//!starts xmms
void myxmms_start()
{
  gint timer;
  time_t lt;
  
  static gchar *exec_command;
  exec_command = "audacious";
  gchar *exec_this = g_strdup_printf("%s &", exec_command);
  system(exec_this);
  
  timer = time(&lt);
  while (!audacious_remote_is_running(dbus_proxy) 
         && ((time(&lt) - timer) < 4))
    {
      usleep(0);
    }
  
  g_free(exec_this);
}

//!selects the last file in the playlist
void myxmms_select_last_file()
{
  gint number;
  number = audacious_remote_get_playlist_length(dbus_proxy);
  audacious_remote_set_playlist_pos(dbus_proxy,(number-1));
}

//!plays the last file of the playlist
void myxmms_play_last_file()
{
  myxmms_select_last_file();
  audacious_remote_play(dbus_proxy);
}

//!add files to the xmms playlist
void myxmms_add_files(GList *list)
{
  //change filenames into URLs
  GList *list_pos = list;

  //for each element of the list
  while (list_pos)
  {
    //duplicate the filename
    gchar *dup_filename = strdup(list_pos->data);
    //free the GList data content
    //g_free(list_pos->data);
    //put the new GList data content
    list_pos->data = g_filename_to_uri(dup_filename,NULL,NULL);
    //free the duplicated filename
    g_free(dup_filename);
    dup_filename = NULL;
    //move to the next element
    list_pos = g_list_next(list_pos);
  }

  audacious_remote_playlist_add(dbus_proxy, list); 
}

//!sets the volume level
void myxmms_set_volume(gint volume)
{
  audacious_remote_set_main_volume(dbus_proxy, volume);
}

//!returns volume level
gint myxmms_get_volume()
{
  return audacious_remote_get_main_volume(dbus_proxy);
}

/*!starts xmms with songs

  \param list The list of the songs to start xmms with
  \todo Which format is this list in?
 */
void myxmms_start_with_songs(GList *list)
{
  myxmms_start();
  myxmms_add_files(list);
}

//!returns TRUE if xmms is running; if not, FALSE 
gint myxmms_is_running()
{
  if (!dbus_connection)
  {
    dbus_connection = dbus_g_bus_get(DBUS_BUS_SESSION, NULL);
  }
  if (!dbus_proxy)
  {
    dbus_proxy = dbus_g_proxy_new_for_name(dbus_connection,
        AUDACIOUS_DBUS_SERVICE,
        AUDACIOUS_DBUS_PATH,
        AUDACIOUS_DBUS_INTERFACE);
  }
  if (!audacious_remote_is_running(dbus_proxy))
        return FALSE;
  else
        return TRUE;
}

//!returns TRUE if xmms is paused, if not, FALSE 
gint myxmms_is_paused()
{
  if (!audacious_remote_is_paused(dbus_proxy))
    return FALSE;
  else
    return TRUE;
}

//!Start playing the current song
void myxmms_play()
{
  audacious_remote_play(dbus_proxy);
}

//!Stop playing the current song
void myxmms_stop()
{
  audacious_remote_stop(dbus_proxy);
}

//!Pause playing the current song
void myxmms_pause()
{
  audacious_remote_pause(dbus_proxy);
}

//!Switch to the next song
void myxmms_next()
{
  audacious_remote_playlist_next(dbus_proxy);
}

//!Switch to the previous song
void myxmms_prev()
{
  audacious_remote_playlist_prev(dbus_proxy);
}

//!jump to time
void myxmms_jump(gint position)
{
  audacious_remote_jump_to_time(dbus_proxy, position);
}

//!returns the total duration of the current song
gint myxmms_get_total_time()
{
  gint playlist_position;
  playlist_position = audacious_remote_get_playlist_pos(dbus_proxy);
  return audacious_remote_get_playlist_time(dbus_proxy,playlist_position);
}

//!returns TRUE if xmms is playing, else FALSE
gint myxmms_is_playing()
{
  if(audacious_remote_is_playing(dbus_proxy))
    return TRUE;
  else
    return FALSE;
}

//!quits the player
void myxmms_quit()
{
  audacious_remote_quit(dbus_proxy);
}

#endif

