#include <cutter.h>

#include "libmp3splt/mp3splt.h"

static splt_state *state = NULL;
static int error = SPLT_OK;

void cut_setup()
{
  state = mp3splt_new_state(NULL);
  error = SPLT_OK;
}

void cut_teardown()
{
  mp3splt_free_state(state);
}

void test_initial_splitpoints()
{
  splt_points *points = mp3splt_get_splitpoints(state, &error);
  cut_assert_equal_int(SPLT_OK, error);

  cut_assert_null(points);

  mp3splt_points_init_iterator(points);
  cut_assert_null(mp3splt_points_next(points));
  cut_assert_null(mp3splt_points_next(points));
}

void test_initial_erase_all_splitpoints()
{
  error = mp3splt_erase_all_splitpoints(state);
  cut_assert_equal_int(SPLT_OK, error);

  splt_points *points = mp3splt_get_splitpoints(state, &error);
  cut_assert_equal_int(SPLT_OK, error);

  cut_assert_null(points);
}

void test_append_and_return_splitpoints()
{
  //first splitpoint
  splt_point *point1 = mp3splt_point_new(1 * 60 * 100, &error);
  cut_assert_equal_int(SPLT_OK, error);
  error = mp3splt_point_set_name(point1, "first_point_name");
  cut_assert_equal_int(SPLT_OK, error);
  error = mp3splt_point_set_type(point1, SPLT_SPLITPOINT);
  cut_assert_equal_int(SPLT_OK, error);

  //append first splitpoint
  error = mp3splt_append_splitpoint(state, point1);
  cut_assert_equal_int(SPLT_OK, error);

  //second splitpoint
  splt_point *point2 = mp3splt_point_new(4 * 60 * 100, &error);
  cut_assert_equal_int(SPLT_OK, error);
  error = mp3splt_point_set_name(point2, "second_point_name");
  cut_assert_equal_int(SPLT_OK, error);
  error = mp3splt_point_set_type(point2, SPLT_SKIPPOINT);
  cut_assert_equal_int(SPLT_OK, error);

  //append second splitpoint
  error = mp3splt_append_splitpoint(state, point2);
  cut_assert_equal_int(SPLT_OK, error);

  splt_points *points = mp3splt_get_splitpoints(state, &error);
  cut_assert_equal_int(SPLT_OK, error);
  mp3splt_points_init_iterator(points);

  //check first splitpoint
  const splt_point *check_point1 = mp3splt_points_next(points);
  cut_assert_equal_int(1 * 60 * 100, mp3splt_point_get_value(check_point1));
  cut_assert_equal_int(SPLT_SPLITPOINT, mp3splt_point_get_type(check_point1));
  char *name1 = mp3splt_point_get_name(check_point1);
  cut_assert_equal_string("first_point_name", name1);
  free(name1);

  //check second splitpoint
  const splt_point *check_point2 = mp3splt_points_next(points);
  cut_assert_equal_int(4 * 60 * 100, mp3splt_point_get_value(check_point2));
  cut_assert_equal_int(SPLT_SKIPPOINT, mp3splt_point_get_type(check_point2));
  char *name2 = mp3splt_point_get_name(check_point2);
  cut_assert_equal_string("second_point_name", name2);
  free(name2);

  cut_assert_null(mp3splt_points_next(points));
}

void test_append_and_erase_all_splitpoints()
{
  splt_point *point1 = mp3splt_point_new(1 * 60 * 100, &error);
  cut_assert_equal_int(SPLT_OK, error);
  error = mp3splt_point_set_name(point1, "first_point_name");
  cut_assert_equal_int(SPLT_OK, error);
  error = mp3splt_point_set_type(point1, SPLT_SPLITPOINT);
  cut_assert_equal_int(SPLT_OK, error);

  //append splitpoint
  error = mp3splt_append_splitpoint(state, point1);
  cut_assert_equal_int(SPLT_OK, error);

  error = mp3splt_erase_all_splitpoints(state);
  cut_assert_equal_int(SPLT_OK, error);

  splt_points *points = mp3splt_get_splitpoints(state, &error);
  cut_assert_null(points);
}

