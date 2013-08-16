/**********************************************************
 * mp3splt-gtk -- utility based on mp3splt,
 *
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
 * The player control tab
 *
 * this file is used for the player control tab
 **********************************************************/

#include "player_window.h"

#define DRAWING_AREA_WIDTH 400
#define DRAWING_AREA_HEIGHT 123 
#define DRAWING_AREA_HEIGHT_WITH_SILENCE_WAVE 232 

//playlist tree enumeration
enum {
  COL_NAME,
  COL_FILENAME,
  PLAYLIST_COLUMNS 
};

static void draw_small_rectangle(gint time_left, gint time_right, 
    GdkColor color, cairo_t *cairo_surface, ui_state *ui);
static gint mytimer(ui_state *ui);

//!function called from the library when scanning for the silence level
static void get_silence_level(long time, float level, void *user_data)
{
  ui_state *ui = (ui_state *)user_data;

  gint converted_level = (gint)floorf(abs(level));
  if (converted_level < 0)
  {
    return;
  }

  //TODO: using an idle here does not work.
  //      At least on windows idles are not executed in order

  if (!ui->infos->silence_points)
  {
    ui->infos->silence_points = g_malloc(sizeof(silence_wave) * 3000);
    ui->infos->malloced_num_of_silence_points = 3000;
  }
  else if (ui->infos->number_of_silence_points >= ui->infos->malloced_num_of_silence_points)
  {
    ui->infos->silence_points = g_realloc(ui->infos->silence_points,
        sizeof(silence_wave) * (ui->infos->number_of_silence_points + 3000));
    ui->infos->malloced_num_of_silence_points = ui->infos->number_of_silence_points + 3000;
  }

  ui->infos->silence_points[ui->infos->number_of_silence_points].time = time;
  ui->infos->silence_points[ui->infos->number_of_silence_points].level = abs(level);

  ui->infos->number_of_silence_points++;
}

static GArray *build_gdk_points_for_douglas_peucker(ui_infos *infos)
{
  GArray *points = g_array_new(TRUE, TRUE, sizeof(GdkPoint));

  gint i = 0;
  for (i = 0;i < infos->number_of_silence_points;i++)
  {
    long time = infos->silence_points[i].time;
    float level = infos->silence_points[i].level;

    GdkPoint point;
    point.x = (gint)time;
    point.y = (gint)floorf(level);
    g_array_append_val(points, point);
  }

  return points;
}

static void douglas_peucker_callback(ui_state *ui)
{
  ui->status->douglas_callback_counter++;

  if (ui->status->douglas_callback_counter % 400 != 0)
  {
    return;
  }

  gtk_progress_bar_pulse(ui->gui->percent_progress_bar);
  gtk_progress_bar_set_text(ui->gui->percent_progress_bar, 
      _("Processing Douglas-Peucker filters ..."));
  gtk_widget_queue_draw(GTK_WIDGET(ui->gui->percent_progress_bar));
  while (gtk_events_pending())
  {
    gtk_main_iteration();
  }

  ui->status->douglas_callback_counter = 0;
}

void compute_douglas_peucker_filters(ui_state *ui)
{
  if (!ui->status->show_silence_wave || ui->status->currently_compute_douglas_peucker_filters)
  {
    return;
  }

  ui_infos *infos = ui->infos;
  gui_status *status = ui->status;

  status->currently_compute_douglas_peucker_filters = TRUE;

  status->douglas_callback_counter = 0;

  GArray *gdk_points_for_douglas_peucker = build_gdk_points_for_douglas_peucker(infos);

  splt_douglas_peucker_free(infos->filtered_points_presence);

  infos->filtered_points_presence =
    splt_douglas_peucker(gdk_points_for_douglas_peucker, douglas_peucker_callback, ui,
        infos->douglas_peucker_thresholds[0], infos->douglas_peucker_thresholds[1],
        infos->douglas_peucker_thresholds[2], infos->douglas_peucker_thresholds[3],
        infos->douglas_peucker_thresholds[4], infos->douglas_peucker_thresholds[5],
        -1.0);

  g_array_free(gdk_points_for_douglas_peucker, TRUE);

  clear_previous_distances(ui);

  check_update_down_progress_bar(ui);

  status->currently_compute_douglas_peucker_filters = FALSE;
}

void set_currently_scanning_for_silence_safe(gint value, ui_state *ui)
{
  lock_mutex(&ui->variables_mutex);
  ui->status->currently_scanning_for_silence = value;
  unlock_mutex(&ui->variables_mutex);
}

gint get_currently_scanning_for_silence_safe(ui_state *ui)
{
  lock_mutex(&ui->variables_mutex);
  gint currently_scanning_for_silence = ui->status->currently_scanning_for_silence;
  unlock_mutex(&ui->variables_mutex);

  return currently_scanning_for_silence;
}

static gboolean detect_silence_end(ui_with_err *ui_err)
{
  ui_state *ui = ui_err->ui;

  mp3splt_set_silence_level_function(ui->mp3splt_state, NULL, NULL);

  set_is_splitting_safe(FALSE, ui);
  set_currently_scanning_for_silence_safe(FALSE, ui);

  compute_douglas_peucker_filters(ui);

  print_status_bar_confirmation(ui_err->err, ui);
  gtk_widget_set_sensitive(ui->gui->cancel_button, FALSE);

  refresh_drawing_area(ui->gui);
  refresh_preview_drawing_areas(ui->gui);

  set_process_in_progress_and_wait_safe(FALSE, ui_err->ui);

  g_free(ui_err);

  return FALSE;
}

static gpointer detect_silence(ui_state *ui)
{
  set_process_in_progress_and_wait_safe(TRUE, ui);

  set_is_splitting_safe(TRUE, ui);
  set_currently_scanning_for_silence_safe(TRUE, ui);

  if (ui->infos->silence_points)
  {
    g_free(ui->infos->silence_points);
    ui->infos->silence_points = NULL;
    ui->infos->number_of_silence_points = 0;
  }

  enter_threads();
  gtk_widget_set_sensitive(ui->gui->cancel_button, TRUE);
  exit_threads();

  lock_mutex(&ui->variables_mutex);
  mp3splt_set_filename_to_split(ui->mp3splt_state, get_input_filename(ui->gui));
  unlock_mutex(&ui->variables_mutex);

  mp3splt_set_silence_level_function(ui->mp3splt_state, get_silence_level, ui);

  gint err = SPLT_OK;
  mp3splt_set_silence_points(ui->mp3splt_state, &err);

  ui_with_err *ui_err = g_malloc0(sizeof(ui_with_err));
  ui_err->err = err;
  ui_err->ui = ui;

  gdk_threads_add_idle_full(G_PRIORITY_HIGH_IDLE, (GSourceFunc)detect_silence_end, ui_err, NULL);

  return NULL;
}

static void detect_silence_action(ui_state *ui)
{
  create_thread((GThreadFunc)detect_silence, ui);
}

/*! Initialize scanning for silence in the background.

  If showing the silence wave is disabled this function won't do anything.
 */
static void scan_for_silence_wave(ui_state *ui)
{
  if (get_currently_scanning_for_silence_safe(ui))
  {
    cancel_button_event(ui->gui->cancel_button, ui);
  }

  if (ui->status->timer_active)
  {
    detect_silence_action(ui);
  }
}

/*! Change the name of the song that is to be cut and played

Manages changing the filename itselves as well as recalculating the silence
wave if needed.
*/
void change_current_filename(const gchar *fname, ui_state *ui)
{
  const gchar *old_fname = get_input_filename(ui->gui);
  if (!old_fname)
  {
    set_input_filename(fname, ui);

    if (ui->status->show_silence_wave)
    {
      scan_for_silence_wave(ui);
    }

    if (gtk_toggle_button_get_active(ui->gui->names_from_filename))
    {
      copy_filename_to_current_description(fname, ui);
    }

    return;
  }

  if (strcmp(old_fname, fname) == 0)
  {
    return;
  }

  set_input_filename(fname, ui);
  if (ui->status->show_silence_wave)
  {
    scan_for_silence_wave(ui);
  }

  if (gtk_toggle_button_get_active(ui->gui->names_from_filename))
  {
    copy_filename_to_current_description(fname, ui);
  }
}

//!resets and sets inactive the progress bar
static void reset_inactive_progress_bar(gui_state *gui)
{
  gtk_widget_set_sensitive(GTK_WIDGET(gui->progress_bar), FALSE);
  gtk_adjustment_set_value(gui->progress_adj, 0);
}

//!resets and sets inactive the volume bar
static void reset_inactive_volume_button(gui_state *gui)
{
  gtk_widget_set_sensitive(GTK_WIDGET(gui->volume_button), FALSE);
  gtk_scale_button_set_value(GTK_SCALE_BUTTON(gui->volume_button), 0);
}

//!resets the label time
static void reset_label_time(gui_state *gui)
{
  gtk_label_set_text(GTK_LABEL(gui->label_time), "");
}

//!resets song infos, frequency, etc..
static void reset_song_infos(gui_state *gui)
{
  gtk_label_set_text(GTK_LABEL(gui->song_infos),"");
}

//!resets the song name label
static void reset_song_name_label(gui_state *gui)
{
  gtk_label_set_text(GTK_LABEL(gui->song_name_label), "");
}

//!clear song data and makes inactive progress bar
static void clear_data_player(gui_state *gui)
{
  reset_song_name_label(gui);
  reset_song_infos(gui);
  reset_inactive_volume_button(gui);
  reset_inactive_progress_bar(gui);
  reset_label_time(gui);
}

//!enables the buttons of the player
void enable_player_buttons(ui_state *ui)
{
  gui_state *gui = ui->gui;

  gtk_widget_set_sensitive(gui->stop_button, TRUE);
  wh_set_image_on_button(GTK_BUTTON(gui->stop_button), g_object_ref(gui->StopButton_active));

  gtk_widget_set_sensitive(gui->pause_button, TRUE);
  wh_set_image_on_button(GTK_BUTTON(gui->pause_button), g_object_ref(gui->PauseButton_active));

  if (ui->infos->selected_player != PLAYER_GSTREAMER)
  {
    gtk_widget_set_sensitive(gui->go_beg_button, TRUE);
    wh_set_image_on_button(GTK_BUTTON(gui->go_beg_button), g_object_ref(gui->Go_BegButton_active));

    gtk_widget_set_sensitive(gui->go_end_button, TRUE);
    wh_set_image_on_button(GTK_BUTTON(gui->go_end_button), g_object_ref(gui->Go_EndButton_active));
  }

  gtk_widget_set_sensitive(gui->play_button, TRUE);
  wh_set_image_on_button(GTK_BUTTON(gui->play_button), g_object_ref(gui->PlayButton_active));

  player_key_actions_set_sensitivity(TRUE, gui);
}

//!disables the buttons of the player
static void disable_player_buttons(gui_state *gui)
{
  gtk_widget_set_sensitive(gui->stop_button, FALSE);
  wh_set_image_on_button(GTK_BUTTON(gui->stop_button), g_object_ref(gui->StopButton_inactive));
 
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui->pause_button), FALSE);
  gtk_widget_set_sensitive(gui->pause_button, FALSE);
  wh_set_image_on_button(GTK_BUTTON(gui->pause_button), g_object_ref(gui->PauseButton_inactive));

  gtk_widget_set_sensitive(gui->go_beg_button, FALSE);
  wh_set_image_on_button(GTK_BUTTON(gui->go_beg_button), g_object_ref(gui->Go_BegButton_inactive));

  gtk_widget_set_sensitive(gui->go_end_button, FALSE);
  wh_set_image_on_button(GTK_BUTTON(gui->go_end_button), g_object_ref(gui->Go_EndButton_inactive));

  gtk_widget_set_sensitive(gui->play_button, FALSE);
  wh_set_image_on_button(GTK_BUTTON(gui->play_button), g_object_ref(gui->PlayButton_inactive));

  gtk_widget_set_sensitive(gui->player_add_button, FALSE);
  gtk_widget_set_sensitive(gui->silence_wave_check_button, FALSE);

  player_key_actions_set_sensitivity(FALSE, gui);
}

//! Show the disconnec button
static void show_disconnect_button(gui_state *gui)
{
  if (!wh_container_has_child(GTK_CONTAINER(gui->player_buttons_hbox), gui->disconnect_button))
  {
    gtk_box_pack_start(gui->player_buttons_hbox, gui->disconnect_button, FALSE, FALSE, 7);
  }

  gtk_widget_show_all(gui->disconnect_button);
}

//! Hide the connect button
void hide_connect_button(gui_state *gui)
{
  gtk_widget_hide(gui->connect_button);
}

//! Switches between connect and disconnect button when connecting to player
static void connect_change_buttons(ui_state *ui)
{
  if (ui->infos->selected_player == PLAYER_GSTREAMER)
  {
    return;
  }

  show_disconnect_button(ui->gui);
  hide_connect_button(ui->gui);
}

//! Hide the disconnect button
static void hide_disconnect_button(gui_state *gui)
{
  gtk_widget_hide(gui->disconnect_button);
}

//! Show the connect button
void show_connect_button(gui_state *gui)
{
  if (!wh_container_has_child(GTK_CONTAINER(gui->player_buttons_hbox), gui->connect_button))
  {
    gtk_box_pack_start(gui->player_buttons_hbox, gui->connect_button, FALSE, FALSE, 7);
  }

  gtk_widget_show_all(gui->connect_button);
}

//!disconnecting changing buttons
static void disconnect_change_buttons(ui_state *ui)
{
  if (ui->infos->selected_player == PLAYER_GSTREAMER)
  {
    return;
  }

  hide_disconnect_button(ui->gui);
  show_connect_button(ui->gui);
}

/*!connect with the song fname

\param fname the file name of the song 
\param start_playing 
-  start playing
- else dont start playing right now.
*/
static void connect_with_song(const gchar *fname, gint start_playing, ui_state *ui)
{
  if (fname == NULL)
  {
    return;
  }

  gui_status *status = ui->status;

  GList *song_list = NULL;
  song_list = g_list_append(song_list, strdup(fname));

  if (start_playing == 0)
  {
    if (!player_is_running(ui))
    {
      player_start_play_with_songs(song_list, ui);
    }
    else
    {
      player_add_play_files(song_list, ui);
    }
  }
  else
  {
    if (status->file_browsed)
    {
      //if the player is not running, start it ,queue to playlist and
      //play the file
      if (!player_is_running(ui))
      {
        player_start_add_files(song_list, ui);
      }
      else
      {
        if (!status->playing)
        {
          player_add_files_and_select(song_list, ui);
        }
        else
        {
          player_add_files(song_list, ui);
        }
      }
    }
  }

  status->playing = player_is_playing(ui);

  if (!status->timer_active)
  {
    status->timeout_id = g_timeout_add(ui->infos->timeout_value, (GSourceFunc)mytimer, ui);
    status->timer_active = TRUE;
  }

  enable_player_buttons(ui);

  if (player_is_running(ui))
  {
    connect_change_buttons(ui);
  }

  g_list_foreach(song_list, (GFunc)g_free, NULL);
  g_list_free(song_list);
}

