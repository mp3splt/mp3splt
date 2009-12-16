/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2009 Alexandru Munteanu - io_fx@yahoo.fr
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

void splt_su_append(char **str, size_t *allocated_size,
    const char *to_append, size_t to_append_size)
{
  if (str == NULL || to_append == NULL || to_append_size == 0)
  {
    return;
  }

  size_t new_allocated_size = *allocated_size;

  if (*str == NULL || *allocated_size == 0)
  {
    *str = malloc(to_append_size + 1);
    *str[0] = '\0';
    new_allocated_size = to_append_size + 1;
  }
  else
  {
    *str = realloc(*str, to_append_size + *allocated_size);
    new_allocated_size = to_append_size + *allocated_size;
  }

  *allocated_size = new_allocated_size;

  strncat(*str, to_append, to_append_size);
}

/*int main()
{
  char *test = NULL;
  size_t test_size = 0;

  splt_su_append(&test, &test_size, "abcd", 5);
  splt_su_append(&test, &test_size, "efg", 4);
  splt_su_append(&test, &test_size, "hijklm", 7);

  assert(strcmp(test, "abcdefghijklm") == 0);

  if (test) 
  {
    free(test);
    test = NULL;
  }

  return EXIT_SUCCESS;
}*/

