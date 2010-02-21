/**********************************************************
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2010 Alexandru Munteanu - io_fx@yahoo.fr
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

static char *splt_tu_get_replaced_with_tags(const char *word,
    const splt_tags *tags, int track, int *err, int replace_tags_in_tags);
static splt_tags *splt_tu_get_tags_to_replace_in_tags(splt_state *state);
static int splt_tu_set_on_tags_field(splt_tags *tags,
    int tags_field, const void *data);
static void splt_tu_set_empty_tags(splt_state *state, int index);
static void splt_tu_free_one_tags(splt_tags *tags);

void splt_tu_free_original_tags(splt_state *state)
{
  if (state->original_tags.year)
  {
    free(state->original_tags.year);
    state->original_tags.year = NULL;
  }
  if (state->original_tags.artist)
  {
    free(state->original_tags.artist);
    state->original_tags.artist = NULL;
  }
  if (state->original_tags.album)
  {
    free(state->original_tags.album);
    state->original_tags.album = NULL;
  }
  if (state->original_tags.title)
  {
    free(state->original_tags.title);
    state->original_tags.title = NULL;
  }
  if (state->original_tags.comment)
  {
    free(state->original_tags.comment);
    state->original_tags.comment = NULL;
  }
  state->original_tags.track = -INT_MAX;
  //12 means "other"
  state->original_tags.genre = 12;
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

int splt_tu_append_original_tags(splt_state *state)
{
  int err = SPLT_OK;

  char *new_title = NULL;
  char *new_artist = NULL;
  char *new_album = NULL;
  char *new_year = NULL;
  char *new_comment = NULL;

  new_title = splt_su_replace_all(state->original_tags.title, "@", "@@", &err);
  if (err != SPLT_OK) { goto end; }

  new_artist = splt_su_replace_all(state->original_tags.artist, "@", "@@", &err);
  if (err != SPLT_OK) { goto end; }

  new_album = splt_su_replace_all(state->original_tags.album, "@", "@@", &err);
  if (err != SPLT_OK) { goto end; }

  new_year = splt_su_replace_all(state->original_tags.year, "@", "@@", &err);
  if (err != SPLT_OK) { goto end; }

  new_comment = splt_su_replace_all(state->original_tags.comment, "@", "@@", &err);
  if (err != SPLT_OK) { goto end; }

  err = splt_tu_append_tags(state, new_title, new_artist, new_album, NULL,
      new_year, new_comment, state->original_tags.track,
      state->original_tags.genre);

end:
  if (new_title) { free(new_title); }
  if (new_artist) { free(new_artist); }
  if (new_album) { free(new_album); }
  if (new_year) { free(new_year); }
  if (new_comment) { free(new_comment); }

  return err;
}

int splt_tu_append_tags(splt_state *state, 
    const char *title, const char *artist,
    const char *album, const char *performer,
    const char *year, const char *comment,
    int track, unsigned char genre)
{
  int error = SPLT_OK;
  int old_tagsnumber = state->split.real_tagsnumber;

  error = splt_tu_set_tags_field(state,
      old_tagsnumber, SPLT_TAGS_TITLE, title);
  if (error != SPLT_OK)
    return error;

  error = splt_tu_set_tags_field(state,
      old_tagsnumber, SPLT_TAGS_ARTIST, artist);
  if (error != SPLT_OK)
    return error;

  error = splt_tu_set_tags_field(state,
      old_tagsnumber, SPLT_TAGS_ALBUM, album);
  if (error != SPLT_OK)
    return error;

  error = splt_tu_set_tags_field(state,
      old_tagsnumber, SPLT_TAGS_PERFORMER, performer);
  if (error != SPLT_OK)
    return error;

  error = splt_tu_set_tags_field(state,
      old_tagsnumber, SPLT_TAGS_YEAR, year);
  if (error != SPLT_OK)
    return error;

  error = splt_tu_set_tags_field(state,
      old_tagsnumber, SPLT_TAGS_COMMENT, comment);
  if (error != SPLT_OK)
    return error;

  error = splt_tu_set_tags_field(state,
      old_tagsnumber, SPLT_TAGS_TRACK, &track);
  if (error != SPLT_OK)
    return error;

  error = splt_tu_set_tags_field(state, old_tagsnumber, SPLT_TAGS_GENRE, &genre);
  return error;
}

//only if char non null and track != -1 and genre != 12
int splt_tu_append_only_non_null_previous_tags(splt_state *state, 
    const char *title, const char *artist,
    const char *album, const char *performer,
    const char *year, const char *comment,
    int track, unsigned char genre)
{
  int error = SPLT_OK;
  int old_tagsnumber = state->split.real_tagsnumber-1;

  if (old_tagsnumber >= 0)
  {
    if (title != NULL)
    {
      error = splt_tu_set_tags_field(state,
          old_tagsnumber, SPLT_TAGS_TITLE, title);
    }
    if (error != SPLT_OK)
      return error;
    if (artist != NULL)
    {
      error = splt_tu_set_tags_field(state,
          old_tagsnumber, SPLT_TAGS_ARTIST, artist);
    }
    if (error != SPLT_OK)
      return error;
    if (album != NULL)
    {
      error = splt_tu_set_tags_field(state,
          old_tagsnumber, SPLT_TAGS_ALBUM, album);
    }
    if (error != SPLT_OK)
      return error;
    if (performer != NULL)
    {
      error = splt_tu_set_tags_field(state,
          old_tagsnumber, SPLT_TAGS_PERFORMER, performer);
    }
    if (error != SPLT_OK)
      return error;
    if (year != NULL)
    {
      error = splt_tu_set_tags_field(state,
          old_tagsnumber, SPLT_TAGS_YEAR, year);
    }
    if (error != SPLT_OK)
      return error;
    if (comment != NULL)
    {
      error = splt_tu_set_tags_field(state,
          old_tagsnumber, SPLT_TAGS_COMMENT, comment);
    }
    if (error != SPLT_OK)
      return error;
    if (track != -1)
    {
      error = splt_tu_set_tags_field(state,
          old_tagsnumber, SPLT_TAGS_TRACK, &track);
    }
    if (error != SPLT_OK)
      return error;
    if (genre != 12)
    {
      error = splt_tu_set_tags_field(state, old_tagsnumber, SPLT_TAGS_GENRE, &genre);
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
  tags->track = -INT_MAX;
  tags->genre = 0x0;
  tags->tags_version = 0;
}

int splt_tu_new_tags_if_necessary(splt_state *state, int index)
{
  int error = SPLT_OK;

  if (!state->split.tags)
  {
    if ((index > state->split.real_tagsnumber) || (index < 0))
    {
      splt_u_error(SPLT_IERROR_INT,__func__, index, NULL);
    }
    else
    {
      if ((state->split.tags = malloc(sizeof(splt_tags))) == NULL)
      {
        error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
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
      splt_u_error(SPLT_IERROR_INT,__func__, index, NULL);
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
    splt_u_error(SPLT_IERROR_INT,__func__, index, NULL);
    return error;
  }
  else
  {
    splt_tu_set_on_tags_field(&state->split.tags[index], tags_field, data);
  }

  if (error != SPLT_OK)
  {
    splt_u_error(SPLT_IERROR_INT,__func__, index, NULL);
  }

  return error;
}

int splt_tu_set_like_x_tags_field(splt_state *state, int tags_field, const void *data)
{
  return splt_tu_set_on_tags_field(&state->split.tags_like_x, tags_field, data);
}

int splt_tu_set_original_tags_field(splt_state *state, int tags_field, const void *data)
{
  return splt_tu_set_on_tags_field(&state->original_tags, tags_field, data);
}

int splt_tu_set_tags_in_tags(splt_state *state, int current_split)
{
  int err = SPLT_OK;

  splt_tags *tags = splt_tu_get_tags_to_replace_in_tags(state);

  if (tags)
  {
    int track = 0;
    if (tags->track > 0)
    {
      track = tags->track;
    }
    else
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

    splt_tags *cur_tags = splt_tu_get_current_tags(state);
    cur_tags->track = track;
    cur_tags->genre = tags->genre;
    cur_tags->tags_version = tags->tags_version;

    int replace_tags_in_tags = splt_o_get_int_option(state, SPLT_OPT_REPLACE_TAGS_IN_TAGS);

    char *t = splt_tu_get_replaced_with_tags(tags->title, tags, track, &err, replace_tags_in_tags);
    if (err != SPLT_OK) { return err; }
    char *y = splt_tu_get_replaced_with_tags(tags->year, tags, track, &err, replace_tags_in_tags);
    if (err != SPLT_OK) { return err; }
    char *a = splt_tu_get_replaced_with_tags(tags->artist, tags, track, &err, replace_tags_in_tags);
    if (err != SPLT_OK) { return err; }
    char *al = splt_tu_get_replaced_with_tags(tags->album, tags, track, &err, replace_tags_in_tags);
    if (err != SPLT_OK) { return err; }
    char *c = splt_tu_get_replaced_with_tags(tags->comment, tags, track, &err, replace_tags_in_tags);
    if (err != SPLT_OK) { return err; }

    splt_su_free_replace(&cur_tags->title, t);
    splt_su_free_replace(&cur_tags->year, y);
    splt_su_free_replace(&cur_tags->artist, a);
    splt_su_free_replace(&cur_tags->album, al);
    splt_su_free_replace(&cur_tags->comment, c);
  }

  return err;
}

splt_tags *splt_tu_get_tags(splt_state *state, int *tags_number)
{
  *tags_number = state->split.real_tagsnumber;
  return state->split.tags;
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
    splt_u_error(SPLT_IERROR_INT,__func__, index, NULL);
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
        return &state->split.tags[index].genre;
        break;
      default:
        splt_u_error(SPLT_IERROR_INT,__func__, index, NULL);
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
      splt_tu_free_one_tags(&state->split.tags[i]);
    }
    free(state->split.tags);
    state->split.tags = NULL;
  }

  state->split.real_tagsnumber = 0;

  splt_tu_free_one_tags(splt_tu_get_tags_like_x(state));
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

static char *splt_tu_get_replaced_with_tags(const char *word,
    const splt_tags *tags, int track, int *error, int replace_tags_in_tags)
{
  int err = SPLT_OK;

  if (!replace_tags_in_tags)
  {
    return splt_su_safe_strdup(word, error);
  }

  char *word_with_tags = NULL;

  char buffer[256] = { '\0' };

  if (word == NULL)
  {
    return NULL;
  }

  const char *title = tags->title;
  const char *artist = tags->artist;
  const char *album= tags->album;
  const char *performer = tags->performer;
  const char *year = tags->year;
  const char *comment = tags->comment;

  int counter = 0;
  const char *ptr = NULL;
  for (ptr = word; *ptr != '\0'; ptr++)
  {
    if (*ptr == '@')
    {
      err = splt_su_append(&word_with_tags, buffer, counter, NULL);
      if (err != SPLT_OK) { goto error; }

      memset(buffer, 256, '\0');
      counter = 0;

      ptr++;

      switch (*ptr)
      {
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
          char track_str[10] = { '\0' };
          snprintf(track_str, 10, "%d",track);
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

static void splt_tu_free_one_tags(splt_tags *tags)
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
  }
}

static void splt_tu_set_empty_tags(splt_state *state, int index)
{
  splt_tu_reset_tags(&state->split.tags[index]);
}

static int splt_tu_set_on_tags_field(splt_tags *tags,
    int tags_field, const void *data)
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
      tags->genre = *((unsigned char *)data);
      break;
    case SPLT_TAGS_VERSION:
      tags->tags_version = *((int *)data);
      break;
    default:
      splt_u_error(SPLT_IERROR_INT,__func__, -500, NULL);
      break;
  }

  return err;
}

