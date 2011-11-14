/**********************************************************
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2011 Alexandru Munteanu - io_fx@yahoo.fr
 *
 *********************************************************/

/**********************************************************
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
 *********************************************************/

#include <string.h>

#include "splt.h"
#include "tags_utils.h"

void splt_tu_free_original_tags(splt_state *state)
{
  splt_tags *tags = &state->original_tags.tags;

  if (tags->year)
  {
    free(tags->year);
    tags->year = NULL;
  }
  if (tags->artist)
  {
    free(tags->artist);
    tags->artist = NULL;
  }
  if (tags->album)
  {
    free(tags->album);
    tags->album = NULL;
  }
  if (tags->title)
  {
    free(tags->title);
    tags->title = NULL;
  }
  if (tags->comment)
  {
    free(tags->comment);
    tags->comment = NULL;
  }
  if (tags->genre)
  {
    free(tags->genre);
    tags->genre = NULL;
  }
  tags->track = -1;

  int err = SPLT_OK;
  splt_p_clear_original_tags(state, &err);
}

//! Get all tags for split point number x
splt_tags *splt_tu_get_tags(splt_state *state, int *tags_number)
{
  *tags_number = state->split.real_tagsnumber;
  return state->split.tags;
}

void splt_tu_auto_increment_tracknumber(splt_state *state)
{
  int current_split = splt_t_get_current_split_file_number(state) - 1;
  int old_current_split = current_split;

  int remaining_tags_like_x = splt_o_get_int_option(state, SPLT_OPT_ALL_REMAINING_TAGS_LIKE_X);
  if (remaining_tags_like_x != -1)
  {
    if (current_split >= state->split.real_tagsnumber)
    {
      current_split = remaining_tags_like_x;
    }

    if (splt_o_get_int_option(state, SPLT_OPT_AUTO_INCREMENT_TRACKNUMBER_TAGS) > 0)
    {
      if (current_split == remaining_tags_like_x)
      {
        if ((old_current_split > 0) && 
            (old_current_split-1 < state->split.real_tagsnumber) && 
            (old_current_split != remaining_tags_like_x))
        {
          int *prev_track = (int *)splt_tu_get_tags_field(state, old_current_split - 1, SPLT_TAGS_TRACK);
          int previous_track = 0;
          if (prev_track != NULL)
          {
            previous_track = *prev_track;
          }
          splt_tu_set_tags_field(state, remaining_tags_like_x, SPLT_TAGS_TRACK, &previous_track);
        }

        if (old_current_split != current_split)
        {
          int tracknumber = 1;
          if (splt_tu_tags_exists(state, current_split))
          {
            int *track = (int *)splt_tu_get_tags_field(state, current_split, SPLT_TAGS_TRACK);
            if (track != NULL)
            {
              tracknumber = *track;
            }
          }
          int new_tracknumber = tracknumber + 1;
          splt_tu_set_tags_field(state, current_split, SPLT_TAGS_TRACK, &new_tracknumber);
          splt_tu_set_like_x_tags_field(state, SPLT_TAGS_TRACK, &new_tracknumber);
        }
      }
    }
  }
}

void splt_tu_get_original_tags(splt_state *state, int *err)
{
  if (! splt_io_input_is_stdin(state))
  {
    splt_tu_free_original_tags(state);
    splt_p_set_original_tags(state, err);
  }
}

int splt_tu_append_tags(splt_state *state, 
    const char *title, const char *artist,
    const char *album, const char *performer,
    const char *year, const char *comment,
    int track, const char *genre, int set_original_tags)
{
  int error = SPLT_OK;
  int old_tagsnumber = state->split.real_tagsnumber;

  error = splt_tu_set_tags_field(state, old_tagsnumber, SPLT_TAGS_TITLE, title);
  if (error != SPLT_OK)
    return error;

  error = splt_tu_set_tags_field(state, old_tagsnumber, SPLT_TAGS_ARTIST, artist);
  if (error != SPLT_OK)
    return error;

  error = splt_tu_set_tags_field(state, old_tagsnumber, SPLT_TAGS_ALBUM, album);
  if (error != SPLT_OK)
    return error;

  error = splt_tu_set_tags_field(state, old_tagsnumber, SPLT_TAGS_PERFORMER, performer);
  if (error != SPLT_OK)
    return error;

  error = splt_tu_set_tags_field(state, old_tagsnumber, SPLT_TAGS_YEAR, year);
  if (error != SPLT_OK)
    return error;

  error = splt_tu_set_tags_field(state, old_tagsnumber, SPLT_TAGS_COMMENT, comment);
  if (error != SPLT_OK)
    return error;

  error = splt_tu_set_tags_field(state, old_tagsnumber, SPLT_TAGS_TRACK, &track);
  if (error != SPLT_OK)
    return error;

  error = splt_tu_set_tags_field(state, old_tagsnumber, SPLT_TAGS_ORIGINAL, &set_original_tags);
  if (error != SPLT_OK)
    return error;

  error = splt_tu_set_tags_field(state, old_tagsnumber, SPLT_TAGS_GENRE, genre);
  return error;
}

