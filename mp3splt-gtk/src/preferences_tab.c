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
 * Filename: preferences_tab.c
 *
 * this file is for the preferences tab, where we choose the preferences.
 *
 *********************************************************/

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <string.h>
#include <libmp3splt/mp3splt.h>

#include "player.h"
#include "util.h"
#include "player_tab.h"
#include "utilities.h"
#include "main_win.h"

//save button
GtkWidget *save_button;
//revert to save button
GtkWidget *revert_to_save_button;

//if we check the enable_save dialog
gint save_dialog;
//file selection checker
GtkWidget *check_save_dialog;

/* split preferences for choosing directory */
//directory entry
GtkWidget *directory_entry;

//output for the cddb,cue and freedb file output
GtkWidget *output_entry;

//if we have selected a correct file
gint incorrect_selected_dir = FALSE;

//choose the player box
GtkWidget *player_combo_box;

//list where we stock the preferences combo box content
GList *player_pref_list = NULL;
//selected player
gint selected_player = PLAYER_SNACKAMP;

//the language radio button
GtkWidget *radio_button;

//radio button for choosing default or custom output options
GtkWidget *radio_output;

//radio button for tags options
GtkWidget *tags_radio;

//split options
//frame mode option
GtkWidget *frame_mode;
//auto-adjust option
GtkWidget *adjust_mode;
//seekable option
GtkWidget *seekable_mode;

//adjust mode parameters
//gap parameter
GtkWidget *spinner_adjust_gap;
//offset parameter
GtkWidget *spinner_adjust_offset;
//threshold parameter
GtkWidget *spinner_adjust_threshold;

extern GtkWidget *player_box;
extern GtkWidget *queue_files_button;
extern splt_state *the_state;

//creates a scrolled window
GtkWidget *create_scrolled_window()
{
  GtkWidget *scrolled_window;
  scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_set_size_request(scrolled_window, 300,130);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_NONE);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_AUTOMATIC);
  return scrolled_window;
}

//updates the save buttons if the preferences content is modified from
//the content of the config file
void update_save_buttons()
{
  //if the preferences are different from the config file, update the
  //save button and the revert save button
  if (check_if_different_from_config_file())
    {
      gtk_widget_set_sensitive(GTK_WIDGET(save_button), TRUE);
      gtk_widget_set_sensitive(GTK_WIDGET(revert_to_save_button), TRUE);
    }
  else
    {
      gtk_widget_set_sensitive(GTK_WIDGET(save_button), FALSE);
      gtk_widget_set_sensitive(GTK_WIDGET(revert_to_save_button), FALSE);
    }
}

//check save dialog event
void check_save_dialog_event (GtkToggleButton *check_button,
                              gpointer data)
{
  //variable to stock if the button is checked or not
  gint checked;
  checked =
    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_button));

  //if checked
  if (checked)
    save_dialog = TRUE;
  else
    save_dialog = FALSE;

  update_save_buttons();
}

//radio box changed event
void radio_box_changed_event (GtkToggleButton *radio_b,
                              gpointer data)
{
  update_save_buttons();
}

//returns the checked output radio box
gboolean get_checked_output_radio_box()
{
  //get the radio buttons
  GSList *radio_button_list;
  radio_button_list = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio_output));
  //we check which bubble is checked
  GtkToggleButton *test;
  gint i, selected = 0;
  //O = default output mode
  //1 = custom output mode
  for(i = 0; i<2;i++)
    {
      test = (GtkToggleButton *)
        g_slist_nth_data(radio_button_list,i);
      if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(test)))
        selected = i;
    }
  
  return selected;
}

//cddb and cue output mode radio box event
void output_radio_box_event  (GtkToggleButton *radio_b,
                              gpointer data)
{
  gint selected = get_checked_output_radio_box();
  
  //we have in selected the selected mode
  //if we select the custom output mode
  if (selected == 0)
    {
      //we set the type entry available
      gtk_widget_set_sensitive(GTK_WIDGET(output_entry), TRUE);
      //we put default output to false
      mp3splt_set_int_option(the_state, SPLT_OPT_OUTPUT_FILENAMES,
                             SPLT_OUTPUT_FORMAT);
    }
  else
    {
      //we set the type entry unavailable
      gtk_widget_set_sensitive(GTK_WIDGET(output_entry), FALSE);
      //we put default output to true
      mp3splt_set_int_option(the_state, SPLT_OPT_OUTPUT_FILENAMES,
                             SPLT_OUTPUT_DEFAULT);
    }
  
  update_save_buttons();
}

