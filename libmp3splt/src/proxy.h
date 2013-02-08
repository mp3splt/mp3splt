/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2013 Alexandru Munteanu - m@ioalex.net
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

#ifndef MP3SPLT_PROXY_H

void splt_pr_set_default_values(splt_state *state);
void splt_pr_free(splt_state *state);

splt_code splt_pr_use_proxy(splt_state *state, const char *proxy_address, int proxy_port);
splt_code splt_pr_use_base64_authentification(splt_state *state, const char *base64_authentification);
char *splt_pr_base64(const unsigned char *source);

int splt_pr_has_proxy(splt_state *state);
int splt_pr_has_proxy_authentification(splt_state *state);

const char *splt_pr_get_proxy_address(splt_state *state);
int splt_pr_get_proxy_port(splt_state *state);
const char *splt_pr_get_proxy_authentification(splt_state *state);

#define MP3SPLT_PROXY_H

#endif

