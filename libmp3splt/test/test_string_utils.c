#include <cutter.h>

#include "libmp3splt/mp3splt.h"
#include "string_utils.h"

static int error = SPLT_OK;
static splt_state *state = NULL;
static char *converted_str = NULL;

static char *sample_text = { "juSt a sAmple of 2 bIts" };

void cut_setup()
{
  state = mp3splt_new_state(NULL);
  error = SPLT_OK;
  converted_str = NULL;
}

void cut_teardown()
{
  mp3splt_free_state(state, NULL);
  if (converted_str)
  {
    free(converted_str);
    converted_str = NULL;
  }
}

void test_replace_all()
{
  char *path = splt_su_replace_all("/my//new//path", "//", "/", &error);
  cut_assert_equal_int(SPLT_OK, error);

  if (path)
  {
    cut_assert_equal_string("/my/new/path", path);
    free(path);
  }
}

void test_convert_with_null_str()
{
  converted_str = splt_su_convert(NULL, SPLT_TO_LOWERCASE, &error);
 
  cut_assert_equal_int(SPLT_OK, error);
  cut_assert_null(converted_str); 
}

void test_convert_with_empty_str()
{
  converted_str = splt_su_convert("", SPLT_TO_LOWERCASE, &error);

  cut_assert_equal_int(SPLT_OK, error);
  cut_assert_equal_string("", converted_str); 
}

void test_convert_with_no_conversion()
{
  converted_str = splt_su_convert(sample_text, SPLT_NO_CONVERSION, &error);

  cut_assert_equal_int(SPLT_OK, error);
  cut_assert_equal_string(sample_text, converted_str); 
}

void test_convert_with_conversion_to_uppercase()
{
  converted_str = splt_su_convert(sample_text, SPLT_TO_UPPERCASE, &error);

  cut_assert_equal_int(SPLT_OK, error);
  cut_assert_equal_string("JUST A SAMPLE OF 2 BITS", converted_str); 
}

void test_convert_with_conversion_to_lowercase()
{
  converted_str = splt_su_convert(sample_text, SPLT_TO_LOWERCASE, &error);

  cut_assert_equal_int(SPLT_OK, error);
  cut_assert_equal_string("just a sample of 2 bits", converted_str); 
}

void test_convert_with_conversion_to_first_uppercase()
{
  converted_str = splt_su_convert(sample_text, SPLT_TO_FIRST_UPPERCASE, &error);

  cut_assert_equal_int(SPLT_OK, error);
  cut_assert_equal_string("JuSt a sAmple of 2 bIts", converted_str); 
}

void test_convert_with_conversion_to_word_first_uppercase()
{
  converted_str = splt_su_convert(sample_text, SPLT_TO_WORD_FIRST_UPPERCASE, &error);

  cut_assert_equal_int(SPLT_OK, error);
  cut_assert_equal_string("JuSt A SAmple Of 2 BIts", converted_str); 
}

void test_get_fname_without_path()
{
  const char *ptr =
    splt_su_get_fname_without_path("/home/ion/hacking/simple file.ogg");

  cut_assert_equal_string("simple file.ogg", ptr);
}

void test_get_formatted_message()
{
  char *msg = splt_su_get_formatted_message(state,
      "This is a formatted %d part %s.", 3, "hellOo!");

  cut_assert_equal_string("This is a formatted 3 part hellOo!.", msg);

  free(msg);
}

