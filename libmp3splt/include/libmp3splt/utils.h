/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2009 Munteanu Alexandru - io_fx@yahoo.fr
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

#include <dirent.h>

#ifndef MP3SPLT_UTILS_H

#ifdef __WIN32__
#include <windows.h>
#include <direct.h>
#endif

/****************************/
/* utils for conversion */

long splt_u_convert_hundreths (const char *s);
float splt_u_convert2dB(double input);
double splt_u_convertfromdB(float input);

/****************************/
/* utils for file infos */

int splt_u_getword (FILE *in, off_t offset, int mode,
    unsigned long *headw);
off_t splt_u_flength(splt_state *state, FILE *in, const char *filename, int *error);

/*****************************************************/
/* utils manipulating strings (including filenames) */


void splt_u_cleanstring(splt_state *state, char *s, int *error);
void splt_u_cleanstring_(splt_state *state, char *s, int *error, int ignore_dirchar);
char *splt_u_cut_spaces_at_the_end(char *c);
char *splt_u_cut_spaces_from_begin(char *c);
const char *splt_u_get_real_name(const char *filename);
void splt_u_set_complete_mins_secs_filename(splt_state *state, int *error);
char *splt_u_get_fname_with_path_and_extension(splt_state *state,
    int *error);

/****************************/
/* utils for splitpoints */

int splt_u_cut_splitpoint_extension(splt_state *state, int index);
void splt_u_order_splitpoints(splt_state *state, int len);
int splt_u_parse_ssplit_file(splt_state *state, FILE *log_file, int *error);

/****************************/
/* utils for the tags       */
int splt_u_put_tags_from_string(splt_state *state, const char *tags, int *error);

/*******************************/
/* utils for the output format */

int splt_u_parse_outformat(char *s, splt_state *state);
int splt_u_put_output_format_filename(splt_state *state);

/******************************/
/* utils for debug and errors */

void splt_u_error(int error_type, const char *function,
    int arg_int, char *arg_char);

/****************************/
/* utils miscellaneous */

float splt_u_silence_position(struct splt_ssplit *temp, float off);
void splt_u_print_debug(splt_state *state, const char *message,
    double optional, const char *optional2);
double splt_u_get_double_pos(long split);
void splt_u_get_mins_secs_hundr(long split_hundr, long *mins, long *secs, long *hundr);
int splt_u_create_directories(splt_state *state, const char *dir);
void splt_u_create_output_dirs_if_necessary(splt_state *state,
    const char *output_filename, int *error);
char *splt_u_strerror(splt_state *state, int error_code);

int splt_u_check_if_directory(char *fname);

int splt_u_stat(const char *path, mode_t *st_mode, off_t *st_size);
FILE *splt_u_fopen(const char *filename, const char *mode);
int splt_u_mkdir(const char *path);

#ifdef __WIN32__
char *splt_u_win32_utf16_to_utf8(const wchar_t *source);
wchar_t *splt_u_win32_utf8_to_utf16(const char *source);
#endif

char *splt_u_safe_strdup(char *input, int *error);

void splt_u_print_overlap_time(splt_state *state);
long splt_u_overlap_time(splt_state *state, int splitpoint_index);

int splt_u_file_is_supported_by_plugins(splt_state *state, const char *fname);

void splt_u_find_filenames(splt_state *state, const char *directory,
    char ***found_files, int *number_of_found_files, int *error);

char *splt_u_str_to_upper(const char *str, int *error);

char *splt_u_get_artist_or_performer_ptr(splt_state *state, int current_split);

short splt_u_fend_sec_is_bigger_than_total_time(splt_state *state,
    double fend_sec);

int splt_u_str_ends_with(const char *str1, const char *str2);

#define MP3SPLT_UTILS_H

#endif

