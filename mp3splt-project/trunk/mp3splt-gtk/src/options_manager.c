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

extern gint debug_is_active;

extern splt_state *the_state;

extern GtkWidget *entry;
extern GtkWidget *output_entry;
extern gchar *filename_to_split;
extern gint selected_split_mode;

extern GtkWidget *frame_mode;
extern GtkWidget *adjust_mode;
extern GtkWidget *spinner_adjust_gap;
extern GtkWidget *spinner_adjust_offset;
extern GtkWidget *spinner_adjust_threshold;
extern GtkWidget *spinner_time;
extern GtkWidget *create_dirs_from_output_files;

void update_output_options()
{
  filename_to_split = (gchar *) gtk_entry_get_text(GTK_ENTRY(entry));
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
        gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(spinner_adjust_offset)));
    mp3splt_set_int_option(the_state, SPLT_OPT_PARAM_GAP,
        gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinner_adjust_gap)));
    mp3splt_set_float_option(the_state, SPLT_OPT_PARAM_THRESHOLD,
        gtk_spin_button_get_value_as_float(GTK_SPIN_BUTTON(spinner_adjust_threshold)));
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
    case SELECTED_SPLIT_ERROR:
      mp3splt_set_int_option(the_state, SPLT_OPT_SPLIT_MODE,
          SPLT_OPTION_ERROR_MODE);
      break;
    default:
      break;
  }

  if (get_checked_tags_radio_box() == 0)
  {
    mp3splt_set_int_option(the_state, SPLT_OPT_TAGS, SPLT_NO_TAGS);
  }
  else
  {
    if (get_checked_tags_radio_box() == 1)
    {
      mp3splt_set_int_option(the_state, SPLT_OPT_TAGS, SPLT_CURRENT_TAGS);
    }
    else
    {
      if (get_checked_tags_radio_box() == 2)
      {
        mp3splt_set_int_option(the_state, SPLT_OPT_TAGS, SPLT_TAGS_ORIGINAL_FILE);
      }
    }
  }

  //tag version options
  gint tags_radio_choice = get_checked_tags_version_radio_box();
  if (tags_radio_choice == 0)
  {
    mp3splt_set_int_option(the_state, SPLT_OPT_FORCE_TAGS_VERSION, 0);
  }
  else
  {
    if (tags_radio_choice == 1)
    {
      mp3splt_set_int_option(the_state, SPLT_OPT_FORCE_TAGS_VERSION, 1);
    }
    else
    {
      if (tags_radio_choice == 2)
      {
        mp3splt_set_int_option(the_state, SPLT_OPT_FORCE_TAGS_VERSION, 2);
      }
      else
      {
        if (tags_radio_choice == 3)
        {
          mp3splt_set_int_option(the_state, SPLT_OPT_FORCE_TAGS_VERSION, 12);
        }
      }
    }
  }

  mp3splt_set_int_option(the_state, SPLT_OPT_DEBUG_MODE, debug_is_active);

  mp3splt_set_int_option(the_state, SPLT_OPT_CREATE_DIRS_FROM_FILENAMES, 
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(create_dirs_from_output_files)));
}

