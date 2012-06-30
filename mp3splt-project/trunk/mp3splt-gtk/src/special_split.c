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
 * The batch processing tab
 *
 * this file is for the special split tab, where automatic
 * split modes can be selected
 *..*******************************************************/

#include "special_split.h"

extern ui_state *ui;

//! Get the split mode
static gint get_selected_split_mode(GtkToggleButton *radio_b)
{
  GSList *radio_button_list = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio_b));

  gint i = 0;
  for(i = 0; i < NUMBER_OF_SPLIT_MODES;i++)
  {
    GtkToggleButton *test = (GtkToggleButton *) g_slist_nth_data(radio_button_list,i);
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(test)))
    {
      return GPOINTER_TO_INT(g_object_get_data(G_OBJECT(test), "split_type_id"));
    }
  }

  return SELECTED_SPLIT_NORMAL;
}

//! Set the split mode
void select_split_mode(int split_mode)
{
  GSList *split_mode_radio_button_list =
    gtk_radio_button_get_group(GTK_RADIO_BUTTON(ui->gui->split_mode_radio_button));

  gint i = 0;
  for(i = 0; i < NUMBER_OF_SPLIT_MODES;i++)
  {
    GtkToggleButton *test = (GtkToggleButton *) g_slist_nth_data(split_mode_radio_button_list, i);
    int id = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(test), "split_type_id"));
    if (split_mode == id)
    {
      gtk_toggle_button_set_active(test, TRUE);
      return;
    }
  }
}

static void deactivate_silence_parameters(gui_state *gui)
{
  gtk_widget_set_sensitive(gui->all_spinner_silence_number_tracks, FALSE);
  gtk_widget_set_sensitive(gui->all_spinner_silence_minimum, FALSE);
  gtk_widget_set_sensitive(gui->all_spinner_track_minimum, FALSE);
  gtk_widget_set_sensitive(gui->all_spinner_silence_offset, FALSE);
  gtk_widget_set_sensitive(gui->all_spinner_silence_threshold, FALSE);
  gtk_widget_set_sensitive(gui->all_silence_remove_silence, FALSE);
  gtk_widget_set_sensitive(gui->all_threshold_label, FALSE);
  gtk_widget_set_sensitive(gui->all_offset_label, FALSE);
  gtk_widget_set_sensitive(gui->all_number_of_tracks_label, FALSE);
  gtk_widget_set_sensitive(gui->all_min_silence_label, FALSE);
  gtk_widget_set_sensitive(gui->all_min_track_label, FALSE);
}

static void activate_silence_parameters(gui_state *gui)
{
  gtk_widget_set_sensitive(gui->all_spinner_silence_number_tracks, TRUE);
  gtk_widget_set_sensitive(gui->all_spinner_silence_minimum, TRUE);
  gtk_widget_set_sensitive(gui->all_spinner_track_minimum, TRUE);
  gtk_widget_set_sensitive(gui->all_spinner_silence_offset, TRUE);
  gtk_widget_set_sensitive(gui->all_spinner_silence_threshold, TRUE);
  gtk_widget_set_sensitive(gui->all_silence_remove_silence, TRUE);
  gtk_widget_set_sensitive(gui->all_threshold_label, TRUE);
  gtk_widget_set_sensitive(gui->all_offset_label, TRUE);
  gtk_widget_set_sensitive(gui->all_number_of_tracks_label, TRUE);
  gtk_widget_set_sensitive(gui->all_min_silence_label, TRUE);
  gtk_widget_set_sensitive(gui->all_min_track_label, TRUE);
}

static void deactivate_trim_parameters(gui_state *gui)
{
  gtk_widget_set_sensitive(gui->all_spinner_trim_silence_threshold, FALSE);
  gtk_widget_set_sensitive(gui->all_trim_threshold_label, FALSE);
}

static void activate_trim_parameters(gui_state *gui)
{
  gtk_widget_set_sensitive(gui->all_spinner_trim_silence_threshold, TRUE);
  gtk_widget_set_sensitive(gui->all_trim_threshold_label, TRUE);
}

