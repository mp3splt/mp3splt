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

#include <signal.h>
#include <locale.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <glib/gi18n.h>
#include <glib.h>

#include <gtk/gtk.h>
#include <libmp3splt/mp3splt.h>

#ifdef __WIN32__
#include <windows.h>
#include <shlwapi.h>
#endif

#include "util.h"
#include "special_split.h"
#include "player.h"
#include "utilities.h"
#include "tree_tab.h"
#include "main_win.h"
#include "snackamp_control.h"
#include "split_files.h"
#include "preferences_tab.h"
#include "multiple_files.h"
#include "preferences_manager.h"
#include "player_tab.h"
#include "import.h"
#include "mp3splt-gtk.h"

#include "ui_manager.h"

//the state
splt_state *the_state = NULL;

//the progress bar
GtkWidget *progress_bar;

extern GArray *splitpoints;
extern gint splitnumber;
extern GtkWidget *queue_files_button;
extern gchar *filename_to_split;
extern gchar *filename_path_of_split;

//EXTERNAL OPTIONS
//frame mode option
extern GtkWidget *frame_mode;
//auto-adjust option
extern GtkWidget *adjust_mode;
//gap parameter
extern GtkWidget *spinner_adjust_gap;
//offset parameter
extern GtkWidget *spinner_adjust_offset;
//threshold parameter
extern GtkWidget *spinner_adjust_threshold;

extern GtkWidget *create_dirs_from_output_files;

//silence mode parameters
//number of tracks parameter
extern GtkWidget *spinner_silence_number_tracks;
//number of tracks parameter
extern GtkWidget *spinner_silence_minimum;
//offset parameter
extern GtkWidget *spinner_silence_offset;
//threshold parameter
extern GtkWidget *spinner_silence_threshold;
//remove silence check button (silence mode parameter
extern GtkWidget *silence_remove_silence;

//spinner time
extern GtkWidget *spinner_time;

//player
extern gint selected_player;

//if we are currently splitting
extern gint we_are_splitting;
//if we quit the main program while splitting
extern gint we_quit_main_program;
//the percent progress bar
extern GtkWidget *percent_progress_bar;

//stop button to cancel the split
extern GtkWidget *cancel_button;

//the output entry
extern GtkWidget *output_entry;

extern GtkWidget *remove_all_files_button;

extern gint split_file_mode;
extern GtkWidget *multiple_files_tree;
extern gint multiple_files_tree_number;

//how many split files
gint split_files = 0;

//! move all options inside
ui_state *ui = NULL;

//! Add another file to the split_file tab
void put_split_filename(const char *filename,int progress_data)
{
  enter_threads();

  if (!gtk_widget_get_sensitive(queue_files_button))
  {
    gtk_widget_set_sensitive(queue_files_button, TRUE);
  }
  if (!gtk_widget_get_sensitive(remove_all_files_button))
  {
    gtk_widget_set_sensitive(remove_all_files_button,TRUE);
  }

  add_split_row(filename);
  split_files++;

  gint fname_status_size = (strlen(filename) + 255);
  gchar *fname_status = g_malloc(sizeof(char) * fname_status_size);
  g_snprintf(fname_status, fname_status_size,
      _(" File '%s' created"), filename);
  put_status_message(fname_status);
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

//!Allows to set the value shown by the progress bar
void change_window_progress_bar(splt_progress *p_bar)
{
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
      g_snprintf(progress_text,1023,
		      _("S: %02d, Level: %.2f dB; scanning for silence..."),
		      p_bar->silence_found_tracks, p_bar->silence_db_level);
      break;
    default:
      g_snprintf(progress_text,1023, " ");
      break;
  }

  gchar printed_value[1024] = { '\0' };

  enter_threads();

  gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(percent_progress_bar),
      p_bar->percent_progress);
  g_snprintf(printed_value,1023,"%6.2f %% %s", p_bar->percent_progress * 100,
      progress_text);

  gtk_progress_bar_set_text(GTK_PROGRESS_BAR(percent_progress_bar),
      printed_value);

#ifdef __WIN32__
  while (gtk_events_pending())
  {
	  gtk_main_iteration();
  }
  gdk_flush();
#endif

  exit_threads();
}

