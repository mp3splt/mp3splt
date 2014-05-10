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
 * Filename: preferences_window.h
 *
 * heder of preferences_window.c
 *********************************************************/

#ifndef PREFERENCES_WINDOW_H

#define PREFERENCES_WINDOW_H

#include "all_includes.h"

#define NUMBER_OF_TAGS_MODES 4

#define NO_TAGS 0
#define DEFAULT_TAGS 1
#define ORIGINAL_FILE_TAGS 2
#define TAGS_FROM_FILENAME 3

#define CUSTOM_DIRECTORY 0
#define FILE_DIRECTORY 1

gboolean get_checked_output_radio_box(ui_state *ui);
gint get_checked_tags_version_radio_box(gui_state *gui);
GString *get_checked_language(ui_state *ui);
GtkWidget *create_choose_preferences(ui_state *ui);
void set_output_directory_and_update_ui(gchar *dirname, ui_state *ui);
gchar *get_output_directory(ui_state *ui);
void refresh_preview_drawing_areas(gui_state *gui);
void update_example_output_dir_for_single_file_split(ui_state *ui);

#endif

