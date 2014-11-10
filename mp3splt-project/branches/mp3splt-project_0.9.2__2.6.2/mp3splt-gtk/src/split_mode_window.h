/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2005-2014 Alexandru Munteanu - m@ioalex.net
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
 * along with this program; if not, write to the Free Software
 * You should have received a copy of the GNU General Public License
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 *
 *********************************************************/

#ifndef SPLIT_MODE_WINDOW_H

#define SPLIT_MODE_WINDOW_H

#include "all_includes.h"

#define FILE_MODE_SINGLE 1
#define FILE_MODE_MULTIPLE 0

#define SELECTED_SPLIT_INTERNAL_SHEET 9
#define SELECTED_SPLIT_CUE_FILE 8
#define SELECTED_SPLIT_CDDB_FILE 7
#define SELECTED_SPLIT_TRIM_SILENCE 6
#define SELECTED_SPLIT_SILENCE 5
#define SELECTED_SPLIT_EQUAL_TIME_TRACKS 4
#define SELECTED_SPLIT_NORMAL 3
#define SELECTED_SPLIT_TIME 2
#define SELECTED_SPLIT_WRAP 1
#define SELECTED_SPLIT_ERROR 0

#define NUMBER_OF_SPLIT_MODES 9

void select_split_mode(int split_mode, ui_state *ui);
GtkWidget *create_special_split_page(ui_state *ui);

gint get_selected_split_mode(ui_state *ui);
void set_selected_split_mode(gint value, ui_state *ui);

#endif

