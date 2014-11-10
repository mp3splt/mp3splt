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
 * along with this program; if not, write to the Free Software
 * You should have received a copy of the GNU General Public License
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
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

#include "preferences_window.h"

static GtkWidget *create_extract_tags_from_filename_options_box(ui_state *ui);
static GtkWidget *create_test_regex_table(ui_state *ui);
static void update_wave_preview_label_markup(gint index, gint interpolation_level, ui_state *ui);

/*!Returns the selected language

must be free() the result after using it.
*/
GString *get_checked_language(ui_state *ui)
{
  GSList *radio_button_list = gtk_radio_button_get_group(GTK_RADIO_BUTTON(ui->gui->radio_button));

  //0 = german, 1 = french, 2 = english
  GtkWidget *our_button = GTK_WIDGET(g_slist_nth_data(radio_button_list, 0));
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(our_button)))
  {
    return g_string_new("de_DE");
  }

  our_button = GTK_WIDGET(g_slist_nth_data(radio_button_list, 1));
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(our_button)))
  {
    return g_string_new("fr_FR");
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

  return 0;
}

static void set_output_directory(gchar *dirname, ui_state *ui)
{
  if (dirname == NULL)
  {
    return;
  }

  lock_mutex(&ui->variables_mutex);
  if (ui->infos->outputdirname != NULL)
  {
    g_string_free(ui->infos->outputdirname, TRUE);
  }
  ui->infos->outputdirname = g_string_new(dirname);
  unlock_mutex(&ui->variables_mutex);
}

static void change_output_dir_options(GtkToggleButton *button, gpointer data)
{
  ui_state *ui = (ui_state *)data;

  GtkWidget *dir_file_chooser_button = ui->gui->custom_dir_file_chooser_button;
  if (!dir_file_chooser_button || !ui->gui->example_output_dir_box)
  {
    return;
  }

  if (rh_get_active_value(ui->gui->output_dir_radio) == CUSTOM_DIRECTORY)
  {
    gchar *directory = gtk_file_chooser_get_current_folder(GTK_FILE_CHOOSER(dir_file_chooser_button));
    set_output_directory(directory, ui);
    gtk_widget_set_sensitive(dir_file_chooser_button, SPLT_TRUE);
    gtk_widget_set_sensitive(ui->gui->example_output_dir_box, SPLT_FALSE);
  }
  else
  {
    set_output_directory("", ui);
    gtk_widget_set_sensitive(dir_file_chooser_button, SPLT_FALSE);
    gtk_widget_set_sensitive(ui->gui->example_output_dir_box, SPLT_TRUE);
  }

  ui_save_preferences(NULL, ui);
}

static void update_output_directory_in_gui(ui_state *ui, char *output_dir)
{
  GtkWidget *custom_dir_file_chooser_button = ui->gui->custom_dir_file_chooser_button;

  if (output_dir == NULL || output_dir[0] == '\0')
  {
    rh_set_radio_value(ui->gui->output_dir_radio, FILE_DIRECTORY, TRUE);
  }
  else
  {
    rh_set_radio_value(ui->gui->output_dir_radio, CUSTOM_DIRECTORY, TRUE);
    gtk_file_chooser_set_filename(GTK_FILE_CHOOSER(custom_dir_file_chooser_button), output_dir);
  }

  change_output_dir_options(GTK_TOGGLE_BUTTON(ui->gui->output_dir_radio), ui);
}

void set_output_directory_and_update_ui(gchar *dirname, ui_state *ui)
{
  if (dirname == NULL)
  {
    return;
  }

  set_output_directory(dirname, ui);

  update_output_directory_in_gui(ui, dirname);
}

/*! Get the name of the output directory

\return 
 - The name of the output directory, if a directory is set.
 - NULL, otherwise.
*/
gchar *get_output_directory(ui_state *ui)
{
  if (ui->infos->outputdirname != NULL)
  {
    return ui->infos->outputdirname->str;
  }

  return NULL;
}

//!cddb and cue output mode radio box event
static void output_radio_box_event(GtkToggleButton *radio_b, ui_state *ui)
{
  gint selected = get_checked_output_radio_box(ui);
  if (selected == 0)
  {
    gtk_widget_set_sensitive(ui->gui->output_entry, TRUE);
    gtk_widget_set_sensitive(ui->gui->output_label, TRUE);
    gtk_widget_set_sensitive(ui->gui->output_default_label, FALSE);
  }
  else
  {
    gtk_widget_set_sensitive(ui->gui->output_entry, FALSE);
    gtk_widget_set_sensitive(ui->gui->output_label, FALSE);
    gtk_widget_set_sensitive(ui->gui->output_default_label, TRUE);
  }

  ui_save_preferences(NULL, ui);
}

#ifdef __WIN32__

//! Create the "select language" box
static GtkWidget *create_language_box(ui_state *ui)
{
  GtkWidget *radio_vbox = wh_vbox_new();

  GtkWidget *radio_button = gtk_radio_button_new_with_label(NULL, "English");
  ui->gui->radio_button = radio_button;
  g_signal_connect(GTK_TOGGLE_BUTTON(radio_button), "toggled",
      G_CALLBACK(ui_save_preferences), ui);
  gtk_box_pack_start(GTK_BOX(radio_vbox), radio_button, TRUE, TRUE, 0);

  radio_button = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(radio_button), "Français");
  ui->gui->radio_button = radio_button;
  g_signal_connect(GTK_TOGGLE_BUTTON(radio_button), "toggled",
      G_CALLBACK(ui_save_preferences), ui);
  gtk_box_pack_start(GTK_BOX(radio_vbox), radio_button, TRUE, TRUE, 0);

  radio_button = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(radio_button), "Deutsch");
  ui->gui->radio_button = radio_button;
  g_signal_connect(GTK_TOGGLE_BUTTON (radio_button), "toggled",
      G_CALLBACK(ui_save_preferences), ui);
  gtk_box_pack_start(GTK_BOX(radio_vbox), radio_button, TRUE, TRUE, 0);

  return wh_set_title_and_get_vbox(radio_vbox,
      _("<b>Choose language (requires restart)</b>"));
}

//! Creates the language preferences page
static GtkWidget *create_pref_language_page(ui_state *ui)
{
  GtkWidget *language_hbox = wh_hbox_new();;
  GtkWidget *language_inside_hbox = wh_hbox_new();;

  GtkWidget *scrolled_window = wh_create_scrolled_window();
  wh_add_box_to_scrolled_window(language_inside_hbox, scrolled_window);
  gtk_box_pack_start(GTK_BOX(language_hbox), scrolled_window, TRUE, TRUE, 0);

  GtkWidget *vbox = wh_vbox_new();;
  gtk_box_pack_start(GTK_BOX(language_inside_hbox), vbox, TRUE, TRUE, 10);

  GtkWidget *lang_box = create_language_box(ui);
  gtk_box_pack_start(GTK_BOX(vbox), lang_box, FALSE, FALSE, 10);

  return language_hbox;
}
#endif

//!disables adjust parameters
static void disable_adjust_parameters(gui_state *gui)
{
  gtk_widget_set_sensitive(gui->adjust_param_vbox, FALSE);
}

//!enables adjust parameters
static void enable_adjust_parameters(gui_state *gui)
{
  gtk_widget_set_sensitive(gui->adjust_param_vbox, TRUE);
}

