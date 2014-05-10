/**********************************************************
 *
 *                for mp3/ogg splitting without decoding
 * mp3splt-gtk -- utility based on mp3splt,
 *
 * Copyright: (C) 2005-2014 Alexandru Munteanu
 * Contact: m@ioalex.net
 *
 * http://mp3splt.sourceforge.net/
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

/*!********************************************************
 * \file 
 * The Douglas Peucker algorithm used to reduce the number of points of the amplitude
 * wave curve
 *
 **********************************************************/

#include "douglas_peucker.h"
#include "utilities.h"

static GArray *splt_copy_as_new_array(GArray *array);
static GArray *splt_merge_arrays_with_bounds(GArray *first, gint first_end_bound, 
    GArray *second, gint second_end_bound);
static GArray *splt_recursive_douglas_peucker(GArray *douglas_points, 
    void (*callback)(ui_state *ui), ui_state *ui, gdouble threshold_to_discard_point);
static GArray *splt_split_as_new_array(GArray *array, gint start_index, gint end_index);
static GArray *build_input_douglas_points(GArray *gdk_points);
static GArray *build_presence_array(GArray *output_douglas_points, guint length);
static GArray *splt_douglas_peucker_for_one_threshold(GArray *input_douglas_points, 
    void (*callback)(ui_state *ui), ui_state *ui, gdouble threshold_to_discard_point);

//returns an array of arrays of ints
//for each threshold, an [arrays of int] set to 1 if the point has been chosen, 0 otherwise
GPtrArray *splt_douglas_peucker(GArray *gdk_points, void (*callback)(ui_state *ui), ui_state *ui,
    gdouble threshold_to_discard_points, ...)
{
  GArray *input_douglas_points = build_input_douglas_points(gdk_points);
  guint length = input_douglas_points->len;

  GPtrArray *presence_points_by_threshold = g_ptr_array_new();

  va_list ap;
  va_start(ap, threshold_to_discard_points);
  while (threshold_to_discard_points > 0)
  {
    GArray *output_douglas_points =
      splt_douglas_peucker_for_one_threshold(input_douglas_points, callback, ui,
          threshold_to_discard_points);

    GArray *presence_array = build_presence_array(output_douglas_points, length);
    g_array_free(output_douglas_points, TRUE);

    g_ptr_array_add(presence_points_by_threshold, (gpointer) presence_array);

    threshold_to_discard_points = va_arg(ap, gdouble);
  }
  va_end(ap);

  g_array_free(input_douglas_points, TRUE);

  return presence_points_by_threshold;
}

void splt_douglas_peucker_free(GPtrArray *douglas_peucker_ptr_array)
{
  if (douglas_peucker_ptr_array == NULL)
  {
    return;
  }

  gint i = 0;
  for (i = 0;i < douglas_peucker_ptr_array->len; i++)
  {
    g_array_free(g_ptr_array_index(douglas_peucker_ptr_array, i), TRUE);
  }

  g_ptr_array_free(douglas_peucker_ptr_array, TRUE);
}

static GArray *splt_douglas_peucker_for_one_threshold(GArray *input_douglas_points, 
    void (*callback)(ui_state *ui), ui_state *ui, gdouble threshold_to_discard_point)
{
  GArray *output_douglas_points =
    splt_recursive_douglas_peucker(splt_copy_as_new_array(input_douglas_points), callback, ui,
        threshold_to_discard_point);

  return output_douglas_points;
}

static gint douglas_points_sort(gconstpointer first, gconstpointer second)
{
  const douglas_point *first_douglas_point = (douglas_point *)first;
  const douglas_point *second_douglas_point = (douglas_point *)second;

  return first_douglas_point->index - second_douglas_point->index;
}

static GArray *build_presence_array(GArray *output_douglas_points, guint length)
{
  g_array_sort(output_douglas_points, douglas_points_sort);

  GArray *presence_array = g_array_new(TRUE, TRUE, sizeof(gint));

  gint current_point_index = -1;
  gint output_douglas_points_index = 0;

  if (output_douglas_points->len > 0)
  {
    douglas_point current_point =
      g_array_index(output_douglas_points, douglas_point, output_douglas_points_index);
    current_point_index = current_point.index;
  }

  gint false_value = 0;
  gint true_value = 1;

  gint i = 0;
  for (i = 0;i < length; i++)
  {
    if (current_point_index == i)
    {
      output_douglas_points_index++;
      douglas_point point =
        g_array_index(output_douglas_points, douglas_point, output_douglas_points_index);
      current_point_index = point.index;
      g_array_append_val(presence_array, true_value);
      continue;
    }

    g_array_append_val(presence_array, false_value);
  }

  return presence_array;
}

static GArray *build_input_douglas_points(GArray *gdk_points)
{
  GArray *input_douglas_points = g_array_new(TRUE, TRUE, sizeof(douglas_point));

  gint i = 0;
  for (i = 0; i < gdk_points->len; i++)
  {
    GdkPoint point = g_array_index(gdk_points, GdkPoint, i);

    douglas_point douglas;
    douglas.point = point;
    douglas.index = i;

    g_array_append_val(input_douglas_points, douglas);
  }

  return input_douglas_points;
}

