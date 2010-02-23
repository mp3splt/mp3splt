/**********************************************************
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <assert.h>
#include <stdarg.h>

#include <ctype.h>

#include "splt.h"

static int splt_su_append_one(char **str, const char *to_append,
    size_t to_append_size);
static void splt_su_clean_string_(splt_state *state, char *s, int *error,
    int ignore_dirchar);
static int splt_u_is_illegal_char(char c, int ignore_dirchar);

char *splt_su_replace_all(const char *str, char *to_replace,
    char *replacement, int *error)
{
  if (str == NULL)
  {
    return NULL;
  }

  if (to_replace == NULL || replacement == NULL)
  {
    return splt_su_safe_strdup(str, NULL);
  }

  char *new_str = NULL;

  int err = SPLT_OK;

  const char *ptr = str;
  const char *prev_ptr = ptr;
  while ((ptr = strstr(ptr, to_replace)) != NULL)
  {
    err = splt_su_append(&new_str, prev_ptr, ptr - prev_ptr,
        replacement, strlen(replacement), NULL);
    if (err != SPLT_OK) { goto error; }
    ptr += strlen(to_replace);
    prev_ptr = ptr;
  }

  if (prev_ptr != NULL)
  {
    err = splt_su_append(&new_str, prev_ptr, (str + strlen(str)) - prev_ptr, NULL);
    if (err != SPLT_OK) { goto error; }
  }
 
  return new_str;

error:
  if (new_str)
  {
    free(new_str);
  }
  *error = err;

  return NULL;
}

char *splt_su_safe_strdup(const char *input, int *error)
{
  if (input == NULL)
  {
    return NULL;
  }

  char *dup_input = strdup(input);
  if (dup_input != NULL)
  {
    return dup_input;
  }
  else
  {
    if (error)
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    }
    return NULL;
  }
}

int splt_su_append_str(char **str, const char *to_append, ...)
{
  int err = SPLT_OK;
  va_list ap;

  va_start(ap, to_append);

  while (to_append)
  {
    size_t to_append_size = strlen(to_append);
    err = splt_su_append(str, to_append, to_append_size, NULL);
    if (err < 0) { break; }
    to_append = va_arg(ap, const char *);
  }

  va_end(ap);

  return err;
}

int splt_su_append(char **str, const char *to_append, ...)
{
  int err = SPLT_OK;
  va_list ap;

  va_start(ap, to_append);

  while (to_append)
  {
    size_t to_append_size = va_arg(ap, size_t);
    err = splt_su_append_one(str, to_append, to_append_size);
    if (err < 0) { break; }
    to_append = va_arg(ap, const char *);
  }

  va_end(ap);

  return err;
}

void splt_su_free_replace(char **str, char *replacement)
{
  if (!str)
  {
    return;
  }

  if (*str)
  {
    free(*str);
  }

  *str = replacement;
}

int splt_su_copy(const char *src, char **dest)
{
  int err = SPLT_OK;

  if (!dest)
  {
    return err;
  }

  if (*dest)
  {
    free(*dest);
    *dest = NULL;
  }

  if (src == NULL)
  {
    *dest = NULL;
  }
  else
  {
    int length = strlen(src)+1;
    if ((*dest = malloc(sizeof(char)*length)) == NULL)
    {
      err = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    }
    else
    {
      snprintf(*dest, length+1,"%s", src);
    }
  }

  return err;
}

static int splt_su_append_one(char **str, const char *to_append, size_t to_append_size)
{
  if (str == NULL || to_append == NULL || to_append_size == 0)
  {
    return SPLT_OK;
  }

  size_t new_size = 0;

  if (*str == NULL)
  {
    new_size = to_append_size + 1;
    *str = malloc(new_size);
    if (*str == NULL)
    {
      return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    }

    *str[0] = '\0';
  }
  else
  {
    new_size = to_append_size + strlen(*str) + 1;
    *str = realloc(*str, new_size);
    if (*str == NULL)
    {
      return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    }
  }

  strncat(*str, to_append, to_append_size);

  return SPLT_OK;
}

void splt_su_clean_string(splt_state *state, char *s, int *error)
{
  splt_su_clean_string_(state, s, error, SPLT_FALSE);
}

char *splt_su_cut_spaces_from_begin(char *c)
{
  if (*c == ' ')
  {
    while (*c == ' ')
    {
      c++;
    }
  }

  return c;
}

char *splt_su_cut_spaces_from_the_end(char *c)
{
  while (*c == ' ')
  {
    *c = '\0';
    c--;
  }

  return c;
}

const char *splt_su_get_fname_without_path(const char *filename)
{
  char *c = NULL;
  while ((c = strchr(filename, SPLT_DIRCHAR)) !=NULL)
  {
    filename = c + 1;
  }

  return filename;
}

char *splt_su_get_fname_with_path_and_extension(splt_state *state, int *error)
{
  char *output_fname_with_path = NULL;
  char *new_filename_path = splt_t_get_new_filename_path(state);
  int current_split = splt_t_get_current_split(state);
  char *output_fname = splt_sp_get_splitpoint_name(state, current_split, error);
  int malloc_number = strlen(new_filename_path) + 10;
  if (output_fname)
  {
    malloc_number += strlen(output_fname);
  }

  //if we don't output to stdout
  if (output_fname && (strcmp(output_fname,"-") != 0))
  {
    if ((output_fname_with_path = malloc(malloc_number)) != NULL)
    {
      //we put the full output filename (with the path)
      //construct full filename with path
      const char *extension = splt_p_get_extension(state, error);
      if (*error >= 0)
      {
        if (new_filename_path[0] == '\0')
        {
          snprintf(output_fname_with_path, malloc_number,
              "%s%s", output_fname, extension);
        }
        else
        {
          snprintf(output_fname_with_path, malloc_number,
              "%s%c%s%s",new_filename_path, SPLT_DIRCHAR,
              output_fname, extension);
        }
      }
      else
      {
        if (output_fname_with_path)
        {
          free(output_fname_with_path);
          output_fname_with_path = NULL;
        }
        return NULL;
      }
    }
    else
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      return NULL;
    }

    char *filename = splt_t_get_filename_to_split(state);
    if (splt_io_check_if_file(state, output_fname_with_path))
    {
      //if input and output are the same file
      if (splt_check_is_the_same_file(state,filename, output_fname_with_path, error))
      {
        splt_e_set_error_data(state,filename);
        *error = SPLT_ERROR_INPUT_OUTPUT_SAME_FILE;
      }
      else
      {
        //if no error from the check_is_the_same..
        if (*error >= 0)
        {
          //TODO
          //warning if a file already exists
        }
      }
    }

    return output_fname_with_path;
  }
  else
  {
    char *returned_result = NULL;
    if (output_fname)
    {
      returned_result = strdup(output_fname);
    }
    else
    {
      returned_result = strdup("-");
    }
    if (returned_result)
    {
      return returned_result;
    }
    else
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      return NULL;
    }
  }
}

void splt_su_cut_extension(char *str)
{
  char *point = strrchr(str , '.');
  if (point)
  {
    *point = '\0';
  }
}

char *splt_su_str_to_upper(const char *str, int *error)
{
  int i = 0;

  char *result = strdup(str);
  if (result == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return NULL;
  }

  for (i = 0;i < strlen(str);i++)
  {
    result[i] = toupper(str[i]);
  }

  return result;
}

char *splt_su_get_file_with_output_path(splt_state *state,
    char *filename, int *error)
{
  char *new_fname = NULL;
  if (filename != NULL)
  {
    //we don't care about the path; we clean the string
    splt_su_clean_string(state, filename, error);
    if (error < 0) { return NULL; }

    char *path_of_split = splt_t_get_path_of_split(state);
    int malloc_number = strlen(filename) + 2;
    if (path_of_split)
    {
      malloc_number += strlen(path_of_split);
    }
    //allocate memory for the m3u file
    if ((new_fname = malloc(malloc_number)) != NULL)
    {
      if (path_of_split)
      {
        if (path_of_split[strlen(path_of_split)] == SPLT_DIRCHAR)
        {
          snprintf(new_fname, malloc_number, "%s%s",
              path_of_split, filename);
        }
        else
        {
          snprintf(new_fname, malloc_number, "%s%c%s",
              path_of_split, SPLT_DIRCHAR, filename);
        }
      }
      else
      {
        snprintf(new_fname, malloc_number, "%s", filename);
      }
    }
    else
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    }
  }

  return new_fname;
}

int splt_su_str_ends_with(const char *str1, const char *str2)
{
  if (!str1 || !str2)
  {
    return SPLT_FALSE;
  }

  int str1_end_index = strlen(str1) - 1;
  int str2_end_index = strlen(str2) - 1;

  while (str1_end_index >= 0 && str2_end_index >= 0)
  {
    if (str1[str1_end_index] != str2[str2_end_index])
    {
      return SPLT_FALSE;
    }

    str1_end_index--;
    str2_end_index--;
  }

  return SPLT_TRUE;
}

static int splt_u_is_illegal_char(char c, int ignore_dirchar)
{
  if ((ignore_dirchar) && (c == SPLT_DIRCHAR))
  {
    return SPLT_FALSE;
  }

  //for the sake of filename portability, we take the the windows illegal
  //characters (will be changed upon feature request)
  if ((c == '\\') || (c == '/') || (c == ':') || (c == '*') ||
      (c == '?') || (c == '"') || (c == '<') ||
      (c == '>') || (c == '|') || (c == '\r'))
  {
    return SPLT_TRUE;
  }

  return SPLT_FALSE;
}

static void splt_su_clean_string_(splt_state *state, char *s, int *error, int ignore_dirchar)
{
  int i = 0, j=0;
  char *copy = NULL;
  if (s)
  {
    copy = strdup(s);
    if (copy)
    {
      for (i=0; i<=strlen(copy); i++)
      {
        if (! splt_u_is_illegal_char(copy[i], ignore_dirchar))
        {
          s[j++] = copy[i];
        }
        else
        {
          s[j++] = '_';
        }
      }
      free(copy);
      copy = NULL;

      // Trim string. I will never stop to be surprised about cddb strings dirtiness! ;-)
      for (i=strlen(s)-1; i >= 0; i--) 
      {
        if (s[i]==' ')
        {
          s[i] = '\0';
        }
        else 
        {
          break;
        }
      }
    }
    else
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    }
  }
}

