/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2006 Munteanu Alexandru
 * Contact: io_alex_2002@yahoo.fr
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 *
 *********************************************************/

/**********************************************************
 * Filename: cddb_cue.h
 *
 * header of cue_cue.c - for the file_tab.c file
 *
 *********************************************************/

void cue_file_chooser_ok_event(gchar *fname);
void cddb_file_chooser_ok_event(gchar *fname);
void close_cddb_cue_popup_window_event( GtkWidget *window,
                                        gpointer data );
void handle_cddb_cue_detached_event (GtkHandleBox *handlebox,
                                     GtkWidget *widget,
                                     gpointer data);
GtkWidget *add_cddb_splitpoints();
GtkWidget *add_cue_splitpoints();
GtkWidget *create_cddb_cue_frame();
