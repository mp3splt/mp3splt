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
 * The freedb tab
 *
 * this file is used for the cddb tab 
 *   (for searching on freedb)
 *********************************************************/

#include <stdlib.h>
#include <string.h>

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <glib/gstdio.h>

#include <libmp3splt/mp3splt.h>

#include "util.h"
#include "main_win.h"
#include "tree_tab.h"
#include "preferences_tab.h"
#include "utilities.h"
#include "mp3splt-gtk.h"

//handle box for detaching window
GtkWidget *freedb_handle_box;

//filename entry
GtkWidget *freedb_entry;
//filename selected entry
GtkWidget *freedb_selected_entry;

//our freedb tree
GtkWidget *freedb_tree;
//we count the number of rows in the table
gint freedb_table_number = 0;
//freedb table enumeration
enum
  {
    ALBUM_NAME,
    NUMBER,
    FREEDB_TABLE
  };

//results of the freedb search
const splt_freedb_results *search_results;
//the selected entry id
gint selected_id = -1;

//the add splitpoint button
GtkWidget *freedb_add_button;
GtkWidget *freedb_search_button;

//the state main mp3splt state
extern splt_state *the_state;
//the spin values
extern gint spin_mins,spin_secs,
  spin_hundr_secs;
extern gchar current_description[255];
//output for the cddb,cue and freedb file output
extern GtkWidget *output_entry;
extern gint debug_is_active;

//!add a row to the table
void add_freedb_row(gchar *album_name, 
                    gint album_id,
                    gint *revisions,
                    gint revisions_number)
{
  //father iter
  GtkTreeIter iter;
  //children iter
  GtkTreeIter child_iter;
  //our tree and the model
  GtkTreeView *tree_view = (GtkTreeView *)freedb_tree;
  GtkTreeModel *model;

  model = gtk_tree_view_get_model(tree_view);
  
  gtk_tree_store_append (GTK_TREE_STORE(model), &iter,NULL);
  //sets the father
  gtk_tree_store_set (GTK_TREE_STORE(model), &iter,
                      ALBUM_NAME, album_name,
                      NUMBER, album_id,
                      -1);
  
  gchar *number;
  gint malloc_number = strlen(album_name) + 20;
  //allocate memory
  number = malloc(malloc_number * sizeof(gchar *));
  gint i;
  for(i = 0; i < revisions_number; i++)
    {
      g_snprintf(number,malloc_number,
          _("%s Revision %d"),album_name, revisions[i]);

      //sets the children..
      gtk_tree_store_append (GTK_TREE_STORE(model),
                             &child_iter, &iter);
      gtk_tree_store_set (GTK_TREE_STORE(model),
                          &child_iter,
                          ALBUM_NAME, number,
                          NUMBER, album_id+i+1,
                          -1);
    }
  freedb_table_number++;
  //free memory
  g_free(number);
}

//!creates the model for the freedb tree
GtkTreeModel *create_freedb_model()
{
  GtkTreeStore *model;
  model = gtk_tree_store_new (FREEDB_TABLE,
                              G_TYPE_STRING,
                              G_TYPE_INT);

  return GTK_TREE_MODEL (model);
}

//!creates the freedb tree
GtkTreeView *create_freedb_tree()
{
  GtkTreeView *tree_view;
  GtkTreeModel *model;
  //create the model
  model = (GtkTreeModel *)create_freedb_model();
  //create the tree view
  tree_view = (GtkTreeView *)
    gtk_tree_view_new_with_model (model);

  return tree_view;
}

