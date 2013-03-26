/**********************************************************
 * mp3splt-gtk -- utility based on mp3splt,
 *
 *
 * Contact: m@ioalex.net
 * Copyright: (C) 2005-2013 Alexandru Munteanu
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

#ifndef UI_TYPES_H

#define UI_TYPES_H

#include <libmp3splt/mp3splt.h>

#ifndef NO_GSTREAMER
  #include <gst/gst.h>
#endif

#ifndef NO_AUDACIOUS
  #include <audacious/audctrl.h>
  #include <audacious/dbus.h>
#endif

#ifdef __WIN32__
  #include <windows.h>
  #include <winsock2.h>
#endif

typedef struct {
  gchar* main_key;
  gchar* second_key;
  gint default_value;
  GtkWidget *spinner;
  void (*update_spinner_value_cb)(GtkWidget *spinner, gpointer data);
  gpointer user_data_for_cb;
} spinner_int_preference;

typedef struct {
  gchar* main_key;
  gchar* second_key;
  gint default_value;
  GtkWidget *range;
  void (*update_adjustment_value)(GtkAdjustment *adjustment, gpointer data);
  gpointer user_data_for_cb;
} range_preference;

typedef struct {
  GArray *spinner_int_preferences;
  GArray *range_preferences;
} preferences_state;

typedef struct
{
  gboolean checked;
  gint mins;
  gint secs;
  gint hundr_secs;
} Split_point;

typedef struct {
  gint root_x_pos;
  gint root_y_pos;
  gint width;
  gint height;
} ui_main_window;

typedef struct {
  long time;
  float level;
} silence_wave;

typedef struct {
  gint index;
  gpointer data;
} preview_index_and_data;

typedef struct {
  gchar *browser_directory;
  ui_main_window *main_win;
  GList *text_options_list;

  silence_wave *silence_points;
  gint number_of_silence_points;
  gint malloced_num_of_silence_points;

  gint player_seconds;
  gint player_minutes;
  gint player_hundr_secs;
  //only for internal use when we change manually we have the real
  //time which is player_seconds and the imaginary time player_seconds2
  gint player_seconds2;
  gint player_minutes2;
  gint player_hundr_secs2;

  gfloat total_time;
  gfloat current_time;

  gint splitnumber;
  gint width_drawing_area;
  gfloat zoom_coeff;
  gfloat zoom_coeff_old;

  gint hundr_secs_th;
  gint tens_of_secs_th;
  gint secs_th;
  gint ten_secs_th;
  gint minutes_th;
  gint ten_minutes_th;

  gint one_minute_time;
  gint three_minutes_time;
  gint six_minutes_time;
  gint ten_minutes_time;
  gint twenty_minutes_time;
  gint fourty_minutes_time;

  GArray *preview_time_windows;

  GPtrArray *filtered_points_presence;
  gint silence_wave_number_of_points_threshold;

  gint selected_player;

  gdouble douglas_peucker_thresholds[6];
  gdouble douglas_peucker_thresholds_defaults[6];

  gint debug_is_active;

  gfloat silence_threshold_value;
  gfloat silence_offset_value;
  gint silence_number_of_tracks;
  gfloat silence_minimum_length;
  gfloat silence_minimum_track_length;
  gboolean silence_remove_silence_between_tracks;

  gint freedb_table_number;
  gint playlist_tree_number;
  gint multiple_files_tree_number;

  gint freedb_selected_id;

  splt_freedb_results *freedb_search_results;

  gint split_file_mode;

  GString *outputdirname;

  preview_index_and_data preview_indexes[6];

  gint timeout_value;
} ui_infos;

typedef struct {
#ifndef NO_GSTREAMER
  const gchar *song_artist;
  const gchar *song_title;
  gint rate;
  GstElement *play;
  GstBus *bus;
  gint _gstreamer_is_running;
#endif

#ifndef NO_AUDACIOUS
  DBusGProxy *dbus_proxy;
  DBusGConnection *dbus_connection;
#endif

  //snackamp
  FILE *in;
  FILE *out;
  gboolean connected;
#ifdef __WIN32__
  SOCKET socket_id;
#else
  gint socket_id;
#endif

} player_infos;

typedef struct {
  GtkWidget *window;

  GtkActionGroup *action_group;
  GtkWidget *open_file_chooser_button;
  GString *input_filename;

  GtkWidget *player_box;
  GtkWidget *playlist_box;

  GtkWidget *song_name_label;
  GtkWidget *song_infos;
  GtkWidget *label_time;

  GtkAdjustment *progress_adj;
  GtkWidget *progress_bar;

  GtkTreeView *tree_view;

  GtkWidget *drawing_area;
  GtkProgressBar *percent_progress_bar;
  GtkWidget *drawing_area_expander;

  //drawing area variables
  gint margin;
  gint real_erase_split_length;
  gint real_move_split_length;
  gint real_checkbox_length;
  gint real_wave_length;

  gint erase_split_ylimit;
  gint progress_ylimit;
  gint splitpoint_ypos;
  gint checkbox_ypos;
  gint text_ypos;
  gint wave_ypos;

  GPtrArray *wave_quality_das;
  GtkWidget *player_scrolled_window;

  GtkStatusbar *status_bar;

  GtkWidget *add_button;
  GtkWidget *remove_row_button;
  GtkWidget *remove_all_button;

  GtkTextTagTable *mess_hist_tag_table;
  GtkTextBuffer *mess_hist_buffer;

  GtkToggleButton *names_from_filename;

  GtkWidget *pause_button;

  GtkBox *player_buttons_hbox;

  //player button images
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

  //player buttons
  GtkWidget *play_button;
  GtkWidget *stop_button;
  GtkWidget *player_add_button;
  GtkWidget *go_beg_button;
  GtkWidget *go_end_button;

  GtkWidget *connect_button;
  GtkWidget *disconnect_button;

  GtkWidget *silence_wave_check_button;

  GtkWidget *cancel_button;

  GtkWidget *volume_button;

  GtkTreeView *playlist_tree;

  GtkWidget *playlist_remove_file_button;
  GtkWidget *playlist_remove_all_files_button;

  GtkWidget *frame_mode;
  GtkWidget *adjust_mode;

  GtkWidget *spinner_adjust_offset;
  GtkWidget *spinner_adjust_gap;
  GtkWidget *spinner_adjust_min;
  GtkWidget *spinner_adjust_threshold;

  GtkWidget *adjust_param_vbox;

  GtkWidget *spinner_time;
  GtkWidget *spinner_equal_tracks;

  GtkWidget *all_spinner_silence_number_tracks;
  GtkWidget *all_spinner_silence_minimum;
  GtkWidget *all_spinner_track_minimum;
  GtkWidget *all_spinner_silence_offset;
  GtkWidget *all_spinner_trim_silence_threshold;
  GtkWidget *all_spinner_silence_threshold;
  GtkWidget *all_silence_remove_silence;

  GtkWidget *all_threshold_label;
  GtkWidget *all_trim_threshold_label;
  GtkWidget *all_offset_label;
  GtkWidget *all_number_of_tracks_label;
  GtkWidget *all_min_silence_label;
  GtkWidget *all_min_track_label;

  GtkWidget *tags_radio;
  GtkWidget *tags_version_radio;

  GtkWidget *output_dir_radio;
  GtkWidget *custom_dir_file_chooser_button;
  GtkWidget *example_output_dir_entry;
  GtkWidget *example_output_dir_box;

  GtkWidget *replace_underscore_by_space_check_box;
  GtkComboBox *artist_text_properties_combo;
  GtkComboBox *album_text_properties_combo;
  GtkComboBox *title_text_properties_combo;
  GtkComboBox *comment_text_properties_combo;
  GtkComboBox *genre_combo;
  GtkWidget *comment_tag_entry;
  GtkWidget *regex_entry;
  GtkWidget *test_regex_fname_entry;
  GtkWidget *sample_result_label;
  GtkWidget *extract_tags_box;

  GtkWidget *create_dirs_from_output_files;

  GtkWidget *output_entry;
  GtkWidget *output_label;
  GtkWidget *output_default_label;

  GtkTreeView *split_tree;

  GtkWidget *spinner_minutes;
  GtkWidget *spinner_seconds;
  GtkWidget *spinner_hundr_secs;

  GtkWidget *scan_trim_silence_button;
  GtkWidget *scan_trim_silence_button_player;
  GtkWidget *scan_silence_button;
  GtkWidget *scan_silence_button_player;

  GtkWidget *spinner_silence_number_tracks;
  GtkWidget *spinner_silence_minimum;
  GtkWidget *spinner_silence_minimum_track;
  GtkWidget *spinner_silence_offset;
  GtkWidget *spinner_silence_threshold;
  GtkWidget *silence_remove_silence;

  GtkWidget *queue_files_button;
  GtkWidget *remove_file_button;
  GtkWidget *remove_all_files_button;

  GtkWidget *freedb_entry;
  GtkWidget *freedb_search_button;
  GtkWidget *freedb_spinner;
  GtkTreeView *freedb_tree;
  GtkWidget *freedb_add_button;

  GtkWidget *radio_output;

  GtkWidget *split_mode_radio_button;

  GtkWidget *time_label;
  GtkWidget *time_label_after;
  GtkWidget *equal_tracks_label;
  GtkWidget *equal_tracks_label_after;

  GtkWidget *multiple_files_component;

  GtkWidget *player_combo_box;
  GtkWidget *radio_button;

  GPtrArray *wave_preview_labels;

  GtkWidget *mess_history_window;
  GtkWidget *mess_hist_view;

  GtkTreeView *multiple_files_tree;

  GtkWidget *multiple_files_remove_file_button;
  GtkWidget *multiple_files_remove_all_files_button;

  GtkWidget *preferences_widget;
  GtkWidget *preferences_window;

  GtkWidget *split_files_widget;
  GtkWidget *split_files_window;
 
  GtkWidget *freedb_widget;
  GtkWidget *freedb_window;

  GtkWidget *splitpoints_widget;
  GtkWidget *splitpoints_window;
} gui_state;

typedef struct {
  gint splitting;
  gint process_in_progress;
  gint mouse_on_progress_bar;
  gint currently_compute_douglas_peucker_filters;
  gint show_silence_wave;
  gboolean playing;
  gboolean timer_active;

  gint quick_preview_end_splitpoint;
  gint preview_start_splitpoint;

  gfloat move_time;

  gboolean button1_pressed;
  gboolean button2_pressed;
  gboolean quick_preview;

  gint button_x;
  gint button_y;
  gint button_x2;
  gint button_y2;

  gboolean move_splitpoints;
  gint splitpoint_to_move;
  gboolean remove_splitpoints;
  gboolean select_splitpoints;
  gboolean check_splitpoint;

  gint first_splitpoint_selected;

  gint spin_mins;
  gint spin_secs;
  gint spin_hundr_secs;

  gchar current_description[255];

  gint preview_start_position;

  gint timeout_id;

  gint currently_scanning_for_silence;

  gchar *filename_to_split;

  gint douglas_callback_counter;

  gboolean stream;

  gint only_press_pause;

  gboolean change_volume;
  gboolean on_the_volume_button;
  gint file_browsed;
  gint preview_row;
  gint selected_split_mode;

  gint should_trim;

  gint file_selection_changed;

  gint stop_split;

  long previous_first_time_drawed;
  gint previous_first_x_drawed;
  long previous_second_time_drawed;
  gint previous_second_x_drawed;
  GHashTable *previous_distance_by_time;
  gfloat previous_zoom_coeff;
  gint previous_interpolation_level;

  gint lock_cue_export;
} gui_status;

#define SPLT_MUTEX GStaticMutex

typedef struct {
  gint return_code;

  ui_infos *infos;
  preferences_state *preferences;
  splt_state *mp3splt_state;

  GArray *splitpoints;
  gui_state *gui;
  gui_status *status;
  player_infos *pi;

  GPtrArray *files_to_split;

  SPLT_MUTEX variables_mutex;

  int importing_cue_from_configuration_directory;
} ui_state;

typedef struct {
  gint err;
  ui_state *ui;
} ui_with_err;

typedef struct {
  ui_state *ui;
  char *fname;
} ui_with_fname;

#endif

