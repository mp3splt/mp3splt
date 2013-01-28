#include <cutter.h>

#include "tests.h"

void mp3splt_assert_equal_error_message(splt_state *state, int error, char *expected_error)
{
  char *actual_error = mp3splt_get_strerror(state, error);
  cut_assert_equal_string(expected_error, actual_error);
  free(actual_error);
}