//creates the general preferences page
GtkWidget *create_pref_general_page()
{
  //our general preferences vertical box
  GtkWidget *general_hbox;
  general_hbox = gtk_hbox_new(FALSE, 0);;
  
  //vertical box inside the scrolled window
  GtkWidget *general_inside_hbox;
  general_inside_hbox = gtk_hbox_new(FALSE, 0);;

  //scrolled window
  GtkWidget *scrolled_window;
  scrolled_window = create_scrolled_window();
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), 
                                        GTK_WIDGET(general_inside_hbox));
  gtk_box_pack_start (GTK_BOX (general_hbox), 
                      scrolled_window, TRUE, TRUE, 0);
  
  //vertical box inside the horizontal box from the scrolled window
  GtkWidget *general_inside_vbox;
  general_inside_vbox = gtk_vbox_new(FALSE, 0);;
  gtk_box_pack_start (GTK_BOX (general_inside_hbox),
                      general_inside_vbox, TRUE, TRUE, 8);
  
  //fake horizontal box
  GtkWidget *label;
  GtkWidget *horiz_fake;
  horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start (GTK_BOX (general_inside_vbox), 
                      horiz_fake, FALSE, FALSE, 5);
  label = gtk_label_new((gchar *)_("Save preferences :"));
  gtk_box_pack_start (GTK_BOX (horiz_fake), label, FALSE, FALSE, 0);
  
  
  //save dialog check button
  check_save_dialog =
    gtk_check_button_new_with_label((gchar *)_(" popup save dialog confirmation "
                                               "if preferences have changed"));
  //event for the file selection check button
  g_signal_connect (G_OBJECT (check_save_dialog),
                    "toggled",
                    G_CALLBACK (check_save_dialog_event), NULL);
  gtk_box_pack_start (GTK_BOX (general_inside_vbox), 
                      check_save_dialog, FALSE, FALSE, 0);
  
  // choose language radio items
  GtkWidget *radio_vbox;
  radio_vbox = gtk_vbox_new (FALSE, 0);
  
  //choose language label
  //fake horizontal box
  horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start (GTK_BOX (radio_vbox), 
                      horiz_fake, FALSE, FALSE, 5);
  
  label = gtk_label_new((gchar *)_("Choose language (require restart) :"));
  gtk_box_pack_start (GTK_BOX (horiz_fake), label, FALSE, FALSE, 0);
  
  GtkWidget *radio_box;
  radio_box = gtk_hbox_new (FALSE, 0);
  //event for the radio button
  gtk_box_pack_start (GTK_BOX (radio_vbox), radio_box, TRUE, TRUE, 2);
  //language radio items
  radio_button = gtk_radio_button_new_with_label(NULL, (gchar *)_("english"));
  g_signal_connect (GTK_TOGGLE_BUTTON (radio_button),
                    "toggled",
                    G_CALLBACK (radio_box_changed_event),
                    NULL);
  
  gtk_box_pack_start (GTK_BOX (radio_box), radio_button, TRUE, TRUE, 2);
  radio_button = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON (radio_button), (gchar *)_("french"));
  gtk_box_pack_start (GTK_BOX (radio_box), radio_button, TRUE, TRUE, 2);
  
  //add the radio button to the vertical box
  gtk_box_pack_start (GTK_BOX (general_inside_vbox), 
                      radio_vbox, FALSE, FALSE, 0);
  
  return general_hbox;
}

//events for browse dir button
void browse_dir_button_event( GtkWidget *widget,
                              gpointer   data )
{
  // file chooser
  GtkWidget *dir_chooser;
      
  //creates and shows the dialog
  dir_chooser = gtk_file_chooser_dialog_new ((gchar *)_("Choose split directory"),
                                             NULL,
                                             GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
                                             GTK_STOCK_CANCEL,
                                             GTK_RESPONSE_CANCEL,
                                             GTK_STOCK_OPEN,
                                             GTK_RESPONSE_ACCEPT,
                                             NULL);
  //if we push open, ..
  if (gtk_dialog_run (GTK_DIALOG (dir_chooser)) 
      == GTK_RESPONSE_ACCEPT)
    {
      gchar *filename;
      filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER(dir_chooser));
          
      //puts the text on the entry
      gtk_entry_set_text(GTK_ENTRY(directory_entry), 
                         filename);
      update_save_buttons();
      
      g_free (filename);
    }
  
  //destroys dialog
  gtk_widget_destroy (dir_chooser);
}

//disables adjust parameters
void disable_adjust_spinners()
{
  gtk_widget_set_sensitive(spinner_adjust_threshold, FALSE);
  gtk_widget_set_sensitive(spinner_adjust_offset, FALSE);
  gtk_widget_set_sensitive(spinner_adjust_gap, FALSE);
}

//enables adjust parameters
void enables_adjust_spinners()
{
  gtk_widget_set_sensitive(spinner_adjust_threshold, TRUE);
  gtk_widget_set_sensitive(spinner_adjust_offset, TRUE);
  gtk_widget_set_sensitive(spinner_adjust_gap, TRUE);
}

