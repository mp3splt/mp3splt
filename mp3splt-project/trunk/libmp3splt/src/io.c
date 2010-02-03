/**********************************************************
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2009 Alexandru Munteanu - io_fx@yahoo.fr
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307,
 * USA.
 *********************************************************/

#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#include "splt.h"

static int splt_io_file_type_is(const char *fname, int file_type)
{
  mode_t st_mode;
  int status = splt_u_stat(fname, &st_mode, NULL);
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
static char *splt_io_get_linked_fname(const char *fname)
{
  //TODO: wild guess of the max size
  size_t linked_fname_size = strlen(fname) + 2048;
  char *linked_fname = malloc(sizeof(char) * linked_fname_size);
  if (linked_fname == NULL)
  {
    return NULL;
  }

  memset(linked_fname, '\0', sizeof(char) * linked_fname_size);

  ssize_t real_link_size = readlink(fname, linked_fname, linked_fname_size);
  if (real_link_size == -1)
  {
    free(linked_fname);
    return NULL;
  }

  linked_fname[real_link_size] = '\0';
  return linked_fname;
}

char *splt_io_get_linked_fname_with_path(const char *fname)
{
  //TODO: loop until the last linked
  char *linked_fname = splt_io_get_linked_fname(fname);
  if (!linked_fname)
  {
    return NULL;
  }

  if (linked_fname[0] == SPLT_DIRCHAR)
  {
    return linked_fname;
  }

  char *slash_ptr = strrchr(fname, SPLT_DIRCHAR);
  if (slash_ptr == NULL)
  {
    return linked_fname;
  }

  size_t path_size = slash_ptr - fname + 1;
  size_t linked_fname_size = strlen(linked_fname);

  char *linked_fname_with_path = NULL;
  size_t allocated_size = 0;

  splt_su_append(&linked_fname_with_path, &allocated_size,
      fname, path_size);
  splt_su_append(&linked_fname_with_path, &allocated_size,
      linked_fname, linked_fname_size);

  free(linked_fname);
  linked_fname = NULL;

  return linked_fname_with_path;
}

static int splt_io_linked_file_type_is(const char *fname, int file_type)
{
  int linked_file_is_of_type = SPLT_FALSE;

  char *linked_fname = splt_io_get_linked_fname_with_path(fname);
  if (linked_fname)
  {
    if (splt_io_file_type_is(linked_fname, file_type))
    {
      linked_file_is_of_type = SPLT_TRUE;
    }

    free(linked_fname);
    linked_fname = NULL;
  }
  else
  {
    //TODO: error
  }

  return linked_file_is_of_type;
}
#endif

int splt_io_check_if_directory(const char *fname)
{
  if (fname != NULL)
  {
    if (splt_io_file_type_is(fname, S_IFDIR))
    {
      return SPLT_TRUE;
    }

#ifndef __WIN32__
    if (splt_io_linked_file_type_is(fname, S_IFDIR))
    {
      return SPLT_TRUE;
    }
#endif
  }

  return SPLT_FALSE;
}

int splt_io_check_if_file(splt_state *state, const char *fname)
{
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
    if (splt_io_linked_file_type_is(fname, S_IFREG))
    {
      return SPLT_TRUE;
    }
#endif
  }

  //TODO: review ?
  splt_t_set_strerror_msg(state);
  splt_t_set_error_data(state, fname);

  return SPLT_FALSE;
}

