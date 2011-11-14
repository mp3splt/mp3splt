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

/*! \file

All functions needed for handling split points (adding to the list,
checking, of this split point already exists,...)
*/
#include <string.h>

#include "splt.h"

int splt_sp_splitpoint_exists(splt_state *state, int index)
{
  if ((index >= 0) && (index < state->split.real_splitnumber))
  {
    return SPLT_TRUE;
  }
  else
  {
    return SPLT_FALSE;
  }
}

int splt_sp_append_splitpoint(splt_state *state, long split_value,
    const char *name, int type)
{
  int error = SPLT_OK;

  splt_struct *split = &state->split;

  splt_d_print_debug(state,"Appending splitpoint _%s_ with value _%ld_\n",
      name, split_value);

  split->real_splitnumber++;

  if (!split->points)
  {
    if ((split->points = malloc(sizeof(splt_point))) == NULL)
    {
      return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    }
  }
  else
  {
    if ((split->points = realloc(split->points,
            split->real_splitnumber * sizeof(splt_point))) == NULL)
    {
      return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    }
  }

  int index = split->real_splitnumber - 1;
  split->points[index].name = NULL;

  error = splt_sp_set_splitpoint_value(state, index, split_value);
  if (error != SPLT_OK) { return error; }

  error = splt_sp_set_splitpoint_name(state, index, name);
  if (error < 0) { return error; }

  splt_sp_set_splitpoint_type(state, index, type);

  return error;
}

splt_point *splt_sp_get_splitpoints(splt_state *state, int *splitpoints_number)
{
  splt_struct *split = &state->split;
  *splitpoints_number = split->real_splitnumber;
  return split->points;
}

void splt_sp_free_splitpoints(splt_state *state)
{
  splt_struct *split = &state->split;

  if (split->points)
  {
    int i = 0;
    for (i = 0; i < split->real_splitnumber; i++)
    {
      if (split->points[i].name)
      {
        free(split->points[i].name);
        split->points[i].name = NULL;
      }
    }

    free(split->points);
    split->points = NULL;
  }

  split->splitnumber = 0;
  split->real_splitnumber = 0;
}

int splt_sp_set_splitpoint_value(splt_state *state, int index, long split_value)
{
  splt_d_print_debug(state,"Splitpoint at _%d_ is %ld_\n", index, split_value);

  int error = SPLT_OK;

  if ((index >= 0) &&
      (index < state->split.real_splitnumber))
  {
    state->split.points[index].value = split_value;
  }
  else
  {
    splt_e_error(SPLT_IERROR_INT,__func__, index, NULL);
  }

  return error;
}

int splt_sp_set_splitpoint_name(splt_state *state, int index, const char *name)
{
  splt_d_print_debug(state,"Splitpoint name at _%d_ is _%s_\n", index, name);

  int error = SPLT_OK;
  splt_point *points = state->split.points;

  if ((index >= 0) && (index < state->split.real_splitnumber))
  {
    error = splt_su_copy(name, &points[index].name);
  }
  else
  {
    splt_e_error(SPLT_IERROR_INT,__func__, index, NULL);
  }

  return error;
}

int splt_sp_set_splitpoint_type(splt_state *state, int index, int type)
{
  int error = SPLT_OK;

  if ((index >= 0) && (index < state->split.real_splitnumber))
  {
    state->split.points[index].type = type;
  }
  else
  {
    splt_e_error(SPLT_IERROR_INT, __func__, index, NULL);
  }

  return error;
}

long splt_sp_get_splitpoint_value(splt_state *state, int index, int *error)
{
  if ((index >= 0) && (index < state->split.real_splitnumber))
  {
    return state->split.points[index].value;
  }
  else
  {
    splt_e_error(SPLT_IERROR_INT, __func__, index, NULL);
    return -1;
  }
}

const char *splt_sp_get_splitpoint_name(splt_state *state, int index, int *error)
{
  if ((index >= 0) && (index < state->split.real_splitnumber))
  {
    return state->split.points[index].name;
  }
  else
  {
    //splt_e_error(SPLT_IERROR_INT,__func__, index, NULL);
    return NULL;
  }
}

