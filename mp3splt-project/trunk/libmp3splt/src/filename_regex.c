/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2010 Alexandru Munteanu - io_fx@yahoo.fr
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307,
 * USA.
 *
 *********************************************************/

#ifndef NO_PCRE

#include <string.h>

#include <pcre.h>

#include "splt.h"

static char *splt_fr_get_pattern(pcre *re, const char *filename, int *ovector,
    int rc, char *key);

splt_tags *splt_fr_parse(splt_state *state, const char *filename, const char *regex, const char *comment, int *error)
{
  const char *errorbits;
  int erroroffset;

  pcre *re = pcre_compile(regex, PCRE_CASELESS | PCRE_UTF8, &errorbits, &erroroffset, NULL);

  if (!re)
  {
    *error = SPLT_INVALID_REGEX;
    char *message =
      splt_su_get_formatted_message(state, "@%u: %s", erroroffset, errorbits);
    splt_e_set_error_data(state, message);
    return NULL;
  }

  //TODO
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
    return NULL;
  }

  char *pattern = NULL;

  pattern = splt_fr_get_pattern(re, filename, ovector, rc, "artist");
  splt_tu_set_field_on_tags(tags, SPLT_TAGS_ARTIST, pattern);

  pattern = splt_fr_get_pattern(re, filename, ovector, rc, "album");
  splt_tu_set_field_on_tags(tags, SPLT_TAGS_ALBUM, pattern);

  pattern = splt_fr_get_pattern(re, filename, ovector, rc, "performer");
  splt_tu_set_field_on_tags(tags, SPLT_TAGS_PERFORMER, pattern);

  pattern = splt_fr_get_pattern(re, filename, ovector, rc, "year");
  splt_tu_set_field_on_tags(tags, SPLT_TAGS_YEAR, pattern);

  //TODO: default comment ?
  pattern = splt_fr_get_pattern(re, filename, ovector, rc, "comment");
  splt_tu_set_field_on_tags(tags, SPLT_TAGS_COMMENT, pattern);

  int track = -1;
  pattern = splt_fr_get_pattern(re, filename, ovector, rc, "tracknum");
  if (pattern)
  {
    track= atoi(pattern);
    splt_tu_set_field_on_tags(tags, SPLT_TAGS_TRACK, &track);
    free(pattern);
    pattern = NULL;
  }

  pattern = splt_fr_get_pattern(re, filename, ovector, rc, "tracks");
  int total_tracks = -1;
  if (pattern)
  {
    total_tracks = atoi(pattern);
    free(pattern);
    pattern = NULL;
  }

  char *title = splt_fr_get_pattern(re, filename, ovector, rc, "title");
  if (title == NULL)
  {
    if (track != -1 && total_tracks != -1)
    {
      title = splt_su_get_formatted_message(state, "Track %s of %s",
          track, total_tracks);
    }
    else if (track != -1 && total_tracks == -1)
    {
      title = splt_su_get_formatted_message(state, "Track %s", track);
    }

    //TODO
    // call to generate correct text
    //ttitle = match_text_option (g_strdup(track_title), text_properties_ttitle);
  }
  splt_tu_set_field_on_tags(tags, SPLT_TAGS_TITLE, title);

  pcre_free(re);

  return tags;
}

static char *splt_fr_get_pattern(pcre *re, const char *filename, int *ovector,
    int rc, char *key)
{
  char *pattern = NULL;

  if (pcre_get_named_substring(re, filename, ovector, rc, key,
        (const char**)&pattern) == PCRE_ERROR_NOSUBSTRING)
  {
    pattern = NULL;
  }
  else
  {
    //TODO
    //pattern = match_text_option(pattern, text_properties_artist);
  }

  return pattern;
}

#endif

