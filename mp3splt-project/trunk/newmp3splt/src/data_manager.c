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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "data_manager.h"
#include "utils.h"

#include "windows_utils.h"

main_data *create_main_struct(int argc, char **orig_argv)
{
  main_data *data = NULL;
  data = my_malloc(sizeof(main_data));

  data->state = NULL;
  data->opt = new_options();
  data->sl = my_malloc(sizeof(silence_level));

  data->filenames = NULL;
  data->number_of_filenames = 0;
  data->splitpoints = NULL;
  data->number_of_splitpoints = 0;

  data->sl->level_sum = 0;
  data->sl->number_of_levels = 0;
  data->sl->print_silence_level = SPLT_TRUE;

  data->argc = argc;
#ifdef __WIN32__
  data->argv = NULL;
  data->argv = win32_get_utf8_args(data);
#else
  data->argv = orig_argv;
#endif

  data->printed_value_length = 0;

  return data;
}

void free_main_struct(main_data **d)
{
  if (d)
  {
    main_data *data = *d;
    if (data)
    {
      //try to stop the split
      mp3splt_stop_split(data->state);
      //free options
      free_options(&data->opt);

      //free silence level
      if (data->sl)
      {
        free(data->sl);
        data->sl = NULL;
      }

      //free filenames & splitpoints
      if (data->filenames)
      {
        int i = 0;
        for (i = 0; i < data->number_of_filenames;i++)
        {
          free(data->filenames[i]);
          data->filenames[i] = NULL;
        }
        free(data->filenames);
        data->filenames = NULL;
      }
      if (data->splitpoints)
      {
        free(data->splitpoints);
        data->splitpoints = NULL;
      }

#ifdef __WIN32__
      //free argv
      if (data->argv)
      {
        int i = 0;
        for (i = 0; i < data->argc;i++)
        {
          if (data->argv[i])
          {
            free(data->argv[i]);
            data->argv[i] = NULL;
          }
        }

        free(data->argv);
        data->argv = NULL;
      }
#endif 

      mp3splt_free_state(data->state);
      data->state = NULL;

      free(data);
      data = NULL;
    }
  }
}

void append_filename(main_data *data, const char *str)
{
  if (data)
  {
    if (!data->filenames)
    {
      data->filenames = my_malloc(sizeof(char *));
    }
    else
    {
      data->filenames = my_realloc(data->filenames, sizeof(char *) *
          (data->number_of_filenames + 1));
    }
    data->filenames[data->number_of_filenames] = NULL;
    if (str != NULL)
    {
      int malloc_size = strlen(str) + 1;
      data->filenames[data->number_of_filenames] = my_malloc(sizeof(char) * malloc_size);
      snprintf(data->filenames[data->number_of_filenames],malloc_size, "%s",str);
      data->number_of_filenames++;
    }
  }
}

void append_splitpoint(main_data *data, long value)
{
  if (data)
  {
    if (!data->splitpoints)
    {
      data->splitpoints = my_malloc(sizeof(long));
    }
    else
    {
      data->splitpoints = my_realloc(data->splitpoints,
          sizeof(long) * (data->number_of_splitpoints + 1));
    }
    data->splitpoints[data->number_of_splitpoints] = value;
    data->number_of_splitpoints++;
  }
}

