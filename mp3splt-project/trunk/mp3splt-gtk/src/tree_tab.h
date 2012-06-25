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

void add_splitpoint(Split_point my_split_point, gint old_index);
gchar *get_splitpoint_name(gint index);
gint get_first_splitpoint_selected();
void select_splitpoint(gint index);
gint get_splitpoint_time(gint this_splitpoint);
void remove_splitpoint(gint index,gint stop_preview);
void update_splitpoint(gint index, Split_point new_point);
void update_splitpoint_from_time(gint index, gdouble time);
void update_splitpoint_check(gint index);
void update_add_button();
void update_minutes_from_spinner(GtkWidget *widget, gpointer data);
void update_seconds_from_spinner(GtkWidget *widget, gpointer data);
void update_hundr_secs_from_spinner(GtkWidget *widget, gpointer data);
GtkTreeModel *create_model();
gboolean check_if_description_exists(gchar *descr,
                                     gint number);
void exchange_elements_from_array(gint element_number1,
                                  gint element_number2);
void row_selection_event();
void exchange_rows_in_the_tree(gint i, gint j, GtkTreeView *tree_view);
void sort_tree(GtkTreeView *tree_view);
void update_current_description(gchar *descr, gint number);
void add_splitpoint_from_player(GtkWidget *widget, gpointer data);
void add_row(gint checked);
GtkWidget *create_init_spinner(GtkWidget *bottomhbox1, 
                               gint min, 
                               gint max, 
                               gchar *label_text,
                               gint type);
void preview_song (GtkTreeView *tree_view,
                   GtkTreePath *path,
                   GtkTreeViewColumn *col,
                   gpointer user_data);
void create_columns (GtkTreeView *tree_view);
void close_popup_window_event( GtkWidget *window,
                               gpointer data );
void handle_detached_event (GtkHandleBox *handlebox,
                            GtkWidget *widget,
                            gpointer data);
GtkWidget *create_splitpoints_frame(ui_state *ui);
void put_splitpoints_in_mp3splt_state(splt_state *state);

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

