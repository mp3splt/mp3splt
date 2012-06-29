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
 * header of tree_tab.c
 * the tree_tab.c file is used for the Splitpoints tab
 * (for the splitpoints table)
 *********************************************************/

#ifndef TREE_TAB_H

#define TREE_TAB_H

#include "all_includes.h"

void add_splitpoint(Split_point my_split_point, gint old_index, ui_state *ui);
gchar *get_splitpoint_name(gint index, ui_state *ui);
gint get_first_splitpoint_selected(gui_state *gui);
void select_splitpoint(gint index, gui_state *gui);
gint get_splitpoint_time(gint this_splitpoint, ui_state *ui);
void remove_splitpoint(gint index,gint stop_preview, ui_state *ui);
void update_splitpoint(gint index, Split_point new_point, ui_state *ui);
void update_splitpoint_from_time(gint index, gdouble time, ui_state *ui);
void update_splitpoint_check(gint index, ui_state *ui);
void update_add_button(ui_state *ui);
void update_minutes_from_spinner(GtkWidget *widget, ui_state *ui);
void update_seconds_from_spinner(GtkWidget *widget, ui_state *ui);
void update_hundr_secs_from_spinner(GtkWidget *widget, ui_state *ui);
void exchange_elements_from_array(gint element_number1,
                                  gint element_number2);
void exchange_rows_in_the_tree(gint i, gint j, GtkTreeView *tree_view);
void sort_tree(GtkTreeView *tree_view);
void add_splitpoint_from_player(GtkWidget *widget, ui_state *ui);
void add_row(gint checked);
GtkWidget *create_splitpoints_frame(ui_state *ui);
void put_splitpoints_in_mp3splt_state(splt_state *state);

void clear_current_description(ui_state *ui);
void copy_filename_to_current_description(const gchar *fname, ui_state *ui);

//!The enum telling which item is kept in which column of the splitpoint tree
enum {
  /*! if we enable this splitpoint for split */
  COL_CHECK,
  /*! the filename of the split */
  COL_DESCRIPTION,
  /*! minutes*/
  COL_MINUTES,
  /*! seconds*/
  COL_SECONDS,
  /*! hundreths of a second */
  COL_HUNDR_SECS,
  /*! length of the split song */
  COL_NUMBER,
  /*! preview button */
  COL_PREVIEW,
  /*! split preview button */
  COL_SPLIT_PREVIEW,
  NUM_COLUMNS = 8
} tree_columns;

#endif

