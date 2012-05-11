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
 * Filename: split_files.h
 *
 * header of split_files.c
 *
 *********************************************************/

GtkTreeModel *create_split_model();
GtkTreeView *create_split_files_tree();
void create_split_columns (GtkTreeView *tree_view);
void remove_all_split_rows ();
guchar *get_real_naame(guchar *filename);
void add_split_row(const gchar *name);
void remove_split_selected_row();
void queue_files_button_event( GtkWidget *widget,
                               gpointer   data );
void remove_file_button_event(GtkWidget *widget,
                              gpointer data);
GtkWidget *create_queue_buttons_hbox();
void split_tree_row_activated (GtkTreeView *tree_view,
                                  GtkTreePath *arg1,
                                  GtkTreeViewColumn *arg2,
                                  gpointer data);
void split_selection_changed(GtkTreeSelection *selection,
                                gpointer data);
void close_split_popup_window_event( GtkWidget *window,
                                        gpointer data );
void handle_split_detached_event (GtkHandleBox *handlebox,
                                     GtkWidget *widget,
                                     gpointer data);
GtkWidget *create_split_files();
gchar *get_filename_from_split_files(gint number);