//adjust event
void adjust_event (GtkToggleButton *adjust_mode,
                   gpointer user_data)
{
  //if it is toggled
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(adjust_mode)))
    {
      if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(seekable_mode)))
        {
          gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(seekable_mode),FALSE);
        }
      if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(frame_mode)))
        {
          gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(frame_mode),TRUE);
        }
      enables_adjust_spinners();
    }
  else
    {
      //disable spinners
      disable_adjust_spinners();
    }
  update_save_buttons();
}

//seekable event
void seekable_event (GtkToggleButton *seekable,
                     gpointer user_data)
{
  //if it is toggled
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(seekable)))
    {
      if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(adjust_mode)))
        {
          gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(adjust_mode),FALSE);
        }
    }
  update_save_buttons();
}

//frame mode event
void frame_event (GtkToggleButton *frame_mode,
                  gpointer user_data)
{
  //if it is toggled
  if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(frame_mode)))
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(adjust_mode)))
      {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(adjust_mode),FALSE);
      }
  
  update_save_buttons();
}

//action for the set default prefs button
void set_default_prefs_event (GtkWidget *widget, 
                              gpointer data)
{
  //set frame mode active
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(frame_mode),
                               TRUE);
  //set seek mode inactive
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(seekable_mode),
                               FALSE);
  //set adjust mode inactive
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(adjust_mode),
                               FALSE);
  //set adjust mode preferences
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_adjust_threshold),
                            SPLT_DEFAULT_PARAM_THRESHOLD);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_adjust_offset),
                            SPLT_DEFAULT_PARAM_OFFSET);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_adjust_gap),
                            SPLT_DEFAULT_PARAM_GAP);
}

//events for the "set current song directory"
void song_dir_button_event( GtkWidget *widget,
                            gpointer   data )
{
  gtk_entry_set_text(GTK_ENTRY(directory_entry), "");
  update_save_buttons();
}

//returns the checked tags radio box
gint get_checked_tags_radio_box()
{
  //get the radio buttons
  GSList *radio_button_list;
  radio_button_list = 
    gtk_radio_button_get_group(GTK_RADIO_BUTTON(tags_radio));
  //we check which bubble is checked
  GtkToggleButton *test;
  gint i, selected = 0;
  //O = No tags
  //1 = Default tags
  //2 = Original file tags
  for(i = 0; i<3;i++)
    {
      test = (GtkToggleButton *)
        g_slist_nth_data(radio_button_list,i);
      if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(test)))
        {
          selected = i;
        }
    }
  
  return selected;
}

//tags radio changed event
void tags_radio_changed_event (GtkToggleButton *radio_b,
                               gpointer data)
{
  update_save_buttons();
}

