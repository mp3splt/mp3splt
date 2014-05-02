#include <cutter.h>

#include "libmp3splt/mp3splt.h"
#include "pair.h"
#include "splt_array.h"
#include "split_points.h"

static int min_track_join = 5;

static int error = SPLT_OK;
static splt_state *state = NULL;

void cut_setup()
{
  state = mp3splt_new_state(NULL);
  error = SPLT_OK;
}

void cut_teardown()
{
  mp3splt_free_state(state);
}

void test_find_intervals_with_no_splitpoint()
{
  splt_array *intervals = 
    splt_sp_find_intervals_between_two_consecutive_big_tracks(state, min_track_join, &error);

  cut_assert_equal_int(SPLT_OK, error);

  cut_assert_not_null(intervals);
  cut_assert_equal_int(0, splt_array_length(intervals));

  splt_array_free(&intervals);
}

void test_find_intervals_with_only_one_splitpoint_should_not_be_possible()
{
  splt_sp_append_splitpoint(state, 0, "", SPLT_SPLITPOINT);

  splt_array *intervals = 
    splt_sp_find_intervals_between_two_consecutive_big_tracks(state, min_track_join, &error);

  cut_assert_equal_int(SPLT_OK, error);

  cut_assert_not_null(intervals);
  cut_assert_equal_int(0, splt_array_length(intervals));

  splt_array_free(&intervals);
}

void test_find_intervals_with_one_segment_greater_than_minimum()
{
  splt_sp_append_splitpoint(state, 0, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 6, "", SPLT_SPLITPOINT);

  splt_array *intervals = 
    splt_sp_find_intervals_between_two_consecutive_big_tracks(state, min_track_join, &error);

  cut_assert_equal_int(SPLT_OK, error);

  cut_assert_not_null(intervals);
  cut_assert_equal_int(0, splt_array_length(intervals));

  splt_array_free(&intervals);
}

void test_find_intervals_with_one_segment_lesser_than_minimum()
{
  splt_sp_append_splitpoint(state, 0, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 4, "", SPLT_SPLITPOINT);

  splt_array *intervals = 
    splt_sp_find_intervals_between_two_consecutive_big_tracks(state, min_track_join, &error);

  cut_assert_equal_int(SPLT_OK, error);

  cut_assert_not_null(intervals);
  cut_assert_equal_int(1, splt_array_length(intervals));

  splt_int_pair *pair = (splt_int_pair *)splt_array_get(intervals, 0);
  int start = splt_int_pair_first(pair);
  int end = splt_int_pair_second(pair);

  cut_assert_equal_int(0, start);
  cut_assert_equal_int(1, end);

  splt_int_pair_free(&pair);

  splt_array_free(&intervals);
}

void test_find_intervals_with_no_segment_lesser_than_minimum()
{
  splt_sp_append_splitpoint(state, 0, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 6, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 12, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 18, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 24, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 30, "", SPLT_SPLITPOINT);

  splt_array *intervals = 
    splt_sp_find_intervals_between_two_consecutive_big_tracks(state, min_track_join, &error);

  cut_assert_equal_int(SPLT_OK, error);

  cut_assert_not_null(intervals);
  cut_assert_equal_int(0, splt_array_length(intervals));

  splt_array_free(&intervals);
}

void test_find_intervals_with_all_segments_lesser_than_minimum()
{
  splt_sp_append_splitpoint(state, 0, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 3, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 6, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 9, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 12, "", SPLT_SPLITPOINT);

  splt_array *intervals = 
    splt_sp_find_intervals_between_two_consecutive_big_tracks(state, min_track_join, &error);

  cut_assert_equal_int(SPLT_OK, error);

  cut_assert_not_null(intervals);
  cut_assert_equal_int(1, splt_array_length(intervals));

  splt_int_pair *pair = (splt_int_pair *)splt_array_get(intervals, 0);
  int start = splt_int_pair_first(pair);
  int end = splt_int_pair_second(pair);

  cut_assert_equal_int(0, start);
  cut_assert_equal_int(4, end);

  splt_int_pair_free(&pair);

  splt_array_free(&intervals);
}

