/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2014 Alexandru Munteanu
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
 * The main file,
 *
 * this file contains the main() function as well as some 
 * globally used functions.
 *********************************************************/

#include "mp3splt-gtk.h"

#ifndef __WIN32__
  #include <langinfo.h>
#endif

ui_state *ui;

static gpointer split_collected_files(ui_for_split *ui_fs);
static gboolean collect_files_to_split(ui_state *ui);

void split_action(ui_state *ui)
{
  set_is_splitting_safe(TRUE, ui);
  if (!collect_files_to_split(ui))
  {
    set_is_splitting_safe(FALSE, ui);
    return;
  }

  gtk_widget_set_sensitive(ui->gui->cancel_button, TRUE);
  remove_all_split_rows(ui);

  ui_for_split *ui_fs = build_ui_for_split(ui);
  ui_fs->pat = get_splitpoints_and_tags_for_mp3splt_state(ui);

  create_thread_and_unref((GThreadFunc)split_collected_files,
      (gpointer) ui_fs, ui, "split");
}

static gboolean collect_files_to_split(ui_state *ui)
{
  //clean
  GPtrArray *files_to_split = ui->files_to_split;
  if (files_to_split && files_to_split->len > 0)
  {
    gint length = files_to_split->len;
    gint i = 0;
    for (i = 0;i < length;i++)
    {
      g_free(g_ptr_array_index(files_to_split, i));
    }
    g_ptr_array_free(ui->files_to_split, TRUE);
  }
  ui->files_to_split = g_ptr_array_new();

  //collect
  if (get_split_file_mode(ui) == FILE_MODE_SINGLE)
  {
    g_ptr_array_add(ui->files_to_split, g_strdup(get_input_filename(ui->gui)));
  }
  else if (ui->infos->multiple_files_tree_number > 0)
  {
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(ui->gui->multiple_files_tree));
    gint row_number = 0;
    while (row_number < ui->infos->multiple_files_tree_number)
    {
      GtkTreePath *path = gtk_tree_path_new_from_indices(row_number ,-1);

      GtkTreeIter iter;
      gtk_tree_model_get_iter(model, &iter, path);

      gchar *filename = NULL;
      gtk_tree_model_get(model, &iter, MULTIPLE_COL_FILENAME, &filename, -1);

      g_ptr_array_add(ui->files_to_split, filename);

      row_number++;
    }
  }
  else
  {
    put_status_message(_(" error: no files found in batch mode"), ui);
    return FALSE;
  }

  return TRUE;
}

static gboolean split_collected_files_end(ui_with_err *ui_err)
{
  gint err = ui_err->err;
  ui_state *ui = ui_err->ui;

  gtk_widget_set_sensitive(ui->gui->cancel_button, FALSE);

  if (err >= 0)
  {
    gtk_progress_bar_set_fraction(ui->gui->percent_progress_bar, 1.0);
    gtk_progress_bar_set_text(ui->gui->percent_progress_bar, _(" finished"));
  }

  set_is_splitting_safe(FALSE, ui);

  set_process_in_progress_and_wait_safe(FALSE, ui_err->ui);

  g_free(ui_err);

  return FALSE;
}

static gint get_stop_split_safe(ui_state *ui)
{
  lock_mutex(&ui->variables_mutex);
  gint stop_split = ui->status->stop_split;
  unlock_mutex(&ui->variables_mutex);
  return stop_split;
}

