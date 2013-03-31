/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2013 Alexandru Munteanu
 * Contact: m@ioalex.net
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 *
 *********************************************************/

/*!*******************************************************
 * \file 
 * Control the gstreamer framework
 *
 * this file has functions to control the 'internal' 
 + gstreamer player
 *********************************************************/

#ifndef NO_GSTREAMER

#include <gst/gst.h>

#include "gstreamer_control.h"

//! Send a call over the dbus
static gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer data)
{
  enter_threads();

  ui_state *ui = (ui_state *) data;

  switch (GST_MESSAGE_TYPE(msg))
  {
    case GST_MESSAGE_ERROR:
    {
      gchar  *debug;
      GError *error;

      gst_message_parse_error (msg, &error, &debug);
      g_free(debug);

      gchar *message = NULL;
      if (error->message != NULL)
      {
        gint malloc_size = strlen(error->message) + 20;
        message = malloc(sizeof(char) * malloc_size);
        if (message)
        {
          memset(message,'\0',malloc_size);
          g_snprintf(message, malloc_size,_("gstreamer error: %s"),error->message);

          put_status_message(message, ui);

          g_free(message);
        }
      }
      g_error_free(error);
      break;
    }
    case GST_MESSAGE_WARNING:
    {
      gchar  *debug;
      GError *error;

      gst_message_parse_warning(msg, &error, &debug);
      g_free(debug);

      gchar *message = NULL;
      if (error->message != NULL)
      {
        gint malloc_size = strlen(error->message) + 20;
        message = malloc(sizeof(char) * malloc_size);
        if (message)
        {
          memset(message,'\0',malloc_size);
          g_snprintf(message, malloc_size,_("Warning: %s"),error->message);

          put_status_message(message, ui);

          g_free(message);
        }
      }
      g_error_free (error);
      break;
    }
    case GST_MESSAGE_INFO:
    {
      gchar  *debug;
      GError *error;

      gst_message_parse_info(msg, &error, &debug);
      g_free(debug);

      gchar *message = NULL;
      if (error->message != NULL)
      {
        gint malloc_size = strlen(error->message) + 20;
        message = malloc(sizeof(char) * malloc_size);
        if (message)
        {
          memset(message,'\0',malloc_size);
          g_snprintf(message, malloc_size,_("Info: %s"),error->message);

          put_status_message(message, ui);

          g_free(message);
        }
      }
      g_error_free (error);
      break;
    }
    case GST_MESSAGE_TAG:
    {
      GstTagList *tag_list = NULL;
      gst_message_parse_tag(msg, &tag_list);
      
      gint number_of_stream = 0;
      g_object_get(ui->pi->play, "current-audio", &number_of_stream, NULL);

      //artist
      const GValue *val = gst_tag_list_get_value_index(tag_list, GST_TAG_ARTIST, 
          number_of_stream);
      if (val != NULL)
      {
        ui->pi->song_artist = g_value_get_string(val);
      }

      //title
      val = gst_tag_list_get_value_index(tag_list, GST_TAG_TITLE, number_of_stream);
      if (val != NULL)
      {
        ui->pi->song_title = g_value_get_string(val);
      }

      //rate (bps)
      val = gst_tag_list_get_value_index(tag_list, GST_TAG_BITRATE, number_of_stream);
      if (val != NULL)
      {
        ui->pi->rate = g_value_get_uint(val);
      }
 
      break;
    }
    default:
      break;
  }

  exit_threads();

  return TRUE;
}

//!Gets information about the< song
void gstreamer_get_song_infos(gchar *total_infos, ui_state *ui)
{
  if (!ui->pi->play)
  {
    return;
  }

  gint freq = 0;
  gint nch = 0;

  gint number_of_stream = 0;
  g_object_get(ui->pi->play, "current-audio", &number_of_stream, NULL);

  //get the stream info
  GList *streaminfo = NULL;
  g_object_get(ui->pi->play, "stream-info", &streaminfo, NULL);

  gchar rate_str[32] = { '\0' };
  gchar freq_str[32] = { '\0' };
  gchar nch_str[32] = { '\0' };

  gchar *_Kbps = _("Kbps");
  gchar *_Khz = _("Khz");

  GObject *object = g_list_nth_data(streaminfo, number_of_stream); 
  if (object)
  {
    GstObject *obj = NULL; 
    g_object_get(G_OBJECT(object), "object", &obj, NULL);

    //get the caps from the first element
    GstCaps *caps = NULL;
    g_object_get(obj, "caps", &caps, NULL);
    if (caps)
    {
      //get the structure from the caps
      GstStructure *structure = NULL;
      structure = gst_caps_get_structure(caps, number_of_stream);

      //get the rate and the number of channels from the structure
      gst_structure_get_int(structure, "rate", &freq);
      gst_structure_get_int(structure, "channels", &nch);

      gst_caps_unref(caps);
    }

    g_snprintf(rate_str, 32, "%d", ui->pi->rate / 1000);
    g_snprintf(freq_str, 32, "%d", freq / 1000);

    if (nch >= 2)
    {
      snprintf(nch_str, 32, "%s", _("stereo"));
    }
    else
    {
      snprintf(nch_str, 32, "%s", _("mono"));
    }
  }

  if (ui->pi->rate != 0)
  {
    g_snprintf(total_infos,512, "%s %s     %s %s    %s", rate_str,_Kbps,freq_str, _Khz,nch_str);
  }
  else 
  {
    total_infos[0] = '\0';
  }
}

