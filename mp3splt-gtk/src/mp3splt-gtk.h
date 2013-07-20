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
 * Filename: mp3splt-gtk.h
 *
 * header of mp3splt-gtk.c
 *
 *********************************************************/

#ifndef MP3SPLT_GTK_H
#define MP3SPLT_GTK_H

#include "all_includes.h"

void split_action(ui_state *ui);
GThread *create_thread(GThreadFunc func, ui_state *ui);
GThread *create_thread_with_fname(GThreadFunc func, ui_with_fname *ui_fname);
void enter_threads();
void exit_threads();
gboolean exit_application(GtkWidget *widget, GdkEvent  *event, gpointer data);
void exit_application_bis(GtkWidget *widget, gpointer data);

#define MP3SPLT_GTK_DATE "20/07/13"

#endif

