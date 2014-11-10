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
 * The magic behind the splitpoint input
 *
 * All functions that are needed in order to read in
 * cddb, cue or similar files.
 *********************************************************/

#include "import.h"

static void set_import_filters(GtkFileChooser *chooser);
static void build_import_filter(GtkFileChooser *chooser,
    const gchar *filter_name, const gchar *filter_pattern,
    const gchar *filter_pattern_upper, 
    GList **filters, GtkFileFilter *all_filter);
static gpointer add_audacity_labels_splitpoints(ui_with_fname *ui_fname);
static gpointer add_cddb_splitpoints(ui_with_fname *ui_fname);
static gpointer add_cue_splitpoints(ui_with_fname *ui_fname);
static gpointer add_plugin_internal_cue_splitpoints(ui_with_fname *ui_fname);

//! What happens if the "Import" button is pressed
void import_event(GtkWidget *widget, ui_state *ui)
{
  GtkWidget *file_chooser =
    gtk_file_chooser_dialog_new(_("Choose file to import"),
        NULL,
        GTK_FILE_CHOOSER_ACTION_OPEN,
        _("_Cancel"),
        GTK_RESPONSE_CANCEL,
        _("_Open"),
        GTK_RESPONSE_ACCEPT,
        NULL);

  wh_set_browser_directory_handler(ui, file_chooser);
  set_import_filters(GTK_FILE_CHOOSER(file_chooser));

  if (gtk_dialog_run(GTK_DIALOG(file_chooser)) == GTK_RESPONSE_ACCEPT)
  {
    gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));
 
    import_file(filename, ui, TRUE);

    g_free(filename);
    filename = NULL;

    remove_status_message(ui->gui);
  }
 
  gtk_widget_destroy(file_chooser);
}

static ui_with_fname *create_ui_with_fname(ui_state *ui, const gchar *filename)
{
  ui_with_fname *ui_fname = g_malloc0(sizeof(ui_with_fname));
  ui_fname->ui = ui;
  ui_fname->fname = strdup(filename);
  ui_fname->is_checked_output_radio_box = get_checked_output_radio_box(ui);
  const gchar *output_format = gtk_entry_get_text(GTK_ENTRY(ui->gui->output_entry));
  if (output_format != NULL)
  {
    ui_fname->output_format = g_strdup(output_format);
  }

  return ui_fname;
}

/*! Handles the import of an input file (audio or splitpoint)
 
  The file type is determined by the extension of the file.
 */
void import_file(gchar *filename, ui_state *ui, gboolean force_import_cue)
{
  if (filename == NULL)
  {
    return;
  }

  gchar *ext = strrchr(filename, '.');
  GString *ext_str = g_string_new(ext);

  g_string_ascii_up(ext_str);

  if (!force_import_cue &&
      ((strstr(ext_str->str, ".MP3") != NULL) ||
       (strstr(ext_str->str, ".OGG") != NULL) ||
       (strstr(ext_str->str, ".FLAC") != NULL)))
  {
    file_chooser_ok_event(filename, ui);
    remove_status_message(ui->gui);
  }
  else if ((strstr(ext_str->str, ".CUE") != NULL))
  {
    ui_with_fname *ui_fname = create_ui_with_fname(ui, filename);
    create_thread_and_unref((GThreadFunc)add_cue_splitpoints,
        (gpointer) ui_fname, ui, "import_cue");
  }
  else if ((strstr(ext_str->str, ".CDDB") != NULL))
  {
    ui_with_fname *ui_fname = create_ui_with_fname(ui, filename);
    create_thread_and_unref((GThreadFunc)add_cddb_splitpoints,
        (gpointer) ui_fname, ui, "import_cddb");
  }
  else if ((strstr(ext_str->str, ".TXT") != NULL))
  {
    ui_with_fname *ui_fname = create_ui_with_fname(ui, filename);
    create_thread_and_unref((GThreadFunc)add_audacity_labels_splitpoints,
        (gpointer) ui_fname, ui, "import_audacity");
  }
  else
  {
    ui_with_fname *ui_fname = create_ui_with_fname(ui, filename);
    create_thread_and_unref((GThreadFunc)add_plugin_internal_cue_splitpoints,
        (gpointer) ui_fname, ui, "import_internal");
  }

  if (ext_str)
  {
    g_string_free(ext_str, FALSE);
  }
}