/*! connects to player with the song from the filename entry

  \param i 0 means then start playing, != 0 means dont start playing
  right now
 */
void connect_to_player_with_song(gint i, ui_state *ui)
{
  connect_with_song(get_input_filename(ui->gui), i, ui);
}

//!play button event
void connect_button_event(GtkWidget *widget, ui_state *ui)
{
  gui_status *status = ui->status;

  if (!player_is_running(ui))
  {
    player_start(ui);
  }

  mytimer(ui);

  if (!status->timer_active)
  {
    if (ui->infos->selected_player == PLAYER_SNACKAMP)
    {
      connect_snackamp(8775, ui);
    }

    status->timeout_id = g_timeout_add(ui->infos->timeout_value, (GSourceFunc)mytimer, ui);
    status->timer_active = TRUE;
  }

  //connect to player with song
  //1 means dont start playing
  connect_to_player_with_song(1, ui);

  enable_player_buttons(ui);

  status->file_browsed = FALSE;
  status->change_volume = TRUE;

  //here we check if we have been connected
  if (player_is_running(ui))
  {
    connect_change_buttons(ui);
  }
  else
  {
    GtkWidget *label;
    switch (ui->infos->selected_player)
    {
      case PLAYER_SNACKAMP:
        label = gtk_label_new
          (_("\n Cannot connect to snackAmp player.\n"
             " Please download and install snackamp from\n"
             "\thttp://snackamp.sourceforge.net\n\n"
             " Verify that snackamp is running.\n"
             " Verify that your snackamp version is >= 3.1.3\n\n"
             " Verify that you have enabled socket interface in snackamp:\n"
             " You have to go to\n"
             "\tTools->Preferences->Miscellaneous\n"
             " from the snackamp menu and check\n"
             "\tEnable Socket Interface\n"
             " Only default port is supported for now(8775)\n"
             " After that, restart snackamp and mp3splt-gtk should work.\n"));
        break;
      case PLAYER_AUDACIOUS:
        label = gtk_label_new 
          (_("\n Cannot connect to Audacious player.\n"
             " Verify that you have installed audacious.\n\n"
             " Put in your PATH variable the directory where the audacious"
             " executable is.\n"
             " If you don't know how to do that, start audacious manually"
             " and then try to connect.\n"));
        break;
      default:
        label = gtk_label_new(_("Cannot connect to player"));
        break;
    }

    GtkWidget *dialog = gtk_dialog_new_with_buttons(_("Cannot connect to player"),
        GTK_WINDOW(ui->gui->window), GTK_DIALOG_MODAL, GTK_STOCK_OK, GTK_RESPONSE_NONE, NULL);
    g_signal_connect_swapped(dialog, "response", G_CALLBACK(gtk_widget_destroy), dialog);
    gtk_container_add(GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(dialog))), label);
    gtk_widget_show_all(dialog);
  }

  ui->infos->current_time = -1;
  check_update_down_progress_bar(ui);

  if (ui->status->show_silence_wave)
  {
    scan_for_silence_wave(ui);
  }

  mytimer(ui);
  refresh_drawing_area(ui->gui);
}

//!checks if we have a stream
static void check_stream(ui_state *ui)
{
  if (((gint)ui->infos->total_time) == -1)
  {
    ui->status->stream = TRUE;
    reset_inactive_progress_bar(ui->gui);
  }
  else
  {
    ui->status->stream = FALSE;
  }
}

//!disconnect button event
void disconnect_button_event(GtkWidget *widget, ui_state *ui)
{
  gui_state *gui = ui->gui;

  if (ui->status->timer_active)
  {
    if (ui->infos->selected_player == PLAYER_SNACKAMP)
    {
      disconnect_snackamp(ui);
    }

    g_source_remove(ui->status->timeout_id);
    ui->status->timer_active = FALSE;
  }

  clear_data_player(gui);
  disconnect_change_buttons(ui);
  disable_player_buttons(gui);

  //update bottom progress bar to 0 and ""
  if (!get_is_splitting_safe(ui))
  {
    gtk_progress_bar_set_fraction(gui->percent_progress_bar, 0);
    gtk_progress_bar_set_text(gui->percent_progress_bar, "");
  }

  const gchar *fname = get_input_filename(gui);
  if (file_exists(fname))
  {
    gtk_widget_set_sensitive(gui->play_button, TRUE);
    wh_set_image_on_button(GTK_BUTTON(gui->play_button), g_object_ref(gui->PlayButton_active));
  }

  player_quit(ui);

  if (get_currently_scanning_for_silence_safe(ui))
  {
    cancel_button_event(ui->gui->cancel_button, ui);
  }

  refresh_drawing_area(ui->gui);
}

void restart_player_timer(ui_state *ui)
{
  if (ui->status->timer_active)
  {
    g_source_remove(ui->status->timeout_id);
    ui->status->timeout_id = g_timeout_add(ui->infos->timeout_value, (GSourceFunc)mytimer, ui);
  }
}

//! play button event
static void play_event(GtkWidget *widget, ui_state *ui)
{
  gui_state *gui = ui->gui;
  gui_status *status = ui->status;

  if (status->timer_active)
  {
    if (!player_is_running(ui))
    {
      player_start(ui);
    }
    player_play(ui);
    status->playing = player_is_playing(ui);
  }
  else
  {
    //0 = also start playing
    connect_to_player_with_song(0, ui);
    if (ui->infos->selected_player == PLAYER_GSTREAMER &&
        ui->status->show_silence_wave)
    {
      scan_for_silence_wave(ui);
    }
  }

  gtk_widget_set_sensitive(gui->pause_button, TRUE);
  wh_set_image_on_button(GTK_BUTTON(gui->pause_button), g_object_ref(gui->PauseButton_active));

  gtk_widget_set_sensitive(gui->stop_button, TRUE);
  wh_set_image_on_button(GTK_BUTTON(gui->stop_button), g_object_ref(gui->StopButton_active));
}

//! stop button event
static void stop_event(GtkWidget *widget, ui_state *ui)
{
  gui_state *gui = ui->gui;

  if (!ui->status->timer_active)
  {
    return;
  }

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui->pause_button), FALSE);

  if (player_is_running(ui))
  {
    ui->status->playing = FALSE;
  }

  player_stop(ui);

  gtk_widget_set_sensitive(gui->pause_button, FALSE);
  wh_set_image_on_button(GTK_BUTTON(gui->pause_button), g_object_ref(gui->PauseButton_inactive));

  gtk_widget_set_sensitive(gui->stop_button, FALSE);
  wh_set_image_on_button(GTK_BUTTON(gui->stop_button), g_object_ref(gui->StopButton_inactive));
}

//! pause button event
void pause_event(GtkWidget *widget, ui_state *ui)
{
  if (!ui->status->timer_active) { return; }
  if (!player_is_running(ui)) { return; }
  if (ui->status->only_press_pause) { return; }

  player_pause(ui);
}

//! Event for the "previous" button 
static void prev_button_event(GtkWidget *widget, ui_state *ui)
{
  if (!ui->status->timer_active) { return; }
  if (!player_is_running(ui)) { return; }
  player_prev(ui);
}

//! event for the "next" button
static void next_button_event(GtkWidget *widget, ui_state *ui)
{
  if (!ui->status->timer_active)
  {
    return;
  }

  if (!player_is_running(ui))
  {
    return;
  }

  player_next(ui);
}

//!changes the position inside the song
static void change_song_position(ui_state *ui)
{
  gint position = 
    ui->infos->player_seconds2 * 1000 + 
    ui->infos->player_minutes2 * 60000 +
    ui->infos->player_hundr_secs2 * 10;

  player_seek(position, ui);
}

//!adds a splitpoint from the player
static void toggle_show_silence_wave(GtkToggleButton *show_silence_toggle_button, ui_state *ui)
{
  gui_status *status = ui->status;

  if (gtk_toggle_button_get_active(show_silence_toggle_button))
  {
    status->show_silence_wave = TRUE;
    scan_for_silence_wave(ui);
    return;
  }

  status->show_silence_wave = FALSE;
  if (get_currently_scanning_for_silence_safe(ui))
  {
    cancel_button_event(ui->gui->cancel_button, ui);
  }

  refresh_drawing_area(ui->gui);
  refresh_preview_drawing_areas(ui->gui);

  ui_save_preferences(NULL, ui);
}

//!when we unclick the volume bar
static gboolean volume_button_unclick_event(GtkWidget *widget, GdkEventCrossing *event, ui_state *ui)
{
  ui->status->change_volume = TRUE;
  return FALSE;
}

//!when we click the volume bar
static gboolean volume_button_click_event(GtkWidget *widget, GdkEventCrossing *event, ui_state *ui)
{
  ui->status->change_volume = FALSE;
  return FALSE;
}

//!when we enter the volume bar
static gboolean volume_button_enter_event(GtkWidget *widget, GdkEventCrossing *event, ui_state *ui)
{
  ui->status->on_the_volume_button = TRUE;
  return FALSE;
}

//!when we leave the volume bar
static gboolean volume_button_leave_event(GtkWidget *widget, GdkEventCrossing *event, ui_state *ui)
{
  ui->status->on_the_volume_button = FALSE;
  return FALSE;
}

//!changes the volume of the player
static void change_volume_event(GtkScaleButton *volume_button, gdouble value, ui_state *ui)
{
  if (!gtk_widget_get_sensitive(GTK_WIDGET(volume_button)))
  {
    return;
  }

  player_set_volume((gint)(value * 100), ui);
}

static GtkWidget *create_volume_button(ui_state *ui)
{
  GtkWidget *volume_button = gtk_volume_button_new();
  ui->gui->volume_button = volume_button;

  g_signal_connect(G_OBJECT(volume_button), "button-press-event",
      G_CALLBACK(volume_button_click_event), ui);
  g_signal_connect(G_OBJECT(volume_button), "button-release-event",
      G_CALLBACK(volume_button_unclick_event), ui);
  g_signal_connect(G_OBJECT(volume_button), "enter-notify-event",
      G_CALLBACK(volume_button_enter_event), ui);
  g_signal_connect(G_OBJECT(volume_button), "leave-notify-event",
      G_CALLBACK(volume_button_leave_event), ui);

  g_signal_connect(GTK_SCALE_BUTTON(volume_button), "value_changed",
      G_CALLBACK(change_volume_event), ui);

  gtk_widget_set_sensitive(GTK_WIDGET(volume_button), FALSE);

  return volume_button;
}