//! Split the file
gpointer split_it(gpointer data)
{
  gint confirmation = SPLT_OK;
  
  enter_threads();

  remove_all_split_rows();

  exit_threads();
 
  gint err = SPLT_OK;

  mp3splt_erase_all_splitpoints(the_state,&err);

  //we erase previous tags if we don't have the option
  //splt_current_tags
  if ((mp3splt_get_int_option(the_state, SPLT_OPT_TAGS, &err) !=
        (SPLT_CURRENT_TAGS) || split_file_mode == FILE_MODE_MULTIPLE))
  {
    mp3splt_erase_all_tags(the_state,&err);
  }

  gint split_mode =
    mp3splt_get_int_option(the_state, SPLT_OPT_SPLIT_MODE, &err);

  enter_threads();
  print_status_bar_confirmation(err);
  
  gchar *format = strdup(gtk_entry_get_text(GTK_ENTRY(output_entry)));
  exit_threads();

  mp3splt_set_oformat(the_state, format, &err);
  if (format)
  {
    free(format);
    format = NULL;
  }
 
  //if we have the normal split mode, enable default output
  gint output_filenames = 
    mp3splt_get_int_option(the_state, SPLT_OPT_OUTPUT_FILENAMES,&err);
  if (mp3splt_get_int_option(the_state, SPLT_OPT_SPLIT_MODE,&err)
      == SPLT_OPTION_NORMAL_MODE)
  {
    mp3splt_set_int_option(the_state, SPLT_OPT_OUTPUT_FILENAMES,
        SPLT_OUTPUT_CUSTOM);
  }

  mp3splt_set_path_of_split(the_state,filename_path_of_split);

  gint multiple_files_error = SPLT_FALSE;
  if (split_file_mode == FILE_MODE_SINGLE)
  {
    enter_threads();

    if (split_mode == SPLT_OPTION_NORMAL_MODE)
    {
      put_splitpoints_in_the_state(the_state);
    }

    print_processing_file(filename_to_split);
    exit_threads();

    mp3splt_set_filename_to_split(the_state, filename_to_split);
    confirmation = mp3splt_split(the_state);
  }
  else
  {
    if (multiple_files_tree_number > 0)
    {
      enter_threads();

      gchar *filename = NULL;
      GtkTreeIter iter;
      GtkTreePath *path;
      GtkTreeModel *model =
        gtk_tree_view_get_model(GTK_TREE_VIEW(multiple_files_tree));

      exit_threads();

      gint row_number = 0;
      while (row_number < multiple_files_tree_number)
      {
        enter_threads();

        if (split_mode == SPLT_OPTION_NORMAL_MODE)
        {
          put_splitpoints_in_the_state(the_state);
        }

        path = gtk_tree_path_new_from_indices(row_number ,-1);
        gtk_tree_model_get_iter(model, &iter, path);
        gtk_tree_model_get(model, &iter, MULTIPLE_COL_FILENAME,
            &filename, -1);

        print_processing_file(filename);

        exit_threads();

        mp3splt_set_filename_to_split(the_state, filename);
        confirmation = mp3splt_split(the_state);

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

        mp3splt_erase_all_tags(the_state, &err);
        print_status_bar_confirmation(err);
        err = SPLT_OK;
        mp3splt_erase_all_splitpoints(the_state, &err);
        print_status_bar_confirmation(err);

        exit_threads();
      }
    }
    else
    {
      multiple_files_error = SPLT_TRUE;

      enter_threads();
      put_status_message(_(" error: no files found in multiple files mode"));
      exit_threads();
    }
  }

  /*! reenable default output if necessary
   */
  mp3splt_set_int_option(the_state, SPLT_OPT_OUTPUT_FILENAMES, output_filenames);
  
  enter_threads();

  //we show infos about the split action
  print_status_bar_confirmation(confirmation);
  
  //see the cancel button
  gtk_widget_set_sensitive(GTK_WIDGET(cancel_button), FALSE);
  
  //we look if we have pushed the exit button
  if (we_quit_main_program)
  {
    quit(NULL,NULL);
  }
  
  if (confirmation >= 0 && !multiple_files_error)
  {
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(percent_progress_bar), 1.0);
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(percent_progress_bar),
        _(" finished"));
  }

  we_are_splitting = FALSE;

  exit_threads();
 
  return NULL;
}

/*!handler for the SIGPIPE signal

  This signal is issued by snackamp on close.
 */
void sigpipe_handler(gint sig)
{
  if (player_is_running() && selected_player == PLAYER_SNACKAMP)
  {
    disconnect_snackamp();
  }
}