//! Issued when the split mode selection changed
static void split_mode_changed(GtkToggleButton *radio_b, ui_state *ui)
{
  gint selected_split_mode = get_selected_split_mode(radio_b);
  ui->status->selected_split_mode = selected_split_mode;

  int enable_time = (selected_split_mode == SELECTED_SPLIT_TIME);
  gtk_widget_set_sensitive(ui->gui->spinner_time, enable_time);
  gtk_widget_set_sensitive(ui->gui->time_label, enable_time);

  int enable_split_equal_time = (selected_split_mode == SELECTED_SPLIT_EQUAL_TIME_TRACKS);
  gtk_widget_set_sensitive(ui->gui->spinner_equal_tracks, enable_split_equal_time);
  gtk_widget_set_sensitive(ui->gui->equal_tracks_label, enable_split_equal_time);

  if (selected_split_mode == SELECTED_SPLIT_SILENCE)
  {
    activate_silence_parameters(ui->gui);
  }
  else
  {
    deactivate_silence_parameters(ui->gui);
  }

  if (selected_split_mode == SELECTED_SPLIT_TRIM_SILENCE)
  {
    activate_trim_parameters(ui->gui);
  }
  else
  {
    deactivate_trim_parameters(ui->gui);
  }

  save_preferences(NULL, NULL);
}

//! Issued when the track duration for split after every n seconds is changed
static void spinner_time_changed(GtkSpinButton *spinner, ui_state *ui)
{
  gint time = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinner));
  gchar time_text[1024] = { '\0' };
  g_snprintf(time_text, 1024, _("Split every %2d seconds."), time);
  gtk_label_set_text(GTK_LABEL(ui->gui->time_label), time_text);

  save_preferences(NULL, NULL);
}

//! Issued when the number of tracks for equal length splitting is changed
static void spinner_equal_tracks_changed(GtkSpinButton *spinner, ui_state *ui)
{
  gint equal_tracks = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinner));
  gchar equal_tracks_text[1024] = { '\0' };
  g_snprintf(equal_tracks_text, 1024, _("Split in %2d equal time tracks."), equal_tracks);
  gtk_label_set_text(GTK_LABEL(ui->gui->equal_tracks_label), equal_tracks_text);

  save_preferences(NULL, NULL);
}

//! Issued when channge between single file and batch processing mode is requested
static void split_file_mode_changed(GtkToggleButton *radio_b, ui_state *ui)
{
  GSList *radio_button_list = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio_b));
  GtkToggleButton *current_radio_button = GTK_TOGGLE_BUTTON(g_slist_nth_data(radio_button_list,0));

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(current_radio_button)))
  {
    ui->infos->split_file_mode = FILE_MODE_MULTIPLE;
    gtk_widget_set_sensitive(ui->gui->multiple_files_component, TRUE);
  }
  else
  {
    ui->infos->split_file_mode = FILE_MODE_SINGLE;
    gtk_widget_set_sensitive(ui->gui->multiple_files_component, FALSE);
  }

  save_preferences(NULL, NULL);
}