//!creates the player buttons hbox
static GtkWidget *create_player_buttons_hbox(ui_state *ui)
{
  GtkBox *player_buttons_hbox = GTK_BOX(wh_hbox_new());
  ui->gui->player_buttons_hbox = player_buttons_hbox;

  GString *imagefile = g_string_new("");
  build_path(imagefile, IMAGEDIR, "backward"ICON_EXT);
  GtkWidget *Go_BegButton_active = gtk_image_new_from_file(imagefile->str);
  ui->gui->Go_BegButton_active = Go_BegButton_active;

  build_path(imagefile, IMAGEDIR, "backward_inactive"ICON_EXT);
  GtkWidget *Go_BegButton_inactive = gtk_image_new_from_file(imagefile->str);
  ui->gui->Go_BegButton_inactive = Go_BegButton_inactive;
  GtkWidget *go_beg_button = gtk_button_new();
  ui->gui->go_beg_button = go_beg_button;
  wh_set_image_on_button(GTK_BUTTON(go_beg_button), g_object_ref(Go_BegButton_inactive));

  gtk_box_pack_start(player_buttons_hbox, go_beg_button, FALSE, FALSE, 0);
  gtk_button_set_relief(GTK_BUTTON(go_beg_button), GTK_RELIEF_NONE);
  g_signal_connect(G_OBJECT(go_beg_button), "clicked", G_CALLBACK(prev_button_event), ui);
  gtk_widget_set_sensitive(go_beg_button, FALSE);
  gtk_widget_set_tooltip_text(go_beg_button, _("Previous track"));

  //play button
  build_path(imagefile, IMAGEDIR, "play"ICON_EXT);
  GtkWidget *PlayButton_active = gtk_image_new_from_file(imagefile->str);
  ui->gui->PlayButton_active = PlayButton_active;

  build_path(imagefile, IMAGEDIR, "play_inactive"ICON_EXT);
  GtkWidget *PlayButton_inactive = gtk_image_new_from_file(imagefile->str);
  ui->gui->PlayButton_inactive = PlayButton_inactive;
  GtkWidget *play_button = gtk_button_new();
  ui->gui->play_button = play_button;
  wh_set_image_on_button(GTK_BUTTON(play_button), g_object_ref(PlayButton_inactive));

  gtk_box_pack_start(player_buttons_hbox, play_button, FALSE, FALSE, 0);
  gtk_button_set_relief(GTK_BUTTON(play_button), GTK_RELIEF_NONE);
  g_signal_connect(G_OBJECT(play_button), "clicked", G_CALLBACK(play_event), ui);
  gtk_widget_set_sensitive(play_button, FALSE);
  gtk_widget_set_tooltip_text(play_button, _("Play"));

  //pause button
  build_path(imagefile, IMAGEDIR, "pause"ICON_EXT);
  GtkWidget *PauseButton_active = gtk_image_new_from_file(imagefile->str);
  ui->gui->PauseButton_active = PauseButton_active;

  build_path(imagefile, IMAGEDIR, "pause_inactive"ICON_EXT);
  GtkWidget *PauseButton_inactive = gtk_image_new_from_file(imagefile->str);
  ui->gui->PauseButton_inactive = PauseButton_inactive;
  GtkWidget *pause_button = gtk_toggle_button_new();
  ui->gui->pause_button = pause_button;
  wh_set_image_on_button(GTK_BUTTON(pause_button), g_object_ref(PauseButton_inactive));
  gtk_box_pack_start(player_buttons_hbox, pause_button, FALSE, FALSE, 0);
  gtk_button_set_relief(GTK_BUTTON(pause_button), GTK_RELIEF_NONE);
  g_signal_connect(G_OBJECT(pause_button), "clicked", G_CALLBACK(pause_event), ui);
  gtk_widget_set_sensitive(pause_button, FALSE);
  gtk_widget_set_tooltip_text(pause_button,_("Pause"));

  //stop button
  build_path(imagefile, IMAGEDIR, "stop"ICON_EXT);
  GtkWidget *StopButton_active = gtk_image_new_from_file(imagefile->str);
  ui->gui->StopButton_active = StopButton_active;

  build_path(imagefile, IMAGEDIR, "stop_inactive"ICON_EXT);
  GtkWidget *StopButton_inactive = gtk_image_new_from_file(imagefile->str);
  ui->gui->StopButton_inactive = StopButton_inactive;
  GtkWidget *stop_button = gtk_button_new();
  ui->gui->stop_button = stop_button;
  wh_set_image_on_button(GTK_BUTTON(stop_button), g_object_ref(StopButton_inactive));
  gtk_box_pack_start(player_buttons_hbox, stop_button, FALSE, FALSE, 0);
  gtk_button_set_relief(GTK_BUTTON(stop_button), GTK_RELIEF_NONE);
  g_signal_connect(G_OBJECT(stop_button), "clicked", G_CALLBACK(stop_event), ui);
  gtk_widget_set_sensitive(stop_button, FALSE);
  gtk_widget_set_tooltip_text(stop_button,_("Stop"));

  //go at the end button
  build_path(imagefile, IMAGEDIR, "forward"ICON_EXT);
  GtkWidget *Go_EndButton_active = gtk_image_new_from_file(imagefile->str);
  ui->gui->Go_EndButton_active = Go_EndButton_active;

  build_path(imagefile, IMAGEDIR, "forward_inactive"ICON_EXT);
  GtkWidget *Go_EndButton_inactive = gtk_image_new_from_file(imagefile->str);
  ui->gui->Go_EndButton_inactive = Go_EndButton_inactive;
  GtkWidget *go_end_button = gtk_button_new();
  ui->gui->go_end_button = go_end_button;
  wh_set_image_on_button(GTK_BUTTON(go_end_button), g_object_ref(Go_EndButton_inactive));
  gtk_box_pack_start(player_buttons_hbox, go_end_button, FALSE, FALSE, 0);
  gtk_button_set_relief(GTK_BUTTON(go_end_button), GTK_RELIEF_NONE);
  g_signal_connect(G_OBJECT(go_end_button), "clicked", G_CALLBACK(next_button_event), ui);
  gtk_widget_set_sensitive(go_end_button, FALSE);
  gtk_widget_set_tooltip_text(go_end_button,_("Next track"));
  g_string_free(imagefile,TRUE);

  GtkWidget *vol_button = create_volume_button(ui);
  gtk_box_pack_start(player_buttons_hbox, vol_button, FALSE, FALSE, 5);
 
  //add button
  GtkWidget *player_add_button = wh_create_cool_button(GTK_STOCK_ADD, _("_Add"), FALSE);
  ui->gui->player_add_button = player_add_button;
  gtk_box_pack_start(player_buttons_hbox, player_add_button, FALSE, FALSE, 0);
  gtk_button_set_relief(GTK_BUTTON(player_add_button), GTK_RELIEF_NONE);
  g_signal_connect(G_OBJECT(player_add_button), "clicked",
      G_CALLBACK(add_splitpoint_from_player), ui);
  gtk_widget_set_sensitive(player_add_button, FALSE);
  gtk_widget_set_tooltip_text(player_add_button,_("Add splitpoint at the current player position"));

  //set splitpoints from trim silence button
  GtkWidget *scan_trim_silence_button = wh_create_cool_button(GTK_STOCK_CUT, NULL, FALSE);
  ui->gui->scan_trim_silence_button_player = scan_trim_silence_button;
  gtk_widget_set_sensitive(scan_trim_silence_button, TRUE);
  g_signal_connect(G_OBJECT(scan_trim_silence_button), "clicked",
      G_CALLBACK(create_trim_silence_window), ui);
  gtk_widget_set_tooltip_text(scan_trim_silence_button,
      _("Set trim splitpoints using silence detection"));
  gtk_box_pack_start(player_buttons_hbox, scan_trim_silence_button, FALSE, FALSE, 0);
  gtk_button_set_relief(GTK_BUTTON(scan_trim_silence_button), GTK_RELIEF_NONE);
 
  //set splitpoints from silence button
  GtkWidget *scan_silence_button = wh_create_cool_button(GTK_STOCK_FIND_AND_REPLACE, NULL, FALSE);
  ui->gui->scan_silence_button_player = scan_silence_button;
  gtk_widget_set_sensitive(scan_silence_button, TRUE);
  g_signal_connect(G_OBJECT(scan_silence_button), "clicked",
      G_CALLBACK(create_detect_silence_and_add_splitpoints_window), ui);
  gtk_widget_set_tooltip_text(scan_silence_button,
      _("Set splitpoints from silence detection"));
  gtk_box_pack_start(player_buttons_hbox, scan_silence_button, FALSE, FALSE, 0);
  gtk_button_set_relief(GTK_BUTTON(scan_silence_button), GTK_RELIEF_NONE);

  //silence wave check button
  GtkWidget *silence_wave_check_button = gtk_check_button_new_with_mnemonic(_("Amplitude _wave"));
  ui->gui->silence_wave_check_button = silence_wave_check_button;
  gtk_box_pack_end(player_buttons_hbox, silence_wave_check_button, FALSE, FALSE, 5);
  g_signal_connect(G_OBJECT(silence_wave_check_button), "toggled",
      G_CALLBACK(toggle_show_silence_wave), ui);
  gtk_widget_set_sensitive(silence_wave_check_button, FALSE);
  gtk_widget_set_tooltip_text(silence_wave_check_button, _("Shows the amplitude level wave"));

  /* connect player button */
  GtkWidget *connect_button = wh_create_cool_button(GTK_STOCK_CONNECT,_("_Connect"), FALSE);
  ui->gui->connect_button = connect_button;
  g_signal_connect(G_OBJECT(connect_button), "clicked", G_CALLBACK(connect_button_event), ui);
  gtk_widget_set_tooltip_text(connect_button,_("Connect to player"));
  
  /* disconnect player button */
  GtkWidget *disconnect_button = wh_create_cool_button(GTK_STOCK_DISCONNECT,_("_Disconnect"), FALSE);
  ui->gui->disconnect_button = disconnect_button;
  g_signal_connect(G_OBJECT(disconnect_button), "clicked", G_CALLBACK(disconnect_button_event), ui);
  gtk_widget_set_tooltip_text(disconnect_button,_("Disconnect from player"));

  return GTK_WIDGET(player_buttons_hbox);
}

//!song information about frequency, rate, stereo, etc
static GtkWidget *create_song_informations_hbox(gui_state *gui)
{
  GtkWidget *song_info_hbox = wh_hbox_new();

  GtkWidget *song_infos = gtk_label_new("");
  gui->song_infos = song_infos;
  gtk_box_pack_start(GTK_BOX(song_info_hbox), song_infos, FALSE, FALSE, 40);

  GtkWidget *label_time = gtk_label_new("");
  gui->label_time = label_time;
  gtk_box_pack_start(GTK_BOX(song_info_hbox), label_time, FALSE, FALSE, 5);

  return song_info_hbox;
}

//!when we unclick the progress bar
static gboolean progress_bar_unclick_event(GtkWidget *widget, GdkEventCrossing *event, ui_state *ui)
{
  change_song_position(ui);

  ui_infos *infos = ui->infos;

  infos->player_minutes = infos->player_minutes2;
  infos->player_seconds = infos->player_seconds2; 
  infos->player_hundr_secs = infos->player_hundr_secs2; 
 
  ui->status->mouse_on_progress_bar = FALSE;
 
  return FALSE;
}

//!when we click the progress bar
static gboolean progress_bar_click_event(GtkWidget *widget, GdkEventCrossing *event, ui_state *ui)
{
  ui->status->mouse_on_progress_bar = TRUE;
  return FALSE;
}

//!returns the total elapsed time
static gfloat get_elapsed_time(ui_state *ui)
{
  gfloat adj_position = gtk_adjustment_get_value(ui->gui->progress_adj);
  return (adj_position * ui->infos->total_time) / 100000;
}

void refresh_drawing_area(gui_state *gui)
{
  gtk_widget_queue_draw(gui->drawing_area);
}

//!updates bottom progress bar
void check_update_down_progress_bar(ui_state *ui)
{
  if (get_is_splitting_safe(ui) ||
      get_currently_scanning_for_silence_safe(ui) ||
      ui->status->currently_compute_douglas_peucker_filters)
  {
    return;
  }

  gfloat progress_time = 0;
  gint splitpoint_time_left = -1;
  gint splitpoint_time_right = -1;
  gint splitpoint_left_index = -1;
  get_current_splitpoints_time_left_right(&splitpoint_time_left, &splitpoint_time_right, 
      &splitpoint_left_index, ui);

  if ((splitpoint_time_left != -1) && (splitpoint_time_right != -1))
  {
    gfloat total_interval = splitpoint_time_right - splitpoint_time_left;
    if (((gint)total_interval) != 0)
    {
      progress_time = (ui->infos->current_time-splitpoint_time_left) / total_interval;
    }
  }
  else
  {
    if (splitpoint_time_right == -1)
    {
      gfloat total_interval = ui->infos->total_time - splitpoint_time_left;
      if (((gint)total_interval) != 0)
      {
        progress_time = (ui->infos->current_time-splitpoint_time_left)/ total_interval;
      }
    }
    else
    {
      gfloat total_interval = splitpoint_time_right;
      if (((gint)total_interval) != 0)
      {
        progress_time = ui->infos->current_time/total_interval;
      }
    }
  }

  if (progress_time < 0)
  {
    progress_time = 0;
  }
  if (progress_time > 1)
  {
    progress_time = 1;
  }
  if ((progress_time >= 0) && (progress_time <= 1))
  {
    gtk_progress_bar_set_fraction(ui->gui->percent_progress_bar, progress_time);
  }

  gchar *progress_description = get_splitpoint_name(splitpoint_left_index-1, ui);
  gchar description_shorted[512] = { '\0' };

  if (splitpoint_time_right != -1)
  {
    if (splitpoint_time_left == -1)
    {
      if (progress_description != NULL)
      {
        g_snprintf(description_shorted, 60, _("before %s"), progress_description);
      }
    }
    else
    {
      if (progress_description != NULL)
      {
        g_snprintf(description_shorted, 60, "%s", progress_description);
      }
    }
  }
  else
  {
    if (splitpoint_time_left != -1)
    {
      if (progress_description != NULL)
      {
        g_snprintf(description_shorted, 60, "%s", progress_description);
      }
    }
    else
    {
      gchar *fname = get_input_filename(ui->gui);
      g_snprintf(description_shorted, 60, "%s", get_real_name_from_filename(fname));
    }
  }

  if (strlen(description_shorted) > 55)
  {
    description_shorted[56] = '.';
    description_shorted[57] = '.';
    description_shorted[58] = '.';
    description_shorted[59] = '\0';
  }

  gtk_progress_bar_set_text(ui->gui->percent_progress_bar, description_shorted);
  g_free(progress_description);
}

//!event when the progress bar value changed
static void progress_bar_value_changed_event(GtkRange *range, ui_state *ui)
{
  refresh_drawing_area(ui->gui);

  ui_infos *infos = ui->infos;
  
  infos->player_hundr_secs2 = (gint)infos->current_time % 100;
 
  gint tt2 = infos->total_time / 100;
  gfloat adj_position = (gint)gtk_adjustment_get_value(ui->gui->progress_adj);
  infos->current_time = (adj_position * tt2) / 100000;
  
  infos->player_seconds2 = (gint)infos->current_time % 60;
  infos->player_minutes2 = (gint)infos->current_time / 60;
  
  infos->current_time = get_elapsed_time(ui);
  
  check_update_down_progress_bar(ui);
}

//!scroll event for the progress bar
static gboolean progress_bar_scroll_event(GtkWidget *widget, GdkEventScroll *event, gpointer user_data)
{
  return FALSE;
}

//!when we enter the progress bar
static gboolean progress_bar_enter_event(GtkWidget *widget, GdkEventCrossing *event, gpointer user_data)
{
  return FALSE;
}

//!when we leave the progress bar
static gboolean progress_bar_leave_event(GtkWidget *widget, GdkEventCrossing *event, gpointer user_data)
{
  return FALSE;
}

//!song progress bar
static GtkWidget *create_song_bar_hbox(ui_state *ui)
{
  GtkAdjustment *progress_adj = 
    GTK_ADJUSTMENT(gtk_adjustment_new(0.0, 0.0, 100001.0, 0, 10000, 1));
  ui->gui->progress_adj = progress_adj;

  GtkWidget *progress_bar = wh_hscale_new(progress_adj);
  ui->gui->progress_bar = progress_bar;
  g_object_set(progress_bar, "draw-value", FALSE, NULL);

  g_signal_connect(G_OBJECT(progress_bar), "button-press-event",
      G_CALLBACK(progress_bar_click_event), ui);
  g_signal_connect(G_OBJECT(progress_bar), "button-release-event",
      G_CALLBACK(progress_bar_unclick_event), ui);
  g_signal_connect(G_OBJECT(progress_bar), "value-changed",
      G_CALLBACK(progress_bar_value_changed_event), ui);

  g_signal_connect(G_OBJECT(progress_bar), "enter-notify-event",
      G_CALLBACK(progress_bar_enter_event), NULL);
  g_signal_connect(G_OBJECT(progress_bar), "leave-notify-event",
      G_CALLBACK(progress_bar_leave_event), NULL);
  g_signal_connect(G_OBJECT(progress_bar), "scroll-event",
      G_CALLBACK(progress_bar_scroll_event), NULL);

  gtk_widget_set_sensitive(GTK_WIDGET(progress_bar), FALSE);

  GtkWidget *song_bar_hbox = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(song_bar_hbox), progress_bar, TRUE, TRUE, 5);
  return song_bar_hbox;
}

//!prints information about the song, frequency, kbps, stereo
static void print_about_the_song(ui_state *ui)
{
  gchar total_infos[512];
  player_get_song_infos(total_infos, ui);

  gtk_label_set_text(GTK_LABEL(ui->gui->song_infos), total_infos);
}

//!prints the player filename
static void print_player_filename(ui_state *ui)
{
  gchar *fname = player_get_filename(ui);
  if (fname != NULL)
  {
    if (strcmp(fname, "disconnect"))
    {
      change_current_filename(fname, ui);
    }
    g_free(fname);
  }

  gchar *title = player_get_title(ui);
  if (title != NULL)
  {
    gtk_label_set_text(GTK_LABEL(ui->gui->song_name_label), title);
    if (title != NULL)
    {
      g_free(title);
    }
  }
}

/*! get time elapsed from the song and print it on the screen

Also prints filename, frequency, bitrate, mono, stereo
*/
static void print_all_song_infos(ui_state *ui)
{
  print_about_the_song(ui);
  print_player_filename(ui);
}