void test_find_intervals_with_no_segment_lesser_than_minimum_at_beginning()
{
  splt_sp_append_splitpoint(state, 0, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 6, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 12, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 18, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 20, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 22, "", SPLT_SPLITPOINT);

  splt_array *intervals = 
    splt_sp_find_intervals_between_two_consecutive_big_tracks(state, min_track_join, &error);

  cut_assert_equal_int(SPLT_OK, error);

  cut_assert_not_null(intervals);
  cut_assert_equal_int(1, splt_array_length(intervals));

  splt_int_pair *pair = (splt_int_pair *)splt_array_get(intervals, 0);
  int start = splt_int_pair_first(pair);
  int end = splt_int_pair_second(pair);

  cut_assert_equal_int(2, start);
  cut_assert_equal_int(5, end);

  splt_int_pair_free(&pair);

  splt_array_free(&intervals);
}

void test_find_intervals_with_no_segment_lesser_than_minimum_at_end()
{
  splt_sp_append_splitpoint(state, 0, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 4, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 8, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 20, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 30, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 40, "", SPLT_SPLITPOINT);

  splt_array *intervals = 
    splt_sp_find_intervals_between_two_consecutive_big_tracks(state, min_track_join, &error);

  cut_assert_equal_int(SPLT_OK, error);

  cut_assert_not_null(intervals);
  cut_assert_equal_int(1, splt_array_length(intervals));

  splt_int_pair *pair = (splt_int_pair *)splt_array_get(intervals, 0);
  int start = splt_int_pair_first(pair);
  int end = splt_int_pair_second(pair);

  cut_assert_equal_int(0, start);
  cut_assert_equal_int(3, end);

  splt_int_pair_free(&pair);

  splt_array_free(&intervals);
}

void test_find_intervals_with_no_segment_lesser_than_minimum_in_the_middle()
{
  splt_sp_append_splitpoint(state, 0, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 4, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 8, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 20, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 30, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 40, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 42, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 44, "", SPLT_SPLITPOINT);

  splt_array *intervals = 
    splt_sp_find_intervals_between_two_consecutive_big_tracks(state, min_track_join, &error);

  cut_assert_equal_int(SPLT_OK, error);

  cut_assert_not_null(intervals);
  cut_assert_equal_int(2, splt_array_length(intervals));

  splt_int_pair *pair = (splt_int_pair *)splt_array_get(intervals, 0);
  int start = splt_int_pair_first(pair);
  int end = splt_int_pair_second(pair);

  cut_assert_equal_int(0, start);
  cut_assert_equal_int(3, end);

  splt_int_pair_free(&pair);

  pair = (splt_int_pair *)splt_array_get(intervals, 1);
  start = splt_int_pair_first(pair);
  end = splt_int_pair_second(pair);

  cut_assert_equal_int(4, start);
  cut_assert_equal_int(7, end);

  splt_int_pair_free(&pair);

  splt_array_free(&intervals);
}

void test_find_intervals_with_only_one_small_segment_in_the_middle_of_big_segments()
{
  splt_sp_append_splitpoint(state, 0, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 10, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 20, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 22, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 30, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 40, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 50, "", SPLT_SPLITPOINT);

  splt_array *intervals = 
    splt_sp_find_intervals_between_two_consecutive_big_tracks(state, min_track_join, &error);

  cut_assert_equal_int(SPLT_OK, error);

  cut_assert_not_null(intervals);
  cut_assert_equal_int(1, splt_array_length(intervals));

  splt_int_pair *pair = (splt_int_pair *)splt_array_get(intervals, 0);
  int start = splt_int_pair_first(pair);
  int end = splt_int_pair_second(pair);

  cut_assert_equal_int(1, start);
  cut_assert_equal_int(4, end);

  splt_int_pair_free(&pair);

  splt_array_free(&intervals);
}