void import_cue_file_from_the_configuration_directory(ui_state *ui)
{
  gchar *configuration_directory = get_configuration_directory();

  gsize filename_size = strlen(configuration_directory) + 20;
  gchar *splitpoints_cue_filename = g_malloc(filename_size * sizeof(gchar));
  g_snprintf(splitpoints_cue_filename, filename_size, "%s%s%s", configuration_directory,
      G_DIR_SEPARATOR_S, "splitpoints.cue");

  if (file_exists(splitpoints_cue_filename))
  {
    ui->importing_cue_from_configuration_directory = TRUE;

    mp3splt_set_int_option(ui->mp3splt_state,
        SPLT_OPT_CUE_SET_SPLITPOINT_NAMES_FROM_REM_NAME, SPLT_TRUE); 
    import_file(splitpoints_cue_filename, ui, FALSE);
  }

  g_free(configuration_directory);
  g_free(splitpoints_cue_filename);
}

static gboolean import_files_to_batch(ui_with_fnames *ui_wf)
{
  ui_state *ui = ui_wf->ui;
  char **splt_filenames = ui_wf->filenames;

  gint i = 0;
  for (i = 0;i < ui_wf->num_of_filenames;i++)
  {
    if (!splt_filenames[i])
    {
      continue;
    }

    multiple_files_add_filename(splt_filenames[i], ui);

    free(splt_filenames[i]);
    splt_filenames[i] = NULL;
  }

  free(splt_filenames);
  splt_filenames = NULL;

  g_free(ui_wf);

  return FALSE;
}

static gboolean import_files_to_batch_end(ui_state *ui)
{
  if (ui->infos->multiple_files_tree_number > 0)
  {
    gtk_widget_set_sensitive(ui->gui->multiple_files_remove_all_files_button, TRUE);
  }

  set_process_in_progress_and_wait_safe(FALSE, ui);

  return FALSE;
}

static gpointer import_files_to_batch_thread(ui_with_list *ui_wl)
{
  ui_state *ui = ui_wl->ui;

  set_process_in_progress_and_wait_safe(TRUE, ui);

  GSList *current_file = ui_wl->list;
  while (current_file)
  {
    gchar *filename = current_file->data;

    int err = SPLT_OK;
    int num_of_files_found = 0;

    char **splt_filenames =
      mp3splt_find_filenames(ui->mp3splt_state, filename, &num_of_files_found, &err);

    if (splt_filenames)
    {
      ui_with_fnames *ui_wf = g_malloc0(sizeof(ui_with_fnames));
      ui_wf->ui = ui;
      ui_wf->filenames = splt_filenames;
      ui_wf->num_of_filenames = num_of_files_found;

      add_idle(G_PRIORITY_HIGH_IDLE, (GSourceFunc)import_files_to_batch, ui_wf, NULL);
    }

    g_free(filename);
    filename = NULL;

    current_file = g_slist_next(current_file);
  }

  g_slist_free(ui_wl->list);

  g_free(ui_wl);

  add_idle(G_PRIORITY_HIGH_IDLE, (GSourceFunc)import_files_to_batch_end, ui, NULL);

  return NULL;
}

void import_files_to_batch_and_free(GSList *files, ui_state *ui)
{
  ui_with_list *ui_wl = g_malloc0(sizeof(ui_with_list));
  ui_wl->ui = ui;
  ui_wl->list = files;

  create_thread_and_unref((GThreadFunc)import_files_to_batch_thread,
      (gpointer) ui_wl, ui, "import_to_batch");
}

