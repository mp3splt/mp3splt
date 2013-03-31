/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2013 Alexandru Munteanu
 * Contact: m@ioalex.net
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
 * Filename: main_window.h
 *
 * header of main_window.c
 *
 *********************************************************/

#ifndef MAIN_WINDOW_H

#define MAIN_WINDOW_H

#include "all_includes.h"

#ifdef __WIN32__
#define PIXMAP_PATH ""
#define IMAGEDIR ""
#endif

#ifndef GDK_Left
#define GDK_Left GDK_KEY_Left
#endif

#ifndef GDK_Right
#define GDK_Right GDK_KEY_Right
#endif

void remove_status_message(gui_state *gui);
void put_status_message(const gchar *text, ui_state *ui);
void put_status_message_with_type(const gchar *text,
    splt_message_type mess_type, ui_state *ui);
void create_application();
void print_status_bar_confirmation(gint error, ui_state *ui);
void cancel_button_event(GtkWidget *widget, ui_state *ui);

gchar* get_input_filename(gui_state *gui);
void set_input_filename(const gchar *filename, ui_state *ui);
void add_filters_to_file_chooser(GtkWidget *file_chooser);

void split_button_event(GtkWidget *widget, ui_state *ui);

void print_status_bar_confirmation_in_idle(gint error, ui_state *ui);

void set_stop_split_safe(gboolean value, ui_state *ui);

gint get_is_splitting_safe(ui_state *ui);
void set_is_splitting_safe(gboolean value, ui_state *ui);

void set_split_file_mode_safe(gint file_mode, ui_state *ui);
gint get_split_file_mode_safe(ui_state *ui);

void set_process_in_progress_and_wait_safe(gboolean value, ui_state *ui);
void set_process_in_progress_safe(gboolean value, ui_state *ui);
gint get_process_in_progress_safe(ui_state *ui);

#endif

