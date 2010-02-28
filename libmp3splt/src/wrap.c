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

#include <string.h>

#include "splt.h"

static void splt_w_free_files(char **files, int number);

void splt_w_set_wrap_default_values(splt_state *state)
{
  splt_wrap *wrap = state->wrap;
  wrap->wrap_files = NULL;
  wrap->wrap_files_num = 0;
}

int splt_w_wrap_put_file(splt_state *state, int wrapfiles, int index,
    const char *filename)
{
  int error = SPLT_OK;

  splt_wrap *wrap = state->wrap;

  if (index == 0)
  {
    if ((wrap->wrap_files = malloc(wrapfiles * sizeof(char*))) == NULL)
    {
      return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    }

    wrap->wrap_files_num = 0;
  }

  if (filename == NULL)
  {
    wrap->wrap_files[index] = NULL;
  }
  else if ((wrap->wrap_files[index] = strdup(filename)) == NULL)
  {
    return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  }

  wrap->wrap_files_num++;

  return error;
}

void splt_w_wrap_free(splt_state *state)
{
  splt_wrap *wrap = state->wrap;
  splt_w_free_files(wrap->wrap_files, wrap->wrap_files_num);
  wrap->wrap_files_num = 0;
}

static void splt_w_free_files(char **files, int number)
{
  if (files != NULL)
  {
    if (number != 0)
    {
      int i = 0;
      for (i = 0; i < number; i++)
      {
        if (files[i])
        {
          free(files[i]);
          files[i] = NULL;
        }
      }
    }

    free(files);
    files = NULL;
  }
}

