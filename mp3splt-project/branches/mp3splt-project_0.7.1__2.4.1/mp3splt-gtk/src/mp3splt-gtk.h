/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2011 Alexandru Munteanu
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
 * Filename: mp3splt-gtk.h
 *
 * header of mp3splt-gtk.c
 *
 *********************************************************/

#ifndef _MP3SPLT_GTK_H
#define _MP3SPLT_GTK_H

void put_options_from_preferences();
gpointer split_it(gpointer data);
void sigpipe_handler(gint sig);

GThread *create_thread(GThreadFunc func, gpointer data,
		gboolean joinable, GError **error);
void enter_threads();
void exit_threads();

#define MP3SPLT_GTK_DATE "14/11/11"

#endif
