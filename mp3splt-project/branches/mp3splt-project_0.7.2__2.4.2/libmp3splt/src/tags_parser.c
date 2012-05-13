/*********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2012 Alexandru Munteanu - io_fx@yahoo.fr
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

/*! \file 

  Parse tags (Artist, Album, Year,...)
 */
#include <string.h>
#include <ctype.h>

#include "splt.h"

static void splt_tp_process_tags(const char *tags,
    tags_parser_utils *tpu, splt_state *state, int *error);
static void splt_tp_process_tag_variable(const char *tag_variable_start, const char *end_paranthesis,
    tags_parser_utils *tpu, splt_state *state, int *error);
static void splt_tp_process_str_tags_variable(const char *tag_variable_start,
    const char *end_paranthesis, int tags_field,
    tags_parser_utils *tpu, splt_state *state, int *error);
static void splt_tp_process_tracknumber_variable(const char *tag_variable_start,
    const char *end_paranthesis, tags_parser_utils *tpu, splt_state *state, 
    int *error);
static void splt_tp_process_auto_increment_tracknumber_variable(
    const char *tag_variable_start, const char *end_paranthesis, 
    tags_parser_utils *tpu, splt_state *state, int *error);

static void splt_tp_check_ambigous_next_position(const char *tag_variable_start,
    tags_parser_utils *tpu);
static void splt_tp_process_original_tags_variable(tags_parser_utils *tpu,
    splt_state *state, int *error);
static void splt_tp_get_original_tags(splt_state *state, int *error);
static int splt_tp_tpu_has_one_current_tag_set(tags_parser_utils *tpu);
static char *splt_tp_look_for_end_paranthesis(tags_parser_utils *tpu);
static void splt_tp_look_for_all_tags_char(const char *tags,
tags_parser_utils *tpu, splt_state *state);
static void splt_tp_tpu_increment_tags_field_counter(tags_parser_utils *tpu,
    int tags_field);

static tags_parser_utils *splt_tp_tpu_new(splt_state *state, int *error);
static void splt_tp_tpu_reset_for_new_tags(splt_state *state, tags_parser_utils *tpu, int *error);
static void splt_tp_tpu_free(tags_parser_utils **tpu);
static void splt_tp_tpu_set_tracknumber(tags_parser_utils *tpu, 
    const char *tracknumber, int *error);
static const char *splt_tp_tpu_get_tracknumber(tags_parser_utils *tpu);
static void splt_tp_tpu_set_tags_value(tags_parser_utils *tpu,
    int tags_field, const void *tag_value);
static void splt_tp_set_track_from_parsed_tracknumber(tags_parser_utils *tpu,
    splt_state *state, int *error);

void splt_tp_put_tags_from_filename(splt_state *state, int *error)
{
#ifndef NO_PCRE
  splt_tags *tags = splt_fr_parse_from_state(state, error);
  if (*error < 0) { return; }

  char *tags_format = splt_su_get_formatted_message(state,
      "%%[@o,@a=%s,@b=%s,@t=%s,@y=%s,@c=%s,@n=%d,@g=%s]",
      tags->artist,
      tags->album,
      tags->title,
      tags->year,
      tags->comment,
      tags->track,
      tags->genre);

  if (tags_format == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    splt_tu_free_one_tags(&tags);
    return;
  }

  splt_tp_put_tags_from_string(state, tags_format, error);
  free(tags_format);

  splt_tu_free_one_tags(&tags);
#else
  splt_c_put_info_message_to_client(state,
      _(" warning: cannot set tags from filename regular expression - compiled without pcre support\n"));
#endif
}

