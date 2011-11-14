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

/*!********************************************************
 * \file 
 * Batch processing internals
 *
 * this file is for management for the the multiple files
 * mode that currently allows only for batch processing.
 *********************************************************/

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <libmp3splt/mp3splt.h>

#include "multiple_files.h"
#include "main_win.h"
#include "ui_manager.h"
#include "widgets_helper.h"

extern splt_state *the_state;
extern ui_state *ui;

GtkWidget *multiple_files_tree = NULL;
gint multiple_files_tree_number = 0;

GtkWidget *multiple_files_remove_file_button = NULL;
GtkWidget *multiple_files_remove_all_files_button = NULL;

#define MY_GTK_RESPONSE 200

//!Create the model for the batch processing file list
GtkTreeModel *create_multiple_files_model()
{
  GtkListStore *model;

  model = gtk_list_store_new(MULTIPLE_FILES_COLUMNS,
                             G_TYPE_STRING,
                             G_TYPE_STRING);

  return GTK_TREE_MODEL(model);
}

GtkTreeView *create_multiple_files_tree()
{
  GtkTreeView *tree_view;
  GtkTreeModel *model;

  model = (GtkTreeModel *)create_multiple_files_model();
  tree_view = (GtkTreeView *)gtk_tree_view_new_with_model(model);

  return tree_view;
}

void create_multiple_files_columns(GtkTreeView *tree_view)
{
  GtkCellRendererText *renderer =
    GTK_CELL_RENDERER_TEXT(gtk_cell_renderer_text_new());
  GtkTreeViewColumn *filename_column = gtk_tree_view_column_new_with_attributes 
    (_("Complete filename"), GTK_CELL_RENDERER(renderer),
     "text", MULTIPLE_COL_FILENAME, NULL);
  gtk_tree_view_insert_column(GTK_TREE_VIEW(tree_view),
      GTK_TREE_VIEW_COLUMN(filename_column),MULTIPLE_COL_FILENAME);

  gtk_tree_view_column_set_alignment(GTK_TREE_VIEW_COLUMN(filename_column), 0.5);
  gtk_tree_view_column_set_sizing(GTK_TREE_VIEW_COLUMN(filename_column),
      GTK_TREE_VIEW_COLUMN_AUTOSIZE);
  gtk_tree_view_column_set_sort_column_id(filename_column, MULTIPLE_COL_FILENAME);
}

void multiple_files_open_button_event(GtkWidget *widget, gpointer data)
{
  gtk_dialog_response(GTK_DIALOG(data), MY_GTK_RESPONSE);
}

