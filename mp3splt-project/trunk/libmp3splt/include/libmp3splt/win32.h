/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2010 Alexandru Munteanu - io_fx@yahoo.fr
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

#ifdef __WIN32__

#ifndef MP3SPLT_WIN32_H
#define MP3SPLT_WIN32_H

#include <dirent.h>

#ifdef __WIN32__
#include <windows.h>
#include <direct.h>
#endif

int scandir(const char *dir, struct dirent ***namelist,
		int(*filter)(const struct dirent *),
		int(*compar)(const struct dirent **, const struct dirent **));

int wscandir(const char *dir, struct _wdirent ***namelist,
		int(*filter)(const struct _wdirent *),
		int(*compar)(const struct _wdirent **, const struct _wdirent **));

int alphasort(const struct dirent **a, const struct dirent **b);

int walphasort(const struct _wdirent **a, const struct _wdirent **b);

#endif

#endif