//creates the splitpoints preferences page
GtkWidget *create_pref_splitpoints_page()
{
  //our general preferences vertical box
  GtkWidget *general_hbox;
  general_hbox = gtk_hbox_new(FALSE,0);
  
  //horizontal box in the scrolled window
  GtkWidget *general_inside_hbox;
  general_inside_hbox = gtk_hbox_new(FALSE,0);
  
  //scrolled window
  GtkWidget *scrolled_window;
  scrolled_window = create_scrolled_window();
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), 
                                        GTK_WIDGET(general_inside_hbox));
  gtk_box_pack_start (GTK_BOX (general_hbox),
                      scrolled_window, TRUE, TRUE, 0);
  
  //vertical box inside the horizontal box from the scrolled window
  GtkWidget *general_inside_vbox;
  general_inside_vbox = gtk_vbox_new(FALSE, 0);;
  gtk_box_pack_start (GTK_BOX (general_inside_hbox),
                      general_inside_vbox, TRUE, TRUE, 8);
  
  //horizontal fake widget box
  GtkWidget *horiz_fake;
  horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start (GTK_BOX (general_inside_vbox), 
                      horiz_fake, FALSE, FALSE, 5);
  
  //directory label
  GtkWidget *label;
  label = gtk_label_new((gchar *)_("Directory for splitted files :"));
  gtk_box_pack_start (GTK_BOX (horiz_fake), 
                      label, FALSE, FALSE, 0);
  
  //horizontal box for the entry and the browse dir button 
  GtkWidget *dir_hbox;
  dir_hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start (GTK_BOX (general_inside_vbox), 
                      dir_hbox, FALSE, FALSE, 0);
  
  //directory entry
  directory_entry = gtk_entry_new();
  gtk_entry_set_editable (GTK_ENTRY (directory_entry), FALSE);
  gtk_box_pack_start (GTK_BOX (dir_hbox), 
                      directory_entry, TRUE, TRUE, 10);
  
  //browse dir button
  GtkWidget *browse_dir_button;
  browse_dir_button = (GtkWidget *)
    create_cool_button(GTK_STOCK_DIRECTORY,(gchar *)_("_Browse dir"),
                       FALSE);
  g_signal_connect (G_OBJECT (browse_dir_button), "clicked",
                    G_CALLBACK (browse_dir_button_event), NULL);
  gtk_box_pack_start (GTK_BOX (dir_hbox), 
                      browse_dir_button, FALSE, FALSE, 0);
  
  //to set the directory for splitted files to the current song
  //directory
  GtkWidget *song_dir_button;
  song_dir_button = (GtkWidget *)
    gtk_button_new_with_label((gchar *)_("Song dir"));
  g_signal_connect (G_OBJECT (song_dir_button), "clicked",
                    G_CALLBACK (song_dir_button_event), NULL);
  gtk_box_pack_start (GTK_BOX (dir_hbox), 
                      song_dir_button,
                      FALSE, FALSE, 5);
  
  //horizontal fake widget box
  horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start (GTK_BOX (general_inside_vbox), 
                      horiz_fake, FALSE, FALSE, 5);
  //label for the split options
  GtkWidget *options_label;
  options_label = gtk_label_new((gchar *)_("Split options :"));
  gtk_box_pack_start (GTK_BOX (horiz_fake),
                      options_label, FALSE, FALSE, 0);
  
  //frame mode option
  frame_mode =
    gtk_check_button_new_with_label((gchar *)
                                    _(" frame mode (useful"
                                      " for mp3 VBR) (mp3 only)"));
  gtk_box_pack_start (GTK_BOX(general_inside_vbox),
                      frame_mode, FALSE, FALSE, 0);
  //frame mode event
  g_signal_connect (G_OBJECT (frame_mode),
                    "toggled",
                    G_CALLBACK (frame_event), NULL);
  
  //seekable option
  seekable_mode =
    gtk_check_button_new_with_label((gchar *)
                                    _(" input not seekable (for streams that can "
                                      "be read only one time)\n"
                                      "(works only with normal and "
                                      "time split modes) (mp3 only)"));
  gtk_box_pack_start (GTK_BOX(general_inside_vbox),
                      seekable_mode, FALSE, FALSE, 0);
  //seekable event
  g_signal_connect (G_OBJECT (seekable_mode),
                    "toggled",
                    G_CALLBACK (seekable_event), NULL);
  
  //auto adjust mode option
  adjust_mode =
    gtk_check_button_new_with_label((gchar *)
                                    _(" auto-adjust mode (uses"
                                      " silence detection to auto"
                                      "-adjust splitpoints)"));
  gtk_box_pack_start (GTK_BOX(general_inside_vbox),
                      adjust_mode, FALSE, FALSE, 0);
  //auto adjust event
  g_signal_connect (G_OBJECT (adjust_mode),
                    "toggled",
                    G_CALLBACK (adjust_event), NULL);
  
  //parameters for the adjust option
  horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start (GTK_BOX (general_inside_vbox), 
                      horiz_fake, FALSE, FALSE, 0);
  
  //vertical parameter box
  GtkWidget *param_vbox;
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
  GtkAdjustment *adj;
  adj = (GtkAdjustment *) gtk_adjustment_new (0.0, -96.0, 0.0, 0.5,
                                              10.0, 0.0);
  //the threshold spinner
  spinner_adjust_threshold =
    gtk_spin_button_new (adj, 0.5, 2);
  g_signal_connect (G_OBJECT (spinner_adjust_threshold), "value_changed",
                    G_CALLBACK (update_save_buttons), NULL);
  //set not editable
  gtk_box_pack_start (GTK_BOX (horiz_fake), 
                      spinner_adjust_threshold,
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
  spinner_adjust_offset =
    gtk_spin_button_new (adj, 0.05, 2);
  g_signal_connect (G_OBJECT (spinner_adjust_offset), "value_changed",
                    G_CALLBACK (update_save_buttons), NULL);
  gtk_box_pack_start (GTK_BOX (horiz_fake), 
                      spinner_adjust_offset,
                      FALSE, FALSE, 0);
  
  //horizontal box fake for the gap level
  horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start (GTK_BOX (param_vbox), 
                      horiz_fake, FALSE, FALSE, 0);
  
  //gap level (seconds)
  label = gtk_label_new("gap level (seconds around splitpoint to "
                        "search for silence) : ");
  gtk_box_pack_start (GTK_BOX (horiz_fake), 
                      label, FALSE, FALSE, 0);
  
  //adjustement for the gap spinner
  adj = (GtkAdjustment *) gtk_adjustment_new (0.0, 0, 2000, 1.0,
                                              10.0, 0.0);
  //the gap spinner
  spinner_adjust_gap =
    gtk_spin_button_new (adj, 1, 0);
  g_signal_connect (G_OBJECT (spinner_adjust_gap), "value_changed",
                    G_CALLBACK (update_save_buttons), NULL);
  gtk_box_pack_start (GTK_BOX (horiz_fake), 
                      spinner_adjust_gap,
                      FALSE, FALSE, 0);
  
  //disable spinners
  disable_adjust_spinners();
  
  //set default preferences button
  //horizontal box fake for the gap level
  horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start (GTK_BOX (general_inside_vbox), 
                      horiz_fake, FALSE, FALSE, 0);
  
  GtkWidget *set_default_prefs_button;
  set_default_prefs_button =
    (GtkWidget *)create_cool_button(GTK_STOCK_PREFERENCES,
                                    (gchar *)_("Set default split"
                                               " options"),FALSE); 
  g_signal_connect (G_OBJECT (set_default_prefs_button), "clicked",
                    G_CALLBACK (set_default_prefs_event), NULL);
  gtk_box_pack_start (GTK_BOX (horiz_fake), set_default_prefs_button,
                      FALSE, FALSE, 5);
  
  //horizontal fake widget box
  horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start (GTK_BOX (general_inside_vbox), 
                      horiz_fake, FALSE, FALSE, 6);

  //label for the split options
  GtkWidget *tag_label;

  //if library has been compiled with libid3tag
  tag_label = gtk_label_new((gchar *)_("Tags options :"));
  gtk_box_pack_start (GTK_BOX (horiz_fake),
                      tag_label, FALSE, FALSE, 0);
      
  //tags radio items
  tags_radio = 
    gtk_radio_button_new_with_label(NULL,
                                    (gchar *)_("Original file tags"));
  gtk_box_pack_start (GTK_BOX (general_inside_vbox), tags_radio,
                      FALSE, FALSE, 2);
  g_signal_connect (GTK_TOGGLE_BUTTON (tags_radio),
                    "toggled",
                    G_CALLBACK (tags_radio_changed_event),
                    NULL);
  tags_radio = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON (tags_radio), (gchar *)_("Default tags (cddb or"
                                               " cue tags)"));
  g_signal_connect (GTK_TOGGLE_BUTTON (tags_radio),
                    "toggled",
                    G_CALLBACK (tags_radio_changed_event),
                    NULL);
  gtk_box_pack_start (GTK_BOX (general_inside_vbox), tags_radio,
                      FALSE, FALSE, 2);
  tags_radio = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON (tags_radio), (gchar *)_("No tags"));
  g_signal_connect (GTK_TOGGLE_BUTTON (tags_radio),
                    "toggled",
                    G_CALLBACK (tags_radio_changed_event),
                    NULL);
  gtk_box_pack_start (GTK_BOX (general_inside_vbox), 
                      tags_radio, FALSE, FALSE, 2);

  //if the library has been compiled with id3tag support
  /*if (!mp3splt_has_id3tag())
    {
      //horizontal fake widget box
      horiz_fake = gtk_hbox_new(FALSE,0);
      gtk_box_pack_start (GTK_BOX (general_inside_vbox), 
                          horiz_fake, FALSE, FALSE, 6);
      
      tag_label = gtk_label_new((gchar *)_("-libmp3splt has been compiled"
                                           " without id3tag support :\n"
                                           "   we don't write"
                                           " the original mp3 tags to the splitted files"));
      gtk_box_pack_start (GTK_BOX (horiz_fake),
                          tag_label, FALSE, FALSE, 0);
    }*/
  
  return general_hbox;
}

