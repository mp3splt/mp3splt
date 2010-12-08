/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2010 Alexandru Munteanu
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
 * The preferences tab
 *
 * this file contains the code for the preferences tab where 
 * the preferences can be chosen.
 ********************************************************/

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <string.h>

#include <libmp3splt/mp3splt.h>

#include "preferences_tab.h"
#include "player.h"
#include "util.h"
#include "player_tab.h"
#include "utilities.h"
#include "main_win.h"
#include "preferences_manager.h"
#include "widgets_helper.h"
#include "combo_helper.h"
#include "radio_helper.h"

/*! The name of the output directory.
  
  Can be accessed by void outputdirectory_set() and 
  outputdirectory_get().
 */
GString *outputdirname = NULL;

//! The textbox that shows the name of the output directory
GtkWidget *directory_entry = NULL;

//!output for the cddb,cue and freedb file output
GtkWidget *output_entry = NULL;
GtkWidget *output_label = NULL;

//!choose the player box
GtkWidget *player_combo_box = NULL;

//!list where we stock the preferences combo box content
GList *player_pref_list = NULL;
GList *text_options_list = NULL;
//selected player
gint selected_player = PLAYER_GSTREAMER;

//!the language radio button
GtkWidget *radio_button = NULL;

//!radio button for choosing default or custom output options
GtkWidget *radio_output = NULL;

//!radio button for tags options
GtkWidget *tags_radio = NULL;
GtkWidget *tags_version_radio = NULL;

//split options
//!frame mode option
GtkWidget *frame_mode = NULL;
//!auto-adjust option
GtkWidget *adjust_mode = NULL;

GtkWidget *create_dirs_from_output_files = NULL;

/*!defgroup modeparameters adjust mode parameters
@{
*/
GtkWidget *spinner_adjust_gap = NULL;
GtkWidget *gap_label = NULL;
GtkWidget *spinner_adjust_offset = NULL;
GtkWidget *offset_label = NULL;
GtkWidget *spinner_adjust_threshold = NULL;
GtkWidget *threshold_label = NULL;
//@}

/*!defgroup options for tags from filename
@{
*/
GtkWidget *replace_underscore_by_space_check_box = NULL;
GtkComboBox *artist_text_properties_combo = NULL;
GtkComboBox *album_text_properties_combo = NULL;
GtkComboBox *title_text_properties_combo = NULL;
GtkComboBox *comment_text_properties_combo = NULL;
GtkWidget *comment_tag_entry = NULL;
GtkWidget *regex_entry = NULL;
GtkWidget *test_regex_entry = NULL;
GtkWidget *sample_result_label = NULL;

GtkWidget *extract_tags_box = NULL;
//@}

extern GtkWidget *player_box;
extern GtkWidget *queue_files_button;
extern splt_state *the_state;
extern gint selected_split_mode;
extern gint split_file_mode;
extern GtkWidget *spinner_time;
extern GtkWidget *spinner_equal_tracks;

static GtkWidget *create_extract_tags_from_filename_options_box();

/*!Returns the selected language

must be free() the result after using it.
*/
GString *get_checked_language()
{
  GSList *radio_button_list;
  radio_button_list = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio_button));
  GtkWidget *our_button;

  //0 = german, 1 = french, 2 = english
  our_button = (GtkWidget *)g_slist_nth_data(radio_button_list, 0);
  if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(our_button)))
  {
    return g_string_new("de");
  }
  else 
  {
    our_button = (GtkWidget *)g_slist_nth_data(radio_button_list, 1);
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(our_button)))
    {
      return g_string_new("fr");
    }
  }

  return g_string_new("en");
}

//!returns the checked output radio box
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
    test = (GtkToggleButton *)g_slist_nth_data(radio_button_list,i);
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(test)))
    {
      selected = i;
    }
  }

  return selected;
}

//!returns the checked tags radio box
gint get_checked_tags_version_radio_box()
{
  //get the radio buttons
  GSList *radio_button_list;
  radio_button_list = gtk_radio_button_get_group(GTK_RADIO_BUTTON(tags_version_radio));

  //we check which bubble is checked
  GtkToggleButton *test;
  gint i, selected = 0;
  //O = The same version as the original file
  //1 = ID3v1
  //2 = ID3v2
  //3 = ID3v1 & ID3v2
  for(i = 0; i<4;i++)
  {
    test = (GtkToggleButton *)g_slist_nth_data(radio_button_list,i);
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(test)))
    {
      selected = i;
    }
  }
  
  return selected;
}

//! Set the name of the output directory
void outputdirectory_set(gchar *dirname)
{
  if(dirname!=NULL)
    {
      // Free the old string before allocating memory for the new one
      if(outputdirname!=NULL)g_string_free(outputdirname,TRUE);
      outputdirname=g_string_new(dirname);

      // Update the text in the gui field displaying the output 
      // directory - if this field is already there and thus can 
      // be updated.
      if(directory_entry!=NULL)
	gtk_entry_set_text(GTK_ENTRY(directory_entry), dirname);
    }
}

/*! Get the name of the output directory

\return 
 - The name of the output directory, if a directory is set.
 - NULL, otherwise.
*/
gchar* outputdirectory_get()
{
  if(outputdirname!=NULL)
    return(outputdirname->str);
  else
    return NULL;
}

