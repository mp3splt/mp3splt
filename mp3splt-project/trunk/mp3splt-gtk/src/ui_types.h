/**********************************************************
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
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

#ifndef UI_TYPES_H

#define UI_TYPES_H

#include <libmp3splt/mp3splt.h>

#include "preferences_manager.h"

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
} ui_infos;

typedef struct {
  GtkWidget *window;
  GtkActionGroup *action_group;

  GtkWidget *browse_entry;
  GtkWidget *browse_button;
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
  //
  gint erase_split_ylimit;
  gint progress_ylimit;
  gint splitpoint_ypos;
  gint checkbox_ypos;
  gint text_ypos;
  gint wave_ypos;

  GPtrArray *wave_quality_das;
  GtkWidget *player_scrolled_window;

  GtkStatusbar *status_bar;
} gui_state;

typedef struct {
  gint splitting;
  gint quit_main_program;
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
} gui_status;

typedef struct {
  gint return_code;

  ui_infos *infos;
  preferences_state *preferences;
  splt_state *mp3splt_state;

  GArray *splitpoints;
  gui_state *gui;
  gui_status *status;
} ui_state;

#endif

