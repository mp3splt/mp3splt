/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2010 Alexandru Munteanu
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

/*!********************************************************
 * \file 
 * The function that allows to export the current list of
 * splitpoints as a Cue sheet.
 *********************************************************/

#include "export.h"

//! Export the current split points into a cue file
static void export_to_cue_file(const gchar* filename, ui_state *ui)
{
  const gchar *old_fname = mp3splt_get_filename_to_split(ui->mp3splt_state);
  gchar *fname = NULL;
  if (old_fname != NULL) { fname = g_strdup(old_fname); }

  mp3splt_set_filename_to_split(ui->mp3splt_state, get_input_filename(ui->gui));

  gchar *directory = g_path_get_dirname(filename);
  mp3splt_set_path_of_split(ui->mp3splt_state, directory);
  g_free(directory);

  mp3splt_erase_all_splitpoints(ui->mp3splt_state);
  mp3splt_erase_all_tags(ui->mp3splt_state);

  put_splitpoints_and_tags_in_mp3splt_state(ui->mp3splt_state, ui);

  gchar *file = g_path_get_basename(filename);
  splt_code err = mp3splt_export(ui->mp3splt_state, CUE_EXPORT, file, SPLT_FALSE);
  print_status_bar_confirmation(err, ui);
  g_free(file);

  mp3splt_set_filename_to_split(ui->mp3splt_state, fname);
  if (fname != NULL) { g_free(fname); }
}

void export_cue_file_in_configuration_directory(ui_state *ui)
{
  if (ui->status->lock_cue_export) { return; }

  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_CUE_DISABLE_CUE_FILE_CREATED_MESSAGE,
      SPLT_TRUE);

  gchar *configuration_directory = get_configuration_directory();

  gsize filename_size = strlen(configuration_directory) + 20;
  gchar *splitpoints_cue_filename = g_malloc(filename_size * sizeof(gchar));
  g_snprintf(splitpoints_cue_filename, filename_size, "%s%s%s", configuration_directory,
      G_DIR_SEPARATOR_S, "splitpoints.cue");

  export_to_cue_file(splitpoints_cue_filename, ui);

  g_free(configuration_directory);
  g_free(splitpoints_cue_filename);

  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_CUE_DISABLE_CUE_FILE_CREATED_MESSAGE,
      SPLT_FALSE);
}

//! Choose the file to save the session to
void export_cue_file_event(GtkWidget *widget, ui_state *ui)
{
  GtkWidget *file_chooser = gtk_file_chooser_dialog_new(_("Cue filename to export"),
      NULL,
      GTK_FILE_CHOOSER_ACTION_SAVE,
      GTK_STOCK_CANCEL,
      GTK_RESPONSE_CANCEL,
      GTK_STOCK_SAVE,
      GTK_RESPONSE_ACCEPT,
      NULL);

  wh_set_browser_directory_handler(ui, file_chooser);

  GtkFileFilter *our_filter = gtk_file_filter_new();
  gtk_file_filter_set_name (our_filter, _("cue files (*.cue)"));
  gtk_file_filter_add_pattern(our_filter, "*.cue");
  gtk_file_filter_add_pattern(our_filter, "*.CUE");
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_chooser), our_filter);
  gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(file_chooser),TRUE);

  if (gtk_dialog_run(GTK_DIALOG(file_chooser)) == GTK_RESPONSE_ACCEPT)
  {
    gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));
    export_to_cue_file(filename, ui);
    g_free(filename);
  }

  gtk_widget_destroy(file_chooser);
}

