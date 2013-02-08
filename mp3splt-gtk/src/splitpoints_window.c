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
 * The splitpoints tab
 *
 * this file is used for the Splitpoints tab
 * (which in turn contains the splitpoints table)
 **********************************************************/

#include "splitpoints_window.h"

//! checks if splitpoints exists in the table and is different from current_split
static gboolean check_if_splitpoint_does_not_exists(gint minutes, gint seconds, gint hundr_secs, 
    gint current_split, ui_state *ui)
{
  GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->tree_view);

  GtkTreeIter iter;
  if (!gtk_tree_model_get_iter_first(model, &iter))
  {
    return TRUE;
  }

  gint tree_minutes;
  gint tree_seconds;
  gint tree_hundr_secs;

  while (TRUE)
  {
    gtk_tree_model_get(GTK_TREE_MODEL(model), &iter,
        COL_MINUTES, &tree_minutes,
        COL_SECONDS, &tree_seconds,
        COL_HUNDR_SECS, &tree_hundr_secs,
        -1);

    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    gint i = gtk_tree_path_get_indices (path)[0];

    if ((minutes == tree_minutes) &&
        (seconds == tree_seconds) && 
        (hundr_secs == tree_hundr_secs) &&
        (i != current_split))
    {
      gtk_tree_path_free(path);
      return FALSE;
    }

    gtk_tree_path_free(path);

    if (!gtk_tree_model_iter_next(model, &iter))
    {
      break;
    }
  }

  return TRUE;
}

/*! updates add button

Makes the add button show whether the spinners splitpoint is already
in the table or not
*/
static void update_add_button(ui_state *ui)
{
  gui_status *status = ui->status;
  if (check_if_splitpoint_does_not_exists(status->spin_mins, status->spin_secs, status->spin_hundr_secs,-1, ui))
  {
    gtk_widget_set_sensitive(GTK_WIDGET(ui->gui->add_button), TRUE);
  }
  else
  {
    gtk_widget_set_sensitive(GTK_WIDGET(ui->gui->add_button), FALSE);
  }
}

//!updates the minutes from the spinner
void update_minutes_from_spinner(GtkWidget *widget, ui_state *ui)
{
  ui->status->spin_mins = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ui->gui->spinner_minutes));
  update_add_button(ui);
}

//!updates the seconds from the spinner
void update_seconds_from_spinner(GtkWidget *widget, ui_state *ui)
{
  ui->status->spin_secs = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ui->gui->spinner_seconds));
  update_add_button(ui);
}

//!updates the hundredth of seconds for the spinner
void update_hundr_secs_from_spinner(GtkWidget *widget, ui_state *ui)
{
  ui->status->spin_hundr_secs = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(ui->gui->spinner_hundr_secs));
  update_add_button(ui);
}

//!creates the model for the tree, gtkliststore
static GtkTreeModel *create_model()
{
  GtkListStore * model = gtk_list_store_new(NUM_COLUMNS,
      G_TYPE_BOOLEAN,
      G_TYPE_STRING,
      G_TYPE_INT,
      G_TYPE_INT, 
      G_TYPE_INT, 
      G_TYPE_STRING,
      G_TYPE_STRING,
      G_TYPE_STRING,
      //tags
      G_TYPE_STRING,
      G_TYPE_STRING,
      G_TYPE_STRING,
      G_TYPE_STRING,
      G_TYPE_INT,
      G_TYPE_INT,
      G_TYPE_STRING);

  return GTK_TREE_MODEL(model);
}

//!order the number column
static void recompute_length_column(ui_state *ui)
{
  gint line_mins, line_secs, line_hundr;
  gint line1_mins, line1_secs, line1_hundr;

  gchar new_length_string[30];

  GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->tree_view);

  gint number = 0;
  for (number = 0;number < ui->infos->splitnumber; number++)
  {
    GtkTreePath *path = gtk_tree_path_new_from_indices(number ,-1);
    GtkTreeIter iter;
    gtk_tree_model_get_iter(model, &iter, path);

    if (number != ui->infos->splitnumber-1)
    {
      GtkTreePath *path2 = gtk_tree_path_new_from_indices (number+1 ,-1);
      GtkTreeIter iter2;
      gtk_tree_model_get_iter(model, &iter2, path2);

      gtk_tree_model_get(GTK_TREE_MODEL(model), &iter,
          COL_MINUTES, &line_mins,
          COL_SECONDS, &line_secs,
          COL_HUNDR_SECS, &line_hundr,
          -1);
      gtk_tree_model_get(GTK_TREE_MODEL(model), &iter2,
          COL_MINUTES, &line1_mins,
          COL_SECONDS, &line1_secs,
          COL_HUNDR_SECS, &line1_hundr,
          -1);

      gint result_secs = 0,result_hundr = 0;
      gint result_mins = line1_mins - line_mins;
      if ((result_secs = line1_secs - line_secs) < 0)
      {
        result_secs = 60 - line_secs + line1_secs;
        result_mins--;
      }

      if ((result_hundr = line1_hundr - line_hundr) < 0)
      {
        result_hundr = 100 - line_hundr + line1_hundr;
        result_secs--;
        if (result_secs < 0)
        {
          result_mins--;
          result_secs = 0;                    
        }
      }

      g_snprintf(new_length_string, 30, "%d:%02d:%02d", result_mins, result_secs, result_hundr);

      gtk_tree_path_free(path2);
    }
    else
    {
      g_snprintf(new_length_string, 30, "%s","-");
    }

    gtk_tree_path_free(path);

    gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_NUMBER, new_length_string, -1);
  }
}

/*! checks if the name of the current track is already in use

\param descr The name of the current track
\param number The number of the track we don't want to compare our
track name with
\result TRUE if the new track name for track (number) is unique
*/
static gboolean check_if_description_exists(gchar *descr, gint number, ui_state *ui)
{
  GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->tree_view);

  GtkTreeIter iter;
  if (!gtk_tree_model_get_iter_first(model, &iter))
  {
    return TRUE;
  }

  gint count = 0;
  while (TRUE)
  {
    gchar *description = NULL;
    gtk_tree_model_get(GTK_TREE_MODEL(model), &iter,
        COL_DESCRIPTION, &description,
        -1);

    if (description != NULL && strcmp(descr, description) == 0 && count != number)
    {
      g_free(description);
      return FALSE;
    }

    g_free(description);

    if (!gtk_tree_model_iter_next(model, &iter))
      break;

    count++;
  }

  return TRUE;
}

//!Gets the number of the first splitpoint with selected "Keep" checkbox 
gint get_first_splitpoint_selected(gui_state *gui)
{
  gint splitpoint_selected = -1;

  GtkTreeModel *model = gtk_tree_view_get_model(gui->tree_view);
  GtkTreeSelection *selection = gtk_tree_view_get_selection(gui->tree_view);

  GList *selected_list = 
    gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(selection), &model);

  if (g_list_length(selected_list) > 0)
  {
    GList *current_element = g_list_first(selected_list);
    GtkTreePath *path = current_element->data;
    splitpoint_selected = gtk_tree_path_get_indices (path)[0];

    g_list_foreach(selected_list, (GFunc)gtk_tree_path_free, NULL);
    g_list_free(selected_list);
  }

  return splitpoint_selected;
}

//!row selection event
static void row_selection_event(GtkTreeSelection *selection, ui_state *ui)
{
  gtk_widget_set_sensitive(ui->gui->remove_row_button, TRUE); 
}

/*! Set the name of the splitpoint (number) to (descr)

\param descr the new name of the split point
\param number The number of the split point

If any split point with a different number already uses the name we
want we chose for this split point this function adds a number as a
postfix (or updates the already-existing postfix) to force the new
splitpoint's name. do be unique.
*/
static void update_current_description(gchar *descr, gint number, ui_state *ui)
{
  gint ll = 0;

  gchar *current_description = ui->status->current_description;

  g_snprintf(current_description, 255, "%s", descr);

  while (ll < ui->infos->splitnumber)
  {
    if (check_if_description_exists(current_description, number, ui))
    {
      ll++;
      continue;
    }

    //we cut the part _* from the string and put it back
    gchar *tmp = NULL;
    gchar *t = current_description;
    while ((t = strstr(t, _("_part"))) != NULL)
    {
      tmp = t++;
    }

    if (tmp != NULL)
    {
      *tmp = '\0';
    }

    gchar *temp = g_strdup(current_description);
    g_snprintf(current_description, 255, _("%s_part%d"), temp, ll + 2);
    g_free(temp);

    ll++;
  }
}

/*!returns secs, mins, hundr of secs from a time

not used for now
*/
void get_hundr_secs_mins_time(gint time_pos, gint *time_hundr,
                              gint *time_secs,gint *time_mins)
{
  *time_hundr = time_pos % 100;
  time_pos = time_pos / 100;
  *time_secs = time_pos % 60;
  time_pos = time_pos / 60;
  *time_mins = time_pos;
}

//!selects a splitpoint
void select_splitpoint(gint index, gui_state *gui)
{
  GtkTreeSelection *selection = gtk_tree_view_get_selection(gui->tree_view);
  GtkTreeModel *model = gtk_tree_view_get_model(gui->tree_view);
  GtkTreePath *path = gtk_tree_path_new_from_indices(index ,-1);

  GtkTreeIter iter;
  gtk_tree_model_get_iter(model, &iter, path);
  gtk_tree_selection_unselect_all(selection);
  gtk_tree_selection_select_iter(selection, &iter);

  gtk_tree_path_free(path);

  remove_status_message(gui);
}

