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
 * along with this program; if not, write to the Free Software
 * You should have received a copy of the GNU General Public License
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

#include "preferences_tab.h"

/*! The name of the output directory.

  Can be accessed by void set_output_directory() and 
  outputdirectory_get().
 */
GString *outputdirname = NULL;

//! The textbox that shows the name of the output directory
GtkWidget *directory_entry = NULL;

//!choose the player box
GtkWidget *player_combo_box = NULL;

//!the language radio button
GtkWidget *radio_button = NULL;

gint preview_indexes[6] = { 0 };
GPtrArray *wave_preview_labels = NULL;

gint douglas_peucker_indexes[5] = { 0, 1, 2, 3, 4};

extern gint timeout_value;

static GtkWidget *create_extract_tags_from_filename_options_box();
static GtkWidget *create_test_regex_table();

extern ui_state *ui;

static void update_wave_preview_label_markup(gint index, gint interpolation_level);

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
    return g_string_new("de_DE");
  }
  else 
  {
    our_button = (GtkWidget *)g_slist_nth_data(radio_button_list, 1);
    if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(our_button)))
    {
      return g_string_new("fr_FR");
    }
  }

  return g_string_new("en");
}

//!returns the checked output radio box
gboolean get_checked_output_radio_box(ui_state *ui)
{
  GSList *radio_button_list = gtk_radio_button_get_group(GTK_RADIO_BUTTON(ui->gui->radio_output));
  //O = default output mode
  //1 = custom output mode
  gint i;
  for(i = 0; i<2;i++)
  {
    GtkToggleButton *test = (GtkToggleButton *)g_slist_nth_data(radio_button_list,i);
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(test)))
    {
      return i;
    }
  }

  return 0;
}

//!returns the checked tags radio box
gint get_checked_tags_version_radio_box(gui_state *gui)
{
  GSList *radio_button_list = gtk_radio_button_get_group(GTK_RADIO_BUTTON(gui->tags_version_radio));

  //O = The same version as the original file
  //1 = ID3v1
  //2 = ID3v2
  //3 = ID3v1 & ID3v2
  gint i = 0;
  for(i = 0; i < 4;i++)
  {
    GtkToggleButton *button = GTK_TOGGLE_BUTTON(g_slist_nth_data(radio_button_list,i));
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(button)))
    {
      return i;
    }
  }
}

//! Set the name of the output directory
void set_output_directory(gchar *dirname)
{
  if (dirname == NULL)
  {
    return;
  }

  if (outputdirname != NULL)
  {
    g_string_free(outputdirname,TRUE);
  }
  outputdirname = g_string_new(dirname);

  gtk_entry_set_text(GTK_ENTRY(directory_entry), dirname);
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

//!cddb and cue output mode radio box event
void output_radio_box_event(GtkToggleButton *radio_b, gpointer data)
{
  GtkWidget *output_label = (GtkWidget *)data;

  gint selected = get_checked_output_radio_box(ui);

  //custom output mode
  if (selected == 0)
  {
    gtk_widget_set_sensitive(ui->gui->output_entry, TRUE);
    gtk_widget_set_sensitive(ui->gui->output_label, TRUE);
    mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_OUTPUT_FILENAMES, SPLT_OUTPUT_FORMAT);
  }
  else
  {
    gtk_widget_set_sensitive(ui->gui->output_entry, FALSE);
    gtk_widget_set_sensitive(ui->gui->output_label, FALSE);
    mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_OUTPUT_FILENAMES, SPLT_OUTPUT_DEFAULT);
  }

  save_preferences(NULL, NULL);
}

//! Create the "select language" box
GtkWidget *create_language_box()
{
  GtkWidget *radio_vbox = wh_vbox_new();

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
  GtkWidget *language_hbox = wh_hbox_new();;
 
  //vertical box inside the scrolled window
  GtkWidget *language_inside_hbox = wh_hbox_new();;

  //scrolled window
  GtkWidget *scrolled_window;
  scrolled_window = wh_create_scrolled_window();
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), 
                                        GTK_WIDGET(language_inside_hbox));
  gtk_box_pack_start(GTK_BOX(language_hbox), scrolled_window, TRUE, TRUE, 0);
 
  //vertical box inside the horizontal box from the scrolled window
  GtkWidget *vbox = wh_vbox_new();;
  gtk_box_pack_start(GTK_BOX(language_inside_hbox), vbox, TRUE, TRUE, 10);

  GtkWidget *lang_box = create_language_box();
  gtk_box_pack_start(GTK_BOX(vbox), lang_box, FALSE, FALSE, 10);
  
  return language_hbox;
}

//! Events for browse dir button
void browse_dir_button_event(GtkWidget *widget, gpointer data)
{
  GtkWidget *dir_chooser = gtk_file_chooser_dialog_new(_("Choose split directory"),
      NULL,
      GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER,
      GTK_STOCK_CANCEL,
      GTK_RESPONSE_CANCEL,
      GTK_STOCK_OPEN,
      GTK_RESPONSE_ACCEPT,
      NULL);

  wh_set_browser_directory_handler(ui, dir_chooser);

  if (gtk_dialog_run(GTK_DIALOG(dir_chooser)) == GTK_RESPONSE_ACCEPT)
  {
    gchar *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dir_chooser));
    set_output_directory(filename);

    g_free(filename);
    filename = NULL;

    save_preferences(NULL, NULL);
  }

  gtk_widget_destroy(dir_chooser);
}

//!disables adjust parameters
void disable_adjust_spinners()
{
  gtk_widget_set_sensitive(ui->gui->spinner_adjust_threshold, FALSE);
  gtk_widget_set_sensitive(ui->gui->spinner_adjust_offset, FALSE);
  gtk_widget_set_sensitive(ui->gui->spinner_adjust_gap, FALSE);
  gtk_widget_set_sensitive(ui->gui->threshold_label, FALSE);
  gtk_widget_set_sensitive(ui->gui->offset_label, FALSE);
  gtk_widget_set_sensitive(ui->gui->gap_label, FALSE);
}