void multiple_files_add_button_event(GtkWidget *widget, gpointer data)
{
  GtkWidget *file_chooser;
  GtkWidget *our_filter;
 
  file_chooser = gtk_file_chooser_dialog_new(_("Choose file or directory"),
                                              NULL,
                                              GTK_FILE_CHOOSER_ACTION_OPEN,
                                              GTK_STOCK_CANCEL,
                                              GTK_RESPONSE_CANCEL,
                                              NULL);

  wh_set_browser_directory_handler(ui, file_chooser);

  GtkWidget *button = gtk_dialog_add_button(GTK_DIALOG(file_chooser),
      GTK_STOCK_ADD, MY_GTK_RESPONSE);
  gtk_button_set_use_stock(GTK_BUTTON(button), TRUE);
  g_signal_connect(G_OBJECT(button), "clicked",
      G_CALLBACK(multiple_files_open_button_event), file_chooser);
  g_signal_connect(G_OBJECT(file_chooser), "file-activated",
      G_CALLBACK(multiple_files_open_button_event), file_chooser);

  gtk_file_chooser_set_select_multiple(GTK_FILE_CHOOSER(file_chooser), TRUE);
 
  //mp3 & ogg filter
  our_filter = (GtkWidget *)gtk_file_filter_new();
  gtk_file_filter_set_name (GTK_FILE_FILTER(our_filter), _("mp3 and ogg files (*.mp3 *.ogg)"));
  gtk_file_filter_add_pattern(GTK_FILE_FILTER(our_filter), "*.mp3");
  gtk_file_filter_add_pattern(GTK_FILE_FILTER(our_filter), "*.MP3");
  gtk_file_filter_add_pattern(GTK_FILE_FILTER(our_filter), "*.ogg");
  gtk_file_filter_add_pattern(GTK_FILE_FILTER(our_filter), "*.OGG");
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_chooser), GTK_FILE_FILTER(our_filter));
  //mp3 filter
  our_filter = (GtkWidget *)gtk_file_filter_new();
  gtk_file_filter_set_name (GTK_FILE_FILTER(our_filter), _("mp3 files (*.mp3)"));
  gtk_file_filter_add_pattern(GTK_FILE_FILTER(our_filter), "*.mp3");
  gtk_file_filter_add_pattern(GTK_FILE_FILTER(our_filter), "*.MP3");
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_chooser), GTK_FILE_FILTER(our_filter));
  //ogg filter
  our_filter = (GtkWidget *)gtk_file_filter_new();
  gtk_file_filter_set_name (GTK_FILE_FILTER(our_filter), _("ogg files (*.ogg)"));
  gtk_file_filter_add_pattern(GTK_FILE_FILTER(our_filter), "*.ogg");
  gtk_file_filter_add_pattern(GTK_FILE_FILTER(our_filter), "*.OGG");
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_chooser), GTK_FILE_FILTER(our_filter));

  //all files filter
  our_filter = (GtkWidget *)gtk_file_filter_new();
  gtk_file_filter_set_name (GTK_FILE_FILTER(our_filter), _("All Files"));
  gtk_file_filter_add_pattern(GTK_FILE_FILTER(our_filter), "*");
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_chooser), GTK_FILE_FILTER(our_filter));

  //we push open, ..
  if (gtk_dialog_run(GTK_DIALOG(file_chooser)) == MY_GTK_RESPONSE)
  {
    GSList *files = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(file_chooser));
    if (files)
    {
      gchar *filename = NULL;

      GtkTreeIter iter;
      GtkTreeModel *model =
        gtk_tree_view_get_model(GTK_TREE_VIEW(multiple_files_tree));

      while (files)
      {
        filename = files->data;

        int err = SPLT_OK;
        int num_of_files_found = 0;
        char **splt_filenames =
          mp3splt_find_filenames(the_state, filename, &num_of_files_found, &err);
        
        if (splt_filenames)
        {
          int i = 0;
          for (i = 0;i < num_of_files_found;i++)
          {
            gtk_list_store_append(GTK_LIST_STORE(model), &iter);

            gtk_list_store_set (GTK_LIST_STORE(model), 
                &iter,
                MULTIPLE_COL_FILENAME, splt_filenames[i],
                -1);
            multiple_files_tree_number++;

            if (splt_filenames[i])
            {
              free(splt_filenames[i]);
              splt_filenames[i] = NULL;
            }
          }

          free(splt_filenames);
          splt_filenames = NULL;
        }

        g_free(filename);
        filename = NULL;

        files = g_slist_next(files);
      }
      g_slist_free(files);

      if (multiple_files_tree_number > 0)
      {
        gtk_widget_set_sensitive(multiple_files_remove_all_files_button, TRUE);
      }
    }
  }

  gtk_widget_destroy(file_chooser);
}

void multiple_files_remove_button_event(GtkWidget *widget, gpointer data)
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  GtkTreePath *path;
  GList *selected_list = NULL;
  GList *current_element = NULL;
  GtkTreeSelection *selection;
  
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(multiple_files_tree));
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(multiple_files_tree));
  selected_list = gtk_tree_selection_get_selected_rows(selection, &model);
  
  //the name of the file that we have clicked on
  gchar *filename = NULL;
  
  while (g_list_length(selected_list) > 0)
  {
    //get the last element
    current_element = g_list_last(selected_list);
    path = current_element->data;
    //get the iter correspondig to the path
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get(model, &iter, MULTIPLE_COL_FILENAME, &filename, -1);
    //remove the path from the selected list
    gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
    selected_list = g_list_remove(selected_list, path);
    multiple_files_tree_number--;

    //free memory
    gtk_tree_path_free(path);
    g_free(filename);
  }
  
  if (multiple_files_tree_number == 0)
  {
    gtk_widget_set_sensitive(multiple_files_remove_all_files_button, FALSE);
  }
  
  gtk_widget_set_sensitive(multiple_files_remove_file_button,FALSE);
  
  //free the selected elements
  g_list_foreach(selected_list, (GFunc)gtk_tree_path_free, NULL);
  g_list_free(selected_list);  
}

