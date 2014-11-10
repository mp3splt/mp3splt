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

#ifndef SPLT_OPTIONS_H

void splt_o_set_options_default_values(splt_state *state);

void splt_o_set_option(splt_state *state, int option_name, const void *data);
void splt_o_set_int_option(splt_state *state, int option_name, int value);
void splt_o_set_long_option(splt_state *state, int option_name, long value);
void splt_o_set_float_option(splt_state *state, int option_name, float value);

int splt_o_get_int_option(splt_state *state, int option_name);
long splt_o_get_long_option(splt_state *state, int option_name);
float splt_o_get_float_option(splt_state *state, int option_name);

void splt_o_set_ioptions_default_values(splt_state *state);
void splt_o_set_default_iopts(splt_state *state);
void splt_o_set_iopt(splt_state *state, int type, int value);
int splt_o_get_iopt(splt_state *state, int type);
void splt_o_iopts_free(splt_state *state);

int splt_o_library_locked(splt_state *state);
void splt_o_lock_library(splt_state *state);
void splt_o_unlock_library(splt_state *state);

int splt_o_messages_locked(splt_state *state);
void splt_o_lock_messages(splt_state *state);
void splt_o_unlock_messages(splt_state *state);

#define SPLT_OPTIONS_H

#endif

