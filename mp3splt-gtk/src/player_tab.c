/**********************************************************
 * mp3splt-gtk -- utility based on mp3splt,
 *
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
 * The player control tab
 *
 * this file is used for the player control tab
 **********************************************************/

#include "player_tab.h"

#define DRAWING_AREA_WIDTH 400
#define DRAWING_AREA_HEIGHT 123 
#define DRAWING_AREA_HEIGHT_WITH_SILENCE_WAVE 232 

gint silence_wave_number_of_points_threshold = DEFAULT_SILENCE_WAVE_NUMBER_OF_POINTS_THRESHOLD;

//! Tells us if the file was browsed or not
gint file_browsed = FALSE;
gint file_in_entry = FALSE;

//handle box for detaching window
GtkWidget *file_handle_box;

GtkWidget *player_buttons_hbox = NULL;

//if we have selected a correct file
gint incorrect_selected_file = FALSE;

gint douglas_callback_counter = 0;

gint currently_compute_amplitude_data = FALSE;

static const gint hundr_secs_th = 20;
static const gint tens_of_secs_th = 3 * 100;
static const gint secs_th = 40 * 100;
static const gint ten_secs_th = 3 * 6000;
static const gint minutes_th = 20 * 6000;
static const gint ten_minutes_th = 3 * 3600 * 100;

const gint one_minute_time = 1 * 6000;
const gint three_minutes_time = 3 * 6000;
const gint six_minutes_time = 6 * 6000;
const gint ten_minutes_time = 10 * 6000;
const gint twenty_minutes_time = 20 * 6000;
const gint fourty_minutes_time = 40 * 6000;

extern GtkWidget *names_from_filename;

extern gint preview_start_position;
extern gint preview_start_splitpoint;
extern GtkWidget *browse_cddb_button;
extern GtkWidget *browse_cue_button;
extern gchar *filename_to_split;
extern gchar *filename_path_of_split;
extern GtkWidget *cancel_button;
extern gint debug_is_active;

extern ui_state *ui;

//volume bar
GtkWidget *volume_button;

//wether to change the volume of the player
gboolean change_volume = TRUE;
//to see if we are on the volume bar
gboolean on_the_volume_button = FALSE;
//variable that stocks if the song is playing or not
gboolean playing = FALSE;
//to see if we have a stream
gboolean stream = FALSE;

//connect and disconnect to player buttons
GtkWidget *connect_button;
GtkWidget *disconnect_button;

//player buttons
GtkWidget *play_button;
GtkWidget *stop_button;
GtkWidget *pause_button;
GtkWidget *player_add_button;
GtkWidget *go_beg_button;
GtkWidget *go_end_button;

//silence wave
GtkWidget *silence_wave_check_button = NULL;

gint we_scan_for_silence = FALSE;

//stock if the timer is active or not
gboolean timer_active = FALSE;
//id of the timeout, used to stop it
gint timeout_id;

//handle for detaching
GtkWidget *player_handle;

//handle for detaching playlist
GtkWidget *playlist_handle;
GtkWidget *playlist_handle_window;

extern gint file_browsed;
extern gint selected_player;

//just used here for the timer hack
gint stay_turn = 0;

//drawing zoom coefficient
gfloat zoom_coeff = 2.0;
gfloat zoom_coeff_old;
//drawing area buttons pressed
gboolean button1_pressed = FALSE;
gboolean button2_pressed = FALSE;
//drawing area pushed point left button
gint button_x;
gint button_y;
//drawing area pushed point right button
gint button_x2;
gint button_y2;
//time where we move
gfloat move_time;

extern gboolean quick_preview;
extern gint quick_preview_end_splitpoint;

gint timeout_value = DEFAULT_TIMEOUT_VALUE;

gint splitpoint_to_move = -1;
gboolean move_splitpoints = FALSE;
gboolean remove_splitpoints = FALSE;
gboolean select_splitpoints = FALSE;
gboolean check_splitpoint = FALSE;

gint only_press_pause = FALSE;

//our playlist tree
GtkTreeView *playlist_tree = NULL;
gint playlist_tree_number = 0;

/*! \defgroup Playerbuttons Button images for the player

\@{
*/
GtkWidget *Go_BegButton_active;
GtkWidget *Go_BegButton_inactive;
GtkWidget *Go_EndButton_active;
GtkWidget *Go_EndButton_inactive;
GtkWidget *PlayButton_active;
GtkWidget *PlayButton_inactive;
GtkWidget *StopButton_active;
GtkWidget *StopButton_inactive;
GtkWidget *PauseButton_active;
GtkWidget *PauseButton_inactive;
//@}

//remove file button
GtkWidget *playlist_remove_file_button;
//remove file button
GtkWidget *playlist_remove_all_files_button;

GPtrArray *filtered_points_presence = NULL;

gdouble douglas_peucker_thresholds[5] = { 2.0, 5.0, 8.0, 11.0, 15.0 };
gdouble douglas_peucker_thresholds_defaults[5] = { 2.0, 5.0, 8.0, 11.0, 15.0 };

//playlist tree enumeration
enum {
  COL_NAME,
  COL_FILENAME,
  PLAYLIST_COLUMNS 
};

//function declarations
gint mytimer(gpointer data);
extern void copy_filename_to_current_description(const gchar *fname);

static void draw_small_rectangle(gint time_left, gint time_right, 
    GdkColor color, cairo_t *cairo_surface);

//!function called from the library when scanning for the silence level
void get_silence_level(long time, float level, void *user_data)
{
  gint converted_level = (gint)floorf(abs(level));
  if (converted_level < 0)
  {
    return;
  }

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

static GArray *build_gdk_points_for_douglas_peucker()
{
  GArray *points = g_array_new(TRUE, TRUE, sizeof(GdkPoint));

  gint i = 0;
  for (i = 0;i < ui->infos->number_of_silence_points;i++)
  {
    long time = ui->infos->silence_points[i].time;
    float level = ui->infos->silence_points[i].level;

    GdkPoint point;
    point.x = (gint)time;
    point.y = (gint)floorf(level);
    g_array_append_val(points, point);
  }

  return points;
}

void douglas_peucker_callback()
{
  douglas_callback_counter++;

  if (douglas_callback_counter % 400 == 0)
  {
    gtk_progress_bar_pulse(ui->gui->percent_progress_bar);
    gtk_progress_bar_set_text(ui->gui->percent_progress_bar, 
        _("Processing Douglas-Peucker filters ..."));
    gtk_widget_queue_draw(GTK_WIDGET(ui->gui->percent_progress_bar));
    while (gtk_events_pending())
    {
      gtk_main_iteration();
    }

    douglas_callback_counter = 0;
  }
}

void compute_douglas_peucker_filters(GtkWidget *widget, gpointer data)
{
  douglas_callback_counter = 0;

  if (!ui->status->show_silence_wave)
  {
    return;
  }

  ui->status->currently_compute_douglas_peucker_filters = TRUE;

  GArray *gdk_points_for_douglas_peucker = build_gdk_points_for_douglas_peucker();

  splt_douglas_peucker_free(filtered_points_presence);
  filtered_points_presence = splt_douglas_peucker(gdk_points_for_douglas_peucker, 
      douglas_peucker_callback,
      douglas_peucker_thresholds[0], douglas_peucker_thresholds[1],
      douglas_peucker_thresholds[2], douglas_peucker_thresholds[3],
      douglas_peucker_thresholds[4], -1.0);

  g_array_free(gdk_points_for_douglas_peucker, TRUE);

  ui->status->currently_compute_douglas_peucker_filters = FALSE;

  check_update_down_progress_bar(ui);
}

gpointer detect_silence(gpointer data)
{
  gint err = SPLT_OK;

  if (ui->infos->silence_points)
  {
    g_free(ui->infos->silence_points);
    ui->infos->silence_points = NULL;
    ui->infos->number_of_silence_points = 0;
  }

  enter_threads();

  gtk_widget_set_sensitive(cancel_button, TRUE);
  filename_to_split = get_input_filename(ui->gui);

  exit_threads();

  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_DEBUG_MODE, debug_is_active);
  mp3splt_set_filename_to_split(ui->mp3splt_state, filename_to_split);

  mp3splt_set_silence_level_function(ui->mp3splt_state, get_silence_level, NULL);
  ui->status->splitting = TRUE;
  we_scan_for_silence = TRUE;

  mp3splt_set_silence_points(ui->mp3splt_state, &err);

  we_scan_for_silence = FALSE;
  ui->status->splitting = FALSE;
  mp3splt_set_silence_level_function(ui->mp3splt_state, NULL, NULL);

  enter_threads();

  compute_douglas_peucker_filters(NULL, NULL);

  print_status_bar_confirmation(err);
  gtk_widget_set_sensitive(cancel_button, FALSE);

  refresh_drawing_area(ui->gui);
  refresh_preview_drawing_areas(ui);

  exit_threads();

  return NULL;
}

/*! Initialize scanning for silence in the background.

  If showing the silence wave is disabled this function won't do anything.
 */
void scan_for_silence_wave()
{
  if (we_scan_for_silence)
  {
    cancel_button_event(NULL, NULL);
  }

  if (timer_active)
  {
    create_thread(detect_silence, NULL, TRUE, NULL);
  }
}

/*! Change the name of the song that is to be cut and played

Manages changing the filename itselves as well as recalculating the silence
wave if needed.
*/
void change_current_filename(const gchar *fname)
{
  const gchar *old_fname = get_input_filename(ui->gui);
  if (!old_fname)
  {
    set_input_filename(fname, ui->gui);
    if (ui->status->show_silence_wave)
    {
      scan_for_silence_wave();
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(names_from_filename)))
    {
      copy_filename_to_current_description(fname);
    }
  }
  else if (strcmp(old_fname, fname) != 0)
  {
    set_input_filename(fname, ui->gui);
    if (ui->status->show_silence_wave)
    {
      scan_for_silence_wave();
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(names_from_filename)))
    {
      copy_filename_to_current_description(fname);
    }
  }
}

//!resets and sets inactive the progress bar
void reset_inactive_progress_bar()
{
  gtk_widget_set_sensitive(GTK_WIDGET(ui->gui->progress_bar), FALSE);
  gtk_adjustment_set_value(ui->gui->progress_adj, 0);
}

//!resets and sets inactive the volume bar
void reset_inactive_volume_button()
{
  gtk_widget_set_sensitive(GTK_WIDGET(volume_button), FALSE);
  gtk_scale_button_set_value(GTK_SCALE_BUTTON(volume_button), 0);
}

