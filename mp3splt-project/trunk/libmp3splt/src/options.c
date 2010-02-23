/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2010 Alexandru Munteanu - io_fx@yahoo.fr
 *
 * http://mp3splt.sourceforge.net
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307,
 * USA.
 *
 *********************************************************/

#include "splt.h"

extern int global_debug;

void splt_o_set_options_default_values(splt_state *state)
{
  state->options.split_mode = SPLT_OPTION_NORMAL_MODE;
  state->options.tags = SPLT_CURRENT_TAGS;
  state->options.xing = SPLT_TRUE;
  state->options.output_filenames = SPLT_OUTPUT_DEFAULT;
  state->options.quiet_mode = SPLT_FALSE;
  state->options.pretend_to_split = SPLT_FALSE;
  state->options.option_frame_mode = SPLT_FALSE;
  state->options.split_time = 6000;
  state->options.overlap_time = 0;
  state->options.option_auto_adjust = SPLT_FALSE;
  state->options.option_input_not_seekable = SPLT_FALSE;
  state->options.create_dirs_from_filenames = SPLT_FALSE;
  state->options.parameter_threshold = SPLT_DEFAULT_PARAM_THRESHOLD;
  state->options.parameter_offset = SPLT_DEFAULT_PARAM_OFFSET;
  state->options.parameter_number_tracks = SPLT_DEFAULT_PARAM_TRACKS;
  state->options.parameter_minimum_length = SPLT_DEFAULT_PARAM_MINIMUM_LENGTH;
  state->options.parameter_remove_silence = SPLT_FALSE;
  state->options.parameter_gap = SPLT_DEFAULT_PARAM_GAP;
  state->options.remaining_tags_like_x = -1;
  state->options.auto_increment_tracknumber_tags = 0;
  state->options.enable_silence_log = SPLT_FALSE;
  state->options.force_tags_version = 0;
  state->options.length_split_file_number = 1;
  state->options.replace_tags_in_tags = SPLT_FALSE;
}

void splt_o_set_option(splt_state *state, int option_name, const void *data)
{
  switch (option_name)
  {
    case SPLT_OPT_DEBUG_MODE:
      global_debug = *((int *)data);
      break;
    case SPLT_OPT_QUIET_MODE:
      state->options.quiet_mode = *((int *)data);
      break;
    case SPLT_OPT_PRETEND_TO_SPLIT:
      state->options.pretend_to_split = *((int *)data);
      break;
    case SPLT_OPT_OUTPUT_FILENAMES:
      state->options.output_filenames = *((int *)data);
      break;
    case SPLT_OPT_SPLIT_MODE:
      state->options.split_mode = *((int *)data);
      break;
    case SPLT_OPT_TAGS:
      state->options.tags = *((int *)data);
      break;
    case SPLT_OPT_XING:
      state->options.xing = *((int *)data);
      break;
    case SPLT_OPT_CREATE_DIRS_FROM_FILENAMES:
      state->options.create_dirs_from_filenames = *((int *)data);
      break;
    case SPLT_OPT_FRAME_MODE:
      state->options.option_frame_mode = *((int *)data);
      break;
    case SPLT_OPT_AUTO_ADJUST:
      state->options.option_auto_adjust = *((int *)data);
      break;
    case SPLT_OPT_INPUT_NOT_SEEKABLE:
      state->options.option_input_not_seekable = *((int *)data);
      break;
    case SPLT_OPT_PARAM_NUMBER_TRACKS:
      state->options.parameter_number_tracks = *((int *)data);
      break;
    case SPLT_OPT_PARAM_REMOVE_SILENCE:
      state->options.parameter_remove_silence = *((int *)data);
      break;
    case SPLT_OPT_PARAM_GAP:
      state->options.parameter_gap = *((int *)data);
      break;
    case SPLT_OPT_ALL_REMAINING_TAGS_LIKE_X:
      state->options.remaining_tags_like_x = *((int *)data);
      break;
    case SPLT_OPT_AUTO_INCREMENT_TRACKNUMBER_TAGS:
      state->options.auto_increment_tracknumber_tags = *((int *)data);
      break;
    case SPLT_OPT_ENABLE_SILENCE_LOG:
      state->options.enable_silence_log = *((int *)data);
      break;
    case SPLT_OPT_FORCE_TAGS_VERSION:
      state->options.force_tags_version = *((int *)data);
      break;
    case SPLT_OPT_LENGTH_SPLIT_FILE_NUMBER:
      state->options.length_split_file_number = *((int *)data);
      break;
    case SPLT_OPT_REPLACE_TAGS_IN_TAGS:
      state->options.replace_tags_in_tags = *((int *)data);
      break;

    case SPLT_OPT_OVERLAP_TIME:
      state->options.overlap_time = *((long *)data);
      break;

    case SPLT_OPT_SPLIT_TIME:
      state->options.split_time = *((float *)data);
      break;
    case SPLT_OPT_PARAM_THRESHOLD:
      state->options.parameter_threshold = *((float *)data);
      break;
    case SPLT_OPT_PARAM_OFFSET:
      state->options.parameter_offset = *((float *)data);
      break;
    case SPLT_OPT_PARAM_MIN_LENGTH:
      state->options.parameter_minimum_length = *((float *)data);
      break;

    default:
      splt_e_error(SPLT_IERROR_INT,__func__, option_name, NULL);
      break;
  }
}