static char *splt_tp_parse_tag_word(const char *cur_pos,
    const char *end_paranthesis, int *ambiguous, int *error)
{
  char *word = NULL;
  char *word_end = NULL;
  char *word_end2 = NULL;
  const char *equal_sign = NULL;

  if ((word_end = strchr(cur_pos,',')))
  {
    if ((word_end2 = strchr(cur_pos,']')) < word_end)
    {
      word_end = word_end2;
      if ((strchr(word_end+1,']') && !strchr(word_end+1,'['))
          || (strchr(word_end+1,']') < strchr(word_end+1,'[')))
      {
        *ambiguous = SPLT_TRUE;
      }
    }

    if (*word_end == ',')
    {
      if (*(word_end+1) != '@')
      {
        *ambiguous = SPLT_TRUE;
      }
    }
  }
  else
  {
    word_end = strchr(cur_pos,']');
  }

  if (word_end <= end_paranthesis)
  {
    if (*(cur_pos+1) == '=')
    {
      equal_sign = cur_pos+1;
      int string_length = word_end-(equal_sign+1);
      if (string_length > 0)
      {
        word = malloc((string_length+1)*sizeof(char));
        memset(word,'\0',(string_length+1)*sizeof(char));
        if (word)
        {
          memcpy(word,equal_sign+1,string_length);
          word[string_length] = '\0';
        }
        else
        {
          *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
          return NULL;
        }
      }
      else
      {
        *ambiguous = SPLT_TRUE;
      }
    }
    else
    {
      *ambiguous = SPLT_TRUE;
    }
  }

  return word;
}

int splt_tp_put_tags_from_string(splt_state *state, const char *tags, int *error)
{
  if (tags == NULL)
  {
    return SPLT_FALSE;
  }

  tags_parser_utils *tpu = splt_tp_tpu_new(state, error);
  if (*error < 0) { goto end; }

  tpu->position = tags;
  while ((tpu->position = strchr(tpu->position, '[')))
  {
    splt_tp_process_tags(tags, tpu, state, error);
    if (*error < 0) { goto end; }

    splt_tu_append_tags_to_state(state, tpu->current_tags,
        !tpu->original_tags_found, error);
    if (*error < 0) { goto end; }

    if (tpu->set_all_tags)
    {
      tpu->we_had_all_tags = SPLT_TRUE;
      tpu->set_all_tags = SPLT_FALSE;
      splt_tu_copy_tags(tpu->all_tags, splt_tu_get_tags_like_x(state), error);
      if (*error < 0) { goto end; }
    }
    else if (tpu->we_had_all_tags && !tpu->original_tags_found)
    {
      int index = state->split.real_tagsnumber - 1;
      splt_tu_set_new_tags_where_current_tags_are_null(state, 
          tpu->current_tags, tpu->all_tags, index, error);
      if (*error < 0) { goto end; }
    }

    tpu->tags_counter++;
  }

end:
  ;
  int ambigous = tpu->ambigous;
  splt_tp_tpu_free(&tpu);

  return ambigous;
}

static void splt_tp_tpu_set_ambigous_if_more_than_one_variable(tags_parser_utils *tpu)
{
  if ((tpu->title_counter > 1) ||
      (tpu->artist_counter > 1) ||
      (tpu->album_counter > 1) ||
      (tpu->performer_counter > 1) ||
      (tpu->year_counter > 1) ||
      (tpu->comment_counter > 1) ||
      (tpu->tracknumber_counter > 1) ||
      (tpu->genre_counter > 1))
  {
    tpu->ambigous = SPLT_TRUE;
  }
}

static char *splt_tp_find_tag_start(const char *position, const char *end_paranthesis)
{
  const char *ptr = position;

  int comma_or_open_bracket_found = SPLT_FALSE;

  while (ptr <= end_paranthesis)
  {
    if (*ptr == ',' || *ptr == '[')
    {
      comma_or_open_bracket_found = SPLT_TRUE;
      break;
    }
    ptr++;
  }

  if (!comma_or_open_bracket_found)
  {
    return NULL;
  }

  return strchr(ptr, '@');
}

static void splt_tp_process_tags(const char *tags, tags_parser_utils *tpu,
    splt_state *state, int *error)
{
  splt_tp_look_for_all_tags_char(tags, tpu, state);

  splt_tp_tpu_reset_for_new_tags(state, tpu, error);
  if (*error < 0) { return; }

  const char *end_paranthesis = splt_tp_look_for_end_paranthesis(tpu);
  if (!end_paranthesis) { return; }

  tpu->position++;

  char *tag_start = NULL;
  while ((tag_start = splt_tp_find_tag_start(tpu->position-1, end_paranthesis)))
  {
    if (tag_start >= end_paranthesis)
    {
      break;
    }

    tpu->position = tag_start + 1;

    splt_tp_process_tag_variable(tpu->position, end_paranthesis, tpu, state, error);
    if (*error < 0)
    {
      return;
    }

    if (tpu->position == tag_start + 1)
    {
      tpu->position++;
    }
  }

  splt_tp_set_track_from_parsed_tracknumber(tpu, state, error);

  splt_tp_tpu_set_ambigous_if_more_than_one_variable(tpu);
}