//!adjust event
static void adjust_event(GtkToggleButton *adjust_mode, ui_state *ui)
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(adjust_mode)))
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ui->gui->frame_mode),TRUE);
    enable_adjust_parameters(ui->gui);
  }
  else
  {
    disable_adjust_parameters(ui->gui);
  }

  ui_save_preferences(NULL, ui);
}

//!frame mode event
static void frame_event(GtkToggleButton *frame_mode, ui_state *ui)
{
  if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(frame_mode)))
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ui->gui->adjust_mode), FALSE);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ui->gui->bit_reservoir_mode), FALSE);
  }

  ui_save_preferences(NULL, ui);
}

static void splitpoints_from_filename_event(GtkToggleButton *frame_mode, ui_state *ui)
{
  gint splitpoints_from_filename = gtk_toggle_button_get_active(ui->gui->names_from_filename);
  if (splitpoints_from_filename == TRUE && ui->status->file_browsed == TRUE)
  {
    copy_filename_to_current_description(get_input_filename(ui->gui), ui);
  }
  else
  {
    clear_current_description(ui);
  }

  ui_save_preferences(NULL, ui);
}

//!action for the set default prefs button
static void set_default_prefs_event(GtkWidget *widget, ui_state *ui)
{
  gui_state *gui = ui->gui;

  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui->frame_mode), FALSE);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui->bit_reservoir_mode), FALSE);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui->adjust_mode), FALSE);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(gui->spinner_adjust_threshold),
      SPLT_DEFAULT_PARAM_THRESHOLD);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(gui->spinner_adjust_offset),
      SPLT_DEFAULT_PARAM_OFFSET);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(gui->spinner_adjust_gap),
      SPLT_DEFAULT_PARAM_GAP);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(gui->spinner_adjust_min),
      SPLT_DEFAULT_PARAM_MINIMUM_LENGTH);
  gtk_toggle_button_set_active(gui->names_from_filename, FALSE);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(gui->create_dirs_from_output_files), TRUE);

  ui_save_preferences(NULL, ui);
}

static void custom_directory_changed(GtkFileChooser *custom_dir_file_chooser, ui_state *ui)
{
  gchar *filename = gtk_file_chooser_get_filename(custom_dir_file_chooser);
  set_output_directory(filename, ui);
  ui_save_preferences(NULL, ui);
}

static GtkWidget *create_custom_directory_box(ui_state *ui)
{
  GtkWidget *custom_dir_file_chooser_button =
    gtk_file_chooser_button_new(_("Browse directory ..."), GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER);
  wh_set_browser_directory_handler(ui, custom_dir_file_chooser_button);

  g_signal_connect(G_OBJECT(custom_dir_file_chooser_button), "selection-changed",
      G_CALLBACK(custom_directory_changed), ui);

  ui->gui->custom_dir_file_chooser_button = custom_dir_file_chooser_button;

  GtkWidget *hbox = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(hbox), custom_dir_file_chooser_button, TRUE, TRUE, 20);

  return hbox;
}

static GtkWidget *create_input_file_directory_example_box(ui_state *ui)
{
  GtkWidget *vbox = wh_vbox_new();

  GtkWidget *example_output_dir_label = gtk_label_new(_("Example for the single file split:"));
  GtkWidget *fake_hbox = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(fake_hbox), example_output_dir_label, FALSE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(vbox), fake_hbox, FALSE, FALSE, 0);

  GtkWidget *example_output_dir_entry = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(example_output_dir_entry), FALSE);
  gtk_box_pack_start(GTK_BOX(vbox), example_output_dir_entry, TRUE, TRUE, 5);
  ui->gui->example_output_dir_entry = example_output_dir_entry;

  GtkWidget *hbox_for_margin = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(hbox_for_margin), vbox, TRUE, TRUE, 20);

  ui->gui->example_output_dir_box = hbox_for_margin;

  return hbox_for_margin;
}

void update_example_output_dir_for_single_file_split(ui_state *ui)
{
  if (!ui->gui->example_output_dir_entry)
  {
    return;
  }

  gchar *dirname = g_path_get_dirname(get_input_filename(ui->gui));
  gtk_entry_set_text(GTK_ENTRY(ui->gui->example_output_dir_entry), dirname);
  g_free(dirname);
}

//!Creates the box the output directory can be choosen in
static GtkWidget *create_directory_box(ui_state *ui)
{
  gui_state *gui = ui->gui;

  GtkWidget *vbox = wh_vbox_new();

  GtkWidget *output_dir_radio = NULL;
  output_dir_radio = rh_append_radio_to_vbox(output_dir_radio, _("Custom directory"),
      CUSTOM_DIRECTORY, change_output_dir_options, ui, vbox);
  gtk_widget_set_tooltip_text(output_dir_radio, _("Create split files in a custom directory"));

  GtkWidget *custom_dir_box = create_custom_directory_box(ui);
  gtk_box_pack_start(GTK_BOX(vbox), custom_dir_box, FALSE, FALSE, 0);

  output_dir_radio = rh_append_radio_to_vbox(output_dir_radio, _("Input file directory"),
      FILE_DIRECTORY, change_output_dir_options, ui, vbox);
  gtk_widget_set_tooltip_text(output_dir_radio,
      _("Create split files in the same directory as the file being split"));
  gui->output_dir_radio = output_dir_radio;

  GtkWidget *input_file_directory_example_box = create_input_file_directory_example_box(ui);
  gtk_box_pack_start(GTK_BOX(vbox), input_file_directory_example_box, FALSE, FALSE, 0);

  return wh_set_title_and_get_vbox(vbox, _("<b>Directory for split files</b>"));
}

static void bit_reservoir_event(GtkToggleButton *bit_reservoir, ui_state *ui)
{
  gint with_bit_reservoir = 
    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(ui->gui->bit_reservoir_mode));
  if (with_bit_reservoir == TRUE)
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(ui->gui->frame_mode), TRUE);
  }

  ui_save_preferences(NULL, ui);
}

