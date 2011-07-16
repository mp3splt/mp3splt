/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2011 Alexandru Munteanu
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

/*!*******************************************************
 * \file 
 * Control the gstreamer framework
 *
 * this file has functions to control the 'internal' 
 + gstreamer player
 *********************************************************/

#include <stdlib.h>
#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#ifndef NO_GSTREAMER
#include <gst/gst.h>
#include "gstreamer_control.h"
#endif

#include "player.h"
#include "main_win.h"
#include "utilities.h"
#include "player_tab.h"
#include "mp3splt-gtk.h"

extern int selected_player;

#ifndef NO_GSTREAMER

const gchar *song_artist = NULL;
const gchar *song_title = NULL;
gint rate = 0;

GstElement *play = NULL;
GstBus *bus = NULL;
gint _gstreamer_is_running = FALSE;
extern GtkWidget *playlist_box;
extern GtkWidget *player_vbox;

extern void add_playlist_file(const gchar *name);

//! Send a call over the dbus
static gboolean bus_call (GstBus *bus, GstMessage *msg, gpointer data)
{
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

          enter_threads();
          put_status_message(message);
          exit_threads();

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

          enter_threads();
          put_status_message(message);
          exit_threads();

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

          enter_threads();
          put_status_message(message);
          exit_threads();

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
      g_object_get(play, "current-audio", &number_of_stream, NULL);

      //artist
      const GValue *val = gst_tag_list_get_value_index(tag_list, GST_TAG_ARTIST, 
          number_of_stream);
      if (val != NULL)
      {
        song_artist = g_value_get_string(val);
      }

      //title
      val = gst_tag_list_get_value_index(tag_list, GST_TAG_TITLE, number_of_stream);
      if (val != NULL)
      {
        song_title = g_value_get_string(val);
      }

      //rate (bps)
      val = gst_tag_list_get_value_index(tag_list, GST_TAG_BITRATE, number_of_stream);
      if (val != NULL)
      {
        rate = g_value_get_uint(val);
      }
 
      break;
    }
    default:
      break;
  }

  return TRUE;
}

//!Gets information about the< song
void gstreamer_get_song_infos(gchar *total_infos)
{
	if (play)
	{
		gint freq = 0;
		gint nch = 0;

		gint number_of_stream = 0;
		g_object_get(play, "current-audio", &number_of_stream, NULL);

		//get the stream info
		GList *streaminfo = NULL;
		g_object_get(play, "stream-info", &streaminfo, NULL);

		gchar rate_str[32] = { '\0' };
		gchar freq_str[32] = { '\0' };
		gchar nch_str[32] = { '\0' };

		gchar *_Kbps = _("Kbps");
		gchar *_Khz = _("Khz");

		//get the first element of the stream info list
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

			g_snprintf(rate_str,32, "%d", rate/1000);
			g_snprintf(freq_str,32, "%d", freq/1000);

			if (nch >= 2)
			{
				snprintf(nch_str, 32, "%s", _("stereo"));
			}
			else
			{
				snprintf(nch_str, 32, "%s", _("mono"));
			}
		}

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
}

/*! returns the filename

The result must be g_free'd after use.
*/
gchar *gstreamer_get_filename()
{
  return strdup(inputfilename_get());
}

//!returns the number of songs of the playlist
gint gstreamer_get_playlist_number()
{
  return 1;
}

