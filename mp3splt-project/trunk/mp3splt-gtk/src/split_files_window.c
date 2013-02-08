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

/*!********************************************************
 * \file 
 * The split tab
 *
 * file that handles the split files tab from the main 
 * window
 **********************************************************/

#include "split_files_window.h"

//!split files enumeration
enum
{
  COL_NAME,
  COL_FILENAME,
  SPLIT_COLUMNS
};

//!creates the model for the split tree
static GtkTreeModel *create_split_model()
{
  GtkListStore *model = gtk_list_store_new(SPLIT_COLUMNS, G_TYPE_STRING, G_TYPE_STRING);
  return GTK_TREE_MODEL(model);
}

//!creates the tree
static GtkTreeView *create_split_files_tree()
{
  return GTK_TREE_VIEW(gtk_tree_view_new_with_model(create_split_model()));
}

//!creates split columns
static void create_split_columns(GtkTreeView *split_tree)
{
  GtkCellRendererText *renderer = GTK_CELL_RENDERER_TEXT(gtk_cell_renderer_text_new());
  g_object_set_data(G_OBJECT(renderer), "col", GINT_TO_POINTER(COL_NAME));
  GtkTreeViewColumn *name_column = gtk_tree_view_column_new_with_attributes 
    (_("Filename"), GTK_CELL_RENDERER(renderer), "text", COL_NAME, NULL);

  gtk_tree_view_insert_column(split_tree, GTK_TREE_VIEW_COLUMN(name_column), COL_NAME);

  gtk_tree_view_column_set_alignment(GTK_TREE_VIEW_COLUMN(name_column), 0.5);
  gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(name_column),
      GTK_TREE_VIEW_COLUMN_AUTOSIZE);
  gtk_tree_view_column_set_resizable(name_column, TRUE);
}

//!removes all rows from the split files table
void remove_all_split_rows(ui_state *ui)
{
  GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->split_tree);
  GtkTreeIter iter;
  while (gtk_tree_model_get_iter_first(model, &iter))
  {
    gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
  }
}

/*! finding the real name of the file, without the path
*/
const gchar *get_real_name_from_filename(const gchar *filename)
{
  const gchar *fname = filename;
  while (strchr(fname, G_DIR_SEPARATOR) != NULL)
  {
    fname = strchr(fname, G_DIR_SEPARATOR) + 1;
  }

  return fname;
}

//!add a row to the table
void add_split_row(const gchar *name, ui_state *ui)
{
  GtkTreeIter iter;
  GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->split_tree);
  gtk_list_store_append(GTK_LIST_STORE(model), &iter);

  gtk_list_store_set(GTK_LIST_STORE(model), &iter,
      COL_NAME, get_real_name_from_filename(name),
      COL_FILENAME, name, -1);
}

//!return the n_th filename from the split files
gchar *get_filename_from_split_files(gint number, gui_state *gui)
{
  gchar *filename = NULL;

  GtkTreeModel *model = gtk_tree_view_get_model(gui->split_tree);
  GtkTreePath *path = gtk_tree_path_new_from_indices(number-1 ,-1);

  GtkTreeIter iter;
  if (gtk_tree_model_get_iter(model, &iter, path))
  {
    gtk_tree_model_get(model, &iter, COL_FILENAME, &filename, -1);
  }
  gtk_tree_path_free(path);

  return filename;
}

//!queue button event
static void queue_files_button_event(GtkWidget *widget, ui_state *ui)
{
  if (!ui->status->timer_active)
  {
    connect_button_event(ui->gui->connect_button, ui);
  }

  GList *file_list = NULL;
  GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->split_tree);

  GtkTreeIter iter;

  gboolean valid_row = gtk_tree_model_get_iter_first(model, &iter);
  while (valid_row)
  {
    gchar *filename = NULL;
    gtk_tree_model_get(model, &iter, COL_FILENAME, &filename, -1);
    file_list = g_list_append(file_list, strdup(filename));
    g_free(filename);

    valid_row = gtk_tree_model_iter_next(model, &iter);
  }

  if (file_list != NULL)
  {
    player_add_files(file_list, ui);
  }

  g_list_foreach(file_list, (GFunc)g_free, NULL);
  g_list_free(file_list);
}

//!event for the remove file button
static void remove_file_button_event(GtkWidget *widget, ui_state *ui)
{
  GtkTreeIter iter;
  GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->split_tree);
  GtkTreeSelection *selection = gtk_tree_view_get_selection(ui->gui->split_tree);
  GList *selected_list = gtk_tree_selection_get_selected_rows(selection, &model);

  while (g_list_length(selected_list) > 0)
  {
    GList *current_element = g_list_last(selected_list);
    GtkTreePath *path = current_element->data;

    gtk_tree_model_get_iter(model, &iter, path);

    gchar *filename = NULL;
    gtk_tree_model_get(model, &iter, COL_FILENAME, &filename, -1);
    g_remove(filename);

    gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
    selected_list = g_list_remove(selected_list, path);

    gtk_tree_path_free(path);
    g_free(filename);
  }

  if (!gtk_tree_model_get_iter_first(model, &iter))
  {
    gtk_widget_set_sensitive(ui->gui->queue_files_button, FALSE);
    gtk_widget_set_sensitive(ui->gui->remove_all_files_button, FALSE);
  }

  gtk_widget_set_sensitive(ui->gui->remove_file_button,FALSE);

  g_list_foreach(selected_list, (GFunc)gtk_tree_path_free, NULL);
  g_list_free(selected_list);
}