int splt_tu_append_original_tags(splt_state *state)
{
  int err = SPLT_OK;

  char *new_title = NULL;
  char *new_artist = NULL;
  char *new_album = NULL;
  char *new_year = NULL;
  char *new_comment = NULL;
  char *new_genre = NULL;

  splt_tags *tags = &state->original_tags.tags;

  new_title = splt_su_replace_all(tags->title, "@", "@@", &err);
  if (err != SPLT_OK) { goto end; }

  new_artist = splt_su_replace_all(tags->artist, "@", "@@", &err);
  if (err != SPLT_OK) { goto end; }

  new_album = splt_su_replace_all(tags->album, "@", "@@", &err);
  if (err != SPLT_OK) { goto end; }

  new_year = splt_su_replace_all(tags->year, "@", "@@", &err);
  if (err != SPLT_OK) { goto end; }

  new_comment = splt_su_replace_all(tags->comment, "@", "@@", &err);
  if (err != SPLT_OK) { goto end; }

  new_genre = splt_su_replace_all(tags->genre, "@", "@@", &err);
  if (err != SPLT_OK) { goto end; }

  err = splt_tu_append_tags(state, new_title, new_artist, new_album, NULL,
      new_year, new_comment, tags->track, new_genre, SPLT_TRUE);

end:
  if (new_title) { free(new_title); }
  if (new_artist) { free(new_artist); }
  if (new_album) { free(new_album); }
  if (new_year) { free(new_year); }
  if (new_comment) { free(new_comment); }
  if (new_genre) { free(new_genre); }

  return err;
}

int splt_tu_append_only_non_null_previous_tags(splt_state *state, 
    const char *title, const char *artist,
    const char *album, const char *performer,
    const char *year, const char *comment,
    int track, const char *genre, int set_original_tags)
{
  int error = SPLT_OK;
  int old_tagsnumber = state->split.real_tagsnumber-1;

  if (old_tagsnumber >= 0)
  {
    if (title != NULL)
    {
      error = splt_tu_set_tags_field(state, old_tagsnumber, SPLT_TAGS_TITLE, title);
    }
    if (error != SPLT_OK)
      return error;

    if (artist != NULL)
    {
      error = splt_tu_set_tags_field(state, old_tagsnumber, SPLT_TAGS_ARTIST, artist);
    }
    if (error != SPLT_OK)
      return error;

    if (album != NULL)
    {
      error = splt_tu_set_tags_field(state, old_tagsnumber, SPLT_TAGS_ALBUM, album);
    }
    if (error != SPLT_OK)
      return error;

    if (performer != NULL)
    {
      error = splt_tu_set_tags_field(state, old_tagsnumber, SPLT_TAGS_PERFORMER, performer);
    }
    if (error != SPLT_OK)
      return error;

    if (year != NULL)
    {
      error = splt_tu_set_tags_field(state, old_tagsnumber, SPLT_TAGS_YEAR, year);
    }
    if (error != SPLT_OK)
      return error;

    if (comment != NULL)
    {
      error = splt_tu_set_tags_field(state, old_tagsnumber, SPLT_TAGS_COMMENT, comment);
    }
    if (error != SPLT_OK)
      return error;

    if (track != -1)
    {
      error = splt_tu_set_tags_field(state, old_tagsnumber, SPLT_TAGS_TRACK, &track);
    }
    if (error != SPLT_OK)
      return error;

    if (set_original_tags != -1)
    {
      error = splt_tu_set_tags_field(state, old_tagsnumber, SPLT_TAGS_ORIGINAL, &set_original_tags);
    }
    if (error != SPLT_OK)
      return error;

    if (genre != NULL)
    {
      error = splt_tu_set_tags_field(state, old_tagsnumber, SPLT_TAGS_GENRE, genre);
    }
  }

  return error;
}

