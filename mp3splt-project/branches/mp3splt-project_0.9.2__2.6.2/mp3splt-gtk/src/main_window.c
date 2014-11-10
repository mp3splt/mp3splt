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
 * The main window
 *
 * main file that initialises the menubar, the toolbar, 
 * the tabs, about window, status error messages
 *
 *********************************************************/

#include "main_window.h"

//! Set the name of the input file
void set_input_filename(const gchar *filename, ui_state *ui)
{
  if (filename == NULL)
  {
    return;
  }

  if (ui->gui->input_filename != NULL)
  {
    g_string_free(ui->gui->input_filename,TRUE);
  }
  ui->gui->input_filename = g_string_new(filename);

  if (ui->gui->open_file_chooser_button != NULL && !ui->status->file_selection_changed)
  {
    gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(ui->gui->open_file_chooser_button), filename);
  }

  update_example_output_dir_for_single_file_split(ui);

  export_cue_file_in_configuration_directory(ui);
}

/*! Get the name of the input file

\return 
 - The name of the input file, if set.
 - "", otherwise.
*/
gchar* get_input_filename(gui_state *gui)
{
  if (gui->input_filename != NULL)
  {
    return gui->input_filename->str;
  }

  return "";
}

static gboolean configure_window_callback(GtkWindow *window, GdkEvent *event, ui_state *ui)
{
  ui_set_main_win_position(ui, event->configure.x, event->configure.y); 
  ui_set_main_win_size(ui, event->configure.width, event->configure.height);

  refresh_drawing_area(ui->gui, ui->infos);
  refresh_preview_drawing_areas(ui->gui);

  clear_previous_distances(ui);

  return FALSE;
}

static void initialize_window(GApplication *app, ui_state *ui)
{
  GtkWidget *window = gtk_application_window_new(GTK_APPLICATION(app));
  ui->gui->window = window;

  g_signal_connect(G_OBJECT(window), "configure-event", G_CALLBACK(configure_window_callback), ui);

  gtk_window_set_title(GTK_WINDOW(window), PACKAGE_NAME" "VERSION);
  gtk_container_set_border_width(GTK_CONTAINER(window), 0);

  g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(exit_application), ui);

  GString *imagefile = g_string_new("");
  build_path(imagefile, PIXMAP_PATH, "mp3splt-gtk_ico"ICON_EXT);
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(imagefile->str, NULL);
  gtk_window_set_default_icon(pixbuf);
  g_string_free(imagefile, TRUE);
}

static void activate_url(GtkAboutDialog *about, const gchar *link, ui_state *ui)
{
#ifdef __WIN32__
  char default_browser[512] = { '\0' };
  DWORD dwType, dwSize = sizeof(default_browser) - 1;

  SHGetValue(HKEY_CURRENT_USER,
        TEXT("Software\\Clients\\StartMenuInternet"),
        TEXT(""),
        &dwType,
        default_browser,
        &dwSize);

  if (default_browser[0] != '\0')
  {
    SHGetValue(HKEY_LOCAL_MACHINE,
        TEXT("SOFTWARE\\Clients\\StartMenuInternet"),
        TEXT(""),
        &dwType,
        default_browser,
        &dwSize);
  }

  if (default_browser[0] != '\0')
  {
    char browser_exe[2048] = { '\0' };
    dwSize = sizeof(browser_exe) - 1;

    char browser_exe_registry[1024] = { '\0' };
    snprintf(browser_exe_registry, 1024,
        "SOFTWARE\\Clients\\StartMenuInternet\\%s\\shell\\open\\command\\",
        default_browser);

    SHGetValue(HKEY_LOCAL_MACHINE,
        TEXT(browser_exe_registry), TEXT(""),
        &dwType, browser_exe, &dwSize);

    if (browser_exe[0] != '\0')
    {
      gint browser_command_size = strlen(browser_exe) + strlen(link) + 2;
      char *browser_command = g_malloc(sizeof(char) * browser_command_size);
      if (browser_command)
      {
        snprintf(browser_command, browser_command_size, "%s %s",
            browser_exe, link);

        STARTUPINFO si;
        PROCESS_INFORMATION pinf;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pinf, sizeof(pinf));

        if (! CreateProcess(NULL, browser_command,
              NULL, NULL, FALSE, 0, NULL, NULL, &si, &pinf))
        {
          put_status_message(_("Error launching external command"), ui);
        }

        CloseHandle(pinf.hProcess);
        CloseHandle(pinf.hThread);

        g_free(browser_command);
        browser_command = NULL;
      }
    }
  }
#endif
}

static void about_window(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  ui_state *ui = (ui_state *) data;

  GtkWidget *dialog = gtk_about_dialog_new();

  GString *imagefile = g_string_new("");
  build_path(imagefile, PIXMAP_PATH, "mp3splt-gtk.png");
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(imagefile->str, NULL);
  gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), pixbuf);
  g_string_free(imagefile, TRUE);
  
  gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), (gchar *)PACKAGE_NAME);
  gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), VERSION);
  gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog),
                                 PACKAGE_NAME" : Copyright © 2005-2014 Alexandru"
                                 " Munteanu \n mp3splt : Copyright © 2002-2005 Matteo Trotta");

  gchar b3[100] = { '\0' };
  gchar *b1 = _("using");

  char *library_version = mp3splt_get_version();
  g_snprintf(b3, 100, "-%s-\n%s libmp3splt %s",
             _("release of "MP3SPLT_GTK_DATE), b1, library_version);
  free(library_version);
  
  gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), b3);
  
  gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(dialog),
                                "\n"
                                "This program is free software; you can "
                                "redistribute it and/or \n"
                                "modify it under the terms of the GNU General Public License\n"
                                "as published by the Free Software "
                                "Foundation; either version 2\n"
                                "of the License, or (at your option) "
                                "any later version.\n\n"
                                "This program is distributed in the "
                                "hope that it will be useful,\n"
                                "but WITHOUT ANY WARRANTY; without even "
                                "the implied warranty of\n"
                                "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
                                "GNU General Public License for more details.\n\n"
                                "You should have received a copy of the GNU General Public License\n"
                                "along with this program; if not, write "
                                "to the Free Software\n"
                                "Foundation, Inc., "
                                "51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.");

  g_signal_connect(G_OBJECT(dialog), "activate-link", G_CALLBACK(activate_url), ui);

  gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(dialog),
      "http://mp3splt.sourceforge.net");
  gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog),
      "http://mp3splt.sourceforge.net");

  gtk_about_dialog_set_translator_credits(GTK_ABOUT_DIALOG(dialog),
      "Mario Blättermann <mariobl@gnome.org>");

  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

