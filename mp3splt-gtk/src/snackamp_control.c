/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2012 Alexandru Munteanu
 * Contact: io_fx@yahoo.fr
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
 * Snackamp control
 *
 * this file contains functions to control the snackamp 
 * player
 **********************************************************/

#include "snackamp_control.h"

//file descriptors for line buffering
FILE *in, *out;
//if we are connected or not
gboolean connected = FALSE;

#ifdef __WIN32__
SOCKET socket_id;
#else
gint socket_id;
#endif

/*!connecting to the player to the port port

Might possibley return an error
*/
gint connect_snackamp(gint port)
{
  //socket and internet structures
  struct sockaddr_in host;
  struct hostent *h;
  gint return_err = 0;
  
#ifdef __WIN32__
  long winsockinit;
  WSADATA winsock;
  winsockinit = WSAStartup(0x0101,&winsock);
#endif
  
  //prepare host..
  if((h = gethostbyname("localhost"))==NULL)
    {
      //fprintf(stderr, "could not get host by name\n");
      return_err = 1;
    }
  
  //we prepare socket
  host.sin_family = AF_INET;
  host.sin_addr.s_addr = 
    ((struct in_addr *) (h->h_addr)) ->s_addr;
  host.sin_port=htons(port);
  
  //if no error, continue
  if (return_err == 0)
    {
      //initialize socket
      if((socket_id=socket(AF_INET, SOCK_STREAM, 0))==-1)
        {
          //fprintf(stderr, "error initialising socket\n");
          return_err = 2;
        }
    }
  
  //if no error, continue
  if (return_err == 0)
    {
      //make connection
      if ((connect(socket_id,
                   (void *)&host, sizeof(host)))==-1)
        {
          //fprintf(stderr, "cannot connect to host\n");
          return_err = 3;
        }
    }
  
  //if no error, continue
  if (return_err == 0)
    {
      /* !@Crap */
#ifdef __WIN32__
#else
      //we prepare file descriptors for 
      //
      if (NULL==(in=fdopen(socket_id, "r")) || 
          NULL==(out=fdopen(socket_id, "w")))
        {
          //fprintf(stderr, "cannot prepare file descriptors..\n");
          return_err = 4;
        }
#endif  
    }
  
  //if no err
  if (return_err == 0)
    {
      //we set the line buffering
      setvbuf(out, NULL, _IOLBF, 0);
      connected = TRUE;
    }
  
  //if socket errors, disconnect snackamp
  if (return_err >= 2)
    disconnect_snackamp();
  
  return return_err;
}

static gchar *cut_begin_end(gchar *result)
{
  if (strchr(result,' ') != NULL)
  {
    gchar *test = strchr(result, ' ');
    g_snprintf(result, strlen(result), "%s",test+1);
  }

  //cut the \n at the end
  gint result_str = strlen(result);
  if (result_str >= 2)
  {
    result[result_str - 2] = '\0';
  }

  return result;
}

/*! disconnecting with the player
possibly returns an error
*/
gint disconnect_snackamp()
{
  connected = FALSE;
#ifdef __WIN32__
  return closesocket(socket_id);
#else
  return close(socket_id);
#endif
}

/*! send a message to snackamp through the socket interface

\return the result; must be g_freed after use
\todo rewrite this function
*/
static gchar *snackamp_socket_send_message(gchar *message)
{
  gchar *result = malloc(1024 * sizeof(gchar *));
  strcpy(result,"disconnected");

#ifdef __WIN32__
  gboolean r = TRUE;

  gint err1 = send(socket_id, message, strlen(message), 0);
  if (err1 <= 0)
  {
    disconnect_snackamp();
  }
  else
  {
    gint err = recv(socket_id, result, 1024, 0);
    if (err <= 0)
    {
      disconnect_snackamp();
      r = FALSE;
    }
  }
#else
  fputs(message, out);
  fgets(result, 1024, in);
#endif

  //if on win32 we put the \0 when we find \n because no line buffering
#ifdef __WIN32__
  if (r)
  {
    if (strchr(result,'\n') != NULL)
    {
      gchar *line_end;
      line_end = strchr(result,'\n') + 1;
      *line_end = '\0';
    }
  }
#endif

  return result;
}

