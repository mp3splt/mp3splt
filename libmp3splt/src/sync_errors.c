/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2013 Alexandru Munteanu - m@ioalex.net
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 *
 *********************************************************/

/*! \file

Handle sync errors and thus ID3 tags

When MP3 was invented there were no plans to include artist, genre and
similar information there. But there is a mechanism that makes MP3
players skip broken packets that are detected by "sync errors".

And it is rather easy to fill broken packets with information about
the current track - which is exactly what id3 tags do. 

So this file handles deliberately broken audio packets - that contain
plain text and images instead.
*/

#include "splt.h"

void splt_se_set_sync_errors_default_values(splt_state *state)
{
  splt_syncerrors *serrors = state->serrors;

  state->syncerrors = 0;
  serrors->serrors_points = NULL;
  serrors->serrors_points_num = 0;
}

int splt_se_serrors_append_point(splt_state *state, off_t point)
{
  int error = SPLT_OK;

  splt_syncerrors *serrors = state->serrors;

  int serrors_num = serrors->serrors_points_num;

  serrors->serrors_points_num++;

  if (point >= 0)
  {
    if (serrors->serrors_points == NULL)
    {
      if((serrors->serrors_points = 
            malloc(sizeof(off_t) * (serrors_num + 1))) == NULL)
      {
        error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      }
      else
      {
        serrors->serrors_points[0] = 0;
      }
    }
    else
    {
      if((serrors->serrors_points = realloc(serrors->serrors_points,
              sizeof(off_t) * (serrors_num + 1))) == NULL)
      {
        error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      }
    }

    if (error == SPLT_OK)
    {
      serrors->serrors_points[serrors_num] = point;

      if (point == -1)
      {
        error = SPLT_ERR_SYNC;
      }
    }
  }
  else
  {
    splt_e_error(SPLT_IERROR_INT, __func__, point, NULL);
  }

  return error;
}

void splt_se_serrors_free(splt_state *state)
{
  splt_syncerrors *serrors = state->serrors;

  if (!serrors)
  {
    return;
  }

  if (serrors->serrors_points)
  {
    free(serrors->serrors_points);
    serrors->serrors_points = NULL;
    serrors->serrors_points_num = 0;
  }
}