/*! Removes status bar message

Used for the ok button event.
*/
void remove_status_message(gui_state *gui)
{
  guint status_id = gtk_statusbar_get_context_id(gui->status_bar, "mess");
  gtk_statusbar_pop(gui->status_bar, status_id);
}

/*! Output a info message to the status message bar

The message type is automatically set to SPLT_MESSAGE_INFO.
If you don't want that use put_status_message instead.
\param text The text that has to be displayed.
*/
void put_status_message(const gchar *text, ui_state *ui)
{
  put_status_message_with_type(text, SPLT_MESSAGE_INFO, ui);
}

/*! Output a message to the status message bar.

\param text The text that has to be displayed.
\param splt_message_type The type of the message.

If the type is to be set to SPLT_MESSAGE_INFO put_status_message
can be used instead; The enum for the message types is defined in
libmp3splt.h
 */
void put_status_message_with_type(const gchar *text, splt_message_type mess_type, ui_state *ui)
{
  gui_state *gui = ui->gui;

  if (mess_type == SPLT_MESSAGE_INFO || mess_type == SPLT_MESSAGE_WARNING)
  {
    guint status_id = gtk_statusbar_get_context_id(gui->status_bar, "mess");
    gtk_statusbar_pop(gui->status_bar, status_id);
    gtk_statusbar_push(gui->status_bar, status_id, text);
  }

  put_message_in_history(text, mess_type, ui);
}

void set_stop_split_safe(gboolean value, ui_state *ui)
{
  lock_mutex(&ui->variables_mutex);  
  ui->status->stop_split = value;
  unlock_mutex(&ui->variables_mutex);  
}

//!event for the cancel button
void cancel_button_event(GtkWidget *widget, ui_state *ui)
{
  lmanager_stop_split(ui);

  set_stop_split_safe(TRUE, ui);

  if (widget != NULL)
  {
    gtk_widget_set_sensitive(widget, FALSE);
  }

  put_status_message(_(" info: stopping the split process.. please wait"), ui);
}

void set_is_splitting_safe(gboolean value, ui_state *ui)
{
  lock_mutex(&ui->variables_mutex);
  ui->status->splitting = value;
  unlock_mutex(&ui->variables_mutex);
}

gint get_is_splitting_safe(ui_state *ui)
{
  lock_mutex(&ui->variables_mutex);
  gint is_splitting = ui->status->splitting;
  unlock_mutex(&ui->variables_mutex);
  return is_splitting;
}

static void _set_process_in_progress_safe(gboolean value, ui_state *ui)
{
  lock_mutex(&ui->variables_mutex);
  if (value)
  {
    ui->status->process_in_progress++;
  }
  else
  {
    ui->status->process_in_progress--;
  }
  unlock_mutex(&ui->variables_mutex);
}

void set_process_in_progress_and_wait_safe(gboolean value, ui_state *ui)
{
  if (value == TRUE)
  {
    while (get_process_in_progress_safe(ui))
    {
      g_usleep(G_USEC_PER_SEC / 4);
    }
  }

  _set_process_in_progress_safe(value, ui);
}

void set_process_in_progress_safe(gboolean value, ui_state *ui)
{
#ifdef __WIN32__
  _set_process_in_progress_safe(value, ui);
#endif
}

gint get_process_in_progress_safe(ui_state *ui)
{
  lock_mutex(&ui->variables_mutex);
  gint process_in_progress = ui->status->process_in_progress;
  unlock_mutex(&ui->variables_mutex);
  return process_in_progress > 0;
}

//!event for the split button
void split_button_event(GtkWidget *widget, ui_state *ui)
{
  if (get_is_splitting_safe(ui))
  {
    put_status_message(_(" error: split in progress..."), ui);
    return;
  }

  if (get_output_directory(ui) != NULL)
  {
    split_action(ui);
  }
  else
  {
    put_status_message(_(" error: no path of split selected"), ui);
  }
}

void set_split_file_mode(gint file_mode, ui_state *ui)
{
  ui->infos->split_file_mode = file_mode;
}

gint get_split_file_mode(ui_state *ui)
{
  return ui->infos->split_file_mode;
}

static void single_file_mode_split_button_event(GtkWidget *widget, ui_state *ui)
{
  set_split_file_mode(FILE_MODE_SINGLE, ui);
  split_button_event(widget, ui);
}

static void show_messages_history_window(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  ui_state *ui = (ui_state *) data;
  wh_show_window(ui->gui->mess_history_window);
}

#ifndef NO_GNOME
static void ShowHelp(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  GError* gerror = NULL;
  gtk_show_uri(gdk_screen_get_default(), "ghelp:mp3splt-gtk",  gtk_get_current_event_time(), &gerror);
}
#endif

static void player_pause_action(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  ui_state *ui = (ui_state *) data;
  pause_event(ui->gui->pause_button, ui);
}
 