//!gets an integer from the string
gint get_integer_from_string(gchar *result)
{
  gint our_integer = 0;
  gint i = 0;
  gchar *number = NULL;
  while ((isdigit(result[i])==0) && (result[i]!='\0') && (result[i]!='-'))
  {
    i++;
    number = result + i;
  }
  if (! (number == (result + strlen(result))))
  {
    our_integer = atoi (number);
  }

  return our_integer;
}

//!Test if we are connected to snackamp
gboolean snackamp_is_connected()
{
  return connected;
}

//!gets informations about the song
void snackamp_get_song_infos(gchar *total_infos)
{
  gchar *result = snackamp_socket_send_message("xmms_remote_get_info\n");
  result = cut_begin_end(result);

  //stereo/mono
  char *a = strstr(result, " ");
  if (a == NULL)
  {
    g_snprintf(total_infos, 512, "disconnected");
    g_free(result);
    return;
  }

  gchar rate_str[32] = { '\0' };
  gchar freq_str[32] = { '\0' };
  gchar nch_str[32] = { '\0' };
  gchar *ptr = NULL;

  if (strstr(a+1, " ") != NULL)
  {
    ptr = strstr(a+1, " ") + 1;
  }

  //rate
  gint i = 0;
  while (result[i] != ' ' && isdigit(result[i]) && i < 16)
  {
    g_sprintf(rate_str, "%s%c",rate_str,result[i]);
    i++;
  }

  //cut the beginning
  if (strchr(result, ' ') != NULL)
  {
    gchar *test = strchr(result,' ');
    g_snprintf(result, strlen(result), "%s",test+1);
  }

  //freq
  i = 0;
  while (result[i] != ' ' && isdigit(result[i]) && i < 16)
  {
    g_sprintf(freq_str, "%s%c",freq_str,result[i]);
    i++;
  }

  //channels int
  gint nch = atoi(ptr);
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
  g_snprintf(total_infos, 512, "%s %s     %s %s    %s", rate_str, _Kbps, freq_str, _Khz, nch_str);

  g_free(result);
}

/*!returns the filename

The result of this query must be freed after use.
*/
gchar *snackamp_get_filename()
{
  gint playlist_pos = snackamp_get_playlist_pos();
  
  //we get the current file
  gchar temp[100];
  g_snprintf(temp, 100, "%s %d\n", "xmms_remote_get_playlist_file", playlist_pos);
 
  gchar *result = snackamp_socket_send_message(temp);
  result = cut_begin_end(result);

  return result;
}

//!returns current song position in the playlist
gint snackamp_get_playlist_pos()
{
  gchar *result = snackamp_socket_send_message("xmms_remote_get_playlist_pos\n");
  gint number = get_integer_from_string(result);
  g_free(result);
  return number;
}

//!stops playing a song
void snackamp_stop()
{
  gchar *result = snackamp_socket_send_message("xmms_remote_stop\n");
  g_free(result);
}

//!returns the number of songs of the playlist
gint snackamp_get_playlist_number()
{
  gchar *result = snackamp_socket_send_message("xmms_remote_get_playlist_length\n");
  gint number = get_integer_from_string(result);
  g_free(result);

  if (number == -1)
  {
    snackamp_stop();
  }

  return number;
}

/*!returns the title of the song

The return value must be g_free'd after use.
*/
gchar *snackamp_get_title_song()
{
  gint playlist_pos = snackamp_get_playlist_pos();
  
  gchar temp[100];
  g_snprintf(temp, 100,"%s %d\n", "xmms_remote_get_playlist_title",playlist_pos);

  gchar *result = snackamp_socket_send_message(temp);
  result = cut_begin_end(result);
  
  return result;
}

//!returns elapsed time
gint snackamp_get_time_elapsed()
{
  gchar *result = snackamp_socket_send_message("xmms_remote_get_output_time\n");
  gint pos = get_integer_from_string(result);
  g_free(result);

  return pos;
}

//!starts snackamp
void snackamp_start()
{
  static gchar *exec_command = "snackAmp";
  gchar *exec_this = g_strdup_printf("%s &", exec_command);
  system(exec_this);
  
  time_t lt;
  gint timer = time(&lt);
  while ((!snackamp_is_running()) && ((time(&lt) - timer) < 8))
  {
    usleep(0);
  }

  g_free(exec_this);
}

//!jumps to the position pos in the playlist
void snackamp_set_playlist_pos(gint pos)
{
  gchar temp[100];
  g_snprintf(temp, 100, "%s %d\n", "xmms_remote_set_playlist_pos",pos);
  gchar *result = snackamp_socket_send_message(temp);
  g_free(result);
}