//! Creates the box for split mode selection
static GtkWidget *create_split_options_box(ui_state *ui)
{
  gui_state *gui = ui->gui;

  GtkWidget *vbox = wh_vbox_new();

  //names from filename
  GtkToggleButton *names_from_filename = 
    GTK_TOGGLE_BUTTON(gtk_check_button_new_with_mnemonic(
          _("_Splitpoint name from filename (manual single file split only)")));
  gtk_widget_set_tooltip_text(GTK_WIDGET(names_from_filename),
      _("Name newly added splitpoints as the input file"));
  gui->names_from_filename = names_from_filename;

  gtk_box_pack_start(GTK_BOX(vbox), GTK_WIDGET(names_from_filename), FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(names_from_filename), "toggled",
      G_CALLBACK(splitpoints_from_filename_event), ui);

  GtkWidget *create_dirs_from_output_files =
    gtk_check_button_new_with_mnemonic(_("Create directories from _filenames "));

  gtk_widget_set_tooltip_text(create_dirs_from_output_files,
      _("If the splitpoint name is 'a/b/output', the directory chain 'a/b' is created in the"
        " output\ndirectory and the file 'output.<extension>' is written in the"
        " '<output_directory>/a/b' directory"));
  gui->create_dirs_from_output_files = create_dirs_from_output_files;
  gtk_box_pack_start(GTK_BOX(vbox), create_dirs_from_output_files, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(create_dirs_from_output_files), "toggled",
      G_CALLBACK(ui_save_preferences), ui);

  //frame mode option
  GtkWidget *frame_mode =
    gtk_check_button_new_with_mnemonic(_("F_rame mode (useful for mp3 VBR) (mp3 only)"));
  gtk_widget_set_tooltip_text(frame_mode,
      _("The split is slower with this option, but some mp3 files having\n"
        "Variable Bit Rate need this mode to be enabled"));
  gui->frame_mode = frame_mode;
  gtk_box_pack_start(GTK_BOX(vbox), frame_mode, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(frame_mode), "toggled", G_CALLBACK(frame_event), ui);

  //bit reservoir handling option
  GtkWidget *bit_reservoir_mode =
    gtk_check_button_new_with_mnemonic(_("_[Experimental] Bit reservoir handling for gapless playback (mp3 only)"));
  gtk_widget_set_tooltip_text(bit_reservoir_mode,
      _("Split files will play gapless only on players "
        "supporting the LAME tag delay and padding values\n"
        "Gapless players examples: cmus, mpg123, foobar2000"));
  gui->bit_reservoir_mode = bit_reservoir_mode;
  gtk_box_pack_start(GTK_BOX(vbox), bit_reservoir_mode, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(bit_reservoir_mode), "toggled", G_CALLBACK(bit_reservoir_event), ui);

  //auto adjust option
  GtkWidget *adjust_mode = gtk_check_button_new_with_mnemonic(_("_Auto-adjust mode (use"
        " silence detection to auto-adjust splitpoints)"));
  gtk_widget_set_tooltip_text(adjust_mode,
      _("Splitpoints will be adjusted to match silences (if found)\n"
        "This mode requires the frame mode"));
  gui->adjust_mode = adjust_mode;
  gtk_box_pack_start(GTK_BOX(vbox), adjust_mode, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(adjust_mode), "toggled", G_CALLBACK(adjust_event), ui);

  //parameters for the adjust option
  GtkWidget *horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(vbox), horiz_fake, FALSE, FALSE, 0);

  GtkWidget *param_vbox = wh_vbox_new();
  gui->adjust_param_vbox = param_vbox;
  gtk_box_pack_start(GTK_BOX(horiz_fake), param_vbox, FALSE, FALSE, 25);

  //threshold level
  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(param_vbox), horiz_fake, FALSE, FALSE, 0);

  GtkWidget *threshold_label = gtk_label_new(_("Threshold level (dB):"));
  gtk_box_pack_start(GTK_BOX(horiz_fake), threshold_label, FALSE, FALSE, 0);

  GtkAdjustment *adj =
    (GtkAdjustment *) gtk_adjustment_new(0.0, -96.0, 0.0, 0.5, 10.0, 0.0);
  GtkWidget *spinner_adjust_threshold = gtk_spin_button_new (adj, 0.5, 2);
  gui->spinner_adjust_threshold = spinner_adjust_threshold;
  g_signal_connect(G_OBJECT(spinner_adjust_threshold), "value_changed",
      G_CALLBACK(ui_save_preferences), ui);
  gtk_box_pack_start(GTK_BOX(horiz_fake), spinner_adjust_threshold,
      FALSE, FALSE, 6);

  //min level
  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(param_vbox), horiz_fake, FALSE, FALSE, 0);

  GtkWidget *min_label = gtk_label_new(_("Minimum silence length (seconds):"));
  gtk_box_pack_start(GTK_BOX(horiz_fake), min_label, FALSE, FALSE, 0);

  adj = (GtkAdjustment *)gtk_adjustment_new(0.0, 0, 2000, 0.5, 10.0, 0.0);
  GtkWidget *spinner_adjust_min = gtk_spin_button_new(adj, 1, 2);
  gui->spinner_adjust_min = spinner_adjust_min;
  g_signal_connect(G_OBJECT(spinner_adjust_min), "value_changed",
      G_CALLBACK(ui_save_preferences), ui);
  gtk_box_pack_start(GTK_BOX(horiz_fake), spinner_adjust_min,
      FALSE, FALSE, 6);

  //offset level
  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(param_vbox), horiz_fake, FALSE, FALSE, 0);

  GtkWidget *offset_label = gtk_label_new(_("Cutpoint offset (0 is the begin of silence "
        "and 1 the end):"));
  gtk_box_pack_start(GTK_BOX(horiz_fake), offset_label, FALSE, FALSE, 0);

  //adjustement for the offset spinner
  adj = (GtkAdjustment *)gtk_adjustment_new(0.0, -2, 2, 0.05, 10.0, 0.0);
  GtkWidget *spinner_adjust_offset = gtk_spin_button_new (adj, 0.05, 2);
  gui->spinner_adjust_offset = spinner_adjust_offset;
  g_signal_connect(G_OBJECT(spinner_adjust_offset), "value_changed",
      G_CALLBACK(ui_save_preferences), ui);
  gtk_box_pack_start(GTK_BOX(horiz_fake), spinner_adjust_offset, FALSE, FALSE, 6);

  //gap level (seconds)
  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(param_vbox), horiz_fake, FALSE, FALSE, 0);

  GtkWidget *gap_label = 
    gtk_label_new(_("Gap level (seconds around splitpoint to search for silence):"));
  gtk_box_pack_start(GTK_BOX(horiz_fake), gap_label, FALSE, FALSE, 0);

  adj = (GtkAdjustment *) gtk_adjustment_new(0.0, 0, 2000, 1.0, 10.0, 0.0);
  GtkWidget *spinner_adjust_gap = gtk_spin_button_new (adj, 1, 0);
  gui->spinner_adjust_gap = spinner_adjust_gap;
  g_signal_connect(G_OBJECT(spinner_adjust_gap), "value_changed",
      G_CALLBACK(ui_save_preferences), ui);
  gtk_box_pack_start(GTK_BOX(horiz_fake), spinner_adjust_gap, FALSE, FALSE, 6);

  disable_adjust_parameters(ui->gui);

  //set default preferences button
  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(vbox), horiz_fake, FALSE, FALSE, 0);

  GtkWidget *set_default_prefs_button =
    wh_create_cool_button("document-properties", _("Set _default split options"),FALSE); 
  g_signal_connect(G_OBJECT(set_default_prefs_button), "clicked",
      G_CALLBACK(set_default_prefs_event), ui);
  gtk_box_pack_start (GTK_BOX (horiz_fake), set_default_prefs_button, FALSE, FALSE, 5);

  return wh_set_title_and_get_vbox(vbox, _("<b>Split options</b>"));
}

//!creates the splitpoints preferences page
static GtkWidget *create_pref_splitpoints_page(ui_state *ui)
{
  GtkWidget *general_hbox = wh_hbox_new();
  GtkWidget *inside_hbox = wh_hbox_new();

  GtkWidget *scrolled_window = wh_create_scrolled_window();
  wh_add_box_to_scrolled_window(inside_hbox, scrolled_window);
  gtk_box_pack_start(GTK_BOX(general_hbox), scrolled_window, TRUE, TRUE, 0);

  GtkWidget *inside_vbox = wh_vbox_new();
  gtk_box_pack_start(GTK_BOX(inside_hbox), inside_vbox, TRUE, TRUE, 5);

  GtkWidget *dir_box = create_directory_box(ui);
  gtk_box_pack_start(GTK_BOX(inside_vbox), dir_box, FALSE, FALSE, 2);

  GtkWidget *split_options_box = create_split_options_box(ui);
  gtk_box_pack_start(GTK_BOX(inside_vbox), split_options_box, FALSE, FALSE, 1);

  return general_hbox;
}

