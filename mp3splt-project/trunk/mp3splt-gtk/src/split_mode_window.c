/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
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
 * The batch processing tab
 *
 * this file is for the special split tab, where automatic
 * split modes can be selected
 *..*******************************************************/

#include "split_mode_window.h"

//! Get the split mode
static gint get_selected_split_mode_(GtkToggleButton *radio_b)
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
void select_split_mode(int split_mode, ui_state *ui)
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

gint get_selected_split_mode(ui_state *ui)
{
  return ui->status->selected_split_mode;
}

void set_selected_split_mode(gint value, ui_state *ui)
{
  ui->status->selected_split_mode = value;
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
  gint selected_split_mode = get_selected_split_mode_(radio_b);
  set_selected_split_mode(selected_split_mode, ui);

  int enable_time = (selected_split_mode == SELECTED_SPLIT_TIME);
  gtk_widget_set_sensitive(ui->gui->spinner_time, enable_time);
  gtk_widget_set_sensitive(ui->gui->time_label, enable_time);
  gtk_widget_set_sensitive(ui->gui->time_label_after, enable_time);

  int enable_split_equal_time = (selected_split_mode == SELECTED_SPLIT_EQUAL_TIME_TRACKS);
  gtk_widget_set_sensitive(ui->gui->spinner_equal_tracks, enable_split_equal_time);
  gtk_widget_set_sensitive(ui->gui->equal_tracks_label, enable_split_equal_time);
  gtk_widget_set_sensitive(ui->gui->equal_tracks_label_after, enable_split_equal_time);

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

  ui_save_preferences(NULL, ui);
}

//! Issued when the track duration for split after every n seconds is changed
static void spinner_time_changed(GtkSpinButton *spinner, ui_state *ui)
{
  ui_save_preferences(NULL, ui);
}

//! Issued when the number of tracks for equal length splitting is changed
static void spinner_equal_tracks_changed(GtkSpinButton *spinner, ui_state *ui)
{
  ui_save_preferences(NULL, ui);
}