//!save preferences event
void save_preferences(GtkWidget *widget, gpointer data)
{
  gchar *filename = get_preferences_filename();

  GKeyFile *my_key_file = g_key_file_new();
  g_key_file_load_from_file(my_key_file, filename, G_KEY_FILE_KEEP_COMMENTS, NULL);

  //save_path
  g_key_file_set_string(my_key_file, "split", "save_path",
			outputdirectory_get());
  //default_player
  g_key_file_set_integer(my_key_file, "player", "default_player", selected_player);

#ifdef __WIN32__
  //language
  GString *selected_lang;
  selected_lang = (GString *)get_checked_language();
  g_key_file_set_string(my_key_file, "general", "language", selected_lang->str);
  g_string_free(selected_lang, TRUE);
  selected_lang = NULL;
#endif

  //frame mode
  g_key_file_set_boolean(my_key_file, "split", "frame_mode",
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(frame_mode)));

  //adjust mode
  g_key_file_set_boolean(my_key_file, "split", "adjust_mode",
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(adjust_mode)));

  //adjust threshold
  g_key_file_set_integer(my_key_file, "split", "adjust_threshold",
      gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinner_adjust_threshold)) * 100);
  //adjust offset
  g_key_file_set_integer(my_key_file, "split", "adjust_offset",
      gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinner_adjust_offset)) * 100);
  //adjust gap
  g_key_file_set_integer(my_key_file, "split", "adjust_gap",
      gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinner_adjust_gap)));

  //output format
  g_key_file_set_string(my_key_file, "output", "output_format",
			outputdirectory_get());
  //default output format
  g_key_file_set_boolean(my_key_file, "output", "default_output_format",
      get_checked_output_radio_box());
  g_key_file_set_boolean(my_key_file, "output", "create_dirs_if_needed",
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(create_dirs_from_output_files)));

  //tags
  g_key_file_set_integer(my_key_file, "split", "tags", rh_get_active_value(tags_radio));

  //replace underscores by space
  g_key_file_set_boolean(my_key_file, "split", "replace_underscore_by_space",
  gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(replace_underscore_by_space_check_box)));

  //artist text properties
  g_key_file_set_integer(my_key_file, "split", "artist_text_properties",
      ch_get_active_value(artist_text_properties_combo));
  //album text properties
  g_key_file_set_integer(my_key_file, "split", "album_text_properties",
      ch_get_active_value(album_text_properties_combo));
  //title text properties
  g_key_file_set_integer(my_key_file, "split", "title_text_properties",
      ch_get_active_value(title_text_properties_combo));
  //comment text properties
  g_key_file_set_integer(my_key_file, "split", "comment_text_properties",
      ch_get_active_value(comment_text_properties_combo));

  //default comment tag
  g_key_file_set_string(my_key_file, "split", "default_comment_tag",
      gtk_entry_get_text(GTK_ENTRY(comment_tag_entry)));

  //regex to parse filename into tags
  g_key_file_set_string(my_key_file, "split", "tags_from_filename_regex",
      gtk_entry_get_text(GTK_ENTRY(regex_entry)));

  //tags version
  g_key_file_set_integer(my_key_file, "split", "tags_version",
      get_checked_tags_version_radio_box());

  //type of split: split mode
  g_key_file_set_integer(my_key_file, "split", "split_mode",
      selected_split_mode);
  //time value
  g_key_file_set_integer(my_key_file, "split", "split_mode_time_value",
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinner_time)));
  //type of split: file mode
  g_key_file_set_integer(my_key_file, "split", "file_mode",
      split_file_mode);
  //equal time tracks value
  g_key_file_set_integer(my_key_file, "split", "split_mode_equal_time_tracks",
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinner_equal_tracks)));

  //our data
  gchar *key_data = g_key_file_to_data(my_key_file, NULL, NULL);

  //we write to the preference file
  FILE *preferences_file;
  preferences_file = (FILE *)g_fopen(filename,"w");
  g_fprintf(preferences_file,"%s", key_data);
  fclose(preferences_file);
  preferences_file = NULL;

  //we free memory
  g_free(key_data);
  g_key_file_free(my_key_file);

  if (filename)
  {
    g_free(filename);
    filename = NULL;
  }
}

//!creates a scrolled window
GtkWidget *create_scrolled_window()
{
  GtkWidget *scrolled_window;
  scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_NONE);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_AUTOMATIC);
  return scrolled_window;
}

//!cddb and cue output mode radio box event
void output_radio_box_event(GtkToggleButton *radio_b, gpointer data)
{
  GtkWidget *output_label = (GtkWidget *)data;

  gint selected = get_checked_output_radio_box();

  //custom output mode
  if (selected == 0)
  {
    gtk_widget_set_sensitive(GTK_WIDGET(output_entry), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(output_label), TRUE);
    mp3splt_set_int_option(the_state, SPLT_OPT_OUTPUT_FILENAMES,
        SPLT_OUTPUT_FORMAT);
  }
  else
  {
    gtk_widget_set_sensitive(GTK_WIDGET(output_entry), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(output_label), FALSE);
    mp3splt_set_int_option(the_state, SPLT_OPT_OUTPUT_FILENAMES,
        SPLT_OUTPUT_DEFAULT);
  }

  save_preferences(NULL, NULL);
}