/*! prints the song time elapsed

\param i 
 - 0 means normal state
 - 1 means we reset the time
*/
static void print_song_time_elapsed(ui_state *ui)
{
  gchar seconds[16], minutes[16], seconds_minutes[64];

  gint time = player_get_elapsed_time(ui);
  ui->infos->player_hundr_secs = (time % 1000) / 10;

  gint temp = (time/1000)/60;
  ui->infos->player_minutes = temp;
  ui->infos->player_seconds = (time/1000) - (temp*60); 

  g_snprintf(minutes, 16, "%d", temp);
  g_snprintf(seconds, 16, "%d", (time/1000) - (temp*60));

  gchar total_seconds[16], total_minutes[16];

  gint tt = ui->infos->total_time * 10;
  temp = (tt / 1000) / 60;

  g_snprintf(total_minutes, 16, "%d", temp);
  g_snprintf(total_seconds, 16, "%d", (tt/1000) - (temp*60));
  g_snprintf(seconds_minutes, 64, "%s  :  %s  /  %s  :  %s", 
      minutes, seconds, total_minutes, total_seconds);

  gtk_label_set_text(GTK_LABEL(ui->gui->label_time), seconds_minutes);
}

//!change volume to match the players volume
static void change_volume_button(ui_state *ui)
{
  if (!player_is_running(ui))
  {
    return;
  }

  gint volume = player_get_volume(ui);
  if (volume < 0)
  {
    return;
  }

  gtk_scale_button_set_value(GTK_SCALE_BUTTON(ui->gui->volume_button), volume / 100.0);
}

void set_quick_preview_end_splitpoint_safe(gint value, ui_state *ui)
{
  lock_mutex(&ui->variables_mutex);
  ui->status->quick_preview_end_splitpoint = value;
  unlock_mutex(&ui->variables_mutex);
}

gint get_quick_preview_end_splitpoint_safe(ui_state *ui)
{
  lock_mutex(&ui->variables_mutex);
  gint quick_preview_end_splitpoint = ui->status->quick_preview_end_splitpoint;
  unlock_mutex(&ui->variables_mutex);
  return quick_preview_end_splitpoint;
}

//!progress bar synchronisation with player
static void change_progress_bar(ui_state *ui)
{
  gui_status *status = ui->status;
  ui_infos *infos = ui->infos;

  if (!player_is_running(ui) || status->mouse_on_progress_bar)
  {
    refresh_drawing_area(ui->gui);
    return;
  }

  infos->total_time = player_get_total_time(ui) / 10;

  infos->current_time = infos->player_seconds * 100 + 
    infos->player_minutes * 6000 +
    infos->player_hundr_secs;

  gdouble adj_position = (infos->current_time * 100000) / infos->total_time;
  gtk_adjustment_set_value(ui->gui->progress_adj, adj_position);

  infos->current_time = get_elapsed_time(ui);

  gint stop_splitpoint = get_splitpoint_time(get_quick_preview_end_splitpoint_safe(ui), ui);
  gint start_splitpoint = get_splitpoint_time(status->preview_start_splitpoint, ui);
  if ((stop_splitpoint < (gint)(infos->current_time-150)) ||
      (start_splitpoint > (gint)(infos->current_time+150)))
  {
    cancel_quick_preview(status);
  }
}

//!creates the filename player hbox
static GtkWidget *create_filename_player_hbox(gui_state *gui)
{
  GtkWidget *song_name_label = gtk_label_new("");
  gtk_label_set_selectable(GTK_LABEL(song_name_label), TRUE);
  gui->song_name_label = song_name_label;

  g_object_set(G_OBJECT(song_name_label), "selectable", FALSE, NULL);

  gtk_label_set_ellipsize(GTK_LABEL(song_name_label), PANGO_ELLIPSIZE_END); 

  GtkWidget *filename_player_hbox = wh_hbox_new();

#if GTK_MAJOR_VERSION <= 2
  //ellipsize does not work as in gtk+2, so we show the label in the middle
  gtk_box_pack_start(GTK_BOX(filename_player_hbox), song_name_label, 
      TRUE, TRUE, 15);
#else
  gtk_box_pack_start(GTK_BOX(filename_player_hbox), song_name_label, FALSE, FALSE, 15);
#endif

  return filename_player_hbox;
}

//!returns the value of the right drawing area
gfloat get_right_drawing_time(gfloat current_time, gfloat total_time, gfloat zoom_coeff)
{
  gfloat right = total_time / zoom_coeff;
  gfloat center = right/2;
  gfloat offset = current_time - center;
  return right + offset;
}

//!returns the value of the left drawing area
gfloat get_left_drawing_time(gfloat current_time, gfloat total_time, gfloat zoom_coeff)
{
  gfloat right = total_time / zoom_coeff;
  gfloat center = right/2;
  return current_time - center;
}

/*!Converts a time value to a string we can display 

\param hundr_or_not = TRUE means we also draw hundr of secs
\param time The time in hundreths of a second
*/
static gchar *get_time_for_drawing(gchar *str, gint time, gboolean hundr_or_not, gint *number_of_chars)
{
  gint mins = time / 6000;
  gint secs = (time / 100) % 60;

  if (hundr_or_not)
  {
    gint hundr = time % 100;
    *number_of_chars = g_snprintf(str, 30, "%d:%02d:%02d", mins, secs, hundr);
  }
  else
  {
    *number_of_chars = g_snprintf(str, 30, "%d:%02d", mins, secs);
  }

  return str;
}

//transform pixels to time
static gfloat pixels_to_time(gfloat width, gint pixels, ui_state *ui)
{
  return (ui->infos->total_time * (gfloat)pixels)/(width * ui->infos->zoom_coeff);
}

static gfloat time_to_pixels_float(gint width, gfloat time, gfloat total_time, gfloat zoom_coeff)
{
  return width * time * zoom_coeff / total_time;
}

static gint time_to_pixels(gint width, gfloat time, gfloat total_time, gfloat zoom_coeff)
{
  return roundf(time_to_pixels_float(width, time, total_time, zoom_coeff));
}

/*!returns the position of a time mark on the screen

\param width The width of the drawing 
\param The time in hundreths of a second
*/
static gint convert_time_to_pixels(gint width, gfloat time, 
    gfloat current_time, gfloat total_time, gfloat zoom_coeff)
{
  return width/2 + time_to_pixels(width, time - current_time, total_time, zoom_coeff);
}

static void draw_motif(GtkWidget *da, cairo_t *gc, gint ylimit, gint x, gint time_interval)
{
  GdkColor color;
  switch (time_interval)
  {
    case HUNDR_SECONDS:
      color.red = 65000;color.green = 0;color.blue = 0;
      break;
    case TENS_OF_SECONDS:
      color.red = 0;color.green = 0;color.blue = 65000;
      break;
    case SECONDS:
      color.red = 0;color.green = 65000;color.blue = 0;
      break;
    case TEN_SECONDS:
      color.red = 65000;color.green = 0;color.blue = 40000;
      break;
    case MINUTES:
      color.red = 1000;color.green = 10000;color.blue = 65000;
      break;
    case TEN_MINUTES:
      color.red = 65000;color.green = 0;color.blue = 0;
      break;
    case HOURS:
      color.red = 0;color.green = 0;color.blue = 0;
      break;
    default:
      break;
  }

  dh_set_color(gc, &color);

  draw_point(gc, x, ylimit+6);
  draw_point(gc, x, ylimit+7);
  draw_point(gc, x, ylimit+8);
  draw_point(gc, x-1, ylimit+8);
  draw_point(gc, x+1, ylimit+8);
  draw_point(gc, x, ylimit+9);
  draw_point(gc, x-1, ylimit+9);
  draw_point(gc, x+1, ylimit+9);
  draw_point(gc, x-2, ylimit+9);
  draw_point(gc, x+2, ylimit+9);
  draw_point(gc, x-3, ylimit+9);
  draw_point(gc, x+3, ylimit+9);
  draw_point(gc, x, ylimit+10);
  draw_point(gc, x-1, ylimit+10);
  draw_point(gc, x+1, ylimit+10);
  draw_point(gc, x-2, ylimit+10);
  draw_point(gc, x+2, ylimit+10);
  draw_point(gc, x-3, ylimit+10);
  draw_point(gc, x+3, ylimit+10);

  cairo_stroke(gc);
}

//!draw the marks, minutes, seconds...
static void draw_marks(gint time_interval, gint left_mark,
    gint right_mark, gint ylimit, GtkWidget *da, cairo_t *gc, ui_state *ui)
{
  gint left2 = (left_mark / time_interval) * time_interval;
  if (left2 < left_mark)
  {
    left2 += time_interval;
  }

  gint i;
  gint i_pixel;
  for (i = left2; i <= right_mark; i += time_interval)
  {
    i_pixel = convert_time_to_pixels(ui->infos->width_drawing_area, i,
        ui->infos->current_time, ui->infos->total_time, ui->infos->zoom_coeff);

    draw_motif(da, gc, ylimit, i_pixel, time_interval);
  }
}

//!full cancel of the quick preview
void cancel_quick_preview_all(ui_state *ui)
{
  cancel_quick_preview(ui->status);
  set_quick_preview_end_splitpoint_safe(-1, ui);
  ui->status->preview_start_splitpoint = -1;
}

//!cancels quick preview
void cancel_quick_preview(gui_status *status)
{
  status->quick_preview = FALSE;
}

/*!motif for splitpoints

\param draw false if we draw the splitpoint we move
\param move FALSE means we don't move the splitpoint,
\param move = TRUE means we move the splitpoint
\param number_splitpoint is the current splitpoint we draw
\param splitpoint_checked = TRUE if the splitpoint is checked
*/
static void draw_motif_splitpoints(GtkWidget *da, cairo_t *gc,
    gint x, gint draw, gint current_point_hundr_secs,
    gboolean move, gint number_splitpoint, ui_state *ui)
{
  int m = ui->gui->margin - 1;

  Split_point point = g_array_index(ui->splitpoints, Split_point, number_splitpoint);
  gboolean splitpoint_checked = point.checked;
 
  GdkColor color;
  color.red = 255 * 212; color.green = 255 * 100; color.blue = 255 * 200;
  dh_set_color(gc, &color);
  
  //if it' the splitpoint we move, don't fill in the circle and the square
  if (!draw)
  {
    dh_draw_rectangle(gc, FALSE, x-6,4, 11,11);
  }
  else
  {
    dh_draw_rectangle(gc, TRUE, x-6,4, 12,12);

    if (number_splitpoint == get_first_splitpoint_selected(ui->gui))
    {
      color.red = 255 * 220; color.green = 255 * 220; color.blue = 255 * 255;
      dh_set_color(gc, &color);

      dh_draw_rectangle(gc, TRUE, x-4,6, 8,8);
    }
  }

  color.red = 255 * 212; color.green = 255 * 196; color.blue = 255 * 221;
  dh_set_color(gc, &color);
  
  gint i = 0;
  for(i = 0;i < 5;i++)
  {
    draw_point(gc, x+i, ui->gui->erase_split_ylimit + m + 3);
    draw_point(gc, x-i, ui->gui->erase_split_ylimit + m + 3);
    draw_point(gc, x+i, ui->gui->erase_split_ylimit + m + 4);
    draw_point(gc, x-i, ui->gui->erase_split_ylimit + m + 4);
  }
  cairo_stroke(gc);
  
  if (move)
  {
    if (splitpoint_checked)
    {
      color.red = 15000;color.green = 40000;color.blue = 25000;
    }
    else
    {
      color.red = 25000;color.green = 25000;color.blue = 40000;
    }
    dh_set_color(gc, &color);

    dh_draw_line(gc, x, ui->gui->erase_split_ylimit + m -8, x, ui->gui->progress_ylimit + m, TRUE, TRUE);
  }
  
  color.red = 255 * 22; color.green = 255 * 35; color.blue = 255 * 91;
  dh_set_color(gc, &color);
  
  //draw the splitpoint motif
  for (i = -3;i <= 1;i++)
  {
    draw_point(gc, x, ui->gui->erase_split_ylimit + m +i);
  }
  for (i = 2;i <= 5;i++)
  {
    draw_point(gc, x, ui->gui->erase_split_ylimit + m + i);
  }
  for (i = 3;i <= 4;i++)
  {
    draw_point(gc, x-1, ui->gui->erase_split_ylimit + m + i);
    draw_point(gc, x+1, ui->gui->erase_split_ylimit + m + i);
  }
  for (i = 6;i <= 11;i++)
  {
    draw_point(gc, x, ui->gui->erase_split_ylimit + m + i);
  }
  
  //bottom splitpoint vertical bar
  for (i = 0;i < ui->gui->margin;i++)
  {
    draw_point(gc, x, ui->gui->progress_ylimit + m - i);
  }

  //bottom checkbox vertical bar
  for (i = 0;i < ui->gui->margin;i++)
  {
    draw_point(gc, x, ui->gui->splitpoint_ypos + m - i - 1);
  }
  cairo_stroke(gc);

  //bottom rectangle
  dh_set_color(gc, &color);
  color.red = 25000;color.green = 25000;color.blue = 25000;
  dh_draw_rectangle(gc, FALSE, x-6, ui->gui->splitpoint_ypos + m, 12,12);

  //draw a cross with 2 lines if the splitpoint is checked
  if (splitpoint_checked)
  {
    gint left = x - 6;
    gint right = x + 6;
    gint top = ui->gui->splitpoint_ypos + m;
    gint bottom = ui->gui->splitpoint_ypos + m + 12;
    dh_draw_line(gc, left, top, right, bottom, FALSE, TRUE);
    dh_draw_line(gc, left, bottom, right, top, FALSE, TRUE);
  }
  
  //-if the splitpoint is checked, set green color
  if (splitpoint_checked)
  {
    color.red = 15000;color.green = 40000;color.blue = 25000;
  }
  else
  {
    color.red = 25000;color.green = 25000;color.blue = 40000;
  }
  dh_set_color(gc, &color);
  
  dh_draw_arc(gc, FALSE, x, ui->gui->progress_ylimit + m+ 1 + 7, 14 / 2, 0, 2 * G_PI);

  //only fill the circle if we don't move that splitpoint
  if (draw)
  {
    dh_draw_arc(gc, TRUE, x, ui->gui->progress_ylimit + m + 1 + 8, 16 / 2, 0, 2 * G_PI);
  }
  
  if (draw)
  {
    gint number_of_chars = 0;
    gchar str[30] = { '\0' };
    get_time_for_drawing(str, current_point_hundr_secs, TRUE, &number_of_chars);
    dh_draw_text(gc, str, x - (number_of_chars * 3), ui->gui->checkbox_ypos + ui->gui->margin - 1);
  }

  if (ui->status->show_silence_wave)
  {
    color.red = 0;color.green = 0;color.blue = 0;
    dh_set_color(gc, &color);
    dh_draw_line(gc, x,ui->gui->text_ypos + ui->gui->margin, x,ui->gui->wave_ypos, move, TRUE);
  }
}

