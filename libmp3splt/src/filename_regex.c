/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2010 David Belohrad
 * Copyright (c) 2010-2013 Alexandru Munteanu - m@ioalex.net
 *
 * http://mp3splt.sourceforge.net
 *
 *********************************************************/

/**********************************************************
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 *
 *********************************************************/

#ifndef NO_PCRE

#include <string.h>
#include <pcre.h>

#include "splt.h"

static char *splt_fr_get_pattern(pcre *re, const char *filename, int *ovector,
    int rc, char *key);
static void splt_fr_copy_pattern_to_tags(pcre *re, const char *filename, 
    int *ovector, int rc, char *key, int tags_field, splt_tags *tags,
    int format, int replace_underscores, int *error);
static int splt_fr_get_int_pattern(pcre *re, const char *filename, 
    int *ovector, int rc, char *key);
static void splt_fr_set_char_field_on_tags_and_convert(splt_tags *tags,
    int tags_field, char *pattern, int format, int replace_underscores, int *error);

/*!

\todo Support calculating of the total number of tracks
\todo What does this function do?
*/
splt_tags *splt_fr_parse_from_state(splt_state *state, int *error)
{
  const char *filename_to_split = splt_t_get_filename_to_split(state);
  char *regex = splt_t_get_input_filename_regex(state);
  char *default_comment = splt_t_get_default_comment_tag(state);
  char *default_genre = splt_t_get_default_genre_tag(state);

  char *filename = splt_su_get_fname_without_path_and_extension(filename_to_split, error);
  if (*error < 0)
  {
    return NULL;
  }

  splt_tags *tags = splt_fr_parse(state, filename, regex, default_comment, default_genre, error);

  if (filename)
  {
    free(filename);
    filename = NULL;
  }

  return tags;
}

splt_tags *splt_fr_parse(splt_state *state, const char *filename, const char *regex,
    const char *default_comment, const char *default_genre, int *error)
{
  const char *errorbits;
  int erroroffset;

  splt_d_print_debug(state, "filename for regex = _%s_\n", filename);
  splt_d_print_debug(state, "regex = _%s_\n", regex);

  if (regex == NULL)
  {
    *error = SPLT_INVALID_REGEX;
    splt_e_set_error_data(state, _("no regular expression provided"));
    return NULL;
  }

  pcre *re = pcre_compile(regex, PCRE_CASELESS | PCRE_UTF8, &errorbits, &erroroffset, NULL);
  if (!re)
  {
    *error = SPLT_INVALID_REGEX;
    char *message = splt_su_get_formatted_message(state, "@%u: %s",
        erroroffset, errorbits);
    splt_e_set_error_data(state, message);
    return NULL;
  }

  int ovector[90] = {0,};
  const size_t ovsize = sizeof(ovector)/sizeof(*ovector);

  int rc = pcre_exec(re, NULL, filename, strlen(filename), 0, 0, ovector, ovsize);
  if (rc == PCRE_ERROR_NOMATCH)
  {
    *error = SPLT_REGEX_NO_MATCH;
    pcre_free(re);
    return NULL;
  }

  splt_tags *tags = splt_tu_new_tags(error);
  if (*error < 0)
  {
    pcre_free(re);
    return NULL;
  }
  splt_tu_reset_tags(tags);

  int replace_underscores = 
    splt_o_get_int_option(state, SPLT_OPT_REPLACE_UNDERSCORES_TAG_FORMAT);

  int format = splt_o_get_int_option(state, SPLT_OPT_ARTIST_TAG_FORMAT);
  splt_fr_copy_pattern_to_tags(re, filename, ovector, rc, "artist",
      SPLT_TAGS_ARTIST, tags, format, replace_underscores, error);
  if (*error < 0) { goto error; }

  format = splt_o_get_int_option(state, SPLT_OPT_ALBUM_TAG_FORMAT);
  splt_fr_copy_pattern_to_tags(re, filename, ovector, rc, "album",
      SPLT_TAGS_ALBUM, tags, format, replace_underscores, error);
  if (*error < 0) { goto error; }

  splt_fr_copy_pattern_to_tags(re, filename, ovector, rc, "year",
      SPLT_TAGS_YEAR, tags, SPLT_NO_CONVERSION, SPLT_FALSE, error);
  if (*error < 0) { goto error; }

  format = splt_o_get_int_option(state, SPLT_OPT_COMMENT_TAG_FORMAT);
  char *pattern = splt_fr_get_pattern(re, filename, ovector, rc, "comment");
  if (pattern)
  {
    splt_fr_set_char_field_on_tags_and_convert(tags, SPLT_TAGS_COMMENT,
        pattern, format, replace_underscores, error);
    pcre_free_substring(pattern);
    if (*error < 0) { goto error; }
  }
  else
  {
    splt_tu_set_field_on_tags(tags, SPLT_TAGS_COMMENT, default_comment);
  }

  int track = splt_fr_get_int_pattern(re, filename, ovector, rc, "tracknum");
  if (track != -1)
  {
    splt_tu_set_field_on_tags(tags, SPLT_TAGS_TRACK, &track);
  }

  //TODO: total tracks support
  int total_tracks = splt_fr_get_int_pattern(re, filename, ovector, rc, "tracks");

  format = splt_o_get_int_option(state, SPLT_OPT_TITLE_TAG_FORMAT);
  char *title = splt_fr_get_pattern(re, filename, ovector, rc, "title");
  if (title)
  {
    splt_fr_set_char_field_on_tags_and_convert(tags, SPLT_TAGS_TITLE,
        title, format, replace_underscores, error);
    pcre_free_substring(title);
    if (*error < 0) { goto error; }
  }
  else 
  {
    if (track != -1 && total_tracks != -1)
    {
      title = splt_su_get_formatted_message(state, "Track %d of %d", track, total_tracks);
    }
    else if (track != -1 && total_tracks == -1)
    {
      title = splt_su_get_formatted_message(state, "Track %d", track);
    }

    if (title)
    {
      splt_fr_set_char_field_on_tags_and_convert(tags, SPLT_TAGS_TITLE, title,
          SPLT_NO_CONVERSION, SPLT_FALSE, error);

      free(title);
      title = NULL;

      if (*error < 0) { goto error; }
    }
  }

  char *genre = splt_fr_get_pattern(re, filename, ovector, rc, "genre");
  if (genre)
  {
    splt_tu_set_field_on_tags(tags, SPLT_TAGS_GENRE, genre);
    pcre_free_substring(genre);
    if (*error < 0) { goto error; }
  }
  else
  {
    splt_tu_set_field_on_tags(tags, SPLT_TAGS_GENRE, default_genre);
  }

  pcre_free(re);

  *error = SPLT_REGEX_OK;

  return tags;

error:
  pcre_free(re);
  splt_tu_free_one_tags(&tags); 
  return NULL;
}