//!resets the label time
static void reset_label_time(gui_state *gui)
{
  if (strcmp(gtk_label_get_text(GTK_LABEL(gui->label_time)), "") == 0)
  {
    gtk_label_set_text(GTK_LABEL(gui->label_time), "");
  }
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
void clear_data_player()
{
  gtk_widget_set_sensitive(ui->gui->browse_button, TRUE);

  reset_song_name_label(ui->gui);
  reset_song_infos(ui->gui);
  reset_inactive_volume_button();
  reset_inactive_progress_bar();
  reset_label_time(ui->gui);
}

//!enables the buttons of the player
void enable_player_buttons()
{
  gtk_widget_set_sensitive(stop_button, TRUE);
  gtk_button_set_image(GTK_BUTTON(stop_button), g_object_ref(StopButton_active));
  
  gtk_widget_set_sensitive(pause_button, TRUE);
  gtk_button_set_image(GTK_BUTTON(pause_button), g_object_ref(PauseButton_active));
 
 if (selected_player != PLAYER_GSTREAMER)
  {
    gtk_widget_set_sensitive(go_beg_button, TRUE);
    gtk_button_set_image(GTK_BUTTON(go_beg_button), g_object_ref(Go_BegButton_active));
    gtk_widget_set_sensitive(go_end_button, TRUE);
    gtk_button_set_image(GTK_BUTTON(go_end_button), g_object_ref(Go_EndButton_active));
  }
  gtk_widget_set_sensitive(play_button, TRUE);
  gtk_button_set_image(GTK_BUTTON(play_button), g_object_ref(PlayButton_active));

  player_key_actions_set_sensitivity(TRUE, ui->gui);
}

//!disables the buttons of the player
void disable_player_buttons()
{
  gtk_widget_set_sensitive(stop_button, FALSE);
  gtk_button_set_image(GTK_BUTTON(stop_button), g_object_ref(StopButton_inactive));
  gtk_widget_set_sensitive(pause_button, FALSE);
  gtk_button_set_image(GTK_BUTTON(pause_button), g_object_ref(PauseButton_inactive));
  gtk_widget_set_sensitive(go_beg_button, FALSE);
  gtk_button_set_image(GTK_BUTTON(go_beg_button), g_object_ref(Go_BegButton_inactive));
  gtk_widget_set_sensitive(go_end_button, FALSE);
  gtk_button_set_image(GTK_BUTTON(go_end_button), g_object_ref(Go_EndButton_inactive));
  gtk_widget_set_sensitive(play_button, FALSE);
  gtk_button_set_image(GTK_BUTTON(play_button), g_object_ref(PlayButton_inactive));
  gtk_widget_set_sensitive(player_add_button, FALSE);
  gtk_widget_set_sensitive(silence_wave_check_button, FALSE);

  player_key_actions_set_sensitivity(FALSE, ui->gui);
}

//! Switches between connect and disconnect button when connecting to player
void connect_change_buttons()
{
  if (selected_player != PLAYER_GSTREAMER)
  {
    show_disconnect_button();
    hide_connect_button();
  }
}

//!disconnecting changing buttons
void disconnect_change_buttons()
{
  if (selected_player != PLAYER_GSTREAMER)
  {
    hide_disconnect_button();
    show_connect_button();
  }
}

/*!connect with the song fname

\param fname the file name of the song 
\param start_playing 
-  start playing
- else dont start playing right now.
*/
void connect_with_song(const gchar *fname, gint start_playing)
{
  //list with songs
  GList *song_list = NULL;

  if (fname != NULL)
  {
    song_list = g_list_append(song_list, strdup(fname));

    //if we must also play the song
    if (start_playing == 0)
    {
      //if the player is not running, start it ,queue to playlist and
      //play the file
      if (!player_is_running())
      {
        player_start_play_with_songs(song_list);
      }
      else
      {
        player_add_play_files(song_list);
      }
    }
    else
    {
      if (file_browsed)
      {
        //if the player is not running, start it ,queue to playlist and
        //play the file
        if (!player_is_running())
          player_start_add_files(song_list);
        else
          if(!playing)
            player_add_files_and_select(song_list);
          else
            player_add_files(song_list);
      }
    }
    playing = TRUE;

    if (!timer_active)
    {
      timeout_id = g_timeout_add(timeout_value, mytimer, NULL);
      timer_active = TRUE;
    }

    enable_player_buttons();
    if (player_is_running())
    {
      connect_change_buttons();
    }
  }

  //TODO: free elements of list
  g_list_free(song_list);
}

/*! connects to player with the song from the filename entry

  \param i 0 means then start playing, != 0 means dont start playing
  right now
 */
void connect_to_player_with_song(gint i)
{
  connect_with_song(get_input_filename(ui->gui), i);
}

//!play button event
void connect_button_event(GtkWidget *widget, gpointer data)
{
  if (!player_is_running())
  {
    player_start();
  }

  mytimer(NULL);

  if (!timer_active)
  {
    if (selected_player == PLAYER_SNACKAMP)
    {
      connect_snackamp(8775);
    }

    timeout_id = g_timeout_add(timeout_value, mytimer, NULL);
    timer_active = TRUE;
  }

  //connect to player with song
  //1 means dont start playing
  connect_to_player_with_song(1);

  if (selected_player != PLAYER_GSTREAMER)
  {
    gtk_widget_set_sensitive(ui->gui->browse_button, FALSE);
  }
  enable_player_buttons();

  file_browsed = FALSE;

  change_volume = TRUE;

  //here we check if we have been connected
  if (!player_is_running())
  {
    //if not, we put a message
    GtkWidget *dialog, *label;
    dialog = gtk_dialog_new_with_buttons(_("Cannot connect to player"),
        GTK_WINDOW(ui->gui->window),
        GTK_DIALOG_MODAL,
        GTK_STOCK_OK,
        GTK_RESPONSE_NONE,
        NULL);

    switch(selected_player)
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

    g_signal_connect_swapped(dialog, "response", 
        G_CALLBACK(gtk_widget_destroy), dialog);
    gtk_container_add(GTK_CONTAINER(
          gtk_dialog_get_content_area(GTK_DIALOG(dialog))), label);
    gtk_widget_show_all(dialog);
  }
  else
  {
    connect_change_buttons();
  }

  ui->infos->current_time = -1;
  check_update_down_progress_bar(ui);
}

//!checks if we have a stream
void check_stream()
{
  if (ui->infos->total_time == -1)
  {
    stream = TRUE;
    reset_inactive_progress_bar();
  }
  else
  {
    stream = FALSE;
  }
}

//!disconnect button event
void disconnect_button_event(GtkWidget *widget, gpointer data)
{
  //if the timer is active, deactivate the function
  if (timer_active)
  {
    //we open socket channel  if dealing with snackamp
    if (selected_player == PLAYER_SNACKAMP)
    {
      disconnect_snackamp();
    }

    g_source_remove(timeout_id);
    timer_active = FALSE;
  }

  clear_data_player();
  gtk_widget_set_sensitive(ui->gui->browse_button, TRUE);
  disconnect_change_buttons();
  disable_player_buttons();

  //update bottom progress bar to 0 and ""
  if (!ui->status->splitting)
  {
    gtk_progress_bar_set_fraction(ui->gui->percent_progress_bar, 0);
    gtk_progress_bar_set_text(ui->gui->percent_progress_bar, "");
  }

  const gchar *fname = get_input_filename(ui->gui);
  if (file_exists(fname))
  {
    file_in_entry = TRUE;
    gtk_widget_set_sensitive(play_button, TRUE);
    gtk_button_set_image(GTK_BUTTON(play_button), g_object_ref(PlayButton_active));
  }

  player_quit();
}

void restart_player_timer()
{
  if (timer_active)
  {
    g_source_remove(timeout_id);
    timeout_id = g_timeout_add(timeout_value, mytimer, NULL);
  }
}

//! play button event
void play_event (GtkWidget *widget, gpointer data)
{
  if (timer_active)
  {
    if (!player_is_running())
    {
      player_start();
    }
    player_play();
    playing = TRUE;
  }
  else
  {
    //connects to player with the song
    //0 means also start playing
    connect_to_player_with_song(0);
    if (selected_player != PLAYER_GSTREAMER)
    {
      gtk_widget_set_sensitive(ui->gui->browse_button, FALSE);
    }
  }

  gtk_widget_set_sensitive(pause_button, TRUE);
  gtk_button_set_image(GTK_BUTTON(pause_button), g_object_ref(PauseButton_active));
  gtk_widget_set_sensitive(stop_button, TRUE);
  gtk_button_set_image(GTK_BUTTON(stop_button), g_object_ref(StopButton_active));
}

//! stop button event
void stop_event(GtkWidget *widget, gpointer data)
{
  if (timer_active)
  {
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pause_button)))
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pause_button), FALSE);
    }

    if (player_is_running())
    {
      playing = FALSE;
    }

    player_stop();

    gtk_widget_set_sensitive(pause_button, FALSE);
    gtk_button_set_image(GTK_BUTTON(pause_button), g_object_ref(PauseButton_inactive));
    gtk_widget_set_sensitive(stop_button, FALSE);
    gtk_button_set_image(GTK_BUTTON(stop_button), g_object_ref(StopButton_inactive));
  }
}

//! pause button event
void pause_event(GtkWidget *widget, gpointer data)
{
  if (!timer_active) { return; }
  if (!player_is_running()) { return; }
  if (only_press_pause) { return; }

  player_pause();
}

//! Event for the "previous" button 
void prev_button_event (GtkWidget *widget, gpointer data)
{
  //only if connected to player
  if (timer_active)
    if (player_is_running())
      player_prev();
}

//! event for the "next" button
void next_button_event (GtkWidget *widget, gpointer data)
{
  //only if connected to player
  if (timer_active)
    if (player_is_running())
      player_next();
}

//!changes the position inside the song
static void change_song_position(ui_state *ui)
{
  gint position = 
    ui->infos->player_seconds2 * 1000 + 
    ui->infos->player_minutes2 * 60000 +
    ui->infos->player_hundr_secs2 * 10;

  player_seek(position);  
}

//!adds a splitpoint from the player
void enable_show_silence_wave(GtkToggleButton *widget, gpointer data)
{
  if (gtk_toggle_button_get_active(widget))
  {
    ui->status->show_silence_wave = TRUE;
    if (ui->infos->number_of_silence_points == 0)
    {
      scan_for_silence_wave();
    }
  }
  else
  {
    ui->status->show_silence_wave = FALSE;
    if (we_scan_for_silence)
    {
      cancel_button_event(NULL, NULL);
    }

    if (ui->infos->silence_points != NULL)
    {
      g_free(ui->infos->silence_points);
      ui->infos->silence_points = NULL;
    }
    ui->infos->number_of_silence_points = 0;

    refresh_drawing_area(ui->gui);
    refresh_preview_drawing_areas(ui);
  }
}

GtkWidget *create_volume_button()
{
  volume_button = gtk_volume_button_new();

  g_signal_connect(G_OBJECT(volume_button), "button-press-event",
                   G_CALLBACK(volume_button_click_event), NULL);
  g_signal_connect(G_OBJECT(volume_button), "button-release-event",
                   G_CALLBACK(volume_button_unclick_event), NULL);
  g_signal_connect(G_OBJECT(volume_button), "enter-notify-event",
                   G_CALLBACK(volume_button_enter_event), NULL);
  g_signal_connect(G_OBJECT(volume_button), "leave-notify-event",
                   G_CALLBACK(volume_button_leave_event), NULL);

  g_signal_connect(GTK_SCALE_BUTTON(volume_button), "value_changed", G_CALLBACK(change_volume_event), NULL);

  gtk_widget_set_sensitive(GTK_WIDGET(volume_button), FALSE);

  return volume_button;
}

