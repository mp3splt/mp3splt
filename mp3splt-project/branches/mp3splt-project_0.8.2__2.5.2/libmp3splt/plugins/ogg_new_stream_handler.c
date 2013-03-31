/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2013 Alexandru Munteanu - <m@ioalex.net>
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 *
 *********************************************************/

#include "ogg_new_stream_handler.h"

#include <string.h>

splt_ogg_new_stream_handler *splt_ogg_nsh_new(splt_state *state, splt_ogg_state *oggstate, 
    ogg_stream_state *stream_out, const char *output_fname, int write_header_packets,
    ogg_stream_state *optional_stream_in)
{
  splt_ogg_new_stream_handler *nsh = malloc(sizeof(*nsh));

  if (nsh == NULL)
  {
    return NULL;
  }

  memset(nsh, 0x0, sizeof(*nsh));

  nsh->header_packet_counter = TOTAL_HEADER_PACKETS;
  nsh->state = state;
  nsh->oggstate = oggstate;
  nsh->stream_out = stream_out;
  nsh->output_fname = output_fname;
  nsh->write_header_packets = write_header_packets;
  nsh->optional_stream_in = optional_stream_in;

  return nsh;
}

void splt_ogg_nsh_free(splt_ogg_new_stream_handler **nsh)
{
  if (!nsh || !*nsh)
  {
    return;
  }

  free(*nsh);
  *nsh = NULL;
}

void splt_ogg_initialise_for_new_stream(splt_ogg_new_stream_handler *nsh, 
    ogg_page *page, ogg_int64_t *cutpoint, ogg_int64_t previous_granulepos)
{
  splt_ogg_state *oggstate = nsh->oggstate;

  ogg_stream_state *stream_in = nsh->optional_stream_in;
  if (nsh->optional_stream_in == NULL)
  {
    stream_in = oggstate->stream_in;
  }

  ogg_stream_clear(stream_in);
  ogg_stream_init(stream_in, ogg_page_serialno(page));
  oggstate->saved_serial = ogg_page_serialno(page);

  if (cutpoint != NULL && *cutpoint != 0)
  {
    *cutpoint -= previous_granulepos;
  }

  nsh->header_packet_counter = 0;
}

int splt_ogg_new_stream_needs_header_packet(splt_ogg_new_stream_handler *nsh)
{
  return nsh->header_packet_counter < TOTAL_HEADER_PACKETS;
}

void splt_ogg_new_stream_handle_header_packet(splt_ogg_new_stream_handler *nsh, ogg_packet *packet, int *error)
{
  splt_ogg_state *oggstate = nsh->oggstate;

  if (!nsh->write_header_packets)
  {
    nsh->header_packet_counter++;
    return;
  }

  splt_state *state = nsh->state;

  if (nsh->header_packet_counter == 0)
  {
    splt_ogg_free_oggstate_headers(oggstate);

    oggstate->headers = malloc(sizeof(splt_v_packet) * TOTAL_HEADER_PACKETS);
    if (oggstate->headers == NULL)
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      return;
    }
    memset(oggstate->headers, 0, sizeof(splt_v_packet) * TOTAL_HEADER_PACKETS);

    splt_ogg_free_vorbis_comment(&oggstate->vc, oggstate->cloned_vorbis_comment); 
    vorbis_info_clear(oggstate->vi);
    vorbis_info_init(oggstate->vi);
  }

  oggstate->headers[nsh->header_packet_counter] = splt_ogg_clone_packet(packet, error);
  if (*error < 0)
  {
    return;
  }

  if (vorbis_synthesis_headerin(oggstate->vi, &oggstate->vc, packet) < 0)
  {
    *error = SPLT_ERROR_INVALID;
    splt_e_set_error_data(state, splt_t_get_filename_to_split(state));
    return;
  }
  oggstate->cloned_vorbis_comment = SPLT_FALSE;

  nsh->header_packet_counter++;

  if (nsh->write_header_packets && nsh->header_packet_counter == TOTAL_HEADER_PACKETS)
  {
    oggstate->serial++;

    ogg_stream_clear(nsh->stream_out);
    ogg_stream_init(nsh->stream_out, oggstate->serial);

    splt_ogg_put_tags(state, error);
    if (*error < 0) { return; }

    splt_ogg_set_tags_in_headers(oggstate, error);
    if (*error < 0) { return; }

    splt_ogg_write_header_packets(state, oggstate, nsh->stream_out, nsh->output_fname, error);
  }
}