static GArray *splt_recursive_douglas_peucker(GArray *douglas_points, void (*callback)(ui_state *ui), 
    ui_state *ui, gdouble threshold_to_discard_point)
{
  GArray *new_points = NULL;

  if (douglas_points->len <= 2)
  {
    new_points = splt_copy_as_new_array(douglas_points);
    g_array_free(douglas_points, TRUE);
    return new_points;
  }

  if (callback != NULL)
  {
    callback(ui);
  }

  douglas_point first_point = g_array_index(douglas_points, douglas_point, 0);
  douglas_point last_point = g_array_index(douglas_points, douglas_point, douglas_points->len - 1);

  distance_and_index *max_distance_point =
    splt_find_point_with_maximum_distance(douglas_points, first_point.point, last_point.point);

  if (max_distance_point == NULL || double_equals(max_distance_point->distance, 0))
  {
    new_points = splt_copy_as_new_array(douglas_points);
    g_array_free(douglas_points, TRUE);

    if (max_distance_point != NULL) { g_free(max_distance_point); }

    return new_points;
  }

  if (max_distance_point->distance >= threshold_to_discard_point)
  {
    GArray *first_half_points = 
      splt_split_as_new_array(douglas_points, 0, max_distance_point->index);
    GArray *first_half_filtered_points = 
      splt_recursive_douglas_peucker(first_half_points, callback, ui, threshold_to_discard_point);

    GArray *second_half_points =
      splt_split_as_new_array(douglas_points, max_distance_point->index, douglas_points->len - 1);
    GArray *second_half_filtered_points = 
      splt_recursive_douglas_peucker(second_half_points, callback, ui, threshold_to_discard_point);

    new_points = 
      splt_merge_arrays_with_bounds(first_half_filtered_points, first_half_filtered_points->len - 2,
          second_half_filtered_points, second_half_filtered_points->len - 1);

    g_array_free(first_half_filtered_points, TRUE);
    g_array_free(second_half_filtered_points, TRUE);
  }
  else
  {
    new_points = g_array_new(TRUE, TRUE, sizeof(douglas_point));
    g_array_append_val(new_points, first_point);
    g_array_append_val(new_points, last_point);
  }

  g_free(max_distance_point);
  g_array_free(douglas_points, TRUE);

  return new_points;
}

distance_and_index *splt_find_point_with_maximum_distance(GArray *douglas_points,
    GdkPoint first_point, GdkPoint last_point)
{
  distance_and_index *max_distance_point = malloc(sizeof(*max_distance_point));
  if (max_distance_point == NULL)
  {
    return NULL;
  }

  max_distance_point->index = 0;
  max_distance_point->distance = 0;

  gint i = 0;
  for (i = 1; i < douglas_points->len - 1; i++)
  {
    douglas_point point = g_array_index(douglas_points, douglas_point, i);

    gdouble perpendicular_distance =
      splt_find_perpendicular_distance(point.point, first_point, last_point);
    if (perpendicular_distance <= max_distance_point->distance)
    {
      continue;
    }

    max_distance_point->index = i;
    max_distance_point->distance = perpendicular_distance;
  }

  return max_distance_point;
}

gdouble splt_find_distance(GdkPoint first, GdkPoint second)
{
  return sqrt(pow(second.x - first.x, 2) + pow(second.y - first.y, 2));
}

gdouble splt_find_perpendicular_distance(GdkPoint point, 
    GdkPoint segment_begin_point, GdkPoint segment_end_point)
{
  gdouble distance_A = splt_find_distance(segment_begin_point, point); 
  gdouble distance_B = splt_find_distance(point, segment_end_point); 
  gdouble distance_C = splt_find_distance(segment_begin_point, segment_end_point); 

  gdouble semiperimeter = (distance_A + distance_B + distance_C) / 2.0;

  gdouble perpendicular_distance =
    2.0 * sqrt(semiperimeter * 
        (semiperimeter - distance_A) *
        (semiperimeter - distance_B) *
        (semiperimeter - distance_C)) / distance_C;

  return perpendicular_distance;
}

static void splt_append_array_with_bounds(GArray *source, GArray *target,
    gint start_index, gint end_index)
{
  gint i = 0;
  for (i = start_index;i <= end_index;i++)
  {
    douglas_point point = g_array_index(source, douglas_point, i);
    g_array_append_val(target, point);
  }
}

static GArray *splt_split_as_new_array(GArray *array, gint start_index, gint end_index)
{
  GArray *new_array = g_array_new(TRUE, TRUE, g_array_get_element_size(array));
  splt_append_array_with_bounds(array, new_array, start_index, end_index); 
  return new_array;
}

static void splt_append_array(GArray *source, GArray *target)
{
  splt_append_array_with_bounds(source, target, 0, source->len - 1);
}

static GArray *splt_copy_as_new_array(GArray *array) {
  GArray *new_array = g_array_new(TRUE, TRUE, g_array_get_element_size(array));
  splt_append_array(array, new_array);
  return new_array;
}

static GArray *splt_merge_arrays_with_bounds(GArray *first, gint first_end_bound, 
    GArray *second, gint second_end_bound)
{
  GArray *new_array = splt_split_as_new_array(first, 0, first_end_bound);
  splt_append_array_with_bounds(second, new_array, 0, second_end_bound);
  return new_array;
}