void test_find_intervals_with_single_big_distance_in_interval()
{
  splt_sp_append_splitpoint(state, 0, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 10, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 20, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 22, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 30, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 31, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 40, "", SPLT_SPLITPOINT);

  splt_array *intervals = 
    splt_sp_find_intervals_between_two_consecutive_big_tracks(state, min_track_join, &error);

  cut_assert_equal_int(SPLT_OK, error);

  cut_assert_not_null(intervals);
  cut_assert_equal_int(1, splt_array_length(intervals));

  splt_int_pair *pair = (splt_int_pair *)splt_array_get(intervals, 0);
  int start = splt_int_pair_first(pair);
  int end = splt_int_pair_second(pair);

  cut_assert_equal_int(1, start);
  cut_assert_equal_int(6, end);

  splt_int_pair_free(&pair);

  splt_array_free(&intervals);
}

void test_find_intervals_complex_with_several_intervals()
{
  splt_sp_append_splitpoint(state, 0, "", SPLT_SPLITPOINT);

  //1
  splt_sp_append_splitpoint(state, 10, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 20, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 22, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 30, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 31, "", SPLT_SPLITPOINT);
  //6
  splt_sp_append_splitpoint(state, 40, "", SPLT_SPLITPOINT);

  splt_sp_append_splitpoint(state, 50, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 51, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 52, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 62, "", SPLT_SPLITPOINT);
  //10

  //11
  splt_sp_append_splitpoint(state, 72, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 82, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 83, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 93, "", SPLT_SPLITPOINT);
  //14

  splt_sp_append_splitpoint(state, 103, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 203, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 303, "", SPLT_SPLITPOINT);

  splt_array *intervals = 
    splt_sp_find_intervals_between_two_consecutive_big_tracks(state, min_track_join, &error);

  cut_assert_equal_int(SPLT_OK, error);

  cut_assert_not_null(intervals);
  cut_assert_equal_int(3, splt_array_length(intervals));

  splt_int_pair *pair = (splt_int_pair *)splt_array_get(intervals, 0);
  int start = splt_int_pair_first(pair);
  int end = splt_int_pair_second(pair);

  cut_assert_equal_int(1, start);
  cut_assert_equal_int(6, end);

  splt_int_pair_free(&pair);

  pair = (splt_int_pair *)splt_array_get(intervals, 1);
  start = splt_int_pair_first(pair);
  end = splt_int_pair_second(pair);

  cut_assert_equal_int(6, start);
  cut_assert_equal_int(10, end);

  splt_int_pair_free(&pair);

  pair = (splt_int_pair *)splt_array_get(intervals, 2);
  start = splt_int_pair_first(pair);
  end = splt_int_pair_second(pair);

  cut_assert_equal_int(11, start);
  cut_assert_equal_int(14, end);

  splt_int_pair_free(&pair);

  splt_array_free(&intervals);
}

void test_join_tracks_with_only_small_segments()
{
  mp3splt_set_float_option(state, SPLT_OPT_PARAM_MIN_TRACK_JOIN, min_track_join / 100.0);

  splt_sp_append_splitpoint(state, 0, "", SPLT_SPLITPOINT);

  //remove
  splt_sp_append_splitpoint(state, 1, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 2, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 3, "", SPLT_SPLITPOINT);

  splt_sp_append_splitpoint(state, 4, "", SPLT_SPLITPOINT);

  splt_sp_join_minimum_tracks_splitpoints(state, &error);

  cut_assert_equal_int(SPLT_OK, error);

  splt_points *points = mp3splt_get_splitpoints(state, &error);

  cut_assert_equal_int(SPLT_OK, error);

  mp3splt_points_init_iterator(points);

  const splt_point *point = mp3splt_points_next(points);
  cut_assert_equal_int(0, mp3splt_point_get_value(point));

  point = mp3splt_points_next(points);
  cut_assert_equal_int(4, mp3splt_point_get_value(point));

  cut_assert_null(mp3splt_points_next(points));
}

