/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2006 Munteanu Alexandru
 * Contact: io_alex_2002@yahoo.fr
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

/**********************************************************
 * Filename: special_split.c
 *
 * this file is for the special split tab, where we have special
 * splits 
 *
 *********************************************************/

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <libmp3splt/mp3splt.h>

#include "special_split.h"
#include "preferences_tab.h"
#include "main_win.h"

//silence mode parameters
//number of tracks parameter
GtkWidget *spinner_silence_number_tracks;
//number of tracks parameter
GtkWidget *spinner_silence_minimum;
//offset parameter
GtkWidget *spinner_silence_offset;
//threshold parameter
GtkWidget *spinner_silence_threshold;
//remove silence check button (silence mode parameter
GtkWidget *silence_remove_silence;

//spinner time
GtkWidget *spinner_time;
//the selected split mode
gint selected_split_mode = SELECTED_SPLIT_NORMAL;

//returns the selected split mode
gint get_selected_split_mode(GtkToggleButton *radio_b)
{
  //get the radio buttons
  GSList *radio_button_list;
  radio_button_list = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio_b));
  //we check which bubble is checked
  GtkToggleButton *test;
  gint i,selected = 4;
  //O = error mode;
  //1 = wrap split;
  //2 = silence split;
  //3 = time split;
  //4 = normal split;
  for(i = 0; i<5;i++)
    {
      test = (GtkToggleButton *)
        g_slist_nth_data(radio_button_list,i);
      if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(test)))
        selected = i;
    }
  
  return selected;
}

//deactivate silence parameters
void deactivate_silence_parameters()
{
  gtk_widget_set_sensitive(GTK_WIDGET(spinner_silence_number_tracks), FALSE);
  gtk_widget_set_sensitive(GTK_WIDGET(spinner_silence_minimum), FALSE);
  gtk_widget_set_sensitive(GTK_WIDGET(spinner_silence_offset), FALSE);
  gtk_widget_set_sensitive(GTK_WIDGET(spinner_silence_threshold), FALSE);
  gtk_widget_set_sensitive(GTK_WIDGET(silence_remove_silence), FALSE);
}

//activate silence parameters
void activate_silence_parameters()
{
  gtk_widget_set_sensitive(GTK_WIDGET(spinner_silence_number_tracks), TRUE);
  gtk_widget_set_sensitive(GTK_WIDGET(spinner_silence_minimum), TRUE);
  gtk_widget_set_sensitive(GTK_WIDGET(spinner_silence_offset), TRUE);
  gtk_widget_set_sensitive(GTK_WIDGET(spinner_silence_threshold), TRUE);
  gtk_widget_set_sensitive(GTK_WIDGET(silence_remove_silence), TRUE);
}

//when the split mode selection changed
void split_mode_changed (GtkToggleButton *radio_b,
                         gpointer data)
{
  selected_split_mode = get_selected_split_mode(radio_b);
  
  //if we select those modes
  if ((selected_split_mode == SELECTED_SPLIT_NORMAL)
      || (selected_split_mode == SELECTED_SPLIT_WRAP)
      || (selected_split_mode == SELECTED_SPLIT_ERROR))
    {
      //disable time and silence stuffs
      //deactivate spinner time
      gtk_widget_set_sensitive(GTK_WIDGET(spinner_time), FALSE);
      //deactivate silence parameters
      deactivate_silence_parameters();
    }
  
  //if we select time
  if (selected_split_mode == SELECTED_SPLIT_TIME)
    {
      //deactivate silence parameters
      deactivate_silence_parameters();
      //activate spinner time
      gtk_widget_set_sensitive(GTK_WIDGET(spinner_time), TRUE);
    }
  
  //if we select silence
  if (selected_split_mode == SELECTED_SPLIT_SILENCE)
    {
      //deactivate spinner time
      gtk_widget_set_sensitive(GTK_WIDGET(spinner_time), FALSE);
      //activate silence parameters
      activate_silence_parameters();
    }
}

//sets the default split modes
void set_default_split_modes (GtkWidget *widget, 
                              gpointer data)
{
  GtkToggleButton *radio_b= GTK_TOGGLE_BUTTON(data);
  //we activate the normal split button
  GSList *radio_button_list;
  radio_button_list = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio_b));
  //we check which bubble is checked
  GtkToggleButton *test;
  test = (GtkToggleButton *)
    g_slist_nth_data(radio_button_list,4);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(test),TRUE);
  
  //we set the default parameters for the silence split
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_silence_number_tracks),
                            SPLT_DEFAULT_PARAM_TRACKS);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_silence_minimum),
                            SPLT_DEFAULT_PARAM_MINIMUM_LENGTH);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_silence_offset),
                            SPLT_DEFAULT_PARAM_OFFSET);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_silence_threshold),
                            SPLT_DEFAULT_PARAM_THRESHOLD);
}