gboolean sigint_called = FALSE;
//!handler for the SIGINT signal
void sigint_handler(gint sig)
{
  if (!sigint_called)
  {
    sigint_called = TRUE;
    we_quit_main_program = TRUE;
    quit(NULL,NULL);
  }
}

//!prints a message from the library
void put_message_from_library(const char *message, splt_message_type mess_type)
{
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

    put_status_message_with_type(mess, mess_type);

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

GThread *create_thread(GThreadFunc func, gpointer data,
		gboolean joinable, GError **error)
{
/*#ifdef __WIN32__
	func(data);
	return NULL;
	return g_thread_create(func, data, joinable, error);
#else*/
	return g_thread_create(func, data, joinable, error);
//#endif
}

void enter_threads()
{
	gdk_threads_enter();
}

void exit_threads()
{
	gdk_threads_leave();
}

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

  int OptionChar;

  //init threads
  g_thread_init(NULL);
  gdk_threads_init();

  gint error = 0;
  
  //close nicely
  signal (SIGINT, sigint_handler);
  
#ifndef __WIN32__
  signal (SIGPIPE, sigpipe_handler);
#endif
   
  //create new state
  the_state = mp3splt_new_state(&error);

  //used for gettext
  setlocale (LC_ALL, "");
  textdomain ("mp3splt-gtk");

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
 
  // Allow the gtk to parse all gtk arguments from the command 
  // line first
  gtk_init(&argc, &argv);
  
  // Now pass all remaining arguments to getopt.
  opterr = 0;
  
  while ((OptionChar = getopt (argc, argv, "d:")) != -1)
    switch (OptionChar)
      {
      case 'd':
	fprintf (stderr, _("Trying to set the output directory to %s.\n"), optarg);
	outputdirectory_set((gchar *)optarg);
#ifdef __WIN32__
	mkdir(optarg);
#else
	mkdir(optarg, 0777);
#endif
	if(!check_if_dir((guchar *)optarg))
	  {
	    fprintf(stderr,_("Error: The specified output directory is inaccessible!\n"));
	    exit(-1);
	  }
	break;
      case '?':
	if (optopt == 'd')
	  fprintf (stderr, _("Option -%c requires an argument.\n"), optopt);
	else if (isprint (optopt))
	  fprintf (stderr, _("Unknown option `-%c'.\n"), optopt);
	else
	  fprintf (stderr,
		   _("Unknown option character `\\x%x'.\n"),
		   optopt);
	return 1;
      default:
	abort ();
      }
  
  // If we have a filename at the command line is checked after the
  // GUI is up.

  //We initialise the splitpoints array
  splitpoints = g_array_new(FALSE, FALSE, sizeof (Split_point));
 
  //check if preferences file exists
  //and if it does not create it
  check_pref_file();
 
  mp3splt_set_progress_function(the_state,change_window_progress_bar);
  mp3splt_set_split_filename_function(the_state,put_split_filename);
  mp3splt_set_message_function(the_state, put_message_from_library);

  mp3splt_set_int_option(the_state, SPLT_OPT_DEBUG_MODE, SPLT_FALSE);
  mp3splt_set_int_option(the_state,
      SPLT_OPT_SET_FILE_FROM_CUE_IF_FILE_TAG_FOUND, SPLT_TRUE);

  //add special directory search for plugins on Windows
#ifdef __WIN32__
  if (executable != NULL)
  {
    if (executable[0] != '\0')
    {
      g_setenv("GST_PLUGIN_PATH", ".\\", TRUE);
      mp3splt_append_plugins_scan_dir(the_state, executable);
      _chdir(executable);
    }
  }
#endif

  //main program
  create_all();
  error = mp3splt_find_plugins(the_state);
  if (error < 0)
  {
    print_status_bar_confirmation(error);
  }

  // Now let's see if we found a filename at the command line.
  if (optind != argc) 
    {
      if(!check_if_file((guchar *)argv[optind]))
	{
	  fprintf (stderr,
		   _("Cannot open input file %s\n"),
		   argv[optind]);
	return 1;
	}

#ifndef __WIN32__
      // If we start the player it will find a file with a relative
      // pathname. But if the player is already running it might
      // need an absolute path.
      char *inputfilename=realpath(argv[optind],NULL);
      handle_import(inputfilename);
      free(inputfilename);
#else
      handle_import(argv[optind]);
#endif
    }

  gdk_threads_enter();
  gtk_main();
  exit_threads();

  mp3splt_free_state(the_state, &error);

  ui_state_free(ui);
  
  return 0;
}

