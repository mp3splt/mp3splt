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
    splt_tags *tags, int track, int *err);
static splt_tags *splt_tu_get_tags_to_replace_in_tags(splt_state *state);

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

  int remaining_tags_like_x = splt_t_get_int_option(state, SPLT_OPT_ALL_REMAINING_TAGS_LIKE_X); 
  if (remaining_tags_like_x != -1)
  {
    if (current_split >= state->split.real_tagsnumber)
    {
      current_split = remaining_tags_like_x;
    }

    if (splt_t_get_int_option(state, SPLT_OPT_AUTO_INCREMENT_TRACKNUMBER_TAGS) > 0)
    {
      if (current_split == remaining_tags_like_x)
      {
        if ((old_current_split > 0) && 
            (old_current_split-1 < state->split.real_tagsnumber) && 
            (old_current_split != remaining_tags_like_x))
        {
          int previous_track = splt_tu_get_tags_int_field(state, old_current_split - 1, SPLT_TAGS_TRACK);
          splt_tu_set_tags_int_field(state, remaining_tags_like_x, SPLT_TAGS_TRACK, previous_track);
        }

        if (old_current_split != current_split)
        {
          int tracknumber = 1;
          if (splt_tu_tags_exists(state, current_split))
          {
            tracknumber = splt_tu_get_tags_int_field(state, current_split, SPLT_TAGS_TRACK);
          }
          splt_tu_set_tags_int_field(state, current_split, SPLT_TAGS_TRACK, tracknumber+1);
          splt_tu_set_like_x_tags_field(state, SPLT_TAGS_TRACK, tracknumber+1,
              NULL, 0x0);
        }
      }
    }
  }
}

