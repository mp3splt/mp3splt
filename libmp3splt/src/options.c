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

void splt_o_set_ioptions_default_values(splt_state *state)
{
  state->iopts.library_locked = SPLT_FALSE;
  state->iopts.messages_locked = SPLT_FALSE;
  state->iopts.current_refresh_rate = SPLT_DEFAULT_PROGRESS_RATE;
  state->iopts.frame_mode_enabled = SPLT_FALSE;
  state->iopts.new_filename_path = NULL;
  state->iopts.split_begin = 0;
  state->iopts.split_end = 0;
}

