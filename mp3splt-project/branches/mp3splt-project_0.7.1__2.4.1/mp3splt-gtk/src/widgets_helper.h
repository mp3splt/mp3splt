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

#ifndef WIDGETS_HELPER_H

#include <gtk/gtk.h>

#include "ui_manager.h"

GtkWidget *wh_set_title_and_get_vbox(GtkWidget *widget, const gchar *title);

GtkWidget *wh_put_in_new_hbox_with_margin_level(GtkWidget *widget, gint margin_level);

GtkWidget *wh_new_table();
void wh_add_in_table(GtkWidget *table, GtkWidget *widget);
void wh_add_in_table_with_label(GtkWidget *table, const gchar *label_text,
    GtkWidget *widget);
void wh_add_in_table_with_label_expand(GtkWidget *table, const gchar *label_text,
    GtkWidget *widget);

GtkWidget *wh_new_entry(gpointer callback);

GtkWidget *wh_new_button(const gchar *button_label);

void wh_get_widget_size(GtkWidget *widget, gint *width, gint *height);

void wh_set_browser_directory_handler(ui_state *ui, GtkWidget* dialog);

#define WIDGETS_HELPER_H
#endif