void splt_tu_get_original_tags(splt_state *state, int *err)
{
  if (! splt_t_is_stdin(state))
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

  error = splt_tu_set_tags_char_field(state,
      old_tagsnumber, SPLT_TAGS_TITLE, title);
  if (error != SPLT_OK)
    return error;

  error = splt_tu_set_tags_char_field(state,
      old_tagsnumber, SPLT_TAGS_ARTIST, artist);
  if (error != SPLT_OK)
    return error;

  error = splt_tu_set_tags_char_field(state,
      old_tagsnumber, SPLT_TAGS_ALBUM, album);
  if (error != SPLT_OK)
    return error;

  error = splt_tu_set_tags_char_field(state,
      old_tagsnumber, SPLT_TAGS_PERFORMER, performer);
  if (error != SPLT_OK)
    return error;

  error = splt_tu_set_tags_char_field(state,
      old_tagsnumber, SPLT_TAGS_YEAR, year);
  if (error != SPLT_OK)
    return error;

  error = splt_tu_set_tags_char_field(state,
      old_tagsnumber, SPLT_TAGS_COMMENT, comment);
  if (error != SPLT_OK)
    return error;

  error = splt_tu_set_tags_int_field(state,
      old_tagsnumber, SPLT_TAGS_TRACK, track);
  if (error != SPLT_OK)
    return error;

  error = splt_tu_set_tags_uchar_field(state,
      old_tagsnumber, SPLT_TAGS_GENRE, genre);
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
      error = splt_tu_set_tags_char_field(state,
          old_tagsnumber, SPLT_TAGS_TITLE, title);
    }
    if (error != SPLT_OK)
      return error;
    if (artist != NULL)
    {
      error = splt_tu_set_tags_char_field(state,
          old_tagsnumber, SPLT_TAGS_ARTIST, artist);
    }
    if (error != SPLT_OK)
      return error;
    if (album != NULL)
    {
      error = splt_tu_set_tags_char_field(state,
          old_tagsnumber, SPLT_TAGS_ALBUM, album);
    }
    if (error != SPLT_OK)
      return error;
    if (performer != NULL)
    {
      error = splt_tu_set_tags_char_field(state,
          old_tagsnumber, SPLT_TAGS_PERFORMER, performer);
    }
    if (error != SPLT_OK)
      return error;
    if (year != NULL)
    {
      error = splt_tu_set_tags_char_field(state,
          old_tagsnumber, SPLT_TAGS_YEAR, year);
    }
    if (error != SPLT_OK)
      return error;
    if (comment != NULL)
    {
      error = splt_tu_set_tags_char_field(state,
          old_tagsnumber, SPLT_TAGS_COMMENT, comment);
    }
    if (error != SPLT_OK)
      return error;
    if (track != -1)
    {
      error = splt_tu_set_tags_int_field(state,
          old_tagsnumber, SPLT_TAGS_TRACK, track);
    }
    if (error != SPLT_OK)
      return error;
    if (genre != 12)
    {
      error = splt_tu_set_tags_uchar_field(state,
          old_tagsnumber, SPLT_TAGS_GENRE, genre);
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

//-index should not be out of bounds
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

//set title, artist, album, year or comment
int splt_tu_set_tags_char_field(splt_state *state, int index,
    int tags_field, const char *data)
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
    switch(tags_field)
    {
      case SPLT_TAGS_TITLE:
        splt_u_print_debug(state,"Setting title tags at ",index,data);
        if (state->split.tags[index].title)
        {
          free(state->split.tags[index].title);
          state->split.tags[index].title = NULL;
        }
        if (data == NULL)
        {
          state->split.tags[index].title = NULL;
        }
        else
        {
          if ((state->split.tags[index].title = malloc((strlen(data)+1) * 
                  sizeof(char))) == NULL)
          {
            error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
          }
          else
          {
            snprintf(state->split.tags[index].title,(strlen(data)+1),"%s", data);
          }
        }
        break;
      case SPLT_TAGS_ARTIST:
        splt_u_print_debug(state,"Setting artist tags at ",index,data);
        if (state->split.tags[index].artist)
        {
          free(state->split.tags[index].artist);
          state->split.tags[index].artist = NULL;
        }
        if (data == NULL)
        {
          state->split.tags[index].artist = NULL;
        }
        else
        {
          if ((state->split.tags[index].artist = malloc((strlen(data)+1) * 
                  sizeof(char))) == NULL)
          {
            error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
          }
          else
          {
            snprintf(state->split.tags[index].artist,(strlen(data)+1),"%s", data);
          }
        }
        break;
      case SPLT_TAGS_ALBUM:
        splt_u_print_debug(state,"Setting album tags at ",index,data);
        if (state->split.tags[index].album)
        {
          free(state->split.tags[index].album);
          state->split.tags[index].album = NULL;
        }
        if (data == NULL)
        {
          state->split.tags[index].album = NULL;
        }
        else
        {
          if ((state->split.tags[index].album = malloc((strlen(data)+1) * 
                  sizeof(char))) == NULL)
          {
            error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
          }
          else
          {
            snprintf(state->split.tags[index].album,(strlen(data)+1),"%s", data);
          }
        }
        break;
      case SPLT_TAGS_YEAR:
        splt_u_print_debug(state,"Setting year tags at ",index,data);
        if (state->split.tags[index].year)
        {
          free(state->split.tags[index].year);
          state->split.tags[index].year = NULL;
        }
        if (data == NULL)
        {
          state->split.tags[index].year = NULL;
        }
        else
        {
          if ((state->split.tags[index].year = malloc((strlen(data)+1) * 
                  sizeof(char))) == NULL)
          {
            error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
          }
          else
          {
            snprintf(state->split.tags[index].year,(strlen(data)+1),"%s", data);
          }
        }
        break;
      case SPLT_TAGS_COMMENT:
        if (state->split.tags[index].comment)
        {
          free(state->split.tags[index].comment);
          state->split.tags[index].comment = NULL;
        }
        if (data == NULL)
        {
          state->split.tags[index].comment = NULL;
        }
        else
        {
          if ((state->split.tags[index].comment = malloc((strlen(data)+1) * 
                  sizeof(char))) == NULL)
          {
            error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
          }
          else
          {
            snprintf(state->split.tags[index].comment,(strlen(data)+1),"%s", data);
          }
        }
        break;
      case SPLT_TAGS_PERFORMER:
        splt_u_print_debug(state,"Setting performer tags at ",index,data);
        if (state->split.tags[index].performer)
        {
          free(state->split.tags[index].performer);
          state->split.tags[index].performer = NULL;
        }
        if (data == NULL)
        {
          state->split.tags[index].performer = NULL;
        }
        else
        {
          if ((state->split.tags[index].performer = malloc((strlen(data)+1) * 
                  sizeof(char))) == NULL)
          {
            error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
          }
          else
          {
            snprintf(state->split.tags[index].performer,(strlen(data)+1),"%s", data);
          }
        }
        break;
      default:
        break;
    }
  }

  if (error != SPLT_OK)
  {
    splt_u_error(SPLT_IERROR_INT,__func__, index, NULL);
  }

  return error;
}