//returns the selected language
//must be free() after
GString *get_checked_language()
{
  //the selected language
  GString *selected_lang = NULL;
  
  //get the radio buttons
  GSList *radio_button_list;
  radio_button_list = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio_button));
  GtkWidget *our_button;
  //0 = french, 1 = english
  our_button = 
    (GtkWidget *)g_slist_nth_data(radio_button_list, 0);
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(our_button)))
    selected_lang = g_string_new("fr");
  else 
    {
      our_button = 
        (GtkWidget *)g_slist_nth_data(radio_button_list, 1);
      if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(our_button)))
        selected_lang = g_string_new("en");
    }
  
  return selected_lang;
}

//save preferences event
void save_preferences_event (GtkWidget *widget, 
                             gpointer data)
{
  gchar *filename;
  filename = (gchar *)
    get_preferences_filename();

  //our key file
  GKeyFile *my_key_file = g_key_file_new();
  g_key_file_load_from_file(my_key_file,
                            filename,
                            G_KEY_FILE_KEEP_COMMENTS,
                            NULL);
  //save_dialog
  g_key_file_set_boolean(my_key_file,
                         "general",
                         "save_dialog",
                         save_dialog);
  //save_path
  g_key_file_set_string(my_key_file,
                        "split",
                        "save_path",
                        gtk_entry_get_text(GTK_ENTRY(directory_entry)));
  //default_player
  g_key_file_set_integer(my_key_file,
                         "player",
                         "default_player",
                         selected_player);

  //saves the language
  GString *selected_lang;
  selected_lang = (GString *)get_checked_language();
  g_key_file_set_string(my_key_file,
                        "general",
                        "language",
                        selected_lang->str);
  
  g_string_free(selected_lang, TRUE);
  
  //write frame mode
  g_key_file_set_boolean(my_key_file,
                         "split",
                         "frame_mode",
                         gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(frame_mode)));
  //write seekable mode
  g_key_file_set_boolean(my_key_file,
                         "split",
                         "seekable_mode",
                         gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(seekable_mode)));
  //write adjust mode
  g_key_file_set_boolean(my_key_file,
                         "split",
                         "adjust_mode",
                         gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(adjust_mode)));
  //write adjust parameters :
  //write adjust threshold
  g_key_file_set_integer(my_key_file,
                         "split",
                         "adjust_threshold",
                         gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinner_adjust_threshold)) * 100);
  //write adjust offset
  g_key_file_set_integer(my_key_file,
                         "split",
                         "adjust_offset",
                         gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinner_adjust_offset)) * 100);
  //write adjust gap
  g_key_file_set_integer(my_key_file,
                         "split",
                         "adjust_gap",
                         gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinner_adjust_gap)));
  
  //write the output format
  g_key_file_set_string(my_key_file,
                        "output",
                        "output_format",
                        gtk_entry_get_text(GTK_ENTRY(output_entry)));
  
  //write the default output format
  g_key_file_set_boolean(my_key_file,
                         "output",
                         "default_output_format",
                         get_checked_output_radio_box());
  
  //write the default output format
  g_key_file_set_integer(my_key_file,
                         "split",
                         "tags",
                         get_checked_tags_radio_box());
  
  //our data
  gchar *key_data;
  key_data = g_key_file_to_data(my_key_file,
                                NULL,
                                NULL);
  
  //we write to the preference file
  FILE *preferences_file;
  preferences_file = (FILE *)g_fopen(filename,"w");
  g_fprintf(preferences_file,"%s", key_data);
  
  fclose(preferences_file);
  
  //we free memory
  g_free(key_data);
  g_free(filename);
  g_key_file_free(my_key_file);
  
  gtk_widget_set_sensitive(GTK_WIDGET(save_button), FALSE);
  gtk_widget_set_sensitive(GTK_WIDGET(revert_to_save_button), FALSE);
}