//! Create the "select language" box
GtkWidget *create_language_box()
{
  GtkWidget *radio_vbox = gtk_vbox_new (FALSE, 0);

  radio_button = gtk_radio_button_new_with_label(NULL, "English");
  g_signal_connect(GTK_TOGGLE_BUTTON(radio_button), "toggled",
      G_CALLBACK(save_preferences), NULL);
  gtk_box_pack_start(GTK_BOX(radio_vbox), radio_button, TRUE, TRUE, 0);

  radio_button = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(radio_button), "Français");
  g_signal_connect(GTK_TOGGLE_BUTTON(radio_button), "toggled",
      G_CALLBACK(save_preferences), NULL);
  gtk_box_pack_start(GTK_BOX(radio_vbox), radio_button, TRUE, TRUE, 0);

  radio_button = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(radio_button), "Deutsch");
  g_signal_connect(GTK_TOGGLE_BUTTON (radio_button), "toggled",
      G_CALLBACK(save_preferences), NULL);
  gtk_box_pack_start(GTK_BOX(radio_vbox), radio_button, TRUE, TRUE, 0);

  return wh_set_title_and_get_vbox(radio_vbox,
      _("<b>Choose language (requires restart)</b>"));
}

//! Creates the language preferences page
GtkWidget *create_pref_language_page()
{
  GtkWidget *language_hbox = gtk_hbox_new(FALSE, 0);;
 
  //vertical box inside the scrolled window
  GtkWidget *language_inside_hbox = gtk_hbox_new(FALSE, 0);;

  //scrolled window
  GtkWidget *scrolled_window;
  scrolled_window = create_scrolled_window();
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), 
                                        GTK_WIDGET(language_inside_hbox));
  gtk_box_pack_start(GTK_BOX(language_hbox), scrolled_window, TRUE, TRUE, 0);
 
  //vertical box inside the horizontal box from the scrolled window
  GtkWidget *vbox = gtk_vbox_new(FALSE, 0);;
  gtk_box_pack_start(GTK_BOX(language_inside_hbox), vbox, TRUE, TRUE, 10);

  GtkWidget *lang_box = create_language_box();
  gtk_box_pack_start(GTK_BOX(vbox), lang_box, FALSE, FALSE, 10);
  
  return language_hbox;
}

//! Events for browse dir button
void browse_dir_button_event(GtkWidget *widget, gpointer data)
{
  // file chooser
  GtkWidget *dir_chooser;

  //creates and shows the dialog
  dir_chooser = gtk_file_chooser_dialog_new(_("Choose split directory"),
      NULL,
      GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
      GTK_STOCK_CANCEL,
      GTK_RESPONSE_CANCEL,
      GTK_STOCK_OPEN,
      GTK_RESPONSE_ACCEPT,
      NULL);

  if (gtk_dialog_run(GTK_DIALOG(dir_chooser)) == GTK_RESPONSE_ACCEPT)
  {
    gchar *filename =
      gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dir_chooser));

    outputdirectory_set(filename);

    g_free (filename);
    filename = NULL;

    save_preferences(NULL, NULL);
  }

  //destroys dialog
  gtk_widget_destroy(dir_chooser);
}

//!disables adjust parameters
void disable_adjust_spinners()
{
  gtk_widget_set_sensitive(spinner_adjust_threshold, FALSE);
  gtk_widget_set_sensitive(spinner_adjust_offset, FALSE);
  gtk_widget_set_sensitive(spinner_adjust_gap, FALSE);
  gtk_widget_set_sensitive(threshold_label, FALSE);
  gtk_widget_set_sensitive(offset_label, FALSE);
  gtk_widget_set_sensitive(gap_label, FALSE);
}

//!enables adjust parameters
void enable_adjust_spinners()
{
  gtk_widget_set_sensitive(spinner_adjust_threshold, TRUE);
  gtk_widget_set_sensitive(spinner_adjust_offset, TRUE);
  gtk_widget_set_sensitive(spinner_adjust_gap, TRUE);
  gtk_widget_set_sensitive(threshold_label, TRUE);
  gtk_widget_set_sensitive(offset_label, TRUE);
  gtk_widget_set_sensitive(gap_label, TRUE);
}

//!adjust event
void adjust_event(GtkToggleButton *adjust_mode, gpointer user_data)
{
  //if it is toggled
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(adjust_mode)))
  {
    if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(frame_mode)))
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(frame_mode),TRUE);
    }
    enable_adjust_spinners();
  }
  else
  {
    //disable spinners
    disable_adjust_spinners();
  }

  save_preferences(NULL, NULL);
}

//!frame mode event
void frame_event(GtkToggleButton *frame_mode, gpointer user_data)
{
  if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(frame_mode)))
  {
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(adjust_mode)))
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(adjust_mode),FALSE);
    }
  }

  save_preferences(NULL, NULL);
}

//!action for the set default prefs button
void set_default_prefs_event(GtkWidget *widget, gpointer data)
{
  //set frame mode inactive
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(frame_mode), FALSE);
  //set adjust mode inactive
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(adjust_mode), FALSE);
  //set adjust mode preferences
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_adjust_threshold),
                            SPLT_DEFAULT_PARAM_THRESHOLD);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_adjust_offset),
                            SPLT_DEFAULT_PARAM_OFFSET);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_adjust_gap),
                            SPLT_DEFAULT_PARAM_GAP);

  save_preferences(NULL, NULL);
}

