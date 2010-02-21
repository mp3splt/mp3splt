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
#include <math.h>

#include "splt.h"

static int splt_of_new_oformat(splt_state *state, const char *format_string);

int splt_of_set_default_values(splt_state *state)
{
  int err = SPLT_OK;

  state->oformat.format_string = NULL;
  splt_of_set_oformat(state, SPLT_DEFAULT_CDDB_CUE_OUTPUT, &err, SPLT_TRUE);

  return err;
}

void splt_of_free_oformat(splt_state *state)
{
  if (state->oformat.format_string)
  {
    free(state->oformat.format_string);
    state->oformat.format_string = NULL;
  }
}

void splt_of_set_oformat_digits_tracks(splt_state *state, int tracks)
{
  int i = (int) (log10((double) (tracks)));
  state->oformat.output_format_digits = (char) (i+'1');

  /* Number of alphabetical "digits": almost base-27 */
  state->oformat.output_alpha_format_digits = 1;
  for (i = (tracks - 1) / 26; i > 0; i /= 27)
    ++ state->oformat.output_alpha_format_digits;
}

void splt_of_set_oformat_digits(splt_state *state)
{
  splt_of_set_oformat_digits_tracks(state, splt_t_get_splitnumber(state));
}

void splt_of_set_oformat(splt_state *state, const char *format_string,
    int *error, int ignore_incorrect_format_warning)
{
  if (format_string == NULL || format_string[0] == '\0')
  {
    *error = SPLT_OUTPUT_FORMAT_ERROR;
    return;
  }

  int j = 0;

  while (j <= SPLT_OUTNUM)
  {
    memset(state->oformat.format[j],'\0',SPLT_MAXOLEN);
    j++;
  }

  int err = SPLT_OK;
  err = splt_of_new_oformat(state, format_string);
  if (err < 0) { *error = err; return; }

  char *new_str = strdup(format_string);
  if (new_str)
  {
    err = splt_u_parse_outformat(new_str, state);
    if (! ignore_incorrect_format_warning)
    {
      *error = err;
    }
    free(new_str);
    new_str = NULL;
  }
  else
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return;
  }

  if (*error > 0)
  {
    splt_of_set_oformat_digits(state); 
  }
}

int splt_of_get_oformat_number_of_digits_as_int(splt_state *state)
{
  return state->oformat.output_format_digits - '0';
}

char splt_of_get_oformat_number_of_digits_as_char(splt_state *state)
{
  return state->oformat.output_format_digits;
}

const char *splt_of_get_oformat(splt_state *state)
{
  return state->oformat.format_string;
}

static int splt_of_new_oformat(splt_state *state, const char *format_string)
{
  int error = SPLT_OK;
  splt_of_free_oformat(state);

  if (format_string != NULL)
  {
    if ((state->oformat.format_string =
          malloc(sizeof(char)*(strlen(format_string)+1))) == NULL)
    {
      error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    }
    else
    {
      snprintf(state->oformat.format_string,
          strlen(format_string)+1,"%s",format_string);
    }
  }
  else
  {
    state->oformat.format_string = NULL;
  }

  return error;
}

