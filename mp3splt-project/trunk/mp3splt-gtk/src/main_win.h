/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2012 Alexandru Munteanu
 * Contact: io_fx@yahoo.fr
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
 * Filename: main_win.h
 *
 * header of main_win.c
 *
 *********************************************************/

#ifndef MAIN_WIN_H

#define MAIN_WIN_H

#include <libmp3splt/mp3splt.h>

#ifdef __WIN32__
#define PIXMAP_PATH ""
#define IMAGEDIR ""
#endif

#define WINDOW_X 700
#define WINDOW_Y 450

enum {
  DROP_PLAIN,
  DROP_STRING,
  DROP_URI_LIST
};

static const GtkTargetEntry drop_types[] = {
  { "text/plain", 0, DROP_PLAIN },
  { "STRING", 0, DROP_STRING },
  { "text/uri-list", 0, DROP_URI_LIST }
};

//close the window and exit button function
void quit(GtkWidget *widget, gpointer data);
void initialize_window();
void about_window(GtkWidget *widget,
                  gpointer *data);
void remove_status_message();
void put_status_message(const gchar *text);
void put_status_message_with_type(const gchar *text,
    splt_message_type mess_type);
void split_button_event();
GtkWidget *create_toolbar();
GtkWidget *create_menu_bar();
GtkWidget *create_cool_button(gchar *stock_id, gchar *label_text,
    gint toggle_or_not);
GtkWidget *create_main_vbox();
void create_all();
void print_status_bar_confirmation(gint confirmation);
void cancel_button_event(GtkWidget *widget, gpointer data);
void put_status_message(const gchar *text);

#endif

