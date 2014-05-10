/**********************************************************
 * mp3splt-gtk -- utility based on mp3splt,
 *
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2014 Alexandru Munteanu
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

/*!********************************************************
 * \file 
 *
 * this file contains the code for the widgets helpers.
 ********************************************************/

#include "widgets_helper.h"

static guint _wh_add_row_to_table();
static void _wh_attach_to_table(GtkWidget *table, GtkWidget *widget,
    guint start_column, guint end_column, guint row, int expand);
static void _wh_add_in_table_with_label(GtkWidget *table, const gchar *label_text,
    GtkWidget *widget, int expand);
static void hide_window_from_button(GtkWidget *window, gpointer data);

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
  GtkWidget *table = gtk_grid_new();
  g_object_set_data(G_OBJECT(table), "rows", GINT_TO_POINTER(0));
  gtk_grid_set_column_spacing(GTK_GRID(table), 5);
  gtk_grid_set_row_spacing(GTK_GRID(table), 4);
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
  return wh_put_in_new_hbox(widget, 6 * margin_level, TRUE, TRUE);
}

GtkWidget *wh_put_in_new_hbox(GtkWidget *widget, gint margin, gboolean expand, gboolean fill)
{
  GtkWidget *hbox = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(hbox), widget, expand, fill, margin);
  return hbox;
}

void wh_put_in_hbox_and_attach_to_vbox(GtkWidget *widget, GtkWidget *vbox, gint vertical_margin)
{
  wh_put_in_hbox_and_attach_to_vbox_with_bottom_margin(widget, vbox, vertical_margin, -1);
}

void wh_put_in_hbox_and_attach_to_vbox_with_bottom_margin(GtkWidget *widget, GtkWidget *vbox,
    gint vertical_margin, gint bottom_margin)
{
  GtkWidget *hbox = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(hbox), widget, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, vertical_margin);

  if (bottom_margin > 0)
  {
    GtkWidget *fake_hbox = wh_hbox_new();
    gtk_box_pack_start(GTK_BOX(vbox), fake_hbox, FALSE, FALSE, bottom_margin);
  }
}

GtkWidget *wh_new_entry(gpointer callback, ui_state *ui)
{
  GtkWidget *entry = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(entry), TRUE);
  
  if (callback)
  {
    g_signal_connect(G_OBJECT(entry), "changed", G_CALLBACK(callback), ui);
  }

  return entry;
}

GtkWidget *wh_new_button(const gchar *button_label)
{
  return gtk_button_new_with_mnemonic(button_label);
}

void wh_get_widget_size(GtkWidget *widget, gint *width, gint *height)
{
  if (width != NULL)
  {
    *width = gtk_widget_get_allocated_width(widget);
  }

  if (height != NULL)
  {
    *height = gtk_widget_get_allocated_height(widget);
  }
}

GtkWidget *wh_create_int_spinner_in_box_with_top_width(gchar *before_label, gchar *after_label,
    gdouble initial_value, gdouble minimum_value, gdouble maximum_value, 
    gdouble step_increment, gdouble page_increment,
    gchar *after_newline_label, 
    void (*spinner_callback)(GtkWidget *spinner, ui_state *ui),
    ui_state *ui, GtkWidget *box, gint top_width)
{
  GtkWidget *horiz_fake = wh_hbox_new();
  GtkWidget *label = gtk_label_new(before_label);
  gtk_box_pack_start(GTK_BOX(horiz_fake), label, FALSE, FALSE, 0);

  GtkAdjustment *adj = (GtkAdjustment *) 
    gtk_adjustment_new(initial_value, minimum_value, maximum_value, step_increment, page_increment, 0.0);

  GtkWidget *spinner = gtk_spin_button_new(adj, 0, 0);
 
  gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(spinner), TRUE);
  g_signal_connect(G_OBJECT(spinner), "value_changed", G_CALLBACK(spinner_callback), ui);
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
    void (*spinner_callback)(GtkWidget *spinner, ui_state *ui),
    ui_state *ui,
    GtkWidget *box)
{
  return wh_create_int_spinner_in_box_with_top_width(before_label, after_label,
      initial_value, minimum_value, maximum_value, step_increment, page_increment,
      after_newline_label, spinner_callback, ui, box, 2);
}

GtkWidget *wh_hbox_new()
{
  GtkWidget *hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_box_set_homogeneous(GTK_BOX(hbox), FALSE);
  return hbox;
}

GtkWidget *wh_vbox_new()
{
  GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_box_set_homogeneous(GTK_BOX(vbox), FALSE);
  return vbox;
}

GtkWidget *wh_hscale_new(GtkAdjustment *adjustment)
{
  return gtk_scale_new(GTK_ORIENTATION_HORIZONTAL, adjustment);
}

GtkWidget *wh_hscale_new_with_range(gdouble min, gdouble max, gdouble step)
{
  return gtk_scale_new_with_range(GTK_ORIENTATION_HORIZONTAL, min, max, step);
}

void wh_get_pointer(GdkEventMotion *event, gint *x, gint *y, GdkModifierType *state)
{
  gdk_window_get_device_position(event->window, event->device, x, y, state); 
}

//!creates a scrolled window
GtkWidget *wh_create_scrolled_window()
{
  GtkWidget *scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_NONE);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
      GTK_POLICY_AUTOMATIC,
      GTK_POLICY_AUTOMATIC);
  return scrolled_window;
}

void wh_add_box_to_scrolled_window(GtkWidget *box, GtkWidget *scrolled_window)
{
  GtkWidget *viewport = gtk_viewport_new(NULL, NULL);
  gtk_container_add(GTK_CONTAINER(viewport), box);
  gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(viewport));
}