static int splt_tu_set_tags_field(splt_tags *tags,
    int tags_field, int int_data, const char *char_data,
    unsigned char uchar_data)
{
  int err = SPLT_OK;

  switch (tags_field)
  {
    case SPLT_TAGS_TITLE:
      err = splt_su_copy(char_data, &tags->title);
      break;
    case SPLT_TAGS_ARTIST:
      err = splt_su_copy(char_data, &tags->artist);
      break;
    case SPLT_TAGS_ALBUM:
      err = splt_su_copy(char_data, &tags->album);
      break;
    case SPLT_TAGS_YEAR:
      err = splt_su_copy(char_data, &tags->year);
      break;
    case SPLT_TAGS_COMMENT:
      err = splt_su_copy(char_data, &tags->comment);
      break;
    case SPLT_TAGS_PERFORMER:
      err = splt_su_copy(char_data, &tags->performer);
      break;
    case SPLT_TAGS_TRACK:
      tags->track = int_data;
      break;
    case SPLT_TAGS_GENRE:
      tags->genre = uchar_data;
      break;
    case SPLT_TAGS_VERSION:
      tags->tags_version = int_data;
      break;
    default:
      splt_u_error(SPLT_IERROR_INT,__func__, -500, NULL);
      break;
  }

  return err;
}

int splt_tu_set_like_x_tags_field(splt_state *state,
    int tags_field, int int_data,
    const char *char_data, unsigned char uchar_data)
{
  return splt_tu_set_tags_field(&state->split.tags_like_x, tags_field,
      int_data, char_data, uchar_data);
}

int splt_tu_set_original_tags_field(splt_state *state,
    int tags_field, int int_data,
    const char *char_data, unsigned char uchar_data)
{
  return splt_tu_set_tags_field(&state->original_tags, tags_field,
      int_data, char_data, uchar_data);
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

    char *t = splt_tu_get_replaced_with_tags(tags->title, tags, track, &err);
    if (err != SPLT_OK) { return err; }
    splt_su_free_replace(&cur_tags->title, t);

    char *a = splt_tu_get_replaced_with_tags(tags->artist, tags, track, &err);
    if (err != SPLT_OK) { return err; }
    splt_su_free_replace(&cur_tags->artist, a);

    char *al = splt_tu_get_replaced_with_tags(tags->album, tags, track, &err);
    if (err != SPLT_OK) { return err; }
    splt_su_free_replace(&cur_tags->album, al);

    char *y = splt_tu_get_replaced_with_tags(tags->year, tags, track, &err);
    if (err != SPLT_OK) { return err; }
    splt_su_free_replace(&cur_tags->year, y);

    char *c = splt_tu_get_replaced_with_tags(tags->comment, tags, track, &err);
    if (err != SPLT_OK) { return err; }
    splt_su_free_replace(&cur_tags->comment, c);
  }

  return err;
}

//set track number
int splt_tu_set_tags_int_field(splt_state *state, int index,
    int tags_field, int data)
{
  int error = SPLT_OK;

  error = splt_tu_new_tags_if_necessary(state,index);

  if (error != SPLT_OK)
  {
    splt_u_error(SPLT_IERROR_INT,__func__, index, NULL);
    return error;
  }

  if ((index >= state->split.real_tagsnumber)
      || (index < 0))
  {
    error = SPLT_ERROR_INEXISTENT_SPLITPOINT;
    splt_u_error(SPLT_IERROR_INT,__func__, index, NULL);
    return error;
  }
  else
  {
    char temp[100] = { '\0' };
    switch (tags_field)
    {
      case SPLT_TAGS_TRACK:
        //debug
        snprintf(temp,100,"%d",data);
        splt_u_print_debug(state,"Setting track tags at",index,temp);
        state->split.tags[index].track = data;
        break;
      case SPLT_TAGS_VERSION:
        splt_u_print_debug(state,"Setting tags version at",index,temp);
        state->split.tags[index].tags_version = data;
        break;
      default:
        break;
    }
  }

  return error;
}

int splt_tu_set_tags_uchar_field(splt_state *state, int index,
    int tags_field, unsigned char data)
{
  int error = SPLT_OK;

  error = splt_tu_new_tags_if_necessary(state,index);
  if (error < 0)
  {
    splt_u_error(SPLT_IERROR_INT,__func__, index, NULL);
    return error;
  }

  if ((index >= state->split.real_tagsnumber) || (index < 0))
  {
    error = SPLT_ERROR_INEXISTENT_SPLITPOINT;
    splt_u_error(SPLT_IERROR_INT,__func__, index, NULL);
    return error;
  }
  else
  {
    char temp[100] = { '\0' };
    switch (tags_field)
    {
      case SPLT_TAGS_GENRE:
        //debug
        snprintf(temp,100,"%uc",data);
        splt_u_print_debug(state,"Setting genre tags at",index,temp);
        state->split.tags[index].genre = data;
        break;
      default:
        break;
    }
  }

  return error;
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

char *splt_tu_get_tags_char_field(splt_state *state, int index, int tags_field)
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
      default:
        splt_u_error(SPLT_IERROR_INT,__func__, index, NULL);
        return NULL;
        break;
    }
  }

  return NULL;
}

