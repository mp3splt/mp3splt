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

#include "splt.h"

/*! \file 

Miscelaneous utilities.
 */

/*! Finish Tags and provide splitpoints with a filename generated from them

  \param state The central structure that keeps all variables for 
  libmp3splt.
  \param current_splt The number of the split point to run this function for..
 */
int splt_u_finish_tags_and_put_output_format_filename(splt_state *state, int current_split)
{
  int err = splt_tu_set_tags_in_tags(state, current_split);
  if (err < 0)
  {
    return err;
  }

  return splt_of_put_output_format_filename(state, current_split);
}

//! Output the amount of time all split files are set to overlap
void splt_u_print_overlap_time(splt_state *state)
{
  long overlap_time = splt_o_get_long_option(state, SPLT_OPT_OVERLAP_TIME);
  if (overlap_time <= 0)
  {
    return;
  }

  long mins = -1;
  long secs = -1;
  long hundr = -1;
  splt_co_get_mins_secs_hundr(overlap_time, &mins, &secs, &hundr);
  splt_c_put_info_message_to_client(state, 
      _(" info: overlapping split files with %ld.%ld.%ld\n"),
      mins, secs, hundr);
}

/*! Is a time value [in seconds] after the end of our file?

\param fend_sec The time [in seconds] we have to compare with the end
of file 
\param state The central structure that keeps all data for this library.

\return SPLT_TRUE or SPLT_FALSE
*/
short splt_u_fend_sec_is_bigger_than_total_time(splt_state *state, double fend_sec)
{
  return fend_sec == -1.0;

  //TODO: comparing with the total time does not work: for example for concatenated
  //mp3's, the total time is WRONG !

/*  double total_time = splt_t_get_total_time_as_double_secs(state);

  if (total_time - 0.01 > 0)
  {
    if (fend_sec >= total_time - 0.01)
    {
      return SPLT_TRUE;
    }
  }
  else
  {
    //we might not have total time for non seekable
    if (splt_o_get_int_option(state, SPLT_OPT_INPUT_NOT_SEEKABLE))
    {
      int current_split = splt_t_get_current_split(state);
      if (splt_sp_splitpoint_exists(state, current_split + 1))
      {
        int err = SPLT_OK;
        long split_end = splt_sp_get_splitpoint_value(state, current_split+1, &err);
        if ((err >= 0) && (split_end == LONG_MAX))
        {
          return SPLT_TRUE;
        }
      }
    }
  }

  return SPLT_FALSE;*/
}