//! Creates the split mode window part
static GtkWidget *create_split_mode(ui_state *ui)
{
  GtkWidget *local_vbox = wh_vbox_new();
  gtk_container_set_border_width(GTK_CONTAINER(local_vbox), 3);

  //normal split
  GtkWidget *split_mode_radio_button =
    gtk_radio_button_new_with_label(NULL, _("Use manual single file splitpoints"));
  gtk_widget_set_tooltip_text(split_mode_radio_button,
      _("Split files using the manually defined splitpoints"));
  ui->gui->split_mode_radio_button = split_mode_radio_button;
  gtk_box_pack_start(GTK_BOX(local_vbox), split_mode_radio_button, FALSE, FALSE, 2);
  g_signal_connect(GTK_TOGGLE_BUTTON(split_mode_radio_button),
      "toggled", G_CALLBACK(split_mode_changed), ui);
  g_object_set_data(G_OBJECT(split_mode_radio_button), "split_type_id",
      GINT_TO_POINTER(SELECTED_SPLIT_NORMAL));

  //time split
  split_mode_radio_button = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(split_mode_radio_button), _("Time"));
  gtk_widget_set_tooltip_text(split_mode_radio_button,
      _("Split every fixed time length; last track will most likely be smaller"));
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

  GtkWidget *time_label = gtk_label_new(_("Split every"));
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

  GtkWidget *time_label_after = gtk_label_new(_("seconds."));
  ui->gui->time_label_after = time_label_after;
  gtk_box_pack_start(GTK_BOX(horiz_fake), time_label_after, FALSE, FALSE, 0);
  gtk_widget_set_sensitive(time_label_after, FALSE);

  //split in equal length
  split_mode_radio_button = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(split_mode_radio_button), _("Equal time tracks"));
  gtk_widget_set_tooltip_text(split_mode_radio_button,
      _("Create smaller tracks having exactly the same time length"));
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
  
  GtkWidget *equal_tracks_label = gtk_label_new(_("Split in"));
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

  GtkWidget *equal_tracks_label_after = gtk_label_new(_("equal tracks."));
  ui->gui->equal_tracks_label_after = equal_tracks_label_after;
  gtk_box_pack_start(GTK_BOX(horiz_fake), equal_tracks_label_after, FALSE, FALSE, 0);
  gtk_widget_set_sensitive(equal_tracks_label_after, FALSE);
 
  //trim using silence detection
  split_mode_radio_button = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(split_mode_radio_button), _("Trim using silence detection"));
  gtk_widget_set_tooltip_text(split_mode_radio_button,
      _("Split by discarding silence from the begin and from the end\n"
        "If no silence is found, the output file is still created"));
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
  gtk_widget_set_tooltip_text(split_mode_radio_button, _("Split where silence is found"));
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
    (GTK_RADIO_BUTTON(split_mode_radio_button), _("Synchronisation error mode (mp3 only)"));
  gtk_widget_set_tooltip_text(split_mode_radio_button,
      _("Split where synchronisation errors are found\n"
        "Useful for splitting concatenated files"));
  ui->gui->split_mode_radio_button = split_mode_radio_button;
  gtk_box_pack_start(GTK_BOX(local_vbox), split_mode_radio_button, FALSE, FALSE, 2);
  g_signal_connect(GTK_TOGGLE_BUTTON(split_mode_radio_button), "toggled",
      G_CALLBACK(split_mode_changed), ui);
  g_object_set_data(G_OBJECT(split_mode_radio_button), "split_type_id",
      GINT_TO_POINTER(SELECTED_SPLIT_ERROR));
 
  //internal sheet mode split
  split_mode_radio_button = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(split_mode_radio_button), _("Internal sheet mode (CUE sheet for FLAC and ID3v2 chapters for MP3)"));
  gtk_widget_set_tooltip_text(split_mode_radio_button, _("Split using internal sheet"));
  ui->gui->split_mode_radio_button = split_mode_radio_button;
  gtk_box_pack_start(GTK_BOX(local_vbox), split_mode_radio_button, FALSE, FALSE, 2);
  g_signal_connect(GTK_TOGGLE_BUTTON(split_mode_radio_button), "toggled",
      G_CALLBACK(split_mode_changed), ui);
  g_object_set_data(G_OBJECT(split_mode_radio_button), "split_type_id",
      GINT_TO_POINTER(SELECTED_SPLIT_INTERNAL_SHEET));
 
  //CUE file with the same name as the input file
  split_mode_radio_button = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(split_mode_radio_button), _("Use CUE file with similar name as the input file"));
  gtk_widget_set_tooltip_text(split_mode_radio_button,
      _("Split using CUE file having similar name as the input file\n"
        "Example: test.cue will be looked for when splitting test.mp3"));
  ui->gui->split_mode_radio_button = split_mode_radio_button;
  gtk_box_pack_start(GTK_BOX(local_vbox), split_mode_radio_button, FALSE, FALSE, 2);
  g_signal_connect(GTK_TOGGLE_BUTTON(split_mode_radio_button), "toggled",
      G_CALLBACK(split_mode_changed), ui);
  g_object_set_data(G_OBJECT(split_mode_radio_button), "split_type_id",
      GINT_TO_POINTER(SELECTED_SPLIT_CUE_FILE));

  //CDDB file with the same name as the input file
  split_mode_radio_button = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(split_mode_radio_button), _("Use CDDB file with similar name as the input file"));
  gtk_widget_set_tooltip_text(split_mode_radio_button,
      _("Split using CDDB file having similar name as the input file\n"
        "Example: test.cddb will be looked for when splitting test.mp3"));
  ui->gui->split_mode_radio_button = split_mode_radio_button;
  gtk_box_pack_start(GTK_BOX(local_vbox), split_mode_radio_button, FALSE, FALSE, 2);
  g_signal_connect(GTK_TOGGLE_BUTTON(split_mode_radio_button), "toggled",
      G_CALLBACK(split_mode_changed), ui);
  g_object_set_data(G_OBJECT(split_mode_radio_button), "split_type_id",
      GINT_TO_POINTER(SELECTED_SPLIT_CDDB_FILE));

  select_split_mode(SELECTED_SPLIT_NORMAL, ui);

  GtkWidget *scrolled_window = wh_create_scrolled_window();
  wh_add_box_to_scrolled_window(local_vbox, scrolled_window);
  return scrolled_window;
}

//! Creates the selection between single file split and batch processing
static GtkWidget *create_single_multiple_split_modes(ui_state *ui)
{
  GtkWidget *local_vbox = wh_vbox_new();
  gtk_container_set_border_width(GTK_CONTAINER(local_vbox), 0);

  GtkWidget *multiple_files_hbox = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(local_vbox), multiple_files_hbox, TRUE, TRUE, 0);

  GtkWidget *multiple_files_component = create_multiple_files_component(ui);
  gtk_box_pack_start(GTK_BOX(multiple_files_hbox), multiple_files_component, TRUE, TRUE, 0);
  ui->gui->multiple_files_component = multiple_files_component;

  return local_vbox;
}

//!creates the special split page
GtkWidget *create_special_split_page(ui_state *ui)
{
  GtkWidget *vbox = wh_vbox_new();;

  GtkWidget *paned_widget = gtk_paned_new(GTK_ORIENTATION_VERTICAL);

  gtk_box_pack_start(GTK_BOX(vbox), paned_widget, TRUE, TRUE, 0);

  gtk_paned_add1(GTK_PANED(paned_widget), create_single_multiple_split_modes(ui));
  gtk_paned_add2(GTK_PANED(paned_widget), create_split_mode(ui));

  gtk_paned_set_position(GTK_PANED(paned_widget), 150);

  return vbox;
}

