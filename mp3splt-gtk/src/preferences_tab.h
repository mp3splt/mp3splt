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
 * Filename: preferences_tab.h
 *
 * heder of preferences_tab.c
 *
 *********************************************************/

#ifndef _PREFERENCES_TAB_H
#define _PREFERENCES_TAB_H

#define NUMBER_OF_TAGS_MODES 4

#define NO_TAGS 0
#define DEFAULT_TAGS 1
#define ORIGINAL_FILE_TAGS 2
#define TAGS_FROM_FILENAME 3

GtkWidget *create_scrolled_window();
gboolean get_checked_output_radio_box();
gint get_checked_tags_version_radio_box();
void radio_box_changed_event(GtkToggleButton *radio_b, gpointer data);
GtkWidget *create_pref_general_page();
void browse_dir_button_event(GtkWidget *widget, gpointer data);
void disable_adjust_spinners();
void enables_adjust_spinners();
void adjust_event(GtkToggleButton *adjust_mode, gpointer user_data);
void frame_event(GtkToggleButton *frame_mode, gpointer user_data);
void set_default_prefs_event(GtkWidget *widget, gpointer data);
void song_dir_button_event(GtkWidget *widget, gpointer data);
GtkWidget *create_pref_splitpoints_page();
GString *get_checked_language();
void save_preferences(GtkWidget *widget, gpointer data);
GtkWidget *create_save_buttons_hbox();
void combo_remove_unavailable_players();
void player_combo_box_event(GtkComboBox *widget, gpointer data);
GtkWidget *create_choose_player_combo();
GtkWidget *create_pref_player_page();
GtkWidget *create_choose_preferences();
void outputdirectory_set(gchar *dirname);
gchar *outputdirectory_get();
#endif
