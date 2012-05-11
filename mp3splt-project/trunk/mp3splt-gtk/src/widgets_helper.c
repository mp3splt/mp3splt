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
  GtkWidget *vbox = gtk_vbox_new(FALSE, 0);

  GtkWidget *label = gtk_label_new(NULL);
  gtk_label_set_markup(GTK_LABEL(label), title);

  GtkWidget *label_hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(label_hbox), label, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), label_hbox, FALSE, FALSE, 5);

  GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
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
  GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
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
    hbox = gtk_hbox_new(FALSE, 0);
    gtk_box_pack_start(GTK_BOX(hbox), widget, FALSE, FALSE, 0);
    my_widget = hbox;
  }

  gtk_table_attach(GTK_TABLE(table), my_widget,
      start_column, end_column, row-1, row,
      xoptions, GTK_FILL | GTK_EXPAND,
      0, 0);
}

