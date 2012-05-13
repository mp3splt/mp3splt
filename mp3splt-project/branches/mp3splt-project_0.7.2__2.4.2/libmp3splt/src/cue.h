/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2012 Alexandru Munteanu - io_fx@yahoo.fr
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

#ifndef SPLT_CUE_H
#define SPLT_CUE_H

//! Internal structure that bundles all variables the cue utils need
typedef struct {
  int tracks;
  int time_for_track;
  int error;
  int performer;
  int title;
  /*! \brief cue input filename
  */
  const char *file;
  int counter;
  /*! \brief Do we have to build the file name from the tags?

    - SPLT_FALSE means we build a filename from the tags 
    - SPLT_TRUE means filename is track name
  */
  int title_is_filename;

  /*! \brief Has this file been created by us?

    If not we append a splitpoint at the end of the file.
  */
  int file_has_been_created_by_us;

  /*! \brief The type of the current track.

    The track type determines if this track will be output in the end
    or not. For a list of possible track types
    \see splt_type_of_splitpoint

   */
  int current_track_type;
} cue_utils;

int splt_cue_put_splitpoints(const char *file, splt_state *state, int *error);

void splt_cue_export_to_file(splt_state *state, const char *out_file,
    short stop_at_total_time, int *error);

#define SPLT_CUE_NOTHING 0
#define SPLT_CUE_TRACK 1
#define SPLT_CUE_TITLE 2
#define SPLT_CUE_PERFORMER 3
#define SPLT_CUE_INDEX 4

#endif