//! Creates the split mode window part
static GtkWidget *create_split_mode()
{
  GtkWidget *local_vbox = wh_vbox_new();
  gtk_container_set_border_width(GTK_CONTAINER(local_vbox), 3);

  //normal split
  GtkWidget *split_mode_radio_button = gtk_radio_button_new_with_label(NULL, _("Normal"));
  ui->gui->split_mode_radio_button = split_mode_radio_button;
  gtk_box_pack_start(GTK_BOX(local_vbox), split_mode_radio_button, FALSE, FALSE, 2);
  g_signal_connect(GTK_TOGGLE_BUTTON(split_mode_radio_button),
      "toggled", G_CALLBACK(split_mode_changed), ui);
  g_object_set_data(G_OBJECT(split_mode_radio_button), "split_type_id",
      GINT_TO_POINTER(SELECTED_SPLIT_NORMAL));

  //time split
  split_mode_radio_button = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(split_mode_radio_button), _("Time"));
  ui->gui->split_mode_radio_button = split_mode_radio_button;
  gtk_box_pack_start(GTK_BOX(local_vbox), split_mode_radio_button, FALSE, FALSE, 2);
  g_signal_connect(GTK_TOGGLE_BUTTON(split_mode_radio_button), "toggled",
      G_CALLBACK(split_mode_changed), ui);
   g_object_set_data(G_OBJECT(split_mode_radio_button), "split_type_id",
      GINT_TO_POINTER(SELECTED_SPLIT_TIME));
 
  //
  GtkWidget *big_horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(local_vbox), big_horiz_fake, FALSE, FALSE, 0);

  GtkWidget *horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(big_horiz_fake), horiz_fake, FALSE, FALSE, 25);
  
  gint default_time = 60;

  gchar time_text[1024] = { '\0' };
  g_snprintf(time_text, 1024, _("Split every %2d seconds."), default_time);
  GtkWidget *time_label = gtk_label_new(time_text);
  ui->gui->time_label = time_label;
  gtk_box_pack_start(GTK_BOX(horiz_fake), time_label, FALSE, FALSE, 0);
  
  GtkAdjustment *adj = (GtkAdjustment *)gtk_adjustment_new(0.0, 1, 2000, 1.0, 10.0, 0.0);
  GtkWidget *spinner_time = gtk_spin_button_new(adj, 1, 0);
  ui->gui->spinner_time = spinner_time;
  gtk_box_pack_start(GTK_BOX(horiz_fake), spinner_time, FALSE, FALSE, 6);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_time), default_time);
  gtk_widget_set_sensitive(GTK_WIDGET(spinner_time), FALSE);
  gtk_widget_set_sensitive(time_label, FALSE);
  g_signal_connect(G_OBJECT(spinner_time), "value-changed", G_CALLBACK(spinner_time_changed), ui);

  //split in equal length
  split_mode_radio_button = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(split_mode_radio_button), _("Equal time tracks"));
  ui->gui->split_mode_radio_button = split_mode_radio_button;
  gtk_box_pack_start(GTK_BOX(local_vbox), split_mode_radio_button, FALSE, FALSE, 2);
  g_signal_connect(GTK_TOGGLE_BUTTON(split_mode_radio_button), "toggled",
      G_CALLBACK(split_mode_changed), ui);
  g_object_set_data(G_OBJECT(split_mode_radio_button), "split_type_id",
      GINT_TO_POINTER(SELECTED_SPLIT_EQUAL_TIME_TRACKS));

  //
  big_horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(local_vbox), big_horiz_fake, FALSE, FALSE, 0);

  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(big_horiz_fake), horiz_fake, FALSE, FALSE, 25);
  
  gint default_tracks = 10;

  gchar equal_length_text[1024] = { '\0' };
  g_snprintf(equal_length_text, 1024, _("Split in %2d equal time tracks."), default_tracks);
  GtkWidget *equal_tracks_label = gtk_label_new(time_text);
  ui->gui->equal_tracks_label = equal_tracks_label;
  gtk_box_pack_start(GTK_BOX(horiz_fake), equal_tracks_label, FALSE, FALSE, 0);
  
  adj = (GtkAdjustment *)gtk_adjustment_new(0.0, 1, 2000, 1.0, 10.0, 0.0);
  GtkWidget *spinner_equal_tracks = gtk_spin_button_new(adj, 1, 0);
  ui->gui->spinner_equal_tracks = spinner_equal_tracks;
  gtk_box_pack_start(GTK_BOX(horiz_fake), spinner_equal_tracks, FALSE, FALSE, 6);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_equal_tracks), default_time);
  gtk_widget_set_sensitive(GTK_WIDGET(spinner_equal_tracks), FALSE);
  gtk_widget_set_sensitive(equal_tracks_label, FALSE);
  g_signal_connect(G_OBJECT(spinner_equal_tracks), "value-changed",
      G_CALLBACK(spinner_equal_tracks_changed), ui);

  //trim using silence detection
  split_mode_radio_button = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(split_mode_radio_button), _("Trim using silence detection"));
  ui->gui->split_mode_radio_button = split_mode_radio_button;
  gtk_box_pack_start(GTK_BOX(local_vbox), split_mode_radio_button, FALSE, FALSE, 2);
  g_signal_connect(GTK_TOGGLE_BUTTON(split_mode_radio_button), "toggled",
      G_CALLBACK(split_mode_changed), ui);
  g_object_set_data(G_OBJECT(split_mode_radio_button), "split_type_id",
      GINT_TO_POINTER(SELECTED_SPLIT_TRIM_SILENCE));

  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(local_vbox), horiz_fake, FALSE, FALSE, 0);
  
  GtkWidget *param_vbox = wh_vbox_new();
  gtk_box_pack_start(GTK_BOX(horiz_fake), param_vbox, FALSE, FALSE, 25);

  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(param_vbox), horiz_fake, FALSE, FALSE, 0);

  GtkWidget *all_trim_threshold_label = gtk_label_new(_("Threshold level (dB) : "));
  ui->gui->all_trim_threshold_label = all_trim_threshold_label;
  gtk_box_pack_start(GTK_BOX(horiz_fake), all_trim_threshold_label, FALSE, FALSE, 0);
 
  adj = (GtkAdjustment *)gtk_adjustment_new(0.0, -96.0, 0.0, 0.5, 10.0, 0.0);
  GtkWidget *all_spinner_trim_silence_threshold = gtk_spin_button_new(adj, 0.5, 2);
  ui->gui->all_spinner_trim_silence_threshold = all_spinner_trim_silence_threshold;
  gtk_box_pack_start(GTK_BOX(horiz_fake), all_spinner_trim_silence_threshold, FALSE, FALSE, 0);

  //silence split
  split_mode_radio_button = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(split_mode_radio_button), _("Silence - split with silence detection"));
  ui->gui->split_mode_radio_button = split_mode_radio_button;
  gtk_box_pack_start(GTK_BOX(local_vbox), split_mode_radio_button, FALSE, FALSE, 2);
  g_signal_connect(GTK_TOGGLE_BUTTON(split_mode_radio_button), "toggled",
      G_CALLBACK(split_mode_changed), ui);
  g_object_set_data(G_OBJECT(split_mode_radio_button), "split_type_id",
      GINT_TO_POINTER(SELECTED_SPLIT_SILENCE));

  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(local_vbox), horiz_fake, FALSE, FALSE, 0);
  
  param_vbox = wh_vbox_new();
  gtk_box_pack_start(GTK_BOX(horiz_fake), param_vbox, FALSE, FALSE, 25);

  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(param_vbox), horiz_fake, FALSE, FALSE, 0);

  GtkWidget *all_threshold_label = gtk_label_new(_("Threshold level (dB) : "));
  ui->gui->all_threshold_label = all_threshold_label;
  gtk_box_pack_start(GTK_BOX(horiz_fake), all_threshold_label, FALSE, FALSE, 0);
 
  adj = (GtkAdjustment *)gtk_adjustment_new(0.0, -96.0, 0.0, 0.5, 10.0, 0.0);
  GtkWidget *all_spinner_silence_threshold = gtk_spin_button_new(adj, 0.5, 2);
  ui->gui->all_spinner_silence_threshold = all_spinner_silence_threshold;
  gtk_box_pack_start(GTK_BOX(horiz_fake), all_spinner_silence_threshold, FALSE, FALSE, 0);
 
  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(param_vbox), horiz_fake, FALSE, FALSE, 0);
  
  GtkWidget *all_offset_label = gtk_label_new(_("Cutpoint offset (0 is the begin of silence,and 1 the end) : "));
  ui->gui->all_offset_label = all_offset_label;
  gtk_box_pack_start(GTK_BOX(horiz_fake), all_offset_label, FALSE, FALSE, 0);

  adj = (GtkAdjustment *)gtk_adjustment_new(0.0, -2, 2, 0.05, 10.0, 0.0);
  GtkWidget *all_spinner_silence_offset = gtk_spin_button_new(adj, 0.05, 2);
  ui->gui->all_spinner_silence_offset = all_spinner_silence_offset;
  gtk_box_pack_start(GTK_BOX(horiz_fake), all_spinner_silence_offset, FALSE, FALSE, 0);
 
  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(param_vbox), horiz_fake, FALSE, FALSE, 0);

  GtkWidget *all_number_of_tracks_label = gtk_label_new(_("Number of tracks (0 means all tracks) : "));
  ui->gui->all_number_of_tracks_label = all_number_of_tracks_label;
  gtk_box_pack_start(GTK_BOX(horiz_fake), all_number_of_tracks_label, FALSE, FALSE, 0);
  
  adj = (GtkAdjustment *)gtk_adjustment_new(0.0, 0, 2000, 1, 10.0, 0.0);
  GtkWidget *all_spinner_silence_number_tracks = gtk_spin_button_new(adj, 1, 0);
  ui->gui->all_spinner_silence_number_tracks = all_spinner_silence_number_tracks;
  gtk_box_pack_start(GTK_BOX(horiz_fake), all_spinner_silence_number_tracks, FALSE, FALSE, 0);
  
  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(param_vbox), horiz_fake, FALSE, FALSE, 0);
  
  GtkWidget *all_min_silence_label = gtk_label_new(_("Minimum silence length (seconds) : "));
  ui->gui->all_min_silence_label = all_min_silence_label;
  gtk_box_pack_start(GTK_BOX(horiz_fake), all_min_silence_label, FALSE, FALSE, 0);

  adj = (GtkAdjustment *)gtk_adjustment_new(0.0, 0, 2000, 0.5, 10.0, 0.0);
  GtkWidget *all_spinner_silence_minimum = gtk_spin_button_new(adj, 1, 2);
  ui->gui->all_spinner_silence_minimum = all_spinner_silence_minimum;
  gtk_box_pack_start(GTK_BOX(horiz_fake), all_spinner_silence_minimum, FALSE, FALSE, 0);

  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(param_vbox), horiz_fake, FALSE, FALSE, 0);
  
  GtkWidget *all_min_track_label = gtk_label_new(_("Minimum track length (seconds) : "));
  ui->gui->all_min_track_label = all_min_track_label;
  gtk_box_pack_start(GTK_BOX(horiz_fake), all_min_track_label, FALSE, FALSE, 0);

  adj = (GtkAdjustment *)gtk_adjustment_new(0.0, 0, 2000, 0.5, 10.0, 0.0);
  GtkWidget *all_spinner_track_minimum = gtk_spin_button_new(adj, 1, 2);
  ui->gui->all_spinner_track_minimum = all_spinner_track_minimum;
  gtk_box_pack_start(GTK_BOX(horiz_fake), all_spinner_track_minimum, FALSE, FALSE, 0);

  GtkWidget *all_silence_remove_silence =
    gtk_check_button_new_with_label(_("Remove silence between tracks"));
  ui->gui->all_silence_remove_silence = all_silence_remove_silence;
  gtk_box_pack_start(GTK_BOX(param_vbox), all_silence_remove_silence, FALSE, FALSE, 0);

  gtk_spin_button_set_value(GTK_SPIN_BUTTON(all_spinner_silence_number_tracks),
                            SPLT_DEFAULT_PARAM_TRACKS);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(all_spinner_silence_minimum),
                            SPLT_DEFAULT_PARAM_MINIMUM_LENGTH);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(all_spinner_silence_offset),
                            SPLT_DEFAULT_PARAM_OFFSET);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(all_spinner_silence_threshold),
                            SPLT_DEFAULT_PARAM_THRESHOLD);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(all_spinner_trim_silence_threshold),
                            SPLT_DEFAULT_PARAM_THRESHOLD);

  deactivate_silence_parameters(ui->gui);
  deactivate_trim_parameters(ui->gui);

  //wrap split
  split_mode_radio_button = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(split_mode_radio_button),
     _("Wrap - split files created with mp3wrap or albumwrap (mp3 only)"));
  ui->gui->split_mode_radio_button = split_mode_radio_button;
  gtk_box_pack_start(GTK_BOX(local_vbox), split_mode_radio_button, FALSE, FALSE, 2);
  g_signal_connect(GTK_TOGGLE_BUTTON(split_mode_radio_button), "toggled",
      G_CALLBACK(split_mode_changed), ui);
  g_object_set_data(G_OBJECT(split_mode_radio_button), "split_type_id",
      GINT_TO_POINTER(SELECTED_SPLIT_WRAP));
  
  //error mode split
  split_mode_radio_button = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(split_mode_radio_button), _("Error mode (mp3 only)"));
  ui->gui->split_mode_radio_button = split_mode_radio_button;
  gtk_box_pack_start(GTK_BOX(local_vbox), split_mode_radio_button, FALSE, FALSE, 2);
  g_signal_connect(GTK_TOGGLE_BUTTON(split_mode_radio_button), "toggled",
      G_CALLBACK(split_mode_changed), ui);
  g_object_set_data(G_OBJECT(split_mode_radio_button), "split_type_id",
      GINT_TO_POINTER(SELECTED_SPLIT_ERROR));
 
  select_split_mode(SELECTED_SPLIT_NORMAL);

  GtkWidget *scrolled_window = wh_create_scrolled_window();
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), 
      GTK_WIDGET(local_vbox));

  return scrolled_window;
}