void splt_tu_reset_tags(splt_tags *tags)
{
  tags->title = NULL;
  tags->artist = NULL;
  tags->album = NULL;
  tags->performer = NULL;
  tags->year = NULL;
  tags->comment = NULL;
  tags->track = -1;
  tags->genre = NULL;
  tags->tags_version = 0;
  tags->set_original_tags = SPLT_FALSE;
}

splt_tags *splt_tu_new_tags(splt_state *state, int *error)
{
  splt_tags *tags = malloc(sizeof(splt_tags));

  if (tags == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return NULL;
  }

  memset(tags, sizeof(splt_tags), '\0');

  splt_tu_reset_tags(tags);

  return tags;
}

static void splt_tu_set_empty_tags(splt_state *state, int index)
{
  splt_tu_reset_tags(&state->split.tags[index]);
}

int splt_tu_new_tags_if_necessary(splt_state *state, int index)
{
  int error = SPLT_OK;

  if (!state->split.tags)
  {
    if ((index > state->split.real_tagsnumber) || (index < 0))
    {
      splt_e_error(SPLT_IERROR_INT,__func__, index, NULL);
    }
    else
    {
      state->split.tags = splt_tu_new_tags(state, &error);
      if (error < 0)
      {
        return error;
      }
      else
      {
        splt_tu_set_empty_tags(state, index);
        state->split.real_tagsnumber++;
      }
    }
  }
  else
  {
    if ((index > state->split.real_tagsnumber) || (index < 0))
    {
      splt_e_error(SPLT_IERROR_INT,__func__, index, NULL);
    }
    else
    {
      if (index == state->split.real_tagsnumber)
      {
        if ((state->split.tags = realloc(state->split.tags,
                sizeof(splt_tags) * (index+1))) == NULL)
        {
          error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
          return error;
        }
        else
        {
          splt_tu_set_empty_tags(state,index);
          state->split.real_tagsnumber++;
        }
      }
    }
  }

  return error;
}

int splt_tu_tags_exists(splt_state *state, int index)
{
  if ((index >= 0) && (index < state->split.real_tagsnumber))
  {
    return SPLT_TRUE;
  }
  else
  {
    return SPLT_FALSE;
  }
}

int splt_tu_set_tags_field(splt_state *state, int index,
    int tags_field, const void *data)
{
  int error = SPLT_OK;

  error = splt_tu_new_tags_if_necessary(state,index);
  if (error != SPLT_OK) { return error; }

  if ((index >= state->split.real_tagsnumber) || (index < 0))
  {
    error = SPLT_ERROR_INEXISTENT_SPLITPOINT;
    splt_e_error(SPLT_IERROR_INT,__func__, index, NULL);
    return error;
  }
  else
  {
    splt_tu_set_field_on_tags(&state->split.tags[index], tags_field, data);
  }

  if (error != SPLT_OK)
  {
    splt_e_error(SPLT_IERROR_INT,__func__, index, NULL);
  }

  return error;
}

int splt_tu_set_like_x_tags_field(splt_state *state, int tags_field, const void *data)
{
  return splt_tu_set_field_on_tags(&state->split.tags_like_x, tags_field, data);
}

int splt_tu_set_original_tags_field(splt_state *state, int tags_field, const void *data)
{
  return splt_tu_set_field_on_tags(&state->original_tags.tags, tags_field, data);
}

void splt_tu_set_original_tags_data(splt_state *state, void *data)
{
  state->original_tags.all_original_tags = data;
}

void *splt_tu_get_original_tags_data(splt_state *state)
{
  return state->original_tags.all_original_tags;
}

splt_tags *splt_tu_get_original_tags_tags(splt_state *state)
{
  return &state->original_tags.tags;
}

