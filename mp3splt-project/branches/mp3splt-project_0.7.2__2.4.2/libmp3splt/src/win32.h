/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2012 Alexandru Munteanu - io_fx@yahoo.fr
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

#ifndef SPLT_WIN32_H

#include <errno.h>
#include <dirent.h>

#include <windows.h>
#include <direct.h>

int scandir(const char *dir, struct dirent ***namelist,
		int(*filter)(const struct dirent *),
		int(*compar)(const struct dirent **, const struct dirent **));

int wscandir(const char *dir, struct _wdirent ***namelist,
		int(*filter)(const struct _wdirent *),
		int(*compar)(const struct _wdirent **, const struct _wdirent **));

int alphasort(const struct dirent **a, const struct dirent **b);

int walphasort(const struct _wdirent **a, const struct _wdirent **b);

char *splt_w32_utf16_to_utf8(const wchar_t *source);
wchar_t *splt_w32_utf8_to_utf16(const char *source);
int splt_w32_check_if_encoding_is_utf8(const char *source);

int splt_w32_str_starts_with_drive_root_directory(const char *str);
int splt_w32_str_is_drive_root_directory(const char *str);

#define SPLT_WIN32_H

#endif

#endif

