/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2005-2012 Alexandru Munteanu - io_fx@yahoo.fr
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
 * Filename: player_tab.h
 *
 * header of player_tab.c
 *
 *********************************************************/

#ifndef PLAYER_TAB_H

#define PLAYER_TAB_H

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
void set_preview_active_if_needed();
void cancel_quick_preview_all(gui_status *status);
void cancel_quick_preview(gui_status *status);
void enable_player_buttons();
void connect_to_player_with_song(gint i, ui_state *ui);
void connect_button_event(GtkWidget *widget, ui_state *ui);
void disconnect_button_event(GtkWidget *widget, ui_state *ui);
void pause_event(GtkWidget *widget, ui_state *ui);
void refresh_drawing_area(gui_state *gui);
void change_volume_event(GtkScaleButton *volume_button, gdouble value, gpointer data);

gboolean volume_button_unclick_event (GtkWidget *widget,
                                   GdkEventCrossing *event,
                                   gpointer user_data);
gboolean volume_button_click_event (GtkWidget *widget,
                                 GdkEventCrossing *event,
                                 gpointer user_data);
gboolean volume_button_enter_event (GtkWidget *widget,
                                 GdkEventCrossing *event,
                                 gpointer user_data);
gboolean volume_button_leave_event (GtkWidget *widget,
                                 GdkEventCrossing *event,
                                 gpointer user_data);
gboolean volume_button_scroll_event (GtkWidget *widget,
                                  GdkEventScroll *event,
                                  gpointer user_data);
GtkWidget *create_volume_control_box();
gfloat get_right_drawing_time(gfloat current_time, gfloat total_time, gfloat zoom_coeff);
gfloat get_left_drawing_time(gfloat current_time, gfloat total_time, gfloat zoom_coeff);

gfloat pixels_to_time(gfloat width,
                      gint pixels);
gint convert_time_to_pixels(gint width, gfloat time, 
    gfloat current_time, gfloat total_time, gfloat zoom_coeff);
gboolean da_expose_event (GtkWidget      *da,
                          GdkEventExpose *event,
                          gpointer       data);
GtkWidget *create_player_control_frame(ui_state *ui);

//moved from the file_tab
void file_chooser_cancel_event();
void file_chooser_ok_event(gchar *fname, ui_state *ui);
void close_file_popup_window_event( GtkWidget *window,
                                    gpointer data );

void change_current_filename(const gchar *fname, ui_state *ui);
GtkWidget *create_player_playlist_frame(ui_state *ui);

void hide_connect_button(gui_state *gui);
void show_connect_button(gui_state *gui);

void restart_player_timer();

void compute_douglas_peucker_filters(ui_state *ui);

gint draw_silence_wave(gint left_mark, gint right_mark, 
    gint interpolation_text_x, gint interpolation_text_y,
    gfloat draw_time, gint width_drawing_area, gint y_margin,
    gfloat current_time, gfloat total_time, gfloat zoom_coeff, 
    GtkWidget *da, cairo_t *gc);

void get_current_splitpoints_time_left_right(gint *time_left, gint *time_right, 
    gint *splitpoint_left, ui_infos *infos);
void player_key_actions_set_sensitivity(gboolean sensitivity, gui_state *gui);
void adjust_zoom_coeff(ui_infos *infos);

#endif