//!event when changing the combo box player
static void player_combo_box_event(GtkComboBox *widget, ui_state *ui)
{
  disconnect_button_event(ui->gui->disconnect_button, ui);

  ui->infos->selected_player = ch_get_active_value(widget);
  if (ui->infos->selected_player == PLAYER_GSTREAMER)
  {
    hide_connect_button(ui->gui);
    gtk_widget_show(ui->gui->playlist_box);
    gtk_widget_set_sensitive(ui->gui->gstreamer_stop_before_end_box, TRUE);
  }
  else
  {
    show_connect_button(ui->gui);
    gtk_widget_hide(ui->gui->playlist_box);
    gtk_widget_set_sensitive(ui->gui->gstreamer_stop_before_end_box, FALSE);
  }

  gtk_widget_show(ui->gui->player_box);
  gtk_widget_show(ui->gui->queue_files_button);

  ui_save_preferences(NULL, ui);
}

static void update_timeout_value(GtkWidget *spinner, ui_state *ui)
{
  ui->infos->timeout_value = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinner));

  restart_player_timer(ui);
  ui_save_preferences(NULL, ui);
}

static void update_gstreamer_stop_before_end_value(GtkWidget *spinner, ui_state *ui)
{
  ui->infos->gstreamer_stop_before_end = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinner));
  ui_save_preferences(NULL, ui);
}

static void update_small_seek_jump_value(GtkWidget *spinner, ui_state *ui)
{
  ui->infos->small_seek_jump_value = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinner));
  ui_save_preferences(NULL, ui);
}

static void update_seek_jump_value(GtkWidget *spinner, ui_state *ui)
{
  ui->infos->seek_jump_value = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinner));
  ui_save_preferences(NULL, ui);
}

static void update_big_seek_jump_value(GtkWidget *spinner, ui_state *ui)
{
  ui->infos->big_seek_jump_value = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinner));
  ui_save_preferences(NULL, ui);
}

//!Create the box the player backend can be selected with
static GtkWidget *create_player_options_box(ui_state *ui)
{
  GtkWidget *vbox = wh_vbox_new();
  GtkWidget *horiz_fake = wh_hbox_new();

  GtkWidget *label = gtk_label_new(_("Player:"));
  gtk_box_pack_start(GTK_BOX(horiz_fake), label, FALSE, FALSE, 0);

  GtkWidget *player_combo_box = GTK_WIDGET(ch_new_combo());
  ui->gui->player_combo_box = player_combo_box;

#ifndef NO_AUDACIOUS
  ch_append_to_combo(GTK_COMBO_BOX(player_combo_box), "Audacious", PLAYER_AUDACIOUS);
#endif
  ch_append_to_combo(GTK_COMBO_BOX(player_combo_box), "SnackAmp", PLAYER_SNACKAMP);
#ifndef NO_GSTREAMER
  ch_append_to_combo(GTK_COMBO_BOX(player_combo_box), "GStreamer", PLAYER_GSTREAMER);
#endif

  g_signal_connect(G_OBJECT(player_combo_box), "changed", G_CALLBACK(player_combo_box_event), ui);

  gtk_box_pack_start(GTK_BOX(horiz_fake), player_combo_box, FALSE, FALSE, 5);
  gtk_box_pack_start(GTK_BOX(vbox), horiz_fake, FALSE, FALSE, 0);

  GtkWidget *spinner = wh_create_int_spinner_in_box(_("Refresh player every "),
      _("milliseconds."),
      (gdouble)DEFAULT_TIMEOUT_VALUE, 20.0, 1000.0, 10.0, 100.0,
      _("\t(higher refresh rate decreases CPU usage - default is 200)"),
      update_timeout_value, ui, vbox);
  ui_register_spinner_int_preference("player", "refresh_rate", DEFAULT_TIMEOUT_VALUE,
      spinner, (void (*)(GtkWidget *, gpointer)) update_timeout_value,
      ui, ui);

  GtkWidget *gstreamer_vbox = wh_vbox_new();
  ui->gui->gstreamer_stop_before_end_box = gstreamer_vbox;

#ifndef NO_GSTREAMER
  GtkWidget *gstreamer_stop_before_end =
    wh_create_int_spinner_in_box(_("Stop GStreamer preview"), _("milliseconds before the end."),
        (gdouble)DEFAULT_GSTREAMER_STOP_BEFORE_END_VALUE, 0.0, 1000.0, 50.0, 100.0,
        NULL, update_gstreamer_stop_before_end_value, ui, gstreamer_vbox);
  ui_register_spinner_int_preference("player", "gstreamer_stop_before_end",
      DEFAULT_GSTREAMER_STOP_BEFORE_END_VALUE, gstreamer_stop_before_end,
      (void (*)(GtkWidget *, gpointer)) update_gstreamer_stop_before_end_value, ui, ui);

  gtk_box_pack_start(GTK_BOX(vbox), gstreamer_vbox, TRUE, TRUE, 0);
#endif

  //Seek times

  GtkWidget *seek_vbox = wh_vbox_new(); 

  GtkWidget *small_seek_jump = wh_create_int_spinner_in_box(_("Small seek jumps for "),
      _("milliseconds."),
      (gdouble)DEFAULT_SMALL_SEEK_JUMP_VALUE, 0.0, (gdouble)G_MAXINT, 100.0, 1000.0,
      NULL,
      update_small_seek_jump_value, ui, seek_vbox);
  ui_register_spinner_int_preference("player", "small_seek_jump", DEFAULT_SMALL_SEEK_JUMP_VALUE,
      small_seek_jump, (void (*)(GtkWidget *, gpointer)) update_small_seek_jump_value,
      ui, ui);

  GtkWidget *seek_jump = wh_create_int_spinner_in_box(_("Seek jumps for "),
      _("milliseconds. (0=auto)"),
      (gdouble)DEFAULT_SEEK_JUMP_VALUE, 0.0, (gdouble)G_MAXINT, 1000.0, 10000.0,
      NULL,
      update_seek_jump_value, ui, seek_vbox);
  ui_register_spinner_int_preference("player", "seek_jump", DEFAULT_SEEK_JUMP_VALUE,
      seek_jump, (void (*)(GtkWidget *, gpointer)) update_seek_jump_value,
      ui, ui);

  GtkWidget *big_seek_jump = wh_create_int_spinner_in_box(_("Big seek jumps for "),
      _("milliseconds. (0=auto)"),
      (gdouble)DEFAULT_BIG_SEEK_JUMP_VALUE, 0.0, (gdouble)G_MAXINT, 1000.0, 60000.0,
      NULL,
      update_big_seek_jump_value, ui, seek_vbox);
  ui_register_spinner_int_preference("player", "big_seek_jump", DEFAULT_BIG_SEEK_JUMP_VALUE,
      big_seek_jump, (void (*)(GtkWidget *, gpointer)) update_big_seek_jump_value,
      ui, ui);

  GtkWidget *hbox_for_margin = wh_put_in_new_hbox(seek_vbox, 3, FALSE, FALSE);

  GtkWidget *seek_times_frame = gtk_frame_new(_("Seek times"));
  gtk_container_add(GTK_CONTAINER(seek_times_frame), hbox_for_margin);

  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(horiz_fake), seek_times_frame, FALSE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(vbox), horiz_fake, FALSE, FALSE, 0);

  return wh_set_title_and_get_vbox(vbox, _("<b>Player options</b>"));
}

