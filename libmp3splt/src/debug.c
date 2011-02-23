/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2011 Alexandru Munteanu - io_fx@yahoo.fr
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
#include <stdarg.h>

#include "splt.h"

extern int global_debug;

static char global_mem_err_mess[1024] = "error allocating memory in splt_d_print_debug !\n";

static void splt_d_send_message(splt_state *state, const char *mess);

void splt_d_print_debug(splt_state *state, const char *message, ...)
{
  if (global_debug)
  {
    va_list ap;

    va_start(ap, message);
    char *mess = splt_su_format_messagev(state, message, ap);
    va_end(ap);

    if (mess)
    {
      splt_d_send_message(state, mess);

      free(mess);
      mess = NULL;
    }
  }
}

void splt_d_send_memory_error_message(splt_state *state)
{
  splt_d_send_message(state, global_mem_err_mess);
}

static void splt_d_send_message(splt_state *state, const char *mess)
{
  if (state)
  {
    splt_c_put_debug_message_to_client(state, mess);
  }
  else
  {
    fprintf(stdout,"%s\n",mess);
    fflush(stdout);
  }

}