/*! returns the filename

The result must be g_free'd after use.
*/
gchar *gstreamer_get_filename(ui_state *ui)
{
  gchar *fname =  get_input_filename(ui->gui);
  if (fname == NULL)
  {
    return NULL;
  }

  return strdup(fname);
}

//!returns the number of songs of the playlist
gint gstreamer_get_playlist_number(ui_state *ui)
{
  return 1;
}

/*!returns the title of the song

The result must be g_free'd after use
*/
gchar *gstreamer_get_title_song(ui_state *ui)
{
  player_infos *pi = ui->pi;

  if (pi->song_artist || pi->song_title)
  {
    gint title_size = 20;

    if (pi->song_artist)
    {
      title_size += strlen(pi->song_artist);
    }

    if (pi->song_title)
    {
      title_size += strlen(pi->song_title);
    }

    gchar *title = malloc(sizeof(char) * title_size);
    memset(title, '\0', title_size);

    if (pi->song_artist && pi->song_title)
    {
      g_snprintf(title, title_size, "%s - %s", pi->song_artist, pi->song_title);
    }
    else if (pi->song_title && !pi->song_artist)
    {
      g_snprintf(title, title_size, "%s", pi->song_title);
    }
    else if (pi->song_artist && !pi->song_title)
    {
      g_snprintf(title, title_size, "%s", pi->song_artist);
    }

    return title;
  }

  gchar *fname = gstreamer_get_filename(ui);
  if (fname != NULL)
  {
    gchar *file = strrchr(fname, G_DIR_SEPARATOR);
    if (file != NULL)
    {
      gchar *alloced_file = strdup(file+1);
      g_free(fname);
      fname = NULL;
      return alloced_file;
    }
    else
    {
      return fname;
    }
  }
  else
  {
    return strdup("-");
  }
}

//!returns elapsed time
gint gstreamer_get_time_elapsed(ui_state *ui)
{
  if (!ui->pi->play)
  {
    return 0;
  }

  GstQuery *query = gst_query_new_position(GST_FORMAT_TIME);
  gint64 time = 0;

  if (gst_element_query(ui->pi->play, query))
  {
    gst_query_parse_position(query, NULL, &time);
  }

  gst_query_unref(query);

  return (gint) (time / GST_MSECOND);
}

//!starts gstreamer
void gstreamer_start(ui_state *ui)
{
  if (ui->pi->play)
  {
    gstreamer_quit(ui);
    gst_object_unref(ui->pi->play);
  }

  gst_init(NULL, NULL);

#ifdef __WIN32__
  gst_default_registry_add_path("./");
#endif

  ui->pi->play = gst_element_factory_make("playbin", "play");
  if (!ui->pi->play)
  {
    put_status_message(_(" error: cannot create gstreamer playbin\n"), ui);
    return;
  }

  gtk_widget_show_all(ui->gui->playlist_box);

  ui->pi->_gstreamer_is_running = TRUE;
  ui->pi->bus = gst_pipeline_get_bus(GST_PIPELINE(ui->pi->play));
  gst_bus_add_watch(ui->pi->bus, bus_call, ui);
  gst_object_unref(ui->pi->bus);

  //add the current filename
  const gchar *fname =  get_input_filename(ui->gui);
  GList *song_list = g_list_append(NULL, strdup(fname));
  gstreamer_add_files(song_list, ui);
  g_list_foreach(song_list, (GFunc)g_free, NULL);
  g_list_free(song_list);
}

//!selects the last file in the playlist
void gstreamer_select_last_file(ui_state *ui)
{
}

//!plays the last file of the playlist
void gstreamer_play_last_file(ui_state *ui)
{
  gstreamer_stop(ui);
  gstreamer_play(ui);
}