//!enables adjust parameters
void enable_adjust_spinners()
{
  gtk_widget_set_sensitive(ui->gui->spinner_adjust_threshold, TRUE);
  gtk_widget_set_sensitive(ui->gui->spinner_adjust_offset, TRUE);
  gtk_widget_set_sensitive(ui->gui->spinner_adjust_gap, TRUE);
  gtk_widget_set_sensitive(ui->gui->threshold_label, TRUE);
  gtk_widget_set_sensitive(ui->gui->offset_label, TRUE);
  gtk_widget_set_sensitive(ui->gui->gap_label, TRUE);
}

//!adjust event
void adjust_event(GtkToggleButton *adjust_mode, gpointer user_data)
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(adjust_mode)))
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ui->gui->frame_mode),TRUE);
    enable_adjust_spinners();
  }
  else
  {
    disable_adjust_spinners();
  }

  save_preferences(NULL, NULL);
}

//!frame mode event
void frame_event(GtkToggleButton *frame_mode, gpointer user_data)
{
  if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(frame_mode)))
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ui->gui->adjust_mode),FALSE);
  }

  save_preferences(NULL, NULL);
}

void splitpoints_from_filename_event(GtkToggleButton *frame_mode, gpointer user_data)
{
  gint splitpoints_from_filename = FALSE;

  if (!gtk_toggle_button_get_active(ui->gui->names_from_filename))
  {
    splitpoints_from_filename = FALSE;
  }
  else
  {
    splitpoints_from_filename = TRUE;
  }

  if (splitpoints_from_filename == TRUE && ui->status->file_browsed == TRUE)
  {
    copy_filename_to_current_description(get_input_filename(ui->gui), ui);
  }
  else
  {
    clear_current_description(ui);
  }

  save_preferences(NULL, NULL);
}

//!action for the set default prefs button
void set_default_prefs_event(GtkWidget *widget, gpointer data)
{
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ui->gui->frame_mode), FALSE);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ui->gui->adjust_mode), FALSE);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ui->gui->spinner_adjust_threshold),
                            SPLT_DEFAULT_PARAM_THRESHOLD);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ui->gui->spinner_adjust_offset),
                            SPLT_DEFAULT_PARAM_OFFSET);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(ui->gui->spinner_adjust_gap),
                            SPLT_DEFAULT_PARAM_GAP);
  gtk_toggle_button_set_active(ui->gui->names_from_filename, FALSE);

  save_preferences(NULL, NULL);
}

//!events for the "set current song directory"
void song_dir_button_event(GtkWidget *widget, gpointer data)
{
  set_output_directory("");
  save_preferences(NULL, NULL);
}

//!Creates the box the output directory can be choosen in
GtkWidget *create_directory_box()
{
  GtkWidget *dir_hbox = wh_hbox_new();
  
  //directory entry
  directory_entry = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(directory_entry), FALSE);
  gtk_box_pack_start(GTK_BOX(dir_hbox), directory_entry, TRUE, TRUE, 0);
  // Put the right text into the text box containing the output directory
  // name if this name was provided on command line
  if(outputdirectory_get()!=NULL)
    gtk_entry_set_text(GTK_ENTRY(directory_entry), outputdirectory_get());
  
  //browse dir button
  GtkWidget *browse_dir_button =
    wh_create_cool_button(GTK_STOCK_DIRECTORY,_("Br_owse dir"), FALSE);
  g_signal_connect(G_OBJECT(browse_dir_button), "clicked",
      G_CALLBACK(browse_dir_button_event), NULL);
  gtk_box_pack_start(GTK_BOX(dir_hbox), browse_dir_button, FALSE, FALSE, 8);
  
  //to set the directory for split files to the current song
  //directory
  GtkWidget *song_dir_button =
    wh_create_cool_button(GTK_STOCK_CLEAR, _("_Song dir"), FALSE);
  g_signal_connect(G_OBJECT(song_dir_button), "clicked",
      G_CALLBACK(song_dir_button_event), NULL);
  gtk_box_pack_start(GTK_BOX(dir_hbox), song_dir_button, FALSE, FALSE, 0);

  return wh_set_title_and_get_vbox(dir_hbox, _("<b>Directory for split files</b>"));
}

