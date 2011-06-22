#include <cutter.h>

#include "splt_array.h"
#include "libmp3splt/mp3splt.h"
#include "string_utils.h"

static splt_array *array = NULL;

void cut_setup()
{
  array = splt_array_new();
}

void cut_teardown()
{
  if (array)
  {
    splt_array_free(&array);
  }
}

void test_new()
{
  cut_assert_not_null(array);
  cut_assert_equal_int(0, splt_array_length(array));
}

void test_append_and_get()
{
  int element1 = 183;
  splt_array_append(array, &element1);
  int element2 = 87;
  splt_array_append(array, &element2);

  cut_assert_equal_int(2, splt_array_length(array));

  cut_assert_equal_int(183, *((int *)splt_array_get(array, 0)));
  cut_assert_equal_int(87, *((int *)splt_array_get(array, 1)));

  cut_assert_null(splt_array_get(array, -1));
  cut_assert_null(splt_array_get(array, 2));
}

void test_append_and_get_char()
{
  char *element1 = NULL;
  splt_su_copy("test", &element1);
  splt_array_append(array, element1);
  char *element2 = NULL;
  splt_su_copy("second_test", &element2);
  splt_array_append(array, element2);

  cut_assert_equal_int(2, splt_array_length(array));

  cut_assert_equal_string("test", (char *)splt_array_get(array, 0));
  cut_assert_equal_string("second_test", (char *)splt_array_get(array, 1));

  if (element1) { free(element1); }
  if (element2) { free(element2); }
}

void test_clear()
{
  int element1 = 183;
  splt_array_append(array, &element1);
  int element2 = 87;
  splt_array_append(array, &element2);

  cut_assert_equal_int(2, splt_array_length(array));

  splt_array_clear(array);

  cut_assert_equal_int(0, splt_array_length(array));
  cut_assert_null(splt_array_get(array, 0));
}

void test_free()
{
  splt_array_free(&array);

  cut_assert_null(array);
}