//!add files to the gstreamer playlist
void gstreamer_add_files(GList *list, ui_state *ui)
{
  player_infos *pi = ui->pi;

  if (pi->song_title) { pi->song_title = NULL; }
  if (pi->song_artist) { pi->song_artist = NULL; }

  if (!ui->pi->play)
  {
    return;
  }

  gint i = 0;
  gchar *song = NULL;
  while ((song = g_list_nth_data(list, i)) != NULL)
  {
    if (!song)
    {
      i++;
      continue;
    }

    add_playlist_file(song, ui);

    gchar *uri = g_filename_to_uri(song, NULL, NULL);
    g_object_set(G_OBJECT(ui->pi->play), "uri", uri, NULL);
    if (uri) { g_free(uri); }

    i++;
  }
}

//!sets volume
void gstreamer_set_volume(gint volume, ui_state *ui)
{
	if (ui->pi->play)
	{
		//values between 0 and 2
		//-documentation says values can be between 0 and 10
		g_object_set(G_OBJECT(ui->pi->play), "volume", (double) volume / 100.0 * 2, NULL);
	}
}

//!returns volume
gint gstreamer_get_volume(ui_state *ui)
{
  if (!ui->pi->play)
  {
    return 0;
  }

  //values between 0 and 2
  //-documentation says values can be between 0 and 10
  double volume = 0;
  g_object_get(G_OBJECT(ui->pi->play), "volume", &volume, NULL);
  return (gint) (volume / 2 * 100);
}

//!starts gstreamer with songs
void gstreamer_start_with_songs(GList *list, ui_state *ui)
{
  gstreamer_start(ui);
  gstreamer_add_files(list, ui);
  gstreamer_play(ui);
}

//!returns TRUE if gstreamer is running; if not, FALSE 
gint gstreamer_is_running(ui_state *ui)
{
  return ui->pi->_gstreamer_is_running;
}

//!returns TRUE if gstreamer is paused, if not, FALSE 
gint gstreamer_is_paused(ui_state *ui)
{
  if (!ui->pi->play)
  {
    return FALSE;
  }

  GstState state;
  gst_element_get_state(ui->pi->play, &state, NULL, GST_CLOCK_TIME_NONE);

  if (state == GST_STATE_PAUSED)
  {
    return TRUE;
  }

  return FALSE;
}

//!plays a song
void gstreamer_play(ui_state *ui)
{
  if (!ui->pi->play)
  {
    return;
  }

  GstState state;
  gst_element_get_state(ui->pi->play, &state, NULL, GST_CLOCK_TIME_NONE);
  if (state == GST_STATE_PLAYING)
  {
    gstreamer_jump(0, ui);
  }

  gst_element_set_state(ui->pi->play, GST_STATE_PLAYING);
}

//!stops a song
void gstreamer_stop(ui_state *ui)
{
  if (!ui->pi->play)
  {
    return;
  }

  gst_element_set_state(ui->pi->play, GST_STATE_NULL);
}

//!pause a song
void gstreamer_pause(ui_state *ui)
{
  if (!ui->pi->play)
  {
    return;
  }

  GstState state;
  gst_element_get_state(ui->pi->play, &state, NULL, GST_CLOCK_TIME_NONE);

  if (state == GST_STATE_PLAYING)
  {
    gst_element_set_state(ui->pi->play, GST_STATE_PAUSED);
  }
  else
  {
    gstreamer_play(ui);
  }
}

//!changes to next song
void gstreamer_next(ui_state *ui)
{
}

//!changes to previous song
void gstreamer_prev(ui_state *ui)
{
}

//!jump to time
void gstreamer_jump(gint position, ui_state *ui)
{
  if (!ui->pi->play)
  {
    return;
  }

  gst_element_seek(ui->pi->play,
      1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
      GST_SEEK_TYPE_SET, position * GST_MSECOND, 0, 0);
}

//!returns total time of the current song
gint gstreamer_get_total_time(ui_state *ui)
{
  if (!ui->pi->play)
  {
    return 0;
  }

  GstQuery *query = gst_query_new_duration(GST_FORMAT_TIME);
  gint time = 0;

  if (gst_element_query(ui->pi->play, query))
  {
    gint64 total_time;
    gst_query_parse_duration(query, NULL, &total_time);
    time = (gint) (total_time / GST_MSECOND);
  }

  gst_query_unref(query);

  return time;
}

//!returns TRUE if gstreamer is playing, else FALSE
gint gstreamer_is_playing(ui_state *ui)
{
  if (!ui->pi->play)
  {
    return FALSE;
  }

  GstState state;
  gst_element_get_state(ui->pi->play, &state, NULL, GST_CLOCK_TIME_NONE);
  if ((state == GST_STATE_PLAYING) || (state == GST_STATE_PAUSED))
  {
    return TRUE;
  }

  return FALSE;
}

//!quits player
void gstreamer_quit(ui_state *ui)
{
  if (ui->pi->play)
  {
    gst_element_set_state(ui->pi->play, GST_STATE_NULL);
  }

  ui->pi->_gstreamer_is_running = FALSE;
}

#endif