static void wave_quality_changed_event(GtkAdjustment *wave_quality_adjustment, ui_state *ui)
{
  gint quality_level = (gint)gtk_adjustment_get_value(GTK_ADJUSTMENT(wave_quality_adjustment));

  gint level = 0;
  for (level = 0; level <= 5; level++)
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

  ui_save_preferences(NULL, ui);
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

static gint get_wave_preview_width_drawing_area(ui_state *ui)
{
  if (ui->infos->width_drawing_area < 50)
  {
    return 500;
  }

  return ui->infos->width_drawing_area;
}

static gboolean wave_quality_draw_event(GtkWidget *drawing_area, cairo_t *cairo_surface,
    preview_index_and_data *data)
{
  ui_state *ui = data->data;
  gint index = data->index;

  gint width = get_wave_preview_width_drawing_area(ui);
  gtk_widget_set_size_request(drawing_area, width, 70);

  gint expected_drawing_time_int = g_array_index(ui->infos->preview_time_windows, gint, index);
  gfloat expected_drawing_time = (gfloat)(expected_drawing_time_int);

  dh_set_white_color(cairo_surface);

  dh_draw_rectangle(cairo_surface, TRUE, 0, 0, width, 70); 
  gfloat current_time = ui->infos->total_time / 2.0;

  gfloat drawing_time = 0;
  gfloat zoom_coeff = 0.2;

  gfloat left_time = 0;
  gfloat right_time = 0;
  while ((((gint)drawing_time) == 0) || (drawing_time > expected_drawing_time))
  {
    left_time = get_left_drawing_time(current_time, ui->infos->total_time, zoom_coeff);
    right_time = get_right_drawing_time(current_time, ui->infos->total_time, zoom_coeff);
    drawing_time = right_time - left_time;
    zoom_coeff += 0.01;

    if (zoom_coeff > 100) { break; }
  }

  ui->infos->drawing_preferences_silence_wave = SPLT_TRUE;

  gint interpolation_level = draw_silence_wave((gint)left_time, (gint)right_time, width / 2, 50,
      drawing_time, width, 0,
      current_time, ui->infos->total_time, zoom_coeff,
      drawing_area, cairo_surface, ui);

  ui->infos->drawing_preferences_silence_wave = SPLT_FALSE;

  update_wave_preview_label_markup(index, interpolation_level, ui);

  return TRUE;
}

static GtkWidget *create_wave_quality_preview_box(ui_state *ui)
{
  GtkWidget *vbox = wh_vbox_new();

  GtkWidget *label_hbox = wh_hbox_new();
  GtkWidget *wave_preview_label = gtk_label_new(NULL);

  gchar wave_availability[256] = { '\0' };
  g_snprintf(wave_availability, 256, "<span style='italic' color='#0000AA'>%s</span>",
      _("Only available if the amplitude wave is shown in the player"));
  gtk_label_set_markup(GTK_LABEL(wave_preview_label), wave_availability);
  gtk_box_pack_start(GTK_BOX(label_hbox), wave_preview_label, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), label_hbox, FALSE, FALSE, 4);

  ui->gui->wave_quality_das = g_ptr_array_new();
  ui->gui->wave_preview_labels = g_ptr_array_new();

  gint i = 0;
  for (i = 0; i < ui->infos->preview_time_windows->len; i++)
  {
    GtkWidget *wave_quality_da = gtk_drawing_area_new(); 
    g_ptr_array_add(ui->gui->wave_quality_das, (gpointer)wave_quality_da);
    ui->infos->preview_indexes[i].index = i;
    ui->infos->preview_indexes[i].data = ui;

    g_signal_connect(wave_quality_da, "draw", G_CALLBACK(wave_quality_draw_event),
        &ui->infos->preview_indexes[i]);

    wh_put_in_hbox_and_attach_to_vbox(wave_quality_da, vbox, 0);

    GtkWidget *minutes_label = gtk_label_new(NULL);
    g_ptr_array_add(ui->gui->wave_preview_labels, minutes_label);
    update_wave_preview_label_markup(i, -1, ui);

    wh_put_in_hbox_and_attach_to_vbox_with_bottom_margin(minutes_label, vbox, 0, 4);
  }

  GtkWidget *hbox_for_margin = wh_put_in_new_hbox(vbox, 6, FALSE, FALSE);

  GtkWidget *wave_preview_frame = gtk_frame_new(_("Wave preview"));
  gtk_container_add(GTK_CONTAINER(wave_preview_frame), hbox_for_margin);
  return wh_put_in_new_hbox(wave_preview_frame, 0, FALSE, FALSE);
}

static void update_wave_preview_label_markup(gint index, gint interpolation_level, ui_state *ui)
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

  GtkWidget *text_label = g_ptr_array_index(ui->gui->wave_preview_labels, index);
  gtk_label_set_markup(GTK_LABEL(text_label), final_text_with_color);
}

static GtkWidget *create_wave_options_box(ui_state *ui)
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
      wave_quality_hscale, (void (*)(GtkAdjustment *,gpointer))wave_quality_changed_event, ui, ui);

  GtkAdjustment *wave_quality_adjustment = gtk_range_get_adjustment(GTK_RANGE(wave_quality_hscale));
  g_signal_connect(G_OBJECT(wave_quality_adjustment), "value-changed",
      G_CALLBACK(wave_quality_changed_event), ui);

  gtk_box_pack_start(GTK_BOX(vbox), range_hbox, FALSE, FALSE, 0);

  GtkWidget *wave_quality_box = create_wave_quality_preview_box(ui);
  gtk_box_pack_start(GTK_BOX(vbox), wave_quality_box, FALSE, FALSE, 0);

  return wh_set_title_and_get_vbox(vbox, _("<b>Amplitude wave options</b>"));
}

//!creates the player preferences page
static GtkWidget *create_pref_player_page(ui_state *ui)
{
  GtkWidget *player_hbox = wh_hbox_new();;
  GtkWidget *inside_hbox = wh_hbox_new();;

  GtkWidget *inside_vbox = wh_vbox_new();;
  gtk_box_pack_start(GTK_BOX(inside_hbox), inside_vbox, TRUE, TRUE, 5);

  GtkWidget *player_scrolled_window = wh_create_scrolled_window();
  ui->gui->player_scrolled_window = player_scrolled_window;
  wh_add_box_to_scrolled_window(inside_hbox, player_scrolled_window);
  gtk_box_pack_start(GTK_BOX(player_hbox), player_scrolled_window, TRUE, TRUE, 0);

  GtkWidget *vbox = wh_vbox_new();;
  gtk_box_pack_start(GTK_BOX(inside_vbox), vbox, TRUE, TRUE, 0);

  GtkWidget *player_options_box = create_player_options_box(ui);
  gtk_box_pack_start(GTK_BOX(vbox), player_options_box, FALSE, FALSE, 3);

  GtkWidget *wave_options_box = create_wave_options_box(ui);
  gtk_box_pack_start(GTK_BOX(vbox), wave_options_box, FALSE, FALSE, 3);

  return player_hbox;
}

static gboolean check_output_format_end(ui_with_err *ui_err)
{
  ui_state *ui = ui_err->ui;

  remove_status_message(ui->gui);
  print_status_bar_confirmation_in_idle(ui_err->err, ui);

  set_process_in_progress_and_wait_safe(FALSE, ui_err->ui);

  g_free(ui_err);

  return FALSE;
}