//!creates the player buttons hbox
static GtkWidget *create_player_buttons_hbox()
{
  player_buttons_hbox = wh_hbox_new();

  //go at the beginning button
  GString *imagefile = g_string_new("");

  build_path(imagefile, IMAGEDIR, "backward"ICON_EXT);
  Go_BegButton_active= gtk_image_new_from_file(imagefile->str);

  build_path(imagefile, IMAGEDIR, "backward_inactive"ICON_EXT);
  Go_BegButton_inactive= gtk_image_new_from_file(imagefile->str);
  go_beg_button = gtk_button_new();
  gtk_button_set_image(GTK_BUTTON(go_beg_button), g_object_ref(Go_BegButton_inactive));

  //put the new button in the box
  gtk_box_pack_start(GTK_BOX(player_buttons_hbox), go_beg_button, FALSE, FALSE, 0);
  gtk_button_set_relief(GTK_BUTTON(go_beg_button), GTK_RELIEF_NONE);
  g_signal_connect(G_OBJECT(go_beg_button), "clicked",
                   G_CALLBACK(prev_button_event),
                   NULL);
  gtk_widget_set_sensitive(go_beg_button, FALSE);
  gtk_widget_set_tooltip_text(go_beg_button, _("Previous"));

  //play button
  build_path(imagefile, IMAGEDIR, "play"ICON_EXT);
  PlayButton_active= gtk_image_new_from_file(imagefile->str);

  build_path(imagefile, IMAGEDIR, "play_inactive"ICON_EXT);
  PlayButton_inactive= gtk_image_new_from_file(imagefile->str);
  play_button = gtk_button_new();
  gtk_button_set_image(GTK_BUTTON(play_button), g_object_ref(PlayButton_inactive));

  //put the new button in the box
  gtk_box_pack_start(GTK_BOX(player_buttons_hbox), play_button, FALSE, FALSE, 0);
  gtk_button_set_relief(GTK_BUTTON(play_button), GTK_RELIEF_NONE);
  g_signal_connect(G_OBJECT(play_button), "clicked",
                   G_CALLBACK(play_event),
                   NULL);
  gtk_widget_set_sensitive(play_button, FALSE);
  gtk_widget_set_tooltip_text(play_button,_("Play"));

  //pause button
  build_path(imagefile, IMAGEDIR, "pause"ICON_EXT);
  PauseButton_active= gtk_image_new_from_file(imagefile->str);

  build_path(imagefile, IMAGEDIR, "pause_inactive"ICON_EXT);
  PauseButton_inactive= gtk_image_new_from_file(imagefile->str);
  pause_button = gtk_toggle_button_new();
  gtk_button_set_image(GTK_BUTTON(pause_button), g_object_ref(PauseButton_inactive));
  //put the new button in the box
  gtk_box_pack_start(GTK_BOX(player_buttons_hbox), pause_button, FALSE, FALSE, 0);
  gtk_button_set_relief(GTK_BUTTON(pause_button), GTK_RELIEF_NONE);
  g_signal_connect(G_OBJECT(pause_button), "clicked",
                   G_CALLBACK(pause_event), NULL);
  gtk_widget_set_sensitive(pause_button, FALSE);
  gtk_widget_set_tooltip_text(pause_button,_("Pause"));

  //stop button
  build_path(imagefile, IMAGEDIR, "stop"ICON_EXT);
  StopButton_active= gtk_image_new_from_file(imagefile->str);

  build_path(imagefile, IMAGEDIR, "stop_inactive"ICON_EXT);
  StopButton_inactive= gtk_image_new_from_file(imagefile->str);
  stop_button = gtk_button_new();
  gtk_button_set_image(GTK_BUTTON(stop_button), g_object_ref(StopButton_inactive));
  //put the new button in the box
  gtk_box_pack_start(GTK_BOX(player_buttons_hbox), stop_button, FALSE, FALSE, 0);
  gtk_button_set_relief(GTK_BUTTON(stop_button), GTK_RELIEF_NONE);
  g_signal_connect(G_OBJECT(stop_button), "clicked",
                   G_CALLBACK(stop_event),
                   NULL);
  gtk_widget_set_sensitive(stop_button, FALSE);
  gtk_widget_set_tooltip_text(stop_button,_("Stop"));

  //go at the end button
  build_path(imagefile, IMAGEDIR, "forward"ICON_EXT);
  Go_EndButton_active= gtk_image_new_from_file(imagefile->str);

  build_path(imagefile, IMAGEDIR, "forward_inactive"ICON_EXT);
  Go_EndButton_inactive= gtk_image_new_from_file(imagefile->str);
  go_end_button = gtk_button_new();
  gtk_button_set_image(GTK_BUTTON(go_end_button), g_object_ref(Go_EndButton_inactive));
  //put the new button in the box
  gtk_box_pack_start(GTK_BOX(player_buttons_hbox), go_end_button, FALSE, FALSE, 0);
  gtk_button_set_relief(GTK_BUTTON(go_end_button), GTK_RELIEF_NONE);
  g_signal_connect(G_OBJECT(go_end_button), "clicked",
                   G_CALLBACK(next_button_event),
                   NULL);
  gtk_widget_set_sensitive(go_end_button, FALSE);
  gtk_widget_set_tooltip_text(go_end_button,_("Next"));
  g_string_free(imagefile,TRUE);

  GtkWidget *vol_button = (GtkWidget *)create_volume_button();
  gtk_box_pack_start(GTK_BOX(player_buttons_hbox), vol_button, FALSE, FALSE, 5);
 
  //add button
  player_add_button = wh_create_cool_button(GTK_STOCK_ADD, _("_Add"), FALSE);
  //put the new button in the box
  gtk_box_pack_start (GTK_BOX(player_buttons_hbox), player_add_button, FALSE, FALSE, 0);
  gtk_button_set_relief(GTK_BUTTON(player_add_button), GTK_RELIEF_NONE);
  g_signal_connect(G_OBJECT(player_add_button), "clicked",
                   G_CALLBACK(add_splitpoint_from_player), NULL);
  gtk_widget_set_sensitive(player_add_button, FALSE);
  gtk_widget_set_tooltip_text(player_add_button,_("Add splitpoint from player"));
  
  //silence wave check button
  silence_wave_check_button = (GtkWidget *)
    gtk_check_button_new_with_mnemonic(_("Amplitude _wave"));
  gtk_box_pack_end(GTK_BOX(player_buttons_hbox), silence_wave_check_button,
      FALSE, FALSE, 5);
  g_signal_connect(G_OBJECT(silence_wave_check_button), "toggled",
      G_CALLBACK(enable_show_silence_wave), NULL);
  gtk_widget_set_sensitive(silence_wave_check_button, FALSE);
  gtk_widget_set_tooltip_text(silence_wave_check_button,
      _("Shows the amplitude level wave"));

  /* connect player button */
  connect_button = wh_create_cool_button(GTK_STOCK_CONNECT,_("_Connect"), FALSE);
  g_signal_connect(G_OBJECT(connect_button), "clicked",
      G_CALLBACK(connect_button_event), NULL);
  gtk_widget_set_tooltip_text(connect_button,_("Connect to player"));
  
  /* disconnect player button */
  disconnect_button = wh_create_cool_button(GTK_STOCK_DISCONNECT,_("_Disconnect"), FALSE);
  g_signal_connect(G_OBJECT (disconnect_button), "clicked",
      G_CALLBACK(disconnect_button_event), NULL);
  gtk_widget_set_tooltip_text(disconnect_button,_("Disconnect from player"));

  return player_buttons_hbox;
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
  if (ui->status->splitting || 
      ui->status->currently_compute_douglas_peucker_filters)
  {
    return;
  }

  gfloat total_interval = 0;
  gfloat progress_time = 0;
  gint splitpoint_time_left = -1;
  gint splitpoint_time_right = -1;
  gint splitpoint_left_index = -1;
  get_current_splitpoints_time_left_right(&splitpoint_time_left, &splitpoint_time_right, 
      &splitpoint_left_index, ui->infos);

  if ((splitpoint_time_left != -1) && (splitpoint_time_right != -1))
  {
    gfloat total_interval = splitpoint_time_right - splitpoint_time_left;
    if (total_interval != 0)
    {
      progress_time = (ui->infos->current_time-splitpoint_time_left) / total_interval;
    }
  }
  else
  {
    if (splitpoint_time_right == -1)
    {
      gfloat total_interval = ui->infos->total_time - splitpoint_time_left;
      if (total_interval != 0)
      {
        progress_time = (ui->infos->current_time-splitpoint_time_left)/ total_interval;
      }
    }
    else
    {
      gfloat total_interval = splitpoint_time_right;
      if (total_interval != 0)
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
      if (fname != NULL && strlen(fname) > 60)
      {
        description_shorted[strlen(description_shorted)-1] = '.';
        description_shorted[strlen(description_shorted)-2] = '.';
        description_shorted[strlen(description_shorted)-3] = '.';
      }
    }
  }

  if (progress_description != NULL && strlen(progress_description) > 60)
  {
    description_shorted[strlen(description_shorted)-1] = '.';
    description_shorted[strlen(description_shorted)-2] = '.';
    description_shorted[strlen(description_shorted)-3] = '.';
  }

  gtk_progress_bar_set_text(ui->gui->percent_progress_bar, description_shorted);
  g_free(progress_description);
}

//!event when the progress bar value changed
static void progress_bar_value_changed_event(GtkRange *range, ui_state *ui)
{
  if (ui->status->currently_compute_douglas_peucker_filters)
  {
    return;
  }

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
    GTK_ADJUSTMENT(gtk_adjustment_new(0.0, 0.0, 100001.0, 0, 10000, 1000));
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
void print_about_the_song()
{
  gchar total_infos[512];
  player_get_song_infos(total_infos);
  
  gtk_label_set_text(GTK_LABEL(ui->gui->song_infos), total_infos);
}

//!prints the player filename
void print_player_filename()
{
  gchar *fname = player_get_filename();
  if (fname != NULL)
  {
    if (strcmp(fname, "disconnect"))
    {
      change_current_filename(fname);
    }
    g_free(fname);
  }

  gchar *title = player_get_title();
  if (title != NULL)
  {
    gtk_label_set_text(GTK_LABEL(ui->gui->song_name_label), title);
    g_free(title);
  }
}

/*! get time elapsed from the song and print it on the screen

Also prints filename, frequency, bitrate, mono, stereo
*/
void print_all_song_infos()
{
  print_about_the_song();
  print_player_filename();
}

/*! prints the song time elapsed

\param i 
 - 0 means normal state
 - 1 means we reset the time
*/
void print_song_time_elapsed()
{
  gint time, temp;
  gchar seconds[16], minutes[16], seconds_minutes[64];

  time = player_get_elapsed_time(ui);

  ui->infos->player_hundr_secs = (time % 1000) / 10;

  temp = (time/1000)/60;
  ui->infos->player_minutes = temp;
  ui->infos->player_seconds = (time/1000) - (temp*60); 

  g_snprintf(minutes, 16, "%d", temp);
  g_snprintf(seconds, 16, "%d", (time/1000) - (temp*60));

  gchar total_seconds[16], total_minutes[16];

  gint tt = ui->infos->total_time * 10;
  temp = (tt/1000)/60;

  //calculate time and print time
  g_snprintf(total_minutes, 16, "%d", temp);
  g_snprintf(total_seconds, 16, "%d", (tt/1000) - (temp*60));
  g_snprintf(seconds_minutes, 64, "%s  :  %s  /  %s  :  %s", 
      minutes, seconds, total_minutes, total_seconds);

  gtk_label_set_text(GTK_LABEL(ui->gui->label_time), seconds_minutes);
}

//!change volume to match the players volume
void change_volume_button()
{
  if (!player_is_running())
  {
    return;
  }

  gint volume = player_get_volume();
  if (volume < 0)
  {
    return;
  }

  gtk_scale_button_set_value(GTK_SCALE_BUTTON(volume_button), volume / 100.0);
}

//!progress bar synchronisation with player
void change_progress_bar()
{
  if (!player_is_running() || ui->status->mouse_on_progress_bar)
  {
    return;
  }

  ui->infos->total_time = player_get_total_time() / 10;

  ui->infos->current_time = ui->infos->player_seconds * 100 + 
    ui->infos->player_minutes * 6000 +
    ui->infos->player_hundr_secs;

  gdouble adj_position = (ui->infos->current_time *100000) / ui->infos->total_time;
  gtk_adjustment_set_value(ui->gui->progress_adj, adj_position);

  ui->infos->current_time = get_elapsed_time(ui);

  gint stop_splitpoint = get_splitpoint_time(quick_preview_end_splitpoint);
  gint start_splitpoint = get_splitpoint_time(preview_start_splitpoint);
  if ((stop_splitpoint < (gint)(ui->infos->current_time-150)) ||
      (start_splitpoint > (gint)(ui->infos->current_time+150)))
  {
    cancel_quick_preview();
  }
}

//!creates the filename player hbox
static GtkWidget *create_filename_player_hbox(gui_state *gui)
{
  GtkWidget *song_name_label = gtk_label_new("");
  gui->song_name_label = song_name_label;

  gtk_label_set_ellipsize(GTK_LABEL(song_name_label), PANGO_ELLIPSIZE_END);
  g_object_set(G_OBJECT(song_name_label), "selectable", FALSE, NULL);

  GtkWidget *filename_player_hbox = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(filename_player_hbox), song_name_label, FALSE, FALSE, 15);

  return filename_player_hbox;
}

//!changes the volume of the player
void change_volume_event(GtkScaleButton *volume_button, gdouble value, gpointer data)
{
  if (!gtk_widget_get_sensitive(GTK_WIDGET(volume_button)))
  {
    return;
  }

  player_set_volume((gint)(value * 100));
}

//!when we unclick the volume bar
gboolean volume_button_unclick_event(GtkWidget *widget, GdkEventCrossing *event, gpointer user_data)
{
  change_volume = TRUE;
  return FALSE;
}

//!when we click the volume bar
gboolean volume_button_click_event(GtkWidget *widget, GdkEventCrossing *event, gpointer user_data)
{
  change_volume = FALSE;
  return FALSE;
}

//!when we enter the volume bar
gboolean volume_button_enter_event(GtkWidget *widget, GdkEventCrossing *event, gpointer user_data)
{
  on_the_volume_button = TRUE;
  return FALSE;
}

//!when we leave the volume bar
gboolean volume_button_leave_event(GtkWidget *widget, GdkEventCrossing *event, gpointer user_data)
{
  on_the_volume_button = FALSE;
  return FALSE;
}

//!when closing the new window after detaching
void close_player_popup_window_event( GtkWidget *window,
                                      gpointer data )
{
  GtkWidget *window_child;

  window_child = gtk_bin_get_child(GTK_BIN(window));

  gtk_widget_reparent(GTK_WIDGET(window_child), GTK_WIDGET(player_handle));

  gtk_widget_destroy(window);
}

//!when we detach the handle
void handle_player_detached_event(GtkHandleBox *handlebox,
                                  GtkWidget *widget,
                                  gpointer data)
{
  //new window
  GtkWidget *window;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  gtk_widget_reparent(GTK_WIDGET(widget), GTK_WIDGET(window));

  g_signal_connect(G_OBJECT(window), "delete_event",
                   G_CALLBACK(close_player_popup_window_event),
                   NULL);
  
  gtk_widget_show(GTK_WIDGET(window));
}

//!returns the value of the right drawing area
gfloat get_right_drawing_time(gfloat current_time, gfloat total_time, gfloat zoom_coeff)
{
  gfloat left = 0;
  gfloat right = total_time / zoom_coeff;
  gfloat center = (right - left) / 2;
  gfloat offset = current_time - center;
  right += offset;
  
  return right;
}