static void player_seek_forward_action(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  ui_state *ui = (ui_state *) data;

  gfloat total_time = ui->infos->total_time;
  gfloat new_time = ui->infos->current_time * 10 + 
    (ui->infos->seek_jump_value != 0 ? ui->infos->seek_jump_value :
     2./100. * total_time * 10);
  if (new_time > total_time * 10) { new_time = total_time * 10; }
  player_seek(new_time, ui);
}
 
static void player_seek_backward_action(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  ui_state *ui = (ui_state *) data;

  gfloat total_time = ui->infos->total_time;
  gfloat new_time = ui->infos->current_time * 10 - 
    (ui->infos->seek_jump_value != 0 ? ui->infos->seek_jump_value :
     2./100. * total_time * 10);
  if (new_time <= 0) { new_time = 0; }
  player_seek(new_time, ui);
}

static void player_big_seek_forward_action(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  ui_state *ui = (ui_state *) data;

  gfloat total_time = ui->infos->total_time;
  gfloat new_time = ui->infos->current_time * 10 + 
    (ui->infos->big_seek_jump_value != 0 ? ui->infos->big_seek_jump_value :
     15./100. * total_time * 10);
  if (new_time > total_time * 10) { new_time = total_time * 10; }
  player_seek(new_time, ui);
}
 
static void player_big_seek_backward_action(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  ui_state *ui = (ui_state *) data;

  gfloat total_time = ui->infos->total_time;
  gfloat new_time = ui->infos->current_time * 10 - 
    (ui->infos->big_seek_jump_value != 0 ? ui->infos->big_seek_jump_value :
     15./100. * total_time * 10);
  if (new_time <= 0) { new_time = 0; }
  player_seek(new_time, ui);
}

static void player_small_seek_forward_action(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  ui_state *ui = (ui_state *) data;

  gfloat total_time = ui->infos->total_time;
  gfloat new_time = ui->infos->current_time * 10 + ui->infos->small_seek_jump_value;
  if (new_time > total_time * 10) { new_time = total_time * 10; }
  player_seek(new_time, ui);
}
 
static void player_small_seek_backward_action(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  ui_state *ui = (ui_state *) data;

  gfloat new_time = ui->infos->current_time * 10 - ui->infos->small_seek_jump_value;
  if (new_time <= 0) { new_time = 0; }
  player_seek(new_time, ui);
}

static void player_seek_to_next_splitpoint_action(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  ui_state *ui = (ui_state *) data;

  gint time_left = -1;
  gint time_right = -1;
  get_current_splitpoints_time_left_right(&time_left, &time_right, NULL, ui);

  if (time_right != -1)
  {
    player_seek(time_right * 10, ui);
  }
}

static void player_seek_to_previous_splitpoint_action(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  ui_state *ui = (ui_state *) data;

  gint time_left = -1;
  gint time_right = -1;
  get_current_splitpoints_time_left_right(&time_left, &time_right, NULL, ui);

  if (time_left != -1)
  {
    player_seek(time_left * 10, ui);
  }
}

static int find_closest_splitpoint(ui_state *ui)
{
  gint left_index_point = -1;
  gint right_index_point = -1;

  gint i = 0;
  for (i = 0; i < ui->infos->splitnumber; i++ )
  {
    gint current_point_hundr_secs = get_splitpoint_time(i, ui);
    if (current_point_hundr_secs <= ui->infos->current_time)
    {
      left_index_point = i;
      continue;
    }

    if (current_point_hundr_secs >= ui->infos->current_time)
    {
      right_index_point = i;
      break;
    }
  }

  if (left_index_point == -1 && right_index_point == -1)
  {
    return -1;
  }

  gint time_to_left = INT_MAX;
  if (left_index_point != -1)
  {
    time_to_left = ui->infos->current_time - get_splitpoint_time(left_index_point, ui);
  }

  gint time_to_right = INT_MAX;
  if (right_index_point != -1)
  {
    time_to_right = get_splitpoint_time(right_index_point, ui) - ui->infos->current_time;
  }

  if (time_to_right > time_to_left)
  {
    return left_index_point;
  }

  return right_index_point;
}

static void delete_closest_splitpoint(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  ui_state *ui = (ui_state *) data;

  int closest_splitpoint_index = find_closest_splitpoint(ui);
  if (closest_splitpoint_index == -1)
  {
    return;
  }

  remove_splitpoint(closest_splitpoint_index, TRUE, ui);
}

static void player_seek_before_closest_splitpoint(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  ui_state *ui = (ui_state *) data;

  int closest_splitpoint_index = find_closest_splitpoint(ui);
  if (closest_splitpoint_index == -1) { return; }

  gint current_point_hundr_secs = get_splitpoint_time(closest_splitpoint_index, ui);
  player_seek((current_point_hundr_secs * 10) - 100 * 3 * 10, ui);

  if (closest_splitpoint_index == 0) { return; }

  set_preview_start_position_safe(get_splitpoint_time(closest_splitpoint_index - 1, ui), ui);
  ui->status->preview_start_splitpoint = closest_splitpoint_index - 1;
  if (closest_splitpoint_index < ui->infos->splitnumber)
  {
    set_quick_preview_end_splitpoint_safe(closest_splitpoint_index, ui);
  }
  else
  {
    set_quick_preview_end_splitpoint_safe(-1, ui);
  }

  if (player_is_paused(ui))
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ui->gui->pause_button), FALSE);
  }

  ui->status->quick_preview = TRUE;
}

static void player_seek_closest_splitpoint_no_pause(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  ui_state *ui = (ui_state *) data;

  int closest_splitpoint_index = find_closest_splitpoint(ui);
  if (closest_splitpoint_index == -1) { return; }

  gint current_point_hundr_secs = get_splitpoint_time(closest_splitpoint_index, ui);
  player_seek(current_point_hundr_secs * 10, ui);

  set_preview_start_position_safe(current_point_hundr_secs, ui);
  ui->status->preview_start_splitpoint = closest_splitpoint_index;

  if (closest_splitpoint_index < (ui->infos->splitnumber - 1))
  {
    set_quick_preview_end_splitpoint_safe(closest_splitpoint_index + 1, ui);
  }
  else
  {
    set_quick_preview_end_splitpoint_safe(-1, ui);
  }

  if (player_is_paused(ui))
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ui->gui->pause_button), FALSE);
  }

  ui->status->quick_preview = TRUE;
}

