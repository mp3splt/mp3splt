#include <cutter.h>

#include "libmp3splt/mp3splt.h"
#include "options.h"
#include "filename_regex.h"
#include "tags_utils.h"

#include "tests.h"

static int error = SPLT_OK;
static splt_state *state = NULL;
static splt_tags *tags = NULL;

static char *NO_DEFAULT_COMMENT = NULL;
static char *NO_DEFAULT_GENRE = NULL;

static void set_tags_format_options(splt_state *state, int format);

void cut_setup()
{
  state = mp3splt_new_state(NULL);
  error = SPLT_OK;
}

void cut_teardown()
{
  mp3splt_free_state(state);
  splt_tu_free_one_tags(&tags);
  tags = NULL;
}

void test_all_fields_no_conversion()
{
  tags = splt_fr_parse(state,
      "artist producing Slow Rock _ album named geek by performer in 2007 with comment and track 2 of 5",

      "(?<artist>.*?) producing (?<genre>.*?) _ (?<album>.*?) named (?<title>.*?) by "
      ".*? in (?<year>.*?) with (?<comment>.*?) and track (?<tracknum>.*?) of (?<tracks>.*)",

      NO_DEFAULT_COMMENT, NO_DEFAULT_GENRE, &error);

  cut_assert_equal_int(SPLT_REGEX_OK, error);

  char *artist = mp3splt_tags_get(tags, SPLT_TAGS_ARTIST);
  cut_assert_equal_string("artist", artist);
  free(artist);
  char *title = mp3splt_tags_get(tags, SPLT_TAGS_TITLE);
  cut_assert_equal_string("geek", title);
  free(title);
  char *album = mp3splt_tags_get(tags, SPLT_TAGS_ALBUM);
  cut_assert_equal_string("album", album);
  free(album);
  char *comment = mp3splt_tags_get(tags, SPLT_TAGS_COMMENT);
  cut_assert_equal_string("comment", comment);
  free(comment);
  char *year = mp3splt_tags_get(tags, SPLT_TAGS_YEAR);
  cut_assert_equal_string("2007", year);
  free(year);
  char *track = mp3splt_tags_get(tags, SPLT_TAGS_TRACK);
  cut_assert_equal_string("2", track);
  free(track);
  char *genre = mp3splt_tags_get(tags, SPLT_TAGS_GENRE);
  cut_assert_equal_string("Slow Rock", genre);
  free(genre);
}

void test_all_fields_to_uppercase()
{
  set_tags_format_options(state, SPLT_TO_UPPERCASE);

  tags = splt_fr_parse(state,

      "artist producing Slow Rock _ album named geek by performer in 2007 with comment and track 2 of 5",

      "(?<artist>.*?) producing (?<genre>.*?) _ (?<album>.*?) named (?<title>.*?) by "
      ".*? in (?<year>.*?) with (?<comment>.*?) and track (?<tracknum>.*?) of (?<tracks>.*)",

      NO_DEFAULT_COMMENT, NO_DEFAULT_GENRE, &error);

  cut_assert_equal_int(SPLT_REGEX_OK, error);

  char *artist = mp3splt_tags_get(tags, SPLT_TAGS_ARTIST);
  cut_assert_equal_string("ARTIST", artist);
  free(artist);
  char *title = mp3splt_tags_get(tags, SPLT_TAGS_TITLE);
  cut_assert_equal_string("GEEK", title);
  free(title);
  char *album = mp3splt_tags_get(tags, SPLT_TAGS_ALBUM);
  cut_assert_equal_string("ALBUM", album);
  free(album);
  char *comment = mp3splt_tags_get(tags, SPLT_TAGS_COMMENT);
  cut_assert_equal_string("COMMENT", comment);
  free(comment);
  char *year = mp3splt_tags_get(tags, SPLT_TAGS_YEAR);
  cut_assert_equal_string("2007", year);
  free(year);
  char *track = mp3splt_tags_get(tags, SPLT_TAGS_TRACK);
  cut_assert_equal_string("2", track);
  free(track);
  char *genre = mp3splt_tags_get(tags, SPLT_TAGS_GENRE);
  cut_assert_equal_string("Slow Rock", genre);
  free(genre);
}

