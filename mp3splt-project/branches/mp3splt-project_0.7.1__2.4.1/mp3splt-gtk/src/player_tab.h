/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2005-2011 Alexandru Munteanu - io_fx@yahoo.fr
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

#ifndef _PLAYER_TAB_H
#define _PLAYER_TAB_H

#define ICON_EXT ".svg"

//!time structure
typedef struct
{
  long time;
  float level;
} silence_wave;

//float comparison
#define DELTA 5

//for the browse dir
#define BROWSE_SONG 1
#define BROWSE_CDDB_FILE 2
#define BROWSE_CUE_FILE 3

void player_quick_preview(gint splitpoint_to_preview);
void check_update_down_progress_bar();
void set_preview_active_if_needed();
void cancel_quick_preview_all();
void cancel_quick_preview();
void check_cancel_quick_preview(gint i);
void get_splitpoint_time_left_right(gint *time_left,
                                    gint *time_right,
                                    gint *splitpoint_left);
gint mytimer(gpointer data);
void reset_inactive_progress_bar();
void reset_inactive_volume_bar();
void reset_label_time();
void reset_song_infos();
void reset_song_name_label();
void clear_data_player();
void enable_player_buttons();
void disable_player_buttons();
void connect_change_buttons();
void disconnect_change_buttons();
void connect_with_song(const gchar *fname, gint i);
void connect_to_player_with_song(gint i);
void connect_button_event (GtkWidget *widget,
                           gpointer data);
void check_stream();
void disconnect_button_event (GtkWidget *widget, 
                              gpointer data);
void play_event (GtkWidget *widget, gpointer data);
void stop_event (GtkWidget *widget, gpointer data);
void pause_event (GtkWidget *widget, gpointer data);
void prev_button_event (GtkWidget *widget, gpointer data);
void next_button_event (GtkWidget *widget, gpointer data);
void change_song_position();
GtkWidget *create_player_buttons_hbox(GtkTreeView *tree_view);
GtkWidget *create_song_informations_hbox();
gboolean progress_bar_unclick_event (GtkWidget *widget,
                                   GdkEventCrossing *event,
                                     gpointer user_data);
gboolean progress_bar_click_event (GtkWidget *widget,
                                   GdkEventCrossing *event,
                                   gpointer user_data);
gfloat get_total_time();
gfloat get_elapsed_time();
void refresh_drawing_area();
void progress_bar_value_changed_event (GtkRange *range,
                                       gpointer user_data);
gboolean progress_bar_scroll_event (GtkWidget *widget,
                                    GdkEventScroll *event,
                                    gpointer user_data);
gboolean progress_bar_enter_event (GtkWidget *widget,
                                   GdkEventCrossing *event,
                                   gpointer user_data);
gboolean progress_bar_leave_event (GtkWidget *widget,
                                   GdkEventCrossing *event,
                                   gpointer user_data);
GtkWidget *create_song_bar_hbox();
void print_about_the_song();
void print_player_filename();
void print_all_song_infos();
void print_song_time_elapsed();
void change_volume_bar();
void change_progress_bar();
GtkWidget *create_filename_player_hbox();
void change_volume_event(GtkWidget *widget,
                         gpointer data);
gboolean volume_bar_unclick_event (GtkWidget *widget,
                                   GdkEventCrossing *event,
                                   gpointer user_data);
gboolean volume_bar_click_event (GtkWidget *widget,
                                 GdkEventCrossing *event,
                                 gpointer user_data);
gboolean volume_bar_enter_event (GtkWidget *widget,
                                 GdkEventCrossing *event,
                                 gpointer user_data);
gboolean volume_bar_leave_event (GtkWidget *widget,
                                 GdkEventCrossing *event,
                                 gpointer user_data);
gboolean volume_bar_scroll_event (GtkWidget *widget,
                                  GdkEventScroll *event,
                                  gpointer user_data);
GtkWidget *create_volume_control_box();
void close_player_popup_window_event( GtkWidget *window,
                                      gpointer data );
void handle_player_detached_event (GtkHandleBox *handlebox,
                                   GtkWidget *widget,
                                   gpointer data);
PangoLayout *get_drawing_text(gchar *str);
gfloat get_right_drawing_time();
gfloat get_left_drawing_time();
gint get_time_hundrsecs(gint time);
gint get_time_secs(gint time);
gint get_time_mins(gint time);
gchar *get_time_for_drawing(gchar *str,
                            gint time,
                            gboolean hundr_or_not,
                            gint *number_of_chars);
gint time_to_pixels(gint width,
                    gfloat time);
gfloat pixels_to_time(gfloat width,
                      gint pixels);
gint get_draw_line_position(gint width,
                            gfloat time);
void draw_motif(GtkWidget *da,cairo_t *gc,
                gint ylimit,gint x,gint model);
void draw_marks(gint time_interval,
                gint left_mark,gint right_mark,
                gint ylimit, GtkWidget *da, cairo_t *gc);
//motif for splitpoints
//draw is false if we draw the splitpoint we move
void draw_motif_splitpoints(GtkWidget *da, cairo_t *gc,
                            gint x,gint draw,
                            gint current_point_hundr_secs,
                            gboolean move,
                            gint number_splitpoint);
void draw_splitpoints(gint left_mark,
                      gint right_mark,
                      GtkWidget *da,
                      cairo_t *gc);
gboolean da_expose_event (GtkWidget      *da,
                          GdkEventExpose *event,
                          gpointer       data);
gboolean da_press_event (GtkWidget    *da,
                         GdkEventButton *event,
                         gpointer     data);
gboolean da_unpress_event (GtkWidget    *da,
                           GdkEventButton *event,
                           gpointer     data);
gboolean da_notify_event (GtkWidget     *da,
                          GdkEventMotion *event,
                          gpointer      data);
GtkWidget *create_drawing_area();
GtkWidget *create_player_control_frame(GtkTreeView *tree_view);

//moved from the file_tab
void file_chooser_cancel_event();
void file_chooser_ok_event(gchar *fname);
void browse_button_event( GtkWidget *widget,
                          gpointer   data );
void close_file_popup_window_event( GtkWidget *window,
                                    gpointer data );
void handle_file_detached_event (GtkHandleBox *handlebox,
                                 GtkWidget *widget,
                                 gpointer data);
GtkWidget *create_choose_file_frame();

void change_current_filename(const gchar *fname);
GtkWidget *create_player_playlist_frame();

void hide_connect_button();
void show_connect_button();
void hide_disconnect_button();
void show_disconnect_button();

void inputfilename_set(const gchar *filename);
gchar *inputfilename_get();

void build_svg_path(GString *imagefile, gchar *svg_filename);

#endif