//!events for the "set current song directory"
void song_dir_button_event(GtkWidget *widget, gpointer data)
{
    outputdirectory_set("");
    save_preferences(NULL, NULL);
}

//!Creates the box the output directory can be choosen in
GtkWidget *create_directory_box()
{
  GtkWidget *dir_hbox = gtk_hbox_new(FALSE, 0);
  
  //directory entry
  directory_entry = gtk_entry_new();
  gtk_entry_set_editable(GTK_ENTRY(directory_entry), FALSE);
  gtk_box_pack_start(GTK_BOX(dir_hbox), directory_entry, TRUE, TRUE, 0);
  // Put the right text into the text box containing the output directory
  // name if this name was provided on command line
  if(outputdirectory_get()!=NULL)
    gtk_entry_set_text(GTK_ENTRY(directory_entry), outputdirectory_get());
  
  //browse dir button
  GtkWidget *browse_dir_button = (GtkWidget *)
    create_cool_button(GTK_STOCK_DIRECTORY,_("Br_owse dir"), FALSE);
  g_signal_connect(G_OBJECT(browse_dir_button), "clicked",
      G_CALLBACK(browse_dir_button_event), NULL);
  gtk_box_pack_start(GTK_BOX(dir_hbox), browse_dir_button, FALSE, FALSE, 8);
  
  //to set the directory for split files to the current song
  //directory
  GtkWidget *song_dir_button = (GtkWidget *)
    create_cool_button(GTK_STOCK_CLEAR, _("_Song dir"), FALSE);
  g_signal_connect(G_OBJECT(song_dir_button), "clicked",
      G_CALLBACK(song_dir_button_event), NULL);
  gtk_box_pack_start(GTK_BOX(dir_hbox), song_dir_button, FALSE, FALSE, 0);

  return wh_set_title_and_get_vbox(dir_hbox, _("<b>Directory for split files</b>"));
}

//! Creates the box for split mode selection
GtkWidget *create_split_options_box()
{
  GtkWidget *vbox = gtk_vbox_new(FALSE, 0);

  create_dirs_from_output_files =
    gtk_check_button_new_with_mnemonic(_("_Create directories from filenames "));
  gtk_box_pack_start(GTK_BOX(vbox), create_dirs_from_output_files, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(create_dirs_from_output_files), "toggled",
      G_CALLBACK(save_preferences), NULL);

  //frame mode option
  frame_mode = gtk_check_button_new_with_mnemonic(_("F_rame mode (useful"
        " for mp3 VBR) (mp3 only)"));
  gtk_box_pack_start(GTK_BOX(vbox), frame_mode, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(frame_mode), "toggled",
      G_CALLBACK(frame_event), NULL);

  //auto adjust option
  adjust_mode = gtk_check_button_new_with_mnemonic(_("_Auto-adjust mode (uses"
        " silence detection to auto-adjust splitpoints)"));
  gtk_box_pack_start(GTK_BOX(vbox), adjust_mode, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(adjust_mode), "toggled",
      G_CALLBACK(adjust_event), NULL);
  
  //parameters for the adjust option
  GtkWidget *horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start(GTK_BOX(vbox), horiz_fake, FALSE, FALSE, 0);
  
  GtkWidget *param_vbox = gtk_vbox_new(FALSE,0);
  gtk_box_pack_start(GTK_BOX(horiz_fake), param_vbox, FALSE, FALSE, 25);
  
  //threshold level
  horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start(GTK_BOX(param_vbox), horiz_fake, FALSE, FALSE, 0);
  
  threshold_label = gtk_label_new(_("Threshold level (dB):"));
  gtk_box_pack_start(GTK_BOX(horiz_fake), threshold_label, FALSE, FALSE, 0);
  
  GtkAdjustment *adj = (GtkAdjustment *) gtk_adjustment_new(0.0, -96.0, 0.0,
      0.5, 10.0, 0.0);
  spinner_adjust_threshold = gtk_spin_button_new (adj, 0.5, 2);
  g_signal_connect(G_OBJECT(spinner_adjust_threshold), "value_changed",
      G_CALLBACK(save_preferences), NULL);
  gtk_box_pack_start(GTK_BOX(horiz_fake), spinner_adjust_threshold,
                      FALSE, FALSE, 6);
  
  //offset level
  horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start(GTK_BOX(param_vbox), horiz_fake, FALSE, FALSE, 0);
  
  offset_label = gtk_label_new(_("Cutpoint offset (0 is the begin of silence "
        "and 1 the end):"));
  gtk_box_pack_start(GTK_BOX(horiz_fake), offset_label, FALSE, FALSE, 0);
  
  //adjustement for the offset spinner
  adj = (GtkAdjustment *)gtk_adjustment_new(0.0, -2, 2, 0.05, 10.0, 0.0);
  //the offset spinner
  spinner_adjust_offset = gtk_spin_button_new (adj, 0.05, 2);
  g_signal_connect(G_OBJECT(spinner_adjust_offset), "value_changed",
      G_CALLBACK(save_preferences), NULL);
  gtk_box_pack_start(GTK_BOX(horiz_fake), spinner_adjust_offset,
      FALSE, FALSE, 6);
  
  //gap level (seconds)
  horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start(GTK_BOX(param_vbox), horiz_fake, FALSE, FALSE, 0);
  
  gap_label = gtk_label_new(_("Gap level (seconds around splitpoint to "
        "search for silence):"));
  gtk_box_pack_start(GTK_BOX(horiz_fake), gap_label, FALSE, FALSE, 0);
  
  adj = (GtkAdjustment *) gtk_adjustment_new(0.0, 0, 2000, 1.0, 10.0, 0.0);
  spinner_adjust_gap = gtk_spin_button_new (adj, 1, 0);
  g_signal_connect(G_OBJECT(spinner_adjust_gap), "value_changed",
      G_CALLBACK(save_preferences), NULL);
  gtk_box_pack_start(GTK_BOX(horiz_fake), spinner_adjust_gap, FALSE, FALSE, 6);
  
  disable_adjust_spinners();
 
  //set default preferences button
  //horizontal box fake for the gap level
  horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start(GTK_BOX(vbox), horiz_fake, FALSE, FALSE, 0);
  
  GtkWidget *set_default_prefs_button =
    (GtkWidget *)create_cool_button(GTK_STOCK_PREFERENCES,
        _("Set _default split" " options"),FALSE); 
  g_signal_connect(G_OBJECT(set_default_prefs_button), "clicked",
      G_CALLBACK(set_default_prefs_event), NULL);
  gtk_box_pack_start (GTK_BOX (horiz_fake), set_default_prefs_button,
                      FALSE, FALSE, 5);

  return wh_set_title_and_get_vbox(vbox, _("<b>Split options</b>"));
}

