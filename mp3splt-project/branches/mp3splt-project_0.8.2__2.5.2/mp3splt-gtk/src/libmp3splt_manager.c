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

#include "libmp3splt_manager.h"

static void lmanager_change_window_progress_bar(splt_progress *p_bar, void *data);
static void lmanager_put_message_from_library(const char *message, splt_message_type mess_type, void *data);

void lmanager_init_and_find_plugins(ui_state *ui)
{
  mp3splt_set_progress_function(ui->mp3splt_state, lmanager_change_window_progress_bar, ui);
  mp3splt_set_split_filename_function(ui->mp3splt_state, lmanager_put_split_filename, ui);
  mp3splt_set_message_function(ui->mp3splt_state, lmanager_put_message_from_library, ui);

  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_DEBUG_MODE, SPLT_FALSE);
  mp3splt_set_int_option(ui->mp3splt_state,
      SPLT_OPT_SET_FILE_FROM_CUE_IF_FILE_TAG_FOUND, SPLT_TRUE);

  gint error = mp3splt_find_plugins(ui->mp3splt_state);
  if (error < 0)
  {
    char *error_from_library = mp3splt_get_strerror(ui->mp3splt_state, error);
    if (error_from_library == NULL) { return; }
    ui_fail(ui, error_from_library);
  }
}

void lmanager_stop_split(ui_state *ui)
{
  gint err = mp3splt_stop_split(ui->mp3splt_state);
  print_status_bar_confirmation(err, ui);
}

static gboolean lmanager_put_split_filename_idle(ui_with_fname *ui_fname)
{
  char *filename = ui_fname->fname;
  ui_state *ui = ui_fname->ui;

  add_split_row(filename, ui);

  gint fname_status_size = (strlen(filename) + 255);
  gchar *fname_status = g_malloc(sizeof(char) * fname_status_size);
  g_snprintf(fname_status, fname_status_size, _(" File '%s' created"), filename);

  put_status_message(fname_status, ui);

  if (fname_status)
  {
    free(fname_status);
    fname_status = NULL;
  }

  gtk_widget_set_sensitive(ui->gui->queue_files_button, TRUE);
  gtk_widget_set_sensitive(ui->gui->remove_all_files_button, TRUE);

#ifdef __WIN32__
  while (gtk_events_pending())
  {
    gtk_main_iteration();
  }
  gdk_flush();
#endif

  if (filename)
  {
    g_free(filename);
  }
  g_free(ui_fname);

  return FALSE;
}

//! Add another file to the split_file tab
void lmanager_put_split_filename(const char *filename, void *data)
{
  ui_state *ui = (ui_state *)data;

  ui_with_fname *ui_fname = g_malloc0(sizeof(ui_with_fname));
  ui_fname->ui = ui;
  ui_fname->fname = NULL;
  if (filename)
  {
    ui_fname->fname = strdup(filename);
  }

  gdk_threads_add_idle_full(G_PRIORITY_HIGH_IDLE,
      (GSourceFunc)lmanager_put_split_filename_idle, ui_fname, NULL);
}

static gboolean lmanager_put_message_from_library_idle(ui_with_message *ui_message)
{
  splt_message_type mess_type = ui_message->mess_type;
  ui_state *ui = ui_message->ui;

  char *mess = ui_message->message;
  if (mess)
  {
    gint i = 0;
    //replace '\n' with ' '
    for (i = 0;i < strlen(mess);i++)
    {
      if (mess[i] == '\n')
      {
        mess[i] = ' ';
      }
    }

    put_status_message_with_type(mess, mess_type, ui);

#ifdef __WIN32__
    while (gtk_events_pending())
    {
      gtk_main_iteration();
    }
    gdk_flush();
#endif

    g_free(mess);
    mess = NULL;
  }

  g_free(ui_message);

  return FALSE;
}

