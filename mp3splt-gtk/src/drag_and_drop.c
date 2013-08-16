/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2013 Alexandru Munteanu
 * Contact: m@ioalex.net
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

#include "drag_and_drop.h"

static void dnd_data_received(GtkWidget *window, GdkDragContext *drag_context,
    gint x, gint y, GtkSelectionData *data, guint info, guint time,
    gboolean discard_data_files, gboolean discard_audio_files,
    gboolean single_file_mode, ui_state *ui);

static void dnd_data_received_data_files(GtkWidget *window, 
    GdkDragContext *drag_context, gint x, gint y, GtkSelectionData *data, guint info, 
    guint time, ui_state *ui)
{
  dnd_data_received(window, drag_context, x, y, data, info, time, FALSE, TRUE, TRUE, ui);
}

static void dnd_data_received_batch_mode_audio_files(GtkWidget *window, 
    GdkDragContext *drag_context, gint x, gint y, GtkSelectionData *data, guint info, 
    guint time, ui_state *ui)
{
  dnd_data_received(window, drag_context, x, y, data, info, time, TRUE, FALSE, FALSE, ui);
}

static void dnd_data_received_single_mode_audio_files(GtkWidget *window, 
    GdkDragContext *drag_context, gint x, gint y, GtkSelectionData *data, guint info, 
    guint time, ui_state *ui)
{
  dnd_data_received(window, drag_context, x, y, data, info, time, TRUE, FALSE, TRUE, ui);
}

static void dnd_data_received_single_mode_audio_and_data_files(GtkWidget *window, 
    GdkDragContext *drag_context, gint x, gint y, GtkSelectionData *data, guint info,
    guint time, ui_state *ui)
{
  dnd_data_received(window, drag_context, x, y, data, info, time, FALSE, FALSE, TRUE, ui);
}

void dnd_add_drag_data_received_to_widget(GtkWidget *widget, drop_type type, ui_state *ui)
{
  gtk_drag_dest_set(widget, GTK_DEST_DEFAULT_ALL, drop_types, 3,
      GDK_ACTION_COPY | GDK_ACTION_MOVE | GDK_ACTION_LINK);

  switch (type)
  {
    case DND_SINGLE_MODE_AUDIO_FILE:
      g_signal_connect(G_OBJECT(widget), "drag-data-received",
          G_CALLBACK(dnd_data_received_single_mode_audio_files), ui);
      break;
    case DND_BATCH_MODE_AUDIO_FILES:
      g_signal_connect(G_OBJECT(widget), "drag-data-received",
          G_CALLBACK(dnd_data_received_batch_mode_audio_files), ui);
      break;
    case DND_DATA_FILES:
      g_signal_connect(G_OBJECT(widget), "drag-data-received",
          G_CALLBACK(dnd_data_received_data_files), ui);
      break;
    case DND_SINGLE_MODE_AUDIO_FILE_AND_DATA_FILES:
      g_signal_connect(G_OBJECT(widget), "drag-data-received",
          G_CALLBACK(dnd_data_received_single_mode_audio_and_data_files), ui);
      break;
    default:
      break;
  }
}

static void dnd_data_received(GtkWidget *window, GdkDragContext *drag_context,
    gint x, gint y, GtkSelectionData *data, guint info, guint time,
    gboolean discard_data_files, gboolean discard_audio_files,
    gboolean single_file_mode, ui_state *ui)
{
  gchar **drop_filenames = gtk_selection_data_get_uris(data);
  if (drop_filenames == NULL)
  {
    const gchar *received_data = (gchar *)gtk_selection_data_get_text(data);
    if (received_data == NULL)
    {
      return;
    }

    drop_filenames = g_strsplit(received_data, "\n", 0);
  }

  GSList *filenames = NULL;

  gint current_index = 0;
  gchar *current_filename = drop_filenames[current_index];
  while (current_filename != NULL)
  {
    gchar *filename = NULL;
    if (strstr(current_filename, "file:") == current_filename)
    {
      filename = g_filename_from_uri(current_filename, NULL, NULL);
    }
    else
    {
      filename = g_strdup(current_filename);
    }

    remove_end_slash_n_r_from_filename(filename);

    if (file_exists(filename))
    {
      gchar *ext = strrchr(filename, '.');
      GString *ext_str = g_string_new(ext);
      g_string_ascii_up(ext_str);
      if ((strstr(ext_str->str, ".MP3") != NULL) ||
          (strstr(ext_str->str, ".OGG") != NULL) ||
          (strstr(ext_str->str, ".FLAC") != NULL))
      {
        filenames = g_slist_append(filenames, filename);
      }
      else if (!discard_data_files)
      {
        import_file(filename, ui);
      }
      g_string_free(ext_str, FALSE);
    }
    else if (directory_exists(filename) && !single_file_mode)
    {
      filenames = g_slist_append(filenames, filename);
    }
    else
    {
      g_free(filename);
    }

    current_index++;
    current_filename = drop_filenames[current_index];
  }
  g_strfreev(drop_filenames);

  if (filenames == NULL)
  {
    return;
  }

  if (g_slist_length(filenames) == 0)
  {
    g_slist_free(filenames);
    return;
  }

  if (discard_audio_files)
  {
    g_slist_foreach(filenames, (GFunc)g_free, NULL);
    g_slist_free(filenames);

    return;
  }

  if (single_file_mode)
  {
    GSList *last_filename = g_slist_last(filenames);
    import_file(last_filename->data, ui);

    g_slist_foreach(filenames, (GFunc)g_free, NULL);
    g_slist_free(filenames);
  }
  else
  {
    import_files_to_batch_and_free(filenames, ui);
  }
}

