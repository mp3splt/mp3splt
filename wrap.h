/*
 * Mp3Splt -- Utility for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <matteo.trotta@lib.unimib.it>
 *
 * http://mp3splt.sourceforge.net
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111, USA.
 */

#ifndef _MP3SPLT_WRAP_H
#define _MP3SPLT_WRAP_H

#define WRAP "MP3WRAP"
#define ALBW "ALBW"
#define ABWINDEXOFFSET 0x539
#define ABWLEN 0x1f5
#define INDEXVERSION 1
#define CRCLEN 4

unsigned long c_crc (FILE *in, off_t begin, off_t end);

void failed (char *s);

int dewrap (FILE *file_input, int listonly, int quiet, char *darg);

#endif

