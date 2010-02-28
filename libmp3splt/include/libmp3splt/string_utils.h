/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2010 Munteanu Alexandru - io_fx@yahoo.fr
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

#ifndef SPLT_STRING_UTILS_H

#include <stdarg.h>

char *splt_su_replace_all(const char *str, char *to_replace, char *replacement, int *error);
char *splt_su_safe_strdup(const char *input, int *error);
int splt_su_append(char **str, const char *to_append, ...);
int splt_su_append_str(char **str, const char *to_append, ...);
void splt_su_free_replace(char **str, char *replacement);
int splt_su_copy(const char *src, char **dest);

void splt_su_clean_string(splt_state *state, char *s, int *error);
char *splt_su_cut_spaces_from_the_end(char *c);
char *splt_su_cut_spaces_from_begin(char *c);

const char *splt_su_get_fname_without_path(const char *filename);
char *splt_su_get_fname_with_path_and_extension(splt_state *state, int *error);

void splt_su_cut_extension(char *str);
char *splt_su_str_to_upper(const char *str, int *error);

char *splt_su_get_file_with_output_path(splt_state *state,
    char *filename, int *error);

int splt_su_str_ends_with(const char *str1, const char *str2);

char *splt_su_format_messagev(splt_state *state, const char *message, va_list ap);

#define SPLT_STRING_UTILS_H

#endif