static void player_seek_closest_splitpoint(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  ui_state *ui = (ui_state *) data;
  player_seek_closest_splitpoint_no_pause(NULL, NULL, ui);
  ui->status->stop_preview_right_after_start = TRUE;
}

static void zoom_in(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  ui_state *ui = (ui_state *) data;

  gdouble fraction = 40./100. * ui->infos->zoom_coeff;
  ui->infos->zoom_coeff += fraction;
  adjust_zoom_coeff(ui->infos);
  refresh_drawing_area(ui->gui, ui->infos);
}

static void zoom_out(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  ui_state *ui = (ui_state *) data;

  gdouble fraction = 40./100. * ui->infos->zoom_coeff;
  ui->infos->zoom_coeff -= fraction; 
  adjust_zoom_coeff(ui->infos);
  refresh_drawing_area(ui->gui, ui->infos);
}

void add_filters_to_file_chooser(GtkWidget *file_chooser)
{
  GtkFileFilter *our_filter = gtk_file_filter_new();
  gtk_file_filter_set_name(our_filter, _("mp3, ogg vorbis and flac files (*.mp3 *.ogg *.flac)"));
  gtk_file_filter_add_pattern(our_filter, "*.mp3");
  gtk_file_filter_add_pattern(our_filter, "*.ogg");
  gtk_file_filter_add_pattern(our_filter, "*.flac");
  gtk_file_filter_add_pattern(our_filter, "*.MP3");
  gtk_file_filter_add_pattern(our_filter, "*.OGG");
  gtk_file_filter_add_pattern(our_filter, "*.FLAC");
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_chooser), our_filter);

  our_filter = gtk_file_filter_new();
  gtk_file_filter_set_name (our_filter, _("mp3 files (*.mp3)"));
  gtk_file_filter_add_pattern(our_filter, "*.mp3");
  gtk_file_filter_add_pattern(our_filter, "*.MP3");
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_chooser), our_filter);

  our_filter = gtk_file_filter_new();
  gtk_file_filter_set_name (our_filter, _("ogg vorbis files (*.ogg)"));
  gtk_file_filter_add_pattern(our_filter, "*.ogg");
  gtk_file_filter_add_pattern(our_filter, "*.OGG");
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_chooser), our_filter);

  our_filter = gtk_file_filter_new();
  gtk_file_filter_set_name (our_filter, _("flac files (*.flac)"));
  gtk_file_filter_add_pattern(our_filter, "*.flac");
  gtk_file_filter_add_pattern(our_filter, "*.FLAC");
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_chooser), our_filter);
}

static void file_selection_changed(GtkFileChooser *open_file_chooser, ui_state *ui)
{
  gchar *filename = gtk_file_chooser_get_filename(open_file_chooser);
  gchar *previous_fname = get_input_filename(ui->gui);

  if (previous_fname != NULL && filename != NULL && 
      strcmp(filename, previous_fname) == 0)
  {
    return;
  }

  if (filename != NULL)
  {
    ui->status->file_selection_changed = TRUE;
    file_chooser_ok_event(filename, ui);
    ui->status->file_selection_changed = FALSE;

    g_free(filename);
    filename = NULL;
    return;
  }
}

static void file_set_event(GtkFileChooserButton *open_file_chooser_button, ui_state *ui)
{
  file_selection_changed(GTK_FILE_CHOOSER(open_file_chooser_button), ui);
}

static GtkWidget *create_choose_file_frame(ui_state *ui)
{
  GtkWidget *open_file_chooser_button = gtk_file_chooser_button_new(_("Open file ..."), GTK_FILE_CHOOSER_ACTION_OPEN);
  dnd_add_drag_data_received_to_widget(open_file_chooser_button, DND_SINGLE_MODE_AUDIO_FILE, ui);

  ui->gui->open_file_chooser_button = open_file_chooser_button;
  add_filters_to_file_chooser(open_file_chooser_button);
  wh_set_browser_directory_handler(ui, open_file_chooser_button);

  g_signal_connect(G_OBJECT(open_file_chooser_button), "file-set", G_CALLBACK(file_set_event), ui);
  g_signal_connect(G_OBJECT(open_file_chooser_button), "selection-changed",
      G_CALLBACK(file_selection_changed), ui);

  gchar *fname = get_input_filename(ui->gui);
  if (fname != NULL && strlen(fname) != 0)
  {
    gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(open_file_chooser_button), get_input_filename(ui->gui));
  }

  return open_file_chooser_button;
}