void splt_o_set_int_option(splt_state *state, int option_name, int value)
{
  splt_o_set_option(state, option_name, &value);
}

void splt_o_set_long_option(splt_state *state, int option_name, long value)
{
  splt_o_set_option(state, option_name, &value);
}

void splt_o_set_float_option(splt_state *state, int option_name, float value)
{
  splt_o_set_option(state, option_name, &value);
}

static void *splt_o_get_option(splt_state *state, int option_name)
{
  switch (option_name)
  {
    case SPLT_OPT_QUIET_MODE:
      return &state->options.quiet_mode;
      break;
    case SPLT_OPT_PRETEND_TO_SPLIT:
    ;
      return &state->options.pretend_to_split;
      break;
    case SPLT_OPT_OUTPUT_FILENAMES:
      return &state->options.output_filenames;
      break;
    case SPLT_OPT_SPLIT_MODE:
      return &state->options.split_mode;
      break;
    case SPLT_OPT_TAGS:
      return &state->options.tags;
      break;
    case SPLT_OPT_XING:
      return &state->options.xing;
      break;
    case SPLT_OPT_CREATE_DIRS_FROM_FILENAMES:
      return &state->options.create_dirs_from_filenames;
      break;
    case SPLT_OPT_FRAME_MODE:
      return &state->options.option_frame_mode;
      break;
    case SPLT_OPT_AUTO_ADJUST:
      return &state->options.option_auto_adjust;
      break;
    case SPLT_OPT_INPUT_NOT_SEEKABLE:
      return &state->options.option_input_not_seekable;
      break;
    case SPLT_OPT_PARAM_NUMBER_TRACKS:
      return &state->options.parameter_number_tracks;
      break;
    case SPLT_OPT_PARAM_REMOVE_SILENCE:
      return &state->options.parameter_remove_silence;
      break;
    case SPLT_OPT_PARAM_GAP:
      return &state->options.parameter_gap;
      break;
    case SPLT_OPT_ALL_REMAINING_TAGS_LIKE_X:
      return &state->options.remaining_tags_like_x;
      break;
    case SPLT_OPT_AUTO_INCREMENT_TRACKNUMBER_TAGS:
      return &state->options.auto_increment_tracknumber_tags;
      break;
    case SPLT_OPT_ENABLE_SILENCE_LOG:
      return &state->options.enable_silence_log;
      break;
    case SPLT_OPT_FORCE_TAGS_VERSION:
      return &state->options.force_tags_version;
      break;
    case SPLT_OPT_LENGTH_SPLIT_FILE_NUMBER:
      return &state->options.length_split_file_number;
      break;
    case SPLT_OPT_REPLACE_TAGS_IN_TAGS:
      return &state->options.replace_tags_in_tags;
      break;
    case SPLT_OPT_OVERLAP_TIME:
      return &state->options.overlap_time;
      break;
    case SPLT_OPT_SPLIT_TIME:
      return &state->options.split_time;
      break;
    case SPLT_OPT_PARAM_THRESHOLD:
      return &state->options.parameter_threshold;
      break;
    case SPLT_OPT_PARAM_OFFSET:
      return &state->options.parameter_offset;
      break;
    case SPLT_OPT_PARAM_MIN_LENGTH:
      return &state->options.parameter_minimum_length;
      break;
    default:
      splt_e_error(SPLT_IERROR_INT,__func__, option_name, NULL);
      break;
  }

  return NULL;
}

