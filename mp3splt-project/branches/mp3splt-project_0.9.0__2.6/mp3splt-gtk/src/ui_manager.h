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

#ifndef UI_MANAGER_H

#define UI_MANAGER_H

#include "external_includes.h"
#include "ui_types.h"
#include "player_window.h"

#define UI_DEFAULT_WIDTH 650
#define UI_DEFAULT_HEIGHT 420

ui_state *ui_state_new();
void ui_state_free(ui_state *ui);

void ui_set_browser_directory(ui_state *ui, const gchar *directory);
const gchar *ui_get_browser_directory(ui_state *ui);

void ui_set_main_win_position(ui_state *ui, gint x, gint y);
void ui_set_main_win_size(ui_state *ui, gint width, gint height);
const ui_main_window *ui_get_main_window_infos(ui_state *ui);

void ui_register_spinner_int_preference(gchar *main_key, gchar *second_key,
    gint default_value, GtkWidget *spinner,
    void (*update_spinner_value_cb)(GtkWidget *spinner, gpointer data),
    gpointer user_data_for_cb, ui_state *ui);

void ui_register_range_preference(gchar *main_key, gchar *second_key,
    gint default_value, GtkWidget *range,
    void (*update_adjustment_value)(GtkAdjustment *adjustment, gpointer user_data),
    gpointer user_data_for_cb, ui_state *ui);

void ui_load_preferences(ui_state *ui);
void ui_save_preferences(GtkWidget *dummy_widget, ui_state *ui);

void ui_fail(ui_state *ui, const gchar *message, ...);

#endif