//!main vbox
static GtkWidget *create_main_vbox(ui_state *ui)
{
  GtkWidget *main_vbox = wh_vbox_new();
  gtk_container_set_border_width(GTK_CONTAINER(main_vbox), 0);

  /* tabbed notebook */
  GtkWidget *notebook = gtk_notebook_new();

  gtk_box_pack_start(GTK_BOX(main_vbox), notebook, TRUE, TRUE, 0);
  gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), TRUE);
  gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), FALSE);
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
   
  /* player page */
  GtkWidget *player_vbox = wh_vbox_new();

  //file & split button hbox
  GtkWidget *top_hbox = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(player_vbox), top_hbox, FALSE, FALSE, 0);

  //choose file
  gtk_box_pack_start(GTK_BOX(top_hbox), create_choose_file_frame(ui), TRUE, TRUE, 0);

  //single mode split button
  GtkWidget *split_button = wh_create_cool_button("system-run",_("Split"), FALSE);
  g_signal_connect(G_OBJECT(split_button), "clicked",
      G_CALLBACK(single_file_mode_split_button_event), ui);
  gtk_widget_set_tooltip_text(split_button, _("Split the current file"));
  gtk_box_pack_start(GTK_BOX(top_hbox), split_button, FALSE, FALSE, 4);

  ui->gui->player_box = create_player_control_frame(ui);
  gtk_box_pack_start(GTK_BOX(player_vbox), ui->gui->player_box, FALSE, FALSE, 0);

  ui->gui->playlist_box = create_player_playlist_frame(ui);
  gtk_box_pack_start(GTK_BOX(player_vbox), ui->gui->playlist_box, TRUE, TRUE, 0);

  GtkWidget *notebook_label = wh_create_cool_label(NULL, _("Manual single file split"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), player_vbox, notebook_label);

  /* splitpoints page */
  ui->gui->splitpoints_widget = create_splitpoints_frame(ui);

  /* split files page */
  ui->gui->split_files_widget = create_split_files_frame(ui);
  
  /* freedb page */
  ui->gui->freedb_widget = create_freedb_frame(ui);
  
  /* special split page */
  GtkWidget *special_split_vbox = wh_vbox_new();
  gtk_container_set_border_width(GTK_CONTAINER(special_split_vbox), 0);
  GtkWidget *frame = create_special_split_page(ui);
  gtk_box_pack_start(GTK_BOX(special_split_vbox), frame, TRUE, TRUE, 0);

  notebook_label = wh_create_cool_label(NULL, _("Batch & automatic split"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), special_split_vbox, notebook_label);
 
  /* preferences widget */
  ui->gui->preferences_widget = create_choose_preferences(ui);

  /* progress bar */
  GtkProgressBar *percent_progress_bar = GTK_PROGRESS_BAR(gtk_progress_bar_new());
  ui->gui->percent_progress_bar = percent_progress_bar;
  gtk_progress_bar_set_fraction(percent_progress_bar, 0.0);
  gtk_progress_bar_set_text(percent_progress_bar, "");

  gtk_progress_bar_set_show_text(percent_progress_bar, TRUE);

  GtkWidget *hbox = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(percent_progress_bar), TRUE, TRUE, 0);

  //stop button
  GtkWidget *cancel_button = wh_create_cool_button("process-stop", _("S_top"), FALSE);
  ui->gui->cancel_button = cancel_button;
  g_signal_connect(G_OBJECT(cancel_button), "clicked", G_CALLBACK(cancel_button_event), ui);
  gtk_box_pack_start(GTK_BOX(hbox), cancel_button, FALSE, TRUE, 3);
  gtk_widget_set_sensitive(cancel_button, FALSE);

  gtk_box_pack_start(GTK_BOX(main_vbox), hbox, FALSE, FALSE, 2);

  /* show messages history dialog */
  create_mess_history_window(ui);
 
  /* statusbar */
  GtkStatusbar *status_bar = GTK_STATUSBAR(gtk_statusbar_new());
  ui->gui->status_bar = status_bar;

  gtk_box_pack_start(GTK_BOX(main_vbox), GTK_WIDGET(status_bar), FALSE, FALSE, 0);

  return main_vbox;
}

static void move_and_resize_main_window(ui_state *ui)
{
  const ui_main_window *main_win = ui_get_main_window_infos(ui);

  gint x = main_win->root_x_pos;
  gint y = main_win->root_y_pos;

  if (x != 0 && y != 0)
  {
    gtk_window_move(GTK_WINDOW(ui->gui->window), x, y);
  }
  else
  {
    gtk_window_set_position(GTK_WINDOW(ui->gui->window), GTK_WIN_POS_CENTER);
  }

  gtk_window_resize(GTK_WINDOW(ui->gui->window), main_win->width, main_win->height);
}

