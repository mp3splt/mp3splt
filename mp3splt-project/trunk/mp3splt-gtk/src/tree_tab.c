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
 * The splitpoints tab
 *
 * this file is used for the Splitpoints tab
 * (which in turn contains the splitpoints table)
 **********************************************************/

#include "tree_tab.h"

//if we have a skippoint or a splitpoint
gint splitpoint_checked = TRUE;

//special buttons like 'set splitpoints from silence detection
GtkWidget *scan_silence_button = NULL;
GtkWidget *scan_trim_silence_button = NULL;

//handle box for detaching window
GtkWidget *handle_box;

/*!\defgroup{} splitpointview Variables for the splitpoint view
@{
*/
//!The minutes spinner
GtkWidget *spinner_minutes;
//!The seconds spinner
GtkWidget *spinner_seconds;
//!The hundreths of seconds spinner
GtkWidget *spinner_hundr_secs;

/*! A bool that helps us catch the case that we add splitpoints during preview

if we add a new splitpoint at the left and we are currently
previewing, we should increment quick_preview start and end
*/
gboolean new_left_splitpoint_added = FALSE;

/*! \defgroup silencedetectiongroup silence detection parameters widgets
@{
*/
//!number of tracks parameter
GtkWidget *spinner_silence_number_tracks = NULL;
GtkWidget *spinner_silence_minimum = NULL;
GtkWidget *spinner_silence_minimum_track = NULL;
//!offset parameter
GtkWidget *spinner_silence_offset = NULL;
//!threshold parameter
GtkWidget *spinner_silence_threshold;
//!remove silence check button (silence mode parameter
GtkWidget *silence_remove_silence = NULL;
// @}

/*!\defgroup silencesplitparameters silence split parameters
  @{
*/
gfloat silence_threshold_value = SPLT_DEFAULT_PARAM_THRESHOLD; 
gfloat silence_offset_value = SPLT_DEFAULT_PARAM_OFFSET;
gint silence_number_of_tracks = SPLT_DEFAULT_PARAM_TRACKS;
gfloat silence_minimum_length = SPLT_DEFAULT_PARAM_MINIMUM_LENGTH;
gfloat silence_minimum_track_length = SPLT_DEFAULT_PARAM_MINIMUM_TRACK_LENGTH;
//@}
gboolean silence_remove_silence_between_tracks = FALSE;

/*!\defgroup SplitOptionGroup options for splitting
\{
*/
extern gchar *filename_path_of_split;
extern gchar *filename_path_of_split;
//@}

extern ui_state *ui;


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
void update_add_button(ui_state *ui)
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
void update_minutes_from_spinner(GtkWidget *widget, gpointer data)
{
  ui->status->spin_mins = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinner_minutes));
  update_add_button(ui);
}

//!updates the seconds from the spinner
void update_seconds_from_spinner(GtkWidget *widget, gpointer data)
{
  ui->status->spin_secs = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinner_seconds));
  update_add_button(ui);
}

//!updates the hundredth of seconds for the spinner
void update_hundr_secs_from_spinner(GtkWidget *widget, gpointer data)
{
  ui->status->spin_hundr_secs = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinner_hundr_secs));
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
      GDK_TYPE_PIXBUF,
      GDK_TYPE_PIXBUF);

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
void row_selection_event()
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

  remove_status_message(ui->gui);
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
      ((index == ui->status->quick_preview_end_splitpoint) &&
       (ui->status->quick_preview_end_splitpoint == (ui->infos->splitnumber-1)) && stop_preview))
  {
    cancel_quick_preview_all(ui->status);
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

  recompute_length_column(ui);
  remove_status_message(ui->gui);
  update_add_button(ui);
  check_update_down_progress_bar(ui);
  refresh_drawing_area(ui->gui);
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
    ui->status->first_splitpoint_selected = get_first_splitpoint_selected(ui->gui);

    gchar *description = get_splitpoint_name(index, ui);
    g_snprintf(ui->status->current_description, 255, "%s", description);
    g_free(description);

    remove_splitpoint(index, FALSE, ui);
    add_splitpoint(new_point, index, ui);
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
      put_status_message(_(" error: you already have the splitpoint in table"), ui->gui);
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
  GtkTreeIter iter;

  gint i;

  Split_point old_point;
  Split_point new_point;
  
  GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->tree_view);
  
  //get the column number
  gint col = GPOINTER_TO_INT(g_object_get_data (G_OBJECT(cell), "col"));

  //get iter number
  gtk_tree_model_get_iter (model, &iter, path);
  //get the indice
  i = gtk_tree_path_get_indices (path)[0];
  old_point = g_array_index(ui->splitpoints, Split_point, i);
  new_point.checked = old_point.checked;
  
  //check which column
  switch (col)
    {
    case COL_DESCRIPTION:
      update_current_description(new_text, i, ui);
      
      //put the new content in the list
      gtk_list_store_set (GTK_LIST_STORE (model), 
                          &iter,
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
    default:
      break;
    }
  //free memory
  gtk_tree_path_free (path);
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

  add_splitpoint(my_split_point, -1, ui);
}