//! Creates the box for split mode selection
GtkWidget *create_split_options_box()
{
  GtkWidget *vbox = wh_vbox_new();

  //names from filename
  GtkToggleButton *names_from_filename = 
    GTK_TOGGLE_BUTTON(gtk_check_button_new_with_mnemonic(_("_Splitpoint name from filename (testing)")));
  ui->gui->names_from_filename = names_from_filename;

  gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(names_from_filename), FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(names_from_filename), "toggled",
      G_CALLBACK(splitpoints_from_filename_event), NULL);

  GtkWidget *create_dirs_from_output_files =
    gtk_check_button_new_with_mnemonic(_("_Create directories from filenames "));
  ui->gui->create_dirs_from_output_files = create_dirs_from_output_files;
  gtk_box_pack_start(GTK_BOX(vbox), create_dirs_from_output_files, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(create_dirs_from_output_files), "toggled",
      G_CALLBACK(save_preferences), NULL);

  //frame mode option
  GtkWidget *frame_mode =
    gtk_check_button_new_with_mnemonic(_("F_rame mode (useful for mp3 VBR) (mp3 only)"));
  ui->gui->frame_mode = frame_mode;
  gtk_box_pack_start(GTK_BOX(vbox), frame_mode, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(frame_mode), "toggled", G_CALLBACK(frame_event), NULL);

  //auto adjust option
  GtkWidget *adjust_mode = gtk_check_button_new_with_mnemonic(_("_Auto-adjust mode (uses"
        " silence detection to auto-adjust splitpoints)"));
  ui->gui->adjust_mode = adjust_mode;
  gtk_box_pack_start(GTK_BOX(vbox), adjust_mode, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(adjust_mode), "toggled",
      G_CALLBACK(adjust_event), NULL);
  
  //parameters for the adjust option
  GtkWidget *horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(vbox), horiz_fake, FALSE, FALSE, 0);
  
  GtkWidget *param_vbox = wh_vbox_new();
  gtk_box_pack_start(GTK_BOX(horiz_fake), param_vbox, FALSE, FALSE, 25);
  
  //threshold level
  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(param_vbox), horiz_fake, FALSE, FALSE, 0);
  
  GtkWidget *threshold_label = gtk_label_new(_("Threshold level (dB):"));
  ui->gui->threshold_label = threshold_label;
  gtk_box_pack_start(GTK_BOX(horiz_fake), threshold_label, FALSE, FALSE, 0);
  
  GtkAdjustment *adj = (GtkAdjustment *) gtk_adjustment_new(0.0, -96.0, 0.0,
      0.5, 10.0, 0.0);
  GtkWidget *spinner_adjust_threshold = gtk_spin_button_new (adj, 0.5, 2);
  ui->gui->spinner_adjust_threshold = spinner_adjust_threshold;
  g_signal_connect(G_OBJECT(spinner_adjust_threshold), "value_changed",
      G_CALLBACK(save_preferences), NULL);
  gtk_box_pack_start(GTK_BOX(horiz_fake), spinner_adjust_threshold,
                      FALSE, FALSE, 6);
  
  //offset level
  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(param_vbox), horiz_fake, FALSE, FALSE, 0);
  
  GtkWidget *offset_label = gtk_label_new(_("Cutpoint offset (0 is the begin of silence "
        "and 1 the end):"));
  ui->gui->offset_label = offset_label;
  gtk_box_pack_start(GTK_BOX(horiz_fake), offset_label, FALSE, FALSE, 0);
  
  //adjustement for the offset spinner
  adj = (GtkAdjustment *)gtk_adjustment_new(0.0, -2, 2, 0.05, 10.0, 0.0);
  GtkWidget *spinner_adjust_offset = gtk_spin_button_new (adj, 0.05, 2);
  ui->gui->spinner_adjust_offset = spinner_adjust_offset;
  g_signal_connect(G_OBJECT(spinner_adjust_offset), "value_changed",
      G_CALLBACK(save_preferences), NULL);
  gtk_box_pack_start(GTK_BOX(horiz_fake), spinner_adjust_offset, FALSE, FALSE, 6);
  
  //gap level (seconds)
  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(param_vbox), horiz_fake, FALSE, FALSE, 0);
  
  GtkWidget *gap_label = gtk_label_new(_("Gap level (seconds around splitpoint to "
        "search for silence):"));
  ui->gui->gap_label = gap_label;
  gtk_box_pack_start(GTK_BOX(horiz_fake), gap_label, FALSE, FALSE, 0);
  
  adj = (GtkAdjustment *) gtk_adjustment_new(0.0, 0, 2000, 1.0, 10.0, 0.0);
  GtkWidget *spinner_adjust_gap = gtk_spin_button_new (adj, 1, 0);
  ui->gui->spinner_adjust_gap = spinner_adjust_gap;
  g_signal_connect(G_OBJECT(spinner_adjust_gap), "value_changed",
      G_CALLBACK(save_preferences), NULL);
  gtk_box_pack_start(GTK_BOX(horiz_fake), spinner_adjust_gap, FALSE, FALSE, 6);
  
  disable_adjust_spinners();
 
  //set default preferences button
  //horizontal box fake for the gap level
  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(vbox), horiz_fake, FALSE, FALSE, 0);
  
  GtkWidget *set_default_prefs_button =
    wh_create_cool_button(GTK_STOCK_PREFERENCES, _("Set _default split" " options"),FALSE); 
  g_signal_connect(G_OBJECT(set_default_prefs_button), "clicked",
      G_CALLBACK(set_default_prefs_event), NULL);
  gtk_box_pack_start (GTK_BOX (horiz_fake), set_default_prefs_button,
                      FALSE, FALSE, 5);

  return wh_set_title_and_get_vbox(vbox, _("<b>Split options</b>"));
}

//!creates the splitpoints preferences page
GtkWidget *create_pref_splitpoints_page()
{
  GtkWidget *general_hbox = wh_hbox_new();
  GtkWidget *inside_hbox = wh_hbox_new();
  
  GtkWidget *scrolled_window = wh_create_scrolled_window();
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), 
                                        GTK_WIDGET(inside_hbox));
  gtk_box_pack_start(GTK_BOX(general_hbox), scrolled_window, TRUE, TRUE, 0);
 
  //vertical box inside the horizontal box from the scrolled window
  GtkWidget *inside_vbox = wh_vbox_new();
  gtk_box_pack_start(GTK_BOX(inside_hbox), inside_vbox, TRUE, TRUE, 5);
 
  GtkWidget *dir_box = create_directory_box();
  gtk_box_pack_start(GTK_BOX(inside_vbox), dir_box, FALSE, FALSE, 2);

  GtkWidget *split_options_box = create_split_options_box();
  gtk_box_pack_start(GTK_BOX(inside_vbox), split_options_box, FALSE, FALSE, 1);
 
  return general_hbox;
}

//!event when changing the combo box player
void player_combo_box_event(GtkComboBox *widget, gpointer data)
{
  disconnect_button_event(ui->gui->disconnect_button, ui);

  ui->infos->selected_player = ch_get_active_value(widget);
  if (ui->infos->selected_player == PLAYER_GSTREAMER)
  {
    hide_connect_button(ui->gui);
    gtk_widget_show(ui->gui->playlist_box);
  }
  else
  {
    show_connect_button(ui->gui);
    gtk_widget_hide(ui->gui->playlist_box);
  }
  
  gtk_widget_show(ui->gui->player_box);
  gtk_widget_show(ui->gui->queue_files_button);

  save_preferences(NULL, NULL);
}

