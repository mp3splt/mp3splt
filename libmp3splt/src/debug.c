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

#include "splt.h"

extern int global_debug;

void splt_d_print_debug(splt_state *state, const char *message,
    double optional, const char *optional2)
{
  if (global_debug)
  {
    int mess_size = 1024;
    if (message)
    {
      mess_size += strlen(message);
    }
    if (optional2)
    {
      mess_size += strlen(optional2);
    }
    char *mess = malloc(sizeof(char) * mess_size);

    if (optional != 0)
    {
      if (optional2 != NULL)
      {
        snprintf(mess, mess_size, "%s %f _%s_\n",message, optional, optional2);
      }
      else
      {
        snprintf(mess, mess_size, "%s %f\n",message, optional);
      }
    }
    else
    {
      if (optional2 != NULL)
      {
        snprintf(mess, mess_size, "%s _%s_\n",message, optional2);
      }
      else
      {
        snprintf(mess, mess_size, "%s\n",message);
      }
    }

    if (state)
    {
      splt_c_put_debug_message_to_client(state, mess);
    }
    else
    {
      fprintf(stdout,"%s",mess);
      fflush(stdout);
    }

    free(mess);
    mess = NULL;
  }
}