//!creates the splitpoints preferences page
GtkWidget *create_pref_splitpoints_page()
{
  GtkWidget *general_hbox = gtk_hbox_new(FALSE,0);
  GtkWidget *inside_hbox = gtk_hbox_new(FALSE,0);
  
  GtkWidget *scrolled_window = create_scrolled_window();
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), 
                                        GTK_WIDGET(inside_hbox));
  gtk_box_pack_start(GTK_BOX(general_hbox), scrolled_window, TRUE, TRUE, 0);
 
  //vertical box inside the horizontal box from the scrolled window
  GtkWidget *inside_vbox = gtk_vbox_new(FALSE, 0);;
  gtk_box_pack_start(GTK_BOX(inside_hbox), inside_vbox, TRUE, TRUE, 10);
 
  GtkWidget *dir_box = create_directory_box();
  gtk_box_pack_start(GTK_BOX(inside_vbox), dir_box, FALSE, FALSE, 10);

  GtkWidget *split_options_box = create_split_options_box();
  gtk_box_pack_start(GTK_BOX(inside_vbox), split_options_box, FALSE, FALSE, 2);
 
  return general_hbox;
}

//!removes unavailable players from the combo
void combo_remove_unavailable_players()
{
  //if we dont have GSTREAMER 
#ifdef NO_GSTREAMER
  gtk_combo_box_remove_text(GTK_COMBO_BOX(player_combo_box),
                            PLAYER_GSTREAMER-1);
  player_pref_list =
    g_list_remove(player_pref_list, GINT_TO_POINTER(PLAYER_GSTREAMER));
#endif

  //if we dont have AUDACIOUS
#ifdef NO_AUDACIOUS
  gtk_combo_box_remove_text(GTK_COMBO_BOX(player_combo_box),
                            PLAYER_AUDACIOUS-1);
  player_pref_list =
    g_list_remove(player_pref_list, GINT_TO_POINTER(PLAYER_AUDACIOUS));
#endif
}

//!event when changing the combo box player
void player_combo_box_event(GtkComboBox *widget, gpointer data)
{
  //disconnect from player
  disconnect_button_event(NULL, NULL);

  gint selected_item;
  selected_item = gtk_combo_box_get_active(widget);
  gint sel_pl = GPOINTER_TO_INT(g_list_nth_data(player_pref_list, selected_item));

  selected_player = sel_pl;

  if (selected_player == PLAYER_GSTREAMER)
  {
    hide_connect_button();
  }
  else
  {
    show_connect_button();
  }
  
  gtk_widget_show(player_box);
  gtk_widget_show(queue_files_button);

  save_preferences(NULL, NULL);
}

//!Create the box the player backend can be selected with
GtkWidget *create_player_options_box()
{
  GtkWidget *horiz_fake = gtk_hbox_new(FALSE,0);

  GtkWidget *label = gtk_label_new(_("Choose a player:"));
  gtk_box_pack_start(GTK_BOX(horiz_fake), label, FALSE, FALSE, 0);

  player_combo_box = gtk_combo_box_new_text();
  g_signal_connect(G_OBJECT(player_combo_box), "changed",
      G_CALLBACK(player_combo_box_event), NULL);
  
  gtk_combo_box_insert_text(GTK_COMBO_BOX(player_combo_box),
                            PLAYER_AUDACIOUS,"Audacious");
  player_pref_list =
    g_list_append(player_pref_list, GINT_TO_POINTER(PLAYER_AUDACIOUS));

  gtk_combo_box_insert_text(GTK_COMBO_BOX(player_combo_box),
                            PLAYER_SNACKAMP,"SnackAmp");
  player_pref_list =
    g_list_append(player_pref_list, GINT_TO_POINTER(PLAYER_SNACKAMP));

  gtk_combo_box_insert_text(GTK_COMBO_BOX(player_combo_box),
                            PLAYER_GSTREAMER,"GStreamer");
  player_pref_list =
    g_list_append(player_pref_list, GINT_TO_POINTER(PLAYER_GSTREAMER));

  gtk_box_pack_start(GTK_BOX(horiz_fake), player_combo_box, FALSE, FALSE, 12);
 
  return wh_set_title_and_get_vbox(horiz_fake, _("<b>Player options</b>"));
}