//!returns the value of the left drawing area
gfloat get_left_drawing_time(gfloat current_time, gfloat total_time, gfloat zoom_coeff)
{
  gfloat right = total_time / zoom_coeff;
  gfloat center = right/2;
  gfloat left = current_time - center;
  
  return left;
}

/*!returns the hundreth of seconds rest of a time value

\param time The time in hundreths of a second
*/
gint get_time_hundrsecs(gint time)
{
  return time % 100;
}

/*!returns the seconds rest of a time

\param time The time in hundreths of a second
*/
gint get_time_secs(gint time)
{
  return (time / 100) % 60;
}

/*!returns the minutes of a time 

\param time The time in hundreth of a second
*/
gint get_time_mins(gint time)
{
  return time / 6000;
}

/*!Converts a time value to a string we can display 

\param hundr_or_not = TRUE means we also draw hundr of secs
\param time The time in hundreths of a second
*/
gchar *get_time_for_drawing(gchar *str,
                            gint time,
                            gboolean hundr_or_not,
                            gint *number_of_chars)
{
  gint mins = get_time_mins(time);
  gint secs = get_time_secs(time);
  
  if (hundr_or_not)
    {
      gint hundr = get_time_hundrsecs(time);
      *number_of_chars = 
        g_snprintf(str,30, "%d:%02d:%02d",
                   mins,secs,hundr);
    }
  else
    {
      *number_of_chars = 
        g_snprintf(str,30, "%d:%02d",
                   mins,secs);
    }
  
  return str;
}

//transform pixels to time
gfloat pixels_to_time(gfloat width, gint pixels)
{
  return (ui->infos->total_time * (gfloat)pixels)/(width * zoom_coeff);
}

static gint time_to_pixels(gint width, gfloat time,
    gfloat total_time, gfloat zoom_coeff)
{
  return (width * time * zoom_coeff) / total_time;
}

/*!returns the position of a time mark on the screen

\param width The width of the drawing 
\param The time in hundreths of a second
*/
gint convert_time_to_pixels(gint width, gfloat time, 
    gfloat current_time, gfloat total_time, gfloat zoom_coeff)
{
  return width/2 + time_to_pixels(width, time - current_time, total_time, zoom_coeff);
}

void draw_motif(GtkWidget *da, cairo_t *gc, gint ylimit, gint x, gint model)
{
  GdkColor color;
  switch(model){
  case 0:
    //hundreths
    color.red = 65000;color.green = 0;color.blue = 0;
    break;
  case 1:
    //tens of seconds
    color.red = 0;color.green = 0;color.blue = 65000;
    break;
  case 2:
    //seconds
    color.red = 0;color.green = 65000;color.blue = 0;
    break;
  case 3:
    //ten seconds
    color.red = 65000;color.green = 0;color.blue = 40000;
    break;
  case 4:
    //minutes
    color.red = 1000;color.green = 10000;color.blue = 65000;
    break;
  case 5:
    //ten minutes
    color.red = 65000;color.green = 0;color.blue = 0;
    break;
  default:
    //hours
    color.red = 0;color.green = 0;color.blue = 0;
    break;
  }

  dh_set_color(gc, &color);

  draw_point(gc,x,ylimit+6);
  draw_point(gc,x,ylimit+7);
  draw_point(gc,x,ylimit+8);
  draw_point(gc,x-1,ylimit+8);
  draw_point(gc,x+1,ylimit+8);
  draw_point(gc,x,ylimit+9);
  draw_point(gc,x-1,ylimit+9);
  draw_point(gc,x+1,ylimit+9);
  draw_point(gc,x-2,ylimit+9);
  draw_point(gc,x+2,ylimit+9);
  draw_point(gc,x-3,ylimit+9);
  draw_point(gc,x+3,ylimit+9);
  draw_point(gc,x,ylimit+10);
  draw_point(gc,x-1,ylimit+10);
  draw_point(gc,x+1,ylimit+10);
  draw_point(gc,x-2,ylimit+10);
  draw_point(gc,x+2,ylimit+10);
  draw_point(gc,x-3,ylimit+10);
  draw_point(gc,x+3,ylimit+10);

  cairo_stroke(gc);

  color.red = 0;color.green = 0;color.blue = 0;
  dh_set_color(gc, &color);
}

//!draw the marks, minutes, seconds...
void draw_marks(gint time_interval, gint left_mark,
                gint right_mark, gint ylimit,
                GtkWidget *da, cairo_t *gc)
{
  gint left2 = (left_mark/time_interval) * time_interval;
  if (left2 < left_mark)
    left2 += time_interval;

  gint i;
  gint i_pixel;
  for (i=left2;i<=right_mark;i+=time_interval)
  {
    i_pixel = convert_time_to_pixels(ui->infos->width_drawing_area, i,
        ui->infos->current_time, ui->infos->total_time, zoom_coeff);

    switch(time_interval){
      case 1:
        draw_motif(da, gc, ylimit, i_pixel,0);
        break;
      case 10:
        draw_motif(da, gc, ylimit,i_pixel,1);
        break;
      case 100:
        draw_motif(da, gc, ylimit,i_pixel,2);
        break;
      case 1000:
        draw_motif(da, gc, ylimit,i_pixel,3);
        break;
      case 6000:
        draw_motif(da, gc, ylimit,i_pixel,4);
        break;
      case 60000:
        draw_motif(da, gc, ylimit,i_pixel,5);
        break;
      default:
        draw_motif(da, gc, ylimit,i_pixel,6);
        break;
    }
  }
}

//!full cancel of the quick preview
void cancel_quick_preview_all()
{
  cancel_quick_preview();
  quick_preview_end_splitpoint = -1;
  preview_start_splitpoint = -1;
}

//!cancels quick preview
void cancel_quick_preview()
{
  quick_preview = FALSE;
}

/*!motif for splitpoints

\param draw false if we draw the splitpoint we move
\param move FALSE means we don't move the splitpoint,
\param move = TRUE means we move the splitpoint
\param number_splitpoint is the current splitpoint we draw
\param splitpoint_checked = TRUE if the splitpoint is checked
*/
void draw_motif_splitpoints(GtkWidget *da, cairo_t *gc,
                            gint x,gint draw,
                            gint current_point_hundr_secs,
                            gboolean move,
                            gint number_splitpoint)
{
  int m = ui->gui->margin - 1;
  GdkColor color;
  Split_point point = g_array_index(ui->splitpoints, Split_point, number_splitpoint);
  gboolean splitpoint_checked = point.checked;
  
  //top color
  color.red = 255 * 212;
  color.green = 255 * 100;
  color.blue = 255 * 200;
  dh_set_color(gc, &color);
  
  //if it' the splitpoint we move, don't fill in the circle and
  //the square
  if (!draw)
  {
    dh_draw_rectangle(gc, FALSE, x-6,4, 11,11);
  }
  else
  {
    dh_draw_rectangle(gc, TRUE, x-6,4, 12,12);

    if (number_splitpoint == get_first_splitpoint_selected())
    {
      //top color
      color.red = 255 * 220;
      color.green = 255 * 220;
      color.blue = 255 * 255;
      dh_set_color(gc, &color);

      dh_draw_rectangle(gc, TRUE, x-4,6, 8,8);
    }
  }

  //default color
  color.red = 255 * 212;
  color.green = 255 * 196;
  color.blue = 255 * 221;
  dh_set_color(gc, &color);
  
  gint i;
  for(i = 0;i<5;i++)
  {
    draw_point (gc,x+i,ui->gui->erase_split_ylimit + m + 3);
    draw_point (gc,x-i,ui->gui->erase_split_ylimit + m + 3);
    draw_point (gc,x+i,ui->gui->erase_split_ylimit + m + 4);
    draw_point (gc,x-i,ui->gui->erase_split_ylimit + m + 4);
  }
  cairo_stroke(gc);
  
  //if we are currently moving this splitpoint
  if (move)
  {
    //we set the green or blue color
    if (splitpoint_checked)
    {
      color.red = 15000;color.green = 40000;color.blue = 25000;
    }
    else
    {
      color.red = 25000;color.green = 25000;color.blue = 40000;
    }
    dh_set_color(gc, &color);

    dh_draw_line(gc, x,ui->gui->erase_split_ylimit + m -8, x,ui->gui->progress_ylimit + m, TRUE, TRUE);
  }
  
  color.red = 255 * 22;
  color.green = 255 * 35;
  color.blue = 255 * 91;
  dh_set_color(gc, &color);
  
  //draw the splitpoint motif
  for (i = -3;i <= 1;i++)
  {
    draw_point (gc,x,ui->gui->erase_split_ylimit + m +i);
  }
  for (i = 2;i <= 5;i++)
  {
    draw_point (gc,x,ui->gui->erase_split_ylimit + m + i);
  }
  for (i = 3;i <= 4;i++)
  {
    draw_point (gc,x-1,ui->gui->erase_split_ylimit + m + i);
    draw_point (gc,x+1,ui->gui->erase_split_ylimit + m + i);
  }
  for (i = 6;i <= 11;i++)
  {
    draw_point (gc,x,ui->gui->erase_split_ylimit + m + i);
  }
  
  //bottom splitpoint vertical bar
  for (i = 0;i < ui->gui->margin;i++)
  {
    draw_point (gc,x,ui->gui->progress_ylimit + m - i);
  }

  //bottom checkbox vertical bar
  for (i = 0;i < ui->gui->margin;i++)
  {
    draw_point (gc,x,ui->gui->splitpoint_ypos + m - i - 1);
  }
  cairo_stroke(gc);

  //bottom rectangle
  dh_set_color(gc, &color);
  color.red = 25000;color.green = 25000;color.blue = 25000;
  dh_draw_rectangle(gc, FALSE, x-6, ui->gui->splitpoint_ypos + m, 12,12);

  //draw a cross with 2 lines if the splitpoint is checked
  if (splitpoint_checked)
  {
    //
    gint left = x - 6;
    gint right = x + 6;
    //
    gint top = ui->gui->splitpoint_ypos + m;
    gint bottom = ui->gui->splitpoint_ypos + m + 12;
    dh_draw_line(gc, left, top, right, bottom, FALSE, TRUE);
    dh_draw_line(gc, left, bottom, right, top, FALSE, TRUE);
  }
  
  //we set the color
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
    //we set the black color
    color.red = 0;color.green = 0;color.blue = 0;
    dh_set_color(gc, &color);

    gboolean dashed = FALSE;
    if (move) { dashed = TRUE; }
    dh_draw_line(gc, x,ui->gui->text_ypos + ui->gui->margin, x,ui->gui->wave_ypos, dashed, TRUE);
  }
}

//!left, right mark in hundreths of seconds
void draw_splitpoints(gint left_mark, gint right_mark, GtkWidget *da, cairo_t *gc)
{
  Split_point current_point;

  gint i = 0;
  for(i = 0; i < ui->infos->splitnumber; i++ )
  {
    gint current_point_hundr_secs = get_splitpoint_time(i);

    //if the splitpoint is > left and < right
    //it must be visible !
    if ((current_point_hundr_secs <= right_mark)
        &&(current_point_hundr_secs >= left_mark))
    {
      //if it's the splitpoint we move, we draw it differently
      gboolean draw = TRUE;
      if (splitpoint_to_move == i)
      {
        draw = FALSE;
      }

      gint split_pixel = 
        convert_time_to_pixels(ui->infos->width_drawing_area, current_point_hundr_secs, 
            ui->infos->current_time, ui->infos->total_time, zoom_coeff);
      draw_motif_splitpoints(da, gc, split_pixel, draw,
          current_point_hundr_secs,
          FALSE, i);
    }
  }
}

gint get_silence_filtered_presence_index(gfloat draw_time)
{
  //num_of_points_coeff_f : ogg ~= 1, mp3 ~= 4
  gfloat num_of_points_coeff_f =
    ceil((ui->infos->number_of_silence_points / ui->infos->total_time) * 10);
  gint num_of_points_coeff = (gint)num_of_points_coeff_f;

  if (draw_time > fourty_minutes_time)
  {
    if (num_of_points_coeff < 3)
    {
      return 2;
    }
    return 4;
  }

  if (draw_time > twenty_minutes_time)
  {
    if (num_of_points_coeff < 3)
    {
      return 1;
    }
    return 3;
  }

  if (draw_time > ten_minutes_time)
  {
    if (num_of_points_coeff < 3)
    {
      return 0;
    }
    return 2;
  }

  if (draw_time > six_minutes_time)
  {
    if (num_of_points_coeff < 3)
    {
      return -1;
    }
    return 1;
  }

  if (draw_time > three_minutes_time)
  {
    if (num_of_points_coeff < 3)
    {
      return -1;
    }
    return 0;
  }

  return -1;
}

