/**********************************************************
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2013 Alexandru Munteanu - m@ioalex.net
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 *********************************************************/

/*! \file

This file contains all I/O functions this library uses.
*/
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>

#ifdef __WIN32__
#include <shlwapi.h>
#endif

#include "splt.h"

static int splt_u_fname_is_directory_parent(char *fname, int fname_size);

int splt_io_input_is_stdin(splt_state *state)
{
  char *filename = splt_t_get_filename_to_split(state);

  if (filename && filename[0] != '\0')
  {
    if ((strcmp(filename,"-") == 0) ||
        (filename[strlen(filename)-1] == '-'))
    {
      return SPLT_TRUE;
    }
  }

  return SPLT_FALSE;
}

int splt_io_input_is_stdout(splt_state *state)
{
  const char *oformat = splt_of_get_oformat(state);

  if (oformat && oformat[0] != '\0')
  {
    if ((strcmp(oformat,"-") == 0))
    {
      return SPLT_TRUE;
    }
    else
    {
      return SPLT_FALSE;
    }
  }

  return SPLT_FALSE;
}

static int splt_io_file_type_is(const char *fname, int file_type)
{
  mode_t st_mode;
  int status = splt_io_stat(fname, &st_mode, NULL);
  if (status == 0)
  {
    if ((st_mode & S_IFMT) == file_type)
    {
      return SPLT_TRUE;
    }
  }

  return SPLT_FALSE;
}

#ifndef __WIN32__
static char *splt_io_readlink(const char *fname)
{
  int bufsize = 1024;

  while (bufsize < INT_MAX)
  {
    char *linked_fname = malloc(sizeof(char) * bufsize);
    if (linked_fname == NULL)
    {
      return NULL;
    }

    ssize_t real_link_size = readlink(fname, linked_fname, bufsize);
    if (real_link_size == -1)
    {
      free(linked_fname);
      return NULL;
    }

    if (real_link_size < bufsize)
    {
      linked_fname[real_link_size] = '\0';
      return linked_fname;
    }

    free(linked_fname);
    bufsize += 1024;
  }

  return NULL;
}

static char *splt_io_get_linked_fname_one_level(const char *fname, int *number_of_symlinks)
{
  char *previous_linked_fname = NULL;

  char *linked_fname = splt_io_readlink(fname);
  if (!linked_fname)
  {
    return NULL;
  }

  int count = 0;
  while (linked_fname != NULL)
  {
    if (previous_linked_fname)
    {
      free(previous_linked_fname);
    }
    previous_linked_fname = linked_fname;
    linked_fname = splt_io_readlink(linked_fname);

    count++;
    if (count > MAX_SYMLINKS)
    {
      if (previous_linked_fname)
      {
        free(previous_linked_fname);
        previous_linked_fname = NULL;
      }
      if (linked_fname)
      {
        free(linked_fname);
      }
      if (number_of_symlinks)
      {
        *number_of_symlinks = MAX_SYMLINKS;
      }
      return NULL;
    }
  }

  if (number_of_symlinks)
  {
    *number_of_symlinks = count;
  }

  linked_fname = previous_linked_fname;

  if (linked_fname[0] == SPLT_DIRCHAR)
  {
    return linked_fname;
  }

  char *slash_ptr = strrchr(fname, SPLT_DIRCHAR);
  if (slash_ptr == NULL)
  {
    return linked_fname;
  }

  char *linked_fname_with_path = NULL;

  size_t path_size = slash_ptr - fname + 1;
  int err = splt_su_append(&linked_fname_with_path, fname, path_size, NULL);
  if (err != SPLT_OK)
  {
    free(linked_fname);
    return NULL;
  }

  err = splt_su_append_str(&linked_fname_with_path, linked_fname, NULL);
  if (err != SPLT_OK)
  {
    free(linked_fname);
    free(linked_fname_with_path);
    return NULL;
  }

  free(linked_fname);
  linked_fname = NULL;

  return linked_fname_with_path;
}

char *splt_io_get_linked_fname(const char *fname, int *number_of_symlinks)
{
  int num_of_symlinks = 0;

  char *output_fname = splt_io_get_linked_fname_one_level(fname, number_of_symlinks);
  while (output_fname != NULL && splt_io_file_type_is(output_fname, S_IFLNK))
  {
    char *new_output_fname = splt_io_get_linked_fname_one_level(output_fname, &num_of_symlinks);
    free(output_fname);
    output_fname = new_output_fname;

    if (num_of_symlinks == MAX_SYMLINKS)
    {
      break;
    }
  }

  return output_fname;
}