//revert to save event
void revert_to_save_event (GtkToggleButton *check_button,
                                 gpointer data)
{
  load_preferences();
}

//popup the save preferences dialog
void popup_save_preferences_dialog()
{
  GtkWidget *dialog, *label;
   
  dialog = gtk_dialog_new_with_buttons ((gchar *)_("Save preferences ?"),
                                        NULL,
                                        GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_STOCK_SAVE,
                                        GTK_RESPONSE_YES,
                                        GTK_STOCK_REVERT_TO_SAVED,
                                        GTK_RESPONSE_NO,
                                        GTK_STOCK_CANCEL,
                                        GTK_RESPONSE_CANCEL,
                                        NULL);
  
  label = gtk_label_new ((gchar *)_("\n\tPreferences have changed.\n"
                         "Save preferences or revert to saved ?"
                         "\n"));
  //text label
  gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox),
                     label);

  gtk_widget_show(label);

  //result from the dialog, after pushing button
  gint result;
  result = gtk_dialog_run (GTK_DIALOG (dialog));

  //if we pushed save preferences
  if (result == GTK_RESPONSE_YES)
    save_preferences_event(NULL, NULL);
  else
    if (result == GTK_RESPONSE_NO)
      //if we pushed revert preferences
      revert_to_save_event(NULL, NULL);
    else 
      //if we push cancel
      if (result == GTK_RESPONSE_CANCEL)
        {
          //what do you want to do ? :)
        }
  
  //destroys dialog
  gtk_widget_destroy (dialog);
}

//creates the preferences save button hbox
GtkWidget *create_save_buttons_hbox()
{
  //our hbox
  GtkWidget *hbox;
  hbox = gtk_hbox_new(FALSE, 0);

  GtkTooltips *tooltip;
  tooltip = gtk_tooltips_new();

  /* save button */
  save_button = (GtkWidget *)create_cool_button(GTK_STOCK_SAVE,
                                                (gchar *)_("Save"), FALSE);
  gtk_widget_set_sensitive(GTK_WIDGET(save_button), FALSE);
  g_signal_connect (G_OBJECT (save_button), "clicked",
                    G_CALLBACK (save_preferences_event), NULL);
  gtk_box_pack_start (GTK_BOX (hbox), save_button, TRUE, FALSE, 5);
  gtk_tooltips_set_tip(tooltip, save_button,(gchar *)_("save preferences"),"");

  /* revert to save button */
  revert_to_save_button = 
    (GtkWidget *)create_cool_button(GTK_STOCK_REVERT_TO_SAVED,
                                    (gchar *)_("Revert to saved"), FALSE);
  gtk_widget_set_sensitive(GTK_WIDGET(revert_to_save_button), FALSE);
  g_signal_connect (G_OBJECT (revert_to_save_button), "clicked",
                    G_CALLBACK (revert_to_save_event), NULL);
  gtk_box_pack_start (GTK_BOX (hbox), revert_to_save_button, TRUE, FALSE, 5);
  gtk_tooltips_set_tip(tooltip, 
                       revert_to_save_button,(gchar *)_("revert to saved preferences"),"");

  return hbox;
}