//!creates freedb columns
void create_freedb_columns (GtkTreeView *tree_view)
{
  //cells renderer
  GtkCellRendererText *renderer;
  //columns
  GtkTreeViewColumn *name_column;
  GtkTreeViewColumn *number_column;
  
  //album title
  renderer = GTK_CELL_RENDERER_TEXT(gtk_cell_renderer_text_new ());
  g_object_set_data(G_OBJECT(renderer), "col", GINT_TO_POINTER(ALBUM_NAME));
  name_column = gtk_tree_view_column_new_with_attributes 
    (_("Album title"), GTK_CELL_RENDERER(renderer),
     "text", ALBUM_NAME, NULL);

  //appends columns to the list of columns of tree_view
  gtk_tree_view_insert_column (GTK_TREE_VIEW (tree_view),
                               GTK_TREE_VIEW_COLUMN (name_column),
                               ALBUM_NAME);

  //middle alignment of the column name
  gtk_tree_view_column_set_alignment(GTK_TREE_VIEW_COLUMN(name_column),
                                     0.5);
  gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN(name_column),
                                   GTK_TREE_VIEW_COLUMN_AUTOSIZE);
  gtk_tree_view_column_set_expand (GTK_TREE_VIEW_COLUMN(name_column),
                                   TRUE);

  //number of the album title
  renderer = GTK_CELL_RENDERER_TEXT(gtk_cell_renderer_text_new ());
  g_object_set_data(G_OBJECT(renderer), "col", GINT_TO_POINTER(NUMBER));
  number_column = gtk_tree_view_column_new_with_attributes
    (_("No"), GTK_CELL_RENDERER(renderer), "text", NUMBER, NULL);

  //appends columns to the list of columns of tree_view
  gtk_tree_view_insert_column (GTK_TREE_VIEW (tree_view),
                               GTK_TREE_VIEW_COLUMN (number_column),
                               NUMBER);

  //middle alignment of the number name
  gtk_tree_view_column_set_alignment(GTK_TREE_VIEW_COLUMN(number_column),
                                     0.5);
  gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN(number_column),
                                   GTK_TREE_VIEW_COLUMN_AUTOSIZE);
  
  //resize.
  gtk_tree_view_column_set_resizable (GTK_TREE_VIEW_COLUMN(name_column),
                                      TRUE);
}

//!when closing the new window after detaching
void close_freedb_popup_window_event( GtkWidget *window,
                                      gpointer data )
{
  GtkWidget *window_child;

  window_child = gtk_bin_get_child(GTK_BIN(window));

  gtk_widget_reparent(GTK_WIDGET(window_child),
                      GTK_WIDGET(freedb_handle_box));

  gtk_widget_destroy(window);
}

//!when we detach the handle
void handle_freedb_detached_event (GtkHandleBox *handlebox,
                                 GtkWidget *widget,
                                 gpointer data)
{
  //new window
  GtkWidget *window;
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_widget_reparent(GTK_WIDGET(widget), GTK_WIDGET(window));

  g_signal_connect (G_OBJECT (window), "delete_event",
                    G_CALLBACK (close_freedb_popup_window_event),
                    NULL);

  gtk_widget_show(GTK_WIDGET(window));
}

//!freedb selection has changed
void freedb_selection_changed(GtkTreeSelection *selection,
                              gpointer data)
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  
  //get the model
  model = 
    gtk_tree_view_get_model(GTK_TREE_VIEW(freedb_tree));
  
  //if we have selected
  if(gtk_tree_selection_get_selected(selection,
                                     &model,
                                     &iter))
    {
      gchar *info;
      gtk_tree_model_get (model, &iter,
                          ALBUM_NAME, &info,
                          NUMBER, &selected_id,
                          -1);
      
      gtk_entry_set_text(GTK_ENTRY(freedb_selected_entry),
                         info);
      g_free(info);
      
      gtk_widget_set_sensitive(GTK_WIDGET(freedb_add_button), TRUE);
    }
}

//!removes all rows from the freedb table
void remove_all_freedb_rows ()
{
  GtkTreeIter iter;
  GtkTreeView *tree_view = (GtkTreeView *)freedb_tree;
  GtkTreeModel *model;
  
  model = gtk_tree_view_get_model(tree_view);
  
  //while we still have rows in the table
  while (freedb_table_number > 0)
    {
      gtk_tree_model_get_iter_first(model, &iter);
      gtk_tree_store_remove (GTK_TREE_STORE (model), &iter);
      freedb_table_number--;
    }
}