int splt_sp_get_splitpoint_type(splt_state *state, int index, int *error)
{
  if ((index >= 0) && (index < state->split.real_splitnumber))
  {
    return state->split.points[index].type;
  }
  else
  {
    //splt_e_error(SPLT_IERROR_INT,__func__, index, NULL);
    //wtf ?
    return 1;
  }
}

void splt_sp_get_mins_secs_hundr_from_splitpoint(long splitpoint,
    long *mins, long *secs, long *hundr)
{
  if (hundr)
  {
    *hundr = splitpoint % 100;
  }

  splitpoint /= 100;

  if (secs)
  {
    *secs = splitpoint % 60;
  }

  if (mins)
  {
    *mins = splitpoint / 60;
  }
}

int splt_sp_cut_splitpoint_extension(splt_state *state, int index)
{
  int error = SPLT_OK;

  if (splt_sp_splitpoint_exists(state,index))
  {
    const char *temp_name = splt_sp_get_splitpoint_name(state, index, &error);
    if (error < 0) { return error; }

    if (temp_name)
    {
      char *new_name = NULL;
      error = splt_su_copy(temp_name, &new_name);
      if (error < 0) { return error; }

      splt_su_cut_extension(new_name);
      error = splt_sp_set_splitpoint_name(state, index, new_name);

      free(new_name);
      new_name = NULL;
    }
  }

  return error;
}

static int splt_point_value_sort(const void *p1, const void *p2)
{
  splt_point *point1 = (splt_point *)p1;
  splt_point *point2 = (splt_point *)p2;

  return point1->value - point2->value;
}

void splt_sp_order_splitpoints(splt_state *state, int len)
{
  qsort(state->split.points, state->split.real_splitnumber, 
      sizeof *state->split.points, splt_point_value_sort);
}

void splt_sp_skip_minimum_track_length_splitpoints(splt_state *state, int *error)
{
  if (state->split.real_splitnumber <= 0) { return; }

  long min_track_length = 
    splt_co_time_to_long(splt_o_get_float_option(state, SPLT_OPT_PARAM_MIN_TRACK_LENGTH));

  int i = 1;
  for (i = 1;i < state->split.real_splitnumber; i++)
  {
    int begin_index = i-1;
    int end_index = i;

    int begin_point_type = splt_sp_get_splitpoint_type(state, begin_index, error);
    if (*error < 0) { return; }

    if (begin_point_type == SPLT_SKIPPOINT)
    {
      continue;
    }

    long begin_track_point = splt_sp_get_splitpoint_value(state, begin_index, error);
    if (*error < 0) { return; }
    long end_track_point = splt_sp_get_splitpoint_value(state, end_index, error);
    if (*error < 0) { return; }

    long track_length = end_track_point - begin_track_point;
    if (track_length >= min_track_length)
    {
      continue;
    }

    long mins1, secs1, hundr1;
    splt_co_get_mins_secs_hundr(track_length, &mins1, &secs1, &hundr1);
    long mins2, secs2, hundr2;
    splt_co_get_mins_secs_hundr(min_track_length, &mins2, &secs2, &hundr2);

    splt_c_put_info_message_to_client(state, 
        _(" info: track too short (%ld.%ld.%ld < %ld.%ld.%ld); skipped.\n"),
        mins1, secs1, hundr1, mins2, secs2, hundr2);

    splt_sp_set_splitpoint_type(state, begin_index, SPLT_SKIPPOINT);
  }
}

long splt_sp_overlap_time(splt_state *state, int splitpoint_index)
{
  int error = SPLT_OK;
  long split_value = splt_sp_get_splitpoint_value(state, splitpoint_index, &error);
  long overlap_time = splt_o_get_long_option(state, SPLT_OPT_OVERLAP_TIME);
  if ((overlap_time > 0) && (split_value != LONG_MAX))
  {
    long overlapped_split_value = split_value + overlap_time;
    long total_time = splt_t_get_total_time(state);
    if ((total_time > 0) && (overlapped_split_value > total_time))
    {
      overlapped_split_value = total_time;
    }

    splt_sp_set_splitpoint_value(state, splitpoint_index, overlapped_split_value);

    return overlapped_split_value;
  }

  return split_value;
}

