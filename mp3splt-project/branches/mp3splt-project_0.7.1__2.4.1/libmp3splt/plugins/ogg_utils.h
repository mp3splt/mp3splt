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

#ifndef MP3SPLT_OGG_UTILS_H

#include "splt.h"
#include "ogg.h"

#define TOTAL_HEADER_PACKETS 3

long splt_ogg_get_blocksize(splt_ogg_state *oggstate, vorbis_info *vi, ogg_packet *op);
ogg_int64_t splt_ogg_compute_first_granulepos(splt_state *state, splt_ogg_state *oggstate,
    ogg_packet *packet, int bs);
int splt_ogg_update_sync(splt_state *state, ogg_sync_state *sync_in, FILE *f, int *error);
splt_v_packet *splt_ogg_clone_packet(ogg_packet *packet, int *error);
void splt_ogg_free_packet(splt_v_packet **p);
void splt_ogg_free_oggstate_headers(splt_ogg_state *oggstate);
int splt_ogg_write_pages_to_file(splt_state *state, ogg_stream_state *stream,
    FILE *file, int flush, int *error, const char *output_fname);
int splt_ogg_write_header_packets(splt_state *state, splt_ogg_state *oggstate,
    ogg_stream_state *stream_out, const char *output_fname, int *error);
void splt_ogg_put_tags(splt_state *state, int *error);
void splt_ogg_set_tags_in_headers(splt_ogg_state *oggstate, int *error);
vorbis_comment *splt_ogg_clone_vorbis_comment(vorbis_comment *comment);
void splt_ogg_free_vorbis_comment(vorbis_comment *vc, short cloned_vorbis_comment);

#define MP3SPLT_OGG_UTILS_H

#endif