static void application_startup(GApplication *app, ui_state *ui)
{
  GtkBuilder *builder = gtk_builder_new();

  gtk_builder_add_from_string(builder,
      "<?xml version=\"1.0\" ?>"
      "<interface>"

      "  <menu id=\"appmenu\">"
      "    <section>"
      "      <item>"
      "        <attribute name=\"label\" translatable=\"yes\">_Preferences</attribute>"
      "        <attribute name=\"action\">app.preferences</attribute>"
      "        <attribute name=\"accel\">&lt;Primary&gt;p</attribute>"
      "      </item>"
      "      <item>"
      "        <attribute name=\"label\" translatable=\"yes\">_Quit</attribute>"
      "        <attribute name=\"action\">app.quit</attribute>"
      "        <attribute name=\"accel\">&lt;Primary&gt;q</attribute>"
      "      </item>"
      "    </section>"
      "  </menu>"

      "  <menu id=\"menubar\">"

      "    <submenu>"
      "      <attribute name=\"label\" translatable=\"yes\">_File</attribute>"
      "      <section>"
      "        <item>"
      "          <attribute name=\"label\" translatable=\"yes\">_Open single file...</attribute>"
      "          <attribute name=\"action\">app.open_single_file</attribute>"
      "          <attribute name=\"accel\">&lt;Primary&gt;o</attribute>"
      "        </item>"
      "        <item>"
      "          <attribute name=\"label\" translatable=\"yes\">_Add files or directories to batch...</attribute>"
      "          <attribute name=\"action\">app.add_files_to_batch</attribute>"
      "          <attribute name=\"accel\">&lt;Primary&gt;d</attribute>"
      "        </item>"
      "      </section>"
      "      <section>"
      "        <item>"
      "          <attribute name=\"label\" translatable=\"yes\">_Import splitpoints from file...</attribute>"
      "          <attribute name=\"action\">app.import_splitpoints_from_file</attribute>"
      "          <attribute name=\"accel\">&lt;Primary&gt;i</attribute>"
      "        </item>"
      "        <item>"
      "          <attribute name=\"label\" translatable=\"yes\">_Import splitpoints from _TrackType.org...</attribute>"
      "          <attribute name=\"action\">app.import_splitpoints_from_tracktype</attribute>"
      "          <attribute name=\"accel\">&lt;Primary&gt;t</attribute>"
      "        </item>"
      "        <item>"
      "          <attribute name=\"label\" translatable=\"yes\">_Export splitpoints...</attribute>"
      "          <attribute name=\"action\">app.export_splitpoints</attribute>"
      "          <attribute name=\"accel\">&lt;Primary&gt;e</attribute>"
      "        </item>"
      "      </section>"
      "      <section>"
      "        <item>"
      "          <attribute name=\"label\" translatable=\"yes\">_Split</attribute>"
      "          <attribute name=\"action\">app.split</attribute>"
      "          <attribute name=\"accel\">&lt;Primary&gt;s</attribute>"
      "        </item>"
      "        <item>"
      "          <attribute name=\"label\" translatable=\"yes\">_Batch split</attribute>"
      "          <attribute name=\"action\">app.batch_split</attribute>"
      "          <attribute name=\"accel\">&lt;Primary&gt;b</attribute>"
      "        </item>"
      "      </section>"
      "    </submenu>"

      "    <submenu>"
      "      <attribute name=\"label\" translatable=\"yes\">_View</attribute>"
      "      <section>"
      "        <item>"
      "          <attribute name=\"label\" translatable=\"yes\">_Splitpoints</attribute>"
      "          <attribute name=\"action\">app.view_splitpoints</attribute>"
      "          <attribute name=\"accel\">&lt;Primary&gt;l</attribute>"
      "        </item>"
      "        <item>"
      "          <attribute name=\"label\" translatable=\"yes\">Split _files</attribute>"
      "          <attribute name=\"action\">app.view_split_files</attribute>"
      "          <attribute name=\"accel\">&lt;Primary&gt;f</attribute>"
      "        </item>"
      "      </section>"
      "    </submenu>"

      "    <submenu>"
      "      <attribute name=\"label\" translatable=\"yes\">_Player</attribute>"
      "      <section>"
      "        <item>"
      "          <attribute name=\"label\" translatable=\"yes\">P_ause / Play</attribute>"
      "          <attribute name=\"action\">app.pause_play</attribute>"
      "          <attribute name=\"accel\">space</attribute>"
      "        </item>"
      "      </section>"

      "      <section>"
      "        <item>"
      "          <attribute name=\"label\" translatable=\"yes\">Seek _forward</attribute>"
      "          <attribute name=\"action\">app.seek_forward</attribute>"
      "          <attribute name=\"accel\">Right</attribute>"
      "        </item>"
      "        <item>"
      "          <attribute name=\"label\" translatable=\"yes\">Seek _backward</attribute>"
      "          <attribute name=\"action\">app.seek_backward</attribute>"
      "          <attribute name=\"accel\">Left</attribute>"
      "        </item>"
      "        <item>"
      "          <attribute name=\"label\" translatable=\"yes\">Small seek f_orward</attribute>"
      "          <attribute name=\"action\">app.small_seek_forward</attribute>"
      "          <attribute name=\"accel\">&lt;Alt&gt;Right</attribute>"
      "        </item>"
      "        <item>"
      "          <attribute name=\"label\" translatable=\"yes\">Small seek back_ward</attribute>"
      "          <attribute name=\"action\">app.small_seek_backward</attribute>"
      "          <attribute name=\"accel\">&lt;Alt&gt;Left</attribute>"
      "        </item>"
      "        <item>"
      "          <attribute name=\"label\" translatable=\"yes\">Big seek fo_rward</attribute>"
      "          <attribute name=\"action\">app.big_seek_forward</attribute>"
      "          <attribute name=\"accel\">&lt;Shift&gt;Right</attribute>"
      "        </item>"
      "        <item>"
      "          <attribute name=\"label\" translatable=\"yes\">Big seek bac_kward</attribute>"
      "          <attribute name=\"action\">app.big_seek_backward</attribute>"
      "          <attribute name=\"accel\">&lt;Shift&gt;Left</attribute>"
      "        </item>"
      "        <item>"
      "          <attribute name=\"label\" translatable=\"yes\">Seek to _next splitpoint</attribute>"
      "          <attribute name=\"action\">app.seek_next_splitpoint</attribute>"
      "          <attribute name=\"accel\">&lt;Primary&gt;Right</attribute>"
      "        </item>"
      "        <item>"
      "          <attribute name=\"label\" translatable=\"yes\">Seek to _previous splitpoint</attribute>"
      "          <attribute name=\"action\">app.seek_previous_splitpoint</attribute>"
      "          <attribute name=\"accel\">&lt;Primary&gt;Left</attribute>"
      "        </item>"
      "      </section>"

      "      <section>"
      "        <item>"
      "          <attribute name=\"label\" translatable=\"yes\">Preview clos_est splitpoint</attribute>"
      "          <attribute name=\"action\">app.preview_closest</attribute>"
      "          <attribute name=\"accel\">&lt;Shift&gt;Up</attribute>"
      "        </item>"
      "        <item>"
      "          <attribute name=\"label\" translatable=\"yes\">Preview _closest splitpoint &amp; pause</attribute>"
      "          <attribute name=\"action\">app.preview_closest_and_pause</attribute>"
      "          <attribute name=\"accel\">&lt;Primary&gt;Up</attribute>"
      "        </item>"
      "        <item>"
      "          <attribute name=\"label\" translatable=\"yes\">Preview before c_losest splitpoint</attribute>"
      "          <attribute name=\"action\">app.preview_before_closest</attribute>"
      "          <attribute name=\"accel\">&lt;Primary&gt;Down</attribute>"
      "        </item>"
      "      </section>"

      "      <section>"
      "        <item>"
      "          <attribute name=\"label\" translatable=\"yes\">Add _splitpoint</attribute>"
      "          <attribute name=\"action\">app.add_splitpoint</attribute>"
      "          <attribute name=\"accel\">s</attribute>"
      "        </item>"
      "        <item>"
      "          <attribute name=\"label\" translatable=\"yes\">_Delete closest splitpoint</attribute>"
      "          <attribute name=\"action\">app.delete_closest_splitpoint</attribute>"
      "          <attribute name=\"accel\">d</attribute>"
      "        </item>"
      "      </section>"

      "      <section>"
      "        <item>"
      "          <attribute name=\"label\" translatable=\"yes\">Zoom _in</attribute>"
      "          <attribute name=\"action\">app.zoom_in</attribute>"
      "          <attribute name=\"accel\">&lt;Primary&gt;plus</attribute>"
      "        </item>"
      "        <item>"
      "          <attribute name=\"label\" translatable=\"yes\">Zoom _out</attribute>"
      "          <attribute name=\"action\">app.zoom_out</attribute>"
      "          <attribute name=\"accel\">&lt;Primary&gt;minus</attribute>"
      "        </item>"
      "      </section>"
      "    </submenu>"

      "    <submenu>"
      "      <attribute name=\"label\" translatable=\"yes\">_Help</attribute>"
      "      <section>"
      "        <item>"
      "          <attribute name=\"label\" translatable=\"yes\">Messages _history</attribute>"
      "          <attribute name=\"action\">app.messages_history</attribute>"
      "          <attribute name=\"accel\">&lt;Primary&gt;h</attribute>"
      "        </item>"
#ifndef NO_GNOME
      "        <item>"
      "          <attribute name=\"label\" translatable=\"yes\">_Contents</attribute>"
      "          <attribute name=\"action\">app.contents</attribute>"
      "          <attribute name=\"accel\">F1</attribute>"
      "        </item>"
#endif
      "        <item>"
      "          <attribute name=\"label\" translatable=\"yes\">_About</attribute>"
      "          <attribute name=\"action\">app.about</attribute>"
      "          <attribute name=\"accel\">&lt;Primary&gt;a</attribute>"
      "        </item>"
      "      </section>"
      "    </submenu>"

      "  </menu>"
      "</interface>", -1, NULL);

  GMenuModel *appmenu = (GMenuModel *) gtk_builder_get_object(builder, "appmenu");
  gtk_application_set_app_menu(GTK_APPLICATION(app), appmenu);

  GMenuModel *menubar = (GMenuModel *) gtk_builder_get_object (builder, "menubar");
  gtk_application_set_menubar(GTK_APPLICATION(app), menubar);

  g_object_unref(builder);
}