//! Set the file chooser filters to "splitpoint file"
static void set_import_filters(GtkFileChooser *chooser)
{
  GtkFileFilter *all_filter = gtk_file_filter_new();
  gtk_file_filter_set_name(GTK_FILE_FILTER(all_filter),
      _("CDDB (*.cddb), CUE (*.cue), Audacity labels (*.txt), internal sheet (*.flac), chapters (*.mp3)"));

  GList *filters = NULL;

  build_import_filter(chooser, _("CDDB files (*.cddb)"), "*.cddb", "*.CDDB", 
      &filters, all_filter);
  build_import_filter(chooser, _("CUE files (*.cue)"), "*.cue", "*.CUE",
      &filters, all_filter);
  build_import_filter(chooser, _("Audacity labels files (*.txt)"), "*.txt", "*.TXT",
      &filters, all_filter);
  build_import_filter(chooser, _("FLAC internal sheet (*.flac)"), "*.flac", "*.FLAC",
      &filters, all_filter);
  build_import_filter(chooser, _("ID3v2 chapters (*.mp3)"), "*.mp3", "*.MP3",
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

static gboolean add_audacity_labels_splitpoints_end(ui_with_err *ui_err)
{
  ui_state *ui = ui_err->ui;
  gint err = ui_err->err;

  if (err >= 0)
  {
    update_splitpoints_from_mp3splt_state(ui);
  }

  print_status_bar_confirmation(err, ui);

  set_process_in_progress_and_wait_safe(FALSE, ui_err->ui);

  g_free(ui_err);

  return FALSE;
}

static gpointer add_audacity_labels_splitpoints(ui_with_fname *ui_fname)
{
  ui_state *ui = ui_fname->ui;

  set_process_in_progress_and_wait_safe(TRUE, ui);

  gchar *filename = ui_fname->fname;
  if (ui_fname->output_format) { g_free(ui_fname->output_format); }
  g_free(ui_fname);

  gint err = mp3splt_import(ui->mp3splt_state, AUDACITY_LABELS_IMPORT, filename);
  g_free(filename);

  ui_with_err *ui_err = g_malloc0(sizeof(ui_with_err));
  ui_err->ui = ui;
  ui_err->err = err;

  add_idle(G_PRIORITY_HIGH_IDLE, (GSourceFunc)add_audacity_labels_splitpoints_end,
      ui_err, NULL);

  return NULL;
}

static gboolean add_plugin_internal_cue_splitpoints_end(ui_with_err *ui_err)
{
  ui_state *ui = ui_err->ui;
  gint err = ui_err->err;

  if (err >= 0)
  {
    update_splitpoints_from_mp3splt_state(ui);
  }

  print_status_bar_confirmation(err, ui);

  set_process_in_progress_and_wait_safe(FALSE, ui_err->ui);

  g_free(ui_err);

  return FALSE;
}

static gpointer add_plugin_internal_cue_splitpoints(ui_with_fname *ui_fname)
{
  ui_state *ui = ui_fname->ui;

  set_process_in_progress_and_wait_safe(TRUE, ui);

  update_output_options(ui, ui_fname->is_checked_output_radio_box, ui_fname->output_format);

  gchar *filename = ui_fname->fname;
  if (ui_fname->output_format) { g_free(ui_fname->output_format); }
  g_free(ui_fname);

  gint err = mp3splt_import(ui->mp3splt_state, PLUGIN_INTERNAL_IMPORT, filename);
  g_free(filename);

  ui_with_err *ui_err = g_malloc0(sizeof(ui_with_err));
  ui_err->ui = ui;
  ui_err->err = err;

  add_idle(G_PRIORITY_HIGH_IDLE, (GSourceFunc)add_plugin_internal_cue_splitpoints_end,
      ui_err, NULL);

  return NULL;
}

static gboolean add_cddb_splitpoints_end(ui_with_err *ui_err)
{
  ui_state *ui = ui_err->ui;
  gint err = ui_err->err;

  if (err >= 0)
  {
    update_splitpoints_from_mp3splt_state(ui);
  }

  print_status_bar_confirmation(err, ui);

  set_process_in_progress_and_wait_safe(FALSE, ui);

  g_free(ui_err);

  return FALSE;
}

//! Add splitpoints from cddb
static gpointer add_cddb_splitpoints(ui_with_fname *ui_fname)
{
  ui_state *ui = ui_fname->ui;

  set_process_in_progress_and_wait_safe(TRUE, ui);

  update_output_options(ui, ui_fname->is_checked_output_radio_box, ui_fname->output_format);

  gchar *filename = ui_fname->fname;
  if (ui_fname->output_format) { g_free(ui_fname->output_format); }
  g_free(ui_fname);

  gint err = mp3splt_import(ui->mp3splt_state, CDDB_IMPORT, filename);
  g_free(filename);

  ui_with_err *ui_err = g_malloc0(sizeof(ui_with_err));
  ui_err->ui = ui;
  ui_err->err = err;

  add_idle(G_PRIORITY_HIGH_IDLE, (GSourceFunc)add_cddb_splitpoints_end,
      ui_err, NULL);

  return NULL;
}

static gboolean add_cue_splitpoints_end(ui_with_err *ui_err)
{
  ui_state *ui = ui_err->ui;

  if (!ui->importing_cue_from_configuration_directory)
  {
    long max_point = 0;

    splt_code err = SPLT_OK;
    splt_points *points = mp3splt_get_splitpoints(ui->mp3splt_state, &err);
    print_status_bar_confirmation(err, ui);
    if (points != NULL)
    {
      mp3splt_points_init_iterator(points);
      const splt_point *point = NULL;
      while ((point = mp3splt_points_next(points)))
      {
        long point_value = mp3splt_point_get_value(point);
        if (point_value > max_point) { max_point = point_value; }
      }
    }

    splt_point *splitpoint = mp3splt_point_new(max_point + (100 * 60 * 100), NULL);
    mp3splt_point_set_name(splitpoint, _("--- last cue splitpoint ---"));
    mp3splt_append_splitpoint(ui->mp3splt_state, splitpoint);
  }
  else
  {
    ui->importing_cue_from_configuration_directory = FALSE;
  }

  if (ui_err->err >= 0)
  {
    update_splitpoints_from_mp3splt_state(ui);
  }
  print_status_bar_confirmation(ui_err->err, ui);

  mp3splt_set_int_option(ui->mp3splt_state,
      SPLT_OPT_CUE_SET_SPLITPOINT_NAMES_FROM_REM_NAME, SPLT_FALSE);

  //The cue file has provided libmp3splt with a input filename.
  //But since we use the filename from the gui instead we need to set
  //the value the gui uses, too, which we do in the next line.
  const gchar *filename_to_split = mp3splt_get_filename_to_split(ui->mp3splt_state);

  if (file_exists(filename_to_split))
  {
    file_chooser_ok_event(filename_to_split, ui);
  }

  set_process_in_progress_and_wait_safe(FALSE, ui_err->ui);

  g_free(ui_err);

  return FALSE;
}

//! Add splitpoints from cue file
static gpointer add_cue_splitpoints(ui_with_fname *ui_fname)
{
  ui_state *ui = ui_fname->ui;

  set_process_in_progress_and_wait_safe(TRUE, ui);

  update_output_options(ui, ui_fname->is_checked_output_radio_box, ui_fname->output_format);

  gchar *filename = ui_fname->fname;
  if (ui_fname->output_format) { g_free(ui_fname->output_format); }
  g_free(ui_fname);

  mp3splt_set_filename_to_split(ui->mp3splt_state, NULL);

  gint err = mp3splt_import(ui->mp3splt_state, CUE_IMPORT, filename);
  g_free(filename);

  ui_with_err *ui_err = g_malloc0(sizeof(ui_with_err));
  ui_err->ui = ui;
  ui_err->err = err;

  add_idle(G_PRIORITY_HIGH_IDLE, (GSourceFunc)add_cue_splitpoints_end,
      ui_err, NULL);

  return NULL;
}