//!prints a message from the library
static void lmanager_put_message_from_library(const char *message, splt_message_type mess_type, void *data)
{
  ui_state *ui = (ui_state *)data;

  ui_with_message *ui_message = g_malloc0(sizeof(ui_with_message));
  ui_message->ui = ui;
  ui_message->message = NULL;
  if (message)
  {
    ui_message->message = strdup(message);
  }
  ui_message->mess_type = mess_type;

  gdk_threads_add_idle_full(G_PRIORITY_HIGH_IDLE,
      (GSourceFunc)lmanager_put_message_from_library_idle, ui_message, NULL);
}

static gboolean lmanager_change_window_progress_bar_idle(ui_with_p_bar *ui_p_bar)
{
  ui_state *ui = ui_p_bar->ui;

  gchar progress_text[1024] = " ";

  switch (ui_p_bar->progress_type)
  {
    case SPLT_PROGRESS_PREPARE:
      g_snprintf(progress_text,1023, _(" preparing \"%s\" (%d of %d)"),
          ui_p_bar->filename_shorted,
          ui_p_bar->current_split,
          ui_p_bar->max_splits);
      break;
    case SPLT_PROGRESS_CREATE:
      g_snprintf(progress_text,1023, _(" creating \"%s\" (%d of %d)"),
          ui_p_bar->filename_shorted,
          ui_p_bar->current_split,
          ui_p_bar->max_splits);
      break;
    case SPLT_PROGRESS_SEARCH_SYNC:
      g_snprintf(progress_text,1023, _(" searching for sync errors..."));
      break;
    case SPLT_PROGRESS_SCAN_SILENCE:
      if (get_currently_scanning_for_silence_safe(ui))
      {
        g_snprintf(progress_text,1023, _("Computing amplitude wave data..."));
      }
      else
      {
        g_snprintf(progress_text,1023,
            _("S: %02d, Level: %.2f dB; scanning for silence..."),
            ui_p_bar->silence_found_tracks, ui_p_bar->silence_db_level);
      }
      break;
    default:
      g_snprintf(progress_text,1023, " ");
      break;
  }

  gchar printed_value[1024] = { '\0' };
  g_snprintf(printed_value, 1023, "%6.2f %% %s", ui_p_bar->percent_progress * 100, progress_text);

  gtk_progress_bar_set_fraction(ui->gui->percent_progress_bar, ui_p_bar->percent_progress);
  gtk_progress_bar_set_text(ui->gui->percent_progress_bar, printed_value);

#ifdef __WIN32__
  while (gtk_events_pending())
  {
    gtk_main_iteration();
  }
  gdk_flush();
#endif

  if (ui_p_bar->filename_shorted)
  {
    g_free(ui_p_bar->filename_shorted);
  }
  g_free(ui_p_bar);

  return FALSE;
}

//!Allows to set the value shown by the progress bar
static void lmanager_change_window_progress_bar(splt_progress *p_bar, void *data)
{
  ui_state *ui = (ui_state *) data;

  ui_with_p_bar *ui_p_bar = g_malloc0(sizeof(ui_with_p_bar));
  ui_p_bar->ui = ui;

  ui_p_bar->progress_type = mp3splt_progress_get_type(p_bar);
  ui_p_bar->filename_shorted = mp3splt_progress_get_filename_shorted(p_bar);
  ui_p_bar->current_split = mp3splt_progress_get_current_split(p_bar);
  ui_p_bar->max_splits = mp3splt_progress_get_max_splits(p_bar);
  ui_p_bar->silence_found_tracks = mp3splt_progress_get_silence_found_tracks(p_bar);
  ui_p_bar->silence_db_level = mp3splt_progress_get_silence_db_level(p_bar);
  ui_p_bar->percent_progress = mp3splt_progress_get_percent_progress(p_bar);

  gdk_threads_add_idle_full(G_PRIORITY_HIGH_IDLE,
      (GSourceFunc)lmanager_change_window_progress_bar_idle, ui_p_bar, NULL);
}