static void parse_command_line_options(gint argc, gchar * argv[], ui_state *ui)
{
  opterr = 0;
  int option;
  while ((option = getopt(argc, argv, "d:")) != -1)
  {
    switch (option)
    {
      case 'd':
        fprintf(stdout, _("Setting the output directory to %s.\n"), optarg);
        set_output_directory_and_update_ui((gchar *)optarg, ui);
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
  char *input_filename = realpath(argv[optind], NULL);
  import_file(input_filename, ui, FALSE);
  free(input_filename);
#else
  import_file(argv[optind], ui, FALSE);
#endif
}

static void application_activate(GApplication *app, ui_state *ui)
{
  initialize_window(app, ui);

  GtkWidget *window_vbox = wh_vbox_new();
  gtk_container_add(GTK_CONTAINER(ui->gui->window), window_vbox);

  gtk_box_pack_start(GTK_BOX(window_vbox), create_main_vbox(ui), TRUE, TRUE, 0);

  ui_load_preferences(ui);

  move_and_resize_main_window(ui);

  gtk_widget_show_all(ui->gui->window);

  if (ui->infos->selected_player != PLAYER_GSTREAMER)
  {
    gtk_widget_hide(ui->gui->playlist_box);
  }

  hide_freedb_spinner(ui->gui);

  import_cue_file_from_the_configuration_directory(ui);

  parse_command_line_options(ui->argc, ui->argv, ui);
}

static void open_file_menu_action(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  ui_state *ui = (ui_state *) data;

  GtkWidget *file_chooser = gtk_file_chooser_dialog_new(_("Choose File"), NULL,
      GTK_FILE_CHOOSER_ACTION_OPEN,
      _("_Cancel"),
      GTK_RESPONSE_CANCEL,
      _("_Open"),
      GTK_RESPONSE_ACCEPT, NULL);

  add_filters_to_file_chooser(file_chooser);
  wh_set_browser_directory_handler(ui, file_chooser);

  if (gtk_dialog_run(GTK_DIALOG(file_chooser)) == GTK_RESPONSE_ACCEPT)
  {
    gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));
    file_chooser_ok_event(filename, ui);
    if (filename)
    {
      g_free(filename);
      filename = NULL;
    }
  }

  gtk_widget_destroy(file_chooser);
  remove_status_message(ui->gui);
}

static void multiple_files_add_menu_action(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  ui_state *ui = (ui_state *) data;
  multiple_files_add_button_event(NULL, ui);
}

static void import_event_menu_action(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  ui_state *ui = (ui_state *) data;
  import_event(NULL, ui);
}

static void show_tracktype_window_menu_action(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  ui_state *ui = (ui_state *) data;

  if (ui->gui->freedb_window == NULL)
  {
    ui->gui->freedb_window = 
      wh_create_window_with_close_button(_("TrackType"), 500, 300, GTK_WIN_POS_CENTER, 
          GTK_WINDOW(ui->gui->window),
          ui->gui->freedb_widget, 
          ui->gui->freedb_add_button, NULL);
  }

  wh_show_window(ui->gui->freedb_window);
  hide_freedb_spinner(ui->gui);
}