static char *splt_tu_get_replaced_with_tags(const char *word,
    const splt_tags *tags, const splt_tags *original_tags,
    int track, int *error, int replace_tags_in_tags, 
    int current_split, splt_state *state)
{
  int err = SPLT_OK;

  if (current_split == -1)
  {
    current_split = splt_t_get_current_split_file_number(state) - 1;
  }

  long mins = -1; long secs = -1; long hundr = -1;
  long point_value = splt_sp_get_splitpoint_value(state, current_split, &err);
  splt_co_get_mins_secs_hundr(point_value, &mins, &secs, &hundr);
  long next_mins = -1; long next_secs = -1; long next_hundr = -1;
  long next_point_value = -1;
  if (splt_sp_splitpoint_exists(state, current_split + 1))
  {
    next_point_value = splt_sp_get_splitpoint_value(state, current_split + 1, &err);
    long total_time = splt_t_get_total_time(state);
    if (total_time > 0 && next_point_value > total_time)
    {
      next_point_value = total_time;
    }
    splt_co_get_mins_secs_hundr(next_point_value, &next_mins, &next_secs, &next_hundr);
  }
  short write_eof = SPLT_FALSE;
  if (next_point_value == LONG_MAX)
  {
    write_eof = SPLT_TRUE;
  }

  long mMsShH_value = 1;
  short eof_written = SPLT_FALSE;

  char *word_with_tags = NULL;

  if (!replace_tags_in_tags)
  {
    err = splt_su_copy(word, &word_with_tags);
    if (err < 0) { *error = err; }
    return word_with_tags;
  }

  char buffer[256] = { '\0' };

  if (word == NULL)
  {
    return NULL;
  }

  const splt_tags *tags_to_replace = tags;

  int counter = 0;
  const char *ptr = NULL;
  for (ptr = word; *ptr != '\0'; ptr++)
  {
    if (*ptr == '@')
    {
      tags_to_replace = tags;
    }
    else if (*ptr == '#')
    {
      tags_to_replace = original_tags;
    }

    const char *title = tags_to_replace->title;
    const char *artist = tags_to_replace->artist;
    const char *album= tags_to_replace->album;
    const char *performer = tags_to_replace->performer;
    const char *year = tags_to_replace->year;
    const char *comment = tags_to_replace->comment;
    const char *genre = tags_to_replace->genre;
    int real_track = tags_to_replace->track;

    if (*ptr == '@' || *ptr == '#')
    {
      err = splt_su_append(&word_with_tags, buffer, counter, NULL);
      if (err != SPLT_OK) { goto error; }

      memset(buffer, 256, '\0');
      counter = 0;

      ptr++;

      switch (*ptr)
      {
        case 's':
          mMsShH_value = secs;
          goto put_value;
        case 'S':
          mMsShH_value = next_secs;
          goto put_value;
        case 'm':
          mMsShH_value = mins;
          goto put_value;
        case 'M':
          mMsShH_value = next_mins;
          goto put_value;
        case 'h':
          mMsShH_value = hundr;
          goto put_value;
        case 'H':
          mMsShH_value = next_hundr;
put_value:
          if (!eof_written)
          {
            if (write_eof &&
                (*ptr == 'S' || *ptr == 'M' || *ptr == 'H'))
            {
              write_eof = SPLT_FALSE;
              eof_written = SPLT_TRUE;

              err = splt_su_append_str(&word_with_tags, "EOF", NULL);
              if (err != SPLT_OK) { goto error; }
            }
            else if (mMsShH_value != -1)
            {
              char temp[6] = { '\0' };
              temp[0] = '%';
              temp[1] = '0';
              char number_of_digits = '2';
              if (*ptr == 'M' || *ptr == 'm')
              {
                number_of_digits = splt_of_get_number_of_digits_from_total_time(state);
              }
              temp[2] = number_of_digits;
              temp[3] = 'l';
              temp[4] = 'd';

              char mMsShH_str[100] = { '\0' };
              snprintf(mMsShH_str, 100, temp, mMsShH_value);

              err = splt_su_append_str(&word_with_tags, mMsShH_str, NULL);
              if (err != SPLT_OK) { goto error; }
            }
          }
          break;
        case 'a':
          if (artist != NULL)
          {
            err = splt_su_append_str(&word_with_tags, artist, NULL);
            if (err != SPLT_OK) { goto error; }
          }
          break;
        case 'p':
          if (performer != NULL)
          {
            err = splt_su_append_str(&word_with_tags, performer, NULL);
            if (err != SPLT_OK) { goto error; }
          }
          break;
        case 'b':
          if (album != NULL)
          {
            err = splt_su_append_str(&word_with_tags, album, NULL);
            if (err != SPLT_OK) { goto error; }
          }
          break;
        case 't':
          if (title != NULL)
          {
            err = splt_su_append_str(&word_with_tags, title, NULL);
            if (err != SPLT_OK) { goto error; }
          }
          break;
        case 'c':
          if (comment != NULL)
          {
            err = splt_su_append_str(&word_with_tags, comment, NULL);
            if (err != SPLT_OK) { goto error; }
          }
          break;
        case 'g':
          if (genre != NULL)
          {
            err = splt_su_append_str(&word_with_tags, genre, NULL);
            if (err != SPLT_OK) { goto error; }
          }
          break;
        case 'y':
          if (year != NULL)
          {
            err = splt_su_append(&word_with_tags, year, NULL);
            if (err != SPLT_OK) { goto error; }
          }
          break;
        case 'N':
        case 'n':
          ;
          int track_to_set = track;

          if (*ptr == 'n')
          {
            track_to_set = real_track;
          }

          char temp[5] = { '\0' };
          temp[0] = '%';
          temp[1] = '0';
          temp[2] = splt_of_get_oformat_number_of_digits_as_char(state);
          temp[3] = 'd';

          char track_str[10] = { '\0' };
          snprintf(track_str, 10, temp, track_to_set);

          err = splt_su_append_str(&word_with_tags, track_str, NULL);
          if (err != SPLT_OK) { goto error; }
          break;
        case '@':
          err = splt_su_append_str(&word_with_tags, "@", NULL);
          if (err != SPLT_OK) { goto error; }
          break;
        default:
          err = splt_su_append(&word_with_tags, (ptr-1), 2, NULL);
          if (err != SPLT_OK) { goto error; }
          break;
      }
    }
    else
    {
      buffer[counter] = *ptr;
      counter++;

      if (counter == 255)
      {
        err = splt_su_append(&word_with_tags, buffer, counter, NULL);
        if (err != SPLT_OK) { goto error; }
        memset(buffer, 256, '\0');
        counter = 0;
      }
    }
  }

  err = splt_su_append(&word_with_tags, buffer, counter, NULL);
  if (err != SPLT_OK) { goto error; }

  return word_with_tags;

error:
  if (word_with_tags)
  {
    free(word_with_tags);
  }

  *error = err;

  return NULL;
}