gint point_is_filtered(gint index, gint filtered_index)
{
  GArray *points_presence = g_ptr_array_index(filtered_points_presence, filtered_index);
  return !g_array_index(points_presence, gint, index);
}

//! Draws the silence wave
gint draw_silence_wave(gint left_mark, gint right_mark, 
    gint interpolation_text_x, gint interpolation_text_y,
    gfloat draw_time, gint width_drawing_area, gint y_margin,
    gfloat current_time, gfloat total_time, gfloat zoom_coeff, 
    GtkWidget *da, cairo_t *gc)
{
  GdkColor color;

  if (!ui->infos->silence_points || we_scan_for_silence || 
      ui->status->currently_compute_douglas_peucker_filters)
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

  gint first_time = SPLT_TRUE;

  gint filtered_index = get_silence_filtered_presence_index(draw_time);
  gint interpolation_level = 
    adjust_filtered_index_according_to_number_of_points(filtered_index, left_mark, right_mark);

  gint stroke_counter = 0;

  gint i = 0;
  for (i = 0;i < ui->infos->number_of_silence_points;i++)
  {
    if (interpolation_level >= 0 && point_is_filtered(i, interpolation_level))
    {
      continue;
    }
    long time = ui->infos->silence_points[i].time;

    if ((time > right_mark) || (time < left_mark)) 
    {
      continue;
    }

    float level = ui->infos->silence_points[i].level;

    gint x = convert_time_to_pixels(width_drawing_area, 
        (gfloat)time, current_time, total_time, zoom_coeff);
    gint y = y_margin + (gint)floorf(level);

    if (first_time)
    {
      cairo_move_to(gc, x, y);
      first_time = SPLT_FALSE;
    }
    else
    {
      cairo_line_to(gc, x, y);
    }

    stroke_counter++;
    if (stroke_counter % 4 == 0)
    {
      cairo_stroke(gc);
      cairo_move_to(gc, x, y);
    }
  }

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

gint adjust_filtered_index_according_to_number_of_points(gint filtered_index, 
    gint left_mark, gint right_mark)
{
  if (filtered_index == 4)
  {
    return filtered_index;
  }

  gint number_of_points = 0;
  gint number_of_filtered_points = 0;

  gint i = 0;
  for (i = 0;i < ui->infos->number_of_silence_points;i++)
  {
    long time = ui->infos->silence_points[i].time;
    if ((time > right_mark) || (time < left_mark)) 
    {
      continue;
    }

    if (filtered_index >= 0 && point_is_filtered(i, filtered_index))
    {
      number_of_filtered_points++;
    }

    number_of_points++;
  }

  if (number_of_points <= silence_wave_number_of_points_threshold)
  {
    return -1;
  }

  if (number_of_points - number_of_filtered_points > silence_wave_number_of_points_threshold)
  {
    return filtered_index + 1;
  }

  return filtered_index;
}

static void draw_rectangles_between_splitpoints(cairo_t *cairo_surface)
{
  GdkColor color;

  //yellow small rectangle
  gint point_time_left = -1;
  gint point_time_right = -1;
  get_current_splitpoints_time_left_right(&point_time_left, &point_time_right, NULL, ui->infos);
  color.red = 255 * 255;color.green = 255 * 255;color.blue = 255 * 210;
  draw_small_rectangle(point_time_left, point_time_right, color, cairo_surface);

  gint gray_factor = 210;
  color.red = 255 * gray_factor;color.green = 255 * gray_factor;color.blue = 255 * gray_factor;

  //gray areas
  if (ui->infos->splitnumber == 0)
  {
    draw_small_rectangle(0, ui->infos->total_time, color, cairo_surface);
    return;
  }

  draw_small_rectangle(0, get_splitpoint_time(0), color, cairo_surface);
  draw_small_rectangle(get_splitpoint_time(ui->infos->splitnumber-1), ui->infos->total_time, color, cairo_surface);
  gint i = 0;
  for (i = 0; i < ui->infos->splitnumber - 1; i++ )
  {
    Split_point point = g_array_index(ui->splitpoints, Split_point, i);
    if (!point.checked)
    {
      gint left_time = get_splitpoint_time(i);
      gint right_time = get_splitpoint_time(i+1);
      draw_small_rectangle(left_time, right_time, color, cairo_surface);
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
  if (ui->gui->drawing_area_expander != NULL &&
      !gtk_expander_get_expanded(GTK_EXPANDER(ui->gui->drawing_area_expander)))
  {
    return;
  }

  gint old_width_drawing_area = ui->infos->width_drawing_area;

  int width = 0, height = 0;
  wh_get_widget_size(da, &width, &height);
  if (ui->status->show_silence_wave)
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

  //
  ui->gui->margin = 4;

  //
  ui->gui->real_erase_split_length = 12;
  gint real_progress_length = 26;
  ui->gui->real_move_split_length = 16;
  ui->gui->real_checkbox_length = 12;
  gint real_text_length = 12;
  ui->gui->real_wave_length = 96;

  gint erase_splitpoint_length = ui->gui->real_erase_split_length + (ui->gui->margin * 2);
  gint progress_length = real_progress_length + ui->gui->margin;
  gint move_split_length = ui->gui->real_move_split_length + ui->gui->margin;
  gint text_length = real_text_length + ui->gui->margin;
  gint checkbox_length = ui->gui->real_checkbox_length + ui->gui->margin;
  gint wave_length = ui->gui->real_wave_length + ui->gui->margin;

  //
  ui->gui->erase_split_ylimit = erase_splitpoint_length;
  ui->gui->progress_ylimit = ui->gui->erase_split_ylimit + progress_length;
  ui->gui->splitpoint_ypos = ui->gui->progress_ylimit + move_split_length;
  ui->gui->checkbox_ypos = ui->gui->splitpoint_ypos + checkbox_length;
  ui->gui->text_ypos = ui->gui->checkbox_ypos + text_length + ui->gui->margin;
  ui->gui->wave_ypos = ui->gui->text_ypos + wave_length + ui->gui->margin;

  gint bottom_left_middle_right_text_ypos = ui->gui->text_ypos;
  if (ui->status->show_silence_wave)
  {
    bottom_left_middle_right_text_ypos = ui->gui->wave_ypos;
  }

  gint nbr_chars = 0;

  wh_get_widget_size(da, &ui->infos->width_drawing_area, NULL);

  if (ui->infos->width_drawing_area != old_width_drawing_area)
  {
    refresh_preview_drawing_areas(ui);
  }

  GdkColor color;
  color.red = 255 * 235;color.green = 255 * 235;
  color.blue = 255 * 235;
  dh_set_color(gc, &color);

  //background rectangle
  dh_draw_rectangle(gc, TRUE, 0,0, ui->infos->width_drawing_area, ui->gui->wave_ypos + text_length + 2);

  color.red = 255 * 255;color.green = 255 * 255;color.blue = 255 * 255;
  dh_set_color(gc, &color);

  //background white rectangles
  dh_draw_rectangle(gc, TRUE, 0, ui->gui->margin, ui->infos->width_drawing_area, ui->gui->real_erase_split_length);
  dh_draw_rectangle(gc, TRUE, 0, ui->gui->erase_split_ylimit, ui->infos->width_drawing_area, progress_length);
  dh_draw_rectangle(gc, TRUE, 0, ui->gui->progress_ylimit+ui->gui->margin, ui->infos->width_drawing_area, ui->gui->real_move_split_length);
  dh_draw_rectangle(gc, TRUE, 0, ui->gui->splitpoint_ypos+ui->gui->margin, ui->infos->width_drawing_area, ui->gui->real_checkbox_length);
  dh_draw_rectangle(gc, TRUE, 0, ui->gui->checkbox_ypos+ui->gui->margin, ui->infos->width_drawing_area, text_length);
  if (ui->status->show_silence_wave)
  {
    dh_draw_rectangle(gc, TRUE, 0, ui->gui->text_ypos + ui->gui->margin, ui->infos->width_drawing_area, wave_length);
  }

  //only if we are playing
  //and the timer active(connected to player)
  if (playing && timer_active)
  {
    gfloat left_time = get_left_drawing_time(ui->infos->current_time, ui->infos->total_time, zoom_coeff);
    gfloat right_time = get_right_drawing_time(ui->infos->current_time, ui->infos->total_time, zoom_coeff);
    gfloat center_time = ui->infos->current_time;

    //marks to draw seconds, minutes...
    gint left_mark = (gint)left_time;
    gint right_mark = (gint)right_time;
    if (left_mark < 0)
    {
      left_mark = 0;
    }
    if (right_mark > ui->infos->total_time)
    {
      right_mark = (gint)ui->infos->total_time;
    }

    gfloat total_draw_time = right_time - left_time;

    gchar str[30] = { '\0' };
    gint beg_pixel = convert_time_to_pixels(ui->infos->width_drawing_area, 0,
        ui->infos->current_time, ui->infos->total_time, zoom_coeff);

    draw_rectangles_between_splitpoints(gc);

    //blue color
    color.red = 255 * 150; color.green = 255 * 150; color.blue = 255 * 255;
    dh_set_color(gc, &color);

    //if it's the first splitpoint from play preview
    if (quick_preview_end_splitpoint != -1)
    {
      gint right_pixel =
        convert_time_to_pixels(ui->infos->width_drawing_area,
            get_splitpoint_time(quick_preview_end_splitpoint),
            ui->infos->current_time, ui->infos->total_time, zoom_coeff);
      gint left_pixel =
        convert_time_to_pixels(ui->infos->width_drawing_area,
            get_splitpoint_time(preview_start_splitpoint),
            ui->infos->current_time, ui->infos->total_time, zoom_coeff);

      gint preview_splitpoint_length = right_pixel - left_pixel + 1;

      //top buttons
      dh_draw_rectangle(gc,
          TRUE, left_pixel,
          ui->gui->progress_ylimit-2,
          preview_splitpoint_length,3);

      //if we have a quick preview on going, put red bar
      if (quick_preview)
      {
        color.red = 255 * 255;color.green = 255 * 160;color.blue = 255 * 160;
        dh_set_color(gc, &color);
        //top buttons
        dh_draw_rectangle (gc,
            TRUE, left_pixel,
            ui->gui->erase_split_ylimit,
            preview_splitpoint_length,
            3);
      }
    }
    else
    {
      //if we draw until the end
      if ((preview_start_splitpoint != -1)&&
          (preview_start_splitpoint != (ui->infos->splitnumber-1)))
      {
        gint left_pixel =
          convert_time_to_pixels(ui->infos->width_drawing_area,
              get_splitpoint_time(preview_start_splitpoint),
              ui->infos->current_time, ui->infos->total_time, zoom_coeff);
        dh_draw_rectangle(gc,
            TRUE, left_pixel,
            ui->gui->progress_ylimit-2,
            ui->infos->width_drawing_area-left_pixel,
            3);
        //if we have a quick preview on going, put red bar
        if (quick_preview)
        {
          color.red = 255 * 255;color.green = 255 * 160;color.blue = 255 * 160;
          dh_set_color(gc, &color);
          dh_draw_rectangle(gc,
              TRUE, left_pixel,
              ui->gui->erase_split_ylimit,
              ui->infos->width_drawing_area-left_pixel,
              3);
        }
      }
    }

    //song start
    if (left_time <= 0)
    {
      color.red = 255 * 235;color.green = 255 * 235;
      color.blue = 255 * 235;
      dh_set_color(gc, &color);
      dh_draw_rectangle(gc,
          TRUE,
          0,0,
          beg_pixel,
          ui->gui->wave_ypos);
    }
    else
    {
      color.red = 30000;color.green = 0;color.blue = 30000;
      dh_set_color(gc, &color);

      get_time_for_drawing(str, left_time, FALSE, &nbr_chars);
      dh_draw_text(gc, str, 15, bottom_left_middle_right_text_ypos);
    }

    gint end_pixel = 
      convert_time_to_pixels(ui->infos->width_drawing_area, ui->infos->total_time, 
          ui->infos->current_time, ui->infos->total_time, zoom_coeff);
    //song end
    if (right_time >= ui->infos->total_time)
    {
      color.red = 255 * 235;color.green = 255 * 235;
      color.blue = 255 * 235;
      dh_set_color(gc, &color);

      dh_draw_rectangle (gc,
          TRUE, end_pixel,0,
          ui->infos->width_drawing_area,
          bottom_left_middle_right_text_ypos);
    }
    else
    {
      color.red = 30000;color.green = 0;color.blue = 30000;
      dh_set_color(gc, &color);

      get_time_for_drawing(str, right_time, FALSE, &nbr_chars);
      dh_draw_text(gc, str, ui->infos->width_drawing_area - 52, bottom_left_middle_right_text_ypos);
    }

    if (total_draw_time < hundr_secs_th)
    {
      //DRAW HUNDR OF SECONDS
      draw_marks(1, left_mark, right_mark,
          ui->gui->erase_split_ylimit+ progress_length/4,
          da, gc);
    }

    if (total_draw_time < tens_of_secs_th)
    {
      //DRAW TENS OF SECONDS
      draw_marks(10, left_mark, right_mark,
          ui->gui->erase_split_ylimit+ progress_length/4,
          da, gc);
    }

    if (total_draw_time < secs_th)
    {
      //DRAW SECONDS
      draw_marks(100, left_mark, right_mark,
          ui->gui->erase_split_ylimit+ progress_length/4,
          da, gc);
    }

    if (total_draw_time < ten_secs_th)
    {
      //DRAW TEN SECONDS
      draw_marks(1000,
          left_mark, right_mark,
          ui->gui->erase_split_ylimit+ progress_length/4,
          da, gc);
    }

    if (total_draw_time < minutes_th)
    {
      //DRAW MINUTES
      draw_marks(6000,
          left_mark, right_mark,
          ui->gui->erase_split_ylimit+ progress_length/4,
          da, gc);
    }

    if (total_draw_time < ten_minutes_th)
    {
      //DRAW TEN MINUTES
      draw_marks(60000,
          left_mark, right_mark,
          ui->gui->erase_split_ylimit+ progress_length/4,
          da, gc);
    }

    //DRAW HOURS
    draw_marks(100 * 3600,
        left_mark, right_mark,
        ui->gui->erase_split_ylimit+progress_length/4,
        da, gc);

    //draw mobile button1 position line
    if (button1_pressed)
    {
      gint move_pixel = convert_time_to_pixels(ui->infos->width_drawing_area, move_time, 
          ui->infos->current_time, ui->infos->total_time, zoom_coeff);

      if (move_splitpoints)
      {
        draw_motif_splitpoints(da, gc, move_pixel,TRUE, move_time,
            TRUE, splitpoint_to_move);

        color.red = 0;color.green = 0;color.blue = 0;
        dh_set_color(gc, &color);

        get_time_for_drawing(str, ui->infos->current_time, FALSE, &nbr_chars);
        dh_draw_text(gc, str, ui->infos->width_drawing_area/2-11, bottom_left_middle_right_text_ypos);
      }
      else
      {
        color.red = 255 * 255;color.green = 0;color.blue = 0;
        dh_set_color(gc, &color);

        dh_draw_line(gc, move_pixel,ui->gui->erase_split_ylimit, move_pixel,ui->gui->progress_ylimit, TRUE, TRUE);

        if (ui->status->show_silence_wave)
        {
          dh_draw_line(gc, move_pixel,ui->gui->text_ypos + ui->gui->margin, move_pixel,ui->gui->wave_ypos, TRUE, TRUE);
        }

        color.red = 0;color.green = 0;color.blue = 0;
        dh_set_color(gc, &color);

        get_time_for_drawing(str, move_time, FALSE, &nbr_chars);
        dh_draw_text(gc, str, ui->infos->width_drawing_area/2-11, bottom_left_middle_right_text_ypos);
      }
    }
    else
    {
      color.red = 0;color.green = 0;color.blue = 0;
      dh_set_color(gc, &color);

      get_time_for_drawing(str, center_time, FALSE, &nbr_chars);
      dh_draw_text(gc, str, ui->infos->width_drawing_area/2-11, bottom_left_middle_right_text_ypos);
    }

    color.red = 255 * 255;color.green = 0;color.blue = 0;
    dh_set_color(gc, &color);

    //the top middle line, current position
    dh_draw_line(gc, ui->infos->width_drawing_area/2,ui->gui->erase_split_ylimit,
        ui->infos->width_drawing_area/2,ui->gui->progress_ylimit, FALSE, TRUE);

    //silence wave
    if (ui->status->show_silence_wave)
    {
      draw_silence_wave(left_mark, right_mark, 
          ui->infos->width_drawing_area/2 + 3, ui->gui->wave_ypos - ui->gui->margin * 4,
          total_draw_time, 
          ui->infos->width_drawing_area, ui->gui->text_ypos + ui->gui->margin,
          ui->infos->current_time, ui->infos->total_time, zoom_coeff,
          da, gc);

      //silence wave middle line
      color.red = 255 * 255;color.green = 0;color.blue = 0;
      dh_set_color(gc, &color);
      dh_draw_line(gc, ui->infos->width_drawing_area/2,ui->gui->text_ypos + ui->gui->margin, ui->infos->width_drawing_area/2, ui->gui->wave_ypos, FALSE, TRUE);
    }

    draw_splitpoints(left_mark, right_mark, da, gc);
  }
  else
  {
    color.red = 255 * 212; color.green = 255 * 100; color.blue = 255 * 200;
    dh_set_color(gc, &color);
    dh_draw_text(gc, _(" left click on splitpoint selects it, right click erases it"),
        0, ui->gui->margin - 3);

    color.red = 0;color.green = 0;color.blue = 0;
    dh_set_color(gc, &color);
    dh_draw_text(gc, _(" left click + move changes song position, right click + move changes zoom"),
        0, ui->gui->erase_split_ylimit + ui->gui->margin);

    color.red = 15000;color.green = 40000;color.blue = 25000;
    dh_set_color(gc, &color);
    dh_draw_text(gc, 
        _(" left click on point + move changes point position, right click play preview"),
        0, ui->gui->progress_ylimit + ui->gui->margin);

    color.red = 0; color.green = 0; color.blue = 0;
    dh_set_color(gc, &color);
    dh_draw_text(gc, _(" left click on rectangle checks/unchecks 'keep splitpoint'"),
        0, ui->gui->splitpoint_ypos + 1);
  }

#if GTK_MAJOR_VERSION <= 2
  cairo_destroy(gc);
#endif

  return TRUE;
}

static void draw_small_rectangle(gint time_left, gint time_right, 
    GdkColor color, cairo_t *cairo_surface)
{
  if (time_left == -1 || time_right == -1)
  {
    return;
  }

  gint pixels_left = convert_time_to_pixels(ui->infos->width_drawing_area, time_left, 
      ui->infos->current_time, ui->infos->total_time, zoom_coeff);
  gint pixels_right = convert_time_to_pixels(ui->infos->width_drawing_area, time_right, 
      ui->infos->current_time, ui->infos->total_time, zoom_coeff);
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
    gint *splitpoint_left, ui_infos *infos)
{
  gint i = 0;
  for (i = 0; i < infos->splitnumber; i++ )
  {
    gint current_point_hundr_secs = get_splitpoint_time(i);
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
gint get_splitpoint_clicked(gint button_y, gint type_clicked, gint type)
{
  gint time_pos,time_right_pos,time_margin;
  gint left_time = get_left_drawing_time(ui->infos->current_time, ui->infos->total_time, zoom_coeff);
  
  gint but_y;
  
  //we see if we click on a right button
  if (type_clicked != 3)
  {
    but_y = button_y;
    time_pos = left_time + pixels_to_time(ui->infos->width_drawing_area,button_x);
  }
  else
  {
    but_y = button_y2;
    time_pos = left_time + pixels_to_time(ui->infos->width_drawing_area,button_x2);
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
    time_right_pos = left_time+
      pixels_to_time(ui->infos->width_drawing_area,button_x + pixels_to_look_for);
  }
  else
  {
    time_right_pos = left_time+
      pixels_to_time(ui->infos->width_drawing_area,button_x2 + pixels_to_look_for);
  }

  //the time margin is the margin for the splitpoint,
  //where we can click at his left or right
  time_margin = time_right_pos - time_pos;
  
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
  else //if (type == 3)
  {
    margin1 = ui->gui->splitpoint_ypos + ui->gui->margin;
    margin2 = ui->gui->splitpoint_ypos + ui->gui->margin + ui->gui->real_checkbox_length;
  }

  gint splitpoint_returned = -1;
  
  //if we are in the area to move the split 
  if ((but_y > margin1) && (but_y < margin2))
  {
    gint current_point_left, current_point_right;

    gint i = 0;
    for(i = 0; i < ui->infos->splitnumber; i++ )
    {
      gint current_point_hundr_secs = get_splitpoint_time(i);
      //left margin
      current_point_left = current_point_hundr_secs - time_margin;
      //right margin
      current_point_right = current_point_hundr_secs + time_margin;

      //if we found a valid splitpoint, we return it
      if ((time_pos >= current_point_left) && (time_pos <= current_point_right))
      {
        splitpoint_returned = i;
        break;
      }
    }
  }
  
  return splitpoint_returned;
}

//!makes a quick preview of the song
void player_quick_preview(gint splitpoint_to_preview)
{
  if (splitpoint_to_preview != -1)
  {
    preview_start_position = get_splitpoint_time(splitpoint_to_preview);
    preview_start_splitpoint = splitpoint_to_preview;

    if (!player_is_playing())
    {
      player_play();
      usleep(50000);
    }

    if (player_is_paused())
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pause_button), FALSE);
    }

    if (splitpoint_to_preview < ui->infos->splitnumber-1)
    {
      quick_preview_end_splitpoint = splitpoint_to_preview + 1;
    }
    else
    {
      quick_preview_end_splitpoint = -1;
    }

    player_seek(preview_start_position * 10);
    change_progress_bar();
    put_status_message(_(" quick preview..."));

    quick_preview = FALSE;
    if (quick_preview_end_splitpoint != -1)
    {
      quick_preview = TRUE;
    }

    if (preview_start_splitpoint == (ui->infos->splitnumber-1))
    {
      cancel_quick_preview_all();
    }
  }
}

//!drawing area press event
gboolean da_press_event (GtkWidget    *da,
                         GdkEventButton *event,
                         gpointer     data)
{
  //only if we are playing
  //and the timer active(connected to player)
  if (playing && timer_active)
  {
    if (event->button == 1)
    {
      button_x = event->x;
      button_y = event->y;
      button1_pressed = TRUE;

      if ((button_y > ui->gui->progress_ylimit + ui->gui->margin) &&
          (button_y < ui->gui->progress_ylimit + ui->gui->margin + ui->gui->real_move_split_length))
      {
        splitpoint_to_move = get_splitpoint_clicked(button_y,1, 2);
        if (splitpoint_to_move != -1)
        {
          move_splitpoints = TRUE;
        }
      }
      else
      {
        //if we are in the area to remove a splitpoint
        if ((button_y > ui->gui->margin) && (button_y < ui->gui->margin + ui->gui->real_erase_split_length))
        {
          gint splitpoint_selected;
          splitpoint_selected = get_splitpoint_clicked(button_y, 1, 1);

          if (splitpoint_selected != -1)
          {
            select_splitpoints = TRUE;
            select_splitpoint(splitpoint_selected);
          }

          refresh_drawing_area(ui->gui);
        }
        else
        {
          //if we are in the area to check a splitpoint
          if ((button_y > ui->gui->splitpoint_ypos + ui->gui->margin) &&
              (button_y < ui->gui->splitpoint_ypos + ui->gui->margin + ui->gui->real_checkbox_length))
          {
            gint splitpoint_selected = get_splitpoint_clicked(button_y, 1, 3);
            if (splitpoint_selected != -1)
            {
              check_splitpoint = TRUE;
              update_splitpoint_check(splitpoint_selected);
            }
            refresh_drawing_area(ui->gui);
          }
        }
      }

      if (!move_splitpoints)
      {
        move_time = ui->infos->current_time;
      }
      else
      {
        move_time = get_splitpoint_time(splitpoint_to_move);
      }
    }
    else
    {
      //right click
      if (event->button == 3)
      {
        button_x2 = event->x;
        button_y2 = event->y;
        button2_pressed = TRUE;
        zoom_coeff_old = zoom_coeff;

      if ((button_y2 > ui->gui->progress_ylimit + ui->gui->margin) &&
          (button_y2 < ui->gui->progress_ylimit + ui->gui->margin + ui->gui->real_move_split_length))
        {
          gint splitpoint_to_preview = -1;

          splitpoint_to_preview = get_splitpoint_clicked(button_y2,3, 2);

          //player quick preview here!!
          player_quick_preview(splitpoint_to_preview);
        }
        else
        {
          //if we are in the area to remove a splitpoint
          if ((button_y2 > ui->gui->margin) && (button_y2 < ui->gui->margin + ui->gui->real_erase_split_length))
          {
            gint splitpoint_to_erase = -1;

            //TRUE means remove splitpoint area
            splitpoint_to_erase = get_splitpoint_clicked(button_y2,3, 1);

            //if we have found a splitpoint to erase
            if (splitpoint_to_erase != -1)
            {
              remove_splitpoints = TRUE;
              remove_splitpoint(splitpoint_to_erase,TRUE);
            }

            refresh_drawing_area(ui->gui);
          }
        }
      }
    }
  }

  return TRUE;
}

//!drawing area release event
gboolean da_unpress_event(GtkWidget *da, GdkEventButton *event, gpointer data)
{
  //only if we are playing
  //and the timer active(connected to player)
  if (playing && timer_active)
  {
    if (event->button == 1)
    {
      button1_pressed = FALSE;
      //if we move the current _position_
      if (!move_splitpoints && !remove_splitpoints &&
          !select_splitpoints && !check_splitpoint)
      {
        remove_status_message();
        player_seek((gint)(move_time * 10));
        change_progress_bar();

        //if we have more than 2 splitpoints
        //if we are outside the split preview, we 
        //cancel split preview
        if (quick_preview_end_splitpoint == -1)
        {
          if (move_time < get_splitpoint_time(preview_start_splitpoint))
          {
            cancel_quick_preview_all();
          }
        }
        else
        {
          if ((move_time < get_splitpoint_time(preview_start_splitpoint)) ||
              (move_time > get_splitpoint_time(quick_preview_end_splitpoint)))
          {
            cancel_quick_preview_all();
          }
          else
          //if we are inside, we turn on quick preview
          {
            //if we don't have a preview with the last
            //splitpoint
            if (quick_preview_end_splitpoint != -1)
            {
              //we unpause the player
              if (player_is_paused())
              {
                player_pause();
              }
              quick_preview = TRUE;
            }
          }
        }
      }
      else
      {
        //if we moved the splitpoint
        if (move_splitpoints)
        {
          //we update the current splitpoint
          update_splitpoint_from_time(splitpoint_to_move, move_time);
          splitpoint_to_move = -1;                
        }
      }
      move_splitpoints = FALSE;
      select_splitpoints = FALSE;
      check_splitpoint = FALSE;
    }
    else
    {
      if (event->button == 3)
      {
        button2_pressed = FALSE;
        remove_splitpoints = FALSE;
      }
    }
  }
  
  refresh_drawing_area(ui->gui);
  
  return TRUE;
}

//!on drawing area event
gboolean da_notify_event(GtkWidget *da, GdkEventMotion *event, gpointer data)
{
  //only if we are playing
  //and the timer active(connected to player)
  if ((playing && timer_active) &&
      (button1_pressed || button2_pressed))
  {
    gint x, y;
    GdkModifierType state;
    wh_get_pointer(event, &x, &y, &state);

    //drawing area width
    gint width = 0;
    wh_get_widget_size(ui->gui->drawing_area, &width, NULL);
    gfloat width_drawing_area = (gfloat) width;

    if (state)
    {
      if (button1_pressed)
      {
        if (move_splitpoints)
        {
          gdouble splitpoint_time = get_splitpoint_time(splitpoint_to_move);
          move_time = splitpoint_time + pixels_to_time(width_drawing_area,(x - button_x));
        }
        else
        {
          //if we remove a splitpoint
          if (remove_splitpoints || select_splitpoints || check_splitpoint)
          {
            move_time = ui->infos->current_time;
          }
          else
          {
            move_time = ui->infos->current_time +
              pixels_to_time(width_drawing_area,(x - button_x));
          }
        }

        if (move_time < 0)
        {
          move_time = 0;
        }
        if (move_time > ui->infos->total_time)
        {
          move_time = ui->infos->total_time;
        }

        refresh_drawing_area(ui->gui);
      }
      else
      {
        if (button2_pressed)
        {
          gint diff = -((event->x - button_x2) * 1);

          if (diff < (-width_drawing_area + 1))
          {
            diff = -width_drawing_area + 1;
          }
          if (diff > (width_drawing_area - 1))
          {
            diff = width_drawing_area - 1;
          }

          zoom_coeff = diff / (width_drawing_area);

          if (zoom_coeff < 0)
          {
            zoom_coeff = 1/(zoom_coeff+1);
          }
          else
          {
            zoom_coeff = 1 - zoom_coeff;
          }

          zoom_coeff = zoom_coeff_old * zoom_coeff;

          adjust_zoom_coeff();

          refresh_drawing_area(ui->gui);
        }
      }
    }
  }

  return TRUE;
}

void adjust_zoom_coeff()
{
  if (zoom_coeff < 0.2)
  {
    zoom_coeff = 0.2;
  }
  if (zoom_coeff > 10 * ui->infos->total_time / 6000)
  {
    zoom_coeff = 10 * ui->infos->total_time / 6000;
  }
}


static void drawing_area_expander_event(GObject *object, GParamSpec *param_spec, gpointer data)
{
  if (object == NULL)
  {
    return;
  }

  GtkExpander *expander = GTK_EXPANDER(object);
  if (gtk_expander_get_expanded(expander))
  {
    gtk_widget_show(silence_wave_check_button);
  }
  else
  {
    gtk_widget_hide(silence_wave_check_button);
  }
}

//!creates the progress drawing area under the player buttons
static GtkWidget *create_drawing_area(ui_state *ui)
{
  GtkWidget *frame = gtk_frame_new(NULL);
 
  GdkColor color;
  color.red = 65000; color.green = 0; color.blue = 0;
  gtk_widget_modify_bg(frame, GTK_STATE_NORMAL, &color);

  gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_NONE);

  GtkWidget *drawing_area = gtk_drawing_area_new();
  ui->gui->drawing_area = drawing_area;

  gtk_widget_set_size_request(drawing_area, DRAWING_AREA_WIDTH, DRAWING_AREA_HEIGHT);

#if GTK_MAJOR_VERSION <= 2
  g_signal_connect(drawing_area, "expose_event", G_CALLBACK(da_draw_event), ui);
#else
  g_signal_connect(drawing_area, "draw", G_CALLBACK(da_draw_event), ui);
#endif

  g_signal_connect(drawing_area, "button_press_event", G_CALLBACK(da_press_event), NULL);
  g_signal_connect(drawing_area, "button_release_event", G_CALLBACK(da_unpress_event), NULL);
  g_signal_connect(drawing_area, "motion_notify_event", G_CALLBACK(da_notify_event), NULL);

  gtk_widget_set_events(drawing_area, gtk_widget_get_events(drawing_area)
      | GDK_LEAVE_NOTIFY_MASK | GDK_BUTTON_PRESS_MASK
      | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK
      | GDK_POINTER_MOTION_HINT_MASK);

  gtk_container_add(GTK_CONTAINER(frame), drawing_area);

  GtkWidget *drawing_area_expander = gtk_expander_new_with_mnemonic(_("Splitpoints _view"));
  ui->gui->drawing_area_expander = drawing_area_expander;
  gtk_expander_set_expanded(GTK_EXPANDER(drawing_area_expander), TRUE);
  g_signal_connect(drawing_area_expander, "notify::expanded", G_CALLBACK(drawing_area_expander_event), NULL);
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
  gtk_container_set_border_width(GTK_CONTAINER(hbox), 0);
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
  hbox = (GtkWidget *)create_player_buttons_hbox();
  gtk_container_set_border_width(GTK_CONTAINER(hbox), 0);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

  return main_hbox;
}

//!add a row to the table
void add_playlist_file(const gchar *name)
{
  if (file_exists(name))
  {
    gboolean name_already_exists_in_playlist = FALSE;

    GtkTreeModel *model = gtk_tree_view_get_model(playlist_tree);

    gchar *filename = NULL;

    GtkTreeIter iter;

    gint i = 0;
    while (i < playlist_tree_number)
    {
      GtkTreePath *path = gtk_tree_path_new_from_indices(i ,-1);
      gtk_tree_model_get_iter(model, &iter, path);
      gtk_tree_model_get(model, &iter, COL_FILENAME, &filename, -1);

      if (strcmp(filename,name) == 0)
      {
        name_already_exists_in_playlist = TRUE;
        break;
      }

      g_free(filename);
      i++;
    }

    if (! name_already_exists_in_playlist)
    {
      gtk_widget_set_sensitive(playlist_remove_all_files_button,TRUE);
      gtk_list_store_append(GTK_LIST_STORE(model), &iter);

      //sets text in the minute, second and milisecond column
      gtk_list_store_set (GTK_LIST_STORE(model), 
          &iter,
          COL_NAME, get_real_name_from_filename(name),
          COL_FILENAME,name,
          -1);
      playlist_tree_number++;
    }
  }
}

//!when closing the new window after detaching
void close_playlist_popup_window_event(GtkWidget *window, gpointer data)
{
  if (playlist_handle_window == NULL)
  {
    return;
  }

  GtkWidget *window_child = gtk_bin_get_child(GTK_BIN(playlist_handle_window));
  gtk_widget_reparent(GTK_WIDGET(window_child), GTK_WIDGET(playlist_handle));
  gtk_widget_destroy(playlist_handle_window);
}

//!when we detach the handle
void handle_playlist_detached_event(GtkHandleBox *handlebox, GtkWidget *widget, gpointer data)
{
  playlist_handle_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  gtk_widget_reparent(GTK_WIDGET(widget), GTK_WIDGET(playlist_handle_window));

  g_signal_connect(G_OBJECT(playlist_handle_window), "delete_event",
                   G_CALLBACK(close_playlist_popup_window_event), NULL);
  
  gtk_widget_show(GTK_WIDGET(playlist_handle_window));
}

//!creates the model for the playlist
GtkTreeModel *create_playlist_model()
{
  GtkListStore * model =
    gtk_list_store_new(PLAYLIST_COLUMNS,
        G_TYPE_STRING,
        G_TYPE_STRING);

  return GTK_TREE_MODEL(model);
}

//!creates the playlist tree
GtkTreeView *create_playlist_tree()
{
  GtkTreeModel *model = create_playlist_model();
  GtkTreeView *playlist_tree = GTK_TREE_VIEW(gtk_tree_view_new_with_model(model));
  gtk_tree_view_set_headers_visible(playlist_tree, FALSE);
  return playlist_tree;
}

//!creates playlist columns
void create_playlist_columns(GtkTreeView *playlist_tree)
{
  GtkCellRendererText *renderer;
  GtkTreeViewColumn *name_column;
  //GtkTreeViewColumn *filename_column;

  //renderer creation
  renderer = GTK_CELL_RENDERER_TEXT(gtk_cell_renderer_text_new());
  g_object_set_data(G_OBJECT(renderer), "col", GINT_TO_POINTER(COL_NAME));
  name_column = gtk_tree_view_column_new_with_attributes 
    (_("History"), GTK_CELL_RENDERER(renderer),
     "text", COL_NAME, NULL);

  //we dont insert the column to the tree view
  /*  renderer = GTK_CELL_RENDERER_TEXT(gtk_cell_renderer_text_new ());
      filename_column = gtk_tree_view_column_new_with_attributes 
      (_("Complete filename"), GTK_CELL_RENDERER(renderer),
      "text", COL_FILENAME,
      NULL);*/
  /*  gtk_tree_view_insert_column (GTK_TREE_VIEW (playlist_tree),
      GTK_TREE_VIEW_COLUMN (filename_column),COL_FILENAME);*/
  
  //appends columns to the list of columns of tree_view
  gtk_tree_view_insert_column(playlist_tree,
      GTK_TREE_VIEW_COLUMN(name_column), COL_NAME);

  //middle alignment of the column name
  gtk_tree_view_column_set_alignment(GTK_TREE_VIEW_COLUMN(name_column), 0.5);
  gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(name_column),
      GTK_TREE_VIEW_COLUMN_AUTOSIZE);
}

