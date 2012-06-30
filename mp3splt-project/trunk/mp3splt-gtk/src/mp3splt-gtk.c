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
 * The main file,
 *
 * this file contains the main() function as well as some 
 * globally used functions.
 *********************************************************/

#include "mp3splt-gtk.h"

extern gchar *filename_path_of_split;

extern GtkWidget *multiple_files_tree;
extern gint multiple_files_tree_number;

ui_state *ui = NULL;

//! Split the file
gpointer split_it(gpointer data)
{
  gint confirmation = SPLT_OK;
  
  enter_threads();

  remove_all_split_rows(ui);

  exit_threads();
 
  gint err = SPLT_OK;

  mp3splt_erase_all_splitpoints(ui->mp3splt_state,&err);

  //we erase previous tags if we don't have the option
  //splt_current_tags
  if ((mp3splt_get_int_option(ui->mp3splt_state, SPLT_OPT_TAGS, &err) !=
        (SPLT_CURRENT_TAGS) || ui->infos->split_file_mode == FILE_MODE_MULTIPLE))
  {
    mp3splt_erase_all_tags(ui->mp3splt_state,&err);
  }

  gint split_mode =
    mp3splt_get_int_option(ui->mp3splt_state, SPLT_OPT_SPLIT_MODE, &err);

  enter_threads();
  print_status_bar_confirmation(err, ui->gui);
  
  gchar *format = strdup(gtk_entry_get_text(GTK_ENTRY(ui->gui->output_entry)));
  exit_threads();

  mp3splt_set_oformat(ui->mp3splt_state, format, &err);
  if (format)
  {
    free(format);
    format = NULL;
  }
 
  //if we have the normal split mode, enable default output
  gint output_filenames = 
    mp3splt_get_int_option(ui->mp3splt_state, SPLT_OPT_OUTPUT_FILENAMES,&err);
  if (mp3splt_get_int_option(ui->mp3splt_state, SPLT_OPT_SPLIT_MODE,&err)
      == SPLT_OPTION_NORMAL_MODE)
  {
    mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_OUTPUT_FILENAMES,
        SPLT_OUTPUT_CUSTOM);
  }

  mp3splt_set_path_of_split(ui->mp3splt_state,filename_path_of_split);

  gint multiple_files_error = SPLT_FALSE;
  if (ui->infos->split_file_mode == FILE_MODE_SINGLE)
  {
    enter_threads();

    if (split_mode == SPLT_OPTION_NORMAL_MODE)
    {
      put_splitpoints_in_mp3splt_state(ui->mp3splt_state);
    }

    print_processing_file(ui->status->filename_to_split);
    exit_threads();

    mp3splt_set_filename_to_split(ui->mp3splt_state, ui->status->filename_to_split);
    confirmation = mp3splt_split(ui->mp3splt_state);
  }
  else
  {
    if (multiple_files_tree_number > 0)
    {
      enter_threads();

      gchar *filename = NULL;
      GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(multiple_files_tree));

      exit_threads();

      gint row_number = 0;
      while (row_number < multiple_files_tree_number)
      {
        enter_threads();

        if (split_mode == SPLT_OPTION_NORMAL_MODE)
        {
          put_splitpoints_in_mp3splt_state(ui->mp3splt_state);
        }

        GtkTreePath *path = gtk_tree_path_new_from_indices(row_number ,-1);

        GtkTreeIter iter;
        gtk_tree_model_get_iter(model, &iter, path);
        gtk_tree_model_get(model, &iter, MULTIPLE_COL_FILENAME,
            &filename, -1);

        print_processing_file(filename);

        exit_threads();

        mp3splt_set_filename_to_split(ui->mp3splt_state, filename);
        confirmation = mp3splt_split(ui->mp3splt_state);

        if (filename)
        {
          g_free(filename);
          filename = NULL;
        }

        if (confirmation < 0)
        {
          break;
        }

        row_number++;

        enter_threads();

        mp3splt_erase_all_tags(ui->mp3splt_state, &err);
        print_status_bar_confirmation(err, ui->gui);
        err = SPLT_OK;
        mp3splt_erase_all_splitpoints(ui->mp3splt_state, &err);
        print_status_bar_confirmation(err, ui->gui);

        exit_threads();
      }
    }
    else
    {
      multiple_files_error = SPLT_TRUE;

      enter_threads();
      put_status_message(_(" error: no files found in multiple files mode"), ui->gui);
      exit_threads();
    }
  }

  /*! reenable default output if necessary
   */
  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_OUTPUT_FILENAMES, output_filenames);
  
  enter_threads();

  print_status_bar_confirmation(confirmation, ui->gui);
  
  //see the cancel button
  gtk_widget_set_sensitive(ui->gui->cancel_button, FALSE);
  
  if (ui->status->quit_main_program)
  {
    exit_application(NULL, NULL);
  }
  
  if (confirmation >= 0 && !multiple_files_error)
  {
    gtk_progress_bar_set_fraction(ui->gui->percent_progress_bar, 1.0);
    gtk_progress_bar_set_text(ui->gui->percent_progress_bar, _(" finished"));
  }

  ui->status->splitting = FALSE;

  exit_threads();
 
  return NULL;
}

