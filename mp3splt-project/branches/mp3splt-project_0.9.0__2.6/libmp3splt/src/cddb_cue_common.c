/**********************************************************
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2013 Alexandru Munteanu - m@ioalex.net
 *
 *********************************************************/

/**********************************************************
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
 * 02111-1307, USA.
 *********************************************************/

/*! \file
Everything that importing from cddb and importing from cue files have
in common.
 */

#include <string.h>

#include "splt.h"

/*! Convert the tags to sensible filenames. 

  What we deem to be a sensible file name can be controlled by the
  user using the format that is handled by oformat_parser.c
 */
void splt_cc_put_filenames_from_tags(splt_state *state, int tracks, int *error, 
    const splt_tags *all_tags, int only_set_name_if_null)
{
  int err = splt_tu_copy_tags_on_all_tracks(state, tracks, all_tags);
  if (err < 0) { *error = err; return; }

  if (splt_o_get_int_option(state, SPLT_OPT_OUTPUT_FILENAMES) == SPLT_OUTPUT_DEFAULT)
  {
    splt_of_set_oformat(state, SPLT_DEFAULT_CDDB_CUE_OUTPUT, error, SPLT_TRUE);
    if (*error < 0) { return; }
  }

  err = splt_of_reparse_oformat(state);
  if (err < 0) { *error = err; return; }

  splt_of_set_oformat_digits_tracks(state, tracks);

  if (err < 0) { *error = err; return; }

  splt_t_set_current_split(state, 0);
  int current_split = 0;

  do {
    const char *splitpoint_name = splt_sp_get_splitpoint_name(state, current_split, &err);
    if (!only_set_name_if_null || (splitpoint_name == NULL))
    {
      err = splt_u_finish_tags_and_put_output_format_filename(state, current_split);
      if (err != SPLT_OK) { *error = err; return; }
    }

    splt_t_current_split_next(state);
    current_split = splt_t_get_current_split(state);
  } while (current_split < tracks);
}