//!selects the last file in the playlist
void snackamp_select_last_file()
{
  gint last_song = snackamp_get_playlist_number();
  snackamp_set_playlist_pos(last_song-1);
}

//!plays a song
void snackamp_play()
{
  gchar *result = snackamp_socket_send_message("xmms_remote_play\n");
  g_free(result);
}

//!plays the last file of the playlist
void snackamp_play_last_file()
{
  snackamp_select_last_file();
  snackamp_play();
}

//!add files to the snackamp playlist
void snackamp_add_files(GList *list)
{
  gint i = 0;
  gchar *song = NULL;
  while ((song = g_list_nth_data(list, i)) != NULL)
  {
    gint malloc_int = strlen(song) + 30;
    gchar *local = malloc(malloc_int * sizeof(gchar *));
    g_snprintf(local, malloc_int, "%s {%s}\n", "xmms_remote_playlist_add ", song);

    gchar *result = snackamp_socket_send_message(local);
    g_free(result);

    g_free(local);
    i++;
  }
}

//!sets volume
void snackamp_set_volume(gint volume)
{
  gchar temp[100];
  g_snprintf(temp, 100, "%s %d\n", "xmms_remote_set_main_volume",volume);
  gchar *result = snackamp_socket_send_message(temp);
  g_free(result);
}

//!returns volume
gint snackamp_get_volume()
{
  gchar *result = snackamp_socket_send_message("xmms_remote_get_main_volume\n");
  gint vol = get_integer_from_string(result);
  g_free(result);
 
  return vol;
}

//!starts snackamp with songs
void snackamp_start_with_songs(GList *list)
{
  snackamp_start();
  snackamp_add_files(list);
}

//!returns TRUE if snackamp is running; if not, FALSE 
gint snackamp_is_running()
{
  if (connected)
  {
    return TRUE;
  }

  if (connect_snackamp(8775) == 0)
  {
    return TRUE;
  }

  return FALSE;
}

//!pause a song
void snackamp_pause()
{
  gchar *result = snackamp_socket_send_message("xmms_remote_pause\n");
  g_free(result);
}

//!changes to next song
void snackamp_next()
{
  gchar *result = snackamp_socket_send_message("xmms_remote_playlist_next\n");
  g_free(result);
}

//!changes to previous song
void snackamp_prev()
{
  gint playlist_pos = snackamp_get_playlist_pos();

  if (playlist_pos > 0)
  {
    gchar *result = snackamp_socket_send_message("xmms_remote_playlist_prev\n");
    g_free(result);
    return;
  }

  snackamp_play_last_file();
}

//!jump to time
void snackamp_jump(gint position)
{
  gint hundr_secs_pos = position / 10;
  gint hundr_secs = hundr_secs_pos % 100;
  if (hundr_secs == 1)
  {
    hundr_secs = 0;
  }

  gint secs = hundr_secs_pos / 100;
  gfloat total_pos = hundr_secs + secs * 100;
  total_pos /= 100;

  gchar temp[100];
  g_snprintf(temp, 100, "%s %f\n", "xmms_remote_jump_to_time", total_pos);

  gchar *result = snackamp_socket_send_message(temp);
  g_free(result);
}

//!returns total time of the current song
gint snackamp_get_total_time()
{
  gchar *result = snackamp_socket_send_message("xmms_remote_get_playlist_time\n");
  gint hundr_secs = get_integer_from_string(result) * 1000;
  g_free(result);

  return hundr_secs;
}

//!returns TRUE if snackamp is playing, else FALSE
gint snackamp_is_playing()
{
  if (!snackamp_is_connected())
  {
    return FALSE;
  }

  gchar *result = snackamp_socket_send_message("xmms_remote_is_playing\n");
  gint i = atoi(result);
  g_free(result);

  if (i == 0)
  {
    return FALSE;
  }

  return TRUE;
}

/*! returns TRUE if snackamp is paused, else FALSE

not yet implemented in snackamp
*/
gint snackamp_is_paused()
{
  if (!snackamp_is_connected())
  {
    return FALSE;
  }

  gchar *result = snackamp_socket_send_message("xmms_remote_is_paused\n");
  result = cut_begin_end(result);

  gint i = atoi(result);
  g_free(result);

  if (i == 1)
  {
    return TRUE;
  }

  return FALSE;
}

/*//quits player
gint snackamp_quit()
{
  return 1;
}
*/
