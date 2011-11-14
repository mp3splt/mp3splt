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

#ifndef SPLT_ERRORS_H

void splt_e_set_errors_default_values(splt_state *state);
void splt_e_free_errors(splt_state *state);

void splt_e_set_error_data(splt_state *state, const char *error_data);
void splt_e_set_error_data_from_splitpoints(splt_state *state, long point1, long point2);
void splt_e_set_error_data_from_splitpoint(splt_state *state, long splitpoint);

void splt_e_set_strerror_msg(splt_state *state);
void splt_e_set_strherror_msg(splt_state *state);
void splt_e_clean_strerror_msg(splt_state *state);

void splt_e_set_strerr_msg_with_data(splt_state *state, const char *message, const char *data);
void splt_e_set_strerror_msg_with_data(splt_state *state, const char *data);

void splt_e_error(int error_type, const char *function, int arg_int, const char *arg_char);

char *splt_e_strerror(splt_state *state, splt_code error_code);

#define SPLT_ERRORS_H

#endif