void splt_tu_free_one_tags(splt_tags **tags)
{
  if (!tags || !*tags)
  {
    return;
  }

  splt_tu_free_one_tags_content(*tags);

  free(*tags);
  *tags = NULL;
}

void splt_tu_free_one_tags_content(splt_tags *tags)
{
  if (tags)
  {
    if (tags->title)
    {
      free(tags->title);
      tags->title = NULL;
    }
    if (tags->artist)
    {
      free(tags->artist);
      tags->artist = NULL;
    }
    if (tags->album)
    {
      free(tags->album);
      tags->album = NULL;
    }
    if (tags->performer)
    {
      free(tags->performer);
      tags->performer = NULL;
    }
    if (tags->year)
    {
      free(tags->year);
      tags->year = NULL;
    }
    if (tags->comment)
    {
      free(tags->comment);
      tags->comment = NULL;
    }
    if (tags->genre)
    {
      free(tags->genre);
      tags->genre = NULL;
    }

    tags->track = -1;
  }
}

void splt_tu_append_tags_to_state(splt_state *state, splt_tags *tags, 
    int append_new_tags, int *error)
{
  int err = SPLT_OK;

  if (append_new_tags)
  {
    err = splt_tu_append_tags(state, tags->title, tags->artist, tags->album,
        tags->performer, tags->year, tags->comment, tags->track, tags->genre,
        SPLT_FALSE);
  }
  else
  {
    err = splt_tu_append_only_non_null_previous_tags(state, tags->title,
        tags->artist, tags->album, tags->performer, tags->year, 
        tags->comment, tags->track, tags->genre,
        SPLT_TRUE);
  }

  if (err < 0)
  {
    *error = err;
  }
}

