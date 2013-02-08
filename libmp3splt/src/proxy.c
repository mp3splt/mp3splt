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

#include "splt.h"

#include <string.h>

static void splt_pr_free_proxy_address(splt_state *state);
static void splt_pr_free_proxy_authentification(splt_state *state);
static char *splt_pr_encode3to4(const unsigned char *source, int srcoffset, int num, 
    char *destination, int destoffset);

splt_code splt_pr_use_proxy(splt_state *state, const char *proxy_address, int proxy_port)
{
  if (proxy_address == NULL || proxy_address[0] == '\0')
  {
    return SPLT_OK;
  }

  splt_pr_free_proxy_address(state);
  splt_su_copy(proxy_address, &state->proxy.proxy_address);

  state->proxy.proxy_port = proxy_port;

  return SPLT_OK;
}

int splt_pr_has_proxy(splt_state *state)
{
  return state->proxy.proxy_address != NULL && state->proxy.proxy_port != -1;
}

int splt_pr_has_proxy_authentification(splt_state *state)
{
  return state->proxy.authentification != NULL;
}

const char *splt_pr_get_proxy_authentification(splt_state *state)
{
  return state->proxy.authentification;
}

const char *splt_pr_get_proxy_address(splt_state *state)
{
  return state->proxy.proxy_address;
}

int splt_pr_get_proxy_port(splt_state *state)
{
  return state->proxy.proxy_port;
}

splt_code splt_pr_use_base64_authentification(splt_state *state, 
    const char *base64_authentification)
{
  if (base64_authentification == NULL)
  {
    return SPLT_OK;
  }

  splt_pr_free_proxy_authentification(state);
  splt_su_copy(base64_authentification, &state->proxy.authentification);

  return SPLT_OK;
}

void splt_pr_set_default_values(splt_state *state)
{
  state->proxy.proxy_address = NULL;
  state->proxy.proxy_port = -1;
  state->proxy.authentification = NULL;
}

void splt_pr_free(splt_state *state)
{
  splt_pr_free_proxy_address(state);
  splt_pr_free_proxy_authentification(state);
  splt_pr_set_default_values(state);
}

char *splt_pr_base64(const unsigned char *source)
{
  int len = strlen((char *)source);

  int d = ((len*4/3)+((len%3)>0?4:0));

  char *out = malloc(d + 1);
  if (out == NULL) { return NULL; }
  memset(out, 0x00, d+1);

  int e = 0;
  for (d = 0;d < (len-2); d+=3,e+=4)
  {
    out = splt_pr_encode3to4(source, d, 3, out, e);
  }

  if (d < len)
  {
    out = splt_pr_encode3to4(source, d, len-d, out, e);
  }

  return out;
}

static void splt_pr_free_proxy_address(splt_state *state)
{
  if (state->proxy.proxy_address)
  {
    free(state->proxy.proxy_address);
    state->proxy.proxy_address = NULL;
  }
}

static void splt_pr_free_proxy_authentification(splt_state *state)
{
  if (state->proxy.authentification)
  {
    size_t authentification_length = strlen(state->proxy.authentification);
    memset(state->proxy.authentification, '\0', authentification_length);
    free(state->proxy.authentification);
    state->proxy.authentification = NULL;
  }
}

/*
 * Base64 Algorithm: Base64.java v. 1.3.6 by Robert Harder
 * Ported and optimized for C by Matteo Trotta
 */
static const char alphabet [] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static char *splt_pr_encode3to4(const unsigned char *source, int srcoffset, int num, char *destination, int destoffset)
{
  int inbuff=
    (num>0 ? (source[srcoffset] << 16) : 0)|
    (num>1 ? (source[srcoffset+1] << 8) : 0) |
    (num > 2 ? (source[srcoffset+2]) : 0);

  switch (num)
  {
    case 3:
      destination[destoffset] = alphabet[(inbuff>>18)];
      destination[destoffset+1] = alphabet[(inbuff>>12) & 0x3f];
      destination[destoffset+2] = alphabet[(inbuff>>6) & 0x3f];
      destination[destoffset+3] = alphabet[(inbuff) & 0x3f];
      return destination;
    case 2:
      destination[destoffset] = alphabet[(inbuff>>18)];
      destination[destoffset+1] = alphabet[(inbuff>>12) & 0x3f];
      destination[destoffset+2] = alphabet[(inbuff>>6) & 0x3f];
      destination[destoffset+3] = '=';
      return destination;
    case 1:
      destination[destoffset] = alphabet[(inbuff>>18)];
      destination[destoffset+1] = alphabet[(inbuff>>12) & 0x3f];
      destination[destoffset+2] = '=';
      destination[destoffset+3] = '=';
      return destination;
    default:
      return destination;
  }
}