void test_join_tracks_with_only_big_segments()
{
  mp3splt_set_float_option(state, SPLT_OPT_PARAM_MIN_TRACK_JOIN, min_track_join / 100.0);

  splt_sp_append_splitpoint(state, 0, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 10, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 20, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 30, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 40, "", SPLT_SPLITPOINT);

  splt_sp_join_minimum_tracks_splitpoints(state, &error);

  cut_assert_equal_int(SPLT_OK, error);

  splt_points *points = mp3splt_get_splitpoints(state, &error);

  cut_assert_equal_int(SPLT_OK, error);

  mp3splt_points_init_iterator(points);

  const splt_point *point = mp3splt_points_next(points);
  cut_assert_equal_int(0, mp3splt_point_get_value(point));

  point = mp3splt_points_next(points);
  cut_assert_equal_int(10, mp3splt_point_get_value(point));

  point = mp3splt_points_next(points);
  cut_assert_equal_int(20, mp3splt_point_get_value(point));

  point = mp3splt_points_next(points);
  cut_assert_equal_int(30, mp3splt_point_get_value(point));

  point = mp3splt_points_next(points);
  cut_assert_equal_int(40, mp3splt_point_get_value(point));

  cut_assert_null(mp3splt_points_next(points));
}

void test_join_tracks_with_small_segments_at_start()
{
  mp3splt_set_float_option(state, SPLT_OPT_PARAM_MIN_TRACK_JOIN, min_track_join / 100.0);

  splt_sp_append_splitpoint(state, 0, "", SPLT_SPLITPOINT);

  //remove
  splt_sp_append_splitpoint(state, 1, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 2, "", SPLT_SPLITPOINT);

  splt_sp_append_splitpoint(state, 30, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 40, "", SPLT_SPLITPOINT);

  splt_sp_join_minimum_tracks_splitpoints(state, &error);

  cut_assert_equal_int(SPLT_OK, error);

  splt_points *points = mp3splt_get_splitpoints(state, &error);

  cut_assert_equal_int(SPLT_OK, error);

  mp3splt_points_init_iterator(points);

  const splt_point *point = mp3splt_points_next(points);
  cut_assert_equal_int(0, mp3splt_point_get_value(point));

  point = mp3splt_points_next(points);
  cut_assert_equal_int(30, mp3splt_point_get_value(point));

  point = mp3splt_points_next(points);
  cut_assert_equal_int(40, mp3splt_point_get_value(point));

  cut_assert_null(mp3splt_points_next(points));
}

void test_join_tracks_with_small_segments_in_the_middle()
{
  mp3splt_set_float_option(state, SPLT_OPT_PARAM_MIN_TRACK_JOIN, min_track_join / 100.0);

  splt_sp_append_splitpoint(state, 0, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 10, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 21, "", SPLT_SPLITPOINT);

  //remove
  splt_sp_append_splitpoint(state, 22, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 23, "", SPLT_SPLITPOINT);

  splt_sp_append_splitpoint(state, 30, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 40, "", SPLT_SPLITPOINT);

  splt_sp_join_minimum_tracks_splitpoints(state, &error);

  cut_assert_equal_int(SPLT_OK, error);

  splt_points *points = mp3splt_get_splitpoints(state, &error);

  cut_assert_equal_int(SPLT_OK, error);

  mp3splt_points_init_iterator(points);

  const splt_point *point = mp3splt_points_next(points);
  cut_assert_equal_int(0, mp3splt_point_get_value(point));

  point = mp3splt_points_next(points);
  cut_assert_equal_int(10, mp3splt_point_get_value(point));

  point = mp3splt_points_next(points);
  cut_assert_equal_int(21, mp3splt_point_get_value(point));

  point = mp3splt_points_next(points);
  cut_assert_equal_int(30, mp3splt_point_get_value(point));

  point = mp3splt_points_next(points);
  cut_assert_equal_int(40, mp3splt_point_get_value(point));

  cut_assert_null(mp3splt_points_next(points));
}