void splt_tu_set_new_tags_where_current_tags_are_null(splt_state *state,
    splt_tags *current_tags, splt_tags *new_tags,
    int index, int *error)
{
  if (!current_tags->title)
  {
    splt_tu_set_tags_field(state, index, SPLT_TAGS_TITLE, new_tags->title);
  }
  if (!current_tags->artist)
  {
    splt_tu_set_tags_field(state, index, SPLT_TAGS_ARTIST, new_tags->artist);
  }
  if (!current_tags->album)
  {
    splt_tu_set_tags_field(state, index, SPLT_TAGS_ALBUM, new_tags->album);
  }
  if (!current_tags->performer)
  {
    splt_tu_set_tags_field(state, index, SPLT_TAGS_PERFORMER,
        new_tags->performer);
  }
  if (!current_tags->year)
  {
    splt_tu_set_tags_field(state, index, SPLT_TAGS_YEAR, new_tags->year);
  }
  if (!current_tags->comment)
  {
    splt_tu_set_tags_field(state, index, SPLT_TAGS_COMMENT, new_tags->comment);
  }
  if (current_tags->track < 0)
  {
    splt_tu_set_tags_field(state, index, SPLT_TAGS_TRACK, &new_tags->track);
  }
  if (!current_tags->genre)
  {
    splt_tu_set_tags_field(state, index, SPLT_TAGS_GENRE, new_tags->genre);
  }
  splt_tu_set_tags_field(state, index, SPLT_TAGS_ORIGINAL, &new_tags->set_original_tags);
}

int splt_tu_has_one_tag_set(splt_tags *tags)
{
  if (tags->title != NULL ||
      tags->artist != NULL ||
      tags->album != NULL ||
      tags->performer != NULL ||
      tags->year != NULL ||
      tags->comment != NULL ||
      tags->track != -1 ||
      tags->genre != NULL)
  {
    return SPLT_TRUE;
  }

  return SPLT_FALSE;
}

void splt_tu_copy_tags(splt_tags *from, splt_tags *to, int *error)
{
  int err = SPLT_OK;

  err = splt_tu_set_field_on_tags(to, SPLT_TAGS_TITLE, from->title);
  if (err < 0) { goto error; }

  err = splt_tu_set_field_on_tags(to, SPLT_TAGS_ARTIST, from->artist);
  if (err < 0) { goto error; }

  err = splt_tu_set_field_on_tags(to, SPLT_TAGS_ALBUM, from->album);
  if (err < 0) { goto error; }

  err = splt_tu_set_field_on_tags(to, SPLT_TAGS_YEAR, from->year);
  if (err < 0) { goto error; }

  err = splt_tu_set_field_on_tags(to, SPLT_TAGS_COMMENT, from->comment);
  if (err < 0) { goto error; }

  err = splt_tu_set_field_on_tags(to, SPLT_TAGS_PERFORMER, from->performer);
  if (err < 0) { goto error; }

  err = splt_tu_set_field_on_tags(to, SPLT_TAGS_TRACK, &from->track);
  if (err < 0) { goto error; }

  err = splt_tu_set_field_on_tags(to, SPLT_TAGS_ORIGINAL, &from->set_original_tags);
  if (err < 0) { goto error; }

  err = splt_tu_set_field_on_tags(to, SPLT_TAGS_GENRE, from->genre);
  if (err < 0) { goto error; }

  err = splt_tu_set_field_on_tags(to, SPLT_TAGS_VERSION, &from->tags_version);
  if (err < 0) { goto error; }

  return;

error:
  *error = err;
}

static splt_tags *splt_tu_get_tags_to_replace_in_tags(splt_state *state)
{
  int current_tags_number = splt_t_get_current_split_file_number(state) - 1;

  int remaining_tags_like_x = splt_o_get_int_option(state, SPLT_OPT_ALL_REMAINING_TAGS_LIKE_X); 

  if ((current_tags_number >= state->split.real_tagsnumber) &&
      (remaining_tags_like_x != -1))
  {
    return splt_tu_get_tags_like_x(state);
  }

  return splt_tu_get_tags_at(state, current_tags_number);
}

