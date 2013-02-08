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

#ifndef MP3SPLT_OGG_NEW_STREAM_HANDLER_H

#include "splt.h"

#include "ogg.h"
#include "ogg_utils.h"

typedef struct {
  int header_packet_counter;
  splt_state *state;
  splt_ogg_state *oggstate;
  ogg_stream_state *stream_out;
  const char *output_fname;
  int write_header_packets;
  ogg_stream_state *optional_stream_in;
} splt_ogg_new_stream_handler;

splt_ogg_new_stream_handler *splt_ogg_nsh_new(splt_state *state, 
    splt_ogg_state *oggstate, ogg_stream_state *stream_out, const char *output_fname,
    int write_header_packets, ogg_stream_state *optional_stream_in);

void splt_ogg_nsh_free(splt_ogg_new_stream_handler **nsh);

void splt_ogg_initialise_for_new_stream(splt_ogg_new_stream_handler *nsh, 
    ogg_page *page, ogg_int64_t *cutpoint, ogg_int64_t previous_granulepos);
int splt_ogg_new_stream_needs_header_packet();
void splt_ogg_new_stream_handle_header_packet(splt_ogg_new_stream_handler *nsh, ogg_packet *packet, int *error);

#define MP3SPLT_OGG_NEW_STREAM_HANDLER_H

#endif