void update_timeout_value(GtkWidget *spinner, gpointer data)
{
  timeout_value = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinner));

  restart_player_timer();
  save_preferences(NULL, NULL);
}

//!Create the box the player backend can be selected with
GtkWidget *create_player_options_box()
{
  GtkWidget *vbox = wh_vbox_new();

  GtkWidget *horiz_fake = wh_hbox_new();

  GtkWidget *label = gtk_label_new(_("Player:"));
  gtk_box_pack_start(GTK_BOX(horiz_fake), label, FALSE, FALSE, 0);

  player_combo_box = GTK_WIDGET(ch_new_combo());

#ifndef NO_AUDACIOUS
  ch_append_to_combo(GTK_COMBO_BOX(player_combo_box), "Audacious", PLAYER_AUDACIOUS);
#endif
  ch_append_to_combo(GTK_COMBO_BOX(player_combo_box), "SnackAmp", PLAYER_SNACKAMP);
#ifndef NO_GSTREAMER
  ch_append_to_combo(GTK_COMBO_BOX(player_combo_box), "GStreamer", PLAYER_GSTREAMER);
#endif

  g_signal_connect(G_OBJECT(player_combo_box), "changed",
      G_CALLBACK(player_combo_box_event), NULL);

  gtk_box_pack_start(GTK_BOX(horiz_fake), player_combo_box, FALSE, FALSE, 5);
  gtk_box_pack_start(GTK_BOX(vbox), horiz_fake, FALSE, FALSE, 0);

  GtkWidget *spinner = wh_create_int_spinner_in_box(_("Refresh player every "),
      _("milliseconds."),
      (gdouble)DEFAULT_TIMEOUT_VALUE, 20.0, 1000.0, 10.0, 100.0,
      _("\t(higher refresh rate decreases CPU usage - default is 200)"),
      update_timeout_value, NULL, vbox);
  ui_register_spinner_int_preference("player", "refresh_rate", DEFAULT_TIMEOUT_VALUE,
      spinner, update_timeout_value, NULL, ui);
 
  return wh_set_title_and_get_vbox(vbox, _("<b>Player options</b>"));
}

void wave_quality_changed_event(GtkAdjustment *wave_quality_adjustment, gpointer user_data)
{
  gint quality_level = (gint)gtk_adjustment_get_value(GTK_ADJUSTMENT(wave_quality_adjustment));

  gint level = 0;
  for (level = 0; level <= 4; level++)
  {
    gdouble default_value = ui->infos->douglas_peucker_thresholds_defaults[level];
    gdouble final_value = default_value - quality_level;
    if (final_value <= 0)
    {
      final_value = 0.1;
    }

    ui->infos->douglas_peucker_thresholds[level] = final_value;
  }

  gint default_number_of_points_th = DEFAULT_SILENCE_WAVE_NUMBER_OF_POINTS_THRESHOLD;
  gint number_of_points_th = default_number_of_points_th + (quality_level * 1000);
  if (number_of_points_th <= 0)
  {
    number_of_points_th = 0;
  }

  ui->infos->silence_wave_number_of_points_threshold = number_of_points_th;

  compute_douglas_peucker_filters(ui);
  refresh_preview_drawing_areas(ui->gui);

  save_preferences(NULL, NULL);
}

void refresh_preview_drawing_areas(gui_state *gui)
{
  gint i = 0;
  for (i = 0; i < gui->wave_quality_das->len; i++)
  {
    gtk_widget_queue_draw(g_ptr_array_index(gui->wave_quality_das, i));
  }

  gtk_widget_queue_draw(gui->player_scrolled_window);
}

#if GTK_MAJOR_VERSION <= 2
gboolean wave_quality_draw_event(GtkWidget *drawing_area, GdkEventExpose *event, gpointer data)
{
  cairo_t *cairo_surface = gdk_cairo_create(drawing_area->window);
#else
gboolean wave_quality_draw_event(GtkWidget *drawing_area, cairo_t *cairo_surface, gpointer data)
{
#endif
  gint width = get_wave_preview_width_drawing_area();
  gtk_widget_set_size_request(drawing_area, width, 70);

  gint *index = (gint *)data;
  gint expected_drawing_time_int =
    g_array_index(ui->infos->preview_time_windows, gint, *index);
  gfloat expected_drawing_time = (gfloat)(expected_drawing_time_int);

  dh_set_white_color(cairo_surface);

  dh_draw_rectangle(cairo_surface, TRUE, 0, 0, width, 70); 
  gfloat current_time = ui->infos->total_time / 2.0;

  gfloat drawing_time = 0;
  gfloat zoom_coeff = 0.2;

  gfloat left_time = 0;
  gfloat right_time = 0;
  while ((drawing_time == 0) || (drawing_time > expected_drawing_time))
  {
    left_time = get_left_drawing_time(current_time, ui->infos->total_time, zoom_coeff);
    right_time = get_right_drawing_time(current_time, ui->infos->total_time, zoom_coeff);
    drawing_time = right_time - left_time;
    zoom_coeff += 0.2;

    if (zoom_coeff > 100) { break; }
  }

  gint interpolation_level = draw_silence_wave((gint)left_time, (gint)right_time, width / 2, 50,
      drawing_time, width, 0,
      current_time, ui->infos->total_time, zoom_coeff,
      drawing_area, cairo_surface);

  update_wave_preview_label_markup(*index, interpolation_level);

#if GTK_MAJOR_VERSION <= 2
  cairo_destroy(cairo_surface);
#endif

  return TRUE;
}

gint get_wave_preview_width_drawing_area()
{
  if (ui->infos->width_drawing_area < 50)
  {
    return 500;
  }

  return ui->infos->width_drawing_area;
}

GtkWidget *create_wave_quality_preview_box()
{
  GtkWidget *vbox = wh_vbox_new();

  GtkWidget *label_hbox = wh_hbox_new();
  GtkWidget *wave_preview_label = gtk_label_new(NULL);

  gchar wave_availability[256] = { '\0' };
  g_snprintf(wave_availability, 256, "<span style='italic' color='#0000AA'>%s</span>",
      _("Wave preview is only available if the amplitude wave is ticked in the player"));
  gtk_label_set_markup(GTK_LABEL(wave_preview_label), wave_availability);
  gtk_box_pack_start(GTK_BOX(label_hbox), wave_preview_label, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), label_hbox, FALSE, FALSE, 4);

  ui->gui->wave_quality_das = g_ptr_array_new();
  wave_preview_labels = g_ptr_array_new();

  gint i = 0;
  for (i = 0; i < ui->infos->preview_time_windows->len; i++)
  {
    GtkWidget *wave_quality_da = gtk_drawing_area_new(); 
    g_ptr_array_add(ui->gui->wave_quality_das, (gpointer)wave_quality_da);
    preview_indexes[i] = i;

#if GTK_MAJOR_VERSION <= 2
    g_signal_connect(wave_quality_da, "expose_event", G_CALLBACK(wave_quality_draw_event), 
        &preview_indexes[i]);
#else
    g_signal_connect(wave_quality_da, "draw", G_CALLBACK(wave_quality_draw_event), 
        &preview_indexes[i]);
#endif
    wh_put_in_hbox_and_attach_to_vbox(wave_quality_da, vbox, 0);

    GtkWidget *minutes_label = gtk_label_new(NULL);
    g_ptr_array_add(wave_preview_labels, minutes_label);
    update_wave_preview_label_markup(i, -1);

    wh_put_in_hbox_and_attach_to_vbox_with_bottom_margin(minutes_label, vbox, 0, 4);
  }

  return vbox;
}