//! Creates the selection between single file split and batch processing
static GtkWidget *create_single_multiple_split_modes()
{
  GtkWidget *local_vbox = wh_vbox_new();
  gtk_container_set_border_width(GTK_CONTAINER(local_vbox), 3);

  //single file
  GtkWidget *file_mode_radio_button = gtk_radio_button_new_with_label(NULL, _("Single file"));
  gtk_box_pack_start(GTK_BOX(local_vbox), file_mode_radio_button, FALSE, FALSE, 2);
  g_signal_connect(GTK_TOGGLE_BUTTON(file_mode_radio_button),
      "toggled", G_CALLBACK(split_file_mode_changed), ui);
 
  //multiple files
  file_mode_radio_button = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(file_mode_radio_button), _("Batch processing"));
  gtk_box_pack_start(GTK_BOX(local_vbox), file_mode_radio_button, FALSE, FALSE, 2);
  g_signal_connect(GTK_TOGGLE_BUTTON(file_mode_radio_button),
      "toggled", G_CALLBACK(split_file_mode_changed), ui);

  ui->gui->file_mode_radio_button = file_mode_radio_button;

  GtkWidget *multiple_files_hbox = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(local_vbox), multiple_files_hbox, TRUE, TRUE, 2);

  GtkWidget *multiple_files_component = create_multiple_files_component();
  gtk_widget_set_sensitive(multiple_files_component, FALSE);
  gtk_box_pack_start(GTK_BOX(multiple_files_hbox), multiple_files_component, TRUE, TRUE, 5);
  ui->gui->multiple_files_component = multiple_files_component;

  GtkWidget *scrolled_window = wh_create_scrolled_window();
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), 
      GTK_WIDGET(local_vbox));

  return scrolled_window;
}

//!creates the special split page
GtkWidget *create_special_split_page()
{
  GtkWidget *vbox = wh_vbox_new();;

  GtkWidget *notebook = gtk_notebook_new();
  gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);
  gtk_notebook_popup_enable(GTK_NOTEBOOK(notebook));
  gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), TRUE);
  gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), FALSE);
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);

  GtkWidget *notebook_label = gtk_label_new(_("Split mode"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), create_split_mode(), notebook_label);

  notebook_label = gtk_label_new(_("File mode"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
      create_single_multiple_split_modes(), notebook_label);

  return vbox;
}