static void order_all_splitpoints_from_table(const char *current_description_base, 
    GtkTreeModel *model, ui_state *ui)
{
  GtkTreeIter iter;
  if (!gtk_tree_model_get_iter_first(model, &iter))
  {
    return;
  }

  int description_base_length = strlen(current_description_base);

  gint i = 0;
  gint description_counter = 0;
  while (i < ui->infos->splitnumber)
  {
    gchar *description = NULL;
    gtk_tree_model_get(GTK_TREE_MODEL(model), &iter,
        COL_DESCRIPTION, &description,
        -1);

    int length = strlen(description);
    if (length >= description_base_length)
    {
      if (strncmp(description, current_description_base, description_base_length) == 0)
      {
        GString *new_description = g_string_new("");
        g_string_append_printf(new_description, "%s_part%d", current_description_base,
            i + 1);
        gchar *new_desc = g_string_free(new_description, FALSE);

        gtk_list_store_set(GTK_LIST_STORE(model), 
            &iter,
            COL_DESCRIPTION, new_desc,
            -1);

        g_free(new_desc);
        description_counter++;
      }
    }

    g_free(description);

    gtk_tree_model_iter_next(model, &iter);
    i++;
  }
}

/*! removes a splitpoint
\param index Number of the split point
\param stop_preview means we stop preview if necessary
*/
void remove_splitpoint(gint index, gint stop_preview, ui_state *ui)
{
  g_array_remove_index(ui->splitpoints, index);

  GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->tree_view);
  GtkTreePath *path = gtk_tree_path_new_from_indices (index ,-1);

  GtkTreeIter iter;
  gtk_tree_model_get_iter(model, &iter, path);

  //cancel quick preview if necessary
  if (((index == ui->status->preview_start_splitpoint) && stop_preview) ||
      ((index == get_quick_preview_end_splitpoint_safe(ui)) &&
       (get_quick_preview_end_splitpoint_safe(ui) == (ui->infos->splitnumber-1)) && stop_preview))
  {
    cancel_quick_preview_all(ui);
  }

  //if we remove a point at the left of the play preview, move the indexes
  if (index < ui->status->preview_start_splitpoint)
  {
    ui->status->preview_start_splitpoint--;
    set_quick_preview_end_splitpoint_safe(ui->status->preview_start_splitpoint + 1, ui);
  }

  gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
  gtk_tree_path_free(path);

  ui->infos->splitnumber--;

  if (get_first_splitpoint_selected(ui->gui) == -1)
  {
    gtk_widget_set_sensitive(ui->gui->remove_row_button, FALSE);  
  }

  if (ui->infos->splitnumber == 0)
  {
    gtk_widget_set_sensitive(ui->gui->remove_all_button, FALSE);
  }

  if (stop_preview)
  {
    order_all_splitpoints_from_table(ui->status->current_description, model, ui);
  }

  recompute_length_column(ui);
  remove_status_message(ui->gui);
  update_add_button(ui);
  check_update_down_progress_bar(ui);
  refresh_drawing_area(ui->gui);

  export_cue_file_in_configuration_directory(ui);
}

/*! adds a splitpoint

\param my_split_point The data for our new split point
\param old_index used when we update a splitpoint to see where we had
the play_preview point 
*/
static void add_splitpoint(Split_point my_split_point, gint old_index, ui_state *ui, 
    gint reorder_names, gchar *old_description)
{
  gchar *current_description_base = g_strdup(ui->status->current_description);

  GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->tree_view);

  if (check_if_splitpoint_does_not_exists(my_split_point.mins,
        my_split_point.secs, my_split_point.hundr_secs,-1, ui))
  {
    gint k = 0;

    update_current_description(current_description_base, -1, ui);

    GtkTreeIter iter;
    if (gtk_tree_model_get_iter_first(model, &iter))
    {
      while (k < ui->infos->splitnumber)
      {
        gint tree_minutes;
        gint tree_seconds;
        gint tree_hundr_secs;
        gtk_tree_model_get(GTK_TREE_MODEL(model), &iter,
            COL_MINUTES, &tree_minutes,
            COL_SECONDS, &tree_seconds,
            COL_HUNDR_SECS, &tree_hundr_secs,
            -1);

        if (my_split_point.mins < tree_minutes)
        {
          break;
        }
        else if (my_split_point.mins == tree_minutes)
        {
          if (my_split_point.secs < tree_seconds)
          {
            break;
          }
          else if (my_split_point.secs == tree_seconds)
          {
            if (my_split_point.hundr_secs < tree_hundr_secs)
            {
              break;
            }
          }
        }

        gtk_tree_model_iter_next(model, &iter);
        k++;
      }

      gtk_list_store_insert(GTK_LIST_STORE(model), &iter,k--);
      g_array_insert_val(ui->splitpoints, k+1, my_split_point);     
    }
    else
    {
      gtk_list_store_append(GTK_LIST_STORE(model), &iter);
      g_array_append_val(ui->splitpoints, my_split_point);
    }

    ui->infos->splitnumber++;

    //we keep the selection on the previous splipoint
    if ((ui->status->first_splitpoint_selected == old_index) &&
        (old_index != -1))
    {
      GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
      gtk_tree_view_set_cursor(ui->gui->tree_view, path, NULL, FALSE);
      gtk_tree_path_free(path);
    }

    if (ui->status->quick_preview)
    {
      //if we move the current start preview splitpoint
      //at the right of the current time, we cancel preview
      if (old_index == ui->status->preview_start_splitpoint)
      {
        if (ui->infos->current_time < get_splitpoint_time(ui->status->preview_start_splitpoint, ui))
        {
          cancel_quick_preview(ui->status);
        }
      }
    }

    //we manage the play preview here
    if (old_index != -1)
    {
      //if we have a split preview on going
      //if we move the point from the left to the right of the
      //the start preview splitpoint
      if ((old_index < ui->status->preview_start_splitpoint))
      {
        if ((k+1) >= ui->status->preview_start_splitpoint)
        {
          ui->status->preview_start_splitpoint--;
          set_quick_preview_end_splitpoint_safe(ui->status->preview_start_splitpoint + 1, ui);
        }
      }
      else
      {
        //if we move from the right of the split preview to his left
        if ((old_index > ui->status->preview_start_splitpoint))
        {
          if ((k+1) <= ui->status->preview_start_splitpoint)
          {
            ui->status->preview_start_splitpoint++;
            set_quick_preview_end_splitpoint_safe(ui->status->preview_start_splitpoint + 1, ui);
          }
        }
        else
        {
          //if we move the start splitpoint on the right of the end splitpoint
          if (old_index == ui->status->preview_start_splitpoint)
          {
            if ((k+1) > ui->status->preview_start_splitpoint)
            {
              ui->status->preview_start_splitpoint += (k+1) - ui->status->preview_start_splitpoint;
              set_quick_preview_end_splitpoint_safe(ui->status->preview_start_splitpoint + 1, ui);
            }
            else
            {
              //if we move the start splitpoint at the left
              if ((k+1) < ui->status->preview_start_splitpoint)
              {
                ui->status->preview_start_splitpoint -= ui->status->preview_start_splitpoint - (k + 1);
                set_quick_preview_end_splitpoint_safe(ui->status->preview_start_splitpoint + 1, ui);
              }
            }
          }
        }
      }

      if (ui->status->preview_start_splitpoint == (ui->infos->splitnumber-1))
      {
        cancel_quick_preview_all(ui);
      }
    }
    else
    {
      //if we add a splitpoint at the left of the quick
      //preview start, add 1
      if ((k+1) <= ui->status->preview_start_splitpoint)
      {
        ui->status->preview_start_splitpoint++;
        set_quick_preview_end_splitpoint_safe(ui->status->preview_start_splitpoint + 1, ui);
      }
    }

    //put values in the line
    //sets text in the minute, second and milisecond column
    gtk_list_store_set(GTK_LIST_STORE(model), 
        &iter,
        COL_CHECK, my_split_point.checked,
        COL_DESCRIPTION, ui->status->current_description,
        COL_MINUTES, my_split_point.mins,
        COL_SECONDS, my_split_point.secs,
        COL_HUNDR_SECS, my_split_point.hundr_secs,
        -1);

    gtk_widget_set_sensitive(ui->gui->remove_all_button, TRUE);

    recompute_length_column(ui);
    remove_status_message(ui->gui);
  }
  else
  {
    put_status_message(_(" error: you already have the splitpoint in table"), ui);
  }

  if (reorder_names)
  {
    if (old_description)
    {
      order_all_splitpoints_from_table(old_description, model, ui);
    }
    else
    {
      order_all_splitpoints_from_table(current_description_base, model, ui);
    }
  }

  if (old_description) { g_free(old_description); }
  if (current_description_base) { g_free(current_description_base); }

  if (gtk_toggle_button_get_active(ui->gui->names_from_filename))
  {
    copy_filename_to_current_description(get_input_filename(ui->gui), ui);
  }
  else
  {
    g_snprintf(ui->status->current_description, 255, "%s", _("description here"));
  }

  update_add_button(ui);
  refresh_drawing_area(ui->gui);
  check_update_down_progress_bar(ui);

  export_cue_file_in_configuration_directory(ui);
}

/*!Set all values of a split point

\param new_point All values for this split point
\param index The number of this split point

Will display an error in the message bar if a splitpoint with a
different index number with exactly the same time value exists and
otherwise update the split point.
*/
void update_splitpoint(gint index, Split_point new_point, ui_state *ui)
{
  int splitpoint_does_not_exists = 
    check_if_splitpoint_does_not_exists(new_point.mins, new_point.secs, new_point.hundr_secs,-1, ui);

  Split_point old_point = g_array_index(ui->splitpoints, Split_point, index);

  if (splitpoint_does_not_exists ||
      (old_point.checked != new_point.checked))
  {
    ui->status->lock_cue_export = SPLT_TRUE;

    ui->status->first_splitpoint_selected = get_first_splitpoint_selected(ui->gui);

    gchar *old_description = g_strdup(ui->status->current_description);

    gchar *description = get_splitpoint_name(index, ui);
    g_snprintf(ui->status->current_description, 255, "%s", description);
    g_free(description);

    //backup tags
    GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->tree_view);
    GtkTreePath *path = gtk_tree_path_new_from_indices(index ,-1);
    GtkTreeIter iter;
    gtk_tree_model_get_iter(model, &iter, path);

    gint year = 0, track = 0;
    gchar *title = NULL, *artist = NULL, *album = NULL, *genre = NULL, *comment = NULL;
    gtk_tree_model_get(GTK_TREE_MODEL(model), &iter,
        COL_TITLE, &title,
        COL_ARTIST, &artist,
        COL_ALBUM, &album,
        COL_GENRE, &genre,
        COL_COMMENT, &comment,
        COL_YEAR, &year,
        COL_TRACK, &track,
        -1);

    remove_splitpoint(index, FALSE, ui);
    add_splitpoint(new_point, index, ui, TRUE, old_description);

    //restore tags
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_path_free(path);

    gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_TITLE, title, -1);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_ARTIST, artist, -1);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_ALBUM, album, -1);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_GENRE, genre, -1);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_COMMENT, comment, -1);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_YEAR, year, -1);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter, COL_TRACK, track, -1);
    free(title); free(artist); free(album); free(genre); free(comment);

    ui->status->lock_cue_export = SPLT_FALSE;

    export_cue_file_in_configuration_directory(ui);
  }
  else
  {
    //don't put error if we move the same splitpoint
    //on the same place
    if ((new_point.mins == old_point.mins) &&
        (new_point.secs == old_point.secs) &&
        (new_point.hundr_secs == old_point.hundr_secs))
    {
    }
    else
    {
      put_status_message(_(" error: you already have the splitpoint in table"), ui);
    }
  }
}

