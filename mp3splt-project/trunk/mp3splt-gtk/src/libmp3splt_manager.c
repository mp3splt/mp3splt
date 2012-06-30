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
  gint err = SPLT_OK;
  mp3splt_stop_split(ui->mp3splt_state, &err);
  print_status_bar_confirmation(err, ui);
}

//! Add another file to the split_file tab
void lmanager_put_split_filename(const char *filename, int progress_data, void *data)
{
  ui_state *ui = (ui_state *)data;

  enter_threads();

  gtk_widget_set_sensitive(ui->gui->queue_files_button, TRUE);
  gtk_widget_set_sensitive(ui->gui->remove_all_files_button,TRUE);

  add_split_row(filename, ui);
  ui->infos->split_files++;

  gint fname_status_size = (strlen(filename) + 255);
  gchar *fname_status = g_malloc(sizeof(char) * fname_status_size);
  g_snprintf(fname_status, fname_status_size, _(" File '%s' created"), filename);
  put_status_message(fname_status, ui);
  if (fname_status)
  {
    free(fname_status);
    fname_status = NULL;
  }

#ifdef __WIN32__
  while (gtk_events_pending())
  {
	  gtk_main_iteration();
  }
  gdk_flush();
#endif

  exit_threads();
}

//!prints a message from the library
static void lmanager_put_message_from_library(const char *message, splt_message_type mess_type, 
    void *data)
{
  ui_state *ui = (ui_state *) data;

  gchar *mess = g_strdup(message);
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
    enter_threads();

    put_status_message_with_type(mess, mess_type, ui);

#ifdef __WIN32__
  while (gtk_events_pending())
  {
	  gtk_main_iteration();
  }
  gdk_flush();
#endif

    exit_threads();
    g_free(mess);
    mess = NULL;
  }
}

//!Allows to set the value shown by the progress bar
static void lmanager_change_window_progress_bar(splt_progress *p_bar, void *data)
{
  ui_state *ui = (ui_state *) data;

  gchar progress_text[1024] = " ";

  switch (p_bar->progress_type)
  {
    case SPLT_PROGRESS_PREPARE:
      g_snprintf(progress_text,1023, _(" preparing \"%s\" (%d of %d)"),
          p_bar->filename_shorted,
          p_bar->current_split,
          p_bar->max_splits);
      break;
    case SPLT_PROGRESS_CREATE:
      g_snprintf(progress_text,1023, _(" creating \"%s\" (%d of %d)"),
          p_bar->filename_shorted,
          p_bar->current_split,
          p_bar->max_splits);
      break;
    case SPLT_PROGRESS_SEARCH_SYNC:
      g_snprintf(progress_text,1023, _(" searching for sync errors..."));
      break;
    case SPLT_PROGRESS_SCAN_SILENCE:
      if (ui->status->currently_scanning_for_silence)
      {
        g_snprintf(progress_text,1023, _("Computing amplitude wave data..."));
      }
      else
      {
        g_snprintf(progress_text,1023,
            _("S: %02d, Level: %.2f dB; scanning for silence..."),
            p_bar->silence_found_tracks, p_bar->silence_db_level);
      }
      break;
    default:
      g_snprintf(progress_text,1023, " ");
      break;
  }

  gchar printed_value[1024] = { '\0' };

  enter_threads();

  gtk_progress_bar_set_fraction(ui->gui->percent_progress_bar, p_bar->percent_progress);
  g_snprintf(printed_value, 1023, "%6.2f %% %s", p_bar->percent_progress * 100, progress_text);
  gtk_progress_bar_set_text(ui->gui->percent_progress_bar, printed_value);

#ifdef __WIN32__
  while (gtk_events_pending())
  {
	  gtk_main_iteration();
  }
  gdk_flush();
#endif

  exit_threads();
}