static int splt_io_linked_file_type_is(const char *fname, int file_type)
{
  int linked_file_is_of_type = SPLT_FALSE;

  int number_of_symlinks = 0;
  char *linked_fname = splt_io_get_linked_fname(fname, &number_of_symlinks);
  if (linked_fname)
  {
    if (splt_io_file_type_is(linked_fname, file_type))
    {
      linked_file_is_of_type = SPLT_TRUE;
    }

    free(linked_fname);
    linked_fname = NULL;
  }

  if (number_of_symlinks == MAX_SYMLINKS)
  {
    errno = ELOOP;
  }

  return linked_file_is_of_type;
}
#endif

int splt_io_check_if_directory(const char *fname)
{
#ifdef __WIN32__
  int is_file = splt_io_check_if_file(NULL, fname);
  if (is_file)
  {
    return SPLT_FALSE;
  }

  if (splt_w32_check_if_encoding_is_utf8(fname))
  {
    wchar_t *wpath = splt_w32_utf8_to_utf16(fname);
    if (PathFileExistsW(wpath) == 1)
    {
      free(wpath);
      return SPLT_TRUE;
    }

    free(wpath);
    return SPLT_FALSE;
  }
  else
  {
    if (PathFileExistsA(fname) == 1)
    {
      return SPLT_TRUE;
    }

    return SPLT_FALSE;
  }
#endif

  if (fname != NULL)
  {
    if (splt_io_file_type_is(fname, S_IFDIR))
    {
      return SPLT_TRUE;
    }

#ifndef __WIN32__
    int is_link = splt_io_file_type_is(fname, S_IFLNK);
    if (is_link && splt_io_linked_file_type_is(fname, S_IFDIR))
    {
      return SPLT_TRUE;
    }
#endif
  }

  return SPLT_FALSE;
}

int splt_io_check_if_file(splt_state *state, const char *fname)
{
  errno = 0;

  if (fname != NULL)
  {
    //stdin: consider as file
    if (fname[0] != '\0' && fname[strlen(fname)-1] == '-')
    {
      return SPLT_TRUE;
    }

    if (splt_io_file_type_is(fname, S_IFREG))
    {
      return SPLT_TRUE;
    }

#ifndef __WIN32__
    int is_link = splt_io_file_type_is(fname, S_IFLNK);
    if (is_link && splt_io_linked_file_type_is(fname, S_IFREG))
    {
      return SPLT_TRUE;
    }
#endif
  }

  if (state != NULL)
  {
    splt_e_set_strerror_msg_with_data(state, fname);
  }

  return SPLT_FALSE;
}

int splt_io_get_word(FILE *in, off_t offset, int mode, unsigned long *headw)
{
  int i;
  *headw = 0;

  if (fseeko(in, offset, mode)==-1)
  {
    return -1;
  }

  for (i=0; i<4; i++)
  {
    if (feof(in)) 
    {
      return -1;
    }
    *headw = *headw << 8;
    *headw |= fgetc(in);
  }

  return 0;
}

off_t splt_io_get_file_length(splt_state *state, FILE *in, const char *filename, int *error)
{
  struct stat info;
  if (fstat(fileno(in), &info)==-1)
  {
    splt_e_set_strerror_msg_with_data(state, filename);
    *error = SPLT_ERROR_CANNOT_OPEN_FILE;
    return -1;
  }
  return info.st_size;
}

void splt_io_create_output_dirs_if_necessary(splt_state *state,
    const char *output_filename, int *error)
{
  if (splt_o_get_int_option(state, SPLT_OPT_CREATE_DIRS_FROM_FILENAMES))
  {
    char *only_dirs = strdup(output_filename);
    if (! only_dirs)
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      return;
    }

    char *dir_char = strrchr(only_dirs, SPLT_DIRCHAR);
    if (dir_char != NULL)
    {
      *dir_char = '\0';
#ifdef __WIN32__
      if (strlen(only_dirs) == 2 && only_dirs[1] == ':' &&
          strlen(output_filename) > 3)
      {
        *dir_char = SPLT_DIRCHAR;
        *(dir_char+1) = '\0';
      }
#endif
      int err = splt_io_create_directories(state, only_dirs);
      if (err < 0) { *error = err; }
    }

    free(only_dirs);
    only_dirs = NULL;
  }
}