int splt_o_get_int_option(splt_state *state, int option_name)
{
  return *((int *)splt_o_get_option(state, option_name));
}

long splt_o_get_long_option(splt_state *state, int option_name)
{
  return *((long *)splt_o_get_option(state, option_name));
}

float splt_o_get_float_option(splt_state *state, int option_name)
{
  return *((float *)splt_o_get_option(state, option_name));
}

void splt_o_set_ioptions_default_values(splt_state *state)
{
  state->iopts.library_locked = SPLT_FALSE;
  state->iopts.messages_locked = SPLT_FALSE;
  state->iopts.current_refresh_rate = SPLT_DEFAULT_PROGRESS_RATE;
  state->iopts.frame_mode_enabled = SPLT_FALSE;
  state->iopts.new_filename_path = NULL;
}

void splt_o_iopts_free(splt_state *state)
{
  if (state->iopts.new_filename_path)
  {
    free(state->iopts.new_filename_path);
    state->iopts.new_filename_path = NULL;
  }
}

void splt_o_set_iopt(splt_state *state, int type, int value)
{
  switch (type)
  {
    case SPLT_INTERNAL_FRAME_MODE_ENABLED:
      state->iopts.frame_mode_enabled = value;
      break;
    case SPLT_INTERNAL_PROGRESS_RATE:
      state->iopts.current_refresh_rate = value;
      break;
    default:
      break;
  }
}

int splt_o_get_iopt(splt_state *state, int type)
{
  switch (type)
  {
    case SPLT_INTERNAL_FRAME_MODE_ENABLED:
      return state->iopts.frame_mode_enabled;
      break;
    case SPLT_INTERNAL_PROGRESS_RATE:
      return state->iopts.current_refresh_rate;
      break;
    default:
      break;
  }

  return 0;
}

void splt_o_set_default_iopts(splt_state *state)
{
  splt_o_set_iopt(state, SPLT_INTERNAL_FRAME_MODE_ENABLED,SPLT_FALSE);
  splt_o_set_iopt(state, SPLT_INTERNAL_PROGRESS_RATE,0);
  splt_t_set_new_filename_path(state, NULL, NULL);
}

int splt_o_library_locked(splt_state *state)
{
  return state->iopts.library_locked;
}

void splt_o_lock_library(splt_state *state)
{
  state->iopts.library_locked = SPLT_TRUE;
}

void splt_o_unlock_library(splt_state *state)
{
  state->iopts.library_locked = SPLT_FALSE;
}

int splt_o_messages_locked(splt_state *state)
{
  return state->iopts.messages_locked;
}

void splt_o_lock_messages(splt_state *state)
{
  state->iopts.messages_locked = SPLT_TRUE;
}

void splt_o_unlock_messages(splt_state *state)
{
  state->iopts.messages_locked = SPLT_FALSE;
}

