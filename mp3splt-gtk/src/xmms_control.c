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

#include "xmms_control.h"

#ifndef NO_AUDACIOUS

#include <audacious/audctrl.h>
#include <audacious/dbus.h>

//!Acquires informations about the song
void myxmms_get_song_infos(gchar *total_infos, ui_state *ui)
{
  gint freq, rate, nch;
  audacious_remote_get_info(ui->pi->dbus_proxy, &rate, &freq, &nch);
  
  gchar rate_str[32] = { '\0' };
  gchar freq_str[32] = { '\0' };
  gchar nch_str[32] = { '\0' };

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
    g_snprintf(total_infos,512, "%s %s     %s %s    %s", rate_str,_Kbps,freq_str, _Khz,nch_str);
    return;
  }

  total_infos[0] = '\0';
}

/*!returns the filename

The filename is allocated by this function and must be g_free'ed after use.
*/
gchar *myxmms_get_filename(ui_state *ui)
{
  gint playlist_position = audacious_remote_get_playlist_pos(ui->pi->dbus_proxy);
  gchar *fname = audacious_remote_get_playlist_file(ui->pi->dbus_proxy, playlist_position);

  gchar *fname2 = g_filename_from_uri(fname, NULL, NULL);
  g_free(fname);

  return fname2;
}

//!returns the number of songs in the playlist
gint myxmms_get_playlist_number(ui_state *ui)
{
  return audacious_remote_get_playlist_length(ui->pi->dbus_proxy);
}

/*!returns the title of the song

The filename is allocated by this function and must be g_free'ed after use.
*/
gchar *myxmms_get_title_song(ui_state *ui)
{
  gint playlist_position = audacious_remote_get_playlist_pos(ui->pi->dbus_proxy);
  return audacious_remote_get_playlist_title(ui->pi->dbus_proxy, playlist_position);
}

//!returns elapsed time
gint myxmms_get_time_elapsed(ui_state *ui)
{
  return audacious_remote_get_output_time(ui->pi->dbus_proxy);
}

//!starts xmms
void myxmms_start(ui_state *ui)
{
  static gchar *exec_command = "audacious";
  gchar *exec_this = g_strdup_printf("%s &", exec_command);
  system(exec_this);

  time_t lt;
  gint timer = time(&lt);
  while (!audacious_remote_is_running(ui->pi->dbus_proxy) && ((time(&lt) - timer) < 4))
  {
    usleep(0);
  }

  g_free(exec_this);
}

//!selects the last file in the playlist
void myxmms_select_last_file(ui_state *ui)
{
  gint number = audacious_remote_get_playlist_length(ui->pi->dbus_proxy);
  audacious_remote_set_playlist_pos(ui->pi->dbus_proxy, number - 1);
}

//!plays the last file of the playlist
void myxmms_play_last_file(ui_state *ui)
{
  myxmms_select_last_file(ui);
  audacious_remote_play(ui->pi->dbus_proxy);
}

//!add files to the xmms playlist
void myxmms_add_files(GList *list, ui_state *ui)
{
  GList *list_pos = list;
  while (list_pos)
  {
    gchar *dup_filename = strdup(list_pos->data);
    list_pos->data = g_filename_to_uri(dup_filename,NULL,NULL);
    g_free(dup_filename);
    list_pos = g_list_next(list_pos);
  }

  audacious_remote_playlist_add(ui->pi->dbus_proxy, list); 
}

//!sets the volume level
void myxmms_set_volume(gint volume, ui_state *ui)
{
  audacious_remote_set_main_volume(ui->pi->dbus_proxy, volume);
}

//!returns volume level
gint myxmms_get_volume(ui_state *ui)
{
  return audacious_remote_get_main_volume(ui->pi->dbus_proxy);
}

/*!starts xmms with songs

  \param list The list of the songs to start xmms with
  \todo Which format is this list in?
 */
void myxmms_start_with_songs(GList *list, ui_state *ui)
{
  myxmms_start(ui);
  myxmms_add_files(list, ui);
}

//!returns TRUE if xmms is running; if not, FALSE 
gint myxmms_is_running(ui_state *ui)
{
  if (!ui->pi->dbus_connection)
  {
    ui->pi->dbus_connection = dbus_g_bus_get(DBUS_BUS_SESSION, NULL);
  }

  if (!ui->pi->dbus_proxy)
  {
    ui->pi->dbus_proxy = dbus_g_proxy_new_for_name(ui->pi->dbus_connection,
        AUDACIOUS_DBUS_SERVICE,
        AUDACIOUS_DBUS_PATH,
        AUDACIOUS_DBUS_INTERFACE);
  }

  if (!audacious_remote_is_running(ui->pi->dbus_proxy))
  {
    return FALSE;
  }

  return TRUE;
}

//!returns TRUE if xmms is paused, if not, FALSE 
gint myxmms_is_paused(ui_state *ui)
{
  if (!audacious_remote_is_paused(ui->pi->dbus_proxy))
  {
    return FALSE;
  }

  return TRUE;
}

//!Start playing the current song
void myxmms_play(ui_state *ui)
{
  audacious_remote_play(ui->pi->dbus_proxy);
}

//!Stop playing the current song
void myxmms_stop(ui_state *ui)
{
  audacious_remote_stop(ui->pi->dbus_proxy);
}

//!Pause playing the current song
void myxmms_pause(ui_state *ui)
{
  audacious_remote_pause(ui->pi->dbus_proxy);
}

//!Switch to the next song
void myxmms_next(ui_state *ui)
{
  audacious_remote_playlist_next(ui->pi->dbus_proxy);
}

//!Switch to the previous song
void myxmms_prev(ui_state *ui)
{
  audacious_remote_playlist_prev(ui->pi->dbus_proxy);
}

//!jump to time
void myxmms_jump(gint position, ui_state *ui)
{
  audacious_remote_jump_to_time(ui->pi->dbus_proxy, position);
}

//!returns the total duration of the current song
gint myxmms_get_total_time(ui_state *ui)
{
  gint playlist_position = audacious_remote_get_playlist_pos(ui->pi->dbus_proxy);
  return audacious_remote_get_playlist_time(ui->pi->dbus_proxy, playlist_position);
}

//!returns TRUE if xmms is playing, else FALSE
gint myxmms_is_playing(ui_state *ui)
{
  if (audacious_remote_is_playing(ui->pi->dbus_proxy))
  {
    return TRUE;
  }

  return FALSE;
}

#endif

