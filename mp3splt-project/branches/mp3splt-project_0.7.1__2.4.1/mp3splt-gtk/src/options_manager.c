/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2011 Alexandru Munteanu
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
 *  Get the current state of all GUI buttons
 *
 * The code contained in this file reads out the current 
 * state of all GUI buttons and saves it into the structure
 * the_state.
 ********************************************************/

#include <string.h>

#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include <libmp3splt/mp3splt.h>

#include "util.h"
#include "player_tab.h"
#include "freedb_tab.h"
#include "main_win.h"
#include "preferences_tab.h"
#include "special_split.h"
#include "player_tab.h"
#include "combo_helper.h"
#include "options_manager.h"
#include "radio_helper.h"

extern gint debug_is_active;

extern splt_state *the_state;

extern GtkWidget *output_entry;
extern gchar *filename_to_split;
extern gint selected_split_mode;

extern GtkWidget *frame_mode;
extern GtkWidget *adjust_mode;
extern GtkWidget *spinner_adjust_gap;
extern GtkWidget *spinner_adjust_offset;
extern GtkWidget *spinner_adjust_threshold;
extern GtkWidget *spinner_time;
extern GtkWidget *spinner_equal_tracks;
extern GtkWidget *create_dirs_from_output_files;

extern GtkWidget *all_spinner_silence_number_tracks;
extern GtkWidget *all_spinner_silence_minimum;
extern GtkWidget *all_spinner_track_minimum;
extern GtkWidget *all_spinner_silence_offset;
extern GtkWidget *all_spinner_silence_threshold;
extern GtkWidget *all_spinner_trim_silence_threshold;
extern GtkWidget *all_silence_remove_silence;

extern GList *text_options_list;
extern GtkWidget *replace_underscore_by_space_check_box;
extern GtkComboBox *artist_text_properties_combo;
extern GtkComboBox *album_text_properties_combo;
extern GtkComboBox *title_text_properties_combo;
extern GtkComboBox *comment_text_properties_combo;
extern GtkComboBox *genre_combo;
extern GtkWidget *comment_tag_entry;
extern GtkWidget *regex_entry;

extern GtkWidget *tags_radio;

/*! Update the output options

  Update the output options in the_state by reading out the state of
  the GUI controls.

  All other options are read out in put_options_from_preferences()
 */
void update_output_options()
{
  filename_to_split = inputfilename_get();
  mp3splt_set_filename_to_split(the_state, filename_to_split);

  if (get_checked_output_radio_box() == 0)
  {
    mp3splt_set_int_option(the_state, SPLT_OPT_OUTPUT_FILENAMES,
        SPLT_OUTPUT_FORMAT);

    const char *data = gtk_entry_get_text(GTK_ENTRY(output_entry));
    gint error = SPLT_OUTPUT_FORMAT_OK;
    mp3splt_set_oformat(the_state, data, &error);
    print_status_bar_confirmation(error);
  }
  else
  {
    mp3splt_set_int_option(the_state, SPLT_OPT_OUTPUT_FILENAMES,
        SPLT_OUTPUT_DEFAULT);
  }

  mp3splt_set_int_option(the_state, SPLT_OPT_DEBUG_MODE, debug_is_active);
}

/*! Update the the_state structure

  Updates the the_state structure by reading out the state of the GUI
  controls. The only exception is that all options that are directly
  connected to audio output have been split into a separate function:
  update_output_options()
 */