//removes unavailable players from the combo
void combo_remove_unavailable_players()
{
  //if we dont have AUDACIOUS
#ifdef NO_AUDACIOUS
  gtk_combo_box_remove_text(GTK_COMBO_BOX(player_combo_box),
                            PLAYER_AUDACIOUS-1);
  player_pref_list =
    g_list_remove(player_pref_list, (gint *)PLAYER_AUDACIOUS);
#endif
}

//event when changing the combo box player
void player_combo_box_event(GtkComboBox *widget,
                            gpointer data)
{
  gint sel_pl;
  gint selected_item;
  selected_item = gtk_combo_box_get_active(widget);
  sel_pl =
    (gint)g_list_nth_data(player_pref_list,
                         selected_item);

  selected_player = sel_pl;

  //disconnect from player
  disconnect_button_event (NULL,
                           NULL);
  
  gtk_widget_show(player_box);
  gtk_widget_show(queue_files_button);

  update_save_buttons();
}

//creates the choose player combo box
GtkWidget *create_choose_player_combo()
{
  GtkWidget *vbox;
  //label
  GtkWidget *label;

  vbox = gtk_vbox_new(FALSE,0);
  
  //fake horizontal
  GtkWidget *horiz_fake;
  horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start (GTK_BOX (vbox), 
                      horiz_fake, FALSE, FALSE, 5);
  
  //label for choose player
  label = gtk_label_new((gchar *)_("Choose a player :"));
  gtk_box_pack_start (GTK_BOX (horiz_fake), 
                      label, FALSE, FALSE, 5);
  
  //choose player combo box
  player_combo_box = gtk_combo_box_new_text();
  g_signal_connect (G_OBJECT (player_combo_box),
                    "changed",
                    G_CALLBACK (player_combo_box_event), NULL);
  
  //
  gtk_combo_box_insert_text(GTK_COMBO_BOX(player_combo_box),
                            PLAYER_AUDACIOUS,(gchar *)_("Audacious"));
  player_pref_list =
    g_list_append(player_pref_list, (gint *)PLAYER_AUDACIOUS);
  //
  gtk_combo_box_insert_text(GTK_COMBO_BOX(player_combo_box),
                            PLAYER_SNACKAMP,(gchar *)_("SnackAmp"));
  player_pref_list =
    g_list_append(player_pref_list, (gint *)PLAYER_SNACKAMP);
  //
  gtk_box_pack_start (GTK_BOX (horiz_fake), 
                      player_combo_box, FALSE, FALSE, 0);
  
  return vbox;
}

//creates the player preferences page
GtkWidget *create_pref_player_page()
{
  //our player preferences vertical box
  GtkWidget *player_hbox;
  player_hbox = gtk_hbox_new(FALSE, 0);;

  //vertical box inside the scrolled window
  GtkWidget *player_inside_hbox;
  player_inside_hbox = gtk_hbox_new(FALSE, 0);;
  
  //scrolled window
  GtkWidget *scrolled_window;
  scrolled_window = create_scrolled_window();
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), 
                                        GTK_WIDGET(player_inside_hbox));
  gtk_box_pack_start (GTK_BOX (player_hbox), 
                      scrolled_window, TRUE, TRUE, 0);
  
  //vertical box inside the horizontal box from the scrolled window
  GtkWidget *general_inside_vbox;
  general_inside_vbox = gtk_vbox_new(FALSE, 0);;
  gtk_box_pack_start (GTK_BOX (player_inside_hbox),
                      general_inside_vbox, TRUE, TRUE, 8);
  
  //choose player combo box
  GtkWidget *choose_player_combo_box;
  choose_player_combo_box = 
    (GtkWidget *)create_choose_player_combo();
  gtk_box_pack_start (GTK_BOX (general_inside_vbox), 
                      choose_player_combo_box,
                      FALSE, FALSE, 5);
  
  return player_hbox;
}

//update the save buttons when output entry event
gboolean output_entry_event (GtkWidget *widget,
                             GdkEventKey *event,
                             gpointer   user_data)  
{
  //we check if the output format is correct
  char *data = (char *)
    gtk_entry_get_text(GTK_ENTRY(output_entry));
  gint error = SPLT_OUTPUT_FORMAT_OK;
  mp3splt_set_oformat(the_state, data, &error);
  print_status_bar_confirmation(error);
  
  update_save_buttons();
  return FALSE;
}