static void show_preferences_window_menu_action(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  ui_state *ui = (ui_state *) data;

  if (ui->gui->preferences_window == NULL)
  {
    ui->gui->preferences_window = 
      wh_create_window_with_close_button(_("Preferences"), 750, 450, GTK_WIN_POS_CENTER, 
          GTK_WINDOW(ui->gui->window), ui->gui->preferences_widget, NULL);
  }

  wh_show_window(ui->gui->preferences_window);
}

static void split_menu_action(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  ui_state *ui = (ui_state *) data;
  single_file_mode_split_button_event(NULL, ui);
}

static void batch_split_menu_action(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  ui_state *ui = (ui_state *) data;
  batch_file_mode_split_button_event(NULL, ui);
}

static void exit_application_menu_action(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  ui_state *ui = (ui_state *) data;
  exit_application(NULL, NULL, ui);
}

static void show_splitpoints_window_menu_action(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  ui_state *ui = (ui_state *) data;

  if (ui->gui->splitpoints_window == NULL)
  {
    ui->gui->splitpoints_window = 
      wh_create_window_with_close_button(_("Splitpoints"), 500, 300, GTK_WIN_POS_CENTER, 
          GTK_WINDOW(ui->gui->window),
          ui->gui->splitpoints_widget, 
          ui->gui->scan_trim_silence_button, ui->gui->scan_silence_button, NULL);
  }

  wh_show_window(ui->gui->splitpoints_window);
}

static void show_split_files_window_menu_action(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  ui_state *ui = (ui_state *) data;

  if (ui->gui->split_files_window == NULL)
  {
    ui->gui->split_files_window = 
      wh_create_window_with_close_button(_("Split files"), 500, 300, GTK_WIN_POS_CENTER, 
          GTK_WINDOW(ui->gui->window),
          ui->gui->split_files_widget, 
          ui->gui->queue_files_button, ui->gui->remove_file_button,
          ui->gui->remove_all_files_button, NULL);
  }

  wh_show_window(ui->gui->split_files_window);
}

static void add_splitpoint_from_player_menu_action(GSimpleAction *action, GVariant *parameter, gpointer data)
{
  ui_state *ui = (ui_state *) data;
  add_splitpoint_from_player(NULL, ui);
}

const static GActionEntry app_entries[] = {
  {"open_single_file", open_file_menu_action},
  {"add_files_to_batch", multiple_files_add_menu_action},
  {"import_splitpoints_from_file", import_event_menu_action},
  {"import_splitpoints_from_tracktype", show_tracktype_window_menu_action},
  {"export_splitpoints", export_menu_action},
  {"preferences", show_preferences_window_menu_action},
  {"split", split_menu_action},
  {"batch_split", batch_split_menu_action},
  {"quit", exit_application_menu_action},

  {"view_splitpoints", show_splitpoints_window_menu_action},
  {"view_split_files", show_split_files_window_menu_action},

  {"pause_play", player_pause_action},
  {"seek_forward", player_seek_forward_action},
  {"seek_backward", player_seek_backward_action},
  {"small_seek_forward", player_small_seek_forward_action},
  {"small_seek_backward", player_small_seek_backward_action},
  {"big_seek_forward", player_big_seek_forward_action},
  {"big_seek_backward", player_big_seek_backward_action},
  {"seek_next_splitpoint", player_seek_to_next_splitpoint_action},
  {"seek_previous_splitpoint", player_seek_to_previous_splitpoint_action},
  {"preview_closest", player_seek_closest_splitpoint_no_pause},
  {"preview_closest_and_pause", player_seek_closest_splitpoint},
  {"preview_before_closest", player_seek_before_closest_splitpoint},
  {"add_splitpoint", add_splitpoint_from_player_menu_action},
  {"delete_closest_splitpoint", delete_closest_splitpoint},
  {"zoom_in", zoom_in},
  {"zoom_out", zoom_out},

  {"messages_history", show_messages_history_window},
#ifndef NO_GNOME
  {"contents", ShowHelp},
#endif
  {"about", about_window}
};

void create_application(ui_state *ui)
{
  GtkApplication *app = gtk_application_new("net.sf.mp3splt", G_APPLICATION_FLAGS_NONE);
  ui->gui->application = app;

  g_action_map_add_action_entries(G_ACTION_MAP(app), app_entries, G_N_ELEMENTS(app_entries), ui);

  player_key_actions_set_sensitivity(FALSE, ui->gui);

  g_signal_connect(app, "startup", G_CALLBACK(application_startup), ui);
  g_signal_connect(app, "activate", G_CALLBACK(application_activate), ui);
}

/*!Output an error message from libmp3splt to the status bar

  \param The error number from the library.
 */
void print_status_bar_confirmation(gint error, ui_state *ui)
{
  char *error_from_library = mp3splt_get_strerror(ui->mp3splt_state, error);
  if (error_from_library == NULL) { return; }

  put_status_message(error_from_library, ui);
  free(error_from_library);
  error_from_library = NULL;
}

static gboolean put_status_message_idle(ui_with_fname *ui_fname)
{
  put_status_message(ui_fname->fname, ui_fname->ui);

  g_free(ui_fname->fname);
  g_free(ui_fname);

  return FALSE;
}

void put_status_message_in_idle(const gchar *text, ui_state *ui)
{
  if (text == NULL) { return; }

  ui_with_fname *ui_fname = g_malloc0(sizeof(ui_with_fname));
  ui_fname->ui = ui;
  ui_fname->fname = strdup(text);
  if (ui_fname->fname == NULL)
  {
    g_free(ui_fname);
    return;
  }

  add_idle(G_PRIORITY_HIGH_IDLE,
      (GSourceFunc)put_status_message_idle, ui_fname, NULL);
}

void print_status_bar_confirmation_in_idle(gint error, ui_state *ui)
{
  char *error_from_library = mp3splt_get_strerror(ui->mp3splt_state, error);
  put_status_message_in_idle(error_from_library, ui);
}

