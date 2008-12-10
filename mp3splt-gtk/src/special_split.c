/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2008 Alexandru Munteanu
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

//spinner time
GtkWidget *spinner_time;
//the selected split mode
gint selected_split_mode = SELECTED_SPLIT_NORMAL;

//returns the selected split mode
gint get_selected_split_mode(GtkToggleButton *radio_b)
{
  //get the radio buttons
  GSList *radio_button_list = NULL;
  radio_button_list = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio_b));
  //we check which bubble is checked
  GtkToggleButton *test = NULL;
  gint i = 0,selected = SELECTED_SPLIT_NORMAL;
  //for each split mode, get the selected one
  for(i = 0; i < NUMBER_OF_SPLIT_MODES;i++)
    {
      test = (GtkToggleButton *)
        g_slist_nth_data(radio_button_list,i);
      if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(test)))
        selected = i;
    }
  
  return selected;
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
    }
  
  //if we select time
  if (selected_split_mode == SELECTED_SPLIT_TIME)
    {
      //activate spinner time
      gtk_widget_set_sensitive(GTK_WIDGET(spinner_time), TRUE);
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
  GtkToggleButton *test = (GtkToggleButton *)
    g_slist_nth_data(radio_button_list, SELECTED_SPLIT_NORMAL);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(test),TRUE);  
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
  options_label = gtk_label_new(_("Split mode :"));
  gtk_box_pack_start (GTK_BOX (horiz_fake),
                      options_label, FALSE, FALSE, 0);
  
  GtkWidget *radio_button;
  //normal split
  radio_button = 
    gtk_radio_button_new_with_label(NULL, _("Normal"));
  gtk_box_pack_start (GTK_BOX (general_inside_vbox), radio_button, FALSE, FALSE, 2);
  g_signal_connect (GTK_TOGGLE_BUTTON (radio_button),
                    "toggled",
                    G_CALLBACK (split_mode_changed),
                    NULL);
  
  //time split
  radio_button = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON (radio_button), _("Time"));
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

  //wrap split
  radio_button = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON (radio_button), _("Wrap - split"
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
    (GTK_RADIO_BUTTON (radio_button), _("Error mode (mp3 only)"));
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
                                    _("Set default options"),FALSE); 
  g_signal_connect (G_OBJECT (set_default_prefs_button), "clicked",
                    G_CALLBACK (set_default_split_modes), radio_button);
  gtk_box_pack_start (GTK_BOX (horiz_fake), set_default_prefs_button,
                      FALSE, FALSE, 5);
  
  //put default values
  set_default_split_modes(NULL,radio_button);
  
  return general_hbox;
}