/*! adds a splitpoint

\param my_split_point The data for our new split point
\param old_index used when we update a splitpoint to see where we had
the play_preview point 
*/
void add_splitpoint(Split_point my_split_point, gint old_index, ui_state *ui)
{
  if (check_if_splitpoint_does_not_exists(my_split_point.mins,
        my_split_point.secs, my_split_point.hundr_secs,-1, ui))
  {
    gint k = 0;

    gchar *temp = g_strdup(ui->status->current_description);
    update_current_description(temp, -1, ui);
    if (temp)
    {
      free(temp);
      temp = NULL;
    }

    GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->tree_view);

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
          ui->status->quick_preview_end_splitpoint = ui->status->preview_start_splitpoint + 1;
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
            ui->status->quick_preview_end_splitpoint = ui->status->preview_start_splitpoint + 1;
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
              ui->status->quick_preview_end_splitpoint = ui->status->preview_start_splitpoint + 1;
            }
            else
            {
              //if we move the start splitpoint at the left
              if ((k+1) < ui->status->preview_start_splitpoint)
              {
                ui->status->preview_start_splitpoint -= ui->status->preview_start_splitpoint - (k + 1);
                ui->status->quick_preview_end_splitpoint = ui->status->preview_start_splitpoint + 1;
              }
            }
          }
        }
      }

      if (ui->status->preview_start_splitpoint == (ui->infos->splitnumber-1))
      {
        cancel_quick_preview_all(ui->status);
      }
    }
    else
    {
      //if we add a splitpoint at the left of the quick
      //preview start, add 1
      if ((k+1) <= ui->status->preview_start_splitpoint)
      {
        ui->status->preview_start_splitpoint++;
        ui->status->quick_preview_end_splitpoint = ui->status->preview_start_splitpoint + 1;
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
    put_status_message(_(" error: you already have the splitpoint in table"), ui->gui);
  }

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
}

//!adds a row to the table
void add_row(gboolean checked)
{
  gui_status *status = ui->status;

  Split_point my_split_point;
  my_split_point.mins = status->spin_mins;
  my_split_point.secs = status->spin_secs;
  my_split_point.hundr_secs = status->spin_hundr_secs;
  my_split_point.checked = checked;
  
  add_splitpoint(my_split_point, -1, ui);
}

static void add_row_clicked(GtkWidget *button, ui_state *ui)
{
  add_row(TRUE);
}

//!set splitpints from silence detection
gpointer detect_silence_and_set_splitpoints(gpointer data)
{
  gint should_trim = GPOINTER_TO_INT(data);

  gint err = SPLT_OK;

  enter_threads();

  gtk_widget_set_sensitive(GTK_WIDGET(scan_silence_button), FALSE);
  gtk_widget_set_sensitive(GTK_WIDGET(scan_trim_silence_button), FALSE);
  gtk_widget_set_sensitive(ui->gui->cancel_button, TRUE);
  ui->status->filename_to_split = get_input_filename(ui->gui);
  gchar *format = strdup(gtk_entry_get_text(GTK_ENTRY(ui->gui->output_entry)));

  exit_threads();

  mp3splt_set_filename_to_split(ui->mp3splt_state, ui->status->filename_to_split);
  mp3splt_erase_all_splitpoints(ui->mp3splt_state, &err);

  if (get_checked_output_radio_box() == 0)
  {
    mp3splt_set_oformat(ui->mp3splt_state, format, &err);
  }
  if (format)
  {
    free(format);
    format = NULL;
  }

  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_PRETEND_TO_SPLIT, SPLT_TRUE);
  mp3splt_set_split_filename_function(ui->mp3splt_state, NULL);
  int old_split_mode = mp3splt_get_int_option(ui->mp3splt_state, SPLT_OPT_SPLIT_MODE, &err);
  int old_tags_option = mp3splt_get_int_option(ui->mp3splt_state, SPLT_OPT_TAGS, &err);
  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_TAGS, SPLT_TAGS_ORIGINAL_FILE);
  if (err >= 0)
  {
    ui->status->splitting = TRUE;
    if (should_trim)
    {
      mp3splt_set_trim_silence_points(ui->mp3splt_state, &err);
    }
    else
    {
      mp3splt_set_silence_points(ui->mp3splt_state, &err);
    }
    ui->status->splitting = FALSE;
  }

  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_TAGS, old_tags_option);
  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_SPLIT_MODE, old_split_mode);
  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_PRETEND_TO_SPLIT, SPLT_FALSE);
  mp3splt_set_split_filename_function(ui->mp3splt_state, lmanager_put_split_filename);
 
  enter_threads();

  if (err >= 0)
  {
    update_splitpoints_from_mp3splt_state();
  }

  print_status_bar_confirmation(err);

  gtk_widget_set_sensitive(ui->gui->cancel_button, FALSE);
  gtk_widget_set_sensitive(GTK_WIDGET(scan_silence_button), TRUE);
  gtk_widget_set_sensitive(GTK_WIDGET(scan_trim_silence_button), TRUE);

  exit_threads();

  return NULL;
}

//!start thread with 'set splitpints from silence detection'
void detect_silence_and_add_splitpoints_start_thread()
{
  create_thread(detect_silence_and_set_splitpoints, GINT_TO_POINTER(SPLT_FALSE), TRUE, NULL);
}

void detect_silence_and_add_trim_splitpoints_start_thread()
{
  create_thread(detect_silence_and_set_splitpoints, GINT_TO_POINTER(SPLT_TRUE), TRUE, NULL);
}