void test_all_fields_to_lowercase()
{
  set_tags_format_options(state, SPLT_TO_LOWERCASE);

  tags = splt_fr_parse(state,
      "arTist producing Slow Rock _ alBum named GEEK by PERFORMER in 2007 with coMMent and track 2 of 5",

      "(?<artist>.*?) producing (?<genre>.*?) _ (?<album>.*?) named (?<title>.*?) by "
      ".*? in (?<year>.*?) with (?<comment>.*?) and track (?<tracknum>.*?) of (?<tracks>.*)",

      NO_DEFAULT_COMMENT, NO_DEFAULT_GENRE, &error);

  cut_assert_equal_int(SPLT_REGEX_OK, error);

  char *artist = mp3splt_tags_get(tags, SPLT_TAGS_ARTIST);
  cut_assert_equal_string("artist", artist);
  free(artist);
  char *title = mp3splt_tags_get(tags, SPLT_TAGS_TITLE);
  cut_assert_equal_string("geek", title);
  free(title);
  char *album = mp3splt_tags_get(tags, SPLT_TAGS_ALBUM);
  cut_assert_equal_string("album", album);
  free(album);
  char *comment = mp3splt_tags_get(tags, SPLT_TAGS_COMMENT);
  cut_assert_equal_string("comment", comment);
  free(comment);
  char *year = mp3splt_tags_get(tags, SPLT_TAGS_YEAR);
  cut_assert_equal_string("2007", year);
  free(year);
  char *track = mp3splt_tags_get(tags, SPLT_TAGS_TRACK);
  cut_assert_equal_string("2", track);
  free(track);
  char *genre = mp3splt_tags_get(tags, SPLT_TAGS_GENRE);
  cut_assert_equal_string("Slow Rock", genre);
  free(genre);
}

void test_all_fields_to_first_uppercase()
{
  set_tags_format_options(state, SPLT_TO_FIRST_UPPERCASE);

  tags = splt_fr_parse(state,
      "arTist good producing Tango _ alBum named gEEK by pERFORMER in 2007 with coMMent and track 2 of 5",

      "(?<artist>.*?) producing (?<genre>.*?) _ (?<album>.*?) named (?<title>.*?) by "
      ".*? in (?<year>.*?) with (?<comment>.*?) and track (?<tracknum>.*?) of (?<tracks>.*)",

      NO_DEFAULT_COMMENT, NO_DEFAULT_GENRE, &error);

  cut_assert_equal_int(SPLT_REGEX_OK, error);

  char *artist = mp3splt_tags_get(tags, SPLT_TAGS_ARTIST);
  cut_assert_equal_string("ArTist good", artist);
  free(artist);
  char *title = mp3splt_tags_get(tags, SPLT_TAGS_TITLE);
  cut_assert_equal_string("GEEK", title);
  free(title);
  char *album = mp3splt_tags_get(tags, SPLT_TAGS_ALBUM);
  cut_assert_equal_string("AlBum", album);
  free(album);
  char *comment = mp3splt_tags_get(tags, SPLT_TAGS_COMMENT);
  cut_assert_equal_string("CoMMent", comment);
  free(comment);
  char *year = mp3splt_tags_get(tags, SPLT_TAGS_YEAR);
  cut_assert_equal_string("2007", year);
  free(year);
  char *track = mp3splt_tags_get(tags, SPLT_TAGS_TRACK);
  cut_assert_equal_string("2", track);
  free(track);
  char *genre = mp3splt_tags_get(tags, SPLT_TAGS_GENRE);
  cut_assert_equal_string("Tango", genre);
  free(genre);
  //tags->total_tracks
}

void test_all_fields_to_word_first_uppercase()
{
  set_tags_format_options(state, SPLT_TO_WORD_FIRST_UPPERCASE);

  tags = splt_fr_parse(state,
      "arTist good producing Tango _ alBum second named gEEK y by pERFORMER me in 2007 with coMMent this and track 2 of 5",

      "(?<artist>.*?) producing (?<genre>.*?) _ (?<album>.*?) named (?<title>.*?) by "
      ".*? in (?<year>.*?) with (?<comment>.*?) and track (?<tracknum>.*?) of (?<tracks>.*)",

      NO_DEFAULT_COMMENT, NO_DEFAULT_GENRE, &error);

  cut_assert_equal_int(SPLT_REGEX_OK, error);

  char *artist = mp3splt_tags_get(tags, SPLT_TAGS_ARTIST);
  cut_assert_equal_string("ArTist Good", artist);
  free(artist);
  char *title = mp3splt_tags_get(tags, SPLT_TAGS_TITLE);
  cut_assert_equal_string("GEEK Y", title);
  free(title);
  char *album = mp3splt_tags_get(tags, SPLT_TAGS_ALBUM);
  cut_assert_equal_string("AlBum Second", album);
  free(album);
  char *comment = mp3splt_tags_get(tags, SPLT_TAGS_COMMENT);
  cut_assert_equal_string("CoMMent This", comment);
  free(comment);
  char *year = mp3splt_tags_get(tags, SPLT_TAGS_YEAR);
  cut_assert_equal_string("2007", year);
  free(year);
  char *track = mp3splt_tags_get(tags, SPLT_TAGS_TRACK);
  cut_assert_equal_string("2", track);
  free(track);
  char *genre = mp3splt_tags_get(tags, SPLT_TAGS_GENRE);
  cut_assert_equal_string("Tango", genre);
  free(genre);
  //tags->total_tracks
}