/*!Set a splitpoint's time value
\param index The split point's number
\param time the new time value
*/
void update_splitpoint_from_time(gint index, gdouble time, ui_state *ui)
{
  Split_point new_point;
  get_hundr_secs_mins_time((gint)time, &new_point.hundr_secs, &new_point.secs, &new_point.mins);
  Split_point old_point = g_array_index(ui->splitpoints, Split_point, index);
  new_point.checked = old_point.checked;

  update_splitpoint(index, new_point, ui);
}

/*!Toggles a splitpoint's "Keep" flag

\param index is the position in the GArray with splitpoints aka the
split point's number
*/
void update_splitpoint_check(gint index, ui_state *ui)
{
  Split_point old_point = g_array_index(ui->splitpoints, Split_point, index);
  old_point.checked ^= 1;
  update_splitpoint(index, old_point, ui);
}

void clear_current_description(ui_state *ui)
{
  update_current_description(_("description here"), -1, ui);
}

void copy_filename_to_current_description(const gchar *fname, ui_state *ui)
{
  if (strcmp(fname, "") == 0)
  {
    clear_current_description(ui);
  }

  gchar *temp = g_strdup(g_path_get_basename(fname));
  gchar *tmp = strrchr(temp,'.');
  if (tmp != NULL) { *tmp = '\0'; }

  g_snprintf(ui->status->current_description, 255, "%s", temp);
  g_free(temp);
}

//!event for editing a cell
static void cell_edited_event(GtkCellRendererText *cell, gchar *path_string, gchar *new_text, ui_state *ui)
{
  GtkTreePath *path = gtk_tree_path_new_from_string (path_string);
  GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->tree_view);

  gint col = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(cell), "col"));

  GtkTreeIter iter;
  gtk_tree_model_get_iter(model, &iter, path);
  gint i = gtk_tree_path_get_indices (path)[0];
  Split_point old_point = g_array_index(ui->splitpoints, Split_point, i);

  Split_point new_point;
  new_point.checked = old_point.checked;

  switch (col)
  {
    case COL_DESCRIPTION:
      update_current_description(new_text, i, ui);

      //put the new content in the list
      gtk_list_store_set(GTK_LIST_STORE(model), &iter,
          col, ui->status->current_description,
          -1);

      if (gtk_toggle_button_get_active(ui->gui->names_from_filename))
      {
        copy_filename_to_current_description(get_input_filename(ui->gui), ui);
      }
      else
      {
        g_snprintf(ui->status->current_description, 255, "%s", _("description here"));
      }
      break;
      //seconds column
    case COL_SECONDS:
      new_point.mins = old_point.mins;
      new_point.secs = atoi(new_text);
      new_point.hundr_secs = old_point.hundr_secs;

      if (new_point.secs < 0)
      {
        new_point.secs = 0;
      }
      if (new_point.secs > 59)
      {
        new_point.secs = 59;
      }

      update_splitpoint(i, new_point, ui);
      break;
      //minutes column
    case COL_MINUTES:
      new_point.mins = atoi(new_text);
      new_point.secs = old_point.secs;
      new_point.hundr_secs = old_point.hundr_secs;

      if (new_point.mins < 0)
      {
        new_point.mins = 0;
      }
      if (new_point.mins > INT_MAX/6000)
      {
        new_point.mins = INT_MAX/6000;
      }

      update_splitpoint(i, new_point, ui);
      break;
      //hundreth column
    case COL_HUNDR_SECS:
      new_point.mins = old_point.mins;
      new_point.secs = old_point.secs;
      new_point.hundr_secs = atoi(new_text);

      if (new_point.hundr_secs < 0)
      {
        new_point.hundr_secs = 0;
      }
      if (new_point.hundr_secs > 99)
      {
        new_point.hundr_secs = 99;
      }

      update_splitpoint(i, new_point, ui);
      break;
    case COL_YEAR:
    case COL_TRACK:
      ;
      gint value = (gint) atoi(new_text);
      if (value < 0) { value = 0; }
      gtk_list_store_set(GTK_LIST_STORE(model), &iter, col, value, -1);
      break;
    default:
      gtk_list_store_set(GTK_LIST_STORE(model), &iter, col, new_text, -1);
      break;
  }

  gtk_tree_path_free(path);

  export_cue_file_in_configuration_directory(ui);
}

void add_splitpoint_from_player(GtkWidget *widget, ui_state *ui)
{
  if (!ui->status->timer_active)
  { 
    return;
  }

  Split_point my_split_point;
  my_split_point.mins = ui->infos->player_minutes;
  my_split_point.secs = ui->infos->player_seconds;
  my_split_point.hundr_secs = ui->infos->player_hundr_secs;
  my_split_point.checked = TRUE;

  add_splitpoint(my_split_point, -1, ui, TRUE, NULL);
}

//!adds a row to the table
void add_row(gboolean checked, ui_state *ui)
{
  gui_status *status = ui->status;

  Split_point my_split_point;
  my_split_point.mins = status->spin_mins;
  my_split_point.secs = status->spin_secs;
  my_split_point.hundr_secs = status->spin_hundr_secs;
  my_split_point.checked = checked;
  
  add_splitpoint(my_split_point, -1, ui, FALSE, NULL);
}

static void add_row_clicked(GtkWidget *button, ui_state *ui)
{
  gui_status *status = ui->status;

  Split_point my_split_point;
  my_split_point.mins = status->spin_mins;
  my_split_point.secs = status->spin_secs;
  my_split_point.hundr_secs = status->spin_hundr_secs;
  my_split_point.checked = TRUE;
  
  add_splitpoint(my_split_point, -1, ui, TRUE, NULL);
}

static gboolean detect_silence_and_set_splitpoints_end(ui_with_err *ui_err)
{
  gint err = ui_err->err;
  ui_state *ui = ui_err->ui;

  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_PRETEND_TO_SPLIT, SPLT_FALSE);
  mp3splt_set_split_filename_function(ui->mp3splt_state, lmanager_put_split_filename, ui);

  if (err >= 0)
  {
    update_splitpoints_from_mp3splt_state(ui);
  }

  print_status_bar_confirmation(err, ui);

  gtk_widget_set_sensitive(ui->gui->cancel_button, FALSE);
  gtk_widget_set_sensitive(ui->gui->scan_silence_button, TRUE);
  gtk_widget_set_sensitive(ui->gui->scan_silence_button_player, TRUE);
  gtk_widget_set_sensitive(ui->gui->scan_trim_silence_button, TRUE);
  gtk_widget_set_sensitive(ui->gui->scan_trim_silence_button_player, TRUE);

  set_is_splitting_safe(FALSE, ui);

  set_process_in_progress_and_wait_safe(FALSE, ui);

  g_free(ui_err);

  return FALSE;
}

static void set_should_trim_safe(gboolean value, ui_state *ui)
{
  lock_mutex(&ui->variables_mutex);
  ui->status->should_trim = value;
  unlock_mutex(&ui->variables_mutex);
}

static gint get_should_trim_safe(ui_state *ui)
{
  lock_mutex(&ui->variables_mutex);
  gint should_trim = ui->status->should_trim;
  unlock_mutex(&ui->variables_mutex);
  return should_trim;
}

//!set splitpints from silence detection
static gpointer detect_silence_and_set_splitpoints(ui_state *ui)
{
  set_process_in_progress_and_wait_safe(TRUE, ui);

  set_is_splitting_safe(TRUE, ui);

  gint err = SPLT_OK;

  enter_threads();

  gtk_widget_set_sensitive(ui->gui->scan_silence_button, FALSE);
  gtk_widget_set_sensitive(ui->gui->scan_silence_button_player, FALSE);
  gtk_widget_set_sensitive(ui->gui->scan_trim_silence_button, FALSE);
  gtk_widget_set_sensitive(ui->gui->scan_trim_silence_button_player, FALSE);
  gtk_widget_set_sensitive(ui->gui->cancel_button, TRUE);
  gchar *format = strdup(gtk_entry_get_text(GTK_ENTRY(ui->gui->output_entry)));

  lock_mutex(&ui->variables_mutex);
  mp3splt_set_filename_to_split(ui->mp3splt_state, get_input_filename(ui->gui));
  unlock_mutex(&ui->variables_mutex);

  gint checked_output_radio_box = get_checked_output_radio_box(ui);

  exit_threads();

  err = mp3splt_erase_all_splitpoints(ui->mp3splt_state);

  if (checked_output_radio_box == 0)
  {
    err = mp3splt_set_oformat(ui->mp3splt_state, format);
  }

  if (format)
  {
    free(format);
    format = NULL;
  }

  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_PRETEND_TO_SPLIT, SPLT_TRUE);
  mp3splt_set_split_filename_function(ui->mp3splt_state, NULL, ui);
  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_TAGS, SPLT_TAGS_ORIGINAL_FILE);

  print_status_bar_confirmation_in_idle(err, ui);

  err = SPLT_OK;
  int old_split_mode = mp3splt_get_int_option(ui->mp3splt_state, SPLT_OPT_SPLIT_MODE, &err);
  int old_tags_option = mp3splt_get_int_option(ui->mp3splt_state, SPLT_OPT_TAGS, &err);

  if (get_should_trim_safe(ui))
  {
    err = mp3splt_set_trim_silence_points(ui->mp3splt_state);
  }
  else
  {
    mp3splt_set_silence_points(ui->mp3splt_state, &err);
  }

  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_TAGS, old_tags_option);
  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_SPLIT_MODE, old_split_mode);

  ui_with_err *ui_err = g_malloc0(sizeof(ui_with_err));
  ui_err->err = err;
  ui_err->ui = ui;

  gdk_threads_add_idle_full(G_PRIORITY_HIGH_IDLE, 
      (GSourceFunc)detect_silence_and_set_splitpoints_end, ui_err, NULL);

  return NULL;
}