static gpointer check_output_format_thread(ui_with_fname *ui_fname)
{
  ui_state *ui = ui_fname->ui;

  set_process_in_progress_and_wait_safe(TRUE, ui);

  gint error = mp3splt_set_oformat(ui->mp3splt_state, ui_fname->fname);

  ui_with_err *ui_err = g_malloc0(sizeof(ui_with_err));
  ui_err->err = error;
  ui_err->ui = ui;

  add_idle(G_PRIORITY_HIGH_IDLE, (GSourceFunc)check_output_format_end, ui_err, NULL);

  g_free(ui_fname->fname);
  g_free(ui_fname);

  return NULL;
}

//!update the save buttons on an output entry event
static gboolean output_entry_event(GtkWidget *widget, GdkEventKey *event, ui_state *ui)
{
  const char *data = gtk_entry_get_text(GTK_ENTRY(ui->gui->output_entry));
  if (data)
  {
    ui_with_fname *ui_fname = g_malloc0(sizeof(ui_with_fname));
    ui_fname->ui = ui;
    ui_fname->fname = strdup(data);

    create_thread_and_unref((GThreadFunc)check_output_format_thread,
        (gpointer) ui_fname, ui, "check_output_format");
  }

  ui_save_preferences(NULL, ui);

  return FALSE;
}

//!Create the box the output file name is displayed in
static GtkWidget *create_output_filename_box(ui_state *ui)
{
  gui_state *gui = ui->gui;

  GtkWidget *vbox = wh_vbox_new();

  GtkWidget *horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(vbox), horiz_fake, FALSE, FALSE, 5);

  //default/custom radio buttons
  GtkWidget *radio_output = gtk_radio_button_new_with_label(NULL, _("Default format"));
  gui->radio_output = radio_output;
  gtk_box_pack_start(GTK_BOX(vbox), radio_output, FALSE, FALSE, 0);

  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(vbox), horiz_fake, FALSE, FALSE, 5);

  GString *outputs_str = g_string_new(_("  Default output: "));
  g_string_append(outputs_str, "<span color='#222288'>");
  g_string_append(outputs_str, SPLT_DEFAULT_OUTPUT);
  g_string_append(outputs_str, "</span>");

  g_string_append(outputs_str, "\n");
  g_string_append(outputs_str, _("  CDDB, CUE and tracktype.org default: "));
  g_string_append(outputs_str, "<span color='#222288'>");
  g_string_append(outputs_str, SPLT_DEFAULT_CDDB_CUE_OUTPUT);
  g_string_append(outputs_str, "</span>");

  g_string_append(outputs_str, "\n");
  g_string_append(outputs_str, _("  Split with silence detection default: "));
  g_string_append(outputs_str, "<span color='#222288'>");
  g_string_append(outputs_str, SPLT_DEFAULT_SILENCE_OUTPUT);
  g_string_append(outputs_str, "</span>");

  g_string_append(outputs_str, "\n");
  g_string_append(outputs_str, _("  Trim using silence detection default: "));
  g_string_append(outputs_str, "<span color='#222288'>");
  g_string_append(outputs_str, SPLT_DEFAULT_TRIM_SILENCE_OUTPUT);
  g_string_append(outputs_str, "</span>");

  g_string_append(outputs_str, "\n");
  g_string_append(outputs_str, _("  Error mode default: "));
  g_string_append(outputs_str, "<span color='#222288'>");
  g_string_append(outputs_str, SPLT_DEFAULT_SYNCERROR_OUTPUT);
  g_string_append(outputs_str, "</span>");

  GtkWidget *default_label = gtk_label_new(NULL);
  gui->output_default_label = default_label;
  gtk_label_set_markup(GTK_LABEL(default_label), outputs_str->str);
  gtk_label_set_selectable(GTK_LABEL(default_label), TRUE);
  gtk_box_pack_start(GTK_BOX(horiz_fake), default_label, FALSE, FALSE, 0);

  g_string_free(outputs_str, TRUE);

  //second radio button
  radio_output = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(radio_output), _("Custom format"));
  gui->radio_output = radio_output;
  gtk_box_pack_start(GTK_BOX(vbox), radio_output, FALSE, FALSE, 0);

  //output entry
  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(vbox), horiz_fake, FALSE, FALSE, 5);

  GtkWidget *output_entry = gtk_entry_new();
  gui->output_entry = output_entry;
  gtk_editable_set_editable(GTK_EDITABLE(output_entry), TRUE);
  g_signal_connect(G_OBJECT(output_entry), "key_release_event", G_CALLBACK(output_entry_event), ui);
  gtk_entry_set_max_length(GTK_ENTRY(output_entry),244);
  gtk_box_pack_start(GTK_BOX(horiz_fake), output_entry, TRUE, TRUE, 0);

  //output label
  horiz_fake = wh_hbox_new();
  gtk_box_pack_start(GTK_BOX(vbox), horiz_fake, FALSE, FALSE, 5);
  GtkWidget *output_label = gtk_label_new(_(
        "  @A: performer if found, otherwise artist\n"
        "  @a: artist name\n"
        "  @p: performer of each song (only with .cue)\n"
        "  @b: album title\n"
        "  @g: genre\n"
        "  @t: song title\n"
        "  @n: track number identifier (not the real ID3 track number) **\n"
        "  @N: track tag number **\n"
        "  @l: track number identifier as lowercase letter (not the real ID3 track number) **\n"
        "  @L: track tag number as lowercase letter **\n"
        "  @u: track number identifier as uppercase letter (not the real ID3 track number) **\n"
        "  @U: track tag number as uppercase letter **\n"
        "  @f: input filename (without extension)\n"
        "  @d: last directory of the input filename or the filename itself if no directory\n"
        "  @m, @s or @h: the number of minutes, seconds or hundreths of seconds of the start splitpoint **\n"
        "  @M, @S or @H: the number of minutes, seconds or hundreths of seconds of the end splitpoint **\n"
        "\n"
        "    (**) a digit may follow for the number of digits to output\n"));
  gtk_label_set_selectable(GTK_LABEL(output_label), TRUE);
  gui->output_label = output_label;
  gtk_box_pack_start(GTK_BOX(horiz_fake), output_label, FALSE, FALSE, 0);

  g_signal_connect(GTK_TOGGLE_BUTTON(gui->radio_output),
      "toggled", G_CALLBACK(output_radio_box_event), ui);

  return wh_set_title_and_get_vbox(vbox,
      _("<b>Output format for batch split and when importing splitpoints</b>"));
}

//!creates the output preferences page
static GtkWidget *create_pref_output_page(ui_state *ui)
{
  GtkWidget *output_hbox = wh_hbox_new();;
  GtkWidget *output_inside_hbox = wh_hbox_new();;

  GtkWidget *scrolled_window = wh_create_scrolled_window();
  wh_add_box_to_scrolled_window(output_inside_hbox, scrolled_window);
  gtk_box_pack_start(GTK_BOX(output_hbox), scrolled_window, TRUE, TRUE, 0);

  GtkWidget *vbox = wh_vbox_new();;
  gtk_box_pack_start(GTK_BOX(output_inside_hbox), vbox, TRUE, TRUE, 5);

  GtkWidget *output_fname_box = create_output_filename_box(ui);
  gtk_box_pack_start(GTK_BOX(vbox), output_fname_box, FALSE, FALSE, 2);

  return output_hbox;
}

