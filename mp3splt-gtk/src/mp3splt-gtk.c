/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2009 Alexandru Munteanu
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

/**********************************************************
 * Filename: mp3splt-gtk.c
 *
 * main file of the program that does the effective split of the
 * mp3/ogg and initialises gtk..
 *
 *********************************************************/

#include <signal.h>
#include <gtk/gtk.h>
#include <libmp3splt/mp3splt.h>
#include <locale.h>
#include <glib/gi18n.h>
#include <string.h>
#include <glib.h>

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

//the state
splt_state *the_state = NULL;

//the progress bar
GtkWidget *progress_bar;

extern GArray *splitpoints;
extern gint splitnumber;
extern GList *player_pref_list;
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
//the selected split mode
extern gint selected_split_mode;

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

extern GtkWidget *split_button;

extern GtkWidget *remove_all_files_button;

extern gint split_file_mode;
extern GtkWidget *multiple_files_tree;
extern gint multiple_files_tree_number;
extern gint debug_is_active;

//how many split files
gint split_files = 0;

//put the split file in the split_file tab
void put_split_filename(const char *filename,int progress_data)
{
  gdk_threads_enter();

  if (!GTK_WIDGET_SENSITIVE(queue_files_button))
  {
    gtk_widget_set_sensitive(queue_files_button, TRUE);
  }
  if (!GTK_WIDGET_SENSITIVE(remove_all_files_button))
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

  gdk_threads_leave();
}

//put the options from the preferences
void put_options_from_preferences()
{
  //preferences options
  //
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(frame_mode)))
  {
    mp3splt_set_int_option(the_state, SPLT_OPT_FRAME_MODE,
        SPLT_TRUE);
  }
  else
  {
    mp3splt_set_int_option(the_state, SPLT_OPT_FRAME_MODE,
        SPLT_FALSE);
  }
  //adjust option
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(adjust_mode)))
  {
    mp3splt_set_int_option(the_state, SPLT_OPT_AUTO_ADJUST,
        SPLT_TRUE);
    //adjust spinners
    mp3splt_set_float_option(the_state, SPLT_OPT_PARAM_OFFSET,
        gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(spinner_adjust_offset)));
    mp3splt_set_int_option(the_state, SPLT_OPT_PARAM_GAP,
        gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinner_adjust_gap)));
    mp3splt_set_float_option(the_state, SPLT_OPT_PARAM_THRESHOLD,
        gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(spinner_adjust_threshold)));
  }
  else
  {
    mp3splt_set_int_option(the_state, SPLT_OPT_AUTO_ADJUST,
        SPLT_FALSE);
  }

  //default seekable false
  mp3splt_set_int_option(the_state, SPLT_OPT_INPUT_NOT_SEEKABLE,
      SPLT_FALSE);

  //we set default option;
  mp3splt_set_int_option(the_state, SPLT_OPT_SPLIT_MODE,
      SPLT_OPTION_NORMAL_MODE);

  //we get the split modes
  switch (selected_split_mode)
  {
    case SELECTED_SPLIT_NORMAL:
      mp3splt_set_int_option(the_state, SPLT_OPT_SPLIT_MODE,
          SPLT_OPTION_NORMAL_MODE);
      break;
    case SELECTED_SPLIT_WRAP:
      mp3splt_set_int_option(the_state, SPLT_OPT_SPLIT_MODE,
          SPLT_OPTION_WRAP_MODE);
      break;
    case SELECTED_SPLIT_TIME:
      mp3splt_set_int_option(the_state, SPLT_OPT_SPLIT_MODE,
          SPLT_OPTION_TIME_MODE);
      //we set the time option
      mp3splt_set_float_option(the_state, SPLT_OPT_SPLIT_TIME,
          gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinner_time)));
      break;
    case SELECTED_SPLIT_ERROR:
      mp3splt_set_int_option(the_state, SPLT_OPT_SPLIT_MODE,
          SPLT_OPTION_ERROR_MODE);
      break;
    default:
      break;
  }

  //tag options
  //0 = No tags
  if (get_checked_tags_radio_box() == 0)
  {
    mp3splt_set_int_option(the_state, SPLT_OPT_TAGS,
        SPLT_NO_TAGS);
  }
  else
  {
    if (get_checked_tags_radio_box() == 1)
    {
      mp3splt_set_int_option(the_state, SPLT_OPT_TAGS,
          SPLT_CURRENT_TAGS);
    }
    else
    {
      if (get_checked_tags_radio_box() == 2)
      {
        mp3splt_set_int_option(the_state, SPLT_OPT_TAGS,
            SPLT_TAGS_ORIGINAL_FILE);
      }
    }
  }

  //tag version options
  gint tags_radio_choice = get_checked_tags_version_radio_box();
  if (tags_radio_choice == 0)
  {
    mp3splt_set_int_option(the_state, SPLT_OPT_FORCE_TAGS_VERSION, 0);
  }
  else
  {
    if (tags_radio_choice == 1)
    {
      mp3splt_set_int_option(the_state, SPLT_OPT_FORCE_TAGS_VERSION, 1);
    }
    else
    {
      if (tags_radio_choice == 2)
      {
        mp3splt_set_int_option(the_state, SPLT_OPT_FORCE_TAGS_VERSION, 2);
      }
      else
      {
        if (tags_radio_choice == 3)
        {
          mp3splt_set_int_option(the_state, SPLT_OPT_FORCE_TAGS_VERSION, 12);
        }
      }
    }
  }
  mp3splt_set_int_option(the_state, SPLT_OPT_DEBUG_MODE, debug_is_active);
}