//! Split the file
static gpointer split_collected_files(ui_for_split *ui_fs)
{
  ui_state *ui = ui_fs->ui;

  set_process_in_progress_and_wait_safe(TRUE, ui);

  put_options_from_preferences(ui_fs);

  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_OUTPUT_FILENAMES, SPLT_OUTPUT_DEFAULT);
  if (!ui_fs->is_checked_output_radio_box)
  {
    mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_OUTPUT_FILENAMES, SPLT_OUTPUT_FORMAT);
  }

  mp3splt_set_path_of_split(ui->mp3splt_state, ui_fs->output_directory);

  gint err = mp3splt_erase_all_splitpoints(ui->mp3splt_state);
  err = mp3splt_erase_all_tags(ui->mp3splt_state);

  gint split_mode = mp3splt_get_int_option(ui->mp3splt_state, SPLT_OPT_SPLIT_MODE, &err);
  print_status_bar_confirmation_in_idle(err, ui);

  err = mp3splt_set_oformat(ui->mp3splt_state, ui_fs->output_format);

  if (mp3splt_get_int_option(ui->mp3splt_state, SPLT_OPT_SPLIT_MODE, &err) == SPLT_OPTION_NORMAL_MODE &&
      ui_fs->split_file_mode == FILE_MODE_SINGLE)
  {
    mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_OUTPUT_FILENAMES, SPLT_OUTPUT_CUSTOM);
  }

  if (split_mode == SPLT_OPTION_NORMAL_MODE)
  {
    gint i = 0;
    for (i = 0;i < ui_fs->pat->splitpoints->len; i++)
    {
      splt_point *point = g_ptr_array_index(ui_fs->pat->splitpoints, i);
      mp3splt_append_splitpoint(ui->mp3splt_state, point);
      splt_tags *tags = g_ptr_array_index(ui_fs->pat->tags, i);
      mp3splt_append_tags(ui->mp3splt_state, tags);
    }

    err = mp3splt_remove_tags_of_skippoints(ui->mp3splt_state);
    print_status_bar_confirmation_in_idle(err, ui);
  }

  gint output_filenames = mp3splt_get_int_option(ui->mp3splt_state, SPLT_OPT_OUTPUT_FILENAMES, &err);

  gint selected_split_mode = ui_fs->selected_split_mode;
  gboolean is_single_file_mode = FALSE;
  if (ui_fs->split_file_mode == FILE_MODE_SINGLE)
  {
    is_single_file_mode = TRUE;
  }

  //files_to_split will not have a read/write issue because the 'splitting' boolean, which does not
  //allow us to modify it while we read it here - no mutex needed
  GPtrArray *files_to_split = ui->files_to_split;
  gint length = files_to_split->len;
  gint i = 0;
  for (i = 0;i < length;i++)
  {
    gchar *filename = g_ptr_array_index(files_to_split, i);

    print_processing_file(filename, ui);

    mp3splt_set_filename_to_split(ui->mp3splt_state, filename);

    gint err = SPLT_OK;

    if (!is_single_file_mode)
    {
      if (selected_split_mode == SELECTED_SPLIT_INTERNAL_SHEET)
      {
        err = mp3splt_import(ui->mp3splt_state, PLUGIN_INTERNAL_IMPORT, filename);
        print_status_bar_confirmation_in_idle(err, ui);
        if (err < 0) { continue; }
      }
      else if ((selected_split_mode == SELECTED_SPLIT_CUE_FILE) ||
          (selected_split_mode == SELECTED_SPLIT_CDDB_FILE))
      {
        gchar *cue_or_cddb = g_strdup(filename);
        gchar *last_ext = g_strrstr(cue_or_cddb, ".");
        *last_ext = '\0';

        GString *cue_or_cddb_file = g_string_new(cue_or_cddb);
        g_free(cue_or_cddb);

        if (selected_split_mode == SELECTED_SPLIT_CUE_FILE)
        {
          g_string_append(cue_or_cddb_file, ".cue");
          err = mp3splt_import(ui->mp3splt_state, CUE_IMPORT, cue_or_cddb_file->str);
        }
        else
        {
          g_string_append(cue_or_cddb_file, ".cddb");
          err = mp3splt_import(ui->mp3splt_state, CDDB_IMPORT, cue_or_cddb_file->str);
        }

        print_status_bar_confirmation_in_idle(err, ui);
        g_string_free(cue_or_cddb_file, SPLT_TRUE);
        if (err < 0) { continue; }
      }
    }

    err = mp3splt_split(ui->mp3splt_state);
    print_status_bar_confirmation_in_idle(err, ui);

    err = mp3splt_erase_all_tags(ui->mp3splt_state);
    print_status_bar_confirmation_in_idle(err, ui);

    err = mp3splt_erase_all_splitpoints(ui->mp3splt_state);
    print_status_bar_confirmation_in_idle(err, ui);

    if (get_stop_split_safe(ui))
    {
      set_stop_split_safe(FALSE, ui);
      break;
    }
  }

  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_OUTPUT_FILENAMES, output_filenames);

  free_ui_for_split(ui_fs);

  ui_with_err *ui_err = g_malloc0(sizeof(ui_with_err));
  ui_err->err = err;
  ui_err->ui = ui;

  add_idle(G_PRIORITY_HIGH_IDLE, (GSourceFunc)split_collected_files_end, ui_err, NULL);

  return NULL;
}