static void detect_silence_and_set_splitpoints_action(ui_state *ui)
{
  create_thread((GThreadFunc)detect_silence_and_set_splitpoints, ui);
}

//!start thread with 'set splitpints from silence detection'
static void detect_silence_and_add_splitpoints_start_thread(ui_state *ui)
{
  set_should_trim_safe(FALSE, ui);
  detect_silence_and_set_splitpoints_action(ui);
}

static void detect_silence_and_add_trim_splitpoints_start_thread(ui_state *ui)
{
  set_should_trim_safe(TRUE, ui);
  detect_silence_and_set_splitpoints_action(ui);
}

//!update silence parameters when 'widget' changes
static void update_silence_parameters(GtkWidget *widget, ui_state *ui)
{
  ui_infos *infos = ui->infos;
  gui_state *gui = ui->gui;

  infos->silence_threshold_value = 
    gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui->spinner_silence_threshold));
  if (gui->spinner_silence_offset != NULL)
  {
    infos->silence_offset_value =
      gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui->spinner_silence_offset));
  }
  if (gui->spinner_silence_number_tracks != NULL)
  {
    infos->silence_number_of_tracks =
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(gui->spinner_silence_number_tracks));
  }
  if (gui->spinner_silence_minimum != NULL)
  {
    infos->silence_minimum_length = 
      gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui->spinner_silence_minimum));
  }
  if (gui->spinner_silence_minimum_track != NULL)
  {
    infos->silence_minimum_track_length = 
      gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui->spinner_silence_minimum_track));
  }
  if (gui->silence_remove_silence != NULL)
  {
    infos->silence_remove_silence_between_tracks = 
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui->silence_remove_silence));
  }
}

//!action when checking the 'remove silence' button
static void silence_remove_silence_checked(GtkToggleButton *button, ui_state *ui)
{
  update_silence_parameters(GTK_WIDGET(button), ui);
}

void create_trim_silence_window(GtkWidget *button, ui_state *ui)
{
  GtkWidget *silence_detection_window =
    gtk_dialog_new_with_buttons(_("Set trim splitpoints using silence detection"),
        GTK_WINDOW(ui->gui->window),
        GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
        GTK_STOCK_OK,
        GTK_RESPONSE_YES,
        GTK_STOCK_CANCEL,
        GTK_RESPONSE_CANCEL,
        NULL);

  gtk_widget_set_size_request(silence_detection_window, 300, 90);

  GtkWidget *general_inside_vbox = wh_vbox_new();

  GtkWidget *horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(general_inside_vbox), horiz_fake, FALSE, FALSE, 10);

  //vertical parameter box
  GtkWidget *param_vbox = wh_vbox_new();
  gtk_box_pack_start(GTK_BOX(horiz_fake), param_vbox, FALSE, FALSE, 25);

  //horizontal box fake for threshold level
  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(param_vbox), horiz_fake, FALSE, FALSE, 0);

  //threshold level
  GtkWidget *label = gtk_label_new(_("Threshold level (dB):"));
  gtk_box_pack_start(GTK_BOX(horiz_fake), label, FALSE, FALSE, 0);

  //adjustement for the threshold spinner
  GtkAdjustment *adj = (GtkAdjustment *) gtk_adjustment_new(0.0, -96.0, 0.0, 0.5, 10.0, 0.0);
  GtkWidget *spinner_silence_threshold = gtk_spin_button_new(adj, 0.5, 2);
  ui->gui->spinner_silence_threshold = spinner_silence_threshold;
  gtk_box_pack_start(GTK_BOX(horiz_fake), spinner_silence_threshold, FALSE, FALSE, 6);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_silence_threshold), ui->infos->silence_threshold_value);
  g_signal_connect(G_OBJECT(spinner_silence_threshold), "value_changed",
      G_CALLBACK(update_silence_parameters), ui);

  gtk_widget_show_all(general_inside_vbox);
  gtk_container_add(GTK_CONTAINER(
        gtk_dialog_get_content_area(GTK_DIALOG(silence_detection_window))), general_inside_vbox);

  gint result = gtk_dialog_run(GTK_DIALOG(silence_detection_window));

  gtk_widget_destroy(silence_detection_window);

  if (result == GTK_RESPONSE_YES)
  {
    mp3splt_set_float_option(ui->mp3splt_state, SPLT_OPT_PARAM_THRESHOLD, ui->infos->silence_threshold_value);
    detect_silence_and_add_trim_splitpoints_start_thread(ui);
  }
}

//!event for clicking the 'detect silence and add splitpoints' button
void create_detect_silence_and_add_splitpoints_window(GtkWidget *button, ui_state *ui)
{
  ui_infos *infos = ui->infos;
  gui_state *gui = ui->gui;

  GtkWidget *silence_detection_window =
    gtk_dialog_new_with_buttons(_("Set splitpoints from silence detection"),
        GTK_WINDOW(gui->window),
        GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
        GTK_STOCK_OK,
        GTK_RESPONSE_YES,
        GTK_STOCK_CANCEL,
        GTK_RESPONSE_CANCEL,
        NULL);

  GtkWidget *general_inside_vbox = wh_vbox_new();
  GtkWidget *horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(general_inside_vbox), horiz_fake, FALSE, FALSE, 10);

  //vertical parameter box
  GtkWidget *param_vbox = wh_vbox_new();
  gtk_box_pack_start(GTK_BOX(horiz_fake), param_vbox, FALSE, FALSE, 25);

  //horizontal box fake for threshold level
  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(param_vbox), horiz_fake, FALSE, FALSE, 0);

  //threshold level
  GtkWidget *label = gtk_label_new(_("Threshold level (dB):"));
  gtk_box_pack_start(GTK_BOX(horiz_fake), label, FALSE, FALSE, 0);

  //adjustement for the threshold spinner
  GtkAdjustment *adj = (GtkAdjustment *) gtk_adjustment_new(0.0, -96.0, 0.0, 0.5, 10.0, 0.0);
  GtkWidget *spinner_silence_threshold = gtk_spin_button_new(adj, 0.5, 2);
  gui->spinner_silence_threshold = spinner_silence_threshold;
  gtk_box_pack_start(GTK_BOX(horiz_fake), spinner_silence_threshold, FALSE, FALSE, 6);

  //horizontal box fake for the offset level
  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(param_vbox), horiz_fake, FALSE, FALSE, 0);

  //offset level
  label = gtk_label_new(_("Cutpoint offset (0 is the begin of silence,"
        "and 1 the end):"));
  gtk_box_pack_start(GTK_BOX(horiz_fake), label, FALSE, FALSE, 0);

  //adjustement for the offset spinner
  adj = (GtkAdjustment *) gtk_adjustment_new(0.0, -2, 2, 0.05, 10.0, 0.0);
  GtkWidget *spinner_silence_offset = gtk_spin_button_new(adj, 0.05, 2);
  gui->spinner_silence_offset = spinner_silence_offset;
  gtk_box_pack_start(GTK_BOX(horiz_fake), spinner_silence_offset, FALSE, FALSE, 6);

  //number of tracks
  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(param_vbox), horiz_fake, FALSE, FALSE, 0);

  label = gtk_label_new(_("Number of tracks (0 means all tracks):"));
  gtk_box_pack_start(GTK_BOX(horiz_fake), label, FALSE, FALSE, 0);

  adj = (GtkAdjustment *)gtk_adjustment_new(0.0, 0, 2000, 1, 10.0, 0.0);
  GtkWidget *spinner_silence_number_tracks = gtk_spin_button_new(adj, 1, 0);
  gui->spinner_silence_number_tracks = spinner_silence_number_tracks;
  gtk_box_pack_start(GTK_BOX(horiz_fake), spinner_silence_number_tracks, FALSE, FALSE, 6);

  //minimum length
  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(param_vbox), horiz_fake, FALSE, FALSE, 0);

  label = gtk_label_new(_("Minimum silence length (seconds):"));
  gtk_box_pack_start(GTK_BOX(horiz_fake), label, FALSE, FALSE, 0);

  adj = (GtkAdjustment *)gtk_adjustment_new(0.0, 0, 2000, 0.5, 10.0, 0.0);
  GtkWidget *spinner_silence_minimum = gtk_spin_button_new(adj, 1, 2);
  gui->spinner_silence_minimum = spinner_silence_minimum;
  gtk_box_pack_start(GTK_BOX(horiz_fake), spinner_silence_minimum, FALSE, FALSE, 6);

  //the minimum track length parameter
  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(param_vbox), horiz_fake, FALSE, FALSE, 0);

  label = gtk_label_new(_("Minimum track length (seconds):"));
  gtk_box_pack_start(GTK_BOX(horiz_fake), label, FALSE, FALSE, 0);

  adj = (GtkAdjustment *)gtk_adjustment_new(0.0, 0, 2000, 0.5, 10.0, 0.0);
  GtkWidget *spinner_silence_minimum_track = gtk_spin_button_new(adj, 1, 2);
  gui->spinner_silence_minimum_track = spinner_silence_minimum_track;
  gtk_box_pack_start(GTK_BOX(horiz_fake), spinner_silence_minimum_track, FALSE, FALSE, 6);

  //remove silence (rm): allows you to remove the silence between
  //tracks
  GtkWidget *silence_remove_silence = gtk_check_button_new_with_mnemonic(_("_Remove silence between tracks"));
  gui->silence_remove_silence = silence_remove_silence;
  gtk_box_pack_start(GTK_BOX(param_vbox), silence_remove_silence, FALSE, FALSE, 0);

  //we set the default parameters for the silence split
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_silence_threshold), 
      infos->silence_threshold_value);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_silence_offset), 
      infos->silence_offset_value);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_silence_number_tracks), 
      infos->silence_number_of_tracks);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_silence_minimum), 
      infos->silence_minimum_length);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_silence_minimum_track),
      infos->silence_minimum_track_length);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(silence_remove_silence),
      infos->silence_remove_silence_between_tracks);

  //add actions when changing the values
  g_signal_connect(G_OBJECT(spinner_silence_threshold), "value_changed",
      G_CALLBACK(update_silence_parameters), ui);
  g_signal_connect(G_OBJECT(spinner_silence_offset), "value_changed",
      G_CALLBACK(update_silence_parameters), ui);
  g_signal_connect(G_OBJECT(spinner_silence_number_tracks), "value_changed",
      G_CALLBACK(update_silence_parameters), ui);
  g_signal_connect(G_OBJECT(spinner_silence_minimum), "value_changed",
      G_CALLBACK(update_silence_parameters), ui);
  g_signal_connect(G_OBJECT(spinner_silence_minimum_track), "value_changed",
      G_CALLBACK(update_silence_parameters), ui);
  g_signal_connect(G_OBJECT(silence_remove_silence), "toggled",
      G_CALLBACK(silence_remove_silence_checked), ui);

  gtk_widget_show_all(general_inside_vbox);
  gtk_container_add(GTK_CONTAINER(
        gtk_dialog_get_content_area(GTK_DIALOG(silence_detection_window))),
      general_inside_vbox);

  gint result = gtk_dialog_run(GTK_DIALOG(silence_detection_window));

  gtk_widget_destroy(silence_detection_window);

  if (result == GTK_RESPONSE_YES)
  {
    mp3splt_set_float_option(ui->mp3splt_state, SPLT_OPT_PARAM_THRESHOLD, 
        infos->silence_threshold_value);
    mp3splt_set_float_option(ui->mp3splt_state, SPLT_OPT_PARAM_OFFSET, 
        infos->silence_offset_value);
    mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_PARAM_NUMBER_TRACKS, 
        infos->silence_number_of_tracks);
    mp3splt_set_float_option(ui->mp3splt_state, SPLT_OPT_PARAM_MIN_LENGTH, 
        infos->silence_minimum_length);
    mp3splt_set_float_option(ui->mp3splt_state, SPLT_OPT_PARAM_MIN_TRACK_LENGTH,
        infos->silence_minimum_track_length);
    mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_PARAM_REMOVE_SILENCE,
        infos->silence_remove_silence_between_tracks);

    detect_silence_and_add_splitpoints_start_thread(ui);
  }
}

