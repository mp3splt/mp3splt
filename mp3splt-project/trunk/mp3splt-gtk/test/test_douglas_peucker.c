#include <cutter.h>

#include <gtk/gtk.h>

#include "tests.h"
#include "douglas_peucker.h"

void test_find_points_distance()
{
  GdkPoint first_point;
  first_point.x = 15;
  first_point.y = 20;

  GdkPoint second_point;
  second_point.x = 35;
  second_point.y = 5;

  gdouble distance = splt_find_distance(first_point, second_point);
  cut_assert_equal_double(25.0, DOUBLE_PRECISION, distance);

  second_point.y = 6;
  distance = splt_find_distance(first_point, second_point);
  cut_assert_equal_double(24.41311123, DOUBLE_PRECISION, distance);
}

void test_find_perpendicular_distance()
{
  GdkPoint point;
  point.x = 5;
  point.y = 2;

  //3x - y + 6 = 0

  GdkPoint first_segment_point;
  first_segment_point.x = 1;
  first_segment_point.y = 9;

  GdkPoint second_segment_point;
  second_segment_point.x = 7;
  second_segment_point.y = 27;

  gdouble distance = splt_find_perpendicular_distance(point, first_segment_point, second_segment_point);
  cut_assert_equal_double(6.00832755, DOUBLE_PRECISION, distance);

  point.x = 1;
  point.y = 29;

  distance = splt_find_perpendicular_distance(point, first_segment_point, second_segment_point);
  cut_assert_equal_double(6.32455532, DOUBLE_PRECISION, distance);
}

void test_find_point_with_maximum_distance()
{
  GArray *points = g_array_new(TRUE, TRUE, sizeof(GdkPoint));

  GdkPoint first_segment_point;
  first_segment_point.x = 1;
  first_segment_point.y = 9;

  GdkPoint second_segment_point;
  second_segment_point.x = 7;
  second_segment_point.y = 27;

  g_array_append_val(points, first_segment_point);

  GdkPoint first_point;
  first_point.x = 5;
  first_point.y = 2;
  g_array_append_val(points, first_point);

  GdkPoint second_point;
  second_point.x = 3;
  second_point.y = 9;
  g_array_append_val(points, second_point);

  GdkPoint third_point;
  third_point.x = 1;
  third_point.y = 29;
  g_array_append_val(points, third_point);

  g_array_append_val(points, second_segment_point);

  distance_and_index *max_distance_point = 
    splt_find_point_with_maximum_distance(points, first_segment_point, second_segment_point);

  cut_assert_equal_int(3, max_distance_point->index);
  cut_assert_equal_double(27.51181564, DOUBLE_PRECISION, max_distance_point->distance);

  g_array_free(points, TRUE);
}