//!left, right mark in hundreths of seconds
static void draw_splitpoints(gint left_mark, gint right_mark, GtkWidget *da, cairo_t *gc,
    ui_state *ui)
{
  gint i = 0;
  for(i = 0; i < ui->infos->splitnumber; i++ )
  {
    gint current_point_hundr_secs = get_splitpoint_time(i, ui);
    if ((current_point_hundr_secs <= right_mark) &&
        (current_point_hundr_secs >= left_mark))
    {
      gint split_pixel = 
        convert_time_to_pixels(ui->infos->width_drawing_area, current_point_hundr_secs, 
            ui->infos->current_time, ui->infos->total_time, ui->infos->zoom_coeff);

      //the splitpoint that we move, draw it differently
      gboolean draw = TRUE;
      if (ui->status->splitpoint_to_move == i)
      {
        draw = FALSE;
      }

      draw_motif_splitpoints(da, gc, split_pixel, draw, current_point_hundr_secs, FALSE, i, ui);
    }
  }
}

static gint get_silence_filtered_presence_index(gfloat draw_time, ui_infos *infos)
{
  //num_of_points_coeff_f : ogg ~= 1, mp3 ~= 4
  gfloat num_of_points_coeff_f =
    ceil((infos->number_of_silence_points / infos->total_time) * 10);
  gint num_of_points_coeff = (gint)num_of_points_coeff_f;

  if (draw_time > infos->fourty_minutes_time)
  {
    if (num_of_points_coeff < 3)
    {
      return 2;
    }
    return 4;
  }

  if (draw_time > infos->twenty_minutes_time)
  {
    if (num_of_points_coeff < 3)
    {
      return 1;
    }
    return 3;
  }

  if (draw_time > infos->ten_minutes_time)
  {
    if (num_of_points_coeff < 3)
    {
      return 0;
    }
    return 2;
  }

  if (draw_time > infos->six_minutes_time)
  {
    if (num_of_points_coeff < 3)
    {
      return -1;
    }
    return 1;
  }

  if (draw_time > infos->three_minutes_time)
  {
    if (num_of_points_coeff < 3)
    {
      return -1;
    }
    return 0;
  }

  return -1;
}

static gint point_is_filtered(gint index, gint filtered_index, ui_infos *infos)
{
  if (!infos->filtered_points_presence)
  {
    return TRUE;
  }

  GArray *points_presence = g_ptr_array_index(infos->filtered_points_presence, filtered_index);
  return !g_array_index(points_presence, gint, index);
}

static gint adjust_filtered_index_according_to_number_of_points(gint filtered_index, 
    gint left_mark, gint right_mark, ui_state *ui)
{
  ui_infos *infos = ui->infos;

  if (filtered_index == 5)
  {
    return filtered_index;
  }

  gint number_of_points = 0;
  gint number_of_filtered_points = 0;

  gint i = 0;
  for (i = 0;i < infos->number_of_silence_points;i++)
  {
    long time = infos->silence_points[i].time;
    if ((time > right_mark) || (time < left_mark)) 
    {
      continue;
    }

    if (filtered_index >= 0 && point_is_filtered(i, filtered_index, infos))
    {
      number_of_filtered_points++;
    }

    number_of_points++;
  }

  if (number_of_points <= ui->infos->silence_wave_number_of_points_threshold)
  {
    return -1;
  }

  if (number_of_points - number_of_filtered_points > ui->infos->silence_wave_number_of_points_threshold)
  {
    return filtered_index + 1;
  }

  return filtered_index;
}

//! Draws the silence wave
gint draw_silence_wave(gint left_mark, gint right_mark, 
    gint interpolation_text_x, gint interpolation_text_y,
    gfloat draw_time, gint width_drawing_area, gint y_margin,
    gfloat current_time, gfloat total_time, gfloat zoom_coeff, 
    GtkWidget *da, cairo_t *gc, ui_state *ui)
{
  if (ui->status->currently_compute_douglas_peucker_filters ||
      get_currently_scanning_for_silence_safe(ui))
  {
    return -1;
  }

  GdkColor color;

  if (!ui->infos->silence_points)
  {
    color.red = 0;color.green = 0;color.blue = 0;
    dh_set_color(gc, &color);
    dh_draw_text_with_size(gc,_("No available wave"), 
        interpolation_text_x, interpolation_text_y, 13);
    return -1;
  }

  double dashes[] = { 1.0, 3.0 };
  cairo_set_dash(gc, dashes, 0, 0.0);
  cairo_set_line_width(gc, 1.0);
  cairo_set_line_cap(gc, CAIRO_LINE_CAP_ROUND);

  color.red = 0;color.green = 0;color.blue = 0;
  dh_set_color(gc, &color);

  gint filtered_index = get_silence_filtered_presence_index(draw_time, ui->infos);
  gint interpolation_level = 
    adjust_filtered_index_according_to_number_of_points(filtered_index, left_mark, right_mark, ui);

  gint stroke_counter = 0;

  if (!double_equals(zoom_coeff, ui->status->previous_zoom_coeff) ||
      interpolation_level != ui->status->previous_interpolation_level)
  {
    clear_previous_distances(ui);
  }
  ui->status->previous_zoom_coeff = zoom_coeff;
  ui->status->previous_interpolation_level = interpolation_level;

  GHashTable* distance_by_time = g_hash_table_new_full(g_int64_hash, g_int64_equal, g_free, g_free);

  gint i = 0;
  gint previous_x = 0;
  long second_splitpoint_time = -2;
  gint second_splitpoint_time_displayed = 0;
  gint missed_lookups = 0;
  gint time_counter = 0;
  for (i = 0;i < ui->infos->number_of_silence_points;i++)
  {
    if (interpolation_level >= 0 && point_is_filtered(i, interpolation_level, ui->infos))
    {
      continue;
    }

    long time = ui->infos->silence_points[i].time;
    if (time_counter == 1)
    {
      second_splitpoint_time = time;
    }
    time_counter++;

    if ((time > right_mark) || (time < left_mark)) 
    {
      continue;
    }

    float level = ui->infos->silence_points[i].level;

    gint x =
      convert_time_to_pixels(width_drawing_area, (gfloat)time, current_time, total_time, zoom_coeff);
    gint y = y_margin + (gint)floorf(level);

    if (time > second_splitpoint_time && second_splitpoint_time > -2)
    {
      gint64 *time_key = g_new(gint64, 1);
      *time_key = (gint64)time;

      gint *has_distance = g_hash_table_lookup(distance_by_time, time_key);
      if (has_distance != NULL) { continue; }

      if (ui->status->previous_distance_by_time != NULL)
      {
        gint *previous_diff = 
          g_hash_table_lookup(ui->status->previous_distance_by_time, time_key);

        //if we miss some lookups at the start, invalid distances
        if (previous_diff == NULL && stroke_counter < 20)
        {
          missed_lookups++;
          if (missed_lookups > 3)
          {
            clear_previous_distances(ui);
          }
        }

        if (previous_diff != NULL && stroke_counter > 0)
        {
          x = previous_x + *previous_diff;
          if (stroke_counter == 1)
          {
            ui->status->previous_second_x_drawed = x;
            ui->status->previous_second_time_drawed = time;
          }
        }
        else if (stroke_counter == 0 && !second_splitpoint_time_displayed)
        {
          if (x < 0) { x = 0; }

          //the first point always needs to grow up
          if (time == ui->status->previous_first_time_drawed && 
              x > ui->status->previous_first_x_drawed)
          {
            x = ui->status->previous_first_x_drawed;
          }
          else
          {
            //when the second point becomes the first point, dont make it grow
            if (time == ui->status->previous_second_time_drawed && 
                x > ui->status->previous_second_x_drawed)
            {
              x = ui->status->previous_second_x_drawed;
            }

            ui->status->previous_first_time_drawed = time;
            ui->status->previous_first_x_drawed = x;
          }
        }
      }

      gint *diff = g_new(gint, 1);
      *diff = x - previous_x;
      if (*diff < 0) { *diff = 0; }

      g_hash_table_insert(distance_by_time, time_key, diff);
    }
    else if (time == second_splitpoint_time)
    {
      second_splitpoint_time_displayed = 1;
    }

    previous_x = x;

    cairo_line_to(gc, x, y);

    stroke_counter++;
    if (stroke_counter % 4 == 0)
    {
      cairo_stroke(gc);
    }
    cairo_move_to(gc, x, y);
  }

  if (ui->status->previous_distance_by_time != NULL)
  {
    g_hash_table_destroy(ui->status->previous_distance_by_time);
  }
  ui->status->previous_distance_by_time = distance_by_time;

  cairo_stroke(gc);

  color.red = 0;color.green = 0;color.blue = 0;
  dh_set_color(gc, &color);

  if (interpolation_level < 0)
  {
    dh_draw_text_with_size(gc,_("No wave interpolation"), 
        interpolation_text_x, interpolation_text_y, 13);
  }
  else
  {
    gchar interpolation_text[128] = { '\0' };
    g_snprintf(interpolation_text, 128, _("Wave interpolation level %d"), interpolation_level + 1);
    dh_draw_text_with_size(gc, interpolation_text, interpolation_text_x, interpolation_text_y, 13);
  }

  return interpolation_level;
}

void clear_previous_distances(ui_state *ui)
{
  gui_status *status = ui->status;

  if (status->previous_distance_by_time != NULL)
  {
    g_hash_table_destroy(status->previous_distance_by_time);
    status->previous_distance_by_time = NULL; 
  }

  status->previous_first_time_drawed = -2;
  status->previous_first_x_drawed = -2;
  status->previous_second_x_drawed = -2;
  status->previous_second_time_drawed = -2;
}

static void draw_rectangles_between_splitpoints(cairo_t *cairo_surface, ui_state *ui)
{
  GdkColor color;

  //yellow small rectangle
  gint point_time_left = -1;
  gint point_time_right = -1;
  get_current_splitpoints_time_left_right(&point_time_left, &point_time_right, NULL, ui);
  color.red = 255 * 255;color.green = 255 * 255;color.blue = 255 * 210;
  draw_small_rectangle(point_time_left, point_time_right, color, cairo_surface, ui);

  gint gray_factor = 210;
  color.red = 255 * gray_factor;color.green = 255 * gray_factor;color.blue = 255 * gray_factor;

  //gray areas
  if (ui->infos->splitnumber == 0)
  {
    draw_small_rectangle(0, ui->infos->total_time, color, cairo_surface, ui);
    return;
  }

  draw_small_rectangle(0, get_splitpoint_time(0, ui), color, cairo_surface, ui);
  draw_small_rectangle(get_splitpoint_time(ui->infos->splitnumber-1, ui), ui->infos->total_time,
      color, cairo_surface, ui);
  gint i = 0;
  for (i = 0; i < ui->infos->splitnumber - 1; i++ )
  {
    Split_point point = g_array_index(ui->splitpoints, Split_point, i);
    if (!point.checked)
    {
      gint left_time = get_splitpoint_time(i, ui);
      gint right_time = get_splitpoint_time(i+1, ui);
      draw_small_rectangle(left_time, right_time, color, cairo_surface, ui);
    }
  }
}

