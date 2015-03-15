/*
 * Mp3Splt -- Utility for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2010 Alexandru Munteanu - <m@ioalex.net>
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef COMMON_H

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include <ctype.h>
#include <getopt.h>
#include <locale.h>

#ifdef __WIN32__
#include <windows.h>
#include <shlwapi.h>
#endif

#include <libmp3splt/mp3splt.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define VERSION "2.6.3.1518"
#define PACKAGE_NAME "mp3splt"
#endif

#ifdef ENABLE_NLS
#  include <libintl.h>
#endif

#ifdef ENABLE_NLS
#  define MP3SPLT_GETTEXT_DOMAIN "mp3splt"
#  define _(STR) gettext(STR)
#else
#  define _(STR) ((const char *)STR)
#endif

#define MP3SPLT_DATE "13/03/15"
#define MP3SPLT_AUTHOR1 "Matteo Trotta"
#define MP3SPLT_AUTHOR2 "Alexandru Munteanu"
#define MP3SPLT_EMAIL1 "<mtrotta AT users.sourceforge.net>"
#define MP3SPLT_EMAIL2 "<m AT ioalex.net>"
#define MP3SPLT_CDDBFILE "query.cddb"

#define COMMON_H
#endif

