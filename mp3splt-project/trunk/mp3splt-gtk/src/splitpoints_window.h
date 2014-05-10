/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2005-2014 Alexandru Munteanu - m@ioalex.net
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
 *
 * header of splitpoints_window.c
 *********************************************************/

#ifndef SPLITPOINTS_WINDOW_H

#define SPLITPOINTS_WINDOW_H

#include "all_includes.h"

//!The enum telling which item is kept in which column of the splitpoint tree
enum {
  COL_CHECK = 0,
  COL_DESCRIPTION = 1,
  COL_MINUTES = 2,
  COL_SECONDS = 3,
  COL_HUNDR_SECS = 4,
  COL_NUMBER = 5,
  COL_PREVIEW = 6,
  COL_SPLIT_PREVIEW = 7,
  //tags
  COL_TITLE = 8,
  COL_ARTIST = 9,
  COL_ALBUM = 10,
  COL_GENRE = 11,
  COL_YEAR = 12,
  COL_TRACK = 13,
  COL_COMMENT = 14,
  NUM_COLUMNS = 15
} tree_columns;

gchar *get_splitpoint_name(gint index, ui_state *ui);
gint get_first_splitpoint_selected(gui_state *gui);
void select_splitpoint(gint index, gui_state *gui);
gint get_splitpoint_time(gint this_splitpoint, ui_state *ui);
void remove_splitpoint(gint index,gint stop_preview, ui_state *ui);
void update_splitpoint(gint index, Split_point new_point, ui_state *ui);
void update_splitpoint_from_time(gint index, gdouble time, ui_state *ui);
void update_splitpoint_check(gint index, ui_state *ui);
void update_minutes_from_spinner(GtkWidget *widget, ui_state *ui);
void update_seconds_from_spinner(GtkWidget *widget, ui_state *ui);
void update_hundr_secs_from_spinner(GtkWidget *widget, ui_state *ui);
void add_splitpoint_from_player(GtkWidget *widget, ui_state *ui);
void add_row(gint checked, ui_state *ui);
GtkWidget *create_splitpoints_frame(ui_state *ui);
points_and_tags *get_splitpoints_and_tags_for_mp3splt_state(ui_state *ui);
void clear_current_description(ui_state *ui);
void copy_filename_to_current_description(const gchar *fname, ui_state *ui);
void remove_all_rows(GtkWidget *widget, ui_state *ui);

void create_detect_silence_and_add_splitpoints_window(GtkWidget *button, ui_state *ui);
void create_trim_silence_window(GtkWidget *button, ui_state *ui);

#endif