static void splt_tp_set_track_from_parsed_tracknumber(tags_parser_utils *tpu,
    splt_state *state, int *error)
{
  const char *tracknumber = splt_tp_tpu_get_tracknumber(tpu);

  if (tracknumber)
  {
    int is_number = SPLT_TRUE;
    int i = 0;
    for (i = 0;i < strlen(tracknumber);i++)
    {
      if (!isdigit(tracknumber[i]))
      {
        is_number = SPLT_FALSE;
        tpu->ambigous = SPLT_TRUE;
      }
    }

    int track = 1;
    if (is_number)
    {
      track = atoi(tracknumber);
    }

    splt_tp_tpu_set_tags_value(tpu, SPLT_TAGS_TRACK, &track);
  }
  else if (tpu->auto_increment_tracknumber)
  {
    splt_tags last_tags = splt_tu_get_last_tags(state);
    int track = last_tags.track + 1;
    splt_tp_tpu_set_tags_value(tpu, SPLT_TAGS_TRACK, &track);
  }
}

static void splt_tp_process_tag_variable(const char *tag_variable_start,
    const char *end_paranthesis, tags_parser_utils *tpu, splt_state *state,
    int *error)
{
  char tag_variable = *tag_variable_start;
  switch (tag_variable)
  {
    case 'o':
      splt_tp_process_original_tags_variable(tpu, state, error);
      if (*error < 0) { return; }
      break;
    case 'a':
      splt_tp_process_str_tags_variable(tag_variable_start, end_paranthesis,
          SPLT_TAGS_ARTIST, tpu, state, error);
      break;
    case 'p':
      splt_tp_process_str_tags_variable(tag_variable_start, end_paranthesis,
          SPLT_TAGS_PERFORMER, tpu, state, error);
      break;
    case 'b':
      splt_tp_process_str_tags_variable(tag_variable_start, end_paranthesis,
          SPLT_TAGS_ALBUM, tpu, state, error);
      break;
    case 't':
      splt_tp_process_str_tags_variable(tag_variable_start, end_paranthesis,
          SPLT_TAGS_TITLE, tpu, state, error);
      break;
    case 'c':
      splt_tp_process_str_tags_variable(tag_variable_start, end_paranthesis,
          SPLT_TAGS_COMMENT, tpu, state, error);
      break;
    case 'g':
      splt_tp_process_str_tags_variable(tag_variable_start, end_paranthesis,
          SPLT_TAGS_GENRE, tpu, state, error);
      break;
    case 'y':
      splt_tp_process_str_tags_variable(tag_variable_start, end_paranthesis,
          SPLT_TAGS_YEAR, tpu, state, error);
      break;
    case 'n':
      splt_tp_process_tracknumber_variable(tag_variable_start,
          end_paranthesis, tpu, state, error);
      break;
    case 'N':
      splt_tp_process_auto_increment_tracknumber_variable(tag_variable_start,
          end_paranthesis, tpu, state, error);
      break;
    default:
      tpu->ambigous = SPLT_TRUE;
      break;
  }

  splt_tp_check_ambigous_next_position(tag_variable_start, tpu);
}

static void splt_tp_process_tracknumber_variable(const char *tag_variable_start,
    const char *end_paranthesis, tags_parser_utils *tpu, splt_state *state, 
    int *error)
{
  char *tag_value = splt_tp_parse_tag_word(tag_variable_start, end_paranthesis,
      &tpu->ambigous, error);
  if (*error < 0) { return; }

  if (tag_value)
  {
    tpu->position += strlen(tag_value) + 1;
    splt_tp_tpu_increment_tags_field_counter(tpu, SPLT_TAGS_TRACK);
    splt_tp_tpu_set_tracknumber(tpu, tag_value, error);
    if (*error < 0) { return; }

    free(tag_value);
    tag_value = NULL;
  }
}

