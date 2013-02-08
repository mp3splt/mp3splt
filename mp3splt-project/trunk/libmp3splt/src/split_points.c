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

All functions needed for handling split points (adding to the list,
checking, of this split point already exists,...)
*/
#include <string.h>

#include "splt.h"

int splt_sp_splitpoint_exists(splt_state *state, int index)
{
  if (state->split.points &&
      (index >= 0) && (index < state->split.points->real_splitnumber))
  {
    return SPLT_TRUE;
  }
  else
  {
    return SPLT_FALSE;
  }
}

int splt_sp_get_real_splitpoints_number(splt_state *state)
{
  if (state->split.points == NULL)
  {
    return 0;
  }

  return state->split.points->real_splitnumber;
}

int splt_sp_append_splitpoint(splt_state *state, long split_value,
    const char *name, int type)
{
  int error = SPLT_OK;

  splt_struct *split = &state->split;

  splt_d_print_debug(state,"Appending splitpoint _%s_ with value _%ld_\n",
      name, split_value);

  if (!split->points)
  {
    split->points = malloc(sizeof(splt_points));
    if (split->points == NULL)
    {
      return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    }

    split->points->real_splitnumber = 0;
    split->points->iterator_counter = 0;

    if ((split->points->points = malloc(sizeof(splt_point))) == NULL)
    {
      free(split->points);
      split->points = NULL;
      return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    }
  }
  else
  {
    if ((split->points->points =
          realloc(split->points->points, (split->points->real_splitnumber + 1) * sizeof(splt_point))) == NULL)
    {
      return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    }
  }

  split->points->real_splitnumber++;

  int index = split->points->real_splitnumber - 1;
  split->points->points[index].name = NULL;

  error = splt_sp_set_splitpoint_value(state, index, split_value);
  if (error != SPLT_OK) { return error; }

  error = splt_sp_set_splitpoint_name(state, index, name);
  if (error < 0) { return error; }

  splt_sp_set_splitpoint_type(state, index, type);

  return error;
}

//unoptimized
int splt_sp_remove_splitpoint(splt_state *state, int index)
{
  splt_d_print_debug(state,"Removing splitpoint at _%d_ ...\n", index);

  int error = SPLT_OK;

  if (state->split.points &&
      (index >= 0) && (index < state->split.points->real_splitnumber))
  {
    if (state->split.points->points[index].name)
    {
      free(state->split.points->points[index].name);
      state->split.points->points[index].name = NULL;
    }

    int i;
    for (i = index + 1; i < state->split.points->real_splitnumber;i++)
    {
      state->split.points->points[i - 1] = state->split.points->points[i];
    }

    state->split.points->real_splitnumber--;
  }
  else
  {
    splt_e_error(SPLT_IERROR_INT, __func__, index, NULL);
  }

  return error;
}

splt_points *splt_sp_get_splitpoints(splt_state *state)
{
  return state->split.points;
}

void splt_sp_free_splitpoints(splt_state *state)
{
  splt_struct *split = &state->split;

  if (split->points)
  {
    int i = 0;
    for (i = 0; i < split->points->real_splitnumber; i++)
    {
      if (split->points->points[i].name)
      {
        free(split->points->points[i].name);
        split->points->points[i].name = NULL;
      }
    }

    free(split->points->points);
    split->points->points = NULL;

    free(split->points);
    split->points = NULL;
  }

  split->splitnumber = 0;
}

void splt_sp_free_one_splitpoint(splt_point *point)
{
  if (!point) { return; }

  if (point->name)
  {
    free(point->name);
  }
  free(point);
}