//creates the special split page
GtkWidget *create_special_split_page()
{
  //our general preferences vertical box
  GtkWidget *general_hbox;
  general_hbox = gtk_hbox_new(FALSE,0);
  
  //horizontal box in the scrolled window
  GtkWidget *general_inside_hbox;
  general_inside_hbox = gtk_hbox_new(FALSE,0);
  
  //scrolled window
  GtkWidget *scrolled_window;
  scrolled_window = (GtkWidget *)create_scrolled_window();
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), 
                                        GTK_WIDGET(general_inside_hbox));
  gtk_box_pack_start (GTK_BOX (general_hbox),
                      scrolled_window, TRUE, TRUE, 0);
  
  //vertical box inside the horizontal box from the scrolled window
  GtkWidget *general_inside_vbox;
  general_inside_vbox = gtk_vbox_new(FALSE, 0);;
  gtk_box_pack_start (GTK_BOX (general_inside_hbox),
                      general_inside_vbox, TRUE, TRUE, 8);
  
  GtkWidget *horiz_fake;
  GtkWidget *label;
  //horizontal fake widget box
  horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start (GTK_BOX (general_inside_vbox), 
                      horiz_fake, FALSE, FALSE, 5);
  
  //label for the split modes
  GtkWidget *options_label;
  options_label = gtk_label_new((gchar *)_("Split mode :"));
  gtk_box_pack_start (GTK_BOX (horiz_fake),
                      options_label, FALSE, FALSE, 0);
  
  GtkWidget *radio_button;
  //normal split
  radio_button = 
    gtk_radio_button_new_with_label(NULL,
                                    (gchar *)_("Normal"));
  gtk_box_pack_start (GTK_BOX (general_inside_vbox), radio_button, FALSE, FALSE, 2);
  g_signal_connect (GTK_TOGGLE_BUTTON (radio_button),
                    "toggled",
                    G_CALLBACK (split_mode_changed),
                    NULL);
  
  //time split
  radio_button = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON (radio_button), (gchar *)_("Time"));
  gtk_box_pack_start (GTK_BOX (general_inside_vbox), radio_button, FALSE, FALSE, 2);
  g_signal_connect (GTK_TOGGLE_BUTTON (radio_button),
                    "toggled",
                    G_CALLBACK (split_mode_changed),
                    NULL);
  
  //parameters for the silence option
  horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start (GTK_BOX (general_inside_vbox), 
                      horiz_fake, FALSE, FALSE, 0);
  
  //vertical parameter box
  GtkWidget *param_vbox;
  param_vbox = gtk_vbox_new(FALSE,0);
  gtk_box_pack_start (GTK_BOX (horiz_fake), 
                      param_vbox, FALSE, FALSE, 25);
  
  //horizontal box fake for the gap level
  horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start (GTK_BOX (param_vbox), 
                      horiz_fake, FALSE, FALSE, 0);
  
  //seconds for the time split
  label = gtk_label_new("split every X seconds. X = ");
  gtk_box_pack_start (GTK_BOX (horiz_fake), 
                      label, FALSE, FALSE, 0);
  
  //adjustement for the time spinner
  GtkAdjustment *adj;
  adj = (GtkAdjustment *) gtk_adjustment_new (0.0, 1, 2000, 1.0,
                                              10.0, 0.0);
  //the time spinner
  spinner_time =
    gtk_spin_button_new (adj, 1, 0);
  gtk_box_pack_start (GTK_BOX (horiz_fake), 
                      spinner_time,
                      FALSE, FALSE, 0);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_time),
                            60);
  
  //deactivate spinner time
  gtk_widget_set_sensitive(GTK_WIDGET(spinner_time), FALSE);
  
  //silence split
  radio_button = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON (radio_button), (gchar *)_("Silence - "
                                                 "split with silence "
                                                 "detection"));
  gtk_box_pack_start (GTK_BOX (general_inside_vbox), radio_button, FALSE, FALSE, 2);
  g_signal_connect (GTK_TOGGLE_BUTTON (radio_button),
                    "toggled",
                    G_CALLBACK (split_mode_changed),
                    NULL);
  
  //parameters for the silence option
  horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start (GTK_BOX (general_inside_vbox), 
                      horiz_fake, FALSE, FALSE, 0);
  
  //vertical parameter box
  param_vbox = gtk_vbox_new(FALSE,0);
  gtk_box_pack_start (GTK_BOX (horiz_fake), 
                      param_vbox, FALSE, FALSE, 25);
  
  //horizontal box fake for threshold level
  horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start (GTK_BOX (param_vbox), 
                      horiz_fake, FALSE, FALSE, 0);
  
  //threshold level
  label = gtk_label_new("threshold level (dB) : ");
  gtk_box_pack_start (GTK_BOX (horiz_fake), 
                      label, FALSE, FALSE, 0);
  
  //adjustement for the threshold spinner
  adj = (GtkAdjustment *) gtk_adjustment_new (0.0, -96.0, 0.0, 0.5,
                                              10.0, 0.0);
  //the threshold spinner
  spinner_silence_threshold =
    gtk_spin_button_new (adj, 0.5, 2);
  //set not editable
  gtk_box_pack_start (GTK_BOX (horiz_fake), 
                      spinner_silence_threshold,
                      FALSE, FALSE, 0);
  
  //horizontal box fake for the offset level
  horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start (GTK_BOX (param_vbox), 
                      horiz_fake, FALSE, FALSE, 0);
  
  //offset level
  label = gtk_label_new("cutpoint offset (0 is the begin of silence,"
                        "and 1 the end) : ");
  gtk_box_pack_start (GTK_BOX (horiz_fake), 
                      label, FALSE, FALSE, 0);
  
  //adjustement for the offset spinner
  adj = (GtkAdjustment *) gtk_adjustment_new (0.0, -2, 2, 0.05,
                                              10.0, 0.0);
  //the offset spinner
  spinner_silence_offset =
    gtk_spin_button_new (adj, 0.05, 2);
  gtk_box_pack_start (GTK_BOX (horiz_fake), 
                      spinner_silence_offset,
                      FALSE, FALSE, 0);
  
  //horizontal box fake for the number of tracks
  horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start (GTK_BOX (param_vbox), 
                      horiz_fake, FALSE, FALSE, 0);
  
  //number of tracks level
  label = gtk_label_new("number of tracks (0 means all tracks) : ");
  gtk_box_pack_start (GTK_BOX (horiz_fake), 
                      label, FALSE, FALSE, 0);
  
  //number of tracks
  adj = (GtkAdjustment *) gtk_adjustment_new (0.0, 0, 2000, 1,
                                              10.0, 0.0);
  //the number of tracks spinner
  spinner_silence_number_tracks =
    gtk_spin_button_new (adj, 1, 0);
  gtk_box_pack_start (GTK_BOX (horiz_fake), 
                      spinner_silence_number_tracks,
                      FALSE, FALSE, 0);
  
  //horizontal box fake for minimum length parameter
  horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start (GTK_BOX (param_vbox), 
                      horiz_fake, FALSE, FALSE, 0);
  
  //the minimum length parameter
  label = gtk_label_new("minimum silence length (seconds) : ");
  gtk_box_pack_start (GTK_BOX (horiz_fake),
                      label, FALSE, FALSE, 0);
  
  //minimum silence length (seconds)
  adj = (GtkAdjustment *) gtk_adjustment_new (0.0, 0, 2000, 0.5,
                                              10.0, 0.0);
  //the minimum silence length in seconds
  spinner_silence_minimum =
    gtk_spin_button_new (adj, 1, 2);
  gtk_box_pack_start (GTK_BOX (horiz_fake), 
                      spinner_silence_minimum,
                      FALSE, FALSE, 0);
  
  //remove silence (rm) : allows you to remove the silence between
  //tracks
  silence_remove_silence =
    gtk_check_button_new_with_label((gchar *)
                                    _(" remove silence between "
                                      "tracks"));
  gtk_box_pack_start (GTK_BOX (param_vbox), 
                      silence_remove_silence,
                      FALSE, FALSE, 0);
  
  //wrap split
  radio_button = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON (radio_button), (gchar *)_("Wrap - split"
                                                 " files created with "
                                                 "mp3wrap or albumwrap"
                                                 " (mp3 only)"));
  gtk_box_pack_start (GTK_BOX (general_inside_vbox), radio_button, FALSE, FALSE, 2);
  g_signal_connect (GTK_TOGGLE_BUTTON (radio_button),
                    "toggled",
                    G_CALLBACK (split_mode_changed),
                    NULL);
  
  //error mode split
  radio_button = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON (radio_button), (gchar *)_("Error mode "
                                                 "(mp3 only)"));
  gtk_box_pack_start (GTK_BOX (general_inside_vbox), radio_button, FALSE, FALSE, 2);
  g_signal_connect (GTK_TOGGLE_BUTTON (radio_button),
                    "toggled",
                    G_CALLBACK (split_mode_changed),
                    NULL);
  
  //set default preferences button
  //horizontal box fake for the gap level
  horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start (GTK_BOX (general_inside_vbox), 
                      horiz_fake, FALSE, FALSE, 0);
  
  GtkWidget *set_default_prefs_button;
  set_default_prefs_button =
    (GtkWidget *)create_cool_button(GTK_STOCK_PREFERENCES,
                                    (gchar *)_("Set default"
                                               " options"),FALSE); 
  g_signal_connect (G_OBJECT (set_default_prefs_button), "clicked",
                    G_CALLBACK (set_default_split_modes), radio_button);
  gtk_box_pack_start (GTK_BOX (horiz_fake), set_default_prefs_button,
                      FALSE, FALSE, 5);
  
  //deactivate the silence parameters spinners
  deactivate_silence_parameters();
  //put default values
  set_default_split_modes(NULL,radio_button);
  
  return general_hbox;
}