static void splt_tp_process_auto_increment_tracknumber_variable(
    const char *tag_variable_start, const char *end_paranthesis, 
    tags_parser_utils *tpu, splt_state *state, int *error)
{
  char *tag_value = splt_tp_parse_tag_word(tag_variable_start, end_paranthesis,
      &tpu->ambigous, error);
  if (*error < 0) { return; }

  splt_o_set_int_option(state, SPLT_OPT_AUTO_INCREMENT_TRACKNUMBER_TAGS, SPLT_TRUE);

  if (tag_value)
  {
    tpu->position += strlen(tag_value) + 1;
    splt_tp_tpu_set_tracknumber(tpu, tag_value, error);
    if (*error < 0) { return; }

    free(tag_value);
    tag_value = NULL;
  }

  if (tpu->set_all_tags)
  {
    tpu->auto_increment_tracknumber = SPLT_TRUE;
  }
}

static const char *splt_tp_tpu_get_tracknumber(tags_parser_utils *tpu)
{
  return tpu->current_tracknumber;
}

static void splt_tp_tpu_set_tracknumber(tags_parser_utils *tpu, 
    const char *tracknumber, int *error)
{
  int err = splt_su_copy(tracknumber, &tpu->current_tracknumber);
  if (err < 0) { *error = err; return; }
}

static void splt_tp_process_str_tags_variable(const char *tag_variable_start,
    const char *end_paranthesis, int tags_field,
    tags_parser_utils *tpu, splt_state *state, int *error)
{
  char *tag_value = splt_tp_parse_tag_word(tag_variable_start, end_paranthesis,
      &tpu->ambigous, error);
  if (*error < 0) { return; }

  if (tag_value)
  {
    splt_tp_tpu_set_tags_value(tpu, tags_field, tag_value);

    tpu->position += strlen(tag_value) + 1;
    splt_tp_tpu_increment_tags_field_counter(tpu, tags_field);

    free(tag_value);
    tag_value = NULL;
  }
}

static void splt_tp_check_ambigous_next_position(const char *tag_variable_start,
    tags_parser_utils *tpu)
{
  char next_position = *(tpu->position+1);
  if ((next_position != ',') &&
      (next_position != ']'))
  {
    tpu->ambigous = SPLT_TRUE;
  }
}

static void splt_tp_process_original_tags_variable(tags_parser_utils *tpu,
    splt_state *state, int *error)
{
  if (tpu->original_tags_found)
  {
    tpu->ambigous = SPLT_TRUE;
  }

  if (splt_io_input_is_stdin(state))
  {
    return;
  }

  splt_o_lock_messages(state);

  splt_tp_get_original_tags(state, error);
  if (*error < 0) { goto end; }

  int err = splt_tu_append_original_tags(state);
  if (err < 0) { *error = err; goto end; }

  if (tpu->set_all_tags)
  {
    splt_tags last_tags = splt_tu_get_last_tags(state);
    splt_tu_copy_tags(&last_tags, tpu->all_tags, error);
    tpu->all_tags->set_original_tags = SPLT_TRUE;
    if (*error < 0) { goto end; }
  }

  tpu->original_tags_found = SPLT_TRUE;

  if (splt_tp_tpu_has_one_current_tag_set(tpu))
  {
    tpu->ambigous = SPLT_TRUE;
  }

end:
  splt_o_unlock_messages(state);
}

static void splt_tp_get_original_tags(splt_state *state, int *error)
{
  splt_check_file_type(state, error);
  if (*error < 0) { return; }

  splt_o_lock_messages(state);

  splt_p_init(state, error);
  if (*error < 0) { return; }

  splt_tu_get_original_tags(state, error);

  splt_p_end(state, error);

  return;
}

static char *splt_tp_look_for_end_paranthesis(tags_parser_utils *tpu)
{
  char *end_paranthesis = strchr(tpu->position,']');
  if (!end_paranthesis)
  {
    tpu->ambigous = SPLT_TRUE;
  }
  else
  {
    char after_end_paranthesis = *(end_paranthesis+1);
    if ((after_end_paranthesis != '[') &&
        (after_end_paranthesis != '%') &&
        (after_end_paranthesis != '\0'))
    {
      tpu->ambigous = SPLT_TRUE;
    }
  }

  return end_paranthesis;
}

static void splt_tp_look_for_all_tags_char(const char *tags,
    tags_parser_utils *tpu, splt_state *state)
{
  if ((tpu->position != tags) && (*(tpu->position-1) == '%'))
  {
    splt_o_set_int_option(state,
        SPLT_OPT_ALL_REMAINING_TAGS_LIKE_X, tpu->tags_counter);
    tpu->set_all_tags = SPLT_TRUE;

    splt_tu_free_one_tags_content(tpu->all_tags);
  }
}