void test_join_tracks_complex()
{
  mp3splt_set_float_option(state, SPLT_OPT_PARAM_MIN_TRACK_JOIN, min_track_join / 100.0);

  splt_sp_append_splitpoint(state, 0, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 10, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 21, "", SPLT_SPLITPOINT);

  //remove
  splt_sp_append_splitpoint(state, 22, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 23, "", SPLT_SPLITPOINT);

  splt_sp_append_splitpoint(state, 30, "", SPLT_SPLITPOINT);

  //remove
  splt_sp_append_splitpoint(state, 31, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 32, "", SPLT_SPLITPOINT);

  splt_sp_append_splitpoint(state, 40, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 50, "", SPLT_SPLITPOINT);

  splt_sp_join_minimum_tracks_splitpoints(state, &error);

  cut_assert_equal_int(SPLT_OK, error);

  splt_points *points = mp3splt_get_splitpoints(state, &error);

  cut_assert_equal_int(SPLT_OK, error);

  mp3splt_points_init_iterator(points);

  const splt_point *point = mp3splt_points_next(points);
  cut_assert_equal_int(0, mp3splt_point_get_value(point));

  point = mp3splt_points_next(points);
  cut_assert_equal_int(10, mp3splt_point_get_value(point));

  point = mp3splt_points_next(points);
  cut_assert_equal_int(21, mp3splt_point_get_value(point));

  point = mp3splt_points_next(points);
  cut_assert_equal_int(30, mp3splt_point_get_value(point));

  point = mp3splt_points_next(points);
  cut_assert_equal_int(40, mp3splt_point_get_value(point));

  point = mp3splt_points_next(points);
  cut_assert_equal_int(50, mp3splt_point_get_value(point));

  cut_assert_null(mp3splt_points_next(points));
}

void test_join_tracks_even_more_complex()
{
  mp3splt_set_float_option(state, SPLT_OPT_PARAM_MIN_TRACK_JOIN, min_track_join / 100.0);

  splt_sp_append_splitpoint(state, 0, "", SPLT_SPLITPOINT);

  //remove
  splt_sp_append_splitpoint(state, 1, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 2, "", SPLT_SPLITPOINT);

  splt_sp_append_splitpoint(state, 10, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 21, "", SPLT_SPLITPOINT);

  //remove
  splt_sp_append_splitpoint(state, 22, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 23, "", SPLT_SPLITPOINT);

  splt_sp_append_splitpoint(state, 30, "", SPLT_SPLITPOINT);

  //remove
  splt_sp_append_splitpoint(state, 31, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 32, "", SPLT_SPLITPOINT);

  splt_sp_append_splitpoint(state, 40, "", SPLT_SPLITPOINT);

  //remove
  splt_sp_append_splitpoint(state, 50, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 51, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 52, "", SPLT_SPLITPOINT);

  splt_sp_append_splitpoint(state, 53, "", SPLT_SPLITPOINT);

  splt_sp_join_minimum_tracks_splitpoints(state, &error);

  cut_assert_equal_int(SPLT_OK, error);

  splt_points *points = mp3splt_get_splitpoints(state, &error);

  cut_assert_equal_int(SPLT_OK, error);

  mp3splt_points_init_iterator(points);

  const splt_point *point = mp3splt_points_next(points);
  cut_assert_equal_int(0, mp3splt_point_get_value(point));

  point = mp3splt_points_next(points);
  cut_assert_equal_int(10, mp3splt_point_get_value(point));

  point = mp3splt_points_next(points);
  cut_assert_equal_int(21, mp3splt_point_get_value(point));

  point = mp3splt_points_next(points);
  cut_assert_equal_int(30, mp3splt_point_get_value(point));

  point = mp3splt_points_next(points);
  cut_assert_equal_int(40, mp3splt_point_get_value(point));

  point = mp3splt_points_next(points);
  cut_assert_equal_int(53, mp3splt_point_get_value(point));

  cut_assert_null(mp3splt_points_next(points));
}