void test_invalid_regex()
{
  splt_fr_parse(state, "another one", "(?ohh my test", 
      NO_DEFAULT_COMMENT, NO_DEFAULT_GENRE, &error);

  cut_assert_equal_int(SPLT_INVALID_REGEX, error);
  mp3splt_assert_equal_error_message(state, error, 
      " regular expression error: @2: unrecognized character after (? or (?-");
}

void test_no_regex_match()
{
  splt_fr_parse(state, "another one", "(?<artist>.*?) one test", 
      NO_DEFAULT_COMMENT, NO_DEFAULT_GENRE, &error);

  cut_assert_equal_int(SPLT_REGEX_NO_MATCH, error);
  mp3splt_assert_equal_error_message(state, error, " regular expression error: no match");
}

void test_no_title_with_total_tracks()
{
  tags = splt_fr_parse(state, "track 3 of 6",
      "track (?<tracknum>.*?) of (?<tracks>.*)",
      NO_DEFAULT_COMMENT, NO_DEFAULT_GENRE, &error);

  cut_assert_equal_int(SPLT_REGEX_OK, error);

  char *title = mp3splt_tags_get(tags, SPLT_TAGS_TITLE);
  cut_assert_equal_string("Track 3 of 6", title);
  free(title);
  char *track = mp3splt_tags_get(tags, SPLT_TAGS_TRACK);
  cut_assert_equal_string("3", track);
  free(track);
}

void test_no_title_no_total_tracks()
{
  tags = splt_fr_parse(state, "track 3", "track (?<tracknum>.*)",
      NO_DEFAULT_COMMENT, NO_DEFAULT_GENRE, &error);

  cut_assert_equal_int(SPLT_REGEX_OK, error);

  char *title = mp3splt_tags_get(tags, SPLT_TAGS_TITLE);
  cut_assert_equal_string("Track 3", title);
  free(title);
  char *track = mp3splt_tags_get(tags, SPLT_TAGS_TRACK);
  cut_assert_equal_string("3", track);
  free(track);
}

void test_default_comment()
{
  tags = splt_fr_parse(state, "track 3", "track (?<tracknum>.*)",
      "default_comment", NO_DEFAULT_GENRE, &error);

  cut_assert_equal_int(SPLT_REGEX_OK, error);

  char *comment = mp3splt_tags_get(tags, SPLT_TAGS_COMMENT);
  cut_assert_equal_string("default_comment", comment);
  free(comment);
}

void test_default_genre()
{
  tags = splt_fr_parse(state, "track 3", "track (?<tracknum>.*)",
      NO_DEFAULT_COMMENT, "Freestyle", &error);

  cut_assert_equal_int(SPLT_REGEX_OK, error);

  char *genre = mp3splt_tags_get(tags, SPLT_TAGS_GENRE);
  cut_assert_equal_string("Freestyle", genre);
  free(genre);
}