static void update_wave_preview_label_markup(gint index, gint interpolation_level)
{
  gint time_window = g_array_index(ui->infos->preview_time_windows, gint, index);

  gchar minutes_text[128] = { '\0' };
  g_snprintf(minutes_text, 128, _("%d minute(s) window"), time_window / 100 / 60);

  gchar interpolation_text[256] = { '\0' };
  if (interpolation_level >= 0)
  {
    g_snprintf(interpolation_text, 256, _("Wave interpolation level %d with threshold of %.1lf"),
        interpolation_level + 1, ui->infos->douglas_peucker_thresholds[interpolation_level]);
  }
  else {
    g_snprintf(interpolation_text, 256, _("No wave interpolation"));
  }

  gchar final_text_with_color[512] = { '\0' };
  g_snprintf(final_text_with_color, 512, 
      "<span color='#DD0000'>%s</span> - <span>%s</span>",
      minutes_text, interpolation_text);

  GtkWidget *text_label = g_ptr_array_index(wave_preview_labels, index);
  gtk_label_set_markup(GTK_LABEL(text_label), final_text_with_color);
}

GtkWidget *create_wave_options_box()
{
  GtkWidget *vbox = wh_vbox_new();
  GtkWidget *range_hbox = wh_hbox_new();

  GtkWidget *wave_quality_label =
    gtk_label_new(_("Wave quality (higher is better but consumes more CPU):"));
  gtk_box_pack_start(GTK_BOX(range_hbox), wave_quality_label, FALSE, FALSE, 0);
 
  GtkWidget *wave_quality_hscale = wh_hscale_new_with_range(-6.0, 6.0, 1.0);
  gtk_scale_set_draw_value(GTK_SCALE(wave_quality_hscale), TRUE);
  gtk_box_pack_start(GTK_BOX(range_hbox), wave_quality_hscale, FALSE, FALSE, 4);
  gtk_widget_set_size_request(wave_quality_hscale, 160, 0);

  gtk_range_set_increments(GTK_RANGE(wave_quality_hscale), 1.0, 1.0);

  ui_register_range_preference("player", "wave_quality", 0.0,
      wave_quality_hscale, wave_quality_changed_event, NULL, ui);

  GtkAdjustment *wave_quality_adjustment = gtk_range_get_adjustment(GTK_RANGE(wave_quality_hscale));
  g_signal_connect(G_OBJECT(wave_quality_adjustment), "value-changed",
      G_CALLBACK(wave_quality_changed_event), NULL);

  gtk_box_pack_start(GTK_BOX(vbox), range_hbox, FALSE, FALSE, 0);

  return wh_set_title_and_get_vbox(vbox, _("<b>Amplitude wave options</b>"));
}

//!creates the player preferences page
GtkWidget *create_pref_player_page()
{
  GtkWidget *player_hbox = wh_hbox_new();;

  GtkWidget *inside_vbox = wh_vbox_new();;
  
  GtkWidget *player_scrolled_window = wh_create_scrolled_window();
  ui->gui->player_scrolled_window = player_scrolled_window;
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(player_scrolled_window), 
                                        GTK_WIDGET(inside_vbox));
  gtk_box_pack_start(GTK_BOX(player_hbox), player_scrolled_window, TRUE, TRUE, 0);
  
  //vertical box inside the horizontal box from the scrolled window
  GtkWidget *vbox = wh_vbox_new();;
  gtk_box_pack_start(GTK_BOX(inside_vbox), vbox, TRUE, TRUE, 5);
  
  //choose player combo box
  GtkWidget *player_options_box = create_player_options_box();
  gtk_box_pack_start(GTK_BOX(vbox), player_options_box, FALSE, FALSE, 3);
  GtkWidget *wave_options_box = create_wave_options_box();
  gtk_box_pack_start(GTK_BOX(vbox), wave_options_box, FALSE, FALSE, 3);

  GtkWidget *wave_quality_box = create_wave_quality_preview_box();
  gtk_box_pack_start(GTK_BOX(inside_vbox), wave_quality_box, FALSE, FALSE, 0);
 
  return player_hbox;
}

