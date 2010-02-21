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
#include <errno.h>

#ifdef __WIN32__
#include <winsock.h>
#else
#include <netdb.h>
#endif

#include "splt.h"

void splt_e_set_errors_default_values(splt_state *state)
{
  state->err.error_data = NULL;
  state->err.strerror_msg = NULL;
}

void splt_e_free_errors(splt_state *state)
{
  if (state->err.error_data)
  {
    free(state->err.error_data);
    state->err.error_data = NULL;
  }
  if (state->err.strerror_msg)
  {
    free(state->err.strerror_msg);
    state->err.strerror_msg = NULL;
  }
}

void splt_e_set_error_data(splt_state *state, const char *error_data)
{
  if (state->err.error_data)
  {
    free(state->err.error_data);
    state->err.error_data = NULL;
  }
  if (error_data)
  {
    state->err.error_data = malloc(sizeof(char) * (strlen(error_data) + 1));
    if (state->err.error_data)
    {
      snprintf(state->err.error_data, strlen(error_data)+1, "%s", error_data);
    }
  }
}

void splt_e_set_error_data_from_splitpoint(splt_state *state, long splitpoint)
{
  char str_value[256] = { '\0' };
  long mins = 0, secs = 0, hundr = 0;
  splt_sp_get_mins_secs_hundr_from_splitpoint(splitpoint, &mins, &secs, &hundr);
  snprintf(str_value,256,"%ldm%lds%ldh",mins,secs,hundr);
  splt_e_set_error_data(state, str_value);
}

void splt_e_set_error_data_from_splitpoints(splt_state *state, long splitpoint1,
    long splitpoint2)
{
  char str_value[256] = { '\0' };
  long mins = 0, secs = 0, hundr = 0;
  long mins2 = 0, secs2 = 0, hundr2 = 0;
  splt_sp_get_mins_secs_hundr_from_splitpoint(splitpoint1, &mins, &secs, &hundr);
  splt_sp_get_mins_secs_hundr_from_splitpoint(splitpoint2, &mins2, &secs2, &hundr2);
  snprintf(str_value,256,"%ldm%lds%ldh, %ldm%lds%ldh",
      mins, secs, hundr, mins2, secs2, hundr2);
  splt_e_set_error_data(state, str_value);
}

void splt_e_set_strerr_msg(splt_state *state, const char *message)
{
  if (state->err.strerror_msg)
  {
    free(state->err.strerror_msg);
    state->err.strerror_msg = NULL;
  }

  if (message)
  {
    state->err.strerror_msg = malloc(sizeof(char) * (strlen(message) + 1));
    if (state->err.strerror_msg)
    {
      snprintf(state->err.strerror_msg,strlen(message)+1,"%s",message);
    }
    else
    {
      splt_u_error(SPLT_IERROR_CHAR,__func__, 0, _("not enough memory"));
    }
  }
  else
  {
    state->err.strerror_msg = NULL;
  }
}

void splt_e_clean_strerror_msg(splt_state *state)
{
  splt_e_set_strerr_msg(state, NULL);
}

void splt_e_set_strerror_msg(splt_state *state)
{
  char *strerr = strerror(errno);
  splt_e_set_strerr_msg(state, strerr);
}

void splt_e_set_strherror_msg(splt_state *state)
{
#ifndef __WIN32__
  const char *hstrerr = hstrerror(h_errno);
  splt_e_set_strerr_msg(state, hstrerr);
#else
  splt_e_set_strerr_msg(state, _("Network error"));
#endif
}


