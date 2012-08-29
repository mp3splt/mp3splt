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

  Utilities that are needed when we want to talk to win32.
 */
#ifdef __WIN32__

#include "splt.h"

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

  if (namelist)
  {
    *namelist = files;
  }
  else
  {
    free_memory = 1;
  }

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

int wscandir(const char *dir, struct _wdirent ***namelist,
		int(*filter)(const struct _wdirent *),
		int(*compar)(const struct _wdirent **, const struct _wdirent **))
{
  struct _wdirent **files = NULL;
  struct _wdirent *file = NULL;
  _WDIR *directory = NULL;
  int number_of_files = 0;

  wchar_t *wdir = splt_w32_utf8_to_utf16(dir);
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

  if (namelist)
  {
    *namelist = files;
  }
  else
  {
    free_memory = 1;
  }

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
  char *name_a = splt_w32_utf16_to_utf8((*a)->d_name);
  char *name_b = splt_w32_utf16_to_utf8((*b)->d_name);

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

static wchar_t *splt_w32_encoding_to_utf16(UINT encoding, const char *source)
{
  wchar_t *dest = NULL;

  int converted_size = MultiByteToWideChar(encoding, 0, source, -1, NULL, 0);
  if (converted_size > 0)
  {
    dest = malloc(sizeof(wchar_t) * converted_size);
    if (dest)
    {
      MultiByteToWideChar(encoding, 0, source, -1, dest, converted_size);
    }
  }

  return dest;
}

wchar_t *splt_w32_utf8_to_utf16(const char *source)
{
  return splt_w32_encoding_to_utf16(CP_UTF8, source);
}


static char *splt_w32_utf16_to_encoding(UINT encoding, const wchar_t *source)
{
  char *dest = NULL;

  int converted_size = WideCharToMultiByte(encoding, 0, source, -1, NULL, 0, NULL, NULL);
  if (converted_size > 0)
  {
    dest = malloc(sizeof(char *) * converted_size);
    if (dest)
    {
      WideCharToMultiByte(encoding, 0, source, -1, dest, converted_size, NULL, NULL);
    }
  }

  return dest;
}

char *splt_w32_utf16_to_utf8(const wchar_t *source)
{
  return splt_w32_utf16_to_encoding(CP_UTF8, source);
}

int splt_w32_check_if_encoding_is_utf8(const char *source)
{
  int is_utf8 = SPLT_FALSE;

  if (source)
  {
    wchar_t *source_wchar = splt_w32_utf8_to_utf16(source);
    if (source_wchar)
    {
      char *source2 = splt_w32_utf16_to_utf8(source_wchar);
      if (source2)
      {
        if (strcmp(source, source2) == 0)
        {
          is_utf8 = SPLT_TRUE;
        }

        free(source2);
        source2 = NULL;
      }

      free(source_wchar);
      source_wchar = NULL;
    }
  }

  return is_utf8;
}

int splt_w32_str_starts_with_drive_root_directory(const char *str)
{
  if (strlen(str) > 2 &&
      str[1] == ':' &&
      str[2] == SPLT_DIRCHAR)
  {
    return SPLT_TRUE;
  }

  return SPLT_FALSE;
}

int splt_w32_str_is_drive_root_directory(const char *str)
{
  return strlen(str) == 3 &&
    splt_w32_str_starts_with_drive_root_directory(str);
}

#endif

