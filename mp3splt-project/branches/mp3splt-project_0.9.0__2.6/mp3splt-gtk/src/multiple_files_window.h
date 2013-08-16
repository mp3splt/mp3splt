/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2005-2013 Alexandru Munteanu - m@ioalex.net
 *
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
 * Filename: multiple_files_window.h
 *********************************************************/

#ifndef MULTIPLE_FILES_WINDOW_H

#define MULTIPLE_FILES_WINDOW_H

#include "widgets_helper.h"
#include "ui_types.h"

enum {
  MULTIPLE_COL_FILENAME,
  MULTIPLE_FILES_COLUMNS
};

GtkWidget *create_multiple_files_component(ui_state *ui);
void batch_file_mode_split_button_event(GtkWidget *widget, ui_state *ui);
void multiple_files_add_button_event(GtkWidget *widget, ui_state *ui);
void multiple_files_add_filename(const gchar *filename, ui_state *ui);

#endif