#if GTK_MAJOR_VERSION <= 2
static gboolean da_draw_event(GtkWidget *da, GdkEventExpose *event, ui_state *ui)
{
  cairo_t *gc = gdk_cairo_create(da->window);
#else
static gboolean da_draw_event(GtkWidget *da, cairo_t *gc, ui_state *ui)
{
#endif

  ui_infos *infos = ui->infos;
  gui_state *gui = ui->gui;
  gui_status *status = ui->status;

#ifdef __WIN32__
  if ((status->playing || status->timer_active) &&
      get_process_in_progress_safe(ui))
  {
    GdkColor mycolor;
    mycolor.red = 255 * 0; mycolor.green = 255 * 0; mycolor.blue = 255 * 255;
    dh_set_color(gc, &mycolor);
    dh_draw_text_with_size(gc, _(" Please wait for the process to finish ..."),
        30, gui->margin - 3, 13);

    return TRUE;
  }
#endif

  set_process_in_progress_safe(TRUE, ui);

  if (gui->drawing_area_expander != NULL &&
      !gtk_expander_get_expanded(GTK_EXPANDER(gui->drawing_area_expander)))
  {
    set_process_in_progress_safe(FALSE, ui);
    return TRUE;
  }

  //on a slow hardware, this improves a lot the computing performance
  if (status->currently_compute_douglas_peucker_filters)
  {
    GdkColor mycolor;
    mycolor.red = 255 * 0; mycolor.green = 255 * 0; mycolor.blue = 255 * 255;
    dh_set_color(gc, &mycolor);
    dh_draw_text_with_size(gc, _(" Please wait ... currently computing Douglas Peucker filters."),
        30, gui->margin - 3, 13);

    set_process_in_progress_safe(FALSE, ui);
    return TRUE;
  }

  gint old_width_drawing_area = infos->width_drawing_area;

  int width = 0, height = 0;
  wh_get_widget_size(da, &width, &height);
  if (status->show_silence_wave)
  {
    if (height != DRAWING_AREA_HEIGHT_WITH_SILENCE_WAVE)
    {
      gtk_widget_set_size_request(da, DRAWING_AREA_WIDTH, DRAWING_AREA_HEIGHT_WITH_SILENCE_WAVE);
    }
  }
  else
  {
    if (height != DRAWING_AREA_HEIGHT)
    {
      gtk_widget_set_size_request(da, DRAWING_AREA_WIDTH, DRAWING_AREA_HEIGHT);
    }
  }

  gint real_progress_length = 26;
  gint real_text_length = 12;

  gint erase_splitpoint_length = gui->real_erase_split_length + (gui->margin * 2);
  gint progress_length = real_progress_length + gui->margin;
  gint move_split_length = gui->real_move_split_length + gui->margin;
  gint text_length = real_text_length + gui->margin;
  gint checkbox_length = gui->real_checkbox_length + gui->margin;
  gint wave_length = gui->real_wave_length + gui->margin;

  //
  gui->erase_split_ylimit = erase_splitpoint_length;
  gui->progress_ylimit = gui->erase_split_ylimit + progress_length;
  gui->splitpoint_ypos = gui->progress_ylimit + move_split_length;
  gui->checkbox_ypos = gui->splitpoint_ypos + checkbox_length;
  gui->text_ypos = gui->checkbox_ypos + text_length + gui->margin;
  gui->wave_ypos = gui->text_ypos + wave_length + gui->margin;

  gint bottom_left_middle_right_text_ypos = gui->text_ypos;
  if (status->show_silence_wave)
  {
    bottom_left_middle_right_text_ypos = gui->wave_ypos;
  }

  gint nbr_chars = 0;

  wh_get_widget_size(da, &infos->width_drawing_area, NULL);

  if (infos->width_drawing_area != old_width_drawing_area)
  {
    refresh_preview_drawing_areas(gui);
  }

  GdkColor color;
  color.red = 255 * 235;color.green = 255 * 235; color.blue = 255 * 235;
  dh_set_color(gc, &color);

  //background rectangle
  dh_draw_rectangle(gc, TRUE, 0,0, infos->width_drawing_area, gui->wave_ypos + text_length + 2);

  color.red = 255 * 255;color.green = 255 * 255;color.blue = 255 * 255;
  dh_set_color(gc, &color);

  //background white rectangles
  dh_draw_rectangle(gc, TRUE, 0, gui->margin, infos->width_drawing_area, gui->real_erase_split_length);
  dh_draw_rectangle(gc, TRUE, 0, gui->erase_split_ylimit, infos->width_drawing_area, progress_length);
  dh_draw_rectangle(gc, TRUE, 0, gui->progress_ylimit+gui->margin, infos->width_drawing_area, gui->real_move_split_length);
  dh_draw_rectangle(gc, TRUE, 0, gui->splitpoint_ypos+gui->margin, infos->width_drawing_area, gui->real_checkbox_length);
  dh_draw_rectangle(gc, TRUE, 0, gui->checkbox_ypos+gui->margin, infos->width_drawing_area, text_length);
  if (status->show_silence_wave)
  {
    dh_draw_rectangle(gc, TRUE, 0, gui->text_ypos + gui->margin, infos->width_drawing_area, wave_length);
  }

  if (!status->playing || !status->timer_active)
  {
    color.red = 255 * 212; color.green = 255 * 100; color.blue = 255 * 200;
    dh_set_color(gc, &color);
    dh_draw_text(gc, _(" left click on splitpoint selects it, right click erases it"),
        0, gui->margin - 3);

    color.red = 0;color.green = 0;color.blue = 0;
    dh_set_color(gc, &color);
    dh_draw_text(gc, _(" left click + move changes song position, right click + move changes zoom"),
        0, gui->erase_split_ylimit + gui->margin);

    color.red = 15000;color.green = 40000;color.blue = 25000;
    dh_set_color(gc, &color);
    dh_draw_text(gc, 
        _(" left click on point + move changes point position, right click play preview"),
        0, gui->progress_ylimit + gui->margin);

    color.red = 0; color.green = 0; color.blue = 0;
    dh_set_color(gc, &color);
    dh_draw_text(gc, _(" left click on rectangle checks/unchecks 'keep splitpoint'"),
        0, gui->splitpoint_ypos + 1);

    set_process_in_progress_safe(FALSE, ui);
    return TRUE;
  }

  gfloat left_time = get_left_drawing_time(infos->current_time, infos->total_time, infos->zoom_coeff);
  gfloat right_time = get_right_drawing_time(infos->current_time, infos->total_time, infos->zoom_coeff);

  //marks to draw seconds, minutes...
  gint left_mark = (gint)left_time;
  gint right_mark = (gint)right_time;
  if (left_mark < 0)
  {
    left_mark = 0;
  }
  if (right_mark > infos->total_time)
  {
    right_mark = (gint)infos->total_time;
  }

  gfloat total_draw_time = right_time - left_time;

  gchar str[30] = { '\0' };
  gint beg_pixel = convert_time_to_pixels(infos->width_drawing_area, 0,
      infos->current_time, infos->total_time, infos->zoom_coeff);

  draw_rectangles_between_splitpoints(gc, ui);

  //blue color
  color.red = 255 * 150; color.green = 255 * 150; color.blue = 255 * 255;
  dh_set_color(gc, &color);

  //if it's the first splitpoint from play preview
  if (get_quick_preview_end_splitpoint_safe(ui) != -1)
  {
    gint right_pixel =
      convert_time_to_pixels(infos->width_drawing_area,
          get_splitpoint_time(get_quick_preview_end_splitpoint_safe(ui), ui),
          infos->current_time, infos->total_time, infos->zoom_coeff);
    gint left_pixel =
      convert_time_to_pixels(infos->width_drawing_area,
          get_splitpoint_time(status->preview_start_splitpoint, ui),
          infos->current_time, infos->total_time, infos->zoom_coeff);

    gint preview_splitpoint_length = right_pixel - left_pixel + 1;

    //top buttons
    dh_draw_rectangle(gc, TRUE, left_pixel, gui->progress_ylimit-2, preview_splitpoint_length, 3);

    //for quick preview, put red bar
    if (status->quick_preview)
    {
      color.red = 255 * 255;color.green = 255 * 160;color.blue = 255 * 160;
      dh_set_color(gc, &color);
      dh_draw_rectangle(gc, TRUE, left_pixel, gui->erase_split_ylimit, preview_splitpoint_length, 3);
    }
  }
  else
  {
    //if we draw until the end
    if ((status->preview_start_splitpoint != -1) &&
        (status->preview_start_splitpoint != (infos->splitnumber-1)))
    {
      gint left_pixel =
        convert_time_to_pixels(infos->width_drawing_area,
            get_splitpoint_time(status->preview_start_splitpoint, ui),
            infos->current_time, infos->total_time, infos->zoom_coeff);
      dh_draw_rectangle(gc, TRUE, left_pixel, gui->progress_ylimit-2, infos->width_drawing_area-left_pixel, 3);

      //red bar quick preview
      if (status->quick_preview)
      {
        color.red = 255 * 255;color.green = 255 * 160;color.blue = 255 * 160;
        dh_set_color(gc, &color);
        dh_draw_rectangle(gc, TRUE, left_pixel, gui->erase_split_ylimit, infos->width_drawing_area-left_pixel, 3);
      }
    }
  }

  //song start
  if (left_time <= 0)
  {
    color.red = 255 * 235;color.green = 255 * 235; color.blue = 255 * 235;
    dh_set_color(gc, &color);
    dh_draw_rectangle(gc, TRUE, 0,0, beg_pixel, gui->wave_ypos);
  }
  else
  {
    color.red = 30000;color.green = 0;color.blue = 30000;
    dh_set_color(gc, &color);

    get_time_for_drawing(str, left_time, FALSE, &nbr_chars);
    dh_draw_text(gc, str, 15, bottom_left_middle_right_text_ypos);
  }

  gint end_pixel = 
    convert_time_to_pixels(infos->width_drawing_area, infos->total_time, 
        infos->current_time, infos->total_time, infos->zoom_coeff);
  //song end
  if (right_time >= infos->total_time)
  {
    color.red = 255 * 235;color.green = 255 * 235;color.blue = 255 * 235;
    dh_set_color(gc, &color);
    dh_draw_rectangle(gc, TRUE, end_pixel,0, infos->width_drawing_area, bottom_left_middle_right_text_ypos);
  }
  else
  {
    color.red = 30000;color.green = 0;color.blue = 30000;
    dh_set_color(gc, &color);

    get_time_for_drawing(str, right_time, FALSE, &nbr_chars);
    dh_draw_text(gc, str, infos->width_drawing_area - 52, bottom_left_middle_right_text_ypos);
  }

  if (total_draw_time < infos->hundr_secs_th)
  {
    draw_marks(HUNDR_SECONDS, left_mark, right_mark,
        gui->erase_split_ylimit + progress_length/4, da, gc, ui);
  }
  if (total_draw_time < infos->tens_of_secs_th)
  {
    draw_marks(TENS_OF_SECONDS, left_mark, right_mark,
        gui->erase_split_ylimit + progress_length/4, da, gc, ui);
  }
  if (total_draw_time < infos->secs_th)
  {
    draw_marks(SECONDS, left_mark, right_mark,
        gui->erase_split_ylimit + progress_length/4, da, gc, ui);
  }
  if (total_draw_time < infos->ten_secs_th)
  {
    draw_marks(TEN_SECONDS, left_mark, right_mark,
        gui->erase_split_ylimit + progress_length/4, da, gc, ui);
  }
  if (total_draw_time < infos->minutes_th)
  {
    draw_marks(MINUTES, left_mark, right_mark,
        gui->erase_split_ylimit + progress_length/4, da, gc, ui);
  }
  if (total_draw_time < infos->ten_minutes_th)
  {
    draw_marks(TEN_MINUTES, left_mark, right_mark,
        gui->erase_split_ylimit + progress_length/4, da, gc, ui);
  }
  draw_marks(HOURS, left_mark, right_mark,
      gui->erase_split_ylimit + progress_length/4, da, gc, ui);

  //draw mobile button1 position line
  if (status->button1_pressed)
  {
    gint move_pixel = convert_time_to_pixels(infos->width_drawing_area, 
        status->move_time, infos->current_time, 
        infos->total_time, infos->zoom_coeff);

    if (status->move_splitpoints)
    {
      draw_motif_splitpoints(da, gc, move_pixel,TRUE, status->move_time,
          TRUE, status->splitpoint_to_move, ui);

      color.red = 0;color.green = 0;color.blue = 0;
      dh_set_color(gc, &color);

      get_time_for_drawing(str, infos->current_time, FALSE, &nbr_chars);
      dh_draw_text(gc, str, infos->width_drawing_area/2-11, bottom_left_middle_right_text_ypos);
    }
    else
    {
      color.red = 255 * 255;color.green = 0;color.blue = 0;
      dh_set_color(gc, &color);
      dh_draw_line(gc, move_pixel, gui->erase_split_ylimit, move_pixel, gui->progress_ylimit, TRUE, TRUE);

      if (status->show_silence_wave)
      {
        dh_draw_line(gc, move_pixel, gui->text_ypos + gui->margin, move_pixel, gui->wave_ypos, TRUE, TRUE);
      }

      color.red = 0;color.green = 0;color.blue = 0;
      dh_set_color(gc, &color);

      get_time_for_drawing(str, status->move_time, FALSE, &nbr_chars);
      dh_draw_text(gc, str, infos->width_drawing_area/2-11, bottom_left_middle_right_text_ypos);
    }
  }
  else
  {
    color.red = 0;color.green = 0;color.blue = 0;
    dh_set_color(gc, &color);

    get_time_for_drawing(str, infos->current_time, FALSE, &nbr_chars);
    dh_draw_text(gc, str, infos->width_drawing_area/2-11, bottom_left_middle_right_text_ypos);
  }

  color.red = 255 * 255;color.green = 0;color.blue = 0;
  dh_set_color(gc, &color);

  //top middle line, current position
  dh_draw_line(gc, infos->width_drawing_area/2, gui->erase_split_ylimit,
      infos->width_drawing_area/2, gui->progress_ylimit, FALSE, TRUE);

  //silence wave
  if (status->show_silence_wave)
  {
    draw_silence_wave(left_mark, right_mark, 
        infos->width_drawing_area/2 + 3, gui->wave_ypos - gui->margin * 4,
        total_draw_time, 
        infos->width_drawing_area, gui->text_ypos + gui->margin,
        infos->current_time, infos->total_time, infos->zoom_coeff,
        da, gc, ui);

    //silence wave middle line
    color.red = 255 * 255;color.green = 0;color.blue = 0;
    dh_set_color(gc, &color);
    dh_draw_line(gc, infos->width_drawing_area/2, gui->text_ypos + gui->margin,
        infos->width_drawing_area/2, gui->wave_ypos, FALSE, TRUE);
  }

  draw_splitpoints(left_mark, right_mark, da, gc, ui);

#if GTK_MAJOR_VERSION <= 2
  cairo_destroy(gc);
#endif

  set_process_in_progress_safe(FALSE, ui);

  return TRUE;
}

static void draw_small_rectangle(gint time_left, gint time_right, 
    GdkColor color, cairo_t *cairo_surface, ui_state *ui)
{
  if (time_left == -1 || time_right == -1)
  {
    return;
  }

  gint pixels_left = convert_time_to_pixels(ui->infos->width_drawing_area, time_left, 
      ui->infos->current_time, ui->infos->total_time, ui->infos->zoom_coeff);
  gint pixels_right = convert_time_to_pixels(ui->infos->width_drawing_area, time_right, 
      ui->infos->current_time, ui->infos->total_time, ui->infos->zoom_coeff);
  gint pixels_length = pixels_right - pixels_left;

  dh_set_color(cairo_surface, &color);
  dh_draw_rectangle(cairo_surface, TRUE, pixels_left, ui->gui->erase_split_ylimit,
      pixels_length, ui->gui->progress_ylimit - ui->gui->erase_split_ylimit+1);

  if (ui->status->show_silence_wave)
  {
    dh_draw_rectangle(cairo_surface, TRUE, pixels_left, ui->gui->text_ypos + ui->gui->margin,
        pixels_length, ui->gui->real_wave_length + ui->gui->margin);
  }
}

void get_current_splitpoints_time_left_right(gint *time_left, gint *time_right, 
    gint *splitpoint_left, ui_state *ui)
{
  ui_infos *infos = ui->infos;

  gint i = 0;
  for (i = 0; i < infos->splitnumber; i++ )
  {
    gint current_point_hundr_secs = get_splitpoint_time(i, ui);
    if (current_point_hundr_secs < infos->current_time - (DELTA * 2))
    {
      *time_left = current_point_hundr_secs;
      continue;
    }

    if (current_point_hundr_secs > infos->current_time + (DELTA * 2))
    {
      *time_right = current_point_hundr_secs;
      if (splitpoint_left != NULL) { *splitpoint_left = i; }
      break;
    }
  }

  if (splitpoint_left != NULL && *splitpoint_left == -1)
  {
    *splitpoint_left = infos->splitnumber;
  }
}

/*!Acquire the number of the splitpoint that has been clicked on

\param type_clicked
 - 3 means right button
 - 1 means left button
\param type
 - 1 means erase splitpoint area,
 - 2 means move splitpoint area,
 - 3 means check splitpoint area
*/
static gint get_splitpoint_clicked(gint button_y, gint type_clicked, gint type, ui_state *ui)
{
  gint time_pos, time_right_pos;
  gint left_time = get_left_drawing_time(ui->infos->current_time, ui->infos->total_time, ui->infos->zoom_coeff);
  
  gint but_y;
  //click on a right button
  if (type_clicked != 3)
  {
    but_y = button_y;
    time_pos = left_time + pixels_to_time(ui->infos->width_drawing_area, ui->status->button_x, ui);
  }
  else
  {
    but_y = ui->status->button_y2;
    time_pos = left_time + pixels_to_time(ui->infos->width_drawing_area, ui->status->button_x2, ui);
  }

  //we get this to find time_right_pos - time_right
  //to see what time we have for X pixels
  gint pixels_to_look_for = ui->gui->real_erase_split_length / 2;
  if (type == 2)
  {
    pixels_to_look_for = ui->gui->real_move_split_length / 2;
  }

  if (type_clicked != 3)
  {
    time_right_pos = left_time +
      pixels_to_time(ui->infos->width_drawing_area, ui->status->button_x + pixels_to_look_for, ui);
  }
  else
  {
    time_right_pos = left_time +
      pixels_to_time(ui->infos->width_drawing_area, ui->status->button_x2 + pixels_to_look_for, ui);
  }

  //the time margin is the margin for the splitpoint,
  //where we can click at his left or right
  gint time_margin = time_right_pos - time_pos;
  
  gint margin1, margin2;
  
  if (type == 2)
  {
    margin1 = ui->gui->progress_ylimit + ui->gui->margin;
    margin2 = ui->gui->progress_ylimit + ui->gui->margin + ui->gui->real_move_split_length;
  }
  else if (type == 1)
  {
    margin1 = ui->gui->margin;
    margin2 = ui->gui->margin + ui->gui->real_erase_split_length;
  }
  else
  {
    margin1 = ui->gui->splitpoint_ypos + ui->gui->margin;
    margin2 = ui->gui->splitpoint_ypos + ui->gui->margin + ui->gui->real_checkbox_length;
  }

  //area outside the split move
  if ((but_y < margin1) || (but_y > margin2))
  {
    return -1;
  }

  gint i = 0;
  for(i = 0; i < ui->infos->splitnumber; i++ )
  {
    gint current_point_hundr_secs = get_splitpoint_time(i, ui);
    gint current_point_left = current_point_hundr_secs - time_margin;
    gint current_point_right = current_point_hundr_secs + time_margin;

    if ((time_pos >= current_point_left) && (time_pos <= current_point_right))
    {
      return i;
    }
  }

  return -1;
}

void set_preview_start_position_safe(gint value, ui_state *ui)
{
  lock_mutex(&ui->variables_mutex);
  ui->status->preview_start_position = value;
  unlock_mutex(&ui->variables_mutex);
}

gint get_preview_start_position_safe(ui_state *ui)
{
  lock_mutex(&ui->variables_mutex);
  gint preview_start_position = ui->status->preview_start_position;
  unlock_mutex(&ui->variables_mutex);

  return preview_start_position;
}

//!makes a quick preview of the song
void player_quick_preview(gint splitpoint_to_preview, ui_state *ui)
{
  if (splitpoint_to_preview == -1)
  {
    return;
  }

  gui_status *status = ui->status;

  set_preview_start_position_safe(get_splitpoint_time(splitpoint_to_preview, ui), ui);
  status->preview_start_splitpoint = splitpoint_to_preview;

  if (!player_is_playing(ui))
  {
    player_play(ui);
    usleep(50000);
  }

  if (player_is_paused(ui))
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ui->gui->pause_button), FALSE);
  }

  if (splitpoint_to_preview < ui->infos->splitnumber-1)
  {
    set_quick_preview_end_splitpoint_safe(splitpoint_to_preview + 1, ui);
  }
  else
  {
    set_quick_preview_end_splitpoint_safe(-1, ui);
  }

  player_seek(get_preview_start_position_safe(ui) * 10, ui);
  change_progress_bar(ui);
  put_status_message(_(" quick preview..."), ui);

  status->quick_preview = FALSE;
  if (get_quick_preview_end_splitpoint_safe(ui) != -1)
  {
    status->quick_preview = TRUE;
  }

  if (status->preview_start_splitpoint == (ui->infos->splitnumber-1))
  {
    cancel_quick_preview_all(ui);
  }
}