//!update silence parameters when 'widget' changes
void update_silence_parameters(GtkWidget *widget, gpointer data)
{
  silence_threshold_value = 
    gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinner_silence_threshold));
  if (spinner_silence_offset != NULL)
  {
    silence_offset_value =
      gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinner_silence_offset));
  }
  if (spinner_silence_number_tracks != NULL)
  {
    silence_number_of_tracks =
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinner_silence_number_tracks));
  }
  if (spinner_silence_minimum != NULL)
  {
    silence_minimum_length = 
      gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinner_silence_minimum));
  }
  if (spinner_silence_minimum_track != NULL)
  {
    silence_minimum_track_length = 
      gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinner_silence_minimum_track));
  }
  if (silence_remove_silence != NULL)
  {
    silence_remove_silence_between_tracks = 
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(silence_remove_silence));
  }
}

//!action when checking the 'remove silence' button
void silence_remove_silence_checked(GtkToggleButton *button, gpointer data)
{
  update_silence_parameters(GTK_WIDGET(button), data);
}

void create_trim_silence_window(GtkWidget *button, gpointer *data)
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
  //add silence parameters
  GtkWidget *horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(general_inside_vbox), horiz_fake, FALSE, FALSE, 10);
  
  //vertical parameter box
  GtkWidget *param_vbox = wh_vbox_new();
  gtk_box_pack_start(GTK_BOX(horiz_fake), param_vbox, FALSE, FALSE, 25);
  
  //horizontal box fake for threshold level
  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(param_vbox), 
      horiz_fake, FALSE, FALSE, 0);
  
  //threshold level
  GtkWidget *label = gtk_label_new(_("Threshold level (dB):"));
  gtk_box_pack_start(GTK_BOX(horiz_fake), label, FALSE, FALSE, 0);
  
  //adjustement for the threshold spinner
  GtkAdjustment *adj = (GtkAdjustment *)
    gtk_adjustment_new(0.0, -96.0, 0.0, 0.5, 10.0, 0.0);
  //the threshold spinner
  spinner_silence_threshold = gtk_spin_button_new(adj, 0.5, 2);
  //set not editable
  gtk_box_pack_start(GTK_BOX(horiz_fake), 
      spinner_silence_threshold, FALSE, FALSE, 6);
  
  //we set the default parameters for the silence split
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_silence_threshold),
      silence_threshold_value);

  //add actions when changing the values
  g_signal_connect(G_OBJECT(spinner_silence_threshold), "value_changed",
      G_CALLBACK(update_silence_parameters), NULL);

  gtk_widget_show_all(general_inside_vbox);
  gtk_container_add(GTK_CONTAINER(
        gtk_dialog_get_content_area(GTK_DIALOG(silence_detection_window))),
      general_inside_vbox);

  //result of the dialog window
  gint result = gtk_dialog_run(GTK_DIALOG(silence_detection_window));

  //we set the silence parameters
  mp3splt_set_float_option(ui->mp3splt_state, SPLT_OPT_PARAM_THRESHOLD,
      silence_threshold_value);

  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_DEBUG_MODE, ui->infos->debug_is_active);

  gtk_widget_destroy(silence_detection_window);

  if (result == GTK_RESPONSE_YES)
  {
    detect_silence_and_add_trim_splitpoints_start_thread();
  }
}