void put_options_from_preferences()
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(frame_mode)))
  {
    mp3splt_set_int_option(the_state, SPLT_OPT_FRAME_MODE, SPLT_TRUE);
  }
  else
  {
    mp3splt_set_int_option(the_state, SPLT_OPT_FRAME_MODE, SPLT_FALSE);
  }

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(adjust_mode)))
  {
    mp3splt_set_int_option(the_state, SPLT_OPT_AUTO_ADJUST, SPLT_TRUE);
    mp3splt_set_float_option(the_state, SPLT_OPT_PARAM_OFFSET,
        gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinner_adjust_offset)));
    mp3splt_set_int_option(the_state, SPLT_OPT_PARAM_GAP,
        gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinner_adjust_gap)));
    mp3splt_set_float_option(the_state, SPLT_OPT_PARAM_THRESHOLD,
        gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinner_adjust_threshold)));
  }
  else
  {
    mp3splt_set_int_option(the_state, SPLT_OPT_AUTO_ADJUST, SPLT_FALSE);
  }

  mp3splt_set_int_option(the_state, SPLT_OPT_INPUT_NOT_SEEKABLE, SPLT_FALSE);

  mp3splt_set_int_option(the_state, SPLT_OPT_SPLIT_MODE, SPLT_OPTION_NORMAL_MODE);

  switch (selected_split_mode)
  {
    case SELECTED_SPLIT_NORMAL:
      mp3splt_set_int_option(the_state, SPLT_OPT_SPLIT_MODE,
          SPLT_OPTION_NORMAL_MODE);
      break;
    case SELECTED_SPLIT_WRAP:
      mp3splt_set_int_option(the_state, SPLT_OPT_SPLIT_MODE,
          SPLT_OPTION_WRAP_MODE);
      break;
    case SELECTED_SPLIT_TIME:
      mp3splt_set_int_option(the_state, SPLT_OPT_SPLIT_MODE,
          SPLT_OPTION_TIME_MODE);
      mp3splt_set_float_option(the_state, SPLT_OPT_SPLIT_TIME,
          gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinner_time)));
      break;
    case SELECTED_SPLIT_EQUAL_TIME_TRACKS:
      mp3splt_set_int_option(the_state, SPLT_OPT_SPLIT_MODE,
          SPLT_OPTION_LENGTH_MODE);
      mp3splt_set_int_option(the_state, SPLT_OPT_LENGTH_SPLIT_FILE_NUMBER,
          gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinner_equal_tracks)));
      break;
    case SELECTED_SPLIT_SILENCE:
      mp3splt_set_int_option(the_state, SPLT_OPT_SPLIT_MODE, SPLT_OPTION_SILENCE_MODE);
      mp3splt_set_float_option(the_state, SPLT_OPT_PARAM_THRESHOLD,
          gtk_spin_button_get_value(GTK_SPIN_BUTTON(all_spinner_silence_threshold)));
      mp3splt_set_float_option(the_state, SPLT_OPT_PARAM_OFFSET,
          gtk_spin_button_get_value(GTK_SPIN_BUTTON(all_spinner_silence_offset)));
      mp3splt_set_int_option(the_state, SPLT_OPT_PARAM_NUMBER_TRACKS,
          gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(all_spinner_silence_number_tracks)));
      mp3splt_set_float_option(the_state, SPLT_OPT_PARAM_MIN_LENGTH,
          gtk_spin_button_get_value(GTK_SPIN_BUTTON(all_spinner_silence_minimum)));
      mp3splt_set_float_option(the_state, SPLT_OPT_PARAM_MIN_TRACK_LENGTH,
          gtk_spin_button_get_value(GTK_SPIN_BUTTON(all_spinner_track_minimum)));
      if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(all_silence_remove_silence)))
      {
        mp3splt_set_int_option(the_state, SPLT_OPT_PARAM_REMOVE_SILENCE, SPLT_TRUE);
      }
      else
      {
        mp3splt_set_int_option(the_state, SPLT_OPT_PARAM_REMOVE_SILENCE, SPLT_FALSE);
      }
      break;
    case SELECTED_SPLIT_TRIM_SILENCE:
      mp3splt_set_int_option(the_state, SPLT_OPT_SPLIT_MODE, SPLT_OPTION_TRIM_SILENCE_MODE);
      mp3splt_set_float_option(the_state, SPLT_OPT_PARAM_THRESHOLD,
          gtk_spin_button_get_value(GTK_SPIN_BUTTON(all_spinner_trim_silence_threshold)));
      break;
    case SELECTED_SPLIT_ERROR:
      mp3splt_set_int_option(the_state, SPLT_OPT_SPLIT_MODE,
          SPLT_OPTION_ERROR_MODE);
      break;
    default:
      break;
  }

  gint selected_tags_value = rh_get_active_value(tags_radio);
  if (selected_tags_value == NO_TAGS)
  {
    mp3splt_set_int_option(the_state, SPLT_OPT_TAGS, SPLT_NO_TAGS);
  }
  else if (selected_tags_value == DEFAULT_TAGS)
  {
    mp3splt_set_int_option(the_state, SPLT_OPT_TAGS, SPLT_CURRENT_TAGS);
  }
  else if (selected_tags_value == ORIGINAL_FILE_TAGS)
  {
    mp3splt_set_int_option(the_state, SPLT_OPT_TAGS, SPLT_TAGS_ORIGINAL_FILE);
  }
  else if (selected_tags_value == TAGS_FROM_FILENAME)
  {
    put_tags_from_filename_regex_options();
  }

  gint tags_radio_choice = get_checked_tags_version_radio_box();
  if (tags_radio_choice == 0)
  {
    mp3splt_set_int_option(the_state, SPLT_OPT_FORCE_TAGS_VERSION, 0);
  }
  else if (tags_radio_choice == 1)
  {
    mp3splt_set_int_option(the_state, SPLT_OPT_FORCE_TAGS_VERSION, 1);
  }
  else if (tags_radio_choice == 2)
  {
    mp3splt_set_int_option(the_state, SPLT_OPT_FORCE_TAGS_VERSION, 2);
  }
  else if (tags_radio_choice == 3)
  {
    mp3splt_set_int_option(the_state, SPLT_OPT_FORCE_TAGS_VERSION, 12);
  }

  mp3splt_set_int_option(the_state, SPLT_OPT_DEBUG_MODE, debug_is_active);

  mp3splt_set_int_option(the_state, SPLT_OPT_CREATE_DIRS_FROM_FILENAMES, 
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(create_dirs_from_output_files)));
}