//!drawing area press event
static gboolean da_press_event(GtkWidget *da, GdkEventButton *event, ui_state *ui)
{
  if (!ui->status->playing || !ui->status->timer_active)
  {
    return TRUE;
  }

  gui_status *status = ui->status;
  gui_state *gui = ui->gui;
  ui_infos *infos = ui->infos;

  //left click
  if (event->button == 1)
  {
    status->button_x = event->x;
    status->button_y = event->y;
    status->button1_pressed = TRUE;

    if ((status->button_y > gui->progress_ylimit + gui->margin) &&
        (status->button_y < gui->progress_ylimit + gui->margin + gui->real_move_split_length))
    {
      status->splitpoint_to_move = get_splitpoint_clicked(status->button_y, 1, 2, ui);
      if (status->splitpoint_to_move != -1)
      {
        status->move_splitpoints = TRUE;
      }
    }
    else
    {
      //area to remove a splitpoint
      if ((status->button_y > gui->margin) &&
          (status->button_y < gui->margin + gui->real_erase_split_length))
      {
        gint splitpoint_selected = get_splitpoint_clicked(status->button_y, 1, 1, ui);
        if (splitpoint_selected != -1)
        {
          status->select_splitpoints = TRUE;
          select_splitpoint(splitpoint_selected, gui);
        }
        refresh_drawing_area(gui);
      }
      else
      {
        //area to check a splitpoint
        if ((status->button_y > gui->splitpoint_ypos + gui->margin) &&
            (status->button_y < gui->splitpoint_ypos + gui->margin + gui->real_checkbox_length))
        {
          gint splitpoint_selected = get_splitpoint_clicked(status->button_y, 1, 3, ui);
          if (splitpoint_selected != -1)
          {
            status->check_splitpoint = TRUE;
            update_splitpoint_check(splitpoint_selected, ui);
          }
          refresh_drawing_area(gui);
        }
      }
    }

    if (!status->move_splitpoints)
    {
      status->move_time = infos->current_time;
    }
    else
    {
      status->move_time = get_splitpoint_time(status->splitpoint_to_move, ui);
    }

    return TRUE;
  }

  //right click
  if (event->button == 3)
  {
    status->button_x2 = event->x;
    status->button_y2 = event->y;
    status->button2_pressed = TRUE;

    infos->zoom_coeff_old = infos->zoom_coeff;

    if ((status->button_y2 > gui->progress_ylimit + gui->margin) &&
        (status->button_y2 < gui->progress_ylimit + gui->margin + gui->real_move_split_length))
    {
      player_quick_preview(get_splitpoint_clicked(status->button_y2, 3, 2, ui), ui);
    }
    else
    {
      //to remove a splitpoint
      if ((status->button_y2 > gui->margin) && 
          (status->button_y2 < gui->margin + gui->real_erase_split_length))
      {
        gint splitpoint_to_erase = get_splitpoint_clicked(status->button_y2, 3, 1, ui);
        if (splitpoint_to_erase != -1)
        {
          status->remove_splitpoints = TRUE;
          remove_splitpoint(splitpoint_to_erase, TRUE, ui);
        }

        refresh_drawing_area(gui);
      }
    }
  }

  return TRUE;
}

//!drawing area release event
static gboolean da_unpress_event(GtkWidget *da, GdkEventButton *event, ui_state *ui)
{
  gui_status *status = ui->status;

  if (!status->playing || !status->timer_active)
  {
    goto end;
  }

  if (event->button == 1)
  {
    status->button1_pressed = FALSE;

    //if moving the current _position_
    if (!status->move_splitpoints && !status->remove_splitpoints &&
        !status->select_splitpoints && !status->check_splitpoint)
    {
      remove_status_message(ui->gui);
      player_seek((gint)(status->move_time * 10), ui);
      change_progress_bar(ui);

      //if more than 2 splitpoints & outside the split preview, cancel split preview
      if (get_quick_preview_end_splitpoint_safe(ui) == -1)
      {
        if (status->move_time < get_splitpoint_time(status->preview_start_splitpoint, ui))
        {
          cancel_quick_preview_all(ui);
        }
      }
      else
      {
        if ((status->move_time < get_splitpoint_time(status->preview_start_splitpoint, ui)) ||
            (status->move_time > get_splitpoint_time(get_quick_preview_end_splitpoint_safe(ui),ui)))
        {
          cancel_quick_preview_all(ui);
        }
        else
        {
          //if don't have a preview with the last splitpoint
          if (get_quick_preview_end_splitpoint_safe(ui) != -1)
          {
            if (player_is_paused(ui))
            {
              player_pause(ui);
            }
            status->quick_preview = TRUE;
          }
        }
      }
    }
    else if (status->move_splitpoints)
    {
      update_splitpoint_from_time(status->splitpoint_to_move, status->move_time, ui);
      status->splitpoint_to_move = -1;                
    }

    status->move_splitpoints = FALSE;
    status->select_splitpoints = FALSE;
    status->check_splitpoint = FALSE;
  }
  else if (event->button == 3)
  {
    status->button2_pressed = FALSE;
    status->remove_splitpoints = FALSE;
  }

end:
  refresh_drawing_area(ui->gui);
  return TRUE;
}

//!on drawing area event
static gboolean da_notify_event(GtkWidget *da, GdkEventMotion *event, ui_state *ui)
{
  gui_status *status = ui->status;
  ui_infos *infos = ui->infos;

  if ((status->playing && status->timer_active) &&
      (status->button1_pressed || status->button2_pressed))
  {
    gint x, y;
    GdkModifierType state;
    wh_get_pointer(event, &x, &y, &state);

    gint width = 0;
    wh_get_widget_size(ui->gui->drawing_area, &width, NULL);
    gfloat width_drawing_area = (gfloat) width;

    if (!state)
    {
      return TRUE;
    }

    if (status->button1_pressed)
    {
      if (status->move_splitpoints)
      {
        gdouble splitpoint_time = get_splitpoint_time(status->splitpoint_to_move, ui);
        status->move_time = splitpoint_time + 
          pixels_to_time(width_drawing_area, (x - status->button_x), ui);
      }
      else
      {
        if (status->remove_splitpoints || status->select_splitpoints || status->check_splitpoint)
        {
          status->move_time = infos->current_time;
        }
        else
        {
          status->move_time =
            infos->current_time + pixels_to_time(width_drawing_area, (x - status->button_x), ui);
        }
      }

      if (status->move_time < 0)
      {
        status->move_time = 0;
      }
      if (status->move_time > infos->total_time)
      {
        status->move_time = infos->total_time;
      }

      refresh_drawing_area(ui->gui);
    }
    else
    {
      if (status->button2_pressed)
      {
        gint diff = -((event->x - status->button_x2) * 1);
        if (diff < (-width_drawing_area + 1))
        {
          diff = -width_drawing_area + 1;
        }
        if (diff > (width_drawing_area - 1))
        {
          diff = width_drawing_area - 1;
        }

        infos->zoom_coeff = diff / (width_drawing_area);

        if (infos->zoom_coeff < 0)
        {
          infos->zoom_coeff = 1 / (infos->zoom_coeff + 1);
        }
        else
        {
          infos->zoom_coeff = 1 - infos->zoom_coeff;
        }

        infos->zoom_coeff = infos->zoom_coeff_old * infos->zoom_coeff;

        adjust_zoom_coeff(infos);

        refresh_drawing_area(ui->gui);
      }
    }
  }

  return TRUE;
}

void adjust_zoom_coeff(ui_infos *infos)
{
  if (infos->zoom_coeff < 0.2)
  {
    infos->zoom_coeff = 0.2;
  }
  if (infos->zoom_coeff > 10 * infos->total_time / 6000)
  {
    infos->zoom_coeff = 10 * infos->total_time / 6000;
  }
}

static void drawing_area_expander_event(GObject *object, GParamSpec *param_spec, ui_state *ui)
{
  if (object == NULL)
  {
    return;
  }

  GtkExpander *expander = GTK_EXPANDER(object);
  if (gtk_expander_get_expanded(expander))
  {
    gtk_widget_show(ui->gui->silence_wave_check_button);
    return;
  }

  gtk_widget_hide(ui->gui->silence_wave_check_button);
}

//!creates the progress drawing area under the player buttons
static GtkWidget *create_drawing_area(ui_state *ui)
{
  GtkWidget *frame = gtk_frame_new(NULL);
 
  gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_NONE);

  GtkWidget *drawing_area = gtk_drawing_area_new();
  dnd_add_drag_data_received_to_widget(drawing_area, DND_SINGLE_MODE_AUDIO_FILE_AND_DATA_FILES, ui);

  ui->gui->drawing_area = drawing_area;

  gtk_widget_set_size_request(drawing_area, DRAWING_AREA_WIDTH, DRAWING_AREA_HEIGHT);

#if GTK_MAJOR_VERSION <= 2
  g_signal_connect(drawing_area, "expose_event", G_CALLBACK(da_draw_event), ui);
#else
  g_signal_connect(drawing_area, "draw", G_CALLBACK(da_draw_event), ui);
#endif

  g_signal_connect(drawing_area, "button_press_event", G_CALLBACK(da_press_event), ui);
  g_signal_connect(drawing_area, "button_release_event", G_CALLBACK(da_unpress_event), ui);
  g_signal_connect(drawing_area, "motion_notify_event", G_CALLBACK(da_notify_event), ui);

  gtk_widget_set_events(drawing_area, gtk_widget_get_events(drawing_area)
      | GDK_LEAVE_NOTIFY_MASK | GDK_BUTTON_PRESS_MASK
      | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK
      | GDK_POINTER_MOTION_HINT_MASK);

  gtk_container_add(GTK_CONTAINER(frame), drawing_area);

  GtkWidget *drawing_area_expander =
    gtk_expander_new_with_mnemonic(_("Splitpoints and amplitude wave v_iew"));
  ui->gui->drawing_area_expander = drawing_area_expander;
  gtk_expander_set_expanded(GTK_EXPANDER(drawing_area_expander), TRUE);
  g_signal_connect(drawing_area_expander, "notify::expanded",
      G_CALLBACK(drawing_area_expander_event), ui);
  gtk_container_add(GTK_CONTAINER(drawing_area_expander), frame);

  return drawing_area_expander;
}

//!creates the control player frame, stop button, play button, etc.
GtkWidget *create_player_control_frame(ui_state *ui)
{
  GtkWidget *main_hbox = wh_hbox_new();
  
  GtkWidget *vbox = wh_vbox_new();
  gtk_box_pack_start(GTK_BOX(main_hbox), vbox, TRUE, TRUE, 0);

  //filename player hbox
  GtkWidget *hbox = create_filename_player_hbox(ui->gui);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);

  //the song informations
  hbox = create_song_informations_hbox(ui->gui);
  gtk_container_set_border_width(GTK_CONTAINER (hbox), 0);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 3);

  //audio progress bar
  hbox = create_song_bar_hbox(ui);
  gtk_container_set_border_width(GTK_CONTAINER(hbox), 0);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

  //drawing area
  GtkWidget *drawing_area = create_drawing_area(ui);
  gtk_container_set_border_width(GTK_CONTAINER(drawing_area), 0);
  gtk_box_pack_start(GTK_BOX(vbox), drawing_area, FALSE, FALSE, 0);

  //player buttons
  hbox = create_player_buttons_hbox(ui);
  gtk_container_set_border_width(GTK_CONTAINER(hbox), 0);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

  return main_hbox;
}

//!add a row to the table
void add_playlist_file(const gchar *name, ui_state *ui)
{
  if (!file_exists(name))
  {
    return;
  }

  gboolean name_already_exists_in_playlist = FALSE;

  GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->playlist_tree);

  gchar *filename = NULL;

  GtkTreeIter iter;

  gint i = 0;
  while (i < ui->infos->playlist_tree_number)
  {
    GtkTreePath *path = gtk_tree_path_new_from_indices(i ,-1);
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get(model, &iter, COL_FILENAME, &filename, -1);

    if (strcmp(filename, name) == 0)
    {
      name_already_exists_in_playlist = TRUE;
      break;
    }

    g_free(filename);
    i++;
  }

  if (! name_already_exists_in_playlist)
  {
    gtk_widget_set_sensitive(ui->gui->playlist_remove_all_files_button,TRUE);
    gtk_list_store_append(GTK_LIST_STORE(model), &iter);

    //sets text in the minute, second and milisecond column
    gtk_list_store_set(GTK_LIST_STORE(model), 
        &iter,
        COL_NAME, get_real_name_from_filename(name),
        COL_FILENAME, name,
        -1);
    ui->infos->playlist_tree_number++;
  }
}