static tags_parser_utils *splt_tp_tpu_new(splt_state *state, int *error)
{
  tags_parser_utils *tpu = NULL;

  tpu = malloc(sizeof(tags_parser_utils));
  if (tpu == NULL)
  {
    goto mem_error;
  }

  tpu->ambigous = SPLT_FALSE;
  tpu->tags_counter = 0;
  tpu->set_all_tags = SPLT_FALSE;
  tpu->we_had_all_tags = SPLT_FALSE;

  tpu->all_tags = splt_tu_new_tags(state, error);
  if (*error < 0) { goto mem_error; }

  tpu->current_tags = splt_tu_new_tags(state, error);
  if (*error < 0) { goto mem_error; }

  tpu->current_tracknumber = NULL;

  splt_tp_tpu_reset_for_new_tags(state, tpu, error);
  if (*error < 0) { goto mem_error; }

  tpu->auto_increment_tracknumber = SPLT_FALSE;

  tpu->position = NULL;

  return tpu;

mem_error:
  splt_tp_tpu_free(&tpu);
  return NULL;
}

static void splt_tp_tpu_reset_for_new_tags(splt_state *state, tags_parser_utils *tpu, int *error)
{
  tpu->title_counter = 0;
  tpu->artist_counter = 0;
  tpu->album_counter = 0;
  tpu->performer_counter = 0;
  tpu->year_counter = 0;
  tpu->comment_counter = 0;
  tpu->tracknumber_counter = 0;
  tpu->genre_counter = 0;

  splt_tu_free_one_tags_content(tpu->current_tags);

  tpu->original_tags_found = SPLT_FALSE;

  if (tpu->current_tracknumber)
  {
    free(tpu->current_tracknumber);
    tpu->current_tracknumber = NULL;
  }
}

static void splt_tp_tpu_increment_tags_field_counter(tags_parser_utils *tpu,
    int tags_field)
{
  switch(tags_field)
  {
    case SPLT_TAGS_TITLE:
      tpu->title_counter++;
      break;
    case SPLT_TAGS_ARTIST:
      tpu->artist_counter++;
      break;
    case SPLT_TAGS_ALBUM:
      tpu->album_counter++;
      break;
    case SPLT_TAGS_YEAR:
      tpu->year_counter++;
      break;
    case SPLT_TAGS_COMMENT:
      tpu->comment_counter++;
      break;
    case SPLT_TAGS_PERFORMER:
      tpu->performer_counter++;
      break;
    case SPLT_TAGS_TRACK:
      tpu->tracknumber_counter++;
      break;
    case SPLT_TAGS_VERSION:
      ;
      break;
    case SPLT_TAGS_GENRE:
      tpu->genre_counter++;
      ;
      break;
  }
}

static void splt_tp_tpu_set_all_tags_field(tags_parser_utils *tpu,
    int tags_field, const void *tag_value)
{
  if (tpu->set_all_tags)
  {
    splt_tu_set_field_on_tags(tpu->all_tags, tags_field, tag_value);
  }
}

static void splt_tp_tpu_set_current_tags_field(tags_parser_utils *tpu,
    int tags_field, const void *tag_value)
{
  splt_tu_set_field_on_tags(tpu->current_tags, tags_field, tag_value);
}

static void splt_tp_tpu_set_tags_value(tags_parser_utils *tpu,
    int tags_field, const void *tag_value)
{
  splt_tp_tpu_set_all_tags_field(tpu, tags_field, tag_value);
  splt_tp_tpu_set_current_tags_field(tpu, tags_field, tag_value);
}

static void splt_tp_tpu_free(tags_parser_utils **tpu)
{
  if (!tpu || !*tpu)
  {
    return;
  }

  splt_tu_free_one_tags(&(*tpu)->current_tags);
  splt_tu_free_one_tags(&(*tpu)->all_tags);

  if ((*tpu)->current_tracknumber)
  {
    free((*tpu)->current_tracknumber);
    (*tpu)->current_tracknumber = NULL;
  }

  free(*tpu);
  *tpu = NULL;
}

static int splt_tp_tpu_has_one_current_tag_set(tags_parser_utils *tpu)
{
  return splt_tu_has_one_tag_set(tpu->current_tags);
}