//!event for clicking the 'detect silence and add splitpoints' button
void create_detect_silence_and_add_splitpoints_window(GtkWidget *button, gpointer *data)
{
  GtkWidget *silence_detection_window =
    gtk_dialog_new_with_buttons(_("Set splitpoints from silence detection"),
        GTK_WINDOW(ui->gui->window),
        GTK_DIALOG_DESTROY_WITH_PARENT | GTK_DIALOG_MODAL,
        GTK_STOCK_OK,
        GTK_RESPONSE_YES,
        GTK_STOCK_CANCEL,
        GTK_RESPONSE_CANCEL,
        NULL);

  GtkWidget *general_inside_vbox = wh_vbox_new();
  //add silence parameters
  GtkWidget *horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(general_inside_vbox), 
                      horiz_fake, FALSE, FALSE, 10);
  
  //vertical parameter box
  GtkWidget *param_vbox = wh_vbox_new();
  gtk_box_pack_start(GTK_BOX(horiz_fake), param_vbox, FALSE, FALSE, 25);
  
  //horizontal box fake for threshold level
  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(param_vbox), 
      horiz_fake, FALSE, FALSE, 0);
  
  //threshold level
  GtkWidget *label = gtk_label_new(_("Threshold level (dB):"));
  gtk_box_pack_start(GTK_BOX(horiz_fake), label, FALSE, FALSE, 0);
  
  //adjustement for the threshold spinner
  GtkAdjustment *adj = (GtkAdjustment *)
    gtk_adjustment_new(0.0, -96.0, 0.0, 0.5, 10.0, 0.0);
  //the threshold spinner
  spinner_silence_threshold = gtk_spin_button_new(adj, 0.5, 2);
  //set not editable
  gtk_box_pack_start(GTK_BOX(horiz_fake), 
      spinner_silence_threshold, FALSE, FALSE, 6);
  
  //horizontal box fake for the offset level
  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(param_vbox), horiz_fake, FALSE, FALSE, 0);
  
  //offset level
  label = gtk_label_new(_("Cutpoint offset (0 is the begin of silence,"
        "and 1 the end):"));
  gtk_box_pack_start(GTK_BOX(horiz_fake), label, FALSE, FALSE, 0);
  
  //adjustement for the offset spinner
  adj = (GtkAdjustment *) gtk_adjustment_new(0.0, -2, 2, 0.05, 10.0, 0.0);
  //the offset spinner
  spinner_silence_offset = gtk_spin_button_new (adj, 0.05, 2);
  gtk_box_pack_start(GTK_BOX(horiz_fake), spinner_silence_offset,
      FALSE, FALSE, 6);
  
  //horizontal box fake for the number of tracks
  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(param_vbox), 
      horiz_fake, FALSE, FALSE, 0);
  
  //number of tracks level
  label = gtk_label_new(_("Number of tracks (0 means all tracks):"));
  gtk_box_pack_start(GTK_BOX(horiz_fake), label, FALSE, FALSE, 0);
  
  //number of tracks
  adj = (GtkAdjustment *)gtk_adjustment_new(0.0, 0, 2000, 1, 10.0, 0.0);
  //the number of tracks spinner
  spinner_silence_number_tracks = gtk_spin_button_new (adj, 1, 0);
  gtk_box_pack_start(GTK_BOX(horiz_fake), spinner_silence_number_tracks,
      FALSE, FALSE, 6);
 
  //horizontal box fake for minimum length parameter
  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(param_vbox), horiz_fake, FALSE, FALSE, 0);
  
  //the minimum length parameter
  label = gtk_label_new(_("Minimum silence length (seconds):"));
  gtk_box_pack_start(GTK_BOX(horiz_fake), label, FALSE, FALSE, 0);
  
  //minimum silence length (seconds)
  adj = (GtkAdjustment *)gtk_adjustment_new(0.0, 0, 2000, 0.5, 10.0, 0.0);
  //the minimum silence length in seconds
  spinner_silence_minimum = gtk_spin_button_new(adj, 1, 2);
  gtk_box_pack_start(GTK_BOX(horiz_fake), spinner_silence_minimum,
      FALSE, FALSE, 6);
  
  //the minimum track length parameter
  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(param_vbox), horiz_fake, FALSE, FALSE, 0);
 
  label = gtk_label_new(_("Minimum track length (seconds):"));
  gtk_box_pack_start(GTK_BOX(horiz_fake), label, FALSE, FALSE, 0);
  
  adj = (GtkAdjustment *)gtk_adjustment_new(0.0, 0, 2000, 0.5, 10.0, 0.0);
  spinner_silence_minimum_track = gtk_spin_button_new(adj, 1, 2);
  gtk_box_pack_start(GTK_BOX(horiz_fake), spinner_silence_minimum_track,
      FALSE, FALSE, 6);
 
  //remove silence (rm): allows you to remove the silence between
  //tracks
  silence_remove_silence =
    gtk_check_button_new_with_mnemonic(_("_Remove silence between tracks"));
  gtk_box_pack_start(GTK_BOX(param_vbox), silence_remove_silence,
      FALSE, FALSE, 0);

  //we set the default parameters for the silence split
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_silence_threshold),
      silence_threshold_value);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_silence_offset),
                            silence_offset_value);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_silence_number_tracks),
                            silence_number_of_tracks);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_silence_minimum),
                            silence_minimum_length);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_silence_minimum_track),
                            silence_minimum_track_length);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(silence_remove_silence),
      silence_remove_silence_between_tracks);

  //add actions when changing the values
  g_signal_connect(G_OBJECT(spinner_silence_threshold), "value_changed",
      G_CALLBACK(update_silence_parameters), NULL);
  g_signal_connect(G_OBJECT(spinner_silence_offset), "value_changed",
      G_CALLBACK(update_silence_parameters), NULL);
  g_signal_connect(G_OBJECT(spinner_silence_number_tracks), "value_changed",
      G_CALLBACK(update_silence_parameters), NULL);
  g_signal_connect(G_OBJECT(spinner_silence_minimum), "value_changed",
      G_CALLBACK(update_silence_parameters), NULL);
  g_signal_connect(G_OBJECT(spinner_silence_minimum_track), "value_changed",
      G_CALLBACK(update_silence_parameters), NULL);
  g_signal_connect(G_OBJECT(silence_remove_silence), "toggled",
      G_CALLBACK(silence_remove_silence_checked), NULL);

  gtk_widget_show_all(general_inside_vbox);
  gtk_container_add(GTK_CONTAINER(
        gtk_dialog_get_content_area(GTK_DIALOG(silence_detection_window))),
      general_inside_vbox);

  //result of the dialog window
  gint result = gtk_dialog_run(GTK_DIALOG(silence_detection_window));

  //we set the silence parameters
  mp3splt_set_float_option(ui->mp3splt_state, SPLT_OPT_PARAM_THRESHOLD,
      silence_threshold_value);
  mp3splt_set_float_option(ui->mp3splt_state, SPLT_OPT_PARAM_OFFSET,
      silence_offset_value);
  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_PARAM_NUMBER_TRACKS,
      silence_number_of_tracks);
  mp3splt_set_float_option(ui->mp3splt_state, SPLT_OPT_PARAM_MIN_LENGTH,
      silence_minimum_length);
  mp3splt_set_float_option(ui->mp3splt_state, SPLT_OPT_PARAM_MIN_TRACK_LENGTH,
      silence_minimum_track_length);
  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_PARAM_REMOVE_SILENCE,
      silence_remove_silence_between_tracks);

  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_DEBUG_MODE, ui->infos->debug_is_active);

  gtk_widget_destroy(silence_detection_window);

  if (result == GTK_RESPONSE_YES)
  {
    detect_silence_and_add_splitpoints_start_thread();
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
    gtk_list_store_remove (GTK_LIST_STORE (model), &iter);
    g_array_remove_index(ui->splitpoints, (ui->infos->splitnumber-1));
    ui->infos->splitnumber--;
  }
  
  gtk_widget_set_sensitive(ui->gui->remove_all_button, FALSE);
  gtk_widget_set_sensitive(ui->gui->remove_row_button, FALSE);
  
  remove_status_message(ui->gui);
  cancel_quick_preview_all(ui->status);
  update_add_button(ui);
  refresh_drawing_area(ui->gui);
  check_update_down_progress_bar(ui);
}