static void change_tags_options(GtkToggleButton *button, gpointer data)
{
  ui_state *ui = (ui_state *)data;

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

  ui_save_preferences(NULL, ui);
}

//!Create the box for the Tags options
static GtkWidget *create_tags_options_box(ui_state *ui)
{
  gui_state *gui = ui->gui;

  GtkWidget *vbox = wh_vbox_new();

  GtkWidget *tags_radio = NULL;
  tags_radio = rh_append_radio_to_vbox(tags_radio, _("Original file tags"),
      ORIGINAL_FILE_TAGS, change_tags_options, ui, vbox);
  gui->tags_radio = tags_radio;
  tags_radio = rh_append_radio_to_vbox(tags_radio, _("Custom tags (from the splitpoints table)"),
      DEFAULT_TAGS, change_tags_options, ui, vbox);
  gui->tags_radio = tags_radio;
  tags_radio = rh_append_radio_to_vbox(tags_radio, _("No tags"),
      NO_TAGS, change_tags_options, ui, vbox);
  gui->tags_radio = tags_radio;
  tags_radio = rh_append_radio_to_vbox(tags_radio, _("Extract tags from filename"),
      TAGS_FROM_FILENAME, change_tags_options, ui, vbox);
  gui->tags_radio = tags_radio;

  GtkWidget *extract_tags_box = create_extract_tags_from_filename_options_box(ui);
  gui->extract_tags_box = extract_tags_box;
  gtk_widget_set_sensitive(extract_tags_box, SPLT_FALSE);
  gtk_box_pack_start(GTK_BOX(vbox), extract_tags_box, FALSE, FALSE, 2);

  return wh_set_title_and_get_vbox(vbox, _("<b>Split files tags</b>"));
}

static GtkComboBox *create_genre_combo(ui_state *ui)
{
  GtkComboBox *combo = ch_new_combo();

  int i = 0;
  for (i = 0;i < SPLT_ID3V1_NUMBER_OF_GENRES;i++)
  {
    ch_append_to_combo(combo, splt_id3v1_genres[i], 0);
  }

  g_signal_connect(G_OBJECT(combo), "changed", G_CALLBACK(ui_save_preferences), ui);

  return combo;
}

static GtkComboBox *create_text_preferences_combo(ui_state *ui)
{
  GtkComboBox *combo = ch_new_combo();

  ch_append_to_combo(combo, _("No change"), SPLT_NO_CONVERSION);
  ch_append_to_combo(combo, _("lowercase"), SPLT_TO_LOWERCASE);
  ch_append_to_combo(combo, _("UPPERCASE"), SPLT_TO_UPPERCASE);
  ch_append_to_combo(combo, _("First uppercase"), SPLT_TO_FIRST_UPPERCASE);
  ch_append_to_combo(combo, _("Word Uppercase"), SPLT_TO_WORD_FIRST_UPPERCASE);

  g_signal_connect(G_OBJECT(combo), "changed", G_CALLBACK(ui_save_preferences), ui);

  return combo;
}

static gboolean test_regex_end(ui_with_fname *ui_fname)
{
  ui_state *ui = ui_fname->ui;

  if (ui_fname->fname)
  {
    gtk_label_set_text(GTK_LABEL(ui->gui->sample_result_label), ui_fname->fname);
    g_free(ui_fname->fname);
  }
  else
  {
    gtk_label_set_text(GTK_LABEL(ui->gui->sample_result_label), "");
  }

  g_free(ui_fname);

  set_process_in_progress_and_wait_safe(FALSE, ui);

  return FALSE;
}

static gpointer test_regex_thread(ui_for_split *ui_fs)
{
  ui_state *ui = ui_fs->ui;

  set_process_in_progress_and_wait_safe(TRUE, ui);

  put_tags_from_filename_regex_options(ui_fs);

  mp3splt_set_filename_to_split(ui->mp3splt_state, ui_fs->test_regex_filename);

  gint error = SPLT_OK;
  splt_tags *tags = mp3splt_parse_filename_regex(ui->mp3splt_state, &error);
  print_status_bar_confirmation_in_idle(error, ui);

  if (error < 0) { goto end; }

  GString *regex_result = g_string_new(NULL);

  g_string_append(regex_result, _("<artist>: "));
  char *artist = mp3splt_tags_get(tags, SPLT_TAGS_ARTIST);
  if (artist)
  {
    g_string_append(regex_result, artist);
    free(artist);
  }
  g_string_append(regex_result, "\n");

  g_string_append(regex_result, _("<album>: "));
  char *album = mp3splt_tags_get(tags, SPLT_TAGS_ALBUM);
  if (album)
  {
    g_string_append(regex_result, album);
    free(album);
  }
  g_string_append(regex_result, "\n");


  g_string_append(regex_result, _("<title>: "));
  char *title = mp3splt_tags_get(tags, SPLT_TAGS_TITLE);
  if (title)
  {
    g_string_append(regex_result, title);
    free(title);
  }
  g_string_append(regex_result, "\n");

  g_string_append(regex_result, _("<genre>: "));
  char *genre = mp3splt_tags_get(tags, SPLT_TAGS_GENRE);
  if (genre)
  {
    g_string_append(regex_result, genre);
    free(genre);
  }
  g_string_append(regex_result, "\n");

  g_string_append(regex_result, _("<comment>: "));
  char *comment = mp3splt_tags_get(tags, SPLT_TAGS_COMMENT);
  if (comment)
  {
    g_string_append(regex_result, comment);
    free(comment);
  }
  g_string_append(regex_result, "\n");

  g_string_append(regex_result, _("<year>: "));
  char *year = mp3splt_tags_get(tags, SPLT_TAGS_YEAR);
  if (year)
  {
    g_string_append(regex_result, year);
    free(year);
  }
  g_string_append(regex_result, "\n");

  g_string_append(regex_result, _("<track>: "));
  gchar *track = mp3splt_tags_get(tags, SPLT_TAGS_TRACK);
  if (track)
  {
    g_string_append(regex_result, track);
    free(track);
  }

end:
  mp3splt_free_one_tag(tags);
  free_ui_for_split(ui_fs);

  ui_with_fname *ui_fname = g_malloc0(sizeof(ui_with_fname));
  ui_fname->ui = ui;
  ui_fname->fname = g_string_free(regex_result, FALSE);

  add_idle(G_PRIORITY_HIGH_IDLE, (GSourceFunc)test_regex_end, ui_fname, NULL);

  return NULL;
}

static void test_regex_event(GtkWidget *widget, ui_state *ui)
{
  ui_for_split *ui_fs = build_ui_for_split(ui);

  const gchar *test_regex_filename = gtk_entry_get_text(GTK_ENTRY(ui->gui->test_regex_fname_entry));
  if (test_regex_filename != NULL)
  {
    ui_fs->test_regex_filename = g_strdup(test_regex_filename);
  }

  create_thread_and_unref((GThreadFunc)test_regex_thread, (gpointer) ui_fs, ui, "test_regex");
}

