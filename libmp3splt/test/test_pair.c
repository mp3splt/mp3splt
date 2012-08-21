#include <cutter.h>

#include "pair.h"

#include "tests.h"

static splt_il_pair *pair = NULL;

void cut_setup()
{
}

void cut_teardown()
{
  if (pair)
  {
    splt_il_pair_free(&pair);
  }
}

void test_new()
{
  pair = splt_il_pair_new(0, 0);
  cut_assert_not_null(pair);
}

void test_get()
{
  int x = 22;
  long axis = 44;
  pair = splt_il_pair_new(x, axis);

  int xx = splt_il_pair_first(pair);
  long axiss = splt_il_pair_second(pair);

  cut_assert_equal_int(22, xx);
  cut_assert_equal_int(44, axiss);
}

void test_free()
{
  pair = splt_il_pair_new(0, 0);
  splt_il_pair_free(&pair);

  cut_assert_null(pair);
}

