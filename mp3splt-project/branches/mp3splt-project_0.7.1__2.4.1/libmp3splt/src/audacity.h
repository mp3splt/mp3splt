/*
 * audacity.h -- Audacity label file parser portion of the Mp3Splt utility
 *                    Utility for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2004 M. Trotta - <matteo.trotta@lib.unimib.it>
 * Copyright (c) 2007 Federico Grau - <donfede@casagrau.org>
 *
 * http://mp3splt.sourceforge.net
 * http://audacity.sourceforge.net/
 */

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

#ifndef _MP3SPLT_AUDACITY_H

int splt_audacity_put_splitpoints(const char *file, splt_state *state, int *error);

typedef struct {
  long begin;
  long end;
  char *name;
} splt_audacity;

#define _MP3SPLT_AUDACITY_H
#endif