static void splt_fr_copy_pattern_to_tags(pcre *re, const char *filename, 
    int *ovector, int rc, char *key, int tags_field, splt_tags *tags,
    int format, int replace_underscores, int *error)
{
  char *pattern = NULL;
  pattern = splt_fr_get_pattern(re, filename, ovector, rc, key);

  splt_fr_set_char_field_on_tags_and_convert(tags, tags_field, pattern,
      format, replace_underscores, error);

  if (pattern)
  {
    pcre_free_substring(pattern);
  }
}

static int splt_fr_get_int_pattern(pcre *re, const char *filename, 
    int *ovector, int rc, char *key)
{
  int value = -1;

  char *pattern = NULL;
  pattern = splt_fr_get_pattern(re, filename, ovector, rc, key);
  if (pattern)
  {
    value = atoi(pattern);
    pcre_free_substring(pattern);
  }

  return value;
}

static char *splt_fr_get_pattern(pcre *re, const char *filename, int *ovector,
    int rc, char *key)
{
  char *pattern = NULL;

  if (pcre_get_named_substring(re, filename, ovector, rc, key,
        (const char**)&pattern) == PCRE_ERROR_NOSUBSTRING)
  {
    return NULL;
  }
  else
  {
    return pattern;
  }
}

static void splt_fr_set_char_field_on_tags_and_convert(splt_tags *tags,
    int tags_field, char *pattern, int format, int replace_underscores, int *error)
{
  if (replace_underscores)
  {
    splt_su_replace_all_char(pattern, '_', ' ');
  }

  char *converted_pattern = splt_su_convert(pattern, format, error);
  if (*error < 0) { return; }

  splt_tu_set_field_on_tags(tags, tags_field, converted_pattern);

  if (converted_pattern)
  {
    free(converted_pattern);
    converted_pattern = NULL;
  }
}

#endif