//!remove a row from the table
static void remove_row(GtkWidget *widget, ui_state *ui)
{
  GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->tree_view);
  GtkTreeSelection *selection = gtk_tree_view_get_selection(ui->gui->tree_view);

  GList *selected_list = gtk_tree_selection_get_selected_rows(selection, &model);

  while ((g_list_length(selected_list) > 0) && (ui->infos->splitnumber > 0))
  {
    GList *current_element = g_list_last(selected_list);
    GtkTreePath *path = current_element->data;
    gint i = gtk_tree_path_get_indices (path)[0];

    remove_splitpoint(i, TRUE, ui);

    selected_list = g_list_remove(selected_list, path);

    gtk_tree_path_free(path);
  }

  g_list_foreach(selected_list, (GFunc)gtk_tree_path_free, NULL);
  g_list_free(selected_list);
}

//!removes all rows from the table
void remove_all_rows(GtkWidget *widget, ui_state *ui)
{
  GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->tree_view);
  
  while (ui->infos->splitnumber > 0)
  {
    GtkTreeIter iter;
    gtk_tree_model_get_iter_first(model, &iter);
    gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
    g_array_remove_index(ui->splitpoints, (ui->infos->splitnumber-1));
    ui->infos->splitnumber--;
  }
  
  gtk_widget_set_sensitive(ui->gui->remove_all_button, FALSE);
  gtk_widget_set_sensitive(ui->gui->remove_row_button, FALSE);
  
  remove_status_message(ui->gui);
  cancel_quick_preview_all(ui);
  update_add_button(ui);
  refresh_drawing_area(ui->gui);
  check_update_down_progress_bar(ui);

  export_cue_file_in_configuration_directory(ui);
}

//!creates and and initialise a spinner
static GtkWidget *create_init_spinner(GtkWidget *bottomhbox1, gint min, gint max, 
    gchar *label_text, gint type, ui_state *ui)
{
  GtkWidget *spinner_box = wh_vbox_new(); 
  GtkWidget *label = gtk_label_new(label_text);
  gtk_box_pack_start(GTK_BOX(spinner_box), label, TRUE, FALSE, 0);

  GtkAdjustment *adj = (GtkAdjustment *) gtk_adjustment_new(0.0, min, max, 1.0, 10.0, 0.0);
  GtkWidget *spinner = gtk_spin_button_new(adj, 0, 0);
  gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(spinner), TRUE);

  if (type == 0)
  {
    g_signal_connect(G_OBJECT(spinner), "value_changed",
        G_CALLBACK(update_minutes_from_spinner), ui);
  }
  else if (type == 1)
  {
    g_signal_connect(G_OBJECT(spinner), "value_changed",
        G_CALLBACK(update_seconds_from_spinner), ui);
  }
  else
  {
    g_signal_connect(G_OBJECT(spinner), "value_changed",
        G_CALLBACK(update_hundr_secs_from_spinner), ui);
  }

  gtk_box_pack_start(GTK_BOX(spinner_box), spinner, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(bottomhbox1), spinner_box, FALSE, FALSE, 5);

  return spinner;
}

//!minutes ,seconds spinners ; add, delete buttons
static GtkWidget *create_init_spinners_buttons(ui_state *ui)
{
  GtkWidget *hbox = wh_hbox_new();
  gtk_container_set_border_width(GTK_CONTAINER(hbox), 0);

  //0 means spinner minutes
  ui->gui->spinner_minutes = create_init_spinner(hbox, 0, INT_MAX/6000, _("Minutes:"), 0, ui);
  //1 means spinner seconds
  ui->gui->spinner_seconds = create_init_spinner(hbox, 0, 59, _("Seconds:"), 1, ui);
  //2 means spinner hundredth
  ui->gui->spinner_hundr_secs = create_init_spinner(hbox, 0, 99, _("Hundredths:"), 2, ui);

  /* add button */
  GtkWidget *add_button = wh_create_cool_button(GTK_STOCK_ADD, _("_Add"), FALSE);
  ui->gui->add_button = add_button;

  gtk_button_set_relief(GTK_BUTTON(add_button), GTK_RELIEF_NONE);
  gtk_widget_set_sensitive(add_button, TRUE);
  g_signal_connect(G_OBJECT(add_button), "clicked", G_CALLBACK(add_row_clicked), ui);
  gtk_box_pack_start(GTK_BOX(hbox), add_button, FALSE, FALSE, 5);
  gtk_widget_set_tooltip_text(add_button,_("Add splitpoint"));

  /* remove row button */
  GtkWidget *remove_row_button = wh_create_cool_button(GTK_STOCK_REMOVE, _("_Remove"), FALSE);
  ui->gui->remove_row_button = remove_row_button;

  gtk_button_set_relief(GTK_BUTTON(remove_row_button), GTK_RELIEF_NONE);
  gtk_widget_set_sensitive(remove_row_button, FALSE);
  g_signal_connect(G_OBJECT(remove_row_button), "clicked", G_CALLBACK(remove_row), ui);
  gtk_box_pack_start(GTK_BOX(hbox), remove_row_button, FALSE, FALSE, 5);
  gtk_widget_set_tooltip_text(remove_row_button, _("Remove selected splitpoints"));

  /* remove all rows button */
  GtkWidget *remove_all_button = wh_create_cool_button(GTK_STOCK_CLEAR, _("R_emove all"), FALSE);
  ui->gui->remove_all_button = remove_all_button;

  gtk_button_set_relief(GTK_BUTTON(remove_all_button), GTK_RELIEF_NONE);
  gtk_widget_set_sensitive(remove_all_button, FALSE);
  g_signal_connect(G_OBJECT(remove_all_button), "clicked", G_CALLBACK(remove_all_rows), ui);
  gtk_box_pack_start(GTK_BOX(hbox), remove_all_button, FALSE, FALSE, 5);
  gtk_widget_set_tooltip_text(remove_all_button, _("Remove all splitpoints"));

  return hbox;
}

//!special buttons like 'set silence from silence detection'
static void create_init_special_buttons(ui_state *ui)
{
  /* set splitpoints from trim silence detection */
  GtkWidget *scan_trim_silence_button =
    wh_create_cool_button(GTK_STOCK_CUT, _("_Trim splitpoints"), FALSE);
  ui->gui->scan_trim_silence_button = scan_trim_silence_button;
  gtk_widget_set_sensitive(scan_trim_silence_button, TRUE);
  g_signal_connect(G_OBJECT(scan_trim_silence_button), "clicked",
      G_CALLBACK(create_trim_silence_window), ui);
  gtk_widget_set_tooltip_text(scan_trim_silence_button,
      _("Set trim splitpoints using silence detection"));

  /* set splitpoints from silence detection */
  GtkWidget *scan_silence_button =
    wh_create_cool_button(GTK_STOCK_FIND_AND_REPLACE, _("_Silence detection"), FALSE);
  ui->gui->scan_silence_button = scan_silence_button;
  gtk_widget_set_sensitive(scan_silence_button, TRUE);
  g_signal_connect(G_OBJECT(scan_silence_button), "clicked",
      G_CALLBACK(create_detect_silence_and_add_splitpoints_window), ui);
  gtk_widget_set_tooltip_text(scan_silence_button,
      _("Set splitpoints from silence detection"));
}

