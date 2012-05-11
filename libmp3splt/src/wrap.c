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

/*! \file

Automatically split mp3 files created with mp3wrap into the original 
tracks
*/
#include <string.h>

#include "splt.h"

//! \todo document this function.
void splt_w_set_wrap_default_values(splt_state *state)
{
  splt_wrap *wrap = state->wrap;
  wrap->wrap_files = NULL;
  wrap->wrap_files_num = 0;
}

//! \todo document this function.
int splt_w_wrap_put_file(splt_state *state, int wrapfiles, int index,
    const char *filename)
{
  splt_wrap *wrap = state->wrap;

  if (index == 0)
  {
    if ((wrap->wrap_files = malloc(wrapfiles * sizeof(char*))) == NULL)
    {
      return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    }
    memset(wrap->wrap_files, 0x0, sizeof(char *) * wrapfiles);

    wrap->wrap_files_num = 0;
  }

  int err = splt_su_copy(filename, &wrap->wrap_files[index]);
  if (err < 0) { return err; }

  wrap->wrap_files_num++;

  return SPLT_OK;
}

//! \todo document this function.
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

//! \todo document this function.
void splt_w_wrap_free(splt_state *state)
{
  splt_wrap *wrap = state->wrap;
  if (!wrap)
  {
    return;
  }
  splt_w_free_files(wrap->wrap_files, wrap->wrap_files_num);
  wrap->wrap_files_num = 0;
}

