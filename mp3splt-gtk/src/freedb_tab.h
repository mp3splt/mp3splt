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
 * Filename: cddb_tab.h
 *
 * this file is the header of cddb_tab.c
 *
 *********************************************************/

#ifndef FREEDB_TAB_H

#define FREEDB_TAB_H

#include "all_includes.h"

void add_freedb_row(gchar *album_name, 
                    gint album_id,
                    gint *revisions,
                    gint revisions_number);
GtkTreeModel *create_freedb_model();
GtkTreeView *create_freedb_tree();
void create_freedb_columns (GtkTreeView *freedb_tree);
void freedb_selection_changed(GtkTreeSelection *selection,
                              gpointer data);
void remove_all_freedb_rows();
gchar *transform_to_utf8(gchar *text, 
                         gint free_or_not,
                         gint *must_be_freed);
gint write_freedbfile_and_get_splitpoints(int *err);
void get_secs_mins_hundr(gfloat time,
                         gint *mins,gint *secs, 
                         gint *hundr);
void update_splitpoints_from_mp3splt_state();
void freedb_add_button_clicked_event(GtkButton *button,
                                     gpointer data);
GtkWidget *create_freedb_frame(ui_state *ui);

void hide_freedb_spinner();

#endif

