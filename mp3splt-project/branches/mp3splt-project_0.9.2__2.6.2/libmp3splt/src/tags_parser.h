/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2014 Alexandru Munteanu - m@ioalex.net
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

#ifndef SPLT_TAGS_PARSER_H

typedef struct {
  splt_tags *current_tags;
  splt_tags *all_tags;
  int ambigous;
  int tags_counter;
  int set_all_tags;

  int title_counter;
  int artist_counter;
  int album_counter;
  int performer_counter;
  int year_counter;
  int comment_counter;
  int tracknumber_counter;
  int genre_counter;

  int original_tags_found;
  int original_tags_value;

  int auto_increment_tracknumber;

  int we_had_all_tags;

  const char *position;

  char *current_tracknumber;
} tags_parser_utils;

int splt_tp_put_tags_from_string(splt_state *state, const char *tags, int *error);
void splt_tp_put_tags_from_filename(splt_state *state, int *error);

#define SPLT_TAGS_PARSER_H

#endif