//!creates and and initialise a spinner
GtkWidget *create_init_spinner(GtkWidget *bottomhbox1, 
                               gint min, gint max, 
                               gchar *label_text, gint type)
{
  GtkWidget *spinner_box = wh_vbox_new(); 
  GtkWidget *label = gtk_label_new(label_text);
  gtk_box_pack_start(GTK_BOX(spinner_box), label, TRUE, FALSE, 0);

  GtkAdjustment *adj =
    (GtkAdjustment *) gtk_adjustment_new(0.0, min, max, 1.0, 10.0, 0.0);
  GtkWidget *spinner = gtk_spin_button_new(adj, 0, 0);
  gtk_spin_button_set_wrap(GTK_SPIN_BUTTON(spinner), TRUE);

  if (type == 0)
  {
    g_signal_connect(G_OBJECT(spinner), "value_changed",
        G_CALLBACK(update_minutes_from_spinner), NULL);
  }
  else if (type == 1)
  {
    g_signal_connect(G_OBJECT(spinner), "value_changed",
        G_CALLBACK(update_seconds_from_spinner), NULL);
  }
  else
  {
    g_signal_connect(G_OBJECT(spinner), "value_changed",
        G_CALLBACK(update_hundr_secs_from_spinner), NULL);
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

  /* minutes and seconds spinners */
  spinner_minutes = create_init_spinner(hbox, 
      0, INT_MAX/6000,
      _("Minutes:"),
      //0 means spinner minutes
      0);
  spinner_seconds = create_init_spinner(hbox, 
      0, 59,
      _("Seconds:"), 
      //1 means spinner seconds
      1);
  //hundredth spinner
  spinner_hundr_secs = create_init_spinner(hbox, 
      0, 99,
      _("Hundredths:"), 
      //2 means spinner hundredth
      2);

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
  gtk_widget_set_tooltip_text(remove_row_button, _("Remove rows"));

  /* remove all rows button */
  GtkWidget *remove_all_button = wh_create_cool_button(GTK_STOCK_DELETE, _("R_emove all"), FALSE);
  ui->gui->remove_all_button = remove_all_button;

  gtk_button_set_relief(GTK_BUTTON(remove_all_button), GTK_RELIEF_NONE);
  gtk_widget_set_sensitive(remove_all_button, FALSE);
  g_signal_connect(G_OBJECT(remove_all_button), "clicked", G_CALLBACK(remove_all_rows), ui);
  gtk_box_pack_start(GTK_BOX(hbox), remove_all_button, FALSE, FALSE, 5);
  gtk_widget_set_tooltip_text(remove_all_button, _("Remove all rows"));

  return hbox;
}

//!special buttons like 'set silence from silence detection'
static GtkWidget *create_init_special_buttons()
{
  GtkWidget *hbox = wh_hbox_new();
  gtk_container_set_border_width(GTK_CONTAINER(hbox), 0);

  /* set splitpoints from trim silence detection */
  scan_trim_silence_button =
    wh_create_cool_button(GTK_STOCK_CUT, _("_Trim splitpoints"), FALSE);
  gtk_widget_set_sensitive(GTK_WIDGET(scan_trim_silence_button), TRUE);
  g_signal_connect(G_OBJECT(scan_trim_silence_button), "clicked",
      G_CALLBACK(create_trim_silence_window), NULL);
  gtk_box_pack_start(GTK_BOX(hbox), scan_trim_silence_button, FALSE, FALSE, 5);
  gtk_widget_set_tooltip_text(scan_trim_silence_button,
      _("Set trim splitpoints using silence detection"));

  /* set splitpoints from silence detection */
  scan_silence_button =
    wh_create_cool_button(GTK_STOCK_ADD, _("_Silence detection"), FALSE);
  gtk_widget_set_sensitive(GTK_WIDGET(scan_silence_button), TRUE);
  g_signal_connect(G_OBJECT(scan_silence_button), "clicked",
      G_CALLBACK(create_detect_silence_and_add_splitpoints_window), NULL);
  gtk_box_pack_start(GTK_BOX(hbox), scan_silence_button, FALSE, FALSE, 5);
  gtk_widget_set_tooltip_text(scan_silence_button,
      _("Set splitpoints from silence detection"));

  return hbox;
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
        COL_DESCRIPTION,&description,
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

static gpointer split_preview(ui_state *ui)
{
  if (ui->status->preview_row + 1 == ui->infos->splitnumber)
  {
    enter_threads();
    put_status_message(_(" cannot split preview last splitpoint"), ui->gui);
    exit_threads();
    return NULL;
  }

  int err = 0;
  mp3splt_erase_all_splitpoints(ui->mp3splt_state, &err);
  mp3splt_erase_all_tags(ui->mp3splt_state, &err);

  mp3splt_append_splitpoint(ui->mp3splt_state, ui->status->preview_start_position,
      "preview", SPLT_SPLITPOINT);
  mp3splt_append_splitpoint(ui->mp3splt_state,
      get_splitpoint_time(ui->status->quick_preview_end_splitpoint, ui),
      NULL, SPLT_SKIPPOINT);

  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_OUTPUT_FILENAMES, SPLT_OUTPUT_CUSTOM);
  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_SPLIT_MODE, SPLT_OPTION_NORMAL_MODE);

  enter_threads();

  put_options_from_preferences(ui);

  //we cut the preferences filename path
  //to find the ~/.mp3splt directory
  gchar *fname_path = get_preferences_filename();
  fname_path[strlen(fname_path)-18] = '\0';

  remove_all_split_rows();  
  ui->status->filename_to_split = get_input_filename(ui->gui);

  exit_threads();

  mp3splt_set_path_of_split(ui->mp3splt_state,fname_path);
  mp3splt_set_filename_to_split(ui->mp3splt_state, ui->status->filename_to_split);
  gint confirmation = mp3splt_split(ui->mp3splt_state);

  enter_threads();

  print_status_bar_confirmation(confirmation);

  gchar *split_file = get_filename_from_split_files(1);
  if (split_file != NULL)
  {
    if (confirmation > 0)
    {
      connect_button_event(ui->gui->connect_button, ui);

      change_current_filename(split_file, ui);
      g_free(split_file);
      split_file = NULL;

      //starts playing, 0 means start playing
      connect_to_player_with_song(0, ui);
    }
  }

  if (confirmation > 0)
  {
    gtk_progress_bar_set_fraction(ui->gui->percent_progress_bar, 1.0);
    gtk_progress_bar_set_text(ui->gui->percent_progress_bar, _(" finished"));
  }

  if (fname_path)
  {
    g_free(fname_path);
    fname_path = NULL;
  }

  exit_threads();

  return NULL;
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
    put_status_message(_(" cannot preview, not connected to player"), ui->gui);
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
    ui->status->preview_start_position = get_splitpoint_time(preview_row, ui);
    ui->status->quick_preview_end_splitpoint = preview_row + 1;
    create_thread((GThreadFunc)split_preview, ui, TRUE, NULL);
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

//!creates columns for the tree
void create_columns(GtkTreeView *tree_view)
{
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
    (_("Keep"), GTK_CELL_RENDERER(renderer_toggle),
     "active", COL_CHECK, NULL);

  /* description */
  renderer = GTK_CELL_RENDERER_TEXT(gtk_cell_renderer_text_new());
  g_signal_connect(renderer, "edited", G_CALLBACK(cell_edited_event), ui);
  g_object_set(renderer, "editable", TRUE, NULL);
  g_object_set_data(G_OBJECT(renderer), "col", GINT_TO_POINTER(COL_DESCRIPTION));
  column_description = gtk_tree_view_column_new_with_attributes
    (_("Filename"), GTK_CELL_RENDERER(renderer),
     "text", COL_DESCRIPTION, NULL);
  
  /* seconds */
  renderer = GTK_CELL_RENDERER_TEXT(gtk_cell_renderer_text_new());
  g_signal_connect(renderer, "edited", G_CALLBACK(cell_edited_event), ui);
  g_object_set(renderer, "editable", TRUE, NULL);
  g_object_set_data(G_OBJECT(renderer), "col", GINT_TO_POINTER(COL_SECONDS));
  column_seconds = gtk_tree_view_column_new_with_attributes
    (_("Secs"), GTK_CELL_RENDERER(renderer),
     "text", COL_SECONDS, NULL);
  
  /* minutes */
  renderer = GTK_CELL_RENDERER_TEXT(gtk_cell_renderer_text_new());
  g_signal_connect(renderer, "edited", G_CALLBACK(cell_edited_event), ui);
  g_object_set(renderer, "editable", TRUE, NULL);
  g_object_set_data(G_OBJECT(renderer), "col", GINT_TO_POINTER(COL_MINUTES));
  column_minutes = gtk_tree_view_column_new_with_attributes 
    (_("Mins"), GTK_CELL_RENDERER(renderer),
     "text", COL_MINUTES, NULL);

  /* hundr secs */
  renderer = GTK_CELL_RENDERER_TEXT(gtk_cell_renderer_text_new());
  g_signal_connect(renderer, "edited", G_CALLBACK(cell_edited_event), ui);
  g_object_set(renderer, "editable", TRUE, NULL);
  g_object_set_data(G_OBJECT(renderer), "col", GINT_TO_POINTER(COL_HUNDR_SECS));
  column_hundr_secs = gtk_tree_view_column_new_with_attributes 
    (_("Hundr"), GTK_CELL_RENDERER(renderer),
     "text", COL_HUNDR_SECS, NULL);
  
  /* Length column */
  //renderer creation
  renderer = GTK_CELL_RENDERER_TEXT(gtk_cell_renderer_text_new());
  g_object_set_data(G_OBJECT(renderer), "col", GINT_TO_POINTER(COL_NUMBER));
  //middle alignment
  g_object_set (G_OBJECT (renderer), "xalign", 1.0, NULL);
  column_number = gtk_tree_view_column_new_with_attributes 
    (_("Length"), GTK_CELL_RENDERER(renderer),
     "text", COL_NUMBER, NULL);
  
  /* column preview */
  //renderer creation
  renderer_pix = GTK_CELL_RENDERER_PIXBUF(gtk_cell_renderer_pixbuf_new());
  //set the icon
  g_object_set(renderer_pix,"stock-id",GTK_STOCK_MEDIA_PLAY,
               "stock-size",GTK_ICON_SIZE_MENU,NULL);
  //create the column
  column_preview = gtk_tree_view_column_new_with_attributes 
    (_("LiveP"), GTK_CELL_RENDERER(renderer_pix), 
     "pixbuf",COL_PREVIEW, NULL);
  g_object_set_data(G_OBJECT(column_preview), "col", GINT_TO_POINTER(COL_PREVIEW));
  
  /* split preview */
  renderer_pix = GTK_CELL_RENDERER_PIXBUF(gtk_cell_renderer_pixbuf_new());
  //set the icon
  g_object_set(renderer_pix,"stock-id",GTK_STOCK_MEDIA_PLAY,
               "stock-size",GTK_ICON_SIZE_MENU,NULL);
  //create the column
  column_split_preview = gtk_tree_view_column_new_with_attributes 
    (_("SplitP"), GTK_CELL_RENDERER(renderer_pix), 
     "pixbuf",COL_SPLIT_PREVIEW, NULL);
  g_object_set_data(G_OBJECT(column_split_preview), "col", GINT_TO_POINTER(COL_SPLIT_PREVIEW));
  
  //appends columns to the list of columns of tree_view
  gtk_tree_view_insert_column (GTK_TREE_VIEW (tree_view),
      GTK_TREE_VIEW_COLUMN(column_check),COL_DESCRIPTION);
  gtk_tree_view_insert_column (GTK_TREE_VIEW (tree_view),
                               GTK_TREE_VIEW_COLUMN (column_description),COL_DESCRIPTION);
  gtk_tree_view_insert_column (GTK_TREE_VIEW (tree_view),
                               GTK_TREE_VIEW_COLUMN (column_minutes),COL_MINUTES);
  gtk_tree_view_insert_column (GTK_TREE_VIEW (tree_view),
                               GTK_TREE_VIEW_COLUMN (column_seconds),COL_SECONDS);
  gtk_tree_view_insert_column (GTK_TREE_VIEW (tree_view),
                               GTK_TREE_VIEW_COLUMN (column_hundr_secs),COL_HUNDR_SECS);
  gtk_tree_view_insert_column (GTK_TREE_VIEW (tree_view),
                               GTK_TREE_VIEW_COLUMN (column_number),COL_NUMBER);
  gtk_tree_view_insert_column (GTK_TREE_VIEW (tree_view),
                               GTK_TREE_VIEW_COLUMN (column_preview),COL_PREVIEW);
  gtk_tree_view_insert_column (GTK_TREE_VIEW (tree_view),
                               GTK_TREE_VIEW_COLUMN (column_split_preview),
                               COL_SPLIT_PREVIEW);
  //middle alignment of the column name
  gtk_tree_view_column_set_alignment(GTK_TREE_VIEW_COLUMN(column_check), 0.5);
  gtk_tree_view_column_set_alignment(GTK_TREE_VIEW_COLUMN(column_description), 0.5);
  gtk_tree_view_column_set_alignment(GTK_TREE_VIEW_COLUMN(column_minutes), 0.5);
  gtk_tree_view_column_set_alignment(GTK_TREE_VIEW_COLUMN(column_seconds), 0.5);
  gtk_tree_view_column_set_alignment(GTK_TREE_VIEW_COLUMN(column_hundr_secs), 0.5);
  gtk_tree_view_column_set_alignment(GTK_TREE_VIEW_COLUMN(column_number), 0.5);
  gtk_tree_view_column_set_alignment(GTK_TREE_VIEW_COLUMN(column_preview), 0.5);
  gtk_tree_view_column_set_alignment(GTK_TREE_VIEW_COLUMN(column_split_preview), 0.5);

  //set the auto resizing for columns
  gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN(column_check),
      GTK_TREE_VIEW_COLUMN_FIXED);
  gtk_tree_view_column_set_fixed_width(GTK_TREE_VIEW_COLUMN(column_check), 70);
  /*gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN(column_description),
                                   GTK_TREE_VIEW_COLUMN_AUTOSIZE);
  gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN(column_minutes),
                                   GTK_TREE_VIEW_COLUMN_AUTOSIZE);
  gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN(column_seconds),
                                   GTK_TREE_VIEW_COLUMN_AUTOSIZE);
  gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN(column_hundr_secs),
                                   GTK_TREE_VIEW_COLUMN_AUTOSIZE);
  gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN(column_number),
                                   GTK_TREE_VIEW_COLUMN_AUTOSIZE);
  gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN(column_preview),
                                   GTK_TREE_VIEW_COLUMN_AUTOSIZE);
  gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN(column_split_preview),
                                   GTK_TREE_VIEW_COLUMN_AUTOSIZE);*/
  
  //sets resize 
  gtk_tree_view_column_set_resizable(column_description, TRUE);
  //set column reorderable
  gtk_tree_view_column_set_reorderable(column_check, TRUE);
  gtk_tree_view_column_set_reorderable(column_description, TRUE);
  gtk_tree_view_column_set_reorderable(column_minutes, TRUE);
  gtk_tree_view_column_set_reorderable(column_seconds, TRUE);
  gtk_tree_view_column_set_reorderable(column_hundr_secs, TRUE);
  gtk_tree_view_column_set_reorderable(column_number, TRUE);
  gtk_tree_view_column_set_reorderable(column_preview, TRUE);
  gtk_tree_view_column_set_reorderable(column_split_preview, TRUE);
  //set column expand
  gtk_tree_view_column_set_expand (column_description, TRUE);
}