int splt_io_create_directories(splt_state *state, const char *dir)
{
  int result = SPLT_OK;

  if (dir == NULL || dir[0] == '\0')
  {
    return result;
  }

  char *dir_to_create = malloc(sizeof(char) * (strlen(dir)+100));
  if (!dir_to_create)
  {
    return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  }
 
  splt_d_print_debug(state, "Starting to create directories for _%s_ ...\n", dir);
 
  const char *ptr = dir;

#ifdef __WIN32__
  int first_time = SPLT_TRUE;
#endif

  while ((ptr = strchr(ptr, SPLT_DIRCHAR)) != NULL)
  {
    //handle C:DIRCHAR on windows
#ifdef __WIN32__
    if (first_time && splt_w32_str_starts_with_drive_root_directory(dir))
    {
      ptr++;
      first_time = SPLT_FALSE;
      continue;
    }
    first_time = SPLT_FALSE;
#endif

    strncpy(dir_to_create, dir, ptr-dir);
    dir_to_create[ptr-dir] = '\0';

    if (dir_to_create[0] != '\0')
    {
      splt_d_print_debug(state,"Checking if _%s_ is a directory ...\n", dir_to_create);

      if (! splt_io_check_if_directory(dir_to_create))
      {
        splt_d_print_debug(state,"Creating directory _%s_ ...\n", dir_to_create);

        if ((splt_io_mkdir(state, dir_to_create)) == -1)
        {
          splt_e_set_strerror_msg_with_data(state, dir_to_create);
          result = SPLT_ERROR_CANNOT_CREATE_DIRECTORY;
          goto end;
        }
      }
    }

    ptr++;
  }

  strncpy(dir_to_create, dir, strlen(dir) + 1);

  if (! splt_io_check_if_directory(dir_to_create))
  {
    splt_d_print_debug(state,"Creating final directory _%s_ ...\n", dir_to_create);

    if ((splt_io_mkdir(state, dir_to_create)) == -1)
    {
      splt_e_set_strerror_msg_with_data(state, dir_to_create);
      result = SPLT_ERROR_CANNOT_CREATE_DIRECTORY;
    }
  }

end:
  if (dir_to_create)
  {
    free(dir_to_create);
    dir_to_create = NULL;
  }
  
  return result;
}

FILE *splt_io_fopen(const char *filename, const char *mode)
{
#ifdef __WIN32__
  if (splt_w32_check_if_encoding_is_utf8(filename))
  {
    wchar_t *wfilename = splt_w32_utf8_to_utf16(filename);
    wchar_t *wmode = splt_w32_utf8_to_utf16(mode);

    FILE *file = _wfopen(wfilename, wmode);

    if (wfilename)
    {
      free(wfilename);
      wfilename = NULL;
    }

    if (wmode)
    {
      free(wmode);
      wmode = NULL;
    }

    return file;
  }
  else
#endif
  {
    return fopen(filename, mode);
  }
}

int splt_io_mkdir(splt_state *state, const char *path)
{
  if (splt_o_get_int_option(state, SPLT_OPT_PRETEND_TO_SPLIT))
  {
    return 0;
  }

#ifdef __WIN32__
  if (splt_w32_check_if_encoding_is_utf8(path))
  {
    wchar_t *wpath = splt_w32_utf8_to_utf16(path);

    int ret = _wmkdir(wpath);

    if (wpath)
    {
      free(wpath);
      wpath = NULL;
    }

    return ret;
  }
  else
  {
    return mkdir(path);
  }
#else
  return mkdir(path, 0755);
#endif
}

int splt_io_stat(const char *path, mode_t *st_mode, off_t *st_size)
{
#ifdef __WIN32__
  if (splt_w32_check_if_encoding_is_utf8(path))
  {
    struct _stat buf;
    wchar_t *wpath = splt_w32_utf8_to_utf16(path);

    int ret = _wstat(wpath, &buf);

    if (wpath)
    {
      free(wpath);
      wpath = NULL;
    }

    if (st_mode != NULL)
    {
      *st_mode = buf.st_mode;
    }

    if (st_size != NULL)
    {
      *st_size = buf.st_size;
    }

    return ret;
  }
  else
#endif
  {
    struct stat buf;

#ifdef __WIN32__
    int ret = stat(path, &buf);
#else
    int ret = lstat(path, &buf);
#endif

    if (st_mode != NULL)
    {
      *st_mode = buf.st_mode;
    }

    if (st_size != NULL)
    {
      *st_size = buf.st_size;
    }

    return ret;
  }
}

