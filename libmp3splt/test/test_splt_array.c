#include <cutter.h>
#include <splt_array.h>

static splt_array *array;

void cut_setup()
{
  array = NULL;
}

void cut_shutdown()
{
  if (array)
  {
    splt_array_free(&array);
  }
}

void test_new()
{
  array = splt_array_new();
  cut_assert_equal_int(0, splt_array_length(array));
}

void test_append_and_get()
{
  array = splt_array_new();

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

void test_clear()
{
  array = splt_array_new();

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
  array = splt_array_new();
  splt_array_free(&array);

  cut_assert_null(array);
}

