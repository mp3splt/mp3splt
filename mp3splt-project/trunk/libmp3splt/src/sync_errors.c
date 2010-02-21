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

void splt_se_set_sync_errors_default_values(splt_state *state)
{
  state->serrors->serrors_points = NULL;
  state->serrors->serrors_points_num = 0;
  state->syncerrors = 0;
}

int splt_se_serrors_append_point(splt_state *state, off_t point)
{
  int error = SPLT_OK;
  int serrors_num = state->serrors->serrors_points_num;

  state->serrors->serrors_points_num++;

  if (point >= 0)
  {
    if (state->serrors->serrors_points == NULL)
    {
      if((state->serrors->serrors_points = 
            malloc(sizeof(off_t) * (serrors_num + 2))) == NULL)
      {
        error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      }
      else
      {
        state->serrors->serrors_points[0] = 0;
      }
    }
    else
    {
      if((state->serrors->serrors_points = realloc(state->serrors->serrors_points,
              sizeof(off_t) * (serrors_num + 2))) == NULL)
      {
        error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      }
    }

    if (error == SPLT_OK)
    {
      state->serrors->serrors_points[serrors_num] = point;

      if (point == -1)
      {
        error = SPLT_ERR_SYNC;
      }
    }
  }
  else
  {
    splt_u_error(SPLT_IERROR_INT, __func__, point, NULL);
  }

  return error;
}

void splt_se_serrors_free(splt_state *state)
{
  if (state->serrors->serrors_points)
  {
    free(state->serrors->serrors_points);
    state->serrors->serrors_points = NULL;
    state->serrors->serrors_points_num = 0;
  }
}

