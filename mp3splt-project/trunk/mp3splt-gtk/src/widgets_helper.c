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

/*!********************************************************
 * \file 
 *
 * this file contains the code for the widgets helpers.
 ********************************************************/

#include "widgets_helper.h"

static guint _wh_add_row_to_table();
static GtkWidget *_wh_put_in_new_hbox_with_margin(GtkWidget *widget, gint margin);
static void _wh_attach_to_table(GtkWidget *table, GtkWidget *widget,
    guint start_column, guint end_column, guint row, int expand);
static void _wh_add_in_table_with_label(GtkWidget *table, const gchar *label_text,
    GtkWidget *widget, int expand);

/*! Generates a window portion containing a caption and a vbox

This function works like a chapter heading in a text processing
system: You specify a title and the text processor returns an object
consisting of a title and a vbox you can put the chapter contents
in.\n
Is used to generate the consecutive named sections in a preferences
tab.
 */
GtkWidget *wh_set_title_and_get_vbox(GtkWidget *widget, const gchar *title)
{
  GtkWidget *vbox = wh_vbox_new();

  GtkWidget *label = gtk_label_new(NULL);
  gtk_label_set_markup(GTK_LABEL(label), title);

  GtkWidget *label_hbox = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(label_hbox), label, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), label_hbox, FALSE, FALSE, 5);

  GtkWidget *hbox = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(hbox), widget, TRUE, TRUE, 16);

  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

  return vbox;
}

GtkWidget *wh_new_table()
{
  GtkWidget *table = gtk_table_new(1, 2, FALSE);
  gtk_table_set_col_spacing(GTK_TABLE(table), 0, 0);
  gtk_table_set_col_spacing(GTK_TABLE(table), 1, 5);
  return table;
}

void wh_add_in_table(GtkWidget *table, GtkWidget *widget)
{
  guint last_row = _wh_add_row_to_table(table);

  _wh_attach_to_table(table, widget, 1, 3, last_row, TRUE);
}

void wh_add_in_table_with_label_expand(GtkWidget *table, const gchar *label_text, GtkWidget *widget)
{
  _wh_add_in_table_with_label(table, label_text, widget, TRUE);
}

void wh_add_in_table_with_label(GtkWidget *table, const gchar *label_text, GtkWidget *widget)
{
  _wh_add_in_table_with_label(table, label_text, widget, FALSE);
}

GtkWidget *wh_put_in_new_hbox_with_margin_level(GtkWidget *widget, gint margin_level)
{
  return _wh_put_in_new_hbox_with_margin(widget, 6 * margin_level);
}

GtkWidget *wh_new_entry(void *callback)
{
  GtkWidget *entry = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(entry), TRUE);
  
  if (callback)
  {
    g_signal_connect(G_OBJECT(entry), "changed", G_CALLBACK(callback), NULL);
  }

  return entry;
}

GtkWidget *wh_new_button(const gchar *button_label)
{
  return gtk_button_new_with_mnemonic(button_label);
}

void wh_get_widget_size(GtkWidget *widget, gint *width, gint *height)
{
#if GTK_MAJOR_VERSION <= 2
  GtkAllocation allocation;
  gtk_widget_get_allocation(widget, &allocation);

  if (width != NULL)
  {
    *width = allocation.width;
  }

  if (height != NULL)
  {
    *height= allocation.height;
  }
#else
  if (width != NULL)
  {
    *width = gtk_widget_get_allocated_width(widget);
  }

  if (height != NULL)
  {
    *height = gtk_widget_get_allocated_height(widget);
  }
#endif
}

GtkWidget *wh_create_int_spinner_in_box_with_top_width(gchar *before_label, gchar *after_label,
    gdouble initial_value,
    gdouble minimum_value, gdouble maximum_value, 
    gdouble step_increment, gdouble page_increment,
    gchar *after_newline_label, 
    void (*spinner_callback)(GtkWidget *spinner, gpointer data),
    gpointer user_data_for_cb,
    GtkWidget *box, gint top_width)
{
  GtkWidget *horiz_fake = wh_hbox_new();
  GtkWidget *label = gtk_label_new(before_label);
  gtk_box_pack_start(GTK_BOX(horiz_fake), label, FALSE, FALSE, 0);

  GtkAdjustment *adj = (GtkAdjustment *) 
    gtk_adjustment_new(initial_value, minimum_value, maximum_value, step_increment, page_increment, 0.0);

  GtkWidget *spinner = gtk_spin_button_new(adj, 0, 0);
 
  gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(spinner), TRUE);
  g_signal_connect(G_OBJECT(spinner), "value_changed",
      G_CALLBACK(spinner_callback), user_data_for_cb);
  gtk_box_pack_start(GTK_BOX(horiz_fake), spinner, FALSE, FALSE, 5);

  if (after_label != NULL)
  {
    gtk_box_pack_start(GTK_BOX(horiz_fake), gtk_label_new(after_label), FALSE, FALSE, 3);
  }

  GtkWidget *fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(box), fake, FALSE, FALSE, top_width);

  gtk_box_pack_start(GTK_BOX(box), horiz_fake, FALSE, FALSE, 1);

  if (after_newline_label != NULL)
  {
    horiz_fake = wh_hbox_new();
    gtk_box_pack_start(GTK_BOX(horiz_fake), gtk_label_new(after_newline_label), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), horiz_fake, FALSE, FALSE, 2);
  }

  fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(box), fake, FALSE, FALSE, 2);

  return spinner;
}