//!creates the player preferences page
GtkWidget *create_pref_player_page()
{
  GtkWidget *player_hbox = gtk_hbox_new(FALSE, 0);;

  GtkWidget *inside_hbox = gtk_hbox_new(FALSE, 0);;
  
  GtkWidget *scrolled_window = create_scrolled_window();
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), 
                                        GTK_WIDGET(inside_hbox));
  gtk_box_pack_start(GTK_BOX(player_hbox), scrolled_window, TRUE, TRUE, 0);
  
  //vertical box inside the horizontal box from the scrolled window
  GtkWidget *vbox = gtk_vbox_new(FALSE, 0);;
  gtk_box_pack_start(GTK_BOX(inside_hbox), vbox, TRUE, TRUE, 10);
  
  //choose player combo box
  GtkWidget *player_options_box = create_player_options_box();
  gtk_box_pack_start(GTK_BOX(vbox), player_options_box, FALSE, FALSE, 10);
 
  return player_hbox;
}

//!update the save buttons on an output entry event
gboolean output_entry_event(GtkWidget *widget, GdkEventKey *event,
    gpointer user_data)
{
  //we check if the output format is correct
  const char *data = gtk_entry_get_text(GTK_ENTRY(output_entry));
  gint error = SPLT_OUTPUT_FORMAT_OK;
  mp3splt_set_oformat(the_state, data, &error);
  remove_status_message();
  print_status_bar_confirmation(error);

  save_preferences(NULL, NULL);
  
  return FALSE;
}

//!Create the box the output file name is displayed in
GtkWidget *create_output_filename_box()
{
  GtkWidget *vbox = gtk_vbox_new(FALSE, 0);

  //default/custom radio buttons
  radio_output = gtk_radio_button_new_with_label(NULL, _("Default format"));
  gtk_box_pack_start(GTK_BOX(vbox), radio_output, FALSE, FALSE, 0);

  radio_output = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(radio_output), _("Custom format"));
  gtk_box_pack_start(GTK_BOX(vbox), radio_output, FALSE, FALSE, 0);

  //output entry
  GtkWidget *horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start(GTK_BOX(vbox), horiz_fake, FALSE, FALSE, 5);

  output_entry = gtk_entry_new();
  gtk_entry_set_editable(GTK_ENTRY(output_entry), TRUE);
  g_signal_connect(G_OBJECT(output_entry), "key_release_event",
      G_CALLBACK(output_entry_event), NULL);
  gtk_entry_set_max_length(GTK_ENTRY(output_entry),244);
  gtk_box_pack_start(GTK_BOX(horiz_fake), output_entry, TRUE, TRUE, 0);

  //output label
  horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start(GTK_BOX(vbox), horiz_fake, FALSE, FALSE, 5);
  output_label = gtk_label_new(_("    @f - file name\n"
        "    @a - artist name\n"
        "    @p - performer of each song (does not"
        " always exist)\n"
        "    @b - album title\n"
        "    @t - song title\n"
        "    @n - track number"));
  gtk_box_pack_start(GTK_BOX(horiz_fake), output_label, FALSE, FALSE, 0);

  g_signal_connect(GTK_TOGGLE_BUTTON(radio_output),
      "toggled", G_CALLBACK(output_radio_box_event), output_label);

  return wh_set_title_and_get_vbox(vbox, _("<b>Output filename format</b>"));
}

//!creates the output preferences page
GtkWidget *create_pref_output_page()
{
  GtkWidget *output_hbox = gtk_hbox_new(FALSE, 0);;
  GtkWidget *output_inside_hbox = gtk_hbox_new(FALSE, 0);;
  
  GtkWidget *scrolled_window = create_scrolled_window();
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), 
                                        GTK_WIDGET(output_inside_hbox));
  gtk_box_pack_start(GTK_BOX(output_hbox), scrolled_window, TRUE, TRUE, 0);
 
  GtkWidget *vbox = gtk_vbox_new(FALSE, 0);;
  gtk_box_pack_start(GTK_BOX(output_inside_hbox), vbox, TRUE, TRUE, 10);

  GtkWidget *output_fname_box = create_output_filename_box();
  gtk_box_pack_start(GTK_BOX(vbox), output_fname_box, FALSE, FALSE, 10);

  return output_hbox;
}

void change_tags_options(GtkToggleButton *button, gpointer data)
{
  if (extract_tags_box != NULL)
  {
    if (rh_get_active_value(tags_radio) == TAGS_FROM_FILENAME)
    {
      gtk_widget_set_sensitive(extract_tags_box, SPLT_TRUE);
    }
    else
    {
      gtk_widget_set_sensitive(extract_tags_box, SPLT_FALSE);
    }
  }

  save_preferences(NULL, NULL);
}

