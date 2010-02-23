/*********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2010 Alexandru Munteanu - io_fx@yahoo.fr
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

#include <string.h>
#include <ctype.h>

#include "splt.h"

static char *splt_tp_parse_tag_word(const char *cur_pos,
    const char *end_paranthesis, int *ambiguous, int *error);

int splt_tp_put_tags_from_string(splt_state *state, const char *tags, int *error)
{
  if (tags != NULL)
  {
    const char *cur_pos = NULL;
    int all_tags = SPLT_FALSE;
    int we_had_all_tags = SPLT_FALSE;

    cur_pos = tags;

    int ambiguous = SPLT_FALSE;
    const char *end_paranthesis = NULL;
    if (!strchr(cur_pos,'['))
    {
      ambiguous = SPLT_TRUE;
    }

    //tags that we put to all the others: %[@t=tag1,@b=album1]
    char *all_artist = NULL;
    char *all_album = NULL;
    char *all_title = NULL;
    char *all_performer = NULL;
    char *all_year = NULL;
    char *all_comment = NULL;
    int all_tracknumber = -1;
    int auto_incremented_tracknumber = -1;
    unsigned char all_genre = 12;
    //when using 'N', auto increment the track number
    short auto_increment_tracknumber = SPLT_FALSE;
    short first_time_auto_increment_tracknumber = SPLT_TRUE;

    int tags_appended = 0;
    int get_out_from_while = SPLT_FALSE;

    while ((cur_pos = strchr(cur_pos,'[')))
    {
      //if we set the tags for all the files
      if (cur_pos != tags)
      {
        //if we have % before [
        if (*(cur_pos-1) == '%')
        {
          splt_o_set_int_option(state, SPLT_OPT_ALL_REMAINING_TAGS_LIKE_X, tags_appended);
          all_tags = SPLT_TRUE;
          if (we_had_all_tags)
          {
            if (all_title) { free(all_title); all_title = NULL; }
            if (all_artist) { free(all_artist); all_artist = NULL; }
            if (all_album) { free(all_album); all_album = NULL; }
            if (all_performer) { free(all_performer); all_performer = NULL; }
            if (all_year) { free(all_year); all_year = NULL; }
            if (all_comment) { free(all_comment); all_comment = NULL; }
          }
        }
      }

      char *title = NULL;
      char *artist = NULL;
      char *album = NULL;
      char *performer = NULL;
      char *year = NULL;
      char *comment = NULL;
      char *tracknumber = NULL;
      //means "other"
      unsigned char genre = 12;

      //how many we have found in one [..]
      short s_title = 0;
      short s_artist = 0;
      short s_album = 0;
      short s_performer = 0;
      short s_year = 0;
      short s_comment = 0;
      short s_tracknumber = 0;

      cur_pos++;

      end_paranthesis = strchr(cur_pos,']');
      if (!end_paranthesis)
      {
        ambiguous = SPLT_TRUE;
      }
      else
      {
        if ((*(end_paranthesis+1) != '[') &&
            (*(end_paranthesis+1) != '%') &&
            (*(end_paranthesis+1) != '\0'))
        {
          ambiguous = SPLT_TRUE;
        }
      }

      int track = -1;

      char *tag = NULL;
      int original_tags = SPLT_FALSE;
      while ((tag = strchr(cur_pos-1,'@')))
      {
        if (tag >= end_paranthesis)
        {
          break;
        }
        else
        {
          cur_pos = tag+1;
        }

        const char *old_pos = cur_pos;
        if (*(cur_pos-1) == '@')
        {
          switch (*cur_pos)
          {
            case 'o':
              //if we have twice @o
              if (original_tags)
              {
                ambiguous = SPLT_TRUE;
              }
              if ((*(cur_pos+1) != ',') &&
                  (*(cur_pos+1) != ']'))
              {
                ambiguous = SPLT_TRUE;
              }

              if (! splt_io_input_is_stdin(state))
              {
                int err = SPLT_OK;
                splt_o_lock_messages(state);
                splt_check_file_type(state, &err);
                if (err < 0)
                {
                  *error = err;
                  splt_o_unlock_messages(state);
                  get_out_from_while = SPLT_TRUE;
                  goto end_while;
                }
                splt_o_unlock_messages(state);

                splt_o_lock_messages(state);
                splt_p_init(state, &err);
                if (err >= 0)
                {
                  splt_tu_get_original_tags(state, &err);
                  if (err < 0) 
                  {
                    *error = err;
                    splt_o_unlock_messages(state);
                    get_out_from_while = SPLT_TRUE;
                    goto end_while;
                  }
                  splt_p_end(state, &err);
                  err = splt_tu_append_original_tags(state);
                  if (err < 0)
                  {
                    *error = err;
                    splt_o_unlock_messages(state);
                    get_out_from_while = SPLT_TRUE;
                    goto end_while;
                  }

                  if (all_tags)
                  {
                    splt_tags last_tags = splt_tu_get_last_tags(state);
                    if (all_title) { free(all_title); all_title = NULL; }
                    if (all_artist) { free(all_artist); all_artist = NULL; }
                    if (all_album) { free(all_album); all_album = NULL; }
                    if (all_performer) { free(all_performer); all_performer = NULL; }
                    if (all_year) { free(all_year); all_year = NULL; }
                    if (all_comment) { free(all_comment); all_comment = NULL; }
                    if (last_tags.title != NULL)
                    {
                      all_title = strdup(last_tags.title);
                    }
                    if (last_tags.artist != NULL)
                    {
                      all_artist = strdup(last_tags.artist);
                    }
                    if (last_tags.album != NULL)
                    {
                      all_album = strdup(last_tags.album);
                    }
                    if (last_tags.performer != NULL)
                    {
                      all_performer = strdup(last_tags.performer);
                    }
                    if (last_tags.year != NULL)
                    {
                      all_year = strdup(last_tags.year);
                    }
                    if (last_tags.comment != NULL)
                    {
                      all_comment = strdup(last_tags.comment);
                    }
                    all_genre = last_tags.genre;
                    all_tracknumber = last_tags.track;
                  }
                  original_tags = SPLT_TRUE;
                  splt_o_unlock_messages(state);
                }
                else
                {
                  *error = err;
                  splt_o_unlock_messages(state);
                  get_out_from_while = SPLT_TRUE;
                  goto end_while;
                }
              }

              //if we have a @a,@p,.. before @n
              //then ambiguous
              if ((artist != NULL) || (performer != NULL) ||
                  (album != NULL) || (title != NULL) ||
                  (comment != NULL) || (year != NULL) ||
                  (tracknumber != NULL))
              {
                ambiguous = SPLT_TRUE;
              }
              break;
            case 'a':
              //TODO: memory leak
              artist = splt_tp_parse_tag_word(cur_pos,end_paranthesis,&ambiguous, error);
              if (*error < 0) { get_out_from_while = SPLT_TRUE; goto end_while; }
              if (artist != NULL)
              {
                if (all_tags)
                {
                  if (all_artist) { free(all_artist); all_artist = NULL; }
                  all_artist = strdup(artist);
                }
                cur_pos += strlen(artist)+2;
                s_artist++;
              }
              else
              {
                cur_pos++;
              }
              break;
            case 'p':
              performer = splt_tp_parse_tag_word(cur_pos,end_paranthesis,&ambiguous, error);
              if (*error < 0) { get_out_from_while = SPLT_TRUE; goto end_while; }
              if (performer != NULL)
              {
                if (all_tags)
                {
                  if (all_performer) { free(all_performer); all_performer = NULL; }
                  all_performer = strdup(performer);
                }
                cur_pos += strlen(performer)+2;
                s_performer++;
              }
              else
              {
                cur_pos++;
              }
              break;
            case 'b':
              album = splt_tp_parse_tag_word(cur_pos,end_paranthesis,&ambiguous, error);
              if (*error < 0) { get_out_from_while = SPLT_TRUE; goto end_while; }
              if (album != NULL)
              {
                if (all_tags)
                {
                  if (all_album) { free(all_album); all_album = NULL; }
                  all_album = strdup(album);
                }
                cur_pos += strlen(album)+2;
                s_album++;
              }
              else
              {
                cur_pos++;
              }
              break;
            case 't':
              title = splt_tp_parse_tag_word(cur_pos,end_paranthesis,&ambiguous, error);
              if (*error < 0) { get_out_from_while = SPLT_TRUE; goto end_while; }
              if (title != NULL)
              {
                if (all_tags)
                {
                  if (all_title) { free(all_title); all_title = NULL; }
                  all_title = strdup(title);
                }

                cur_pos += strlen(title)+2;
                s_title++;
              }
              else
              {
                cur_pos++;
              }
              break;
            case 'c':
              comment = splt_tp_parse_tag_word(cur_pos,end_paranthesis,&ambiguous, error);
              if (*error < 0) { get_out_from_while = SPLT_TRUE; goto end_while; }
              if (comment != NULL)
              {
                if (all_tags)
                {
                  if (all_comment) { free(all_comment); all_comment = NULL; }
                  all_comment = strdup(comment);
                }

                cur_pos += strlen(comment)+2;
                s_comment++;
              }
              else
              {
                cur_pos++;
              }
              break;
            case 'y':
              year = splt_tp_parse_tag_word(cur_pos,end_paranthesis,&ambiguous, error);
              if (*error < 0) { get_out_from_while = SPLT_TRUE; goto end_while; }
              if (year != NULL)
              {
                if (all_tags)
                {
                  if (all_year) { free(all_year); all_year = NULL; }
                  all_year = strdup(year);
                }

                cur_pos += strlen(year)+2;
                s_year++;
              }
              else
              {
                cur_pos++;
              }
              break;
            case 'n':
              tracknumber = splt_tp_parse_tag_word(cur_pos,end_paranthesis,&ambiguous, error);
              if (*error < 0) { get_out_from_while = SPLT_TRUE; goto end_while; }
              if (tracknumber != NULL)
              {
                cur_pos += strlen(tracknumber)+2;
                s_tracknumber++;
              }
              else
              {
                cur_pos++;
              }
              if (auto_increment_tracknumber)
              {
                first_time_auto_increment_tracknumber = SPLT_TRUE;
              }
              break;
            case 'N':
              tracknumber = splt_tp_parse_tag_word(cur_pos,end_paranthesis, &ambiguous, error);
              if (*error < 0) { get_out_from_while = SPLT_TRUE; goto end_while; }
              splt_o_set_int_option(state, SPLT_OPT_AUTO_INCREMENT_TRACKNUMBER_TAGS,
                  SPLT_TRUE);
              if (auto_increment_tracknumber)
              {
                first_time_auto_increment_tracknumber = SPLT_TRUE;
              }
              else if (all_tags)
              {
                auto_increment_tracknumber = SPLT_TRUE;
                first_time_auto_increment_tracknumber = SPLT_TRUE;
              }
              break;
            default:
              ambiguous = SPLT_TRUE;
              break;
          }
        }

        if (cur_pos <= old_pos)
        {
          cur_pos++;
        }
      }

      //we check that we really have the tracknumber as integer
      if (tracknumber)
      {
        int is_number = SPLT_TRUE;
        int i = 0;
        for (i = 0;i < strlen(tracknumber);i++)
        {
          if (!isdigit(tracknumber[i]))
          {
            is_number = SPLT_FALSE;
            ambiguous = SPLT_TRUE;
          }
        }
        if (is_number)
        {
          track = atoi(tracknumber);

          if (all_tags || auto_increment_tracknumber)
          {
            all_tracknumber = track;
          }
        }
      }

      //
      if (auto_increment_tracknumber)
      {
        if (first_time_auto_increment_tracknumber)
        {
          auto_incremented_tracknumber = all_tracknumber;
          first_time_auto_increment_tracknumber = SPLT_FALSE;
        }
        track = auto_incremented_tracknumber++;
      }

      if ((s_title > 1) || (s_artist > 1)
          || (s_album > 1) || (s_performer > 1)
          || (s_year > 1) || (s_comment > 1)
          || (s_tracknumber > 1))
      {
        ambiguous = SPLT_TRUE;
      }

      if (!original_tags)
      {
        if (track == -1)
        {
          track = 0;
        }

        int err = splt_tu_append_tags(state, title, artist,
            album, performer, year, comment, track, genre);
        if (err < 0)
        {
          *error = err;
          get_out_from_while = SPLT_TRUE;
        }
      }
      else
      {
        int err = splt_tu_append_only_non_null_previous_tags(state, title, artist,
            album, performer, year, comment, track, genre);
        if (err < 0)
        {
          *error = err;
          get_out_from_while = SPLT_TRUE;
        }
      }

      if (we_had_all_tags && !original_tags)
      {
        int index = state->split.real_tagsnumber - 1;
        if (!title)
        {
          splt_tu_set_tags_field(state, index, SPLT_TAGS_TITLE, all_title);
        }
        if (!artist)
        {
          splt_tu_set_tags_field(state, index, SPLT_TAGS_ARTIST, all_artist);
        }
        if (!album)
        {
          splt_tu_set_tags_field(state, index, SPLT_TAGS_ALBUM, all_album);
        }
        if (!performer)
        {
          splt_tu_set_tags_field(state, index, SPLT_TAGS_PERFORMER, all_performer);
        }
        if (!year)
        {
          splt_tu_set_tags_field(state, index, SPLT_TAGS_YEAR, all_year);
        }
        if (!comment)
        {
          splt_tu_set_tags_field(state, index, SPLT_TAGS_COMMENT, all_comment);
        }
        if (!tracknumber && ! auto_incremented_tracknumber)
        {
          splt_tu_set_tags_field(state, index, SPLT_TAGS_TRACK, &all_tracknumber);
        }
        if (genre == 12)
        {
          splt_tu_set_tags_field(state, index, SPLT_TAGS_GENRE, &all_genre);
        }
      }

end_while:
      if (title) { free(title); title = NULL; }
      if (artist) { free(artist); artist = NULL; }
      if (album) { free(album); album = NULL; }
      if (performer) { free(performer); performer = NULL; }
      if (year) { free(year); year = NULL; }
      if (comment) { free(comment); comment = NULL; }
      if (tracknumber) { free(tracknumber); tracknumber = NULL; }

      if (get_out_from_while)
      {
        break;
      }

      if (all_tags)
      {
        we_had_all_tags = SPLT_TRUE;
        all_tags = SPLT_FALSE;
        splt_tu_set_like_x_tags_field(state, SPLT_TAGS_TITLE, all_title);
        splt_tu_set_like_x_tags_field(state, SPLT_TAGS_ARTIST, all_artist);
        splt_tu_set_like_x_tags_field(state, SPLT_TAGS_ALBUM, all_album);
        splt_tu_set_like_x_tags_field(state, SPLT_TAGS_PERFORMER, all_performer);
        splt_tu_set_like_x_tags_field(state, SPLT_TAGS_YEAR, all_year);
        splt_tu_set_like_x_tags_field(state, SPLT_TAGS_COMMENT, all_comment);
        splt_tu_set_like_x_tags_field(state, SPLT_TAGS_TRACK, &all_tracknumber);
        splt_tu_set_like_x_tags_field(state, SPLT_TAGS_GENRE, &all_genre);
      }

      tags_appended++;
    }

    if (all_title) { free(all_title); all_title = NULL; }
    if (all_artist) { free(all_artist); all_artist = NULL; }
    if (all_album) { free(all_album); all_album = NULL; }
    if (all_performer) { free(all_performer); all_performer = NULL; }
    if (all_year) { free(all_year); all_year = NULL; }
    if (all_comment) { free(all_comment); all_comment = NULL; }

    return ambiguous;
  }

  return SPLT_FALSE;
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

  cur_pos = word_end;

  return word;
}