//!update the save buttons on an output entry event
gboolean output_entry_event(GtkWidget *widget, GdkEventKey *event,
    gpointer user_data)
{
  const char *data = gtk_entry_get_text(GTK_ENTRY(ui->gui->output_entry));
  gint error = SPLT_OUTPUT_FORMAT_OK;
  mp3splt_set_oformat(ui->mp3splt_state, data, &error);
  remove_status_message(ui->gui);
  print_status_bar_confirmation(error, ui->gui);

  save_preferences(NULL, NULL);
  
  return FALSE;
}

//!Create the box the output file name is displayed in
GtkWidget *create_output_filename_box()
{
  GtkWidget *vbox = wh_vbox_new();

  //default/custom radio buttons
  GtkWidget *radio_output = gtk_radio_button_new_with_label(NULL, _("Default format"));
  ui->gui->radio_output = radio_output;
  gtk_box_pack_start(GTK_BOX(vbox), radio_output, FALSE, FALSE, 0);

  radio_output = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(radio_output), _("Custom format"));
  ui->gui->radio_output = radio_output;
  gtk_box_pack_start(GTK_BOX(vbox), radio_output, FALSE, FALSE, 0);

  //output entry
  GtkWidget *horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(vbox), horiz_fake, FALSE, FALSE, 5);

  GtkWidget *output_entry = gtk_entry_new();
  ui->gui->output_entry = output_entry;
  gtk_editable_set_editable(GTK_EDITABLE(output_entry), TRUE);
  g_signal_connect(G_OBJECT(output_entry), "key_release_event", G_CALLBACK(output_entry_event), NULL);
  gtk_entry_set_max_length(GTK_ENTRY(output_entry),244);
  gtk_box_pack_start(GTK_BOX(horiz_fake), output_entry, TRUE, TRUE, 0);

  //output label
  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(vbox), horiz_fake, FALSE, FALSE, 5);
  GtkWidget *output_label = gtk_label_new(_("    @f - file name\n"
        "    @a - artist name\n"
        "    @p - performer of each song (does not"
        " always exist)\n"
        "    @b - album title\n"
        "    @t - song title\n"
        "    @g - genre\n"
        "    @n - track number"));
  ui->gui->output_label = output_label;
  gtk_box_pack_start(GTK_BOX(horiz_fake), output_label, FALSE, FALSE, 0);

  g_signal_connect(GTK_TOGGLE_BUTTON(ui->gui->radio_output),
      "toggled", G_CALLBACK(output_radio_box_event), output_label);

  return wh_set_title_and_get_vbox(vbox, _("<b>Output filename format</b>"));
}

//!creates the output preferences page
GtkWidget *create_pref_output_page()
{
  GtkWidget *output_hbox = wh_hbox_new();;
  GtkWidget *output_inside_hbox = wh_hbox_new();;
  
  GtkWidget *scrolled_window = wh_create_scrolled_window();
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), 
                                        GTK_WIDGET(output_inside_hbox));
  gtk_box_pack_start(GTK_BOX(output_hbox), scrolled_window, TRUE, TRUE, 0);
 
  GtkWidget *vbox = wh_vbox_new();;
  gtk_box_pack_start(GTK_BOX(output_inside_hbox), vbox, TRUE, TRUE, 5);

  GtkWidget *output_fname_box = create_output_filename_box();
  gtk_box_pack_start(GTK_BOX(vbox), output_fname_box, FALSE, FALSE, 2);

  return output_hbox;
}

void change_tags_options(GtkToggleButton *button, gpointer data)
{
  if (ui->gui->extract_tags_box != NULL)
  {
    if (rh_get_active_value(ui->gui->tags_radio) == TAGS_FROM_FILENAME)
    {
      gtk_widget_set_sensitive(ui->gui->extract_tags_box, SPLT_TRUE);
    }
    else
    {
      gtk_widget_set_sensitive(ui->gui->extract_tags_box, SPLT_FALSE);
    }
  }

  save_preferences(NULL, NULL);
}

//!Create the box for the Tags options
GtkWidget *create_tags_options_box()
{
  GtkWidget *vbox = wh_vbox_new();

  GtkWidget *tags_radio = NULL;
  tags_radio = rh_append_radio_to_vbox(tags_radio, _("Original file tags"),
      ORIGINAL_FILE_TAGS, change_tags_options, vbox);
  tags_radio = rh_append_radio_to_vbox(tags_radio, _("Default tags (cddb or cue tags)"),
      DEFAULT_TAGS, change_tags_options, vbox);
  tags_radio = rh_append_radio_to_vbox(tags_radio, _("No tags"),
      NO_TAGS, change_tags_options, vbox);
  tags_radio = rh_append_radio_to_vbox(tags_radio, _("Extract tags from filename"),
      TAGS_FROM_FILENAME, change_tags_options, vbox);
  ui->gui->tags_radio = tags_radio;

  GtkWidget *extract_tags_box = create_extract_tags_from_filename_options_box();
  ui->gui->extract_tags_box = extract_tags_box;
  gtk_widget_set_sensitive(extract_tags_box, SPLT_FALSE);
  gtk_box_pack_start(GTK_BOX(vbox), extract_tags_box, FALSE, FALSE, 2);

  return wh_set_title_and_get_vbox(vbox, _("<b>Split files tags</b>"));
}