int splt_tu_set_tags_in_tags(splt_state *state, int current_split)
{
  int err = SPLT_OK;

  splt_tags *tags = splt_tu_get_tags_to_replace_in_tags(state);
  splt_tags *cur_tags = splt_tu_get_current_tags(state);

  if (!tags || !cur_tags)
  {
    return err;
  }

  int track = -1;
  if (tags->track > 0)
  {
    track = tags->track;
  }
  else if (splt_tu_has_one_tag_set(tags))
  {
    if (current_split != -1)
    {
      track = current_split + 1;
    }
    else
    {
      track = splt_t_get_current_split_file_number(state);
    }
  }

  cur_tags->track = track;
  cur_tags->tags_version = tags->tags_version;

  int replace_tags_in_tags = splt_o_get_int_option(state, SPLT_OPT_REPLACE_TAGS_IN_TAGS);

  splt_tags *original_tags = splt_tu_get_original_tags_tags(state);

  char *t = splt_tu_get_replaced_with_tags(tags->title, tags, original_tags,
      track, &err, replace_tags_in_tags, current_split, state);
  if (err != SPLT_OK) { return err; }
  char *y = splt_tu_get_replaced_with_tags(tags->year, tags, original_tags, track, &err, replace_tags_in_tags, 
      current_split, state);
  if (err != SPLT_OK) { return err; }
  char *a = splt_tu_get_replaced_with_tags(tags->artist, tags, original_tags, track, &err, replace_tags_in_tags, 
      current_split, state);
  if (err != SPLT_OK) { return err; }
  char *al = splt_tu_get_replaced_with_tags(tags->album, tags, original_tags, track, &err, replace_tags_in_tags, 
      current_split, state);
  if (err != SPLT_OK) { return err; }
  char *c = splt_tu_get_replaced_with_tags(tags->comment, tags, original_tags, track, &err, replace_tags_in_tags,
      current_split, state);
  if (err != SPLT_OK) { return err; }
  char *g = splt_tu_get_replaced_with_tags(tags->genre, tags, original_tags, track, &err, replace_tags_in_tags,
      current_split, state);
  if (err != SPLT_OK) { return err; }

  splt_su_free_replace(&cur_tags->title, t);
  splt_su_free_replace(&cur_tags->year, y);
  splt_su_free_replace(&cur_tags->artist, a);
  splt_su_free_replace(&cur_tags->album, al);
  splt_su_free_replace(&cur_tags->comment, c);
  splt_su_free_replace(&cur_tags->genre, g);

  return err;
}

splt_tags *splt_tu_get_tags_at(splt_state *state, int tags_index)
{
  if (!splt_tu_tags_exists(state, tags_index))
  {
    return NULL;
  }

  return &state->split.tags[tags_index];
}

splt_tags splt_tu_get_last_tags(splt_state *state)
{
  return state->split.tags[state->split.real_tagsnumber-1];
}

void *splt_tu_get_tags_field(splt_state *state, int index, int tags_field)
{
  if ((index >= state->split.real_tagsnumber) || (index < 0))
  {
    splt_e_error(SPLT_IERROR_INT,__func__, index, NULL);
    return NULL;
  }
  else
  {
    switch(tags_field)
    {
      case SPLT_TAGS_TITLE:
        return state->split.tags[index].title;
        break;
      case SPLT_TAGS_ARTIST:
        return state->split.tags[index].artist;
        break;
      case SPLT_TAGS_ALBUM:
        return state->split.tags[index].album;
        break;
      case SPLT_TAGS_YEAR:
        return state->split.tags[index].year;
        break;
      case SPLT_TAGS_COMMENT:
        return state->split.tags[index].comment;
        break;
      case SPLT_TAGS_PERFORMER:
        return state->split.tags[index].performer;
        break;
      case SPLT_TAGS_TRACK:
        return &state->split.tags[index].track;
        break;
      case SPLT_TAGS_VERSION:
        return &state->split.tags[index].tags_version;
        break;
      case SPLT_TAGS_GENRE:
        return state->split.tags[index].genre;
        break;
      case SPLT_TAGS_ORIGINAL:
        return &state->split.tags[index].set_original_tags;
        break;
      default:
        splt_e_error(SPLT_IERROR_INT,__func__, index, NULL);
        return NULL;
    }
  }

  return NULL;
}

splt_tags *splt_tu_get_tags_like_x(splt_state *state)
{
  return &state->split.tags_like_x;
}

void splt_tu_free_tags(splt_state *state)
{
  if (state->split.tags)
  {
    int i = 0;
    for (i = 0; i < state->split.real_tagsnumber; i++)
    {
      splt_tu_free_one_tags_content(&state->split.tags[i]);
    }
    free(state->split.tags);
    state->split.tags = NULL;
  }

  state->split.real_tagsnumber = 0;

  splt_tu_free_one_tags_content(splt_tu_get_tags_like_x(state));
}

