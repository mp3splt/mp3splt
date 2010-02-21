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
  state->wrap->wrap_files = NULL;
  state->wrap->wrap_files_num = 0;
}

int splt_w_wrap_put_file(splt_state *state, int wrapfiles, int index,
    const char *filename)
{
  int error = SPLT_OK;

  if (index == 0)
  {
    if ((state->wrap->wrap_files = malloc(wrapfiles * sizeof(char*))) == NULL)
    {
      error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    }
    else
    {
      state->wrap->wrap_files_num = 0;
    }
  }

  if (error == SPLT_OK)
  {
    if (filename == NULL)
    {
      state->wrap->wrap_files[index] = NULL;
      state->wrap->wrap_files_num++;
    }
    else
    {
      if ((state->wrap->wrap_files[index] = strdup(filename)) == NULL)
      {
        error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      }
      else
      {
        state->wrap->wrap_files_num++;
      }
    }
  }

  return error;
}

void splt_w_wrap_free(splt_state *state)
{
  splt_w_free_files(state->wrap->wrap_files, state->wrap->wrap_files_num);
  state->wrap->wrap_files_num = 0;
}

static void splt_w_free_files(char **files, int number)
{
  int i = 0;
  if (files != NULL)
  {
    if (number != 0)
    {
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