//!creates the model for the playlist
static GtkTreeModel *create_playlist_model()
{
  GtkListStore * model = gtk_list_store_new(PLAYLIST_COLUMNS, G_TYPE_STRING, G_TYPE_STRING);
  return GTK_TREE_MODEL(model);
}

//!creates the playlist tree
static GtkTreeView *create_playlist_tree()
{
  GtkTreeModel *model = create_playlist_model();
  GtkTreeView *playlist_tree = GTK_TREE_VIEW(gtk_tree_view_new_with_model(model));
  gtk_tree_view_set_headers_visible(playlist_tree, FALSE);
  return playlist_tree;
}

//!creates playlist columns
void create_playlist_columns(GtkTreeView *playlist_tree)
{
  GtkCellRendererText *renderer = GTK_CELL_RENDERER_TEXT(gtk_cell_renderer_text_new());
  g_object_set_data(G_OBJECT(renderer), "col", GINT_TO_POINTER(COL_NAME));

  GtkTreeViewColumn *name_column = gtk_tree_view_column_new_with_attributes 
    (_("History"), GTK_CELL_RENDERER(renderer), "text", COL_NAME, NULL);
  gtk_tree_view_insert_column(playlist_tree, GTK_TREE_VIEW_COLUMN(name_column), COL_NAME);

  gtk_tree_view_column_set_alignment(GTK_TREE_VIEW_COLUMN(name_column), 0.5);
  gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(name_column), GTK_TREE_VIEW_COLUMN_AUTOSIZE);
}

//!split selection has changed
static void playlist_selection_changed(GtkTreeSelection *selec, ui_state *ui)
{
  GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->playlist_tree);
  GtkTreeSelection *selection = gtk_tree_view_get_selection(ui->gui->playlist_tree);
  GList *selected_list = gtk_tree_selection_get_selected_rows(selection, &model);
  if (g_list_length(selected_list) > 0)
  {
    gtk_widget_set_sensitive(ui->gui->playlist_remove_file_button, TRUE);
  }
  else
  {
    gtk_widget_set_sensitive(ui->gui->playlist_remove_file_button, FALSE);
  }
}

//!event for the remove file button
static void playlist_remove_file_button_event(GtkWidget *widget, ui_state *ui)
{
  GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->playlist_tree);
  GtkTreeSelection *selection = gtk_tree_view_get_selection(ui->gui->playlist_tree);
  GList *selected_list = gtk_tree_selection_get_selected_rows(selection, &model);

  gchar *filename = NULL;

  while (g_list_length(selected_list) > 0)
  {
    GList *current_element = g_list_last(selected_list);
    GtkTreePath *path = current_element->data;

    GtkTreeIter iter;
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get(model, &iter, 
        COL_FILENAME, &filename, -1);

    //remove the path from the selected list
    gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
    selected_list = g_list_remove(selected_list, path);
    //remove 1 to the row number of the table
    ui->infos->playlist_tree_number--;

    gtk_tree_path_free(path);
    g_free(filename);
  }

  if (ui->infos->playlist_tree_number == 0)
  {
    gtk_widget_set_sensitive(ui->gui->playlist_remove_all_files_button, FALSE);
  }

  gtk_widget_set_sensitive(ui->gui->playlist_remove_file_button,FALSE);

  g_list_foreach(selected_list, (GFunc)gtk_tree_path_free, NULL);
  g_list_free(selected_list);  
}

//!event for the remove file button
static void playlist_remove_all_files_button_event(GtkWidget *widget, ui_state *ui)
{
  GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->playlist_tree);
  
  gchar *filename = NULL;
  while (ui->infos->playlist_tree_number > 0)
  {
    GtkTreeIter iter;
    gtk_tree_model_get_iter_first(model, &iter);
    gtk_tree_model_get(model, &iter, 
        COL_FILENAME, &filename, -1);
    gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
    ui->infos->playlist_tree_number--;
    g_free(filename);
  }
  
  gtk_widget_set_sensitive(ui->gui->playlist_remove_all_files_button, FALSE);
  gtk_widget_set_sensitive(ui->gui->playlist_remove_file_button, FALSE);
}

//!creates the horizontal queue buttons horizontal box
static GtkWidget *create_delete_buttons_hbox(ui_state *ui)
{
  GtkWidget *hbox = wh_hbox_new();

  GtkWidget *playlist_remove_file_button = 
    wh_create_cool_button(GTK_STOCK_REMOVE, _("_Erase selected entries"), FALSE);
  ui->gui->playlist_remove_file_button = playlist_remove_file_button;
  gtk_box_pack_start(GTK_BOX(hbox), playlist_remove_file_button, FALSE, FALSE, 5);
  gtk_widget_set_sensitive(playlist_remove_file_button,FALSE);
  g_signal_connect(G_OBJECT(playlist_remove_file_button), "clicked",
                   G_CALLBACK(playlist_remove_file_button_event), ui);
 
  GtkWidget *playlist_remove_all_files_button =
    wh_create_cool_button(GTK_STOCK_CLEAR, _("E_rase all history"),FALSE);
  ui->gui->playlist_remove_all_files_button = playlist_remove_all_files_button;
  gtk_box_pack_start(GTK_BOX(hbox), playlist_remove_all_files_button, FALSE, FALSE, 5);
  gtk_widget_set_sensitive(playlist_remove_all_files_button,FALSE);
  g_signal_connect(G_OBJECT(playlist_remove_all_files_button), "clicked",
                   G_CALLBACK(playlist_remove_all_files_button_event), ui);

  return hbox;
}

//!creates the playlist of the player
GtkWidget *create_player_playlist_frame(ui_state *ui)
{
  GtkWidget *vbox = wh_vbox_new();

  //scrolled window and the tree 
  //create the tree and add it to the scrolled window
  GtkTreeView *playlist_tree = create_playlist_tree();
  dnd_add_drag_data_received_to_widget(GTK_WIDGET(playlist_tree), DND_SINGLE_MODE_AUDIO_FILE, ui);

  ui->gui->playlist_tree = playlist_tree;
  GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_NONE);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
      GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

  create_playlist_columns(playlist_tree);
  gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(playlist_tree));
  g_signal_connect(G_OBJECT(playlist_tree), "row-activated",
                   G_CALLBACK(split_tree_row_activated), ui);
 
  //selection for the tree
  GtkTreeSelection *playlist_tree_selection = gtk_tree_view_get_selection(playlist_tree);
  g_signal_connect(G_OBJECT(playlist_tree_selection), "changed",
                   G_CALLBACK(playlist_selection_changed), ui);
  gtk_tree_selection_set_mode(GTK_TREE_SELECTION(playlist_tree_selection), GTK_SELECTION_MULTIPLE);

  //horizontal box with delete buttons
  GtkWidget *delete_buttons_hbox = create_delete_buttons_hbox(ui);
  gtk_box_pack_start(GTK_BOX(vbox), delete_buttons_hbox, FALSE, FALSE, 2);

  GtkWidget *history_expander = gtk_expander_new_with_mnemonic(_("Files h_istory"));
  gtk_expander_set_expanded(GTK_EXPANDER(history_expander), FALSE);
  gtk_container_add(GTK_CONTAINER(history_expander), vbox);

  GtkWidget *main_hbox = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(main_hbox), history_expander, TRUE, TRUE, 4);

  return main_hbox;
}

static void action_set_sensitivity(gchar *name, gboolean sensitivity, gui_state *gui)
{
  GtkAction *action = gtk_action_group_get_action(gui->action_group, name);
  gtk_action_set_sensitive(action, sensitivity);
}

void player_key_actions_set_sensitivity(gboolean sensitivity, gui_state *gui)
{
  action_set_sensitivity("Player_pause", sensitivity, gui);
  action_set_sensitivity("Player_forward", sensitivity, gui);
  action_set_sensitivity("Player_backward", sensitivity, gui);
  action_set_sensitivity("Player_small_forward", sensitivity, gui);
  action_set_sensitivity("Player_small_backward", sensitivity, gui);
  action_set_sensitivity("Player_big_forward", sensitivity, gui);
  action_set_sensitivity("Player_big_backward", sensitivity, gui);
  action_set_sensitivity("Player_next_splitpoint", sensitivity, gui);
  action_set_sensitivity("Player_previous_splitpoint", sensitivity, gui);
  action_set_sensitivity("Player_before_closest_splitpoint", sensitivity, gui);
  action_set_sensitivity("Add_splitpoint", sensitivity, gui);
  action_set_sensitivity("Delete_closest_splitpoint", sensitivity, gui);
  action_set_sensitivity("Zoom_in", sensitivity, gui);
  action_set_sensitivity("Zoom_out", sensitivity, gui);
}

/*! timer used to print infos about the song

Examples are the elapsed time and if it uses variable bitrate
*/
static gint mytimer(ui_state *ui)
{
#ifdef __WIN32__
  if (get_process_in_progress_safe(ui))
  {
    return TRUE;
  }
#endif

  set_process_in_progress_safe(TRUE, ui);

  ui_infos *infos = ui->infos;
  gui_state *gui = ui->gui;
  gui_status *status = ui->status;

  //TODO: file from file chooser can be NULL and != from the real filename of mp3splt-gtk.
  //but this does not work: the user can no longer select a folder if we do this 
  /*if (gui->open_file_chooser_button != NULL)
  {
    gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(gui->open_file_chooser_button));
    if (filename == NULL)
    {
      gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(gui->open_file_chooser_button), 
          get_input_filename(ui->gui));
    }
  }*/

  if (!player_is_running(ui))
  {
    gtk_widget_set_sensitive(gui->silence_wave_check_button, FALSE);

    clear_data_player(gui);
    status->playing = FALSE;
    disconnect_button_event(gui->disconnect_button, ui);

    set_process_in_progress_safe(FALSE, ui);
    return FALSE;
  }

  gtk_widget_set_sensitive(gui->silence_wave_check_button, TRUE);

  if (status->playing)
  {
    if (player_get_playlist_number(ui) > -1)
    {
      if (player_is_playing(ui))
      {
        print_all_song_infos(ui);
        print_song_time_elapsed(ui);
        gtk_widget_set_sensitive(GTK_WIDGET(gui->progress_bar), TRUE);
      }

      check_stream(ui);

      //if we have a stream, we must not change the progress bar
      if (!status->stream)
      {
        change_progress_bar(ui);
      }

      //part of quick preview
      if (status->preview_start_splitpoint != -1)
      {
        //if we have a splitpoint after the current
        //previewed one, update quick_preview_end
        if (status->preview_start_splitpoint + 1 < infos->splitnumber)
        {
          set_quick_preview_end_splitpoint_safe(status->preview_start_splitpoint + 1, ui);
        }
        else
        {
          if (status->preview_start_splitpoint + 1 == infos->splitnumber)
          {
            set_quick_preview_end_splitpoint_safe(-1, ui);
          }
        }
      }

      //if we have a preview, stop if needed
      if (status->quick_preview)
      {
        gint stop_splitpoint = get_splitpoint_time(get_quick_preview_end_splitpoint_safe(ui), ui);

        if ((stop_splitpoint < (gint)infos->current_time)
            && (get_quick_preview_end_splitpoint_safe(ui) != -1))
        {
          gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui->pause_button), TRUE);
          cancel_quick_preview(status);
          put_status_message(_(" quick preview finished, song paused"), ui);
        }
      }

      gtk_widget_set_sensitive(GTK_WIDGET(gui->volume_button), TRUE);
    }
    else
    {
      status->playing = FALSE;
      reset_label_time(gui);
    }

    if (player_is_paused(ui))
    {
      status->only_press_pause = TRUE;
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui->pause_button), TRUE);
      status->only_press_pause = FALSE;
    }
    else
    {
      status->only_press_pause = TRUE;
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui->pause_button), FALSE);
      status->only_press_pause = FALSE;
    }
  }
  else
  {
    if ((infos->player_minutes != 0) || (infos->player_seconds != 0))
    {
      infos->player_minutes = 0;
      infos->player_seconds = 0;
    }

    print_player_filename(ui);
    reset_song_infos(gui);
    reset_label_time(gui);
    reset_inactive_progress_bar(gui);
    gtk_widget_set_sensitive(gui->player_add_button, FALSE);
  }

  //if connected, almost always change volume bar
  if ((ui->status->change_volume)&& (!ui->status->on_the_volume_button))
  {
    change_volume_button(ui);
  }

  status->playing = player_is_playing(ui);
  if (status->playing)
  {
    gtk_widget_set_sensitive(gui->player_add_button, TRUE);
    gtk_widget_set_sensitive(gui->stop_button, TRUE);
    wh_set_image_on_button(GTK_BUTTON(gui->stop_button), g_object_ref(gui->StopButton_active));

    gtk_widget_set_sensitive(gui->pause_button, TRUE);
    wh_set_image_on_button(GTK_BUTTON(gui->pause_button), g_object_ref(gui->PauseButton_active));

    player_key_actions_set_sensitivity(TRUE, gui);
  }
  else
  {
    gtk_widget_set_sensitive(gui->stop_button, FALSE);
    wh_set_image_on_button(GTK_BUTTON(gui->stop_button), g_object_ref(gui->StopButton_inactive));

    gtk_widget_set_sensitive(gui->pause_button, FALSE);
    wh_set_image_on_button(GTK_BUTTON(gui->pause_button), g_object_ref(gui->PauseButton_inactive));

    player_key_actions_set_sensitivity(FALSE, gui);
  }

  set_process_in_progress_safe(FALSE, ui);

  return TRUE;
}

//event for the file chooser ok button
void file_chooser_ok_event(const gchar *fname, ui_state *ui)
{
  change_current_filename(fname, ui);
  gtk_widget_set_sensitive(ui->gui->play_button, TRUE);
  wh_set_image_on_button(GTK_BUTTON(ui->gui->play_button), g_object_ref(ui->gui->PlayButton_active));

  ui->status->file_browsed = TRUE;

  if (ui->status->timer_active)
  {
    GList *song_list = NULL;
    song_list = g_list_append(song_list, g_strdup(fname));

    if (!player_is_running(ui))
    {
      player_start(ui);
    }
    else if (ui->status->playing)
    {
      player_stop(ui);
    }

    player_add_files_and_select(song_list, ui);

    if (ui->status->playing && !player_is_paused(ui))
    {
      player_play(ui);
    }

    g_list_foreach(song_list, (GFunc)g_free, NULL);
    g_list_free(song_list);
  }
}