int splt_sp_set_splitpoint_value(splt_state *state, int index, long split_value)
{
  splt_d_print_debug(state,"Splitpoint at _%d_ is %ld_\n", index, split_value);

  int error = SPLT_OK;

  if (state->split.points &&
      (index >= 0) && (index < state->split.points->real_splitnumber))
  {
    state->split.points->points[index].value = split_value;
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
  splt_point *points = state->split.points->points;

  if (state->split.points && 
      (index >= 0) && (index < state->split.points->real_splitnumber))
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

  if (state->split.points &&
      (index >= 0) && (index < state->split.points->real_splitnumber))
  {
    state->split.points->points[index].type = type;
  }
  else
  {
    splt_e_error(SPLT_IERROR_INT, __func__, index, NULL);
  }

  return error;
}

long splt_sp_get_splitpoint_value(splt_state *state, int index, int *error)
{
  if (state->split.points && 
      (index >= 0) && (index < state->split.points->real_splitnumber))
  {
    return state->split.points->points[index].value;
  }
  else
  {
    splt_e_error(SPLT_IERROR_INT, __func__, index, NULL);
    return -1;
  }
}

const char *splt_sp_get_splitpoint_name(splt_state *state, int index, int *error)
{
  if (state->split.points &&
      (index >= 0) && (index < state->split.points->real_splitnumber))
  {
    return state->split.points->points[index].name;
  }
  else
  {
    //splt_e_error(SPLT_IERROR_INT,__func__, index, NULL);
    return NULL;
  }
}

int splt_sp_get_splitpoint_type(splt_state *state, int index, int *error)
{
  if (state->split.points && 
      (index >= 0) && (index < state->split.points->real_splitnumber))
  {
    return state->split.points->points[index].type;
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
  if (!state->split.points)
  {
    return;
  }

  qsort(state->split.points->points, state->split.points->real_splitnumber, 
      sizeof *state->split.points->points, splt_point_value_sort);
}

void splt_sp_skip_minimum_track_length_splitpoints(splt_state *state, int *error)
{
  if (!state->split.points ||
      (state->split.points->real_splitnumber <= 0))
  {
    return;
  }

  long min_track_length = 
    splt_co_time_to_long(splt_o_get_float_option(state, SPLT_OPT_PARAM_MIN_TRACK_LENGTH));

  int i = 1;
  for (i = 1;i < state->split.points->real_splitnumber; i++)
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

splt_array *splt_sp_find_intervals_between_two_consecutive_big_tracks(splt_state *state,
    int min_track_join, int *error)
{
  splt_array *intervals = splt_array_new();
  if (!intervals) { return NULL; }

  if (!state->split.points ||
      state->split.points->real_splitnumber == 1)
  {
    return intervals;
  }

  int previous_was_big_track = SPLT_FALSE;
  int start_interval = 0;
  int fix_start_point = SPLT_FALSE;
  int end_interval = 1;

  int appended_last = SPLT_FALSE;

  int i = 1;
  for (i = 1;i < state->split.points->real_splitnumber; i++)
  {
    int begin_index = i-1;
    int end_index = i;

    long begin_track_point = splt_sp_get_splitpoint_value(state, begin_index, error);
    if (*error < 0) { splt_array_free(&intervals); return NULL; }
    long end_track_point = splt_sp_get_splitpoint_value(state, end_index, error);
    if (*error < 0) { splt_array_free(&intervals); return NULL; }

    long track_length = end_track_point - begin_track_point;
    if (track_length >= min_track_join)
    {
      if (previous_was_big_track)
      {
        if (!fix_start_point)
        {
          start_interval = begin_index;
        }
        else
        {
          end_interval = begin_index;

          splt_int_pair *pair = splt_int_pair_new(start_interval, end_interval);
          splt_array_append(intervals, (void *) pair);

          fix_start_point = SPLT_FALSE;
          start_interval = end_interval;
        }
      }

      previous_was_big_track = SPLT_TRUE;
    }
    else
    {
      fix_start_point = SPLT_TRUE;
      previous_was_big_track = SPLT_FALSE;

      if (!appended_last && 
          (i == state->split.points->real_splitnumber - 1 || i == state->split.points->real_splitnumber - 2))
      {
        end_interval = state->split.points->real_splitnumber - 1;
        splt_int_pair *pair = splt_int_pair_new(start_interval, end_interval);
        splt_array_append(intervals, (void *) pair);
        appended_last = SPLT_TRUE;
      }
    }
  }

  return intervals;
}

static void free_intervals(splt_array *intervals)
{
  long number_of_intervals = splt_array_length(intervals);
  long i = 0;
  for (i = 0; i < number_of_intervals; i++)
  {
    splt_int_pair *pair = (splt_int_pair *) splt_array_get(intervals, i);
    splt_int_pair_free(&pair);
  }
  splt_array_free(&intervals);
}

static int backward_sort(const void *a, const void *b)
{
  const int *ia = (const int *)a;
  const int *ib = (const int *)b;
  return *ib - *ia;
}

void splt_sp_join_minimum_tracks_splitpoints(splt_state *state, int *error)
{
  if (!state->split.points ||
      (state->split.points->real_splitnumber <= 0))
  {
    return;
  }

  float min_track_join_f = splt_o_get_float_option(state, SPLT_OPT_PARAM_MIN_TRACK_JOIN);
  long min_track_join = splt_co_time_to_long(min_track_join_f);

  if (min_track_join <= 0) { return; }

  splt_array *intervals = 
    splt_sp_find_intervals_between_two_consecutive_big_tracks(state, min_track_join, error);
  if (!intervals) { return; }

  while (intervals && splt_array_length(intervals) > 0)
  {
    long number_of_intervals = splt_array_length(intervals);

    int indexes_to_remove[state->split.points->real_splitnumber];
    int number_of_indexes_to_remove = 0;

    long i = 0;
    for (i = 0; i < number_of_intervals; i++)
    {
      splt_int_pair *pair = (splt_int_pair *) splt_array_get(intervals, i);
      int begin_index = splt_int_pair_first(pair);
      int end_index = splt_int_pair_second(pair);

      //if the last segment is small
      long end_time = splt_sp_get_splitpoint_value(state, end_index, error);
      if (*error < 0) { goto end; }
      int start_index = end_index - 1;
      long before_end_time = splt_sp_get_splitpoint_value(state, start_index, error);
      if (*error < 0) { goto end; }

      long track_length = end_time - before_end_time;
      while (track_length < min_track_join)
      {
        if (start_index == 0)
        {
          break;
        }

        start_index--;
        track_length = end_time - splt_sp_get_splitpoint_value(state, start_index, error);
        if (*error < 0) { goto end; }
      }
      //join last segments
      if (start_index != end_index - 1)
      {
        int j;
        int number_of_indexes_removed = 0;
        for (j = end_index - 1; j > start_index; j--)
        {
          indexes_to_remove[number_of_indexes_to_remove] = j;
          number_of_indexes_to_remove++;
          number_of_indexes_removed++;
        }
        end_index -= number_of_indexes_removed;
      }

      //join small segments with what follows until >= min_track_join
      int start_join = begin_index;
      int end_join = start_join + 1;
      while (end_join <= end_index)
      {
        long end_join_time = splt_sp_get_splitpoint_value(state, end_join, error);
        if (*error < 0) { goto end; }
        long start_join_time = splt_sp_get_splitpoint_value(state, start_join, error);
        if (*error < 0) { goto end; }

        long track_length = end_join_time - start_join_time;
        while (track_length < min_track_join)
        {
          if (end_join == end_index)
          {
            break;
          }

          end_join++;
          track_length = splt_sp_get_splitpoint_value(state, end_join, error) - start_join_time;
        }

        //join segments
        int k;
        for (k = end_join - 1; k > start_join; k--)
        {
          indexes_to_remove[number_of_indexes_to_remove] = k;
          number_of_indexes_to_remove++;
        }

        start_join = end_join;
        end_join = start_join + 1;
      }
    }

    qsort(indexes_to_remove, number_of_indexes_to_remove, sizeof(int), backward_sort);

    int l = 0;
    for (l = 0;l < number_of_indexes_to_remove;l++)
    {
      int err = splt_sp_remove_splitpoint(state, indexes_to_remove[l]);
      if (err < 0) { *error = err; goto end; }
    }

    if (state->split.points->real_splitnumber == 2)
    {
      break;
    }

    free_intervals(intervals);

    intervals =
      splt_sp_find_intervals_between_two_consecutive_big_tracks(state, min_track_join, error);
  }

end:
  free_intervals(intervals);
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

