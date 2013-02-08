/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2005-2013 Alexandru Munteanu - m@ioalex.net
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

/**********************************************************
 * Filename: player_window.h
 *
 * header of player_window.c
 *
 *********************************************************/

#ifndef PLAYER_WINDOW_H

#define PLAYER_WINDOW_H

#include "all_includes.h"

#define ICON_EXT ".svg"

#define DEFAULT_TIMEOUT_VALUE 200
#define DEFAULT_SILENCE_WAVE_NUMBER_OF_POINTS_THRESHOLD 4000

//float comparison
#define DELTA 5

#define HUNDR_SECONDS 1
#define TENS_OF_SECONDS 10
#define SECONDS 100
#define TEN_SECONDS 1000
#define MINUTES 6000
#define TEN_MINUTES 60000
#define HOURS 360000

void player_quick_preview(gint splitpoint_to_preview, ui_state *ui);
void check_update_down_progress_bar(ui_state *ui);
void cancel_quick_preview_all(ui_state *ui);
void cancel_quick_preview(gui_status *status);
void enable_player_buttons(ui_state *ui);
void connect_to_player_with_song(gint i, ui_state *ui);
void connect_button_event(GtkWidget *widget, ui_state *ui);
void disconnect_button_event(GtkWidget *widget, ui_state *ui);
void pause_event(GtkWidget *widget, ui_state *ui);
void refresh_drawing_area(gui_state *gui);
gfloat get_right_drawing_time(gfloat current_time, gfloat total_time, gfloat zoom_coeff);
gfloat get_left_drawing_time(gfloat current_time, gfloat total_time, gfloat zoom_coeff);

GtkWidget *create_player_control_frame(ui_state *ui);

void file_chooser_ok_event(const gchar *fname, ui_state *ui);

void change_current_filename(const gchar *fname, ui_state *ui);
GtkWidget *create_player_playlist_frame(ui_state *ui);

void hide_connect_button(gui_state *gui);
void show_connect_button(gui_state *gui);

void restart_player_timer(ui_state *ui);

void compute_douglas_peucker_filters(ui_state *ui);

gint draw_silence_wave(gint left_mark, gint right_mark, 
    gint interpolation_text_x, gint interpolation_text_y,
    gfloat draw_time, gint width_drawing_area, gint y_margin,
    gfloat current_time, gfloat total_time, gfloat zoom_coeff, 
    GtkWidget *da, cairo_t *gc, ui_state *ui);

void get_current_splitpoints_time_left_right(gint *time_left, gint *time_right, 
    gint *splitpoint_left, ui_state *ui);
void player_key_actions_set_sensitivity(gboolean sensitivity, gui_state *gui);
void adjust_zoom_coeff(ui_infos *infos);

void add_playlist_file(const gchar *name, ui_state *ui);

void set_preview_start_position_safe(gint value, ui_state *ui);
gint get_preview_start_position_safe(ui_state *ui);

void set_quick_preview_end_splitpoint_safe(gint value, ui_state *ui);
gint get_quick_preview_end_splitpoint_safe(ui_state *ui);

void set_currently_scanning_for_silence_safe(gint value, ui_state *ui);
gint get_currently_scanning_for_silence_safe(ui_state *ui);

void clear_previous_distances(ui_state *ui);

#endif

