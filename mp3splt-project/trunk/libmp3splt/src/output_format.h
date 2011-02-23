/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2011 Munteanu Alexandru - io_fx@yahoo.fr
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

#ifndef SPLT_OUTPUT_FORMAT_H

int splt_of_set_default_values(splt_state *state);
void splt_of_free_oformat(splt_state *state);

void splt_of_set_oformat_digits_tracks(splt_state *state, int tracks);
void splt_of_set_oformat_digits(splt_state *state);
void splt_of_set_oformat(splt_state *state, const char *format_string,
    int *error, int ignore_incorrect_format_warning);
int splt_of_reparse_oformat(splt_state *state);

int splt_of_get_oformat_number_of_digits_as_int(splt_state *state);
char splt_of_get_oformat_number_of_digits_as_char(splt_state *state);
const char *splt_of_get_oformat(splt_state *state);

#define SPLT_OUTPUT_FORMAT_H

#endif

