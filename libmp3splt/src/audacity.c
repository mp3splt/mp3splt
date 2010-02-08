/*
 * audacity.c -- Audacity label file parser portion of the Mp3Splt utility
 *                    Utility for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2004 M. Trotta - <matteo.trotta@lib.unimib.it>
 * Copyright (c) 2007 Federico Grau - <donfede@casagrau.org>
 * Copyright (c) 2010 Alexandru Munteanu <io_fx@yahoo.fr>
 *
 * http://mp3splt.sourceforge.net
 * http://audacity.sourceforge.net/
 */

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
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "splt.h"

static void append_splitpoints(splt_state *state, long start_point, long end_point, char *label_name, 
    long last_start_point, long last_end_point, const char *last_label_name,
    int *append_start_point)
{
  /*
  // FIXME: what/why is this...
  state->splitpoints[tracks] = floorf(label_start);
  label_start = (label_start - state->splitpoints[tracks])*4/3;
  state->splitpoints[tracks] += label_start;
   */

  if (last_start_point != -1 && last_end_point != -1)
  {
    if (*append_start_point)
    {
      splt_t_append_splitpoint(state, last_start_point, last_label_name, SPLT_SPLITPOINT);
    }

    if (start_point == last_end_point)
    {
      splt_t_append_splitpoint(state, last_end_point, label_name, SPLT_SPLITPOINT);
      *append_start_point = SPLT_FALSE;
    }
    else
    {
      splt_t_append_splitpoint(state, last_end_point, "skip", SPLT_SKIPPOINT);
      *append_start_point = SPLT_TRUE;
    }
  }
}

//returns the number of tracks found
int splt_audacity_put_splitpoints(const char *file, splt_state *state, int *error)
{
  if (file == NULL)
  {
    *error = SPLT_INVALID_AUDACITY_FILE;
    return 0;
  }

  splt_t_free_splitpoints_tags(state);

  *error = SPLT_AUDACITY_OK;

  char *client_infos = malloc(sizeof(char) * (strlen(file)+200));
  if (client_infos == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return 0;
  }
  snprintf(client_infos, strlen(file) + 200,
      _(" reading informations from audacity labels file '%s' ...\n"),file);
  splt_t_put_info_message_to_client(state, client_infos);
  free(client_infos);
  client_infos = NULL;

	FILE *file_input = NULL;
	char line[2048] = { '\0' };
	double label_start;
	double label_end;
	char *label_name = NULL;
  char *last_label_name = NULL;
	int i = 0;
	int tracks = -1;
	char *ptr = NULL;;
  long start_point = -1;
  long end_point = -1;
  long last_start_point = -1;
  long last_end_point = -1;
  int append_start_point = SPLT_TRUE;

	if (!(file_input = splt_u_fopen(file, "r")))
  {
    splt_t_set_strerror_msg(state);
    splt_t_set_error_data(state,file);
    *error = SPLT_ERROR_CANNOT_OPEN_FILE;
    return tracks;
  }

	if (fseek(file_input, 0, SEEK_SET) != 0)
  {
    splt_t_set_strerror_msg(state);
    splt_t_set_error_data(state,file);
    *error = SPLT_ERROR_SEEKING_FILE;
    goto end;
  }

  while (fgets(line, 2048, file_input) != NULL)
  {
		ptr = line;

		errno = 0;
		label_start = strtod(ptr, &ptr);
		if (errno != 0)
    {
      *error = SPLT_INVALID_AUDACITY_FILE;
      goto end;
    }
		errno = 0;
		label_end = strtod(ptr, &ptr);
		if (errno != 0)
    {
      *error = SPLT_INVALID_AUDACITY_FILE;
      goto end;
    }

		//trim off whitespace before the name.
		while (isspace(*ptr))
    {
			ptr++;
		}

		//trim off whitespace after the name.
		i = strlen(ptr) - 1;
		while (isspace(ptr[i]))
    {
			ptr[i] = '\0';
			i--;
		}

    if (label_name)
    {
      free(label_name);
      label_name = NULL;
    }
    label_name = strdup(ptr);
    if (!label_name)
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      goto end;
    }

    start_point = (long) (floorf(label_start) * 100.0);
    end_point = (long) (floorf(label_end) * 100.0);

    append_splitpoints(state, start_point, end_point, label_name, last_start_point,
        last_end_point, last_label_name, &append_start_point);

    last_end_point = end_point;
    last_start_point = start_point;
    if (last_label_name)
    {
      free(last_label_name);
      last_label_name = NULL;
    }
    last_label_name = strdup(label_name);
    if (!last_label_name)
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      goto end;
    }

    if (label_name)
    {
      free(label_name);
      label_name = NULL;
    }

		tracks++;
	}

  append_splitpoints(state, start_point, end_point, label_name,
      last_start_point, last_end_point, last_label_name, &append_start_point);

end:
  if (label_name)
  {
    free(label_name);
    label_name = NULL;
  }
  if (last_label_name)
  {
    free(last_label_name);
    last_label_name = NULL;
  }
  if (fclose(file_input) != 0)
  {
    splt_t_set_strerror_msg(state);
    splt_t_set_error_data(state, file);
    *error = SPLT_ERROR_CANNOT_CLOSE_FILE;
  }
  file_input = NULL;

	return tracks;
}