void splt_io_find_filenames(splt_state *state, const char *directory,
    char ***found_files, int *number_of_found_files, int *error)
{
#ifdef __WIN32__
  struct _wdirent **files = NULL;
#else
  struct dirent **files = NULL;
#endif

  //TODO: handle scandir error
#ifdef __WIN32__
  int num_of_files = wscandir(directory, &files, NULL, walphasort);
#else
  int num_of_files = scandir(directory, &files, NULL, alphasort);
#endif

  int new_number_of_files = num_of_files;

  if (files == NULL) { return; }

  while (new_number_of_files-- > 0)
  {
#ifdef __WIN32__
    char *fname = splt_w32_utf16_to_utf8(files[new_number_of_files]->d_name);
#else
    char *fname = files[new_number_of_files]->d_name;
#endif

    int fname_size = strlen(fname);

    if (*error >= 0)
    {
      int path_with_fname_size = fname_size + strlen(directory) + 2;
      char *path_with_fname = malloc(sizeof(char) * path_with_fname_size);
      if (path_with_fname == NULL)
      {
        *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
        free(files[new_number_of_files]);
        files[new_number_of_files] = NULL;
        continue;
      }

      snprintf(path_with_fname, path_with_fname_size, "%s%c%s", directory,
          SPLT_DIRCHAR, fname);

      if (splt_io_check_if_file(state, path_with_fname))
      {
        if (splt_p_file_is_supported_by_plugins(state, fname))
        {
          if (!(*found_files))
          {
            (*found_files) = malloc(sizeof(char *));
          }
          else
          {
            (*found_files) = realloc((*found_files),
                sizeof(char *) * ((*number_of_found_files) + 1));
          }
          if (*found_files == NULL)
          {
            *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
            goto end;
          }

          int fname_size = strlen(path_with_fname) + 1;
          (*found_files)[(*number_of_found_files)] = malloc(sizeof(char) * fname_size);
          if ((*found_files)[(*number_of_found_files)] == NULL)
          {
            *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
            goto end;
          }

          memset((*found_files)[(*number_of_found_files)], '\0', fname_size);
          strncat((*found_files)[*(number_of_found_files)], path_with_fname, fname_size);
          (*number_of_found_files)++;
        }
      }
      else if (splt_io_check_if_directory(path_with_fname))
      {
        if (! splt_u_fname_is_directory_parent(fname, fname_size))
        {
          splt_io_find_filenames(state, path_with_fname, found_files,
              number_of_found_files, error);
        }
      }

end:
      if (path_with_fname)
      {
        free(path_with_fname);
        path_with_fname = NULL;
      }
    }

    free(files[new_number_of_files]);
    files[new_number_of_files] = NULL;
  }

  if (files)
  {
    free(files);
    files = NULL;
  }
}

size_t splt_io_fwrite(splt_state *state, const void *ptr, size_t size, size_t nmemb, FILE *stream)
{
  if (splt_o_get_int_option(state, SPLT_OPT_PRETEND_TO_SPLIT))
  {
    if (state->split.write_cb != NULL)
    {
      state->split.write_cb(ptr, size, nmemb, state->split.write_cb_data);
    }
    return size * nmemb;
  }
  else
  {
    return fwrite(ptr, size, nmemb, stream);
  }
}

char *splt_io_readline(FILE *stream, int *error)
{
  if (feof(stream))
  {
    return NULL;
  }

  int err = SPLT_OK;
  int bufsize = 1024;
  char *buffer = malloc(sizeof(char) * bufsize);
  buffer[0] = '\0';

  char *line = NULL;
  while (fgets(buffer, bufsize, stream) != NULL)
  {
    err = splt_su_append_str(&line, buffer, NULL);
    if (err < 0) { *error = err; break; }

    if (line != NULL && line[strlen(line)-1] == '\n')
    {
      free(buffer);
      return line;
    }

    buffer[0] = '\0';  
  }

  free(buffer);

  if (*error < 0)
  {
    free(line);
    return NULL;
  }

  return line;
}

unsigned char *splt_io_fread(FILE *file, int start, size_t size)
{
  unsigned char *bytes = malloc(sizeof(unsigned char) * size);

  if (! bytes)
  {
    return NULL;
  }

  size_t bytes_read = fread(bytes, 1, size, file);

  if (bytes_read != size)
  {
    if (bytes)
    {
      free(bytes);
      bytes = NULL;
    }

    return NULL;
  }

  return bytes;
}

static int splt_u_fname_is_directory_parent(char *fname, int fname_size)
{
  return ((fname_size == 1) && (strcmp(fname, ".") == 0)) ||
    ((fname_size == 2) && (strcmp(fname, "..") == 0));
}