static GtkWidget *create_extract_tags_from_filename_options_box(ui_state *ui)
{
  GtkWidget *table = wh_new_table();

  GtkWidget *regex_entry = wh_new_entry(ui_save_preferences, ui);
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
  gtk_label_set_selectable(GTK_LABEL(regex_label), TRUE);
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
      G_CALLBACK(ui_save_preferences), ui);
 
  wh_add_in_table(table, replace_underscore_by_space_check_box);

  GtkComboBox *artist_text_properties_combo = create_text_preferences_combo(ui);
  ui->gui->artist_text_properties_combo = artist_text_properties_combo;
  wh_add_in_table_with_label(table, 
      _("Artist text properties:"), GTK_WIDGET(artist_text_properties_combo));

  GtkComboBox *album_text_properties_combo = create_text_preferences_combo(ui);
  ui->gui->album_text_properties_combo = album_text_properties_combo;
  wh_add_in_table_with_label(table,
      _("Album text properties:"), GTK_WIDGET(album_text_properties_combo));

  GtkComboBox *title_text_properties_combo = create_text_preferences_combo(ui);
  ui->gui->title_text_properties_combo = title_text_properties_combo;
  wh_add_in_table_with_label(table,
      _("Title text properties:"), GTK_WIDGET(title_text_properties_combo));

  GtkComboBox *comment_text_properties_combo = create_text_preferences_combo(ui);
  ui->gui->comment_text_properties_combo = comment_text_properties_combo;
  wh_add_in_table_with_label(table,
      _("Comment text properties:"), GTK_WIDGET(comment_text_properties_combo));

  GtkComboBox *genre_combo = create_genre_combo(ui);
  ui->gui->genre_combo = genre_combo;
  wh_add_in_table_with_label(table, _("Genre tag:"), GTK_WIDGET(genre_combo));

  GtkWidget *comment_tag_entry = wh_new_entry(ui_save_preferences, ui);
  ui->gui->comment_tag_entry = comment_tag_entry;
  wh_add_in_table_with_label_expand(table, _("Comment tag:"), comment_tag_entry);

  GtkWidget *test_regex_expander = gtk_expander_new(_("Regular expression test"));
  gtk_container_add(GTK_CONTAINER(test_regex_expander), create_test_regex_table(ui));
  wh_add_in_table(table, test_regex_expander);

  return wh_put_in_new_hbox_with_margin_level(GTK_WIDGET(table), 3);
}

static GtkWidget *create_test_regex_table(ui_state *ui)
{
  GtkWidget *table = wh_new_table();

  GtkWidget *sample_test_hbox = wh_hbox_new();
  GtkWidget *test_regex_fname_entry = wh_new_entry(ui_save_preferences, ui);
  ui->gui->test_regex_fname_entry = test_regex_fname_entry;
  gtk_box_pack_start(GTK_BOX(sample_test_hbox), test_regex_fname_entry, TRUE, TRUE, 0);

  GtkWidget *test_regex_button = wh_new_button(_("_Test"));
  gtk_box_pack_start(GTK_BOX(sample_test_hbox), test_regex_button, FALSE, FALSE, 5);
  g_signal_connect(G_OBJECT(test_regex_button), "clicked", G_CALLBACK(test_regex_event), ui);

  wh_add_in_table_with_label_expand(table, _("Sample filename:"), sample_test_hbox);

  GtkWidget *sample_result_label = gtk_label_new("");
  ui->gui->sample_result_label = sample_result_label;
  gtk_misc_set_alignment(GTK_MISC(ui->gui->sample_result_label), 0.0, 0.5);
  wh_add_in_table_with_label_expand(table, _("Sample result:"), ui->gui->sample_result_label);

  return wh_put_in_new_hbox_with_margin_level(GTK_WIDGET(table), 3);
}

//!Create the box we can select with if to create ID1 and/or ID2 Tags
static GtkWidget *create_tags_version_box(ui_state *ui)
{
  gui_state *gui = ui->gui;

  GtkWidget *vbox = wh_vbox_new();

  GtkWidget *tags_version_radio = gtk_radio_button_new_with_label(NULL, _("ID3v1 & ID3v2 tags"));
  gui->tags_version_radio = tags_version_radio;
  gtk_box_pack_start(GTK_BOX(vbox), tags_version_radio, FALSE, FALSE, 0);
  g_signal_connect(GTK_TOGGLE_BUTTON(tags_version_radio), "toggled", 
      G_CALLBACK(ui_save_preferences), ui);

  tags_version_radio = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(tags_version_radio), _("ID3v2 tags"));
  gui->tags_version_radio = tags_version_radio;
  gtk_box_pack_start(GTK_BOX(vbox), tags_version_radio, FALSE, FALSE, 0);
  g_signal_connect(GTK_TOGGLE_BUTTON(tags_version_radio), "toggled", 
      G_CALLBACK(ui_save_preferences), ui);

  tags_version_radio = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(tags_version_radio), _("ID3v1 tags"));
  gui->tags_version_radio = tags_version_radio;
  g_signal_connect(GTK_TOGGLE_BUTTON(tags_version_radio), "toggled", 
      G_CALLBACK(ui_save_preferences), ui);
  gtk_box_pack_start(GTK_BOX(vbox), tags_version_radio, FALSE, FALSE, 0);

  tags_version_radio = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON (tags_version_radio),_("Same tags version as the input file"));
  gui->tags_version_radio = tags_version_radio;
  g_signal_connect(GTK_TOGGLE_BUTTON(tags_version_radio), "toggled", 
      G_CALLBACK(ui_save_preferences), ui);
  gtk_box_pack_start(GTK_BOX(vbox), tags_version_radio, FALSE, FALSE, 0);

  return wh_set_title_and_get_vbox(vbox, _("<b>Tags version (mp3 only)</b>"));
}

//! Create the tags settings tab
static GtkWidget *create_pref_tags_page(ui_state *ui)
{
  GtkWidget *outside_vbox = wh_vbox_new();;
  GtkWidget *inside_hbox = wh_hbox_new();

  GtkWidget *scrolled_window = wh_create_scrolled_window();
  wh_add_box_to_scrolled_window(inside_hbox, scrolled_window);
  gtk_box_pack_start(GTK_BOX(outside_vbox), scrolled_window, TRUE, TRUE, 0);

  GtkWidget *vbox = wh_vbox_new();;
  gtk_box_pack_start(GTK_BOX(inside_hbox), vbox, TRUE, TRUE, 5);

  GtkWidget *tags_version_box = create_tags_version_box(ui);
  gtk_box_pack_start(GTK_BOX(vbox), tags_version_box, FALSE, FALSE, 2);

  GtkWidget *tags_opts_box = create_tags_options_box(ui);
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
  GtkWidget *splitpoints_prefs = create_pref_splitpoints_page(ui);
  GtkWidget *notebook_label = gtk_label_new(_("Split"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), splitpoints_prefs, notebook_label);

  /* tags preferences */
  GtkWidget *tags_prefs = create_pref_tags_page(ui);
  notebook_label = gtk_label_new(_("Tags"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), tags_prefs, notebook_label);

  /* player preferences */
  GtkWidget *player_prefs = create_pref_player_page(ui);
  notebook_label = gtk_label_new(_("Player"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), player_prefs, notebook_label);

  /* output preferences */
  GtkWidget *output_prefs = create_pref_output_page(ui);
  notebook_label = gtk_label_new(_("Output format"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), output_prefs, notebook_label);

  /* language preferences page */
#ifdef __WIN32__
  GtkWidget *language_prefs = create_pref_language_page(ui);
  notebook_label = gtk_label_new(_("Language"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), language_prefs, notebook_label);
#endif

  return pref_vbox;
}

