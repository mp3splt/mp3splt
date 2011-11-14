/**********************************************************
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2011 Alexandru Munteanu - io_fx@yahoo.fr
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307,
 * USA.
 *********************************************************/

#include "silence_processors.h"

splt_scan_silence_data *splt_scan_silence_data_new(splt_state *state, short first, 
    float min, short set_new_length)
{
  splt_scan_silence_data *ssd = malloc(sizeof(splt_scan_silence_data));
  if (!ssd)
  {
    return NULL;
  }

  ssd->state = state;
  ssd->first = first;
  ssd->min = min;
  ssd->set_new_length = set_new_length;

  ssd->flush = SPLT_FALSE;
  ssd->silence_begin = 0;
  ssd->silence_end = 0;
  ssd->len = 0;
  ssd->found = 0;
  ssd->shot = SPLT_DEFAULTSHOT;
  ssd->silence_begin_was_found = SPLT_FALSE;
  ssd->continue_after_silence = SPLT_FALSE;

  return ssd;
}

void splt_free_scan_silence_data(splt_scan_silence_data **ssd)
{
  if (!ssd || !*ssd)
  {
    return;
  }

  free(*ssd);
  *ssd = NULL;
}

short splt_scan_silence_processor(double time, int silence_was_found,
    short must_flush, splt_scan_silence_data *ssd, int *found_silence_points, int *error)
{
  if (time < 0) { return SPLT_TRUE; }

  short stop = SPLT_FALSE;

  if (must_flush)
  {
    ssd->flush = SPLT_TRUE;
    stop = SPLT_TRUE;
  }

  if (!ssd->flush && silence_was_found)
  {
    if (ssd->len == 0)
    {
      ssd->silence_begin = time;
    }

    if (ssd->first == SPLT_FALSE) 
    {
      ssd->len++;
    }

    if (ssd->shot < SPLT_DEFAULTSHOT)
    {
      ssd->shot += 2;
    }

    ssd->silence_end = time;

    *found_silence_points = ssd->found;
    return stop;
  }

  if (ssd->len > SPLT_DEFAULTSILLEN)
  {
    if (ssd->flush || (ssd->shot <= 0))
    {
      double begin_position = ssd->silence_begin;
      double end_position = ssd->silence_end;

      if (ssd->set_new_length)
      {
        ssd->len = (int) (ssd->silence_end * 100.0 - ssd->silence_begin * 100.0);
      }

      if ((end_position - begin_position - ssd->min) >= 0.f)
      {
        if (splt_siu_ssplit_new(&ssd->state->silence_list,
              begin_position, end_position, ssd->len, error) == -1)
        {
          ssd->found = -1;
          *found_silence_points = ssd->found;
          return SPLT_TRUE;
        }

        ssd->found++;
      }

      ssd->len = 0;
      ssd->shot = SPLT_DEFAULTSHOT;
    }
  }
  else 
  {
    ssd->len = 0;
  }

  if (ssd->flush)
  {
    return -1;
  }

  if (ssd->first && (ssd->shot <= 0))
  {
    ssd->first = SPLT_FALSE;
  }

  if (ssd->shot > 0) 
  {
    ssd->shot--;
  }

  if (ssd->found >= SPLT_MAXSILENCE) 
  {
    stop = SPLT_TRUE;
  }

  *found_silence_points = ssd->found;

  return stop;
}

static short splt_detect_where_begin_silence_ends(double time, int silence_was_found,
    short must_flush, splt_scan_silence_data *ssd, int *found_silence_points, int *error)
{
  if (silence_was_found)
  {
    if (ssd->shot < SPLT_DEFAULTSHOT)
    {
      ssd->shot += 2;
    }

    ssd->silence_end = time;
  }

  if (ssd->shot <= 0)
  {
    if (splt_siu_ssplit_new(&ssd->state->silence_list, ssd->silence_end, ssd->silence_end, 0, error) == -1)
    {
      return SPLT_TRUE;
    }

    ssd->found++;
    ssd->silence_begin_was_found = SPLT_TRUE;
    ssd->shot = SPLT_DEFAULTSHOT;
  }

  if (ssd->shot > 0)
  {
    ssd->shot--;
  }

  return SPLT_FALSE;
}

static short splt_detect_where_end_silence_begins(double time, int silence_was_found, 
    short must_flush, splt_scan_silence_data *ssd, int *found_silence_points, int *error)
{
  if (time < 0)
  {
    if (splt_siu_ssplit_new(&ssd->state->silence_list, ssd->silence_begin, ssd->silence_begin, 0, error) == -1)
    {
      return SPLT_TRUE;
    }

    ssd->found++;

    return SPLT_TRUE;
  }

  if (silence_was_found)
  {
    if (ssd->len == 0)
    {
      ssd->silence_begin = time;
      ssd->continue_after_silence = SPLT_FALSE;
    }

    if (ssd->first == SPLT_FALSE) 
    {
      ssd->len++;
    }

    if (ssd->shot < SPLT_DEFAULTSHOT)
    {
      ssd->shot += 2;
    }

    return SPLT_FALSE;
  }
  else if (ssd->continue_after_silence)
  {
    ssd->silence_begin = time;
  }

  if (ssd->len > SPLT_DEFAULTSILLEN)
  {
    if (ssd->shot <= 0)
    {
      ssd->len = 0;
      ssd->shot = SPLT_DEFAULTSHOT;
      ssd->continue_after_silence = SPLT_TRUE;
    }
  }
  else
  {
    ssd->len = 0;
  }

  if (ssd->first && (ssd->shot <= 0))
  {
    ssd->first = SPLT_FALSE;
  }

  if (ssd->shot > 0)
  {
    ssd->shot--;
  }

  return SPLT_FALSE;
}

short splt_trim_silence_processor(double time, int silence_was_found, 
    short must_flush, splt_scan_silence_data *ssd, int *found_silence_points, int *error)
{
  if (!ssd->silence_begin_was_found)
  {
    splt_detect_where_begin_silence_ends(time, silence_was_found, must_flush, ssd, found_silence_points, error);
  }
  else 
  {
    splt_detect_where_end_silence_begins(time, silence_was_found, must_flush, ssd, found_silence_points, error);
  }

  return SPLT_FALSE;
}

