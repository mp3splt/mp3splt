#include <cutter.h>

#include "pair.h"

#include "tests.h"

static splt_pair *pair = NULL;

void cut_setup()
{
}

void cut_teardown()
{
  if (pair)
  {
    splt_pair_free(&pair);
  }
}

void test_new()
{
  pair = splt_pair_new(NULL, NULL);
  cut_assert_not_null(pair);
}

void test_get()
{
  int x = 22;
  double axis = 44.5;
  pair = splt_pair_new(&x, &axis);

  int xx = *((int *)splt_pair_first(pair));
  double axiss = *((double *)splt_pair_second(pair));

  cut_assert_equal_int(22, xx);
  cut_assert_equal_double(44.5, DOUBLE_PRECISION, axiss);
}

void test_free()
{
  pair = splt_pair_new(NULL, NULL);
  splt_pair_free(&pair);

  cut_assert_null(pair);
}

