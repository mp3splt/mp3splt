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

#ifdef __WIN32__

#include "splt.h"

#include "win32.h"

#include <errno.h>
#include <dirent.h>

#include <windows.h>
#include <direct.h>

//-returns -1 for not enough memory, -2 for other errors
//-a positive (or 0) number if success
int scandir(const char *dir, struct dirent ***namelist,
		int(*filter)(const struct dirent *),
		int(*compar)(const struct dirent **, const struct dirent **))
{
  struct dirent **files = NULL;
  struct dirent *file = NULL;
  DIR *directory = NULL;
  int number_of_files = 0;

  directory = opendir(dir);
  if (directory == NULL)
  {
    return -2;
  }

  int free_memory = 0;
  int we_have_error = 0;

  while ((file = readdir(directory)))
  {
    if ((filter == NULL) || (filter(file)))
    {
      if (files == NULL)
      {
        files = malloc((sizeof *files));
      }
      else
      {
        files = realloc(files, (sizeof *files) * (number_of_files + 1));
      }
      if (files == NULL)
      {
        free_memory = 1;
        we_have_error = 1;
        break;
      }

      files[number_of_files] = malloc(sizeof(struct dirent));
      if (files[number_of_files] == NULL)
      {
        free_memory = 1;
        we_have_error = 1;
        break;
      }

      *files[number_of_files] = *file;
      number_of_files++;
    }
  }

  //we should have a valid 'namelist' argument
  if (namelist)
  {
    *namelist = files;
  }
  else
  {
    free_memory = 1;
  }

  //-free memory if error
  if (free_memory)
  {
    while (number_of_files--)
    {
      if (files[number_of_files])
      {
        free(files[number_of_files]);
        files[number_of_files] = NULL;
      }
    }
    free(files);
    files = NULL;
  }

  if (closedir(directory) == -1)
  {
    return -2;
  }

  qsort(*namelist, number_of_files, sizeof **namelist,
      (int (*)(const void *, const void *)) compar);

  if (we_have_error)
  {
    errno = ENOMEM;
    return -1;
  }

  return number_of_files;
}

//-returns -1 for not enough memory, -2 for other errors
//-a positive (or 0) number if success
int wscandir(const char *dir, struct _wdirent ***namelist,
		int(*filter)(const struct _wdirent *),
		int(*compar)(const struct _wdirent **, const struct _wdirent **))
{
  struct _wdirent **files = NULL;
  struct _wdirent *file = NULL;
  _WDIR *directory = NULL;
  int number_of_files = 0;

  wchar_t *wdir = splt_u_win32_utf8_to_utf16(dir);
  directory = _wopendir(wdir);
  if (wdir) { free(wdir); wdir = NULL; }
  if (directory == NULL)
  {
    return -2;
  }

  int free_memory = 0;
  int we_have_error = 0;

  while ((file = _wreaddir(directory)))
  {
    if ((filter == NULL) || (filter(file)))
    {
      if (files == NULL)
      {
        files = malloc((sizeof *files));
      }
      else
      {
        files = realloc(files, (sizeof *files) * (number_of_files + 1));
      }
      if (files == NULL)
      {
        free_memory = 1;
        we_have_error = 1;
        break;
      }

      files[number_of_files] = malloc(sizeof(struct _wdirent));
      if (files[number_of_files] == NULL)
      {
        free_memory = 1;
        we_have_error = 1;
        break;
      }

      *files[number_of_files] = *file;
      number_of_files++;
    }
  }

  //we should have a valid 'namelist' argument
  if (namelist)
  {
    *namelist = files;
  }
  else
  {
    free_memory = 1;
  }

  //-free memory if error
  if (free_memory)
  {
    while (number_of_files--)
    {
      if (files[number_of_files])
      {
        free(files[number_of_files]);
        files[number_of_files] = NULL;
      }
    }
    free(files);
    files = NULL;
  }

  if (_wclosedir(directory) == -1)
  {
    return -2;
  }

  qsort(*namelist, number_of_files, sizeof **namelist,
      (int (*)(const void *, const void *)) compar);

  if (we_have_error)
  {
    errno = ENOMEM;
    return -1;
  }

  return number_of_files;
}

int alphasort(const struct dirent **a, const struct dirent **b)
{
  char *name_a = (char *)(*a)->d_name;
  char *name_b = (char *)(*b)->d_name;

  return strcoll(name_a, name_b);
}

int walphasort(const struct _wdirent **a, const struct _wdirent **b)
{
  char *name_a = splt_u_win32_utf16_to_utf8((*a)->d_name);
  char *name_b = splt_u_win32_utf16_to_utf8((*b)->d_name);

  int ret = strcoll(name_a, name_b);

  if (name_a)
  {
    free(name_a);
    name_a = NULL;
  }

  if (name_b)
  {
    free(name_b);
    name_b = NULL;
  }

  return ret;
}

#endif