/*! Does this GtkContainer contain that object?

\param GtkContainer The Container that has to be searched for the
child object.
\param my_child The child that has to be searched for.
 */
gboolean wh_container_has_child(GtkContainer *container, GtkWidget *my_child)
{
  GList *children = gtk_container_get_children(GTK_CONTAINER(container));
  int i = 0;
  GtkWidget *child = NULL;
  while ((child = g_list_nth_data(children, i)) != NULL)
  {
    if (child == my_child)
    {
      return TRUE;
    }
    i++;
  }

  return FALSE;
}

void wh_set_image_on_button(GtkButton *button, GtkWidget *image)
{
  GtkImage *previous_image = GTK_IMAGE(gtk_button_get_image(button));
  if (previous_image != NULL && previous_image == GTK_IMAGE(image)) { return; }
  gtk_button_set_image(button, image);
}

static void _wh_folder_changed_event(GtkFileChooser *chooser, ui_state *ui)
{
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

GtkWidget *wh_create_cool_button(gchar *icon_name, gchar *label_text,
    gint toggle_or_not)
{
  GtkWidget *box = wh_hbox_new();
  gtk_container_set_border_width(GTK_CONTAINER(box), 0);

  if (icon_name != NULL)
  {
    GtkWidget *image = gtk_image_new_from_icon_name(icon_name, GTK_ICON_SIZE_MENU);
    gtk_box_pack_start(GTK_BOX(box), image, FALSE, FALSE, 0);
  }

  if (label_text != NULL)
  {
    GtkWidget *label = gtk_label_new(label_text);
    gtk_label_set_text_with_mnemonic(GTK_LABEL(label),label_text);
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 3);
  }

  GtkWidget *button;
  if (toggle_or_not)
  {
    button = gtk_toggle_button_new();
  }
  else
  {
    button = gtk_button_new();
  }

  gtk_container_add(GTK_CONTAINER(button), box);

  return button;
}

GtkWidget *wh_create_cool_label(gchar *icon_name, gchar *label_text)
{
  GtkWidget *box = wh_hbox_new();
  gtk_container_set_border_width(GTK_CONTAINER(box), 0);

  if (icon_name != NULL)
  {
    GtkWidget *image = gtk_image_new_from_icon_name(icon_name, GTK_ICON_SIZE_MENU);
    gtk_box_pack_start(GTK_BOX(box), image, FALSE, FALSE, 0);
  }

  if (label_text != NULL)
  {
    GtkWidget *label = gtk_label_new(label_text);
    gtk_label_set_text_with_mnemonic(GTK_LABEL(label),label_text);
    gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 3);
  }

  gtk_widget_show_all(box);

  return box;
}

GtkWidget *wh_create_window_with_close_button(gchar *title, gint width, gint height,
    GtkWindowPosition position, GtkWindow *parent_window,
    GtkWidget *main_area_widget, GtkWidget *bottom_widget, ...)
{
  GtkWidget *window = gtk_dialog_new();

  g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(gtk_widget_hide_on_delete), window);
  gtk_window_set_title(GTK_WINDOW(window), title);
  gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);
  gtk_window_set_default_size(GTK_WINDOW(window), width, height);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

  GtkWidget *vbox = wh_vbox_new();

  GtkContainer *dialog_container = GTK_CONTAINER(gtk_dialog_get_content_area(GTK_DIALOG(window)));
  gtk_box_pack_start(GTK_BOX(dialog_container), vbox, TRUE, TRUE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), main_area_widget, TRUE, TRUE, 2);

  GtkWidget *bottom_hbox = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(vbox), bottom_hbox, FALSE, FALSE, 3);

  va_list ap;
  va_start(ap, bottom_widget);
  while (bottom_widget)
  {
    gtk_box_pack_start(GTK_BOX(bottom_hbox), bottom_widget, FALSE, FALSE, 3);
    bottom_widget = va_arg(ap, GtkWidget *);
  }
  va_end(ap);

  GtkWidget *close_button = wh_create_cool_button("window-close", _("_Close"), FALSE);
  gtk_box_pack_end(GTK_BOX(bottom_hbox), close_button, FALSE, FALSE, 3);
  g_signal_connect(G_OBJECT(close_button), "clicked",
      G_CALLBACK(hide_window_from_button), window);

  return window;
}

void wh_show_window(GtkWidget *window)
{
  if (!gtk_widget_get_visible(window))
  {
    gtk_widget_show_all(window);
    return;
  }

  gtk_window_present(GTK_WINDOW(window));
}

static void hide_window_from_button(GtkWidget *widget, gpointer data)
{
  GtkWidget *window = (GtkWidget *)data;
  gtk_widget_hide(window);
}

static guint _wh_add_row_to_table(GtkWidget *table)
{
  int number_of_rows = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(table), "rows"));
  number_of_rows++;
  g_object_set_data(G_OBJECT(table), "rows", GINT_TO_POINTER(number_of_rows));
  gtk_grid_insert_row(GTK_GRID(table), number_of_rows);
  return number_of_rows;
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

  gtk_widget_set_halign(my_widget, GTK_ALIGN_FILL);

  if (expand)
  {
    gtk_widget_set_hexpand(my_widget, TRUE);
  }
  else
  {
    hbox = wh_hbox_new();
    gtk_box_pack_start(GTK_BOX(hbox), widget, FALSE, FALSE, 0);
    my_widget = hbox;
  }

  gtk_grid_attach(GTK_GRID(table), my_widget, start_column, row - 1, end_column - start_column, 1);
}