GThread *create_thread(GThreadFunc func, gpointer data, gboolean joinable, GError **error)
{
  return g_thread_create(func, data, joinable, error);
}

void enter_threads()
{
	gdk_threads_enter();
}

void exit_threads()
{
	gdk_threads_leave();
}

//close the window and exit button function
void exit_application(GtkWidget *widget, gpointer *data)
{
  save_preferences(NULL, NULL);

  if (ui->status->splitting)
  {
    lmanager_stop_split(ui);
    ui->status->quit_main_program = TRUE;
    put_status_message(_(" info: stopping the split process before exiting"), ui->gui);
  }

  if (player_is_running())
  {
    player_quit();
  }

  gtk_main_quit();
}

static gboolean sigint_called = FALSE;
static void sigint_handler(gint sig)
{
  if (!sigint_called)
  {
    sigint_called = TRUE;
    ui->status->quit_main_program = TRUE;
    exit_application(NULL, NULL);
  }
}

#ifndef __WIN32__
static void sigpipe_handler(gint sig)
{
  if (player_is_running() && ui->infos->selected_player == PLAYER_SNACKAMP)
  {
    disconnect_snackamp();
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

static void init_i18n_and_plugin_paths(ui_state *ui)
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

  bindtextdomain(MP3SPLT_LIB_GETTEXT_DOMAIN, "translations");
  bindtextdomain("mp3splt-gtk", "translations");
#else
  bindtextdomain("mp3splt-gtk", LOCALEDIR);
#endif
  bind_textdomain_codeset("mp3splt-gtk", "UTF-8");

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

static void parse_command_line_options(gint argc, gchar * argv[])
{
  opterr = 0;
  int option;
  while ((option = getopt(argc, argv, "d:")) != -1)
  {
    switch (option)
    {
      case 'd':
        fprintf(stdout, _("Setting the output directory to %s.\n"), optarg);
        set_output_directory((gchar *)optarg, ui);
#ifdef __WIN32__
        mkdir(optarg);
#else
        mkdir(optarg, 0777);
#endif
        if (!directory_exists(optarg))
        {
          ui_fail(ui, "Error: The specified output directory is inaccessible!\n");
        }
        break;
      case '?':
        if (optopt == 'd')
          ui_fail(ui, _("Option -%c requires an argument.\n"), optopt);
        else if (isprint(optopt))
          ui_fail(ui, _("Unknown option `-%c'.\n"), optopt, NULL);
        else
          ui_fail(ui, _("Unknown option character `\\x%x'.\n"), optopt);
        break;
      default:
        ui_fail(ui, NULL);
    }
  }

  if (optind == argc)
  {
    return;
  }

  if (!file_exists(argv[optind]))
  {
    ui_fail(ui, _("Cannot open input file %s\n"), argv[optind]);
  }

#ifndef __WIN32__
  char *input_filename = realpath(argv[optind],NULL);
  import_file(input_filename);
  free(input_filename);
#else
  import_file(argv[optind]);
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
gint main(gint argc, gchar *argv[], gchar **envp)
{
  ui = ui_state_new();

  g_thread_init(NULL);
  gdk_threads_init();

  register_application_signals();
  init_i18n_and_plugin_paths(ui);

  gtk_init(&argc, &argv);

  lmanager_init_and_find_plugins(ui);

#ifdef __WIN32__
  set_language_env_variable_from_preferences();
#endif

  create_application(ui);

  parse_command_line_options(argc, argv);

  enter_threads();
  gtk_main();
  exit_threads();

  gint return_code = ui->return_code;
  ui_state_free(ui);

  return return_code;
}