int splt_tu_get_tags_int_field(splt_state *state, int index, int tags_field)
{
  if ((index >= state->split.real_tagsnumber) || (index < 0))
  {
    splt_u_error(SPLT_IERROR_INT,__func__, index, NULL);
    return 0;
  }
  else
  {
    switch (tags_field)
    {
      case SPLT_TAGS_TRACK:
        return state->split.tags[index].track;
        break;
      case SPLT_TAGS_VERSION:
        return state->split.tags[index].tags_version;
        break;
      default:
        splt_u_error(SPLT_IERROR_INT,__func__, index, NULL);
        break;
    }
  }

  return 0;
}

unsigned char splt_tu_get_tags_uchar_field(splt_state *state, int index,
    int tags_field)
{
  if ((index >= state->split.real_tagsnumber)
      || (index < 0))
  {
    splt_u_error(SPLT_IERROR_INT,__func__, index, NULL);
    return 0x0;
  }
  else
  {
    switch (tags_field)
    {
      case SPLT_TAGS_GENRE:
        return state->split.tags[index].genre;
        break;
      default:
        splt_u_error(SPLT_IERROR_INT,__func__, index, NULL);
        break;
    }
  }

  return 0x0;
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

  int remaining_tags_like_x = splt_t_get_int_option(state, SPLT_OPT_ALL_REMAINING_TAGS_LIKE_X); 
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

  int remaining_tags_like_x = splt_t_get_int_option(state, SPLT_OPT_ALL_REMAINING_TAGS_LIKE_X); 
  if ((current_tags_number >= state->split.real_tagsnumber) &&
      (remaining_tags_like_x != -1))
  {
    return splt_tu_get_tags_like_x(state);
  }

  return splt_tu_get_tags_at(state, current_tags_number);
}

static char *splt_tu_get_replaced_with_tags(const char *word,
    splt_tags *tags, int track, int *error)
{
  int err = SPLT_OK;

  char *word_with_tags = NULL;
  size_t word_with_tags_size = 0;

  char buffer[256] = { '\0' };

  //TODO: no title or artist, reprint @t or @a ?

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
      err = splt_su_append(&word_with_tags, &word_with_tags_size, buffer, counter);
      if (err != SPLT_OK) { goto error; }
      memset(buffer, 256, '\0');
      counter = 0;

      ptr++;

      switch (*ptr)
      {
        case 'a':
          if (artist != NULL)
          {
            err = splt_su_append(&word_with_tags, &word_with_tags_size,
                artist, strlen(artist));
            if (err != SPLT_OK) { goto error; }
          }
          break;
        case 'p':
          if (performer != NULL)
          {
            err = splt_su_append(&word_with_tags, &word_with_tags_size,
                performer, strlen(performer));
            if (err != SPLT_OK) { goto error; }
          }
          break;
        case 'b':
          if (album != NULL)
          {
            err = splt_su_append(&word_with_tags, &word_with_tags_size,
                album, strlen(album));
            if (err != SPLT_OK) { goto error; }
          }
          break;
        case 't':
          if (title != NULL)
          {
            err = splt_su_append(&word_with_tags, &word_with_tags_size,
                title, strlen(title));
            if (err != SPLT_OK) { goto error; }
          }
          break;
        case 'c':
          if (comment != NULL)
          {
            err = splt_su_append(&word_with_tags, &word_with_tags_size,
                comment, strlen(comment));
            if (err != SPLT_OK) { goto error; }
          }
          break;
        case 'y':
          if (year != NULL)
          {
            err = splt_su_append(&word_with_tags, &word_with_tags_size,
                year, strlen(year));
            if (err != SPLT_OK) { goto error; }
          }
          break;
        case 'N':
        case 'n':
          ;
          char track_str[10] = { '\0' };
          snprintf(track_str, 10, "%d",track);
          err = splt_su_append(&word_with_tags, &word_with_tags_size,
              track_str, strlen(track_str));
          if (err != SPLT_OK) { goto error; }
          break;
        case '@':
          err = splt_su_append(&word_with_tags, &word_with_tags_size, "@", 1);
          if (err != SPLT_OK) { goto error; }
          break;
        default:
          err = splt_su_append(&word_with_tags, &word_with_tags_size, (ptr-1), 2);
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
        err = splt_su_append(&word_with_tags, &word_with_tags_size, buffer, counter);
        if (err != SPLT_OK) { goto error; }
        memset(buffer, 256, '\0');
        counter = 0;
      }
    }
  }

  err = splt_su_append(&word_with_tags, &word_with_tags_size, buffer, counter);
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

