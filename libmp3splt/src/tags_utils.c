/**********************************************************
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2009 Alexandru Munteanu - io_fx@yahoo.fr
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

static char *splt_tu_get_replaced_with_tags(const char *word, splt_tags *tags, int track);

//int splt_tu_get_tags_index(splt_state

int splt_tu_set_tags_in_tags(splt_state *state, int current_split)
{
  //TODO: error handling
  int err = SPLT_OK;

  splt_tags *tags = splt_t_get_tags_to_replace_in_tags(state);

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

    splt_tags *cur_tags = splt_t_get_current_tags(state);
    cur_tags->track = track;
    cur_tags->genre = tags->genre;
    cur_tags->tags_version = tags->tags_version;

    splt_su_free_replace(&cur_tags->title,
        splt_tu_get_replaced_with_tags(tags->title, tags, track));
    splt_su_free_replace(&cur_tags->artist,
        splt_tu_get_replaced_with_tags(tags->artist, tags, track));
    splt_su_free_replace(&cur_tags->album,
        splt_tu_get_replaced_with_tags(tags->album, tags, track));
    splt_su_free_replace(&cur_tags->year,
        splt_tu_get_replaced_with_tags(tags->year, tags, track));
    splt_su_free_replace(&cur_tags->comment,
        splt_tu_get_replaced_with_tags(tags->comment, tags, track));
  }

  return err;
}

static char *splt_tu_get_replaced_with_tags(const char *word, splt_tags *tags,
    int track)
{
  char *word_with_tags = NULL;
  size_t word_with_tags_size = 0;

  char buffer[256] = { '\0' };

  //TODO: handle memory errors

  //TODO: set track ??
  //TODO: leaks
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
      splt_su_append(&word_with_tags, &word_with_tags_size, buffer, counter);
      memset(buffer, 256, '\0');
      counter = 0;

      ptr++;

      switch (*ptr)
      {
        case 'a':
          if (artist != NULL)
          {
            splt_su_append(&word_with_tags, &word_with_tags_size,
                artist, strlen(artist));
          }
          break;
        case 'p':
          if (performer != NULL)
          {
            splt_su_append(&word_with_tags, &word_with_tags_size,
                performer, strlen(performer));
          }
          break;
        case 'b':
          if (album != NULL)
          {
            splt_su_append(&word_with_tags, &word_with_tags_size,
                album, strlen(album));
          }
          break;
        case 't':
          if (title != NULL)
          {
            splt_su_append(&word_with_tags, &word_with_tags_size,
                title, strlen(title));
          }
          break;
        case 'c':
          if (comment != NULL)
          {
            splt_su_append(&word_with_tags, &word_with_tags_size,
                comment, strlen(comment));
          }
          break;
        case 'y':
          if (year != NULL)
          {
            splt_su_append(&word_with_tags, &word_with_tags_size,
                year, strlen(year));
          }
          break;
        case 'N':
        case 'n':
          ;
          char track_str[10] = { '\0' };
          snprintf(track_str, 10, "%d",track);
          splt_su_append(&word_with_tags, &word_with_tags_size,
              track_str, strlen(track_str));
          break;
        case '@':
          splt_su_append(&word_with_tags, &word_with_tags_size, "@", 1);
          break;
        default:
          splt_su_append(&word_with_tags, &word_with_tags_size, (ptr-1), 2);
          break;
      }
    }
    else
    {
      buffer[counter] = *ptr;
      counter++;

      if (counter == 255)
      {
        splt_su_append(&word_with_tags, &word_with_tags_size, buffer, counter);
        memset(buffer, 256, '\0');
        counter = 0;
      }
    }
  }

  splt_su_append(&word_with_tags, &word_with_tags_size, buffer, counter);

  return word_with_tags;
}