void test_mixed_formats()
{
  splt_o_set_int_option(state, SPLT_OPT_ARTIST_TAG_FORMAT, SPLT_TO_UPPERCASE);
  splt_o_set_int_option(state, SPLT_OPT_ALBUM_TAG_FORMAT, SPLT_TO_FIRST_UPPERCASE);
  splt_o_set_int_option(state, SPLT_OPT_TITLE_TAG_FORMAT, SPLT_TO_LOWERCASE);
  splt_o_set_int_option(state, SPLT_OPT_COMMENT_TAG_FORMAT, SPLT_TO_WORD_FIRST_UPPERCASE);

  tags = splt_fr_parse(state,
      "arTist_good producing Tribal _ alBum named gEEK by pERFORMER in 2007 with coMMent kk and track 2 of 5",

      "(?<artist>.*?) producing (?<genre>.*?) _ (?<album>.*?) named (?<title>.*?) by "
      ".*? in (?<year>.*?) with (?<comment>.*?) and track (?<tracknum>.*?) of (?<tracks>.*)",

      NO_DEFAULT_COMMENT, NO_DEFAULT_GENRE, &error);

  cut_assert_equal_int(SPLT_REGEX_OK, error);

  char *artist = mp3splt_tags_get(tags, SPLT_TAGS_ARTIST);
  cut_assert_equal_string("ARTIST_GOOD", artist);
  free(artist);
  char *title = mp3splt_tags_get(tags, SPLT_TAGS_TITLE);
  cut_assert_equal_string("geek", title);
  free(title);
  char *album = mp3splt_tags_get(tags, SPLT_TAGS_ALBUM);
  cut_assert_equal_string("AlBum", album);
  free(album);
  char *comment = mp3splt_tags_get(tags, SPLT_TAGS_COMMENT);
  cut_assert_equal_string("CoMMent Kk", comment);
  free(comment);
  char *year = mp3splt_tags_get(tags, SPLT_TAGS_YEAR);
  cut_assert_equal_string("2007", year);
  free(year);
  char *track = mp3splt_tags_get(tags, SPLT_TAGS_TRACK);
  cut_assert_equal_string("2", track);
  free(track);
  char *genre = mp3splt_tags_get(tags, SPLT_TAGS_GENRE);
  cut_assert_equal_string("Tribal", genre);
  free(genre);
}

void test_with_replace_underscores()
{
  splt_o_set_int_option(state, SPLT_OPT_ARTIST_TAG_FORMAT, SPLT_TO_UPPERCASE);
  splt_o_set_int_option(state, SPLT_OPT_ALBUM_TAG_FORMAT, SPLT_TO_FIRST_UPPERCASE);
  splt_o_set_int_option(state, SPLT_OPT_TITLE_TAG_FORMAT, SPLT_TO_LOWERCASE);
  splt_o_set_int_option(state, SPLT_OPT_COMMENT_TAG_FORMAT, SPLT_TO_WORD_FIRST_UPPERCASE);

  splt_o_set_int_option(state, SPLT_OPT_REPLACE_UNDERSCORES_TAG_FORMAT, SPLT_TRUE);

  tags = splt_fr_parse(state,
      "arTist_good producing Dance Hall _ alBum named gE_EK by pERFORMER in 2007 with coMMent kk and track 2 of 5",

      "(?<artist>.*?) producing (?<genre>.*?) _ (?<album>.*?) named (?<title>.*?) by "
      ".*? in (?<year>.*?) with (?<comment>.*?) and track (?<tracknum>.*?) of (?<tracks>.*)",

      NO_DEFAULT_COMMENT, NO_DEFAULT_GENRE, &error);

  cut_assert_equal_int(SPLT_REGEX_OK, error);

  char *artist = mp3splt_tags_get(tags, SPLT_TAGS_ARTIST);
  cut_assert_equal_string("ARTIST GOOD", artist);
  free(artist);
  char *title = mp3splt_tags_get(tags, SPLT_TAGS_TITLE);
  cut_assert_equal_string("ge ek", title);
  free(title);
  char *album = mp3splt_tags_get(tags, SPLT_TAGS_ALBUM);
  cut_assert_equal_string("AlBum", album);
  free(album);
  char *comment = mp3splt_tags_get(tags, SPLT_TAGS_COMMENT);
  cut_assert_equal_string("CoMMent Kk", comment);
  free(comment);
  char *year = mp3splt_tags_get(tags, SPLT_TAGS_YEAR);
  cut_assert_equal_string("2007", year);
  free(year);
  char *track = mp3splt_tags_get(tags, SPLT_TAGS_TRACK);
  cut_assert_equal_string("2", track);
  free(track);
  char *genre = mp3splt_tags_get(tags, SPLT_TAGS_GENRE);
  cut_assert_equal_string("Dance Hall", genre);
  free(genre);
}

static void set_tags_format_options(splt_state *state, int format)
{
  splt_o_set_int_option(state, SPLT_OPT_ARTIST_TAG_FORMAT, format);
  splt_o_set_int_option(state, SPLT_OPT_ALBUM_TAG_FORMAT, format);
  splt_o_set_int_option(state, SPLT_OPT_TITLE_TAG_FORMAT, format);
  splt_o_set_int_option(state, SPLT_OPT_COMMENT_TAG_FORMAT, format);
}