//!split selection has changed
void playlist_selection_changed(GtkTreeSelection *selec,
                                gpointer data)
{
  GtkTreeModel *model = gtk_tree_view_get_model(playlist_tree);
  GtkTreeSelection *selection = gtk_tree_view_get_selection(playlist_tree);
  GList *selected_list = gtk_tree_selection_get_selected_rows(selection, &model);

  if (g_list_length(selected_list) > 0)
  {
    gtk_widget_set_sensitive(playlist_remove_file_button, TRUE);
  }
  else
  {
    gtk_widget_set_sensitive(playlist_remove_file_button, FALSE);
  }
}

//!event for the remove file button
void playlist_remove_file_button_event(GtkWidget *widget, gpointer data)
{ 
  GtkTreeModel *model = gtk_tree_view_get_model(playlist_tree);
  GtkTreeSelection *selection = gtk_tree_view_get_selection(playlist_tree);
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
    playlist_tree_number--;

    gtk_tree_path_free(path);
    g_free(filename);
  }

  if (playlist_tree_number == 0)
  {
    gtk_widget_set_sensitive(playlist_remove_all_files_button, FALSE);
  }

  gtk_widget_set_sensitive(playlist_remove_file_button,FALSE);

  //we free the selected elements
  g_list_foreach(selected_list, (GFunc)gtk_tree_path_free, NULL);
  g_list_free(selected_list);  
}