GtkWidget *wh_create_int_spinner_in_box(gchar *before_label, gchar *after_label,
    gdouble initial_value,
    gdouble minimum_value, gdouble maximum_value, 
    gdouble step_increment, gdouble page_increment,
    gchar *after_newline_label, 
    void (*spinner_callback)(GtkWidget *spinner, gpointer data),
    gpointer user_data_for_cb,
    GtkWidget *box)
{
  return wh_create_int_spinner_in_box_with_top_width(before_label, after_label,
      initial_value, minimum_value, maximum_value, step_increment, page_increment,
      after_newline_label, spinner_callback, user_data_for_cb, box, 2);
}

GtkWidget *wh_hbox_new()
{
#if GTK_MAJOR_VERSION <= 2
  return gtk_hbox_new(FALSE, 0);
#else
  GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_set_homogeneous(GTK_BOX(hbox), FALSE);
  return hbox;
#endif
}

GtkWidget *wh_vbox_new()
{
#if GTK_MAJOR_VERSION <= 2
  return gtk_vbox_new(FALSE, 0);
#else
  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_set_homogeneous(GTK_BOX(vbox), FALSE);
  return vbox;
#endif
}

GtkWidget *wh_hscale_new(GtkAdjustment *adjustment)
{
#if GTK_MAJOR_VERSION <= 2
  return gtk_hscale_new(adjustment);
#else
  return gtk_scale_new(GTK_ORIENTATION_HORIZONTAL, adjustment);
#endif
}

GtkWidget *wh_hscale_new_with_range(gdouble min, gdouble max, gdouble step)
{
#if GTK_MAJOR_VERSION <= 2
  return gtk_hscale_new_with_range(min, max, step);
#else
  return gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, min, max, step);
#endif
}

void wh_get_pointer(GdkEventMotion *event, gint *x, gint *y, GdkModifierType *state)
{
#if GTK_MAJOR_VERSION <= 2
  gdk_window_get_pointer(event->window, x, y, state);
#else
  gdk_window_get_device_position(event->window, event->device, x, y, state);
#endif
}

static void _wh_folder_changed_event(GtkFileChooser *chooser, gpointer data)
{
  ui_state *ui = (ui_state *) data;
  ui_set_browser_directory(ui, gtk_file_chooser_get_current_folder(chooser));
}

void wh_set_browser_directory_handler(ui_state *ui, GtkWidget* dialog)
{
  const gchar *browser_dir = ui_get_browser_directory(ui);
  if (browser_dir)
  {
    gtk_file_chooser_set_current_folder(GTK_FILE_CHOOSER(dialog), browser_dir);
  }

  g_signal_connect(GTK_FILE_CHOOSER(dialog), "current-folder-changed",
      G_CALLBACK(_wh_folder_changed_event), ui);
}

static guint _wh_add_row_to_table(GtkWidget *table)
{
  guint rows;
  guint columns;

  g_object_get(G_OBJECT(table),
      "n-rows", &rows,
      "n-columns", &columns,
      NULL);

  guint new_rows = rows + 1;

  gtk_table_resize(GTK_TABLE(table), new_rows, columns);
  gtk_table_set_row_spacing(GTK_TABLE(table), new_rows - 1, 4);

  return new_rows;
}

static GtkWidget *_wh_put_in_new_hbox_with_margin(GtkWidget *widget, gint margin)
{
  GtkWidget *hbox = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(hbox), widget, TRUE, TRUE, margin);
  return hbox;
}

static void _wh_add_in_table_with_label(GtkWidget *table, const gchar *label_text,
    GtkWidget *widget, int expand)
{
  guint last_row = _wh_add_row_to_table(table);

  GtkWidget *label = gtk_label_new(label_text);
  gtk_misc_set_alignment(GTK_MISC(label), 0.0, 0.5);

  _wh_attach_to_table(table, label, 1, 2, last_row, FALSE);
  _wh_attach_to_table(table, widget, 2, 3, last_row, expand);
}

static void _wh_attach_to_table(GtkWidget *table, GtkWidget *widget,
    guint start_column, guint end_column, guint row, int expand)
{
  GtkWidget *my_widget = widget;
  GtkWidget *hbox;

  GtkAttachOptions xoptions = GTK_FILL;
  if (expand)
  {
    xoptions |= GTK_EXPAND;
  }
  else
  {
    hbox = wh_hbox_new();
    gtk_box_pack_start(GTK_BOX(hbox), widget, FALSE, FALSE, 0);
    my_widget = hbox;
  }

  gtk_table_attach(GTK_TABLE(table), my_widget,
      start_column, end_column, row-1, row,
      xoptions, GTK_FILL | GTK_EXPAND,
      0, 0);
}