//changes the progress bar
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
      g_snprintf(progress_text,2047,
          _("S: %02d, Level: %.2f dB; scanning for silence..."),
          p_bar->silence_found_tracks, p_bar->silence_db_level);
      break;
    default:
      g_snprintf(progress_text,1023, " ");
      break;
  }

  gchar printed_value[1024] = { '\0' };

  gdk_threads_enter();

  gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(percent_progress_bar),
      p_bar->percent_progress);
  g_snprintf(printed_value,1023,"%6.2f %% %s", p_bar->percent_progress * 100,
      progress_text);

  gtk_progress_bar_set_text(GTK_PROGRESS_BAR(percent_progress_bar),
      printed_value);

  gdk_threads_leave();
}

//effective split of the file
gpointer split_it(gpointer data)
{
  gint confirmation = SPLT_OK;
  
  gdk_threads_enter();

  remove_all_split_rows();

  gdk_threads_leave();
 
  gint err = SPLT_OK;
  //erase previous splitpoints
  mp3splt_erase_all_splitpoints(the_state,&err);
  //we erase previous tags if we don't have the option
  //splt_current_tags
  if (mp3splt_get_int_option(the_state, SPLT_OPT_TAGS,&err)
      != SPLT_CURRENT_TAGS)
  {
    mp3splt_erase_all_tags(the_state,&err);
  }

  gint split_mode =
    mp3splt_get_int_option(the_state, SPLT_OPT_SPLIT_MODE, &err);

  gdk_threads_enter();
  print_status_bar_confirmation(err);
  
  //we put the splitpoints in the state only if the normal mode
  if (split_mode == SPLT_OPTION_NORMAL_MODE)
  {
    put_splitpoints_in_the_state(the_state);
  }

  gchar *format = strdup(gtk_entry_get_text(GTK_ENTRY(output_entry)));
  gdk_threads_leave();

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
    gdk_threads_enter();
    print_processing_file(filename_to_split);
    gdk_threads_leave();

    mp3splt_set_filename_to_split(the_state, filename_to_split);
    confirmation = mp3splt_split(the_state);
  }
  else
  {
    if (multiple_files_tree_number > 0)
    {
      gdk_threads_enter();

      gchar *filename = NULL;
      GtkTreeIter iter;
      GtkTreePath *path;
      GtkTreeModel *model =
        gtk_tree_view_get_model(GTK_TREE_VIEW(multiple_files_tree));

      gdk_threads_leave();

      gint row_number = 0;
      while (row_number < multiple_files_tree_number)
      {
        gdk_threads_enter();

        path = gtk_tree_path_new_from_indices(row_number ,-1);
        gtk_tree_model_get_iter(model, &iter, path);
        gtk_tree_model_get(model, &iter, MULTIPLE_COL_FILENAME,
            &filename, -1);

        print_processing_file(filename);

        gdk_threads_leave();

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
      }
    }
    else
    {
      multiple_files_error = SPLT_TRUE;

      gdk_threads_enter();
      put_status_message(_(" error: no files found in multiple files mode"));
      gdk_threads_leave();
    }
  }

  //reenable default output if necessary
  mp3splt_set_int_option(the_state, SPLT_OPT_OUTPUT_FILENAMES, output_filenames);
  
  gdk_threads_enter();

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

  gdk_threads_leave();
 
  return NULL;
}

