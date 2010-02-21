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

#include "string_utils.h"
#include "mp3splt.h"

static int splt_su_append_one(char **str, const char *to_append,
    size_t to_append_size);

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

/*int main()
{
  char *a = NULL;

  splt_su_append(&a, "this is a test", 6, "another one", 4, "FFFF", 2, NULL);

  free(a);
  a = NULL;

  splt_su_append_str(&a, "BISthis is a test", "another one", "FFFF", NULL);


  free(a);

  return 0;
}*/