//!search the freedb.org
gpointer freedb_search(gpointer data)
{
  enter_threads();
 
  gtk_widget_set_sensitive(GTK_WIDGET(freedb_add_button), FALSE);
  gtk_widget_set_sensitive(GTK_WIDGET(freedb_search_button), FALSE);
  gtk_editable_set_editable(GTK_EDITABLE(freedb_entry), FALSE);
 
  put_status_message(_("please wait... contacting tracktype.org"));
 
  const gchar *freedb_search = gtk_entry_get_text(GTK_ENTRY(freedb_entry));
  
  exit_threads();
  
  gint err = SPLT_OK;
  search_results = mp3splt_get_freedb_search(the_state, freedb_search, &err,
      SPLT_FREEDB_SEARCH_TYPE_CDDB_CGI, "\0",-1);
 
  enter_threads();

  print_status_bar_confirmation(err);
 
  remove_all_freedb_rows();
  gtk_entry_set_text(GTK_ENTRY(freedb_selected_entry), "");
 
  if (err >= 0)
  {
    gint i = 0;
    for (i = 0; i< search_results->number;i++)
    {
      gint must_be_free = SPLT_FALSE;
      search_results->results[i].name =
        transform_to_utf8(search_results->results[i].name,
            TRUE, &must_be_free);
      add_freedb_row(search_results->results[i].name,
          search_results->results[i].id,
          search_results->results[i].revisions,
          search_results->results[i].revision_number);
    }

    if (search_results->number > 0)
    {
      //select the first result
      GtkTreeModel *model;
      GtkTreePath *path;
      GtkTreeIter iter;
      GtkTreeSelection *selection;
      selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(freedb_tree));

      model = gtk_tree_view_get_model(GTK_TREE_VIEW(freedb_tree));
      path = gtk_tree_path_new_from_indices (0 ,-1);
      gtk_tree_model_get_iter(model, &iter, path);
      gtk_tree_selection_select_iter(selection, &iter);
      gtk_tree_path_free(path);
    }
  }
 
  gtk_widget_set_sensitive(GTK_WIDGET(freedb_search_button), TRUE);
  gtk_editable_set_editable(GTK_EDITABLE(freedb_entry), TRUE);
 
  exit_threads();

  return NULL;
}

//! Start a thread for the freedb search
void freedb_search_start_thread()
{
  mp3splt_set_int_option(the_state, SPLT_OPT_DEBUG_MODE, debug_is_active);
  create_thread(freedb_search, NULL, TRUE, NULL);
}

//!we push the search button
void freedb_search_button_event(GtkWidget *widget, gpointer   data)
{
  freedb_search_start_thread();
}

/*!search entry backspace event

when we push Enter for the search entry
*/
void freedb_entry_activate_event(GtkEntry *entry, gpointer data)
{
  freedb_search_start_thread();
}

/*!returns the number of splitpoints

we put the new splitpoints in "the_state"
*/
void write_freedbfile(int *err)
{
  gchar *filename = NULL;
 
  enter_threads();
   
  put_status_message(_("please wait... contacting tracktype.org"));
  
  //we suppose directory exists
  //it should be created when mp3splt-gtk starts
  gchar mp3splt_dir[14] = ".mp3splt-gtk";

  const gchar *home_dir = g_get_home_dir();
  gint malloc_number = strlen(mp3splt_dir) + strlen(home_dir) + 20;
  filename = malloc(malloc_number * sizeof(gchar *));
  
  g_snprintf(filename,malloc_number,
             "%s%s%s%s%s", home_dir,
             G_DIR_SEPARATOR_S, mp3splt_dir,
             G_DIR_SEPARATOR_S, "query.cddb");
 
  exit_threads();

  //we write the freedb file ...
  mp3splt_write_freedb_file_result(the_state, selected_id,
                                   filename, err,
                                   //for now cddb.cgi get file type
                                   SPLT_FREEDB_GET_FILE_TYPE_CDDB_CGI,
                                   "\0",-1);

  enter_threads();
  print_status_bar_confirmation(*err);
  exit_threads();

  if(get_checked_output_radio_box())
  {
    mp3splt_set_int_option(the_state, SPLT_OPT_OUTPUT_FILENAMES,
        SPLT_OUTPUT_DEFAULT);
  }
  else
  {
    mp3splt_set_int_option(the_state, SPLT_OPT_OUTPUT_FILENAMES,
        SPLT_OUTPUT_FORMAT);

    const char *data = gtk_entry_get_text(GTK_ENTRY(output_entry));
    gint error = SPLT_OUTPUT_FORMAT_OK;
    mp3splt_set_oformat(the_state, data, &error);
    enter_threads();
    print_status_bar_confirmation(error);
    exit_threads();
  }

  mp3splt_put_cddb_splitpoints_from_file(the_state,filename, err);
 
  if (filename)
  {
    g_free(filename);
    filename = NULL;
  }
}

