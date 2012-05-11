/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2012 Alexandru Munteanu - io_fx@yahoo.fr
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

#ifndef MP3SPLT_IO_H

int splt_io_check_if_directory(const char *fname);
int splt_io_check_if_file(splt_state *state, const char *fname);

int splt_io_input_is_stdin(splt_state *state);
int splt_io_input_is_stdout(splt_state *state);

#ifndef __WIN32__
#define MAX_SYMLINKS 200
char *splt_io_get_linked_fname(const char *fname, int *number_of_symlinks);
#endif

int splt_io_get_word (FILE *in, off_t offset, int mode, unsigned long *headw);
off_t splt_io_get_file_length(splt_state *state, FILE *in, const char *filename, int *error);

void splt_io_create_output_dirs_if_necessary(splt_state *state,
    const char *output_filename, int *error);

int splt_io_create_directories(splt_state *state, const char *dir);

int splt_io_stat(const char *path, mode_t *st_mode, off_t *st_size);
FILE *splt_io_fopen(const char *filename, const char *mode);
int splt_io_mkdir(splt_state *state, const char *path);
unsigned char *splt_io_fread(FILE *file, int start, size_t size);
size_t splt_io_fwrite(splt_state *state, const void *ptr,
    size_t size, size_t nmemb, FILE *stream);

void splt_io_find_filenames(splt_state *state, const char *directory,
    char ***found_files, int *number_of_found_files, int *error);

char *splt_io_readline(FILE *stream, int *error);

#define MP3SPLT_IO_H

#endif