/*! returns the name of the splitpoint

result must be g_free'd after use
*/
gchar *get_splitpoint_name(gint index, ui_state *ui)
{
  GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->tree_view);

  gchar *description = NULL;

  GtkTreeIter iter;
  if (!gtk_tree_model_get_iter_first(model, &iter))
  {
    return NULL;
  }

  if (index == -1)
  {
    index = 0;
  }

  if (index >= 0)
  {
    GtkTreePath *path = gtk_tree_path_new_from_indices(index ,-1);
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get(GTK_TREE_MODEL(model), &iter,
        COL_DESCRIPTION, &description,
        -1);
    gtk_tree_path_free(path);
  }

  return description;
}

//!returns a splitpoint from the table
gint get_splitpoint_time(gint splitpoint_index, ui_state *ui)
{
  if (splitpoint_index < 0 ||
      splitpoint_index >= ui->splitpoints->len)
  {
    return -1;
  }

  Split_point point = g_array_index(ui->splitpoints, Split_point, splitpoint_index);
  return point.mins * 6000 + point.secs * 100 + point.hundr_secs;
}

static gboolean split_preview_end(ui_with_err *ui_err)
{
  gint err = ui_err->err;
  ui_state *ui = ui_err->ui;

  print_status_bar_confirmation(err, ui);

  gchar *split_file = get_filename_from_split_files(1, ui->gui);
  if (split_file != NULL && err > 0)
  {
    connect_button_event(ui->gui->connect_button, ui);

    change_current_filename(split_file, ui);
    g_free(split_file);

    //0 means start playing
    connect_to_player_with_song(0, ui);
  }

  if (err > 0)
  {
    gtk_progress_bar_set_fraction(ui->gui->percent_progress_bar, 1.0);
    gtk_progress_bar_set_text(ui->gui->percent_progress_bar, _(" finished"));
  }

  set_process_in_progress_and_wait_safe(FALSE, ui_err->ui);

  g_free(ui_err);

  return FALSE;
}

static gpointer split_preview(ui_state *ui)
{
  set_process_in_progress_and_wait_safe(TRUE, ui);

  int err = mp3splt_erase_all_splitpoints(ui->mp3splt_state);
  err = mp3splt_erase_all_tags(ui->mp3splt_state);

  enter_threads();

  splt_point *splitpoint = mp3splt_point_new(get_preview_start_position_safe(ui), NULL);
  mp3splt_point_set_name(splitpoint, "preview");
  mp3splt_point_set_type(splitpoint, SPLT_SPLITPOINT);
  mp3splt_append_splitpoint(ui->mp3splt_state, splitpoint);

  splitpoint = mp3splt_point_new(
      get_splitpoint_time(get_quick_preview_end_splitpoint_safe(ui), ui), NULL);
  mp3splt_point_set_type(splitpoint, SPLT_SKIPPOINT);
  mp3splt_append_splitpoint(ui->mp3splt_state, splitpoint);

  lock_mutex(&ui->variables_mutex);
  mp3splt_set_filename_to_split(ui->mp3splt_state, get_input_filename(ui->gui));
  unlock_mutex(&ui->variables_mutex);

  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_OUTPUT_FILENAMES, SPLT_OUTPUT_CUSTOM);
  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_SPLIT_MODE, SPLT_OPTION_NORMAL_MODE);

  put_options_from_preferences(ui);
  remove_all_split_rows(ui);

  exit_threads();

  gchar *fname_path = get_preferences_filename();
  fname_path[strlen(fname_path) - 18] = '\0';
  mp3splt_set_path_of_split(ui->mp3splt_state, fname_path);
  if (fname_path) { g_free(fname_path); }

  err = mp3splt_split(ui->mp3splt_state);

  ui_with_err *ui_err = g_malloc0(sizeof(ui_with_err));
  ui_err->err = err;
  ui_err->ui = ui;

  gdk_threads_add_idle_full(G_PRIORITY_HIGH_IDLE, (GSourceFunc)split_preview_end, ui_err, NULL);

  return NULL;
}

static void split_preview_action(ui_state *ui)
{
  create_thread((GThreadFunc)split_preview, ui);
}

//!the row clicked event, preview the song
static void preview_song(GtkTreeView *tree_view, GtkTreePath *path,
    GtkTreeViewColumn *col, ui_state *ui)
{
  gint number = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(col), "col"));
  if (number != COL_PREVIEW && number != COL_SPLIT_PREVIEW)
  {
    return;
  }

  if (!ui->status->timer_active)
  {
    put_status_message(_(" cannot preview, not connected to player"), ui);
    return;
  }

  //get the split begin position to find the end position
  gint preview_row = gtk_tree_path_get_indices(path)[0];
  ui->status->preview_row = preview_row;
  if (number == COL_PREVIEW)
  {
    player_quick_preview(preview_row, ui);
  }
  else if (number == COL_SPLIT_PREVIEW)
  {
    set_preview_start_position_safe(get_splitpoint_time(preview_row, ui), ui);
    set_quick_preview_end_splitpoint_safe(preview_row + 1, ui);

    if (ui->status->preview_row + 1 == ui->infos->splitnumber)
    {
      put_status_message(_(" cannot split preview last splitpoint"), ui);
      return;
    }

    split_preview_action(ui);
  }
}

//!toggle 'check' button
static void toggled_splitpoint_event(GtkCellRendererToggle *cell,
    gchar *path_str, ui_state *ui)
{
  GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->tree_view);
  GtkTreePath *path = gtk_tree_path_new_from_string(path_str);
  gboolean checked = FALSE;

  GtkTreeIter  iter;
  gtk_tree_model_get_iter(model, &iter, path);
  gtk_tree_model_get(model, &iter, COL_CHECK, &checked, -1);

  //toggle the value
  checked ^= 1;

  //get the indice
  gint index = gtk_tree_path_get_indices (path)[0];
  Split_point new_point;
  Split_point old_point;

  //put new 'checked' value to splitpoint
  old_point = g_array_index(ui->splitpoints, Split_point, index);
  new_point.mins = old_point.mins;
  new_point.secs = old_point.secs;
  new_point.hundr_secs = old_point.hundr_secs;
  new_point.checked = checked;

  update_splitpoint(index, new_point, ui);

  gtk_tree_path_free(path);
}

static void clone_tag(ui_state *ui, gint column)
{
  gui_state *gui = ui->gui;

  GtkTreeModel *model = gtk_tree_view_get_model(gui->tree_view);
  GtkTreeSelection *selection = gtk_tree_view_get_selection(gui->tree_view);

  GList *selected_list = 
    gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(selection), &model);

  if (g_list_length(selected_list) <= 0)
  {
    return;
  }

  GList *current_element = g_list_first(selected_list);
  GtkTreePath *path = current_element->data;
  GtkTreeIter iter;
  gtk_tree_model_get_iter(model, &iter, path);

  gchar *value = NULL;
  gint int_value = 0;
  if (column == COL_YEAR || column == COL_TRACK)
  {
    gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, column, &int_value, -1);
  }
  else
  {
    gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, column, &value, -1);
  }

  gint number = 0;
  for (number = 0;number < ui->infos->splitnumber; number++)
  {
    GtkTreePath *path2 = gtk_tree_path_new_from_indices(number ,-1);
    GtkTreeIter iter2;
    gtk_tree_model_get_iter(model, &iter2, path2);
    gtk_tree_path_free(path2);

    if (column == COL_YEAR || column == COL_TRACK)
    {
      gtk_list_store_set(GTK_LIST_STORE(model), &iter2, column, int_value, -1);
    }
    else
    {
      gtk_list_store_set(GTK_LIST_STORE(model), &iter2, column, value, -1);
    }
  }

  if (value) { g_free(value); }

  g_list_foreach(selected_list, (GFunc)gtk_tree_path_free, NULL);
  g_list_free(selected_list);
}

static void clone_all_event(GtkMenuItem *menuitem, ui_state *ui)
{
  clone_tag(ui, COL_TITLE);
  clone_tag(ui, COL_ARTIST);
  clone_tag(ui, COL_ALBUM);
  clone_tag(ui, COL_GENRE);
  clone_tag(ui, COL_YEAR);
  clone_tag(ui, COL_TRACK);
  clone_tag(ui, COL_COMMENT);

  export_cue_file_in_configuration_directory(ui);
}

static void clone_title_event(GtkMenuItem *menuitem, ui_state *ui)
{
  clone_tag(ui, COL_TITLE);
  export_cue_file_in_configuration_directory(ui);
}

static void clone_artist_event(GtkMenuItem *menuitem, ui_state *ui)
{
  clone_tag(ui, COL_ARTIST);
  export_cue_file_in_configuration_directory(ui);
}

static void clone_album_event(GtkMenuItem *menuitem, ui_state *ui)
{
  clone_tag(ui, COL_ALBUM);
  export_cue_file_in_configuration_directory(ui);
}

static void clone_genre_event(GtkMenuItem *menuitem, ui_state *ui)
{
  clone_tag(ui, COL_GENRE);
  export_cue_file_in_configuration_directory(ui);
}

static void clone_year_event(GtkMenuItem *menuitem, ui_state *ui)
{
  clone_tag(ui, COL_YEAR);
  export_cue_file_in_configuration_directory(ui);
}

static void clone_track_event(GtkMenuItem *menuitem, ui_state *ui)
{
  clone_tag(ui, COL_TRACK);
  export_cue_file_in_configuration_directory(ui);
}

static void clone_comment_event(GtkMenuItem *menuitem, ui_state *ui)
{
  clone_tag(ui, COL_COMMENT);
  export_cue_file_in_configuration_directory(ui);
}

