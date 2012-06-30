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
  COL_CHECK,
  COL_DESCRIPTION,
  COL_MINUTES,
  COL_SECONDS,
  COL_HUNDR_SECS,
  COL_NUMBER,
  COL_PREVIEW,
  COL_SPLIT_PREVIEW,
  NUM_COLUMNS = 8
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
void put_splitpoints_in_mp3splt_state(splt_state *state, ui_state *ui);
void clear_current_description(ui_state *ui);
void copy_filename_to_current_description(const gchar *fname, ui_state *ui);

#endif