//creates the output preferences page
GtkWidget *create_pref_output_page()
{
  //our output preferences vertical box
  GtkWidget *output_hbox;
  output_hbox = gtk_hbox_new(FALSE, 0);;
  
  //vertical box inside the scrolled window
  GtkWidget *output_inside_hbox;
  output_inside_hbox = gtk_hbox_new(FALSE, 0);;
  
  //scrolled window
  GtkWidget *scrolled_window;
  scrolled_window = create_scrolled_window();
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), 
                                        GTK_WIDGET(output_inside_hbox));
  gtk_box_pack_start (GTK_BOX (output_hbox), 
                      scrolled_window, TRUE, TRUE, 0);
  
  //vertical box inside the horizontal box from the scrolled window
  GtkWidget *output_inside_vbox;
  output_inside_vbox = gtk_vbox_new(FALSE, 0);;
  gtk_box_pack_start (GTK_BOX (output_inside_hbox),
                      output_inside_vbox, TRUE, TRUE, 8);
  
  //horizontal fake widget box
  GtkWidget *horiz_fake;
  horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start (GTK_BOX (output_inside_vbox), 
                      horiz_fake, FALSE, FALSE, 5);
  
  //output label
  GtkWidget *label;
  label = gtk_label_new((gchar *)
                        _("Output filename format for cddb files and cue files"
                          " (includes freedb search) :\n"
                          "    @a - artist name\n"
                          "    @p - performer of each song (does not"
                          " always exist)\n"
                          "    @b - album title\n"
                          "    @t - song title\n"
                          "    @n - track number"));
  gtk_box_pack_start (GTK_BOX (horiz_fake), 
                      label, FALSE, FALSE, 0);
  
  //radio button for choosing default or custom output options
  radio_output = 
    gtk_radio_button_new_with_label(NULL, 
                                    (gchar *)_("Default format : \n"
                                               "   @a - @n - @t where @a"
                                               " is the performer if found"));
  g_signal_connect (GTK_TOGGLE_BUTTON (radio_output),
                    "toggled",
                    G_CALLBACK (output_radio_box_event),
                    NULL);
  //add the radio button to the box
  gtk_box_pack_start (GTK_BOX (output_inside_vbox), 
                      radio_output, FALSE, FALSE, 0);
  radio_output = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON (radio_output), (gchar *)_("Custom format"));
  //add the radio button to the box
  gtk_box_pack_start (GTK_BOX (output_inside_vbox), 
                      radio_output, FALSE, FALSE, 0);
  
  //fake box
  horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start (GTK_BOX (output_inside_vbox), 
                      horiz_fake, FALSE, FALSE, 5);
  
  //output entry
  output_entry = gtk_entry_new();
  gtk_entry_set_editable (GTK_ENTRY (output_entry), TRUE);
  g_signal_connect (G_OBJECT (output_entry), "key_release_event",
                    G_CALLBACK (output_entry_event), NULL);
  gtk_entry_set_max_length (GTK_ENTRY(output_entry),244);
  gtk_box_pack_start (GTK_BOX (horiz_fake),
                      output_entry, TRUE, TRUE, 0);
  
  return output_hbox;
}

//creates the preferences 
GtkWidget *create_choose_preferences()
{
  //our preferences vbox
  GtkWidget *pref_vbox;
  pref_vbox = gtk_vbox_new(FALSE, 0);

  /* tabbed notebook */
  GtkWidget *notebook;
  //label for the notebook
  GtkWidget *notebook_label;
  notebook = gtk_notebook_new();
  gtk_box_pack_start (GTK_BOX (pref_vbox), notebook, TRUE, TRUE, 5);
  
  gtk_notebook_popup_enable(GTK_NOTEBOOK(notebook));
  gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), TRUE);
  gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), FALSE);
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
  
  /* general preferences page */
  GtkWidget *general_prefs;
  general_prefs = (GtkWidget *)create_pref_general_page();
  notebook_label = gtk_label_new((gchar *)_("General"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), 
                           general_prefs,
                           (GtkWidget *)notebook_label);

  /* split preferences */
  GtkWidget *splitpoints_prefs;
  splitpoints_prefs = (GtkWidget *)create_pref_splitpoints_page();
  notebook_label = gtk_label_new((gchar *)_("Split"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), 
                           splitpoints_prefs,
                           (GtkWidget *)notebook_label);
  
  /* output preferences */
  GtkWidget *output_prefs;
  output_prefs = (GtkWidget *)create_pref_output_page();
  notebook_label = gtk_label_new((gchar *)_("Output"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), 
                           output_prefs,
                           (GtkWidget *)notebook_label);
  
  //if we have a player
  /* player preferences */
  GtkWidget *player_prefs;
  player_prefs = (GtkWidget *)create_pref_player_page();
  notebook_label = gtk_label_new((gchar *)_("Player"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), 
                           player_prefs,
                           (GtkWidget *)notebook_label);

  /* buttons horizontal box*/
  GtkWidget *buttons_hbox;
  buttons_hbox = create_save_buttons_hbox();
  gtk_box_pack_start (GTK_BOX (pref_vbox), buttons_hbox, FALSE, FALSE, 5);
  
  return pref_vbox;
}