//!Create the box for the Tags options
GtkWidget *create_tags_options_box()
{
  GtkWidget *vbox = gtk_vbox_new(FALSE, 0);

  tags_radio = rh_append_radio_to_vbox(tags_radio, _("Original file tags"),
      ORIGINAL_FILE_TAGS, change_tags_options, vbox);

  tags_radio = rh_append_radio_to_vbox(tags_radio, _("Default tags (cddb or cue tags)"),
      DEFAULT_TAGS, change_tags_options, vbox);

  tags_radio = rh_append_radio_to_vbox(tags_radio, _("No tags"),
      NO_TAGS, change_tags_options, vbox);

  tags_radio = rh_append_radio_to_vbox(tags_radio, _("Extract tags from filename"),
      TAGS_FROM_FILENAME, change_tags_options, vbox);

  extract_tags_box = create_extract_tags_from_filename_options_box();
  gtk_widget_set_sensitive(extract_tags_box, SPLT_FALSE);
  gtk_box_pack_start(GTK_BOX(vbox), extract_tags_box, FALSE, FALSE, 2);

  return wh_set_title_and_get_vbox(vbox, _("<b>Split files tags</b>"));
}

static GtkComboBox *create_text_preferences_combo()
{
  GtkComboBox *combo = ch_new_combo();

  ch_append_to_combo(combo, _("No change"), SPLT_NO_CONVERSION);
  ch_append_to_combo(combo, _("lowercase"), SPLT_TO_LOWERCASE);
  ch_append_to_combo(combo, _("UPPERCASE"), SPLT_TO_UPPERCASE);
  ch_append_to_combo(combo, _("First uppercase"), SPLT_TO_FIRST_UPPERCASE);
  ch_append_to_combo(combo, _("Word Uppercase"), SPLT_TO_WORD_FIRST_UPPERCASE);

  g_signal_connect(G_OBJECT(combo), "changed", G_CALLBACK(save_preferences), NULL);

  return combo;
}

static GtkWidget *create_extract_tags_from_filename_options_box()
{
  GtkWidget *table = wh_new_table();

  text_options_list =
    g_list_append(text_options_list, GINT_TO_POINTER(SPLT_NO_CONVERSION));
  text_options_list = 
    g_list_append(text_options_list, GINT_TO_POINTER(SPLT_TO_LOWERCASE));
  text_options_list = 
    g_list_append(text_options_list, GINT_TO_POINTER(SPLT_TO_UPPERCASE));
  text_options_list =
    g_list_append(text_options_list, GINT_TO_POINTER(SPLT_TO_FIRST_UPPERCASE));
  text_options_list =
    g_list_append(text_options_list, GINT_TO_POINTER(SPLT_TO_WORD_FIRST_UPPERCASE));

  replace_underscore_by_space_check_box =
    gtk_check_button_new_with_mnemonic(_("_Replace underscores by spaces"));
  g_signal_connect(G_OBJECT(replace_underscore_by_space_check_box), "toggled",
      G_CALLBACK(save_preferences), NULL);
 
  wh_add_in_table(table, replace_underscore_by_space_check_box);

  artist_text_properties_combo = create_text_preferences_combo();
  wh_add_in_table_with_label(table, 
      _("Artist text properties:"), GTK_WIDGET(artist_text_properties_combo));

  album_text_properties_combo = create_text_preferences_combo();
  wh_add_in_table_with_label(table,
      _("Album text properties:"), GTK_WIDGET(album_text_properties_combo));

  title_text_properties_combo = create_text_preferences_combo();
  wh_add_in_table_with_label(table,
      _("Title text properties:"), GTK_WIDGET(title_text_properties_combo));

  comment_text_properties_combo = create_text_preferences_combo();
  wh_add_in_table_with_label(table,
      _("Comment text properties:"), GTK_WIDGET(comment_text_properties_combo));

  //TODO: genre

  comment_tag_entry = wh_new_entry(save_preferences);
  wh_add_in_table_with_label_expand(table, _("Comment tag:"), comment_tag_entry);

  regex_entry = wh_new_entry(save_preferences);
  wh_add_in_table_with_label_expand(table, _("Regular expression:"), regex_entry);

  GtkWidget *regex_label = gtk_label_new(_(
        "Above enter PERL-like regular expression using named subgroups.\nFollowing names are recognized:\n"
        "    (?<artist>)    - artist name\n"
        "    (?<album>)    - album title\n"
        "    (?<title>)    - track title\n"
        "    (?<tracknum>) - current track number\n"
        "    (?<tracks>)   - total number of tracks\n"
        "    (?<year>)     - year of emission\n"
        "    (?<comment>)  - comment"));
  gtk_misc_set_alignment(GTK_MISC(regex_label), 0.0, 0.5);
  wh_add_in_table(table, wh_put_in_new_hbox_with_margin_level(regex_label, 2));

  GtkWidget *sample_test_hbox = gtk_hbox_new(FALSE, 0);
  test_regex_entry = wh_new_entry(save_preferences);
  gtk_box_pack_start(GTK_BOX(sample_test_hbox), test_regex_entry, TRUE, TRUE, 5);

  GtkWidget *test_regex_button = wh_new_button(_("Test"));
  gtk_box_pack_start(GTK_BOX(sample_test_hbox), test_regex_button, FALSE, FALSE, 5);

  wh_add_in_table_with_label_expand(table, _("Sample filename:"), sample_test_hbox);

  sample_result_label = gtk_label_new("");
  wh_add_in_table_with_label_expand(table, _("Sample result:"), sample_result_label);

  return wh_put_in_new_hbox_with_margin_level(GTK_WIDGET(table), 3);
}

