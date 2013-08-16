/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2013 Alexandru Munteanu - m@ioalex.net
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

#ifndef SPLT_PAIR_H

typedef struct {
  int first;
  int second;
} splt_int_pair;

splt_int_pair *splt_int_pair_new(int first, int second);
void splt_int_pair_free(splt_int_pair **pair);
int splt_int_pair_first(splt_int_pair *pair);
int splt_int_pair_second(splt_int_pair *pair);

typedef struct {
  int first;
  long second;
} splt_il_pair;

splt_il_pair *splt_il_pair_new(int first, long second);
void splt_il_pair_free(splt_il_pair **pair);
int splt_il_pair_first(splt_il_pair *pair);
long splt_il_pair_second(splt_il_pair *pair);

#define SPLT_PAIR_H

#endif

