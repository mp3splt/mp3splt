/**********************************************************
 *
 * audacity.c -- Audacity label file parser portion of the Mp3Splt utility
 *                    Utility for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2004 M. Trotta - <matteo.trotta@lib.unimib.it>
 * Copyright (c) 2007 Federico Grau - <donfede@casagrau.org>
 * Copyright (c) 2010-2011 Alexandru Munteanu <io_fx@yahoo.fr>
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

/*!\file 

Input splitpoints from audacity.
*/

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "splt.h"


static long splt_audacity_get_begin(splt_audacity *sa)
{
  return sa->begin;
}

static long splt_audacity_get_end(splt_audacity *sa)
{
  return sa->end;
}

static const char *splt_audacity_get_name(splt_audacity *sa)
{
  return sa->name;
}

static int splt_audacity_append_splitpoints(splt_state *state,
    splt_audacity *previous_aud, splt_audacity *aud, int *append_begin_point)
{
  int err = SPLT_OK;

  long previous_begin_point = -1;
  long previous_end_point = -1;

  if (previous_aud)
  {
    previous_begin_point = splt_audacity_get_begin(previous_aud);
    previous_end_point = splt_audacity_get_end(previous_aud);
  }

  long start_point = splt_audacity_get_begin(aud);

  if (previous_begin_point != -1 && previous_end_point != -1)
  {
    if (*append_begin_point)
    {
      err = splt_sp_append_splitpoint(state, previous_begin_point, 
          splt_audacity_get_name(previous_aud), SPLT_SPLITPOINT);
      if (err < 0) { return err; }
    }

    if (start_point == previous_end_point)
    {
      err = splt_sp_append_splitpoint(state, previous_end_point, 
          splt_audacity_get_name(aud), SPLT_SPLITPOINT);
      *append_begin_point = SPLT_FALSE;
      if (err < 0) { return err; }
    }
    else
    {
      err = splt_sp_append_splitpoint(state, previous_end_point, "skip", SPLT_SKIPPOINT);
      *append_begin_point = SPLT_TRUE;
      if (err < 0) { return err; }
    }
  }

  return err;
}

static splt_audacity *splt_audacity_new()
{
  splt_audacity *sa = malloc(sizeof(splt_audacity));
  if (!sa)
  {
    return NULL;
  }

  sa->begin = -1;
  sa->end = -1;
  sa->name = NULL;

  return sa;
}

static void splt_audacity_free(splt_audacity **sa)
{
  if (sa)
  {
    if (*sa)
    {
      splt_audacity *s = *sa;

      if (s->name)
      {
        free(s->name);
        s->name = NULL;
      }

      free(*sa);
      *sa = NULL;
    }
  }
}

static int splt_audacity_set_name(splt_audacity *sa, const char *name)
{
  return splt_su_copy(name, &sa->name);
}

static long to_hundreths(char *str)
{
  long hun = 0;
  long seconds = 0, hundreths = 0;
  sscanf(str, "%ld.%4ld", &seconds, &hundreths);

  if (hundreths >= 100)
  {
    double hun_in_double = (double)hundreths;
    hundreths = round(hun_in_double / 100.0);
  }

  return seconds * 100 + hundreths;
}

static char *splt_audacity_set_begin(splt_audacity *sa, char *str)
{
  int hun = to_hundreths(str);
  if (hun == -1) { return NULL; }
  sa->begin = hun;
  return strchr(str, '\t');
}

static char *splt_audacity_set_end(splt_audacity *sa, char *str)
{
  int hun = to_hundreths(str);
  if (hun == -1) { return NULL; }
  sa->end  = hun;
  return strchr(str, '\t');
}

static splt_audacity *splt_audacity_process_line(splt_state *state, char *line,
    splt_audacity *previous_aud, int *append_begin_point, int *error)
{
  splt_audacity *aud = splt_audacity_new();
  if (!aud)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return NULL;
  }

  char *ptr = line;

  errno = 0;
  ptr = splt_audacity_set_begin(aud, ptr);
  if (ptr == NULL || *ptr == '\0') {
    *error = SPLT_INVALID_AUDACITY_FILE;
    goto error;
  }
  ptr++;

  errno = 0;
  ptr = splt_audacity_set_end(aud, ptr);
  if (ptr == NULL || *ptr == '\0')
  {
    *error = SPLT_INVALID_AUDACITY_FILE;
    goto error;
  }
  ptr++;

  ptr = splt_su_trim_spaces(ptr);

  int err = splt_audacity_set_name(aud, ptr);
  if (err < 0) { *error = err; goto error; }

  err = splt_audacity_append_splitpoints(state, previous_aud, aud, append_begin_point);
  if (err < 0) { *error = err; goto error; }

  return aud;

error:
  splt_audacity_free(&aud);
  return NULL;
}

int splt_audacity_put_splitpoints(const char *file, splt_state *state, int *error)
{
	int tracks = -1;

  if (file == NULL)
  {
    *error = SPLT_INVALID_AUDACITY_FILE;
    return tracks;
  }

  splt_t_free_splitpoints_tags(state);

  *error = SPLT_AUDACITY_OK;

  splt_c_put_info_message_to_client(state, 
      _(" reading informations from audacity labels file '%s' ...\n"), file);

	FILE *file_input = NULL;

	if (!(file_input = splt_io_fopen(file, "r")))
  {
    splt_e_set_strerror_msg_with_data(state, file);
    *error = SPLT_ERROR_CANNOT_OPEN_FILE;
    return tracks;
  }

	if (fseek(file_input, 0, SEEK_SET) != 0)
  {
    splt_e_set_strerror_msg_with_data(state, file);
    *error = SPLT_ERROR_SEEKING_FILE;
    goto end;
  }

  int append_begin_point = SPLT_TRUE;
  int err = SPLT_OK;

  splt_audacity *aud = NULL;
  splt_audacity *previous_aud = NULL;

  tracks = 0;
  char *line = NULL;
  while ((line = splt_io_readline(file_input, error)) != NULL)
  {
    if (*error < 0) { goto end; }

    if (splt_su_is_empty_line(line))
    {
      free(line);
      line = NULL;
      continue;
    }

    aud = splt_audacity_process_line(state, line, previous_aud,
        &append_begin_point, &err);
    if (err < 0) { goto end; }

    if (previous_aud)
    {
      splt_audacity_free(&previous_aud);
    }
    previous_aud = aud;

    free(line);
    line = NULL;

    tracks++;
	}

  if (previous_aud)
  {
    err = splt_audacity_append_splitpoints(state, previous_aud, aud, &append_begin_point);
    if (err < 0) { *error = err; }
  }

end:
  if (line)
  {
    free(line);
    line = NULL;
  }

  if (previous_aud)
  {
    splt_audacity_free(&previous_aud);
  }

  if (fclose(file_input) != 0)
  {
    splt_e_set_strerror_msg_with_data(state, file);
    *error = SPLT_ERROR_CANNOT_CLOSE_FILE;
  }
  file_input = NULL;

	return tracks;
}