static GtkComboBox *create_genre_combo()
{
  GtkComboBox *combo = ch_new_combo();

  int i = 0;
  for (i = 0;i < SPLT_ID3V1_NUMBER_OF_GENRES;i++)
  {
    ch_append_to_combo(combo, splt_id3v1_genres[i], 0);
  }

  g_signal_connect(G_OBJECT(combo), "changed", G_CALLBACK(save_preferences), NULL);

  return combo;
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

void test_regex_event(GtkWidget *widget, gpointer data)
{
  put_tags_from_filename_regex_options(ui);

  const gchar *test_regex_filename = gtk_entry_get_text(GTK_ENTRY(ui->gui->test_regex_fname_entry));
  mp3splt_set_filename_to_split(ui->mp3splt_state, test_regex_filename);

  gint error = SPLT_OK;
  splt_tags *tags = mp3splt_parse_filename_regex(ui->mp3splt_state, &error);
  print_status_bar_confirmation(error, ui->gui);

  if (error >= 0)
  {
    GString *regex_result = g_string_new(NULL);

    g_string_append(regex_result, _("<artist>: "));
    if (tags->artist)
    {
      g_string_append(regex_result, tags->artist);
    }
    g_string_append(regex_result, "\n");

    g_string_append(regex_result, _("<album>: "));
    if (tags->album)
    {
      g_string_append(regex_result, tags->album);
    }
    g_string_append(regex_result, "\n");


    g_string_append(regex_result, _("<title>: "));
    if (tags->title)
    {
      g_string_append(regex_result, tags->title);
    }
    g_string_append(regex_result, "\n");

    g_string_append(regex_result, _("<genre>: "));
    if (tags->genre)
    {
      g_string_append(regex_result, tags->genre);
    }
    g_string_append(regex_result, "\n");

    g_string_append(regex_result, _("<comment>: "));
    if (tags->comment)
    {
      g_string_append(regex_result, tags->comment);
    }
    g_string_append(regex_result, "\n");

    g_string_append(regex_result, _("<year>: "));
    if (tags->year)
    {
      g_string_append(regex_result, tags->year);
    }
    g_string_append(regex_result, "\n");

    g_string_append(regex_result, _("<track>: "));
    if (tags->track >= 0)
    {
      g_string_append_printf(regex_result, "%d", tags->track);
    }

    gchar *regex_result_text = g_string_free(regex_result, FALSE);
    if (regex_result_text)
    {
      gtk_label_set_text(GTK_LABEL(ui->gui->sample_result_label), regex_result_text);
      g_free(regex_result_text);
    }
  }
  else
  {
    gtk_label_set_text(GTK_LABEL(ui->gui->sample_result_label), "");
  }

  mp3splt_free_one_tag(tags);
}

static GtkWidget *create_extract_tags_from_filename_options_box()
{
  GtkWidget *table = wh_new_table();

  GtkWidget *regex_entry = wh_new_entry(save_preferences);
  ui->gui->regex_entry = regex_entry;
  wh_add_in_table_with_label_expand(table, _("Regular expression:"), regex_entry);

  GtkWidget *regex_label = gtk_label_new(_(
        "Above enter PERL-like regular expression using named subgroups.\nFollowing names are recognized:\n"
        "    (?<artist>)   - artist name\n"
        "    (?<album>)    - album title\n"
        "    (?<title>)    - track title\n"
        "    (?<tracknum>) - current track number\n"
        //"    (?<tracks>)   - total number of tracks\n"
        "    (?<year>)     - year of emission\n"
        "    (?<genre>)    - genre\n"
        "    (?<comment>)  - comment"));
  gtk_misc_set_alignment(GTK_MISC(regex_label), 0.0, 0.5);
  wh_add_in_table(table, wh_put_in_new_hbox_with_margin_level(regex_label, 2));

  ui_infos *infos = ui->infos;
 
  infos->text_options_list =
    g_list_append(infos->text_options_list, GINT_TO_POINTER(SPLT_NO_CONVERSION));
  infos->text_options_list =
    g_list_append(infos->text_options_list, GINT_TO_POINTER(SPLT_TO_LOWERCASE));
  infos->text_options_list =
    g_list_append(infos->text_options_list, GINT_TO_POINTER(SPLT_TO_UPPERCASE));
  infos->text_options_list =
    g_list_append(infos->text_options_list, GINT_TO_POINTER(SPLT_TO_FIRST_UPPERCASE));
  infos->text_options_list =
    g_list_append(infos->text_options_list, GINT_TO_POINTER(SPLT_TO_WORD_FIRST_UPPERCASE));

  GtkWidget *replace_underscore_by_space_check_box =
    gtk_check_button_new_with_mnemonic(_("_Replace underscores by spaces"));
  ui->gui->replace_underscore_by_space_check_box = replace_underscore_by_space_check_box;
  g_signal_connect(G_OBJECT(replace_underscore_by_space_check_box), "toggled",
      G_CALLBACK(save_preferences), NULL);
 
  wh_add_in_table(table, replace_underscore_by_space_check_box);

  GtkComboBox *artist_text_properties_combo = create_text_preferences_combo();
  ui->gui->artist_text_properties_combo = artist_text_properties_combo;
  wh_add_in_table_with_label(table, 
      _("Artist text properties:"), GTK_WIDGET(artist_text_properties_combo));

  GtkComboBox *album_text_properties_combo = create_text_preferences_combo();
  ui->gui->album_text_properties_combo = album_text_properties_combo;
  wh_add_in_table_with_label(table,
      _("Album text properties:"), GTK_WIDGET(album_text_properties_combo));

  GtkComboBox *title_text_properties_combo = create_text_preferences_combo();
  ui->gui->title_text_properties_combo = title_text_properties_combo;
  wh_add_in_table_with_label(table,
      _("Title text properties:"), GTK_WIDGET(title_text_properties_combo));

  GtkComboBox *comment_text_properties_combo = create_text_preferences_combo();
  ui->gui->comment_text_properties_combo = comment_text_properties_combo;
  wh_add_in_table_with_label(table,
      _("Comment text properties:"), GTK_WIDGET(comment_text_properties_combo));

  GtkComboBox *genre_combo = create_genre_combo();
  ui->gui->genre_combo = genre_combo;
  wh_add_in_table_with_label(table, _("Genre tag:"), GTK_WIDGET(genre_combo));

  GtkWidget *comment_tag_entry = wh_new_entry(save_preferences);
  ui->gui->comment_tag_entry = comment_tag_entry;
  wh_add_in_table_with_label_expand(table, _("Comment tag:"), comment_tag_entry);

  GtkWidget *test_regex_expander = gtk_expander_new(_("Regular expression test"));
  gtk_container_add(GTK_CONTAINER(test_regex_expander), create_test_regex_table());
  wh_add_in_table(table, test_regex_expander);

  return wh_put_in_new_hbox_with_margin_level(GTK_WIDGET(table), 3);
}

static GtkWidget *create_test_regex_table()
{
  GtkWidget *table = wh_new_table();

  GtkWidget *sample_test_hbox = wh_hbox_new();
  GtkWidget *test_regex_fname_entry = wh_new_entry(save_preferences);
  ui->gui->test_regex_fname_entry = test_regex_fname_entry;
  gtk_box_pack_start(GTK_BOX(sample_test_hbox), test_regex_fname_entry, TRUE, TRUE, 0);

  GtkWidget *test_regex_button = wh_new_button(_("_Test"));
  gtk_box_pack_start(GTK_BOX(sample_test_hbox), test_regex_button, FALSE, FALSE, 5);
  g_signal_connect(G_OBJECT(test_regex_button), "clicked",
      G_CALLBACK(test_regex_event), NULL);

  wh_add_in_table_with_label_expand(table, _("Sample filename:"), sample_test_hbox);

  GtkWidget *sample_result_label = gtk_label_new("");
  ui->gui->sample_result_label = sample_result_label;
  gtk_misc_set_alignment(GTK_MISC(ui->gui->sample_result_label), 0.0, 0.5);
  wh_add_in_table_with_label_expand(table, _("Sample result:"), ui->gui->sample_result_label);

  return wh_put_in_new_hbox_with_margin_level(GTK_WIDGET(table), 3);
}

//!Create the box we can select with if to create ID1 and/or ID2 Tags
GtkWidget *create_tags_version_box()
{
  GtkWidget *vbox = wh_vbox_new();

  GtkWidget *tags_version_radio = gtk_radio_button_new_with_label(NULL, _("ID3v1 & ID3v2 tags"));
  ui->gui->tags_version_radio = tags_version_radio;
  gtk_box_pack_start(GTK_BOX(vbox), tags_version_radio, FALSE, FALSE, 0);
  g_signal_connect(GTK_TOGGLE_BUTTON(tags_version_radio), "toggled", 
      G_CALLBACK(save_preferences), NULL);

  tags_version_radio = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(tags_version_radio), _("ID3v2 tags"));
  ui->gui->tags_version_radio = tags_version_radio;
  gtk_box_pack_start(GTK_BOX(vbox), tags_version_radio, FALSE, FALSE, 0);
  g_signal_connect(GTK_TOGGLE_BUTTON(tags_version_radio), "toggled", 
      G_CALLBACK(save_preferences), NULL);

  tags_version_radio = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(tags_version_radio), _("ID3v1 tags"));
  ui->gui->tags_version_radio = tags_version_radio;
  g_signal_connect(GTK_TOGGLE_BUTTON(tags_version_radio), "toggled", 
      G_CALLBACK(save_preferences), NULL);
  gtk_box_pack_start(GTK_BOX(vbox), tags_version_radio, FALSE, FALSE, 0);

  tags_version_radio = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON (tags_version_radio),_("Same tags version as the input file"));
  ui->gui->tags_version_radio = tags_version_radio;
  g_signal_connect(GTK_TOGGLE_BUTTON(tags_version_radio), "toggled", 
      G_CALLBACK(save_preferences), NULL);
  gtk_box_pack_start(GTK_BOX(vbox), tags_version_radio, FALSE, FALSE, 0);

  return wh_set_title_and_get_vbox(vbox, _("<b>Tags version (mp3 only)</b>"));
}

