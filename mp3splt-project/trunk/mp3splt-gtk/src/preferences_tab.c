/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2009 Alexandru Munteanu
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
#include "preferences_manager.h"

/* split preferences for choosing directory */
//directory entry
GtkWidget *directory_entry = NULL;

//output for the cddb,cue and freedb file output
GtkWidget *output_entry = NULL;
GtkWidget *output_label = NULL;

//if we have selected a correct file
gint incorrect_selected_dir = FALSE;

//choose the player box
GtkWidget *player_combo_box = NULL;

//list where we stock the preferences combo box content
GList *player_pref_list = NULL;
//selected player
gint selected_player = PLAYER_GSTREAMER;

//the language radio button
GtkWidget *radio_button = NULL;

//radio button for choosing default or custom output options
GtkWidget *radio_output = NULL;

//radio button for tags options
GtkWidget *tags_radio = NULL;
GtkWidget *tags_version_radio = NULL;

//split options
//frame mode option
GtkWidget *frame_mode = NULL;
//auto-adjust option
GtkWidget *adjust_mode = NULL;

GtkWidget *create_dirs_from_output_files = NULL;

//adjust mode parameters
GtkWidget *spinner_adjust_gap = NULL;
GtkWidget *gap_label = NULL;
GtkWidget *spinner_adjust_offset = NULL;
GtkWidget *offset_label = NULL;
GtkWidget *spinner_adjust_threshold = NULL;
GtkWidget *threshold_label = NULL;

extern GtkWidget *player_box;
extern GtkWidget *queue_files_button;
extern splt_state *the_state;
extern gint selected_split_mode;
extern gint split_file_mode;
extern GtkWidget *spinner_time;

//returns the selected language
//must be free() after
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
    test = (GtkToggleButton *)g_slist_nth_data(radio_button_list,i);
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(test)))
    {
      selected = i;
    }
  }

  return selected;
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

//returns the checked tags radio box
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

//save preferences event
void save_preferences(GtkWidget *widget, gpointer data)
{
  gchar *filename = get_preferences_filename();

  GKeyFile *my_key_file = g_key_file_new();
  g_key_file_load_from_file(my_key_file, filename, G_KEY_FILE_KEEP_COMMENTS, NULL);

  //save_path
  g_key_file_set_string(my_key_file, "split", "save_path",
      gtk_entry_get_text(GTK_ENTRY(directory_entry)));
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
      gtk_entry_get_text(GTK_ENTRY(output_entry)));
  //default output format
  g_key_file_set_boolean(my_key_file, "output", "default_output_format",
      get_checked_output_radio_box());
  g_key_file_set_boolean(my_key_file, "output", "create_dirs_if_needed",
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(create_dirs_from_output_files)));

  //tags
  g_key_file_set_integer(my_key_file, "split", "tags",
      get_checked_tags_radio_box());
  //tags version
  g_key_file_set_integer(my_key_file, "split", "tags_version",
      get_checked_tags_version_radio_box());

  //type of split: split mode
  g_key_file_set_integer(my_key_file, "split", "split_mode",
      selected_split_mode);
  //type of split: time value
  g_key_file_set_integer(my_key_file, "split", "split_mode_time_value",
      gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinner_time)));
  //type of split: file mode
  g_key_file_set_integer(my_key_file, "split", "file_mode",
      split_file_mode);

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

//creates a scrolled window
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

//cddb and cue output mode radio box event
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

  return set_title_and_get_vbox(radio_vbox,
      _("<b>Choose language (requires restart)</b>"));
}

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

//events for browse dir button
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

    gtk_entry_set_text(GTK_ENTRY(directory_entry), filename);

    g_free (filename);
    filename = NULL;

    save_preferences(NULL, NULL);
  }

  //destroys dialog
  gtk_widget_destroy(dir_chooser);
}

//disables adjust parameters
void disable_adjust_spinners()
{
  gtk_widget_set_sensitive(spinner_adjust_threshold, FALSE);
  gtk_widget_set_sensitive(spinner_adjust_offset, FALSE);
  gtk_widget_set_sensitive(spinner_adjust_gap, FALSE);
  gtk_widget_set_sensitive(threshold_label, FALSE);
  gtk_widget_set_sensitive(offset_label, FALSE);
  gtk_widget_set_sensitive(gap_label, FALSE);
}

//enables adjust parameters
void enable_adjust_spinners()
{
  gtk_widget_set_sensitive(spinner_adjust_threshold, TRUE);
  gtk_widget_set_sensitive(spinner_adjust_offset, TRUE);
  gtk_widget_set_sensitive(spinner_adjust_gap, TRUE);
  gtk_widget_set_sensitive(threshold_label, TRUE);
  gtk_widget_set_sensitive(offset_label, TRUE);
  gtk_widget_set_sensitive(gap_label, TRUE);
}

//adjust event
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

//frame mode event
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

//action for the set default prefs button
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

//events for the "set current song directory"
void song_dir_button_event(GtkWidget *widget, gpointer data)
{
  gtk_entry_set_text(GTK_ENTRY(directory_entry), "");
  save_preferences(NULL, NULL);
}

GtkWidget *create_directory_box()
{
  GtkWidget *dir_hbox = gtk_hbox_new(FALSE, 0);
  
  //directory entry
  directory_entry = gtk_entry_new();
  gtk_entry_set_editable(GTK_ENTRY(directory_entry), FALSE);
  gtk_box_pack_start(GTK_BOX(dir_hbox), directory_entry, TRUE, TRUE, 0);
  
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

  return set_title_and_get_vbox(dir_hbox, _("<b>Directory for split files</b>"));
}

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

  return set_title_and_get_vbox(vbox, _("<b>Split options</b>"));
}

//creates the splitpoints preferences page
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

//removes unavailable players from the combo
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

//event when changing the combo box player
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
 
  return set_title_and_get_vbox(horiz_fake, _("<b>Player options</b>"));
}

//creates the player preferences page
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

//update the save buttons when output entry event
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

  return set_title_and_get_vbox(vbox, _("<b>Output filename format</b>"));
}

//creates the output preferences page
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

GtkWidget *create_tags_options_box()
{
  GtkWidget *vbox = gtk_vbox_new(FALSE, 0);

  GtkWidget *horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start(GTK_BOX(vbox), horiz_fake, FALSE, FALSE, 0);

  tags_radio = gtk_radio_button_new_with_label(NULL, _("Original file tags"));
  gtk_box_pack_start(GTK_BOX(vbox), tags_radio, FALSE, FALSE, 0);
  g_signal_connect(GTK_TOGGLE_BUTTON(tags_radio), "toggled", 
      G_CALLBACK(save_preferences), NULL);

  tags_radio = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(tags_radio), _("Default tags (cddb or cue tags)"));
  g_signal_connect(GTK_TOGGLE_BUTTON(tags_radio), "toggled", 
      G_CALLBACK(save_preferences), NULL);
  gtk_box_pack_start(GTK_BOX(vbox), tags_radio, FALSE, FALSE, 0);

  tags_radio = gtk_radio_button_new_with_label_from_widget(
      GTK_RADIO_BUTTON(tags_radio),_("No tags"));
  g_signal_connect(GTK_TOGGLE_BUTTON(tags_radio), "toggled", 
      G_CALLBACK(save_preferences), NULL);
  gtk_box_pack_start(GTK_BOX(vbox), tags_radio, FALSE, FALSE, 0);

  return set_title_and_get_vbox(vbox, _("<b>Split files tags</b>"));
}

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

  return set_title_and_get_vbox(vbox, _("<b>Tags version (mp3 only)</b>"));
}

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

//creates the preferences 
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

