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
 *  Get the current state of all GUI buttons
 *
 * The code contained in this file reads out the current 
 * state of all GUI buttons and saves it into the structure
 * ui->mp3splt_state.
 ********************************************************/

#include "options_manager.h"

/*! Update the output options

  Update the output options in ui->mp3splt_state by reading out the state of
  the GUI controls.

  All other options are read out in put_options_from_preferences()
 */
void update_output_options(ui_state *ui, gboolean is_checked_output_radio_box, gchar *output_format)
{
  if (!is_checked_output_radio_box)
  {
    mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_OUTPUT_FILENAMES, SPLT_OUTPUT_FORMAT);
    gint error = mp3splt_set_oformat(ui->mp3splt_state, output_format);
    print_status_bar_confirmation_in_idle(error, ui);
  }
  else
  {
    mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_OUTPUT_FILENAMES, SPLT_OUTPUT_DEFAULT);
  }
}

ui_for_split *build_ui_for_split(ui_state *ui)
{
  gui_state *gui = ui->gui;

  ui_for_split *ui_fs = g_malloc0(sizeof(ui_for_split));
  ui_fs->ui = ui;

  ui_fs->frame_mode = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui->frame_mode));
  ui_fs->bit_reservoir_mode =
    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui->bit_reservoir_mode));

  ui_fs->adjust_mode = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui->adjust_mode));
  ui_fs->adjust_offset = gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui->spinner_adjust_offset));
  ui_fs->adjust_gap = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(gui->spinner_adjust_gap));
  ui_fs->adjust_threshold =
    gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui->spinner_adjust_threshold));
  ui_fs->adjust_min = gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui->spinner_adjust_min));

  ui_fs->split_file_mode = get_split_file_mode(ui);

  ui_fs->selected_split_mode = get_selected_split_mode(ui);

  ui_fs->time_split_value = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(gui->spinner_time));
  ui_fs->equal_tracks_value = 
    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(gui->spinner_equal_tracks));

  ui_fs->silence_threshold =
    gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui->all_spinner_silence_threshold));
  ui_fs->silence_offset =
    gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui->all_spinner_silence_offset));
  ui_fs->silence_number =
    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(gui->all_spinner_silence_number_tracks));
  ui_fs->silence_minimum_length =
    gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui->all_spinner_silence_minimum));
  ui_fs->silence_minimum_track_length =
    gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui->all_spinner_track_minimum));
  ui_fs->silence_remove =
    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui->all_silence_remove_silence));

  ui_fs->trim_silence_threshold =
    gtk_spin_button_get_value(GTK_SPIN_BUTTON(gui->all_spinner_trim_silence_threshold));

  ui_fs->single_silence_threshold = ui->infos->silence_threshold_value;
  ui_fs->single_silence_offset = ui->infos->silence_offset_value;
  ui_fs->single_silence_number = ui->infos->silence_number_of_tracks;
  ui_fs->single_silence_minimum_length = ui->infos->silence_minimum_length;
  ui_fs->single_silence_minimum_track_length = ui->infos->silence_minimum_track_length;
  ui_fs->single_silence_remove = ui->infos->silence_remove_silence_between_tracks;

  ui_fs->selected_tags_value = rh_get_active_value(gui->tags_radio);
  ui_fs->tags_version = get_checked_tags_version_radio_box(gui);

  ui_fs->create_dirs_from_filenames =
    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui->create_dirs_from_output_files));

  ui_fs->regex_replace_underscores =
    gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(gui->replace_underscore_by_space_check_box));
  ui_fs->regex_artist_tag_format = ch_get_active_value(gui->artist_text_properties_combo);
  ui_fs->regex_album_tag_format = ch_get_active_value(gui->album_text_properties_combo);
  ui_fs->regex_title_tag_format = ch_get_active_value(gui->title_text_properties_combo);
  ui_fs->regex_comment_tag_format = ch_get_active_value(gui->comment_text_properties_combo);

  const gchar *regex = gtk_entry_get_text(GTK_ENTRY(gui->regex_entry));
  if (regex != NULL)
  {
    ui_fs->regex = g_strdup(regex);
  }

  const gchar *regex_default_comment = gtk_entry_get_text(GTK_ENTRY(gui->comment_tag_entry));
  if (regex_default_comment != NULL)
  {
    ui_fs->regex_default_comment = g_strdup(regex_default_comment);
  }

  const gchar *regex_default_genre = ch_get_active_str_value(gui->genre_combo);
  if (regex_default_genre != NULL)
  {
    ui_fs->regex_default_genre = g_strdup(regex_default_genre);
  }

  const gchar *output_format = gtk_entry_get_text(GTK_ENTRY(ui->gui->output_entry));
  if (output_format != NULL)
  {
    ui_fs->output_format = g_strdup(output_format);
  }

  const gchar *output_directory = get_output_directory(ui);
  if (output_directory != NULL)
  {
    ui_fs->output_directory = g_strdup(output_directory);
  }

  ui_fs->is_checked_output_radio_box = get_checked_output_radio_box(ui);

  return ui_fs;
}

