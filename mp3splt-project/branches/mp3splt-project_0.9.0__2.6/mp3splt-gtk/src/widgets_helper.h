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

#ifndef WIDGETS_HELPER_H

#define WIDGETS_HELPER_H

#include "ui_manager.h"

GtkWidget *wh_set_title_and_get_vbox(GtkWidget *widget, const gchar *title);

void wh_put_in_hbox_and_attach_to_vbox(GtkWidget *widget, GtkWidget *vbox, gint vertical_margin);
GtkWidget *wh_put_in_new_hbox_with_margin_level(GtkWidget *widget, gint margin_level);
void wh_put_in_hbox_and_attach_to_vbox_with_bottom_margin(GtkWidget *widget, GtkWidget *vbox,
    gint vertical_margin, gint bottom_margin);

GtkWidget *wh_new_table();
void wh_add_in_table(GtkWidget *table, GtkWidget *widget);
void wh_add_in_table_with_label(GtkWidget *table, const gchar *label_text,
    GtkWidget *widget);
void wh_add_in_table_with_label_expand(GtkWidget *table, const gchar *label_text,
    GtkWidget *widget);

GtkWidget *wh_new_entry(gpointer callback, ui_state *ui);

GtkWidget *wh_new_button(const gchar *button_label);

void wh_get_widget_size(GtkWidget *widget, gint *width, gint *height);

void wh_set_browser_directory_handler(ui_state *ui, GtkWidget* dialog);

GtkWidget *wh_create_int_spinner_in_box(gchar *before_label, gchar *after_label,
    gdouble initial_value,
    gdouble minimum_value, gdouble maximum_value, 
    gdouble step_increment, gdouble page_increment,
    gchar *after_newline_label, 
    void (*spinner_callback)(GtkWidget *spinner, ui_state *ui),
    ui_state *ui,
    GtkWidget *box);

GtkWidget *wh_create_int_spinner_in_box_with_top_width(gchar *before_label, gchar *after_label,
    gdouble initial_value,
    gdouble minimum_value, gdouble maximum_value, 
    gdouble step_increment, gdouble page_increment,
    gchar *after_newline_label, 
    void (*spinner_callback)(GtkWidget *spinner, ui_state *ui),
    ui_state *ui,
    GtkWidget *box, gint top_width);

GtkWidget *wh_hbox_new();
GtkWidget *wh_vbox_new();
GtkWidget *wh_hscale_new(GtkAdjustment *adjustment);
GtkWidget *wh_hscale_new_with_range(gdouble min, gdouble max, gdouble step);
void wh_get_pointer(GdkEventMotion *event, gint *x, gint *y, GdkModifierType *state);

GtkWidget *wh_create_scrolled_window();

gboolean wh_container_has_child(GtkContainer *cont, GtkWidget *my_child);

GtkWidget *wh_create_cool_button(gchar *stock_id, gchar *label_text,
    gint toggle_or_not);
GtkWidget *wh_create_cool_label(gchar *stock_id, gchar *label_text);

void wh_set_image_on_button(GtkButton *button, GtkWidget *image);

GtkWidget *wh_create_window_with_close_button(gchar *title, gint width, gint height,
    GtkWindowPosition position, GtkWindow *parent_window,
    GtkWidget *main_area_widget, GtkWidget *bottom_widget, ...);
void wh_show_window(GtkWidget *window);

#endif