void put_tags_from_filename_regex_options()
{
  mp3splt_set_int_option(the_state, SPLT_OPT_TAGS, SPLT_TAGS_FROM_FILENAME_REGEX);

  gint underscores = gtk_toggle_button_get_active(
      GTK_TOGGLE_BUTTON(replace_underscore_by_space_check_box));
  mp3splt_set_int_option(the_state, SPLT_OPT_REPLACE_UNDERSCORES_TAG_FORMAT, underscores);

  mp3splt_set_int_option(the_state, SPLT_OPT_ARTIST_TAG_FORMAT, 
      ch_get_active_value(artist_text_properties_combo));

  mp3splt_set_int_option(the_state, SPLT_OPT_ALBUM_TAG_FORMAT, 
      ch_get_active_value(album_text_properties_combo));

  mp3splt_set_int_option(the_state, SPLT_OPT_TITLE_TAG_FORMAT,
      ch_get_active_value(title_text_properties_combo));

  mp3splt_set_int_option(the_state, SPLT_OPT_COMMENT_TAG_FORMAT, 
      ch_get_active_value(comment_text_properties_combo));

  const gchar *regular_expression = gtk_entry_get_text(GTK_ENTRY(regex_entry));
  mp3splt_set_input_filename_regex(the_state, regular_expression);

  const gchar *default_comment = gtk_entry_get_text(GTK_ENTRY(comment_tag_entry));
  if (strlen(default_comment) == 0)
  {
    default_comment = NULL;
  }
  mp3splt_set_default_comment_tag(the_state, default_comment);

  mp3splt_set_default_genre_tag(the_state, ch_get_active_str_value(genre_combo));
}

