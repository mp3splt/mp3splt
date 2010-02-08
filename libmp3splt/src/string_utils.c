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

#include "splt.h"

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
  size_t new_str_size = 0;

  int err = SPLT_OK;

  const char *ptr = str;
  const char *prev_ptr = ptr;
  while ((ptr = strstr(ptr, to_replace)) != NULL)
  {
    err = splt_su_append(&new_str, &new_str_size, prev_ptr, ptr - prev_ptr);
    if (err != SPLT_OK) { goto error; }
    err = splt_su_append(&new_str, &new_str_size, replacement, strlen(replacement));
    if (err != SPLT_OK) { goto error; }
    ptr += strlen(to_replace);
    prev_ptr = ptr;
  }

  if (prev_ptr != NULL)
  {
    err = splt_su_append(&new_str, &new_str_size, prev_ptr, (str + strlen(str)) - prev_ptr);
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

int splt_su_append(char **str, size_t *allocated_size,
    const char *to_append, size_t to_append_size)
{
  if (str == NULL || to_append == NULL || to_append_size == 0)
  {
    return SPLT_OK;
  }

  size_t new_allocated_size = *allocated_size;

  if (*str == NULL || *allocated_size == 0)
  {
    *str = malloc(to_append_size + 1);
    if (*str == NULL)
    {
      return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    }

    *str[0] = '\0';
    new_allocated_size = to_append_size + 1;
  }
  else
  {
    *str = realloc(*str, to_append_size + *allocated_size);
    if (*str == NULL)
    {
      return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    }

    new_allocated_size = to_append_size + *allocated_size;
  }

  *allocated_size = new_allocated_size;

  strncat(*str, to_append, to_append_size);

  return SPLT_OK;
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