void test_join_tracks_small_parts_joined_bigger_than_big_part()
{
  mp3splt_set_float_option(state, SPLT_OPT_PARAM_MIN_TRACK_JOIN, min_track_join / 100.0);

  splt_sp_append_splitpoint(state, 0, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 10, "", SPLT_SPLITPOINT);

  //remove
  splt_sp_append_splitpoint(state, 12, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 14, "", SPLT_SPLITPOINT);

  splt_sp_append_splitpoint(state, 16, "", SPLT_SPLITPOINT);

  //remove
  splt_sp_append_splitpoint(state, 18, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 19, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 20, "", SPLT_SPLITPOINT);

  splt_sp_append_splitpoint(state, 30, "", SPLT_SPLITPOINT);

  splt_sp_join_minimum_tracks_splitpoints(state, &error);

  cut_assert_equal_int(SPLT_OK, error);

  splt_points *points = mp3splt_get_splitpoints(state, &error);

  cut_assert_equal_int(SPLT_OK, error);

  mp3splt_points_init_iterator(points);

  const splt_point *point = mp3splt_points_next(points);
  cut_assert_equal_int(0, mp3splt_point_get_value(point));

  point = mp3splt_points_next(points);
  cut_assert_equal_int(10, mp3splt_point_get_value(point));

  point = mp3splt_points_next(points);
  cut_assert_equal_int(16, mp3splt_point_get_value(point));

  point = mp3splt_points_next(points);
  cut_assert_equal_int(30, mp3splt_point_get_value(point));

  cut_assert_null(mp3splt_points_next(points));
}

void test_join_tracks_with_two_splitpoints()
{
  mp3splt_set_float_option(state, SPLT_OPT_PARAM_MIN_TRACK_JOIN, min_track_join / 100.0);

  splt_sp_append_splitpoint(state, 0, "", SPLT_SPLITPOINT);
  splt_sp_append_splitpoint(state, 2, "", SPLT_SPLITPOINT);

  splt_sp_join_minimum_tracks_splitpoints(state, &error);

  cut_assert_equal_int(SPLT_OK, error);

  splt_points *points = mp3splt_get_splitpoints(state, &error);

  cut_assert_equal_int(SPLT_OK, error);

  mp3splt_points_init_iterator(points);

  const splt_point *point = mp3splt_points_next(points);
  cut_assert_equal_int(0, mp3splt_point_get_value(point));

  point = mp3splt_points_next(points);
  cut_assert_equal_int(2, mp3splt_point_get_value(point));

  cut_assert_null(mp3splt_points_next(points));
}

void test_join_tracks_with_three_splitpoints()
{
  mp3splt_set_float_option(state, SPLT_OPT_PARAM_MIN_TRACK_JOIN, min_track_join / 100.0);

  splt_sp_append_splitpoint(state, 0, "", SPLT_SPLITPOINT);

  //remove
  splt_sp_append_splitpoint(state, 1, "", SPLT_SPLITPOINT);

  splt_sp_append_splitpoint(state, 2, "", SPLT_SPLITPOINT);

  splt_sp_join_minimum_tracks_splitpoints(state, &error);

  cut_assert_equal_int(SPLT_OK, error);

  splt_points *points = mp3splt_get_splitpoints(state, &error);

  cut_assert_equal_int(SPLT_OK, error);

  mp3splt_points_init_iterator(points);

  const splt_point *point = mp3splt_points_next(points);
  cut_assert_equal_int(0, mp3splt_point_get_value(point));

  point = mp3splt_points_next(points);
  cut_assert_equal_int(2, mp3splt_point_get_value(point));

  cut_assert_null(mp3splt_points_next(points));
}

