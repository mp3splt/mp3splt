/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2010 Alexandru Munteanu
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
 * The magic behind the splitpoint input
 *
 * All functions that are needed in order to read in
 * cddb, cue or similar files.
 *********************************************************/

#include <string.h>

#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include "player_tab.h"
#include "main_win.h"
#include "freedb_tab.h"
#include "import.h"
#include "options_manager.h"
#include "mp3splt-gtk.h"
#include "utilities.h"
#include "ui_manager.h"
#include "widgets_helper.h"

extern splt_state *the_state;
extern ui_state *ui;

static void set_import_filters(GtkFileChooser *chooser);
static void build_import_filter(GtkFileChooser *chooser,
    const gchar *filter_name, const gchar *filter_pattern,
    const gchar *filter_pattern_upper, 
    GList **filters, GtkFileFilter *all_filter);
static gpointer add_audacity_labels_splitpoints(gpointer data);
static gpointer add_cddb_splitpoints(gpointer data);
static gpointer add_cue_splitpoints(gpointer data);

//! What happens if the "Import" button is pressed
void import_event(GtkWidget *widget, gpointer *data)
{
  GtkWidget *file_chooser =
    gtk_file_chooser_dialog_new(_("Choose file to import"),
        NULL,
        GTK_FILE_CHOOSER_ACTION_OPEN,
        GTK_STOCK_CANCEL,
        GTK_RESPONSE_CANCEL,
        GTK_STOCK_OPEN,
        GTK_RESPONSE_ACCEPT,
        NULL);

  wh_set_browser_directory_handler(ui, file_chooser);

  set_import_filters(GTK_FILE_CHOOSER(file_chooser));

  if (gtk_dialog_run(GTK_DIALOG(file_chooser)) == GTK_RESPONSE_ACCEPT)
  {
    gchar *filename =
      gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));
 
    handle_import(filename);

    g_free(filename);
    filename = NULL;

    remove_status_message();
  }
 
  gtk_widget_destroy(file_chooser);
}

/*! Handles the import of an input file (audio or splitpoint)
 
  The file type is determined by the extension of the file.
 */
void handle_import(gchar *filename)
{
  if (filename == NULL)
  {
    return;
  }

  gchar *ext = strrchr(filename, '.');
  GString *ext_str = g_string_new(ext);

  g_string_ascii_up(ext_str);

  if ((strstr(ext_str->str, ".MP3") != NULL) ||
      (strstr(ext_str->str, ".OGG") != NULL))
  {
    file_chooser_ok_event(filename);
    remove_status_message();
  }
  else if ((strstr(ext_str->str, ".CUE") != NULL))
  {
    update_output_options();
    create_thread(add_cue_splitpoints, strdup(filename), TRUE, NULL);
  }
  else if ((strstr(ext_str->str, ".CDDB") != NULL))
  {
    update_output_options();
    create_thread(add_cddb_splitpoints, strdup(filename), TRUE, NULL);
  }
  else if ((strstr(ext_str->str, ".TXT") != NULL))
  {
    create_thread(add_audacity_labels_splitpoints, strdup(filename), TRUE, NULL);
  }

  if (ext_str)
  {
    g_string_free(ext_str, FALSE);
  }
}

//! Set the file chooser filters to "splitpoint file"
static void set_import_filters(GtkFileChooser *chooser)
{
  GtkFileFilter *all_filter = gtk_file_filter_new();
  gtk_file_filter_set_name(GTK_FILE_FILTER(all_filter),
      _("CDDB (*.cddb), CUE (*.cue), Audacity labels (*.txt)"));

  GList *filters = NULL;

  build_import_filter(chooser, _("CDDB files (*.cddb)"), "*.cddb", "*.CDDB", 
      &filters, all_filter);
  build_import_filter(chooser, _("CUE files (*.cue)"), "*.cue", "*.CUE",
      &filters, all_filter);
  build_import_filter(chooser, _("Audacity labels files (*.txt)"), "*.txt", "*.TXT",
      &filters, all_filter);
  build_import_filter(chooser, _("All files"), "*", NULL, &filters, NULL);

  gtk_file_chooser_add_filter(chooser, all_filter);

  GList *iter = NULL;
  for (iter = filters; iter != NULL; iter = g_list_next(iter))
  {
    gtk_file_chooser_add_filter(chooser, iter->data);
  }
}

static void build_import_filter(GtkFileChooser *chooser,
    const gchar *filter_name, const gchar *filter_pattern,
    const gchar *filter_pattern_upper,
    GList **filters, GtkFileFilter *all_filter)
{
  GtkFileFilter *filter = gtk_file_filter_new();
  gtk_file_filter_set_name(GTK_FILE_FILTER(filter), filter_name);

  gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter), filter_pattern);

  if (filter_pattern_upper)
  {
    gtk_file_filter_add_pattern(GTK_FILE_FILTER(filter), filter_pattern_upper);
  }

  if (all_filter)
  {
    gtk_file_filter_add_pattern(GTK_FILE_FILTER(all_filter), filter_pattern);
    if (filter_pattern_upper)
    {
      gtk_file_filter_add_pattern(GTK_FILE_FILTER(all_filter), filter_pattern_upper);
    }
  }

  *filters = g_list_append(*filters, filter);
}

/*! Add splitpoints from audacity

data pointer will be freed by g_free() after doung this.
*/
static gpointer add_audacity_labels_splitpoints(gpointer data)
{
  gchar *filename = data;

  gint err = SPLT_OK;
  mp3splt_put_audacity_labels_splitpoints_from_file(the_state, filename, &err);
 
  enter_threads();
 
  if (err >= 0)
  {
    update_splitpoints_from_the_state();
  }
 
  print_status_bar_confirmation(err);
 
  exit_threads();

  if (filename)
  {
    g_free(filename);
    filename = NULL;
  }

  return NULL;
}

//! Add splitpoints from cddb
static gpointer add_cddb_splitpoints(gpointer data)
{
  gchar *filename = data;

  gint err = SPLT_OK;
  mp3splt_put_cddb_splitpoints_from_file(the_state, filename, &err);

  enter_threads();
 
  if (err >= 0)
  {
    update_splitpoints_from_the_state();
  }
  print_status_bar_confirmation(err);

  exit_threads();

  if (filename)
  {
    g_free(filename);
    filename = NULL;
  }

  return NULL;
}

//! Add splitpoints from cue file
static gpointer add_cue_splitpoints(gpointer data)
{
  gchar *filename = data;

  gint err = SPLT_OK;
  mp3splt_set_filename_to_split(the_state, NULL);
  mp3splt_put_cue_splitpoints_from_file(the_state, filename, &err);
 
  enter_threads();
 
  if (err >= 0)
  {
    update_splitpoints_from_the_state();
  }
  print_status_bar_confirmation(err);

  // The cue file has provided libmp3splt with a input filename.
  // But since we use the filename from the gui instead we need to set
  // the value the gui uses, too, which we do in the next line.
  char *filename_to_split = mp3splt_get_filename_to_split(the_state);
  if (is_filee(filename_to_split))
  {
    inputfilename_set(filename_to_split);
  }
  
  exit_threads();
  enable_player_buttons();

  return NULL;
}