void test_douglas_peucker()
{
  GArray *points = g_array_new(TRUE, TRUE, sizeof(GdkPoint));

  GdkPoint first_segment_point;
  first_segment_point.x = 1;
  first_segment_point.y = 9;

  g_array_append_val(points, first_segment_point);

  GdkPoint first_point;
  first_point.x = 5;
  first_point.y = 2;
  g_array_append_val(points, first_point);

  GdkPoint second_point;
  second_point.x = 3;
  second_point.y = 9;
  g_array_append_val(points, second_point);

  GdkPoint third_point;
  third_point.x = 1;
  third_point.y = 29;
  g_array_append_val(points, third_point);

  GdkPoint fourth_point;
  fourth_point.x = 2;
  fourth_point.y = 29;
  g_array_append_val(points, fourth_point);

  GdkPoint second_segment_point;
  second_segment_point.x = 7;
  second_segment_point.y = 27;
  g_array_append_val(points, second_segment_point);

  //6.4 - filter all points except first segment and second segment
  //0.4 - filter just one point
  //4.1 - filter three points

  GPtrArray *points_presence_by_threshold = splt_douglas_peucker(points, NULL, NULL, 6.4, 0.4, 4.1, -1);
  cut_assert_equal_int(3, points_presence_by_threshold->len);

  //filter all points except first segment and second segment

  gdouble distance = splt_find_perpendicular_distance(first_point, first_segment_point, second_segment_point);
  cut_assert_equal_double(6.00832755, DOUBLE_PRECISION, distance);

  distance = splt_find_perpendicular_distance(second_point, first_segment_point, second_segment_point);
  cut_assert_equal_double(1.89736659, DOUBLE_PRECISION, distance);

  distance = splt_find_perpendicular_distance(third_point, first_segment_point, second_segment_point);
  cut_assert_equal_double(6.32455532, DOUBLE_PRECISION, distance);

  distance = splt_find_perpendicular_distance(fourth_point, first_segment_point, second_segment_point);
  cut_assert_equal_double(5.37587202, DOUBLE_PRECISION, distance);

  GArray *points_presence = g_ptr_array_index(points_presence_by_threshold, 0);

  cut_assert_equal_int(6, points_presence->len);

  cut_assert_equal_int(1, g_array_index(points_presence, gint, 0));
  cut_assert_equal_int(0, g_array_index(points_presence, gint, 1));
  cut_assert_equal_int(0, g_array_index(points_presence, gint, 2));
  cut_assert_equal_int(0, g_array_index(points_presence, gint, 3));
  cut_assert_equal_int(0, g_array_index(points_presence, gint, 4));
  cut_assert_equal_int(1, g_array_index(points_presence, gint, 5));

  //filter just one point

  //the 6.324... is found as the max silence, so we compute left part between the first
  //segment point and the third point and right part between third point and second
  //segment point

  //distance on the left part
  distance = splt_find_perpendicular_distance(first_point, first_segment_point, third_point);
  cut_assert_equal_double(4.0, DOUBLE_PRECISION, distance);

  distance = splt_find_perpendicular_distance(second_point, first_segment_point, third_point);
  cut_assert_equal_double(2.0, DOUBLE_PRECISION, distance);

  //distance on the right part
  distance = splt_find_perpendicular_distance(fourth_point, third_point, second_segment_point);
  cut_assert_equal_double(0.316227766, DOUBLE_PRECISION, distance);

  points_presence = g_ptr_array_index(points_presence_by_threshold, 1);

  cut_assert_equal_int(6, points_presence->len);

  cut_assert_equal_int(1, g_array_index(points_presence, gint, 0));
  cut_assert_equal_int(1, g_array_index(points_presence, gint, 1));
  cut_assert_equal_int(1, g_array_index(points_presence, gint, 2));
  cut_assert_equal_int(1, g_array_index(points_presence, gint, 3));
  cut_assert_equal_int(0, g_array_index(points_presence, gint, 4));
  cut_assert_equal_int(1, g_array_index(points_presence, gint, 5));

  //filter 3 points

  points_presence = g_ptr_array_index(points_presence_by_threshold, 2);

  cut_assert_equal_int(6, points_presence->len);

  cut_assert_equal_int(1, g_array_index(points_presence, gint, 0));
  cut_assert_equal_int(0, g_array_index(points_presence, gint, 1));
  cut_assert_equal_int(0, g_array_index(points_presence, gint, 2));
  cut_assert_equal_int(1, g_array_index(points_presence, gint, 3));
  cut_assert_equal_int(0, g_array_index(points_presence, gint, 4));
  cut_assert_equal_int(1, g_array_index(points_presence, gint, 5));

  splt_douglas_peucker_free(points_presence_by_threshold);
  g_array_free(points, TRUE);
}

void test_douglas_peucker_with_only_one_point()
{
  GArray *points = g_array_new(TRUE, TRUE, sizeof(GdkPoint));

  GdkPoint first_segment_point;
  first_segment_point.x = 1;
  first_segment_point.y = 9;

  g_array_append_val(points, first_segment_point);

  GPtrArray *points_presence_by_threshold = splt_douglas_peucker(points, NULL, NULL, 6.4, -1);
  cut_assert_equal_int(1, points_presence_by_threshold->len);

  GArray *presence_points = g_ptr_array_index(points_presence_by_threshold, 0);

  cut_assert_equal_int(1, presence_points->len);

  gint is_present = g_array_index(presence_points, gint, 0);
  cut_assert_equal_int(1, is_present);

  splt_douglas_peucker_free(points_presence_by_threshold);
  g_array_free(points, TRUE);
}