//!event for the remove file button
void playlist_remove_all_files_button_event(GtkWidget *widget, gpointer data)
{
  GtkTreeModel *model = gtk_tree_view_get_model(playlist_tree);
  
  gchar *filename = NULL;
  while (playlist_tree_number > 0)
  {
    GtkTreeIter iter;
    gtk_tree_model_get_iter_first(model, &iter);
    gtk_tree_model_get(model, &iter, 
        COL_FILENAME, &filename, -1);
    gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
    playlist_tree_number--;
    g_free(filename);
  }
  
  gtk_widget_set_sensitive(playlist_remove_all_files_button, FALSE);
  gtk_widget_set_sensitive(playlist_remove_file_button, FALSE);
}

//!creates the horizontal queue buttons horizontal box
GtkWidget *create_delete_buttons_hbox()
{
  //our horizontal box
  GtkWidget *hbox = wh_hbox_new();

  //button for removing a file
  playlist_remove_file_button =
    wh_create_cool_button(GTK_STOCK_DELETE, _("_Erase selected entries"),FALSE);
  gtk_box_pack_start(GTK_BOX(hbox),
                     playlist_remove_file_button, FALSE, FALSE, 5);
  gtk_widget_set_sensitive(playlist_remove_file_button,FALSE);
  g_signal_connect(G_OBJECT(playlist_remove_file_button), "clicked",
                   G_CALLBACK(playlist_remove_file_button_event), NULL);
 
  //button for removing a file
  playlist_remove_all_files_button =
    wh_create_cool_button(GTK_STOCK_DELETE, _("E_rase all history"),FALSE);
  gtk_box_pack_start(GTK_BOX(hbox),
                     playlist_remove_all_files_button, FALSE, FALSE, 5);
  gtk_widget_set_sensitive(playlist_remove_all_files_button,FALSE);
  g_signal_connect(G_OBJECT(playlist_remove_all_files_button), "clicked",
                   G_CALLBACK(playlist_remove_all_files_button_event), NULL);
  
  return hbox;
}