//!returns the seconds, minutes, and hudreths
void get_secs_mins_hundr(gfloat time,
                         gint *mins,gint *secs, 
                         gint *hundr)
{
  *mins = (gint)(time / 6000);
  *secs = (gint)(time - (*mins * 6000))
    / 100;
  *hundr = (gint)(time - (*mins * 6000)
                  - (*secs * 100));
}

/*!updates the current splitpoints in the_state

Takes the splitpoints from the table displayed in the gui

max_splits is the maximum number of splitpoints to update
*/
void update_splitpoints_from_the_state()
{
  gint max_splits = 0;
  gint err = SPLT_OK;

  exit_threads();
  const splt_point *points = mp3splt_get_splitpoints(the_state, &max_splits,&err);
  enter_threads();

  print_status_bar_confirmation(err);
  
  //only if we have splitpoints
  if (max_splits > 0)
  {
    //erase rows from the splitpoints table
    remove_all_rows(NULL,NULL);
    gint i;

    //for each splitpoint, we put it in the table
    for(i = 0; i < max_splits;i++)
    {
      //ugly hack because we use maximum ints in the GUI
      //-GUI must be changed to accept long values
      long old_point_value = points[i].value;
      int point_value = (int) old_point_value;
      if (old_point_value > INT_MAX)
      {
        point_value = INT_MAX;
      }

      //we get the minutes, seconds and hundreths
      get_secs_mins_hundr(point_value,
          &spin_mins, &spin_secs,
          &spin_hundr_secs);

      gint must_be_free = FALSE;
      gchar *result_utf8 = points[i].name;

      if (result_utf8 != NULL)
      {
        result_utf8 = transform_to_utf8(result_utf8, 
            FALSE, &must_be_free);

        g_snprintf(current_description,255,
            "%s", result_utf8);
      }
      else
      {
        g_snprintf(current_description, 255, "%s", _("description here"));
      }

      if (must_be_free)
      {
        g_free(result_utf8);
        result_utf8 = NULL;
      }

      //we add the row
      if (points[i].type == SPLT_SPLITPOINT)
      {
        add_row(TRUE);
      }
      else if (points[i].type == SPLT_SKIPPOINT)
      {
        add_row(FALSE);
      }
    }

    //we reput the "description here" name
    g_snprintf(current_description, 255, "%s",
        _("description here"));

    update_minutes_from_spinner(NULL,NULL);
    update_seconds_from_spinner(NULL,NULL);
    update_hundr_secs_from_spinner(NULL,NULL);
    update_add_button();
  }
}

gpointer put_freedb_splitpoints(gpointer data)
{
  gint err = SPLT_OK;

  enter_threads();
  gtk_widget_set_sensitive(GTK_WIDGET(freedb_add_button), FALSE);  
  exit_threads();
 
  write_freedbfile(&err);
 
  enter_threads();
 
  update_splitpoints_from_the_state();
  print_status_bar_confirmation(err);
  gtk_widget_set_sensitive(GTK_WIDGET(freedb_add_button), TRUE);

  exit_threads();
  
  return NULL;
}

//!event for the freedb add button when clicked
void freedb_add_button_clicked_event(GtkButton *button, gpointer data)
{
  mp3splt_set_int_option(the_state, SPLT_OPT_DEBUG_MODE, debug_is_active);
  create_thread(put_freedb_splitpoints, NULL, TRUE, NULL);
}

