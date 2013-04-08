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

  lock_mutex(&ui->variables_mutex);
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

  unlock_mutex(&ui->variables_mutex);
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

  refresh_drawing_area(ui->gui);
  refresh_preview_drawing_areas(ui->gui);

  clear_previous_distances(ui);

  return FALSE;
}

static void initialize_window(ui_state *ui)
{
  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
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

static void about_window(GtkWidget *widget, ui_state *ui)
{
  GtkWidget *dialog = gtk_about_dialog_new();

  GString *imagefile = g_string_new("");
  build_path(imagefile, PIXMAP_PATH, "mp3splt-gtk.png");
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(imagefile->str, NULL);
  gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), pixbuf);
  g_string_free(imagefile, TRUE);
  
  gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), (gchar *)PACKAGE_NAME);
  gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), VERSION);
  gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog),
                                 PACKAGE_NAME" : Copyright © 2005-2013 Alexandru"
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

  if (mess_type == SPLT_MESSAGE_INFO)
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

static void show_preferences_window(GtkWidget *widget, ui_state *ui)
{
  if (ui->gui->preferences_window == NULL)
  {
    ui->gui->preferences_window = 
      wh_create_window_with_close_button(_("Preferences"), 600, 450, GTK_WIN_POS_CENTER, 
          GTK_WINDOW(ui->gui->window), ui->gui->preferences_widget, NULL);
  }

  wh_show_window(ui->gui->preferences_window);
}

