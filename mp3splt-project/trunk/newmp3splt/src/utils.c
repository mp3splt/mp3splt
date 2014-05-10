/*
 * Mp3Splt -- Utility for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2014 Alexandru Munteanu - <m@ioalex.net>
 *
 * http://mp3splt.sourceforge.net
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <stdlib.h>

#include "common.h"
#include "print_utils.h"

extern FILE *console_out;

void *my_malloc(size_t size)
{
  void *allocated = malloc(size);
  if (! allocated)
  {
    print_error(_("cannot allocate memory !"));
    exit(1);
  }
  else
  {
    return allocated;
  }

  return NULL;
}

void *my_realloc(void *ptr, size_t size)
{
  void *allocated = realloc(ptr, size);
  if (! allocated)
  {
    print_error(_("cannot allocate memory !"));
    exit(1);
  }
  else
  {
    return allocated;
  }
  
  return NULL;
}

//returns the converted string s in hundredth of seconds
//returns -1 if it cannot convert
long c_hundreths(const char *s)
{
  long minutes=0, seconds=0, hundredths=0, i;
  long hun = -1;

  if (!s)
  {
    return -LONG_MAX;
  }

  if (strcmp(s,"EOF") == 0)
  {
    return LONG_MAX;
  }

  short negative = SPLT_FALSE;
  const char *scan_start = s;
  if (strlen(s) > 4 && strncmp(s, "EOF-", 4) == 0)
  {
    negative = SPLT_TRUE;
    scan_start = s+4;
  }

  for(i=0; i<strlen(scan_start); i++)
  {
    if ((scan_start[i]<0x30 || scan_start[i] > 0x39) && (scan_start[i]!='.'))
    {
      return -LONG_MAX;
    }
  }

  if (sscanf(scan_start, "%ld.%ld.%ld", &minutes, &seconds, &hundredths) < 2)
  {
    return -LONG_MAX;
  }

  if ((seconds > 59) || (hundredths > 99))
  {
    return -LONG_MAX;
  }

  if (scan_start[strlen(scan_start)-2] == '.')
  {
    hundredths *= 10;
  }

  hun = hundredths;
  hun += (minutes*60 + seconds) * 100;

  if (negative)
  {
    return -hun;
  }

  return hun;
}

void show_files_and_ask_for_confirmation(main_data *data)
{
  int j = 0;

  char junk[18] = { '\0' };
  print_message(_("List of found files:\n"));

  for (j = 0;j < data->number_of_filenames; j++)
  {
    fprintf(console_out, "  %s\n", data->filenames[j]);

    if (((j+1) % 22 == 0) && (j+1 < data->number_of_filenames))
    {
      fprintf(console_out, _("\n-- 'Enter' for more, 's' to split, 'c' to cancel:"));
      fflush(console_out);

      fgets(junk, 16, stdin);

      if (junk[1] == '\n')
      {
        if (junk[0] == 'c')
        {
          print_message_exit(_("\n split aborted."), data);
        }
        if (junk[0] == 's')
        {
          goto split;
        }
      }

      print_message("\n");
    }
  }

  int answer_is_correct = SPLT_FALSE;
  do {
    fprintf(console_out, _("\n-- 's' to split, 'c' to cancel:"));
    fflush(console_out);

    fgets(junk, 16, stdin);

    if (junk[1] == '\n')
    {
      if (junk[0] == 'c')
      {
        print_message_exit(_("\n split aborted."), data);
      }
      if (junk[0] == 's')
      {
        answer_is_correct = SPLT_TRUE;
      }
    }

  } while (!answer_is_correct);

  fprintf(console_out, "\n");
  fflush(console_out);

split:
  ;
}

void get_silence_level(long time, float level, void *user_data)
{
  silence_level *sl = user_data;
  if (level == INT_MIN)
  {
    sl->print_silence_level = SPLT_FALSE;
  }
  else if (level == INT_MAX)
  {
    sl->print_silence_level = SPLT_TRUE;
  }
  else
  {
    sl->level_sum += level;
    sl->number_of_levels++;
  }
}