//! Create the tags settings tab
GtkWidget *create_pref_tags_page()
{
  GtkWidget *outside_vbox = wh_vbox_new();;
  GtkWidget *inside_hbox = wh_hbox_new();

  GtkWidget *scrolled_window = wh_create_scrolled_window();
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), 
                                        GTK_WIDGET(inside_hbox));
  gtk_box_pack_start(GTK_BOX(outside_vbox), scrolled_window, TRUE, TRUE, 0);

  GtkWidget *vbox = wh_vbox_new();;
  gtk_box_pack_start(GTK_BOX(inside_hbox), vbox, TRUE, TRUE, 5);

  GtkWidget *tags_version_box = create_tags_version_box();
  gtk_box_pack_start(GTK_BOX(vbox), tags_version_box, FALSE, FALSE, 2);

  GtkWidget *tags_opts_box = create_tags_options_box();
  gtk_box_pack_start(GTK_BOX(vbox), tags_opts_box, FALSE, FALSE, 1);
  
  return outside_vbox;
}

//!creates the preferences tab
GtkWidget *create_choose_preferences(ui_state *ui)
{
  GtkWidget *pref_vbox = wh_vbox_new();

  GtkWidget *notebook = gtk_notebook_new();
  gtk_box_pack_start(GTK_BOX(pref_vbox), notebook, TRUE, TRUE, 0);

  gtk_notebook_popup_enable(GTK_NOTEBOOK(notebook));
  gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), TRUE);
  gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), FALSE);
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);

  /* split preferences */
  GtkWidget *splitpoints_prefs = create_pref_splitpoints_page();
  GtkWidget *notebook_label = gtk_label_new(_("Split"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), splitpoints_prefs, notebook_label);

  /* tags preferences */
  GtkWidget *tags_prefs = create_pref_tags_page();
  notebook_label = gtk_label_new(_("Tags"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), tags_prefs, notebook_label);

  /* output preferences */
  GtkWidget *output_prefs = create_pref_output_page();
  notebook_label = gtk_label_new(_("Output"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), output_prefs, notebook_label);

  /* player preferences */
  GtkWidget *player_prefs = create_pref_player_page();
  notebook_label = gtk_label_new(_("Player"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), player_prefs, notebook_label);

  /* language preferences page */
#ifdef __WIN32__
  GtkWidget *language_prefs = create_pref_language_page();
  notebook_label = gtk_label_new(_("Language"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), language_prefs, notebook_label);
#endif

  return pref_vbox;
}