//!creates the freedb box
GtkWidget *create_freedb_frame()
{
  //main freedb box
  GtkWidget *freedb_hbox;
  freedb_hbox = gtk_hbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (freedb_hbox), 5);
  
  /* handle box for detaching */
  freedb_handle_box = gtk_handle_box_new();
  gtk_container_add(GTK_CONTAINER (freedb_handle_box), GTK_WIDGET(freedb_hbox));
  //handle event
  g_signal_connect(freedb_handle_box, "child-detached",
                   G_CALLBACK(handle_freedb_detached_event),
                   NULL);
  
  //vertical box
  GtkWidget *freedb_vbox;
  freedb_vbox = gtk_vbox_new(FALSE,0);
  gtk_box_pack_start (GTK_BOX(freedb_hbox), freedb_vbox , TRUE, TRUE, 4);
  
  /* search box */
  //horizontal search box
  GtkWidget *search_hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX(freedb_vbox), search_hbox , FALSE, FALSE, 3);
  //top label
  GtkWidget *label = gtk_label_new(_("Search tracktype.org:"));
  gtk_box_pack_start(GTK_BOX(search_hbox), label, FALSE, FALSE, 0);
  //top entry
  freedb_entry = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(freedb_entry), TRUE);
  gtk_box_pack_start(GTK_BOX(search_hbox), freedb_entry, TRUE, TRUE, 6);
  g_signal_connect(G_OBJECT(freedb_entry), "activate",
      G_CALLBACK(freedb_entry_activate_event), NULL);
  
  //search button
  freedb_search_button = (GtkWidget *)
    create_cool_button(GTK_STOCK_FIND, _("_Search"),FALSE);
  g_signal_connect(G_OBJECT(freedb_search_button), "clicked",
      G_CALLBACK(freedb_search_button_event), NULL);
  gtk_box_pack_start(GTK_BOX(search_hbox), freedb_search_button,
                      FALSE, FALSE, 0);
  
  /* freedb scrolled window and the tree */
  //create the tree and add it to the scrolled window
  freedb_tree = (GtkWidget *) create_freedb_tree();
  //scrolled window for the tree
  GtkWidget *scrolled_window;
  scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_NONE);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                  GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_box_pack_start (GTK_BOX (freedb_vbox),
                      scrolled_window, TRUE, TRUE, 5);
  //create columns
  create_freedb_columns (GTK_TREE_VIEW(freedb_tree));
  //add the tree to the scrolled window
  gtk_container_add (GTK_CONTAINER (scrolled_window), 
                     GTK_WIDGET(freedb_tree));
  
  /* expand all rows after the treeview widget has been realized */
  /*g_signal_connect (freedb_tree, "realize",
    G_CALLBACK (gtk_tree_view_expand_all), NULL);*/
  
  //selection for the tree
  GtkWidget *freedb_tree_selection;
  freedb_tree_selection = (GtkWidget *)
    gtk_tree_view_get_selection(GTK_TREE_VIEW(freedb_tree));
  g_signal_connect (G_OBJECT (freedb_tree_selection), "changed",
                    G_CALLBACK (freedb_selection_changed), NULL);

  /* selected album box */
  //horizontal selected box
  GtkWidget *selected_hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(freedb_vbox), selected_hbox , FALSE, FALSE, 3);
  //top selected label
  GtkWidget *label_selected = gtk_label_new(_("Selected album:"));
  gtk_box_pack_start(GTK_BOX(selected_hbox), label_selected , FALSE, FALSE, 0);
  //freedb selected entry
  freedb_selected_entry = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(freedb_selected_entry), FALSE);
  gtk_box_pack_start(GTK_BOX(selected_hbox), freedb_selected_entry, TRUE, TRUE, 6);
  //add button
  freedb_add_button = (GtkWidget *)
    create_cool_button(GTK_STOCK_ADD,_("_Add splitpoints"), FALSE);
  //gtk_button_set_relief(GTK_BUTTON(freedb_add_button), GTK_RELIEF_NONE);
  gtk_widget_set_sensitive(GTK_WIDGET(freedb_add_button), FALSE);
  g_signal_connect(G_OBJECT(freedb_add_button), "clicked",
      G_CALLBACK(freedb_add_button_clicked_event), NULL);
  gtk_box_pack_start(GTK_BOX(selected_hbox), freedb_add_button, FALSE, FALSE, 0);
  gtk_widget_set_tooltip_text(freedb_add_button, 
      _("Set splitpoints to the splitpoints table"));
  
  return freedb_handle_box;
}

