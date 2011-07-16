/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2011 Alexandru Munteanu - <io_fx@yahoo.fr>
 *
 * Parts of this file have been copied from the 'vcut' 1.6
 * program provided with 'vorbis-tools' :
 *      vcut (c) 2000-2001 Michael Smith <msmith@xiph.org>
 *
 * Some parts from a more recent version of vcut :
 *           (c) 2008 Michael Gold <mgold@ncf.ca>
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

#include "ogg_utils.h"

long splt_ogg_get_blocksize(splt_ogg_state *oggstate, vorbis_info *vi, ogg_packet *op)
{
  //if this < 0, there is a problem
  int this = vorbis_packet_blocksize(vi, op);
  int ret = (this + oggstate->prevW)/4;

  oggstate->prevW = this;

  return ret;
}

ogg_int64_t splt_ogg_compute_first_granulepos(splt_state *state, splt_ogg_state *oggstate, ogg_packet *packet, int bs)
{
  ogg_int64_t first_granpos = 0;

  if (packet->granulepos >= 0)
  {
    /*fprintf(stdout,"oggstate->total_blocksize + bs = %ld\n",oggstate->total_blocksize + bs);
    fprintf(stdout,"packet granulepos = %ld\n",packet->granulepos);
    fflush(stdout);*/

    if ((packet->granulepos > oggstate->total_blocksize + bs) &&
        (oggstate->total_blocksize > 0) &&
        !packet->e_o_s &&
        (oggstate->first_granpos == 0))
    {
      first_granpos = packet->granulepos;
      oggstate->first_granpos = first_granpos;
      splt_c_put_info_message_to_client(state,
          _(" warning: unexpected position in ogg vorbis stream - split from 0.0 to EOF to fix.\n"));
    }

    oggstate->total_blocksize = packet->granulepos;
  }
  else if (oggstate->total_blocksize == -1)
  {
    oggstate->total_blocksize = 0;
  }
  else {
    oggstate->total_blocksize += bs;
    /*fprintf(stdout,"blocksize = %d, total = %ld\n", bs, oggstate->total_blocksize);
    fflush(stdout);*/
  }

  return first_granpos;
}

int splt_ogg_update_sync(splt_state *state, ogg_sync_state *sync_in, FILE *f, int *error)
{
  char *buffer = ogg_sync_buffer(sync_in, SPLT_OGG_BUFSIZE);
  if (!buffer)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return -1;
  }
  int bytes = fread(buffer,1,SPLT_OGG_BUFSIZE,f);

  if (ogg_sync_wrote(sync_in, bytes) != 0)
  {
    splt_e_set_error_data(state, splt_t_get_filename_to_split(state));
    *error = SPLT_ERROR_INVALID;
    return -1;
  }

  return bytes;
}