static void auto_increment_track_event(GtkMenuItem *menuitem, ui_state *ui)
{
  GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->tree_view);
 
  gint number = 0;
  for (number = 0;number < ui->infos->splitnumber; number++)
  {
    GtkTreePath *path2 = gtk_tree_path_new_from_indices(number ,-1);
    GtkTreeIter iter2;
    gtk_tree_model_get_iter(model, &iter2, path2);
    gtk_tree_path_free(path2);
    gtk_list_store_set(GTK_LIST_STORE(model), &iter2, COL_TRACK, GINT_TO_POINTER(number + 1), -1);
  }

  export_cue_file_in_configuration_directory(ui);
}

static void build_and_show_popup_menu(GtkWidget *treeview, GdkEventButton *event, ui_state *ui)
{
  GtkWidget *menu = gtk_menu_new();

  GtkWidget *item = gtk_image_menu_item_new_with_label(_("Clone all tags"));
  GtkWidget *image = gtk_image_new_from_stock(GTK_STOCK_COPY, GTK_ICON_SIZE_MENU);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), image);
  g_signal_connect(item, "activate", G_CALLBACK(clone_all_event), ui);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

  gtk_menu_shell_append(GTK_MENU_SHELL(menu), gtk_separator_menu_item_new());

  item = gtk_image_menu_item_new_with_label(_("Clone title"));
  image = gtk_image_new_from_stock(GTK_STOCK_COPY, GTK_ICON_SIZE_MENU);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), image);
  g_signal_connect(item, "activate", G_CALLBACK(clone_title_event), ui);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

  item = gtk_image_menu_item_new_with_label(_("Clone artist"));
  image = gtk_image_new_from_stock(GTK_STOCK_COPY, GTK_ICON_SIZE_MENU);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), image);
  g_signal_connect(item, "activate", G_CALLBACK(clone_artist_event), ui);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

  item = gtk_image_menu_item_new_with_label(_("Clone album"));
  image = gtk_image_new_from_stock(GTK_STOCK_COPY, GTK_ICON_SIZE_MENU);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), image);
  g_signal_connect(item, "activate", G_CALLBACK(clone_album_event), ui);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

  item = gtk_image_menu_item_new_with_label(_("Clone genre"));
  image = gtk_image_new_from_stock(GTK_STOCK_COPY, GTK_ICON_SIZE_MENU);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), image);
  g_signal_connect(item, "activate", G_CALLBACK(clone_genre_event), ui);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

  item = gtk_image_menu_item_new_with_label(_("Clone year"));
  image = gtk_image_new_from_stock(GTK_STOCK_COPY, GTK_ICON_SIZE_MENU);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), image);
  g_signal_connect(item, "activate", G_CALLBACK(clone_year_event), ui);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

  item = gtk_image_menu_item_new_with_label(_("Clone track"));
  image = gtk_image_new_from_stock(GTK_STOCK_COPY, GTK_ICON_SIZE_MENU);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), image);
  g_signal_connect(item, "activate", G_CALLBACK(clone_track_event), ui);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

  item = gtk_image_menu_item_new_with_label(_("Clone comment"));
  image = gtk_image_new_from_stock(GTK_STOCK_COPY, GTK_ICON_SIZE_MENU);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), image);
  g_signal_connect(item, "activate", G_CALLBACK(clone_comment_event), ui);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

  gtk_menu_shell_append(GTK_MENU_SHELL(menu), gtk_separator_menu_item_new());

  item = gtk_image_menu_item_new_with_label(_("Auto-increment track"));
  image = gtk_image_new_from_stock(GTK_STOCK_GO_DOWN, GTK_ICON_SIZE_MENU);
  gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(item), image);
  g_signal_connect(item, "activate", G_CALLBACK(auto_increment_track_event), ui);
  gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);

  gtk_widget_show_all(menu);

  gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,
      (event != NULL) ? event->button : 0, gdk_event_get_time((GdkEvent*)event));
}

static gboolean show_popup(GtkWidget *treeview, ui_state *ui)
{
  build_and_show_popup_menu(treeview, NULL, ui);
  return TRUE;
}

static gboolean select_and_show_popup(GtkWidget *treeview, GdkEventButton *event, ui_state *ui)
{
  if (event->type != GDK_BUTTON_PRESS || event->button != 3)
  {
    return FALSE;
  }

  GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(treeview));
  if (gtk_tree_selection_count_selected_rows(selection)  <= 1)
  {
    GtkTreePath *path;
    if (gtk_tree_view_get_path_at_pos(GTK_TREE_VIEW(treeview),
          (gint) event->x, (gint) event->y, &path, NULL, NULL, NULL))
    {
      gtk_tree_selection_unselect_all(selection);
      gtk_tree_selection_select_path(selection, path);
      gtk_tree_path_free(path);
    }
    else
    {
      return FALSE;
    }
  }

  build_and_show_popup_menu(treeview, event, ui);

  return TRUE;
}