//!Issued when closing the new window after detaching
void close_popup_window_event( GtkWidget *window,
                               gpointer data )
{
  GtkWidget *window_child;

  window_child = gtk_bin_get_child(GTK_BIN(window));

  gtk_widget_reparent(GTK_WIDGET(window_child), GTK_WIDGET(handle_box));

  gtk_widget_destroy(window);
}

//!Issued when we detach the handle
void handle_detached_event(GtkHandleBox *handlebox, GtkWidget *widget, gpointer data)
{
  GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_widget_reparent(GTK_WIDGET(widget), GTK_WIDGET(window));

  g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(close_popup_window_event), NULL);

  gtk_widget_show(GTK_WIDGET(window));
}

//!creates the tree view
static void create_tree_view(gui_state *gui)
{
  GtkTreeView *tree_view = GTK_TREE_VIEW(gtk_tree_view_new_with_model(create_model()));
  gui->tree_view = tree_view;

  g_signal_connect(tree_view, "row-activated", G_CALLBACK(preview_song), ui);

  GtkTreeSelection *selection = gtk_tree_view_get_selection(tree_view);
  g_signal_connect(selection, "changed", G_CALLBACK(row_selection_event), NULL);
}

/*!creates the choose splitpoints frame

This frame contains the spinners, arrows, the tree, add, remove
buttons
*/
GtkWidget *create_splitpoints_frame(ui_state *ui)
{
  create_tree_view(ui->gui);

  /* choose splitpoins vbox */
  GtkWidget *choose_splitpoints_vbox = wh_vbox_new();
  gtk_container_set_border_width(GTK_CONTAINER(choose_splitpoints_vbox), 0);

  /* handle box for detaching */
  handle_box = gtk_handle_box_new();
  gtk_container_add(GTK_CONTAINER (handle_box), GTK_WIDGET(choose_splitpoints_vbox));
  g_signal_connect(handle_box, "child-detached", G_CALLBACK(handle_detached_event), NULL);

  /* spinner buttons hbox */
  GtkWidget *spinners_buttons_hbox = create_init_spinners_buttons(ui);
  gtk_box_pack_start(GTK_BOX(choose_splitpoints_vbox), spinners_buttons_hbox, FALSE, FALSE, 3);

  /* horizontal box for the tree */
  GtkWidget *tree_hbox = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(choose_splitpoints_vbox), tree_hbox, TRUE, TRUE, 0);

  /* scrolled window for the tree */
  GtkWidget *scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_NONE);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
      GTK_POLICY_AUTOMATIC,
      GTK_POLICY_AUTOMATIC);
  gtk_box_pack_start(GTK_BOX(tree_hbox), scrolled_window, TRUE, TRUE, 0);

  GtkTreeSelection *selection = gtk_tree_view_get_selection(ui->gui->tree_view);
  gtk_tree_selection_set_mode(selection, GTK_SELECTION_MULTIPLE);
  create_columns(ui->gui->tree_view);
  gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(ui->gui->tree_view));

  /* special buttons like 'set silence from silence detection' */
  GtkWidget *special_buttons_hbox = create_init_special_buttons();
  gtk_box_pack_start(GTK_BOX(choose_splitpoints_vbox), special_buttons_hbox, FALSE, FALSE, 2);

  return handle_box;
}

static void garray_to_array(GArray *spltpoints, glong *hundredth)
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
void put_splitpoints_in_mp3splt_state(splt_state *state)
{
  glong hundr[ui->infos->splitnumber];
  garray_to_array(ui->splitpoints, hundr);
  gint i;

  GtkTreeModel *model = gtk_tree_view_get_model(ui->gui->tree_view);
  GtkTreeIter iter;
  GtkTreePath *path = NULL;
  gchar *description = NULL;

  for (i = 0; i < ui->infos->splitnumber; i++)
  {
    path = gtk_tree_path_new_from_indices(i ,-1);
    gtk_tree_model_get_iter(model, &iter, path);
    gtk_tree_model_get(GTK_TREE_MODEL(model), &iter,
        COL_DESCRIPTION,&description,
        -1);

    //get the 'checked' value from the current splitpoint
    Split_point point = g_array_index(ui->splitpoints, Split_point, i);
    gint splitpoint_type = SPLT_SPLITPOINT;

    if (point.checked == FALSE)
    {
      splitpoint_type = SPLT_SKIPPOINT;
    }

    exit_threads();
    mp3splt_append_splitpoint(state,hundr[i], description, splitpoint_type);
    enter_threads();

    gtk_tree_path_free(path);
  }
}