splt_tags *splt_tu_get_current_tags(splt_state *state)
{
  int current_tags_number = splt_t_get_current_split_file_number(state) - 1;

  int remaining_tags_like_x = splt_o_get_int_option(state, SPLT_OPT_ALL_REMAINING_TAGS_LIKE_X); 
  if ((current_tags_number >= state->split.real_tagsnumber) &&
      (remaining_tags_like_x != -1))
  {
    current_tags_number = remaining_tags_like_x; 
  }

  return splt_tu_get_tags_at(state, current_tags_number);
}

char *splt_tu_get_artist_or_performer_ptr(splt_tags *tags)
{
  if (!tags)
  {
    return NULL;
  }

  char *artist_or_performer = tags->artist;

  if (tags->performer == NULL)
  {
    return artist_or_performer;
  }

  if (tags->performer[0] != '\0')
  {
    return tags->performer;
  }

  return artist_or_performer;
}

int splt_tu_copy_first_common_tags_on_all_tracks(splt_state *state, int tracks)
{
  int err = SPLT_OK;

  char *artist0 = NULL;
  char *album0 = NULL;
  char *year0 = NULL;
  char *genre0 = NULL;

  char *first_artist = (char *)splt_tu_get_tags_field(state, 0, SPLT_TAGS_ARTIST);
  err = splt_su_copy(first_artist, &artist0);
  if (err < 0) { goto function_end; }

  char *first_album = (char *)splt_tu_get_tags_field(state, 0, SPLT_TAGS_ALBUM);
  err = splt_su_copy(first_album, &album0);
  if (err < 0) { goto function_end; }

  char *first_year = (char *)splt_tu_get_tags_field(state, 0, SPLT_TAGS_YEAR);
  err = splt_su_copy(first_year, &year0);
  if (err < 0) { goto function_end; }

  char *first_genre = (char *)splt_tu_get_tags_field(state, 0, SPLT_TAGS_GENRE);
  err = splt_su_copy(first_genre, &genre0);
  if (err < 0) { goto function_end; }

  int i = 0;
  for (i = 0; i < tracks;i++)
  {
    if (i != 0)
    {
      err = splt_tu_set_tags_field(state, i, SPLT_TAGS_ARTIST, artist0);
      if (err != SPLT_OK) { break; }

      err = splt_tu_set_tags_field(state, i, SPLT_TAGS_ALBUM, album0);
      if (err != SPLT_OK) { break; }

      err = splt_tu_set_tags_field(state, i, SPLT_TAGS_YEAR, year0);
      if (err != SPLT_OK) { break; }

      err = splt_tu_set_tags_field(state, i, SPLT_TAGS_GENRE, genre0);
      if (err != SPLT_OK) { break; }
    }
  }

function_end:
  if (artist0)
  {
    free(artist0);
    artist0 = NULL;
  }
  if (album0)
  {
    free(album0);
    album0 = NULL;
  }
  if (year0)
  {
    free(year0);
    year0 = NULL;
  }
  if (genre0)
  {
    free(genre0);
    genre0 = NULL;
  }

  return err;
}

int splt_tu_set_field_on_tags(splt_tags *tags, int tags_field, const void *data)
{
  int err = SPLT_OK;

  switch (tags_field)
  {
    case SPLT_TAGS_TITLE:
      err = splt_su_copy((char *)data, &tags->title);
      break;
    case SPLT_TAGS_ARTIST:
      err = splt_su_copy((char *)data, &tags->artist);
      break;
    case SPLT_TAGS_ALBUM:
      err = splt_su_copy((char *)data, &tags->album);
      break;
    case SPLT_TAGS_YEAR:
      err = splt_su_copy((char *)data, &tags->year);
      break;
    case SPLT_TAGS_COMMENT:
      err = splt_su_copy((char *)data, &tags->comment);
      break;
    case SPLT_TAGS_PERFORMER:
      err = splt_su_copy((char *)data, &tags->performer);
      break;
    case SPLT_TAGS_TRACK:
      tags->track = *((int *)data);
      break;
    case SPLT_TAGS_GENRE:
      err = splt_su_copy((char *)data, &tags->genre);
      break;
    case SPLT_TAGS_VERSION:
      tags->tags_version = *((int *)data);
      break;
    case SPLT_TAGS_ORIGINAL:
      tags->set_original_tags = *((int *)data);
      break;
    default:
      splt_e_error(SPLT_IERROR_INT,__func__, -500, NULL);
      break;
  }

  return err;
}