//!event for the remove file button
static void remove_all_files_button_event(GtkWidget *widget, ui_state *ui)
{
  GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->split_tree);
  GtkTreeIter iter;

  while (gtk_tree_model_get_iter_first(model, &iter))
  {
    gchar *filename;
    gtk_tree_model_get(model, &iter, COL_FILENAME, &filename, -1);
    g_remove(filename);

    gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
    g_free(filename);
  }

  gtk_widget_set_sensitive(ui->gui->remove_all_files_button,FALSE);
  gtk_widget_set_sensitive(ui->gui->remove_file_button,FALSE);
  gtk_widget_set_sensitive(ui->gui->queue_files_button,FALSE);
}

//!creates the horizontal queue buttons horizontal box
static void create_queue_buttons(ui_state *ui)
{
  //button for queueing all files
  GtkWidget *queue_files_button =
    wh_create_cool_button(GTK_STOCK_INDEX, _("_Queue to player"),FALSE);
  ui->gui->queue_files_button = queue_files_button;
  gtk_widget_set_sensitive(queue_files_button, FALSE);
  g_signal_connect(G_OBJECT(queue_files_button), "clicked",
      G_CALLBACK(queue_files_button_event), ui);

  //button for removing a file
  GtkWidget *remove_file_button =
    wh_create_cool_button(GTK_STOCK_DELETE, _("_Delete selected"), FALSE);
  ui->gui->remove_file_button = remove_file_button;
  gtk_widget_set_sensitive(remove_file_button, FALSE);
  g_signal_connect(G_OBJECT(remove_file_button), "clicked",
      G_CALLBACK(remove_file_button_event), ui);

  //button for removing a file
  GtkWidget *remove_all_files_button =
    wh_create_cool_button(GTK_STOCK_DELETE, _("D_elete all"), FALSE);
  ui->gui->remove_all_files_button = remove_all_files_button;
  gtk_widget_set_sensitive(remove_all_files_button, FALSE);
  g_signal_connect(G_OBJECT(remove_all_files_button), "clicked",
      G_CALLBACK(remove_all_files_button_event), ui);
}

//! Issued when a row is clicked on
void split_tree_row_activated(GtkTreeView *split_tree, GtkTreePath *arg1,
    GtkTreeViewColumn *arg2, ui_state *ui)
{
  GtkTreeModel *model = gtk_tree_view_get_model(split_tree);
  GtkTreeSelection *selection = gtk_tree_view_get_selection(split_tree);
  GList *selected_list = gtk_tree_selection_get_selected_rows(selection, &model);

  GList *current_element = g_list_first(selected_list);
  GtkTreePath *path = current_element->data;

  GtkTreeIter iter;
  gtk_tree_model_get_iter(model, &iter, path);

  gchar *filename = NULL;
  gtk_tree_model_get(model, &iter, COL_FILENAME, &filename, -1);

  connect_button_event(ui->gui->connect_button, ui);
  change_current_filename(filename, ui);
  connect_to_player_with_song(0, ui);

  gtk_tree_path_free(path);
  if (filename)
  {
    g_free(filename);
  }
}

//!split selection has changed
static void split_selection_changed(GtkTreeSelection *selec, ui_state *ui)
{
  GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->split_tree);
  GtkTreeSelection *selection = gtk_tree_view_get_selection(ui->gui->split_tree);
  GList *selected_list = gtk_tree_selection_get_selected_rows(selection, &model);
  if (g_list_length(selected_list) > 0)
  {
    gtk_widget_set_sensitive(ui->gui->remove_file_button, TRUE);
  }
  else
  {
    gtk_widget_set_sensitive(ui->gui->remove_file_button, FALSE);
  }
}

//!creates the split files tab
GtkWidget *create_split_files_frame(ui_state *ui)
{
  GtkWidget *vbox = wh_vbox_new();

  GtkTreeView *split_tree = create_split_files_tree();
  ui->gui->split_tree = split_tree;
  GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_NONE);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
      GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
  create_split_columns(split_tree);
  gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(split_tree));
  g_signal_connect(G_OBJECT(split_tree), "row-activated", G_CALLBACK(split_tree_row_activated), ui);
  
  //selection for the tree
  GtkTreeSelection *split_tree_selection = gtk_tree_view_get_selection(split_tree);
  g_signal_connect(G_OBJECT(split_tree_selection), "changed", 
      G_CALLBACK(split_selection_changed), ui);
  gtk_tree_selection_set_mode(GTK_TREE_SELECTION(split_tree_selection), GTK_SELECTION_MULTIPLE);
  
  create_queue_buttons(ui);
  
  return vbox;
}