//!creates the playlist of the player
GtkWidget *create_player_playlist_frame()
{
  GtkWidget *vbox = wh_vbox_new();

  // scrolled window and the tree 
  //create the tree and add it to the scrolled window
  playlist_tree = create_playlist_tree();
  GtkWidget *scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_NONE);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_AUTOMATIC);
  gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
  //create columns
  create_playlist_columns(playlist_tree);
  //add the tree to the scrolled window
  gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(playlist_tree));
  g_signal_connect(G_OBJECT(playlist_tree), "row-activated",
                   G_CALLBACK(split_tree_row_activated), NULL);

  //selection for the tree
  GtkWidget *playlist_tree_selection = (GtkWidget *)
    gtk_tree_view_get_selection(playlist_tree);
  g_signal_connect(G_OBJECT(playlist_tree_selection), "changed",
                   G_CALLBACK(playlist_selection_changed), NULL);
  gtk_tree_selection_set_mode(GTK_TREE_SELECTION(playlist_tree_selection),
                              GTK_SELECTION_MULTIPLE);

  //horizontal box with delete buttons
  GtkWidget *delete_buttons_hbox = (GtkWidget *)create_delete_buttons_hbox();
  gtk_box_pack_start(GTK_BOX(vbox), delete_buttons_hbox, FALSE, FALSE, 2);

  GtkWidget *history_expander = gtk_expander_new_with_mnemonic(_("H_istory"));
  gtk_expander_set_expanded(GTK_EXPANDER(history_expander), FALSE);
  gtk_container_add(GTK_CONTAINER(history_expander), vbox);

  GtkWidget *main_hbox = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(main_hbox), history_expander, TRUE, TRUE, 4);

  /* handle box for detaching */
  playlist_handle = gtk_handle_box_new();
  gtk_container_add(GTK_CONTAINER(playlist_handle), GTK_WIDGET(main_hbox));
  g_signal_connect(playlist_handle, "child-detached",
                   G_CALLBACK(handle_playlist_detached_event), NULL);

  return playlist_handle;
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
  action_set_sensitivity("Add_splitpoint", sensitivity, gui);
  action_set_sensitivity("Delete_closest_splitpoint", sensitivity, gui);
  action_set_sensitivity("Zoom_in", sensitivity, gui);
  action_set_sensitivity("Zoom_out", sensitivity, gui);
}

/*! timer used to print infos about the song

Examples are the elapsed time and if it uses variable bitrate
*/
gint mytimer(gpointer data)
{
  if (ui->status->currently_compute_douglas_peucker_filters)
  {
    return TRUE;
  }

  if (player_is_running())
  {
    if (playing)
    {
      //if we have at least one song on the playlist
      if (player_get_playlist_number() > -1)
      {
        //if the player is playing, print the time
        if (player_is_playing())
        {
          print_all_song_infos();
          print_song_time_elapsed();
          gtk_widget_set_sensitive(GTK_WIDGET(ui->gui->progress_bar), TRUE);
        }

        check_stream();

        //if we have a stream, we must not change the progress bar
        if(!stream)
        {
          change_progress_bar();
        }

        //part of quick preview
        if (preview_start_splitpoint != -1)
        {
          //if we have a splitpoint after the current
          //previewed one, update quick_preview_end
          if (preview_start_splitpoint+1 < ui->infos->splitnumber)
          {
            quick_preview_end_splitpoint = preview_start_splitpoint+1;
          }
          else
          {
            if (preview_start_splitpoint+1 == ui->infos->splitnumber)
            {
              quick_preview_end_splitpoint = -1;
            }
          }
        }

        //if we have a preview, stop if needed
        if (quick_preview)
        {
          gint stop_splitpoint = get_splitpoint_time(quick_preview_end_splitpoint);

          if ((stop_splitpoint < (gint)ui->infos->current_time)
              && (quick_preview_end_splitpoint != -1))
          {
            gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pause_button), TRUE);
            cancel_quick_preview();
            put_status_message(_(" quick preview finished, song paused"));
          }
        }

        //enable volume bar if needed
        if(!gtk_widget_is_sensitive(volume_button))
          gtk_widget_set_sensitive(GTK_WIDGET(volume_button), TRUE);
      }
      else
      {
        playing = FALSE;
        reset_label_time(ui->gui);
      }

      if (player_is_paused())
      {
        if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pause_button)))
        {
          only_press_pause = TRUE;
          gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pause_button), TRUE);
          only_press_pause = FALSE;
        }
      }
      else
      {
        if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pause_button)))
        {
          only_press_pause = TRUE;
          gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pause_button), FALSE);
          only_press_pause = FALSE;
        }
      }
    }
    else
    {
      //if not playing but still connected
      if ((ui->infos->player_minutes != 0) ||
          (ui->infos->player_seconds != 0))
      {
        ui->infos->player_minutes = 0;
        ui->infos->player_seconds = 0;
      }

      print_player_filename();
      reset_song_infos(ui->gui);
      reset_label_time(ui->gui);
      reset_inactive_progress_bar();
      gtk_widget_set_sensitive(player_add_button, FALSE);
      gtk_widget_set_sensitive(silence_wave_check_button, FALSE);
    }

    //if connected, almost always change volume bar
    if ((change_volume)&& (!on_the_volume_button))
    {
      change_volume_button();
    }

    playing = player_is_playing();

    if (playing)
    {
      if (!gtk_widget_get_sensitive(player_add_button))
      {
        gtk_widget_set_sensitive(player_add_button, TRUE);
      }
      if (!gtk_widget_get_sensitive(silence_wave_check_button))
      {
        gtk_widget_set_sensitive(silence_wave_check_button, TRUE);
      }

      if (!gtk_widget_get_sensitive(stop_button))
      {
        gtk_widget_set_sensitive(stop_button, TRUE);
        gtk_button_set_image(GTK_BUTTON(stop_button), g_object_ref(StopButton_active));
      }
      if (!gtk_widget_get_sensitive(pause_button))
      {
        gtk_widget_set_sensitive(pause_button, TRUE);
        gtk_button_set_image(GTK_BUTTON(pause_button), g_object_ref(PauseButton_active));
      }

      player_key_actions_set_sensitivity(TRUE, ui->gui);
    }
    else
    {
      if (gtk_widget_get_sensitive(stop_button))
      {
        gtk_widget_set_sensitive(stop_button, FALSE);
        gtk_button_set_image(GTK_BUTTON(stop_button), g_object_ref(StopButton_inactive));
      }
      if (gtk_widget_get_sensitive(pause_button))
      {
        gtk_widget_set_sensitive(pause_button, FALSE);
        gtk_button_set_image(GTK_BUTTON(pause_button), g_object_ref(PauseButton_inactive));
      }
  
      player_key_actions_set_sensitivity(FALSE, ui->gui);
    }

    return TRUE;
  }
  else
  {
    //if connected and player not running, disconnect..

    clear_data_player();
    playing = FALSE;
    disconnect_button_event(disconnect_button, NULL);

    return FALSE;
  }
}

/*!event for the file chooser cancel button

Moved here from the file tab
*/
void file_chooser_cancel_event()
{
  gtk_widget_set_sensitive(ui->gui->browse_button, TRUE);
}

//event for the file chooser ok button
void file_chooser_ok_event(gchar *fname)
{
  change_current_filename(fname);
  gtk_widget_set_sensitive(ui->gui->browse_button, TRUE);
  gtk_widget_set_sensitive(play_button, TRUE);
  gtk_button_set_image(GTK_BUTTON(play_button), g_object_ref(PlayButton_active));

  file_browsed = TRUE;

  if (timer_active)
  {
    GList *song_list = NULL;
    song_list = g_list_append(song_list, fname);
    player_start_add_files(song_list);
  }
}

//when closing the new window after detaching
void close_file_popup_window_event( GtkWidget *window,
                                    gpointer data )
{
  GtkWidget *window_child;

  window_child = gtk_bin_get_child(GTK_BIN(window));

  gtk_widget_reparent(GTK_WIDGET(window_child), GTK_WIDGET(file_handle_box));

  gtk_widget_destroy(window);
}

/*!fix ogg stream action

we split from 0 to a big number
*/
gpointer fix_ogg_stream(gpointer data)
{
  ui->status->splitting = TRUE;

  enter_threads();

  put_options_from_preferences();

  exit_threads();

  gint err = 0;

  mp3splt_erase_all_splitpoints(ui->mp3splt_state,&err);
  
  mp3splt_append_splitpoint(ui->mp3splt_state, 0, NULL, SPLT_SPLITPOINT);
  mp3splt_append_splitpoint(ui->mp3splt_state, LONG_MAX-1, NULL, SPLT_SKIPPOINT);
 
  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_OUTPUT_FILENAMES,
                         SPLT_OUTPUT_DEFAULT);
  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_SPLIT_MODE,
                         SPLT_OPTION_NORMAL_MODE);
 
  enter_threads();

  remove_all_split_rows();  
  filename_to_split = get_input_filename(ui->gui);

  exit_threads();
  
  gint confirmation = SPLT_OK;
  mp3splt_set_path_of_split(ui->mp3splt_state,filename_path_of_split);
  mp3splt_set_filename_to_split(ui->mp3splt_state,filename_to_split);
  confirmation = mp3splt_split(ui->mp3splt_state);
  
  enter_threads();

  print_status_bar_confirmation(confirmation);

  exit_threads();

  ui->status->splitting = FALSE;

  return NULL;
}

//! Hide the connect button
void hide_connect_button()
{
  gtk_widget_hide(connect_button);
}

//! Show the connect button
void show_connect_button()
{
  if (! wh_container_has_child(GTK_CONTAINER(player_buttons_hbox), connect_button))
  {
    gtk_box_pack_start(GTK_BOX(player_buttons_hbox), connect_button, FALSE, FALSE, 7);
  }
  gtk_widget_show_all(connect_button);
}

//! Hide the disconnect button
void hide_disconnect_button()
{
  gtk_widget_hide(disconnect_button);
}

//! Show the disconnec button
void show_disconnect_button()
{
  if (! wh_container_has_child(GTK_CONTAINER(player_buttons_hbox), disconnect_button))
  {
    gtk_box_pack_start(GTK_BOX(player_buttons_hbox), disconnect_button, FALSE, FALSE, 7);
  }
  gtk_widget_show_all(disconnect_button);
}