//handler for the SIGPIPE signal
void sigpipe_handler(gint sig)
{
  if (player_is_running() && selected_player == PLAYER_SNACKAMP)
  {
    disconnect_snackamp();
  }
}

gboolean sigint_called = FALSE;
//handler for the SIGINT signal
void sigint_handler(gint sig)
{
  if (!sigint_called)
  {
    sigint_called = TRUE;
    we_quit_main_program = TRUE;
    quit(NULL,NULL);
  }
}

//prints a message from the library
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
    gdk_threads_enter();
    put_status_message_with_type(mess, mess_type);
    gdk_threads_leave();
    g_free(mess);
    mess = NULL;
  }
}

gint main (gint argc, gchar *argv[], gchar **envp)
{
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

  gchar *executable = strdup(argv[0]);
  gchar *executable_dir = NULL;

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

  if (executable_dir != NULL)
  {
    gint translation_dir_length = strlen(executable_dir) + 30;
    gchar *translation_dir = malloc(sizeof(gchar) * translation_dir_length);
    g_snprintf(translation_dir,translation_dir_length,
        "%s%ctranslations",executable_dir,SPLT_DIRCHAR);

    bindtextdomain(MP3SPLT_LIB_GETTEXT_DOMAIN, translation_dir);
    bindtextdomain("mp3splt-gtk", translation_dir);

    if (translation_dir)
    {
      free(translation_dir);
      translation_dir = NULL;
    }
  }

#else
  bindtextdomain ("mp3splt-gtk", LOCALEDIR);
#endif

  bind_textdomain_codeset ("mp3splt-gtk", "UTF-8");
 
  gtk_init(&argc, &argv);
  
  //we initialise the splitpoints array
  splitpoints = g_array_new(FALSE, FALSE, sizeof (Split_point));
  
  //checks if preferences file exists
  //and if it does not, it creates it
  check_pref_file();
  
  //put the callback progress bar function
  mp3splt_set_progress_function(the_state,change_window_progress_bar);
  //put the callback function to receive the split file
  mp3splt_set_split_filename_function(the_state,put_split_filename);
  //put the callback function for miscellaneous messages
  mp3splt_set_message_function(the_state, put_message_from_library);
  //debug on or off
  mp3splt_set_int_option(the_state,SPLT_OPT_DEBUG_MODE, SPLT_FALSE);

  //add special directory search for plugins on Windows
#ifdef __WIN32__
  if (executable != NULL)
  {
    if (executable[0] != '\0')
    {
      g_setenv("GST_PLUGIN_PATH",executable,TRUE);
      mp3splt_append_plugins_scan_dir(the_state, executable);
    }
    free(executable);
    executable = NULL;
  }

  if (mp3splt_uninstall_file[0] != '\0')
  {
    mp3splt_append_plugins_scan_dir(the_state, mp3splt_uninstall_file);
  }
#endif

  //main program
  create_all();
  error = mp3splt_find_plugins(the_state);
  if (error < 0)
  {
    print_status_bar_confirmation(error);
  }
  
  gdk_threads_enter();
  gtk_main();
  gdk_threads_leave();
  
  return 0;
}