static gpointer thread_wrapper_function(gpointer data)
{
  ui_with_data *ui_wd = (ui_with_data *) data;
  ui_state *ui = ui_wd->ui;

  set_process_in_progress_and_wait_safe(TRUE, ui);

  //some general options
  if (ui_wd->filename_to_split != NULL)
  {
    mp3splt_set_filename_to_split(ui->mp3splt_state, ui_wd->filename_to_split);
  }

  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_DEBUG_MODE, ui->infos->debug_is_active);
  if (ui_wd->is_checked_output_radio_box)
  {
    mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_OUTPUT_FILENAMES, SPLT_OUTPUT_FORMAT);
  }
  else
  {
    mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_OUTPUT_FILENAMES, SPLT_OUTPUT_DEFAULT);
  }

  set_process_in_progress_and_wait_safe(FALSE, ui);

  gpointer returned_value = ui_wd->thread(ui_wd->data);

  if (ui_wd->filename_to_split) { g_free(ui_wd->filename_to_split); }
  g_free(ui_wd);

  return returned_value;
}

GThread *create_thread(GThreadFunc func, gpointer data, ui_state *ui, const char *name)
{
  ui_with_data *ui_wd = g_malloc0(sizeof(ui_with_data));
  ui_wd->ui = ui;
  ui_wd->data = data;
  ui_wd->thread = func;
  ui_wd->is_checked_output_radio_box = get_checked_output_radio_box(ui);
  gchar *input_filename = get_input_filename(ui->gui);
  if (input_filename != NULL)
  {
    ui_wd->filename_to_split = g_strdup(input_filename);
  }
  return g_thread_new(name, thread_wrapper_function, ui_wd);
}

void create_thread_and_unref(GThreadFunc func, gpointer data, ui_state *ui, const char *name)
{
  g_thread_unref(create_thread(func, data, ui, name));
}

void add_idle(gint priority, GSourceFunc function, gpointer data, GDestroyNotify notify)
{
  gdk_threads_add_idle_full(priority, function, data, notify);
}

gboolean exit_application(GtkWidget *widget, GdkEvent  *event, gpointer data)
{
  ui_state *ui = (ui_state *)data;

  ui_save_preferences(NULL, ui);

  if (get_is_splitting_safe(ui))
  {
    lmanager_stop_split(ui);
    put_status_message(_(" info: stopping the split process before exiting"), ui);
  }

  if (player_is_running(ui))
  {
    player_quit(ui);
  }

  g_application_quit(G_APPLICATION(ui->gui->application));

  return FALSE;
}

static gboolean sigint_called = FALSE;
static void sigint_handler(gint sig)
{
  if (!sigint_called)
  {
    sigint_called = TRUE;
    exit_application(NULL, NULL, ui);
  }
}

#ifndef __WIN32__
static void sigpipe_handler(gint sig)
{
  if (player_is_running(ui) &&
      ui->infos->selected_player == PLAYER_SNACKAMP)
  {
    disconnect_snackamp(ui);
  }
}
#endif

static void register_application_signals()
{
  signal(SIGINT, sigint_handler);
#ifndef __WIN32__
  signal(SIGPIPE, sigpipe_handler);
#endif
}

