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
#include "plugin_utils.h"

static char *splt_pu_replace_with_tags(const char *word, splt_tags *tags, int track);

splt_tags *splt_pu_replace_tags_in_tags(splt_tags *tags,
    int current_tags, const char *artist_or_performer, int track)
{
  splt_tags *new_tags = malloc(sizeof(splt_tags));
  splt_t_reset_tags(new_tags);

  //todo: handle errors
  if (new_tags == NULL)
  {
    return NULL;
  }

  new_tags->title = splt_pu_replace_with_tags(tags[current_tags].title,
      &tags[current_tags], track);
  new_tags->artist = splt_pu_replace_with_tags(artist_or_performer,
      &tags[current_tags], track);
  new_tags->album = splt_pu_replace_with_tags(tags[current_tags].album,
      &tags[current_tags], track);
  new_tags->year = splt_pu_replace_with_tags(tags[current_tags].year,
      &tags[current_tags], track);
  new_tags->comment = splt_pu_replace_with_tags(tags[current_tags].comment,
      &tags[current_tags], track);

  return new_tags;
}

static char *splt_pu_replace_with_tags(const char *word, splt_tags *tags, int track)
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

