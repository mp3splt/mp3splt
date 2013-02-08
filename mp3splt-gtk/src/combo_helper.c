/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *
 *                for mp3/ogg splitting without decoding
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

/*!********************************************************
 * \file 
 *
 * this file contains the code for the combo helpers.
 ********************************************************/

#include <string.h>

#include "combo_helper.h"

GtkComboBox *ch_new_combo()
{
  GtkListStore *store = gtk_list_store_new(2, G_TYPE_STRING, G_TYPE_INT);
  GtkComboBox *combo = GTK_COMBO_BOX(
      gtk_combo_box_new_with_model(GTK_TREE_MODEL(store)));

  GtkCellRenderer *cell = gtk_cell_renderer_text_new();
  gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(combo), cell, TRUE);
  gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(combo), cell, "text", 0, NULL);

  return combo;
}

void ch_append_to_combo(GtkComboBox *combo, const gchar *text, gint value)
{
  GtkTreeIter iter;
  GtkListStore *store = GTK_LIST_STORE(gtk_combo_box_get_model(combo));
  gtk_list_store_append(store, &iter);
  gtk_list_store_set(store, &iter, 0, text, 1, value, -1); 
}

gchar *ch_get_active_str_value(GtkComboBox *combo)
{
  gchar *value = NULL;

  GtkTreeIter iter;
  gboolean has_selection = gtk_combo_box_get_active_iter(combo, &iter);

  if (has_selection)
  {
    GtkTreeModel *store = gtk_combo_box_get_model(combo);
    gtk_tree_model_get(store, &iter, 0, &value, -1);
  }
 
  return value;
}

gint ch_get_active_value(GtkComboBox *combo)
{
  gint value = -1;

  GtkTreeIter iter;
  gboolean has_selection = gtk_combo_box_get_active_iter(combo, &iter);

  if (has_selection)
  {
    GtkTreeModel *store = gtk_combo_box_get_model(combo);
    gtk_tree_model_get(store, &iter, 1, &value, -1);
  }
 
  return value;
}

void ch_set_active_str_value(GtkComboBox *combo, gchar *new_value)
{
  GtkTreeIter iter;
  GtkTreeModel *store = gtk_combo_box_get_model(combo);

  gboolean valid_row = gtk_tree_model_get_iter_first(store, &iter);
  while (valid_row)
  {
    gchar *value;
    gtk_tree_model_get(store, &iter, 0, &value, -1);

    if (strcmp(value, new_value) == 0)
    {
      gtk_combo_box_set_active_iter(combo, &iter);
      g_free(value);
      return;
    }

    valid_row = gtk_tree_model_iter_next(store, &iter);
    g_free(value);
  }
}

void ch_set_active_value(GtkComboBox *combo, gint new_value)
{
  GtkTreeIter iter;
  GtkTreeModel *store = gtk_combo_box_get_model(combo);

  gboolean valid_row = gtk_tree_model_get_iter_first(store, &iter);
  while (valid_row)
  {
    gint value;
    gtk_tree_model_get(store, &iter, 1, &value, -1);

    if (value == new_value)
    {
      gtk_combo_box_set_active_iter(combo, &iter);
      return;
    }

    valid_row = gtk_tree_model_iter_next(store, &iter);
  }
}