void free_ui_for_split(ui_for_split *ui_fs)
{
  if (ui_fs->pat)
  {
    free_points_and_tags(&ui_fs->pat);
  }

  if (ui_fs->regex) { g_free(ui_fs->regex); }
  if (ui_fs->regex_default_comment) { g_free(ui_fs->regex_default_comment); }
  if (ui_fs->regex_default_genre) { g_free(ui_fs->regex_default_genre); }
  if (ui_fs->output_format) { g_free(ui_fs->output_format); }
  if (ui_fs->output_directory) { g_free(ui_fs->output_directory); }
  if (ui_fs->test_regex_filename) { g_free(ui_fs->test_regex_filename); }

  g_free(ui_fs);
}

/*! Update the ui->mp3splt_state structure

  Updates the ui->mp3splt_state structure by reading out the state of the GUI
  controls. The only exception is that all options that are directly
  connected to audio output have been split into a separate function:
  update_output_options(ui)
 */
void put_options_from_preferences(ui_for_split *ui_fs)
{
  ui_state *ui = ui_fs->ui;

  if (ui_fs->frame_mode)
  {
    mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_FRAME_MODE, SPLT_TRUE);
  }
  else
  {
    mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_FRAME_MODE, SPLT_FALSE);
  }

  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_HANDLE_BIT_RESERVOIR,
      ui_fs->bit_reservoir_mode);

  if (ui_fs->adjust_mode)
  {
    mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_AUTO_ADJUST, SPLT_TRUE);
    mp3splt_set_float_option(ui->mp3splt_state, SPLT_OPT_PARAM_OFFSET, ui_fs->adjust_offset);
    mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_PARAM_GAP, ui_fs->adjust_gap); 
    mp3splt_set_float_option(ui->mp3splt_state, SPLT_OPT_PARAM_THRESHOLD, ui_fs->adjust_threshold);
    mp3splt_set_float_option(ui->mp3splt_state, SPLT_OPT_PARAM_MIN_LENGTH, ui_fs->adjust_min);
  }
  else
  {
    mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_AUTO_ADJUST, SPLT_FALSE);
  }

  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_INPUT_NOT_SEEKABLE, SPLT_FALSE);
  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_SPLIT_MODE, SPLT_OPTION_NORMAL_MODE);

  if (ui_fs->split_file_mode == FILE_MODE_SINGLE)
  {
    mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_SPLIT_MODE, SPLT_OPTION_NORMAL_MODE);
  }
  else
  {
    switch (ui_fs->selected_split_mode)
    {
      case SELECTED_SPLIT_NORMAL:
        mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_SPLIT_MODE, SPLT_OPTION_NORMAL_MODE);
        break;
      case SELECTED_SPLIT_WRAP:
        mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_SPLIT_MODE, SPLT_OPTION_WRAP_MODE);
        break;
      case SELECTED_SPLIT_TIME:
        mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_SPLIT_MODE, SPLT_OPTION_TIME_MODE);
        mp3splt_set_long_option(ui->mp3splt_state, SPLT_OPT_SPLIT_TIME,
            ui_fs->time_split_value * 100);
        break;
      case SELECTED_SPLIT_EQUAL_TIME_TRACKS:
        mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_SPLIT_MODE, SPLT_OPTION_LENGTH_MODE);
        mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_LENGTH_SPLIT_FILE_NUMBER,
            ui_fs->equal_tracks_value);
        break;
      case SELECTED_SPLIT_SILENCE:
        mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_SPLIT_MODE, SPLT_OPTION_SILENCE_MODE);
        mp3splt_set_float_option(ui->mp3splt_state, SPLT_OPT_PARAM_THRESHOLD,
            ui_fs->silence_threshold);
        mp3splt_set_float_option(ui->mp3splt_state, SPLT_OPT_PARAM_OFFSET, ui_fs->silence_offset);
        mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_PARAM_NUMBER_TRACKS, 
            ui_fs->silence_number);
        mp3splt_set_float_option(ui->mp3splt_state, SPLT_OPT_PARAM_MIN_LENGTH,
            ui_fs->silence_minimum_length);
        mp3splt_set_float_option(ui->mp3splt_state, SPLT_OPT_PARAM_MIN_TRACK_LENGTH,
            ui_fs->silence_minimum_track_length);
        if (ui_fs->silence_remove)
        {
          mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_PARAM_REMOVE_SILENCE, SPLT_TRUE);
        }
        else
        {
          mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_PARAM_REMOVE_SILENCE, SPLT_FALSE);
        }
        break;
      case SELECTED_SPLIT_TRIM_SILENCE:
        mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_SPLIT_MODE, SPLT_OPTION_TRIM_SILENCE_MODE);
        mp3splt_set_float_option(ui->mp3splt_state, SPLT_OPT_PARAM_THRESHOLD, ui_fs->trim_silence_threshold);
        break;
      case SELECTED_SPLIT_ERROR:
        mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_SPLIT_MODE, SPLT_OPTION_ERROR_MODE);
        break;
      default:
        break;
    }
  }

  int selected_tags_value = ui_fs->selected_tags_value;;
  if (selected_tags_value == NO_TAGS)
  {
    mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_TAGS, SPLT_NO_TAGS);
  }
  else if (selected_tags_value == DEFAULT_TAGS)
  {
    mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_TAGS, SPLT_CURRENT_TAGS);
  }
  else if (selected_tags_value == ORIGINAL_FILE_TAGS)
  {
    mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_TAGS, SPLT_TAGS_ORIGINAL_FILE);
  }
  else if (selected_tags_value == TAGS_FROM_FILENAME)
  {
    put_tags_from_filename_regex_options(ui_fs);
  }

  int tags_radio_choice = ui_fs->tags_version;
  if (tags_radio_choice == 0)
  {
    mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_FORCE_TAGS_VERSION, 0);
  }
  else if (tags_radio_choice == 1)
  {
    mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_FORCE_TAGS_VERSION, 1);
  }
  else if (tags_radio_choice == 2)
  {
    mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_FORCE_TAGS_VERSION, 2);
  }
  else if (tags_radio_choice == 3)
  {
    mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_FORCE_TAGS_VERSION, 12);
  }

  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_CREATE_DIRS_FROM_FILENAMES,
      ui_fs->create_dirs_from_filenames);
}

void put_tags_from_filename_regex_options(ui_for_split *ui_fs)
{
  ui_state *ui = ui_fs->ui;

  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_TAGS, SPLT_TAGS_FROM_FILENAME_REGEX);

  int underscores = ui_fs->regex_replace_underscores;
  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_REPLACE_UNDERSCORES_TAG_FORMAT, underscores);

  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_ARTIST_TAG_FORMAT, 
      ui_fs->regex_artist_tag_format);
  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_ALBUM_TAG_FORMAT, 
      ui_fs->regex_album_tag_format);
  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_TITLE_TAG_FORMAT,
      ui_fs->regex_title_tag_format);
  mp3splt_set_int_option(ui->mp3splt_state, SPLT_OPT_COMMENT_TAG_FORMAT, 
      ui_fs->regex_comment_tag_format);

  mp3splt_set_input_filename_regex(ui->mp3splt_state, ui_fs->regex);

  const gchar *default_comment = ui_fs->regex_default_comment;
  if (strlen(default_comment) == 0)
  {
    default_comment = NULL;
  }
  mp3splt_set_default_comment_tag(ui->mp3splt_state, default_comment);

  mp3splt_set_default_genre_tag(ui->mp3splt_state, ui_fs->regex_default_genre);
}

