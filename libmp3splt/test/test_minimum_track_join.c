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
  mp3splt_free_state(state, NULL);
}

void test_with_no_splitpoint()
{
  splt_array *intervals = 
    splt_sp_find_intervals_between_two_consecutive_big_tracks(state, min_track_join, &error);

  cut_assert_equal_int(SPLT_OK, error);

  cut_assert_not_null(intervals);
  cut_assert_equal_int(0, splt_array_length(intervals));

  splt_array_free(&intervals);
}

void test_with_only_one_splitpoint()
{
  splt_sp_append_splitpoint(state, 1, "", SPLT_SPLITPOINT);

  splt_array *intervals = 
    splt_sp_find_intervals_between_two_consecutive_big_tracks(state, min_track_join, &error);

  cut_assert_equal_int(SPLT_OK, error);

  cut_assert_not_null(intervals);
  cut_assert_equal_int(1, splt_array_length(intervals));

  splt_int_pair *pair = (splt_int_pair *) splt_array_get(intervals, 0);

  cut_assert_equal_int(0, splt_int_pair_first(pair));
  cut_assert_equal_int(1, splt_int_pair_second(pair));

  splt_int_pair_free(&pair);
  splt_array_free(&intervals);
}

