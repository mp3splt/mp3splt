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

/**********************************************************
 * Filename: split_files_window.h
 *
 * header of split_files_window.c
 *********************************************************/

#ifndef SPLIT_FILES_WINDOW_H

#define SPLIT_FILES_WINDOW_H

#include "all_includes.h"

void remove_all_split_rows(ui_state *ui);
void add_split_row(const gchar *name, ui_state *ui);
void split_tree_row_activated(GtkTreeView *split_tree,
    GtkTreePath *arg1, GtkTreeViewColumn *arg2, ui_state *ui);
GtkWidget *create_split_files_frame(ui_state *ui);
gchar *get_filename_from_split_files(gint number, gui_state *gui);
const gchar *get_real_name_from_filename(const gchar *filename);

#endif