static void init_i18n_and_plugin_paths(gchar *argv[], ui_state *ui)
{
  setlocale(LC_ALL, "");
  textdomain("mp3splt-gtk");

#ifdef __WIN32__
  char mp3splt_uninstall_file[2048] = { '\0' };
  DWORD dwType, dwSize = sizeof(mp3splt_uninstall_file) - 1;
  SHGetValue(HKEY_LOCAL_MACHINE,
      TEXT("SOFTWARE\\mp3splt-gtk"),
      TEXT("UninstallString"),
      &dwType,
      mp3splt_uninstall_file,
      &dwSize);

  gchar *end = strrchr(mp3splt_uninstall_file, SPLT_DIRCHAR);
  if (end) { *end = '\0'; }

  gchar *executable_dir = NULL;
  gchar *executable = strdup(argv[0]);

  end = strrchr(executable, SPLT_DIRCHAR);
  if (end)
  {
    *end = '\0';
    executable_dir = executable;
  }
  else
  {
    if (mp3splt_uninstall_file[0] != '\0')
    {
      executable_dir = mp3splt_uninstall_file;
    }
  }

  bindtextdomain(LIBMP3SPLT_WITH_SONAME, "translations");
  bindtextdomain("mp3splt-gtk", "translations");
  bind_textdomain_codeset("mp3splt-gtk", "UTF-8");
#else
  bindtextdomain("mp3splt-gtk", LOCALEDIR);
  bind_textdomain_codeset("mp3splt-gtk", nl_langinfo(CODESET));
#endif

#ifdef __WIN32__
  if (executable != NULL)
  {
    if (executable[0] != '\0')
    {
      g_setenv("GST_PLUGIN_PATH", ".\\", TRUE);
      mp3splt_append_plugins_scan_dir(ui->mp3splt_state, executable);
      _chdir(executable);
    }
  }
#endif
}

#ifdef __WIN32__
//!sets the language, loaded only at start
static void set_language_env_variable_from_preferences()
{
  GKeyFile *key_file = g_key_file_new();
  gchar *filename = get_preferences_filename();

  g_key_file_load_from_file(key_file, filename, G_KEY_FILE_KEEP_COMMENTS, NULL);

  if (filename)
  {
    g_free(filename);
    filename = NULL;
  }
  
  gchar *lang = g_key_file_get_string(key_file, "general", "language", NULL);
 
  gchar lang_env[32] = { '\0' };
  g_snprintf(lang_env, 32, "LANG=%s", lang);
  putenv(lang_env);

  g_free(lang);
  g_key_file_free(key_file);
}
#endif

/*! The traditional C main function

\todo 
 - Handle the case that more than one input file is specified at the 
   command line. Until now we just open the first one of the specified 
   files which on windows is basically what notepad does.\n
   And decide what to do in this case: 
   - Going into the multiple files mode will mean that our functionality
     is enabled if several files are opened at once in Windows
   - And opening a separate instance of our program would mean that
     windows and nautilus behaviour are consistent (nautilus seems to
     open every file separately) but - does this really make sense?
 - Handle the case that the specified inputfile is a playlist file
 - Set the full path to the file to make sure that the player will find 
   it even if we are called in a different directory than the file is in
   and stuff.
 - Gstreamer needs a fully qualified path to the audio file in order to 
   be able to play it back. Don't know why. But what I know is that on
   solaris realpath() may return a relative filename. And there might
   be an old system around that does not malloc() memory for a pathname
   if the pathname we give to it is 0 => find a solution that works 
   everywhere.
 */
gint main(gint argc, gchar **argv, gchar **envp)
{
  ui = ui_state_new();

  ui->argc = argc;
  ui->argv = argv;

  register_application_signals();
  init_i18n_and_plugin_paths(argv, ui);

  gtk_init(&argc, &argv);

  lmanager_init_and_find_plugins(ui);

#ifdef __WIN32__
  set_language_env_variable_from_preferences();
#endif

  create_application(ui);
  int application_code =
    g_application_run(G_APPLICATION(ui->gui->application), argc, argv);

  gint return_code = ui->return_code;
  ui_state_free(ui);

  if (application_code != 0)
  {
    return application_code;
  }

  return return_code;
}