void multiple_files_remove_all_button_event(GtkWidget *widget, gpointer data)
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(multiple_files_tree));
  
  //filename to erase
  gchar *filename = NULL;
  //for all the splitnumbers
  while (multiple_files_tree_number > 0)
  {
    gtk_tree_model_get_iter_first(model, &iter);
    gtk_tree_model_get(model, &iter, MULTIPLE_COL_FILENAME, &filename, -1);
    gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
    multiple_files_tree_number--;
    g_free(filename);
  }
  
  gtk_widget_set_sensitive(multiple_files_remove_all_files_button,FALSE);
  gtk_widget_set_sensitive(multiple_files_remove_file_button,FALSE);
}

GtkWidget *create_multiple_files_buttons_hbox()
{
  GtkWidget *hbox;
  hbox = gtk_hbox_new(FALSE,0);

  //button for adding file(s)
  GtkWidget *multiple_files_add_button = (GtkWidget *)
    create_cool_button(GTK_STOCK_ADD, _("_Add files"), FALSE);
  gtk_box_pack_start(GTK_BOX(hbox), multiple_files_add_button, TRUE, FALSE, 5);
  gtk_widget_set_sensitive(multiple_files_add_button, TRUE);
  g_signal_connect(G_OBJECT(multiple_files_add_button), "clicked",
                   G_CALLBACK(multiple_files_add_button_event), NULL);

  //button for removing a file
  multiple_files_remove_file_button = (GtkWidget *)
    create_cool_button(GTK_STOCK_DELETE, _("_Remove selected entries"),FALSE);
  gtk_box_pack_start(GTK_BOX(hbox),
      multiple_files_remove_file_button, TRUE, FALSE, 5);
  gtk_widget_set_sensitive(multiple_files_remove_file_button,FALSE);
  g_signal_connect(G_OBJECT(multiple_files_remove_file_button), "clicked",
                   G_CALLBACK(multiple_files_remove_button_event), NULL);
  
  //button for removing a file
  multiple_files_remove_all_files_button = (GtkWidget *)
    create_cool_button(GTK_STOCK_DELETE, _("R_emove all entries"),FALSE);
  gtk_box_pack_start(GTK_BOX(hbox), multiple_files_remove_all_files_button,
      TRUE, FALSE, 5);
  gtk_widget_set_sensitive(multiple_files_remove_all_files_button,FALSE);
  g_signal_connect(G_OBJECT(multiple_files_remove_all_files_button), "clicked",
                   G_CALLBACK(multiple_files_remove_all_button_event), NULL);

  return hbox;
}

void multiple_files_selection_changed(GtkTreeSelection *selec, gpointer data)
{
  GtkTreeModel *model;
  GtkTreeSelection *selection;
  GList *selected_list = NULL;
  
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(multiple_files_tree));
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(multiple_files_tree));
  selected_list = gtk_tree_selection_get_selected_rows(selection, &model);

  if (g_list_length(selected_list) > 0)
  {
    gtk_widget_set_sensitive(multiple_files_remove_file_button,TRUE);
  }
  else
  {
    gtk_widget_set_sensitive(multiple_files_remove_file_button,FALSE);
  }
}

GtkWidget *create_multiple_files_component()
{
  GtkWidget *vbox = gtk_vbox_new(FALSE, 0);

  multiple_files_tree = (GtkWidget *)create_multiple_files_tree();

  GtkWidget *scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_NONE);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW (scrolled_window),
      GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);

  //create columns
  create_multiple_files_columns(GTK_TREE_VIEW(multiple_files_tree));

  //add the tree to the scrolled window
  gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(multiple_files_tree));

  //selection for the tree
  GtkWidget *multiple_files_tree_selection = (GtkWidget *)
    gtk_tree_view_get_selection(GTK_TREE_VIEW(multiple_files_tree));
  g_signal_connect(G_OBJECT(multiple_files_tree_selection), "changed",
                   G_CALLBACK(multiple_files_selection_changed), NULL);
  gtk_tree_selection_set_mode(GTK_TREE_SELECTION(multiple_files_tree_selection),
                              GTK_SELECTION_MULTIPLE);

  //bottom horizontal box with buttons
  GtkWidget *buttons_hbox = (GtkWidget *)create_multiple_files_buttons_hbox();
  gtk_box_pack_start(GTK_BOX(vbox), buttons_hbox, FALSE, FALSE, 5);

  return vbox;
}

