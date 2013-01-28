#include <cutter.h>

#include "libmp3splt/mp3splt.h"

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

void test_initial_tags()
{
  splt_tags_group *tags_group = mp3splt_get_tags_group(state, &error);
  cut_assert_equal_int(SPLT_OK, error);

  cut_assert_null(tags_group);

  mp3splt_tags_group_init_iterator(tags_group);
  cut_assert_null(mp3splt_tags_group_next(tags_group));
  cut_assert_null(mp3splt_tags_group_next(tags_group));
}

void test_initial_erase_all_tags()
{
  error = mp3splt_erase_all_tags(state);
  cut_assert_equal_int(SPLT_OK, error);

  splt_tags_group *tags_group = mp3splt_get_tags_group(state, &error);
  cut_assert_equal_int(SPLT_OK, error);

  cut_assert_null(tags_group);
}

void test_append_and_return_tags()
{
  //first tags
  splt_tags *tags1 = mp3splt_tags_new(&error);
  cut_assert_equal_int(SPLT_OK, error);
  error = mp3splt_tags_set(tags1,
      SPLT_TAGS_ARTIST, "artist1",
      SPLT_TAGS_ALBUM, "album1",
      SPLT_TAGS_GENRE, "other1",
      SPLT_TAGS_TRACK, "10",
      0);
  cut_assert_equal_int(SPLT_OK, error);
 
  //append first tags 
  error = mp3splt_append_tags(state, tags1);
  cut_assert_equal_int(SPLT_OK, error);

  //second tags
  splt_tags *tags2 = mp3splt_tags_new(&error);
  cut_assert_equal_int(SPLT_OK, error);
  error = mp3splt_tags_set(tags2,
      SPLT_TAGS_ARTIST, "artist2",
      SPLT_TAGS_ALBUM, "album2",
      SPLT_TAGS_GENRE, "other2",
      SPLT_TAGS_TRACK, "20",
      0);
  cut_assert_equal_int(SPLT_OK, error);

  //append second tags 
  error = mp3splt_append_tags(state, tags2);
  cut_assert_equal_int(SPLT_OK, error);

  splt_tags_group *tags_group = mp3splt_get_tags_group(state, &error);
  cut_assert_equal_int(SPLT_OK, error);
  mp3splt_tags_group_init_iterator(tags_group);

  //check first tags
  splt_tags *check_tags1 = mp3splt_tags_group_next(tags_group);
  char *artist1 = mp3splt_tags_get(check_tags1, SPLT_TAGS_ARTIST);
  cut_assert_equal_string("artist1", artist1);
  free(artist1);
  char *album1 = mp3splt_tags_get(check_tags1, SPLT_TAGS_ALBUM);
  cut_assert_equal_string("album1", album1);
  free(album1);
  char *genre1 = mp3splt_tags_get(check_tags1, SPLT_TAGS_GENRE);
  cut_assert_equal_string("other1", genre1);
  free(genre1);
  char *track1 = mp3splt_tags_get(check_tags1, SPLT_TAGS_TRACK);
  cut_assert_equal_string("10", track1);
  free(track1);
  char *title1 = mp3splt_tags_get(check_tags1, SPLT_TAGS_TITLE);
  cut_assert_null(title1);

  //check second tags
  splt_tags *check_tags2 = mp3splt_tags_group_next(tags_group);
  char *artist2 = mp3splt_tags_get(check_tags2, SPLT_TAGS_ARTIST);
  cut_assert_equal_string("artist2", artist2);
  free(artist2);
  char *album2 = mp3splt_tags_get(check_tags2, SPLT_TAGS_ALBUM);
  cut_assert_equal_string("album2", album2);
  free(album2);
  char *genre2 = mp3splt_tags_get(check_tags2, SPLT_TAGS_GENRE);
  cut_assert_equal_string("other2", genre2);
  free(genre2);
  char *track2 = mp3splt_tags_get(check_tags2, SPLT_TAGS_TRACK);
  cut_assert_equal_string("20", track2);
  free(track2);
  char *title2 = mp3splt_tags_get(check_tags2, SPLT_TAGS_TITLE);
  cut_assert_null(title2);

  cut_assert_null(mp3splt_tags_group_next(tags_group));
}

void test_append_and_erase_all_tags()
{
  splt_tags *tags1 = mp3splt_tags_new(&error);
  cut_assert_equal_int(SPLT_OK, error);
  error = mp3splt_tags_set(tags1,
      SPLT_TAGS_ARTIST, "artist1",
      SPLT_TAGS_ALBUM, "album1",
      SPLT_TAGS_GENRE, "other1",
      SPLT_TAGS_TRACK, "10",
      0);
  cut_assert_equal_int(SPLT_OK, error);

  //append tags 
  error = mp3splt_append_tags(state, tags1);
  cut_assert_equal_int(SPLT_OK, error);

  error = mp3splt_erase_all_tags(state);
  cut_assert_equal_int(SPLT_OK, error);

  splt_tags_group *tags_group = mp3splt_get_tags_group(state, &error);
  cut_assert_null(tags_group);
}