//!Create the box we can select with if to create ID1 and/or ID2 Tags
GtkWidget *create_tags_version_box()
{
  GtkWidget *vbox = gtk_vbox_new(FALSE, 0);

  tags_version_radio = 
    gtk_radio_button_new_with_label(NULL, _("ID3v1 & ID3v2 tags"));
  gtk_box_pack_start(GTK_BOX(vbox), tags_version_radio, FALSE, FALSE, 0);
  g_signal_connect(GTK_TOGGLE_BUTTON(tags_version_radio), "toggled", 
      G_CALLBACK(save_preferences), NULL);

  tags_version_radio = 
    gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(tags_version_radio), _("ID3v2 tags"));
  gtk_box_pack_start(GTK_BOX(vbox), tags_version_radio, FALSE, FALSE, 0);
  g_signal_connect(GTK_TOGGLE_BUTTON(tags_version_radio), "toggled", 
      G_CALLBACK(save_preferences), NULL);

  tags_version_radio = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(tags_version_radio), _("ID3v1 tags"));
  g_signal_connect(GTK_TOGGLE_BUTTON(tags_version_radio), "toggled", 
      G_CALLBACK(save_preferences), NULL);
  gtk_box_pack_start(GTK_BOX(vbox), tags_version_radio, FALSE, FALSE, 0);

  tags_version_radio = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON (tags_version_radio),_("Same tags version as the input file"));
  g_signal_connect(GTK_TOGGLE_BUTTON(tags_version_radio), "toggled", 
      G_CALLBACK(save_preferences), NULL);
  gtk_box_pack_start(GTK_BOX(vbox), tags_version_radio, FALSE, FALSE, 0);

  return wh_set_title_and_get_vbox(vbox, _("<b>Tags version (mp3 only)</b>"));
}

//! Create the tags settings tab
GtkWidget *create_pref_tags_page()
{
  GtkWidget *outside_vbox = gtk_vbox_new(FALSE, 0);;
  GtkWidget *inside_hbox = gtk_hbox_new(FALSE, 0);

  GtkWidget *scrolled_window = create_scrolled_window();
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), 
                                        GTK_WIDGET(inside_hbox));
  gtk_box_pack_start(GTK_BOX(outside_vbox), scrolled_window, TRUE, TRUE, 0);

  GtkWidget *vbox = gtk_vbox_new(FALSE, 0);;
  gtk_box_pack_start(GTK_BOX(inside_hbox), vbox, TRUE, TRUE, 10);

  GtkWidget *tags_opts_box = create_tags_options_box();
  gtk_box_pack_start(GTK_BOX(vbox), tags_opts_box, FALSE, FALSE, 10);

  GtkWidget *tags_version_box = create_tags_version_box();
  gtk_box_pack_start(GTK_BOX(vbox), tags_version_box, FALSE, FALSE, 0);
  
  return outside_vbox;
}

//!creates the preferences tab
GtkWidget *create_choose_preferences()
{
  //our preferences vbox
  GtkWidget *pref_vbox = gtk_vbox_new(FALSE, 0);

  GtkWidget *notebook = gtk_notebook_new();
  gtk_box_pack_start(GTK_BOX(pref_vbox), notebook, TRUE, TRUE, 0);
  
  gtk_notebook_popup_enable(GTK_NOTEBOOK(notebook));
  gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), TRUE);
  gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), FALSE);
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
  
  /* split preferences */
  GtkWidget *splitpoints_prefs = (GtkWidget *)create_pref_splitpoints_page();
  GtkWidget *notebook_label = gtk_label_new(_("Split"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), splitpoints_prefs,
                           (GtkWidget *)notebook_label);

  /* tags preferences */
  GtkWidget *tags_prefs = (GtkWidget *)create_pref_tags_page();
  notebook_label = gtk_label_new(_("Tags"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), tags_prefs,
                           (GtkWidget *)notebook_label);

  /* output preferences */
  GtkWidget *output_prefs = (GtkWidget *)create_pref_output_page();
  notebook_label = gtk_label_new(_("Output"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), output_prefs,
                           (GtkWidget *)notebook_label);
  
  /* player preferences */
  GtkWidget *player_prefs = (GtkWidget *)create_pref_player_page();
  notebook_label = gtk_label_new(_("Player"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), player_prefs,
                           (GtkWidget *)notebook_label);

  /* language preferences page */
#ifdef __WIN32__
  GtkWidget *language_prefs = (GtkWidget *)create_pref_language_page();
  notebook_label = gtk_label_new(_("Language"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), language_prefs,
                           (GtkWidget *)notebook_label);
#endif
  
  return pref_vbox;
}