static void show_tracktype_window(GtkWidget *widget, ui_state *ui)
{
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

static void show_split_files_window(GtkWidget *widget, ui_state *ui)
{
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

static void show_splitpoints_window(GtkWidget *widget, ui_state *ui)
{
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

void set_split_file_mode_safe(gint file_mode, ui_state *ui)
{
  lock_mutex(&ui->variables_mutex);
  ui->infos->split_file_mode = file_mode;
  unlock_mutex(&ui->variables_mutex);
}

gint get_split_file_mode_safe(ui_state *ui)
{
  lock_mutex(&ui->variables_mutex);
  gint file_mode = ui->infos->split_file_mode;
  unlock_mutex(&ui->variables_mutex);

  return file_mode;
}

static void single_file_mode_split_button_event(GtkWidget *widget, ui_state *ui)
{
  set_split_file_mode_safe(FILE_MODE_SINGLE, ui);
  split_button_event(widget, ui);
}

//!event for the "messages history" button
static void show_messages_history_window(GtkWidget *widget, ui_state *ui)
{
  wh_show_window(ui->gui->mess_history_window);
}

#ifndef NO_GNOME
static void ShowHelp(GtkWidget *widget, ui_state *ui)
{
  GError* gerror = NULL;
  gtk_show_uri(gdk_screen_get_default(), "ghelp:mp3splt-gtk",  gtk_get_current_event_time(), &gerror);
}
#endif

static gchar *my_dgettext(const gchar *key, const gchar *domain)
{
  return dgettext("mp3splt-gtk", key);
}

static void player_pause_action(GtkWidget *widget, ui_state *ui)
{
  pause_event(ui->gui->pause_button, ui);
}
 
static void player_seek_forward_action(GtkWidget *widget, ui_state *ui)
{
  gfloat total_time = ui->infos->total_time;
  gfloat new_time = ui->infos->current_time * 10 + 2./100. * total_time * 10;
  if (new_time > total_time * 10) { new_time = total_time * 10; }
  player_seek(new_time, ui);
}
 
static void player_seek_backward_action(GtkWidget *widget, ui_state *ui)
{
  gfloat total_time = ui->infos->total_time;
  gfloat new_time = ui->infos->current_time * 10 - 2./100. * total_time * 10;
  if (new_time <= 0) { new_time = 0; }
  player_seek(new_time, ui);
}

static void player_big_seek_forward_action(GtkWidget *widget, ui_state *ui)
{
  gfloat total_time = ui->infos->total_time;
  gfloat new_time = ui->infos->current_time * 10 + 15./100. * total_time * 10;
  if (new_time > total_time * 10) { new_time = total_time * 10; }
  player_seek(new_time, ui);
}
 
static void player_big_seek_backward_action(GtkWidget *widget, ui_state *ui)
{
  gfloat total_time = ui->infos->total_time;
  gfloat new_time = ui->infos->current_time * 10 - 15./100. * total_time * 10;
  if (new_time <= 0) { new_time = 0; }
  player_seek(new_time, ui);
}

static void player_small_seek_forward_action(GtkWidget *widget, ui_state *ui)
{
  gfloat total_time = ui->infos->total_time;
  gfloat new_time = ui->infos->current_time * 10 + 100 * 3 * 10;
  if (new_time > total_time * 10) { new_time = total_time * 10; }
  player_seek(new_time, ui);
}
 
static void player_small_seek_backward_action(GtkWidget *widget, ui_state *ui)
{
  gfloat new_time = ui->infos->current_time * 10 - 100 * 3 * 10;
  if (new_time <= 0) { new_time = 0; }
  player_seek(new_time, ui);
}

static void player_seek_to_next_splitpoint_action(GtkWidget *widget, ui_state *ui)
{
  gint time_left = -1;
  gint time_right = -1;
  get_current_splitpoints_time_left_right(&time_left, &time_right, NULL, ui);

  if (time_right != -1)
  {
    player_seek(time_right * 10, ui);
  }
}

static void player_seek_to_previous_splitpoint_action(GtkWidget *widget, ui_state *ui)
{
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

static void delete_closest_splitpoint(GtkWidget *widget, ui_state *ui)
{
  int closest_splitpoint_index = find_closest_splitpoint(ui);
  if (closest_splitpoint_index == -1)
  {
    return;
  }

  remove_splitpoint(closest_splitpoint_index, TRUE, ui);
}

static void player_seek_before_closest_splitpoint(GtkWidget *widget, ui_state *ui)
{
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

static void zoom_in(GtkWidget *widget, ui_state *ui)
{
  gdouble fraction = 40./100. * ui->infos->zoom_coeff;
  ui->infos->zoom_coeff += fraction;
  adjust_zoom_coeff(ui->infos);
  refresh_drawing_area(ui->gui);
}

static void zoom_out(GtkWidget *widget, ui_state *ui)
{
  gdouble fraction = 40./100. * ui->infos->zoom_coeff;
  ui->infos->zoom_coeff -= fraction; 
  adjust_zoom_coeff(ui->infos);
  refresh_drawing_area(ui->gui);
}

static gboolean window_key_press_event(GtkWidget *window, GdkEventKey *event, ui_state *ui)
{
  if (event->type != GDK_KEY_PRESS) { return FALSE; }

  if (event->state != 0)
  {
    return FALSE;
  }

  switch (event->keyval)
  {
    case GDK_Left:
      player_seek_backward_action(NULL, ui);
      return TRUE;
    case GDK_Right:
      player_seek_forward_action(NULL, ui);
      return TRUE;
    default:
      return FALSE;
  }
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

/*! \brief Events for browse button

Also used for the cddb and cue browses.
*/
static void open_file_button_event(GtkWidget *widget, ui_state *ui)
{
  GtkWidget *file_chooser = gtk_file_chooser_dialog_new(_("Choose File"), NULL,
      GTK_FILE_CHOOSER_ACTION_OPEN,
      GTK_STOCK_CANCEL,
      GTK_RESPONSE_CANCEL,
      GTK_STOCK_OPEN,
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

//!creates the menu bar
static GtkWidget *create_menu_bar(ui_state *ui)
{
  static const GtkActionEntry entries[] = {
    //name, stock id, label, accelerator, tooltip, callback
    { "FileMenu", NULL, N_("_File"), NULL, NULL, NULL },
    { "ViewMenu", NULL, N_("_View"), NULL, NULL, NULL },
    { "PlayerMenu", NULL, N_("_Player"), NULL, NULL, NULL },
    { "HelpMenu", NULL, N_("_Help"), NULL, NULL, NULL },

    { "Open", GTK_STOCK_OPEN, N_("_Open single file..."), "<Ctrl>O", N_("Open"),
      G_CALLBACK(open_file_button_event) },

    { "AddFilesToBatch", GTK_STOCK_DIRECTORY,
      N_("_Add files or directories to batch ..."), "<Ctrl>D", 
      N_("Add files or directories to batch"),
      G_CALLBACK(multiple_files_add_button_event) },

    { "Import", GTK_STOCK_FILE, N_("_Import splitpoints from file..."), "<Ctrl>I", 
      N_("Import splitpoints from file..."), G_CALLBACK(import_event) },

    { "ImportFromTrackType", GTK_STOCK_FIND, N_("Import splitpoints from _TrackType.org..."), "<Ctrl>T",
      N_("Import splitpoints from TrackType.org..."),
      G_CALLBACK(show_tracktype_window) },

    { "Export", GTK_STOCK_SAVE_AS, N_("_Export splitpoints..."), "<Ctrl>E",
      N_("Export splitpoints"), G_CALLBACK(export_cue_file_event) },

    { "Preferences", GTK_STOCK_PREFERENCES, N_("_Preferences"), "<Ctrl>P", N_("Preferences"),
      G_CALLBACK(show_preferences_window) },

    { "SplitFiles", NULL, N_("Split _files"), "<Ctrl>F", N_("Split files"),
      G_CALLBACK(show_split_files_window) },

    { "Splitpoints", GTK_STOCK_EDIT, N_("_Splitpoints"), "<Ctrl>L", N_("Splitpoints"),
      G_CALLBACK(show_splitpoints_window) },

    { "Split", GTK_STOCK_APPLY, N_("_Split !"), "<Ctrl>S", N_("Split !"),
      G_CALLBACK(single_file_mode_split_button_event) },

    { "BatchSplit", GTK_STOCK_EXECUTE, N_("_Batch split !"), "<Ctrl>B", N_("Batch split !"),
      G_CALLBACK(batch_file_mode_split_button_event) },

    { "Quit", GTK_STOCK_QUIT, N_("_Quit"), "<Ctrl>Q", N_("Quit"),
      G_CALLBACK(exit_application_bis) },

#ifndef NO_GNOME
    { "Contents", GTK_STOCK_HELP, N_("_Contents"), "F1", N_("Contents"),
      G_CALLBACK(ShowHelp)},
#endif

    { "Messages history", GTK_STOCK_INFO, N_("Messages _history"), "<Ctrl>H", N_("Messages history"),
      G_CALLBACK(show_messages_history_window) },

    { "About", GTK_STOCK_ABOUT, N_("_About"), "<Ctrl>A", N_("About"),
      G_CALLBACK(about_window)},

    //player key bindings
    { "Player_pause", NULL, N_("P_ause / Play"), "space", N_("Pause/Play"),
      G_CALLBACK(player_pause_action)},

    { "Player_forward", GTK_STOCK_MEDIA_FORWARD, N_("Seek _forward"), "Right", N_("Seek forward"),
      G_CALLBACK(player_seek_forward_action)},
    { "Player_backward", GTK_STOCK_MEDIA_REWIND, N_("Seek _backward"), "Left", N_("Seek backward"),
      G_CALLBACK(player_seek_backward_action)},

    { "Player_small_forward", NULL, N_("Small seek f_orward"), "<Alt>Right", N_("Small seek forward"),
      G_CALLBACK(player_small_seek_forward_action)},
    { "Player_small_backward", NULL, N_("Small seek back_ward"), "<Alt>Left", N_("Small seek backward"),
      G_CALLBACK(player_small_seek_backward_action)},

    { "Player_big_forward", NULL, N_("Big seek fo_rward"), "<Shift>Right", N_("Big seek forward"),
      G_CALLBACK(player_big_seek_forward_action)},
    { "Player_big_backward", NULL, N_("Big seek bac_kward"), "<Shift>Left", N_("Big seek backward"),
      G_CALLBACK(player_big_seek_backward_action)},

    { "Player_next_splitpoint", GTK_STOCK_MEDIA_NEXT, N_("Seek to _next splitpoint"), "<Ctrl>Right", 
      N_("Seek to next splitpoint"), G_CALLBACK(player_seek_to_next_splitpoint_action)},
    { "Player_previous_splitpoint", GTK_STOCK_MEDIA_PREVIOUS, N_("Seek to _previous splitpoint"), "<Ctrl>Left", 
      N_("Seek to previous splitpoint"), G_CALLBACK(player_seek_to_previous_splitpoint_action)},

    { "Player_before_closest_splitpoint", GTK_STOCK_JUMP_TO,
      N_("_Quick preview before closest splitpoint"), "<Ctrl>Down", N_("Quick preview before closest splitpoint"),
      G_CALLBACK(player_seek_before_closest_splitpoint)},

    { "Add_splitpoint", GTK_STOCK_ADD, N_("Add _splitpoint"), "s", 
      N_("Add splitpoint"), G_CALLBACK(add_splitpoint_from_player)},

    { "Delete_closest_splitpoint", GTK_STOCK_REMOVE, N_("_Delete closest splitpoint"), "d", 
      N_("Delete closest splitpoint"), G_CALLBACK(delete_closest_splitpoint)},

    { "Zoom_in", GTK_STOCK_ZOOM_IN, N_("Zoom _in"), "<Ctrl>plus", N_("Zoom in"), G_CALLBACK(zoom_in)},
    { "Zoom_out", GTK_STOCK_ZOOM_OUT, N_("Zoom _out"), "<Ctrl>minus", N_("Zoom out"), G_CALLBACK(zoom_out)},
  };

  static const gchar *ui_info = 
    "<ui>"
    "  <menubar name='MenuBar'>"
    "    <menu action='FileMenu'>"
    "      <menuitem action='Open'/>"
    "      <menuitem action='AddFilesToBatch'/>"
    "      <separator/>"
    "      <menuitem action='Import'/>"
    "      <menuitem action='ImportFromTrackType'/>"
    "      <menuitem action='Export'/>"
    "      <separator/>"
    "      <menuitem action='Preferences'/>"
    "      <separator/>"
    "      <menuitem action='Split'/>"
    "      <menuitem action='BatchSplit'/>"
    "      <separator/>"
    "      <menuitem action='Quit'/>"
    "    </menu>"
    "    <menu action='ViewMenu'>"
    "      <menuitem action='Splitpoints'/>"
    "      <menuitem action='SplitFiles'/>"
    "    </menu>"
    "    <menu action='PlayerMenu'>"
    "      <menuitem action='Player_pause'/>"
    "      <separator/>"
    "      <menuitem action='Player_forward'/>"
    "      <menuitem action='Player_backward'/>"
    "      <menuitem action='Player_small_forward'/>"
    "      <menuitem action='Player_small_backward'/>"
    "      <menuitem action='Player_big_forward'/>"
    "      <menuitem action='Player_big_backward'/>"
    "      <menuitem action='Player_next_splitpoint'/>"
    "      <menuitem action='Player_previous_splitpoint'/>"
    "      <menuitem action='Player_before_closest_splitpoint'/>"
    "      <separator/>"
    "      <menuitem action='Add_splitpoint'/>"
    "      <menuitem action='Delete_closest_splitpoint'/>"
    "      <separator/>"
    "      <menuitem action='Zoom_in'/>"
    "      <menuitem action='Zoom_out'/>"
    "    </menu>"
    "    <menu action='HelpMenu'>"
#ifndef NO_GNOME
    "      <menuitem action='Contents'/>"
    "      <separator/>"
#endif
    "      <menuitem action='Messages history'/>"
    "      <separator/>"
    "      <menuitem action='About'/>"
    "    </menu>"
    "  </menubar>"
    "</ui>";

  GtkActionGroup *action_group = gtk_action_group_new("Actions");
  ui->gui->action_group = action_group;

  gtk_action_group_set_translation_domain(action_group, "mp3splt-gtk");
  gtk_action_group_set_translate_func(action_group,
                  (GtkTranslateFunc)my_dgettext, NULL, NULL);

  gtk_action_group_add_actions(action_group, entries, G_N_ELEMENTS(entries), ui);
  GtkUIManager *uim = gtk_ui_manager_new();
  gtk_ui_manager_insert_action_group(uim, action_group, 0);

  g_signal_connect(G_OBJECT(ui->gui->window), "key_press_event",
      G_CALLBACK(window_key_press_event), ui);

  gtk_window_add_accel_group(GTK_WINDOW(ui->gui->window), gtk_ui_manager_get_accel_group(uim));
  gtk_ui_manager_add_ui_from_string(uim, ui_info, -1, NULL);
 
  GtkWidget *menu_box = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(menu_box), gtk_ui_manager_get_widget(uim, "/MenuBar"), FALSE, FALSE, 0);
 
  player_key_actions_set_sensitivity(FALSE, ui->gui);

  return menu_box;
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
  GtkWidget *split_button = wh_create_cool_button(GTK_STOCK_APPLY,_("Split !"), FALSE);
  g_signal_connect(G_OBJECT(split_button), "clicked",
      G_CALLBACK(single_file_mode_split_button_event), ui);
  gtk_widget_set_tooltip_text(split_button, _("Split the current file"));
  gtk_box_pack_start(GTK_BOX(top_hbox), split_button, FALSE, FALSE, 4);

  ui->gui->player_box = create_player_control_frame(ui);
  gtk_box_pack_start(GTK_BOX(player_vbox), ui->gui->player_box, FALSE, FALSE, 0);

  ui->gui->playlist_box = create_player_playlist_frame(ui);
  gtk_box_pack_start(GTK_BOX(player_vbox), ui->gui->playlist_box, TRUE, TRUE, 0);

  GtkWidget *notebook_label = wh_create_cool_label(GTK_STOCK_APPLY, _("Manual single file split"));
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

  notebook_label = wh_create_cool_label(GTK_STOCK_EXECUTE, _("Batch & automatic split"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), special_split_vbox, notebook_label);
 
  /* preferences widget */
  ui->gui->preferences_widget = create_choose_preferences(ui);

  /* progress bar */
  GtkProgressBar *percent_progress_bar = GTK_PROGRESS_BAR(gtk_progress_bar_new());
  ui->gui->percent_progress_bar = percent_progress_bar;
  gtk_progress_bar_set_fraction(percent_progress_bar, 0.0);
  gtk_progress_bar_set_text(percent_progress_bar, "");

#if GTK_MAJOR_VERSION >= 3
  gtk_progress_bar_set_show_text(percent_progress_bar, TRUE);
#endif

  GtkWidget *hbox = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(hbox), GTK_WIDGET(percent_progress_bar), TRUE, TRUE, 0);

  //stop button
  GtkWidget *cancel_button = wh_create_cool_button(GTK_STOCK_STOP, _("S_top"), FALSE);
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

void create_application(ui_state *ui)
{
  initialize_window(ui);

  GtkWidget *window_vbox = wh_vbox_new();
  gtk_container_add(GTK_CONTAINER(ui->gui->window), window_vbox);

  gtk_box_pack_start(GTK_BOX(window_vbox), create_menu_bar(ui), FALSE, FALSE, 0);  
  gtk_box_pack_start(GTK_BOX(window_vbox), create_main_vbox(ui), TRUE, TRUE, 0);

  ui_load_preferences(ui);

  move_and_resize_main_window(ui);

  gtk_widget_show_all(ui->gui->window);

  if (ui->infos->selected_player != PLAYER_GSTREAMER)
  {
    gtk_widget_hide(ui->gui->playlist_box);
  }

  hide_freedb_spinner(ui->gui);
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

static gboolean print_status_bar_confirmation_idle(ui_with_err *ui_err)
{
  print_status_bar_confirmation(ui_err->err, ui_err->ui);
  g_free(ui_err);
  return FALSE;
}

void print_status_bar_confirmation_in_idle(gint error, ui_state *ui)
{
  ui_with_err *ui_err = g_malloc0(sizeof(ui_with_err));
  ui_err->err = error;
  ui_err->ui = ui;

  gdk_threads_add_idle_full(G_PRIORITY_HIGH_IDLE,
      (GSourceFunc)print_status_bar_confirmation_idle, ui_err, NULL);
}