/*!returns the title of the song

The result must be g_free'd after use
*/
gchar *gstreamer_get_title_song()
{
  if (song_artist || song_title)
  {
    gint title_size = 20;

    if (song_artist)
    {
      title_size += strlen(song_artist);
    }

    if (song_title)
    {
      title_size += strlen(song_title);
    }

    gchar *title = malloc(sizeof(char) * title_size);
    memset(title, title_size, '\0');

    if (song_artist && song_title)
    {
      g_snprintf(title, title_size, "%s - %s", song_artist, song_title);
    }
    else if (song_title && !song_artist)
    {
      g_snprintf(title, title_size, "%s", song_title);
    }
    else if (song_artist && !song_title)
    {
      g_snprintf(title, title_size, "%s", song_artist);
    }

    return title;
  }
  else
  {
    gchar *fname = gstreamer_get_filename();
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
}

//!returns elapsed time
gint gstreamer_get_time_elapsed()
{
	if (play)
	{
		GstQuery *query = gst_query_new_position(GST_FORMAT_TIME);
		gint64 time = 0;

		if (gst_element_query(play, query))
		{
			gst_query_parse_position(query, NULL, &time);
		}

		gst_query_unref(query);

		return (gint) (time / GST_MSECOND);
	}
	else
	{
		return 0;
	}
}

//!starts gstreamer
void gstreamer_start()
{
  if (play)
  {
    gstreamer_quit();
    gst_object_unref(play);
  }

  gst_init(NULL, NULL);

#ifdef __WIN32__
  gst_default_registry_add_path("./");
#endif

  play = gst_element_factory_make("playbin", "play");
	//if we have started the player
  if (play)
	{
    if (! container_has_child(GTK_CONTAINER(player_vbox), playlist_box))
    {
      gtk_box_pack_start(GTK_BOX(player_vbox), playlist_box, TRUE, TRUE, 0);
    }
    gtk_widget_show_all(playlist_box);

		_gstreamer_is_running = TRUE;
		bus = gst_pipeline_get_bus (GST_PIPELINE (play));
		gst_bus_add_watch(bus, bus_call, NULL);
		gst_object_unref(bus);

		//add the current filename
		const gchar *fname =  inputfilename_get();
		GList *song_list = NULL;
		song_list = g_list_append(song_list, strdup(fname));
		gstreamer_add_files(song_list);
    //TODO: free memory from GList *song_list
	}
	else
	{
		enter_threads();
		put_status_message(_(" error: cannot create gstreamer playbin\n"));
		exit_threads();
	}
}

//!selects the last file in the playlist
void gstreamer_select_last_file()
{
}

//!plays the last file of the playlist
void gstreamer_play_last_file()
{
  gstreamer_stop();
  gstreamer_play();
}

//!add files to the gstreamer playlist
void gstreamer_add_files(GList *list)
{
  gchar *song = NULL;
  gint i = 0;
  gchar *uri = NULL;
  int len_uri = 20;

  if (song_title)
  {
    song_title = NULL;
  }
  if (song_artist)
  {
    song_artist = NULL;
  }

	if (play)
  {
    while ((song = g_list_nth_data(list, i)) != NULL)
    {
      if (song)
      {
        //add file to playlist
        add_playlist_file(song);
        len_uri += strlen(song);
        uri = malloc(sizeof(char) * len_uri);
        g_snprintf(uri,len_uri,"file://%s",song);
        g_object_set(G_OBJECT(play), "uri", uri, NULL);
        if (uri)
        {
          g_free(uri);
          uri = NULL;
        }
      }
      i++;
    }
  }
}

//!sets volume
void gstreamer_set_volume(gint volume)
{
	if (play)
	{
		//values between 0 and 2
		//-documentation says values can be between 0 and 10
		g_object_set(G_OBJECT(play), "volume", (double) volume / 100.0 * 2, NULL);
	}
}

//!returns volume
gint gstreamer_get_volume()
{
	if (play)
	{
		double volume = 0;
		//values between 0 and 2
		//-documentation says values can be between 0 and 10
		g_object_get(G_OBJECT(play), "volume", &volume, NULL);

		return (gint) (volume / 2 * 100);
	}
	else
	{
		return 0;
	}
}

//!starts gstreamer with songs
void gstreamer_start_with_songs(GList *list)
{
  gstreamer_start();
  gstreamer_add_files(list);
  gstreamer_play();
}

//!returns TRUE if gstreamer is running; if not, FALSE 
gint gstreamer_is_running()
{
  return _gstreamer_is_running;
}

//!returns TRUE if gstreamer is paused, if not, FALSE 
gint gstreamer_is_paused()
{
	if (play)
	{
		GstState state;
		gst_element_get_state(play, &state, NULL, GST_CLOCK_TIME_NONE);

		if (state == GST_STATE_PAUSED)
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

//!plays a song
void gstreamer_play()
{
	if (play)
	{
		GstState state;
		gst_element_get_state(play, &state, NULL, GST_CLOCK_TIME_NONE);

    if (state == GST_STATE_PLAYING)
    {
      gstreamer_jump(0);
    }
    else
    {
      gst_element_set_state(play, GST_STATE_PLAYING);
    }
  }
}

//!stops a song
void gstreamer_stop()
{
	if (play)
	{
		gst_element_set_state(play, GST_STATE_NULL);
	}
}

//!pause a song
void gstreamer_pause()
{
	if (play)
	{
		GstState state;
		gst_element_get_state(play, &state, NULL, GST_CLOCK_TIME_NONE);

    if (state == GST_STATE_PLAYING)
    {
      gst_element_set_state(play, GST_STATE_PAUSED);
    }
    else
    {
      gstreamer_play();
    }
  }
}

//!changes to next song
void gstreamer_next()
{
}

//!changes to previous song
void gstreamer_prev()
{
}

//!jump to time
void gstreamer_jump(gint position)
{
	if (play)
	{
		gst_element_seek(play,
				1.0, GST_FORMAT_TIME, GST_SEEK_FLAG_FLUSH,
				GST_SEEK_TYPE_SET, position * GST_MSECOND, 0, 0);
	}
}

//!returns total time of the current song
gint gstreamer_get_total_time()
{
	if (play)
	{
		GstQuery *query = gst_query_new_duration(GST_FORMAT_TIME);
		gint time = 0;

		if (gst_element_query(play,query))
		{
			gint64 total_time;
			gst_query_parse_duration (query, NULL, &total_time);
			time = (gint) (total_time / GST_MSECOND);
		}

		gst_query_unref(query);

		return time;
	}
	else
	{
		return 0;
	}
}

//!returns TRUE if gstreamer is playing, else FALSE
gint gstreamer_is_playing()
{
	if (play)
	{
		GstState state;
		gst_element_get_state(play, &state, NULL, GST_CLOCK_TIME_NONE);

		if ((state == GST_STATE_PLAYING) || (state == GST_STATE_PAUSED))
		{
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

//!quits player
void gstreamer_quit()
{
  if (play)
  {
    gst_element_set_state(play, GST_STATE_NULL);
  }
  _gstreamer_is_running = FALSE;

  gtk_widget_hide(playlist_box);
}

#endif