//!creates columns for the tree
static void create_columns(ui_state *ui)
{
  gui_state *gui = ui->gui;

  GtkTreeView *tree_view = gui->tree_view;

  GtkCellRendererText *renderer;
  GtkCellRendererPixbuf *renderer_pix;
  GtkCellRendererToggle *renderer_toggle;

  GtkTreeViewColumn *column_number;
  GtkTreeViewColumn *column_check = NULL;
  GtkTreeViewColumn *column_description;
  GtkTreeViewColumn *column_hundr_secs;
  GtkTreeViewColumn *column_minutes;
  GtkTreeViewColumn *column_seconds;
  GtkTreeViewColumn *column_preview;
  GtkTreeViewColumn *column_split_preview;

  /* Check point / skip point */
  renderer_toggle = GTK_CELL_RENDERER_TOGGLE(gtk_cell_renderer_toggle_new());
  g_signal_connect(renderer_toggle, "toggled", G_CALLBACK(toggled_splitpoint_event), ui);
  g_object_set_data(G_OBJECT(renderer_toggle), "col", GINT_TO_POINTER(COL_CHECK));
  column_check = gtk_tree_view_column_new_with_attributes
    (_("Keep"), GTK_CELL_RENDERER(renderer_toggle), "active", COL_CHECK, NULL);

  /* description */
  renderer = GTK_CELL_RENDERER_TEXT(gtk_cell_renderer_text_new());
  g_signal_connect(renderer, "edited", G_CALLBACK(cell_edited_event), ui);
  g_object_set(renderer, "editable", TRUE, NULL);
  g_object_set_data(G_OBJECT(renderer), "col", GINT_TO_POINTER(COL_DESCRIPTION));
  column_description = gtk_tree_view_column_new_with_attributes
    (_("Filename"), GTK_CELL_RENDERER(renderer), "text", COL_DESCRIPTION, NULL);

  /* seconds */
  renderer = GTK_CELL_RENDERER_TEXT(gtk_cell_renderer_text_new());
  g_signal_connect(renderer, "edited", G_CALLBACK(cell_edited_event), ui);
  g_object_set(renderer, "editable", TRUE, NULL);
  g_object_set_data(G_OBJECT(renderer), "col", GINT_TO_POINTER(COL_SECONDS));
  column_seconds = gtk_tree_view_column_new_with_attributes
    (_("Secs"), GTK_CELL_RENDERER(renderer), "text", COL_SECONDS, NULL);

  /* minutes */
  renderer = GTK_CELL_RENDERER_TEXT(gtk_cell_renderer_text_new());
  g_signal_connect(renderer, "edited", G_CALLBACK(cell_edited_event), ui);
  g_object_set(renderer, "editable", TRUE, NULL);
  g_object_set_data(G_OBJECT(renderer), "col", GINT_TO_POINTER(COL_MINUTES));
  column_minutes = gtk_tree_view_column_new_with_attributes 
    (_("Mins"), GTK_CELL_RENDERER(renderer), "text", COL_MINUTES, NULL);

  /* hundr secs */
  renderer = GTK_CELL_RENDERER_TEXT(gtk_cell_renderer_text_new());
  g_signal_connect(renderer, "edited", G_CALLBACK(cell_edited_event), ui);
  g_object_set(renderer, "editable", TRUE, NULL);
  g_object_set_data(G_OBJECT(renderer), "col", GINT_TO_POINTER(COL_HUNDR_SECS));
  column_hundr_secs = gtk_tree_view_column_new_with_attributes 
    (_("Hundr"), GTK_CELL_RENDERER(renderer), "text", COL_HUNDR_SECS, NULL);

  /* Length column */
  //renderer creation
  renderer = GTK_CELL_RENDERER_TEXT(gtk_cell_renderer_text_new());
  g_object_set(G_OBJECT (renderer), "xalign", 1.0, NULL);
  g_object_set_data(G_OBJECT(renderer), "col", GINT_TO_POINTER(COL_NUMBER));
  column_number = gtk_tree_view_column_new_with_attributes 
    (_("Length"), GTK_CELL_RENDERER(renderer), "text", COL_NUMBER, NULL);

  /* column preview */
  renderer_pix = GTK_CELL_RENDERER_PIXBUF(gtk_cell_renderer_pixbuf_new());
  g_object_set(renderer_pix, "stock-id", GTK_STOCK_MEDIA_PLAY,
      "stock-size", GTK_ICON_SIZE_MENU, NULL);
  column_preview = gtk_tree_view_column_new_with_attributes 
    (_("LiveP"), GTK_CELL_RENDERER(renderer_pix), NULL);
  g_object_set_data(G_OBJECT(column_preview), "col", GINT_TO_POINTER(COL_PREVIEW));

  /* split preview */
  renderer_pix = GTK_CELL_RENDERER_PIXBUF(gtk_cell_renderer_pixbuf_new());
  g_object_set(renderer_pix, "stock-id", GTK_STOCK_MEDIA_PLAY,
      "stock-size", GTK_ICON_SIZE_MENU, NULL);
  column_split_preview = gtk_tree_view_column_new_with_attributes 
    (_("SplitP"), GTK_CELL_RENDERER(renderer_pix), NULL);
  g_object_set_data(G_OBJECT(column_split_preview), "col", GINT_TO_POINTER(COL_SPLIT_PREVIEW));

  gtk_tree_view_insert_column(GTK_TREE_VIEW(tree_view), column_check, COL_CHECK);
  gtk_tree_view_insert_column(GTK_TREE_VIEW(tree_view), column_description, COL_DESCRIPTION);
  gtk_tree_view_insert_column(GTK_TREE_VIEW(tree_view), column_minutes, COL_MINUTES);
  gtk_tree_view_insert_column(GTK_TREE_VIEW(tree_view), column_seconds, COL_SECONDS);
  gtk_tree_view_insert_column(GTK_TREE_VIEW(tree_view), column_hundr_secs, COL_HUNDR_SECS);
  gtk_tree_view_insert_column(GTK_TREE_VIEW(tree_view), column_number, COL_NUMBER);
  gtk_tree_view_insert_column(GTK_TREE_VIEW(tree_view), column_preview, COL_PREVIEW);
  gtk_tree_view_insert_column(GTK_TREE_VIEW(tree_view), column_split_preview, COL_SPLIT_PREVIEW);

  gtk_tree_view_column_set_alignment(column_check, 0.5);
  gtk_tree_view_column_set_alignment(column_description, 0.5);
  gtk_tree_view_column_set_alignment(column_minutes, 0.5);
  gtk_tree_view_column_set_alignment(column_seconds, 0.5);
  gtk_tree_view_column_set_alignment(column_hundr_secs, 0.5);
  gtk_tree_view_column_set_alignment(column_number, 0.5);
  gtk_tree_view_column_set_alignment(column_preview, 0.5);
  gtk_tree_view_column_set_alignment(column_split_preview, 0.5);

  gtk_tree_view_column_set_sizing(column_check, GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_fixed_width(column_check, 70);

  gtk_tree_view_column_set_resizable(column_description, TRUE);

  gtk_tree_view_column_set_reorderable(column_check, TRUE);
  gtk_tree_view_column_set_reorderable(column_description, TRUE);
  gtk_tree_view_column_set_reorderable(column_minutes, TRUE);
  gtk_tree_view_column_set_reorderable(column_seconds, TRUE);
  gtk_tree_view_column_set_reorderable(column_hundr_secs, TRUE);
  gtk_tree_view_column_set_reorderable(column_number, TRUE);
  gtk_tree_view_column_set_reorderable(column_preview, TRUE);
  gtk_tree_view_column_set_reorderable(column_split_preview, TRUE);

  gtk_tree_view_column_set_expand(column_description, TRUE);

  gint i = 0;
  for (i = COL_TITLE;i < NUM_COLUMNS;i++)
  {
    renderer = GTK_CELL_RENDERER_TEXT(gtk_cell_renderer_text_new());
    g_signal_connect(renderer, "edited", G_CALLBACK(cell_edited_event), ui);
    g_object_set(renderer, "editable", TRUE, NULL);
    g_object_set_data(G_OBJECT(renderer), "col", GINT_TO_POINTER(i));

    gchar column_name[255] = { '\0' };
    gint minimum_width = 100;
    switch (i)
    {
      case COL_TITLE:
        g_snprintf(column_name, 255, _("Title"));
        break;
      case COL_ARTIST:
        g_snprintf(column_name, 255, _("Artist"));
        break;
      case COL_ALBUM:
        g_snprintf(column_name, 255, _("Album"));
        break;
      case COL_GENRE:
        g_snprintf(column_name, 255, _("Genre"));
        minimum_width = 70;
        break;
      case COL_COMMENT:
        g_snprintf(column_name, 255, _("Comment"));
        break;
      case COL_YEAR:
        g_snprintf(column_name, 255, _("Year"));
        minimum_width = 40;
        break;
      case COL_TRACK:
        g_snprintf(column_name, 255, _("Track"));
        minimum_width = 20;
        break;
    }

    GtkTreeViewColumn *tag_column = gtk_tree_view_column_new_with_attributes
      (column_name, GTK_CELL_RENDERER(renderer), "text", i, NULL);

    gtk_tree_view_insert_column(GTK_TREE_VIEW(tree_view), tag_column, i);
    gtk_tree_view_column_set_alignment(tag_column, 0.5);
    gtk_tree_view_column_set_resizable(tag_column, TRUE);
    gtk_tree_view_column_set_reorderable(tag_column, TRUE);
    gtk_tree_view_column_set_min_width(tag_column, minimum_width);
  }
}

//!creates the tree view
static void create_tree_view(ui_state *ui)
{
  GtkTreeView *tree_view = GTK_TREE_VIEW(gtk_tree_view_new_with_model(create_model()));
  dnd_add_drag_data_received_to_widget(GTK_WIDGET(tree_view), DND_DATA_FILES, ui);
 
  ui->gui->tree_view = tree_view;

  g_signal_connect(tree_view, "row-activated", G_CALLBACK(preview_song), ui);

  GtkTreeSelection *selection = gtk_tree_view_get_selection(tree_view);
  g_signal_connect(selection, "changed", G_CALLBACK(row_selection_event), ui);
}

/*!creates the choose splitpoints frame

This frame contains the spinners, arrows, the tree, add, remove
buttons
*/
GtkWidget *create_splitpoints_frame(ui_state *ui)
{
  gui_state *gui = ui->gui;

  create_tree_view(ui);

  /* choose splitpoins vbox */
  GtkWidget *choose_splitpoints_vbox = wh_vbox_new();
  gtk_container_set_border_width(GTK_CONTAINER(choose_splitpoints_vbox), 0);

  /* spinner buttons hbox */
  GtkWidget *spinners_buttons_hbox = create_init_spinners_buttons(ui);
  gtk_box_pack_start(GTK_BOX(choose_splitpoints_vbox), spinners_buttons_hbox, FALSE, FALSE, 3);

  /* horizontal box for the tree */
  GtkWidget *tree_hbox = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(choose_splitpoints_vbox), tree_hbox, TRUE, TRUE, 0);

  /* scrolled window for the tree */
  GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_NONE);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
      GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
  gtk_box_pack_start(GTK_BOX(tree_hbox), scrolled_window, TRUE, TRUE, 0);

  GtkTreeSelection *selection = gtk_tree_view_get_selection(gui->tree_view);
  gtk_tree_selection_set_mode(selection, GTK_SELECTION_MULTIPLE);
  create_columns(ui);
  gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(gui->tree_view));

  g_signal_connect(gui->tree_view, "popup-menu", G_CALLBACK(show_popup), ui);
  g_signal_connect(gui->tree_view, "button-press-event", G_CALLBACK(select_and_show_popup), ui);

  /* special buttons like 'set silence from silence detection' */
  create_init_special_buttons(ui);

  return choose_splitpoints_vbox;
}

static void garray_to_array(GArray *spltpoints, glong *hundredth, ui_state *ui)
{
  gint i = 0;
  for(i = 0; i < ui->infos->splitnumber; i++ )
  {
    Split_point point = g_array_index(ui->splitpoints, Split_point, i);
    if (point.mins >= (INT_MAX-1)/6000)
    {
      hundredth[i] = LONG_MAX;
    }
    else
    {
      hundredth[i] = point.mins * 6000 +
        point.secs * 100 + point.hundr_secs;
    }
  }
}

//!puts the splitpoints into the state
void put_splitpoints_and_tags_in_mp3splt_state(splt_state *state, ui_state *ui)
{
  glong hundr[ui->infos->splitnumber];
  garray_to_array(ui->splitpoints, hundr, ui);

  GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->tree_view);

  gint i;
  for (i = 0; i < ui->infos->splitnumber; i++)
  {
    GtkTreePath *path = gtk_tree_path_new_from_indices(i ,-1);
    GtkTreeIter iter;
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_path_free(path);

    gchar *description = NULL;
    gtk_tree_model_get(GTK_TREE_MODEL(model), &iter, COL_DESCRIPTION, &description, -1);

    //get the 'checked' value from the current splitpoint
    Split_point point = g_array_index(ui->splitpoints, Split_point, i);
    gint splitpoint_type = SPLT_SPLITPOINT;

    if (point.checked == FALSE)
    {
      splitpoint_type = SPLT_SKIPPOINT;
    }

    splt_point *splitpoint = mp3splt_point_new(hundr[i], NULL);
    mp3splt_point_set_name(splitpoint, description);
    g_free(description);
    mp3splt_point_set_type(splitpoint, splitpoint_type);
    mp3splt_append_splitpoint(state, splitpoint);

    gint year = 0, track = 0;
    gchar *title = NULL, *artist = NULL, *album = NULL, *genre = NULL, *comment = NULL;

    gtk_tree_model_get(GTK_TREE_MODEL(model), &iter,
        COL_TITLE, &title,
        COL_ARTIST, &artist,
        COL_ALBUM, &album,
        COL_GENRE, &genre,
        COL_COMMENT, &comment,
        COL_YEAR, &year,
        COL_TRACK, &track,
        -1);

    splt_tags *tags = mp3splt_tags_new(NULL);

    if (year > 0)
    {
      gchar year_str[10] = { '\0' };
      g_snprintf(year_str, 10, "%d", year);
      mp3splt_tags_set(tags, SPLT_TAGS_YEAR, year_str, 0);
    }

    if (track <= 0) { track = -2; }
    gchar track_str[10] = { '\0' };
    g_snprintf(track_str, 10, "%d", track);
    mp3splt_tags_set(tags, SPLT_TAGS_TRACK, track_str, 0);

    mp3splt_tags_set(tags,
        SPLT_TAGS_TITLE, title,
        SPLT_TAGS_ARTIST, artist,
        SPLT_TAGS_ALBUM, album,
        SPLT_TAGS_GENRE, genre,
        SPLT_TAGS_COMMENT, comment,
        0);
    mp3splt_append_tags(state, tags);

    free(title); free(artist); free(album); free(genre); free(comment);
  }
}


