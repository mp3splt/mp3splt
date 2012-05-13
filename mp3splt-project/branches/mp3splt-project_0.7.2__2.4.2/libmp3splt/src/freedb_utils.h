/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2012 Munteanu Alexandru - io_fx@yahoo.fr
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

#ifndef SPLT_FREEDB_UTILS_H

void splt_fu_set_default_values(splt_state *state);
void splt_fu_freedb_free_search(splt_state *state);

int splt_fu_freedb_init_search(splt_state *state);
int splt_fu_freedb_append_result(splt_state *state, const char *album_name, int revision);

int splt_fu_freedb_get_found_cds(splt_state *state);
void splt_fu_freedb_found_cds_next(splt_state *state);

void splt_fu_freedb_set_disc(splt_state *state, int index,
    const char *discid, const char *category, int category_size);

const char *splt_fu_freedb_get_disc_category(splt_state *state, int index);
const char *splt_fu_freedb_get_disc_id(splt_state *state, int index);

#define SPLT_FREEDB_UTILS_H

#endif

