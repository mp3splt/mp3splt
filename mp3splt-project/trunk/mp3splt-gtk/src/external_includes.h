/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2014 Alexandru Munteanu
 * Contact: m@ioalex.net
 *
 * http://mp3splt.sourceforge.net/
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

#ifndef EXTERNAL_INCLUDES_H

#define EXTERNAL_INCLUDES_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define VERSION "0.9.2"
#define PACKAGE_NAME "mp3splt-gtk"
#endif

#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <locale.h>
#include <ctype.h>

#include <sys/stat.h>
#include <sys/types.h>

#ifdef __WIN32__

#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

#include <shlwapi.h>
#define usleep(x) Sleep(x/1000)

#else

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#endif

#include <gtk/gtk.h>

#include <glib.h>
#include <glib/gi18n.h>
#include <glib/gstdio.h>
#include <glib/gprintf.h>

#include <gdk/gdkkeysyms.h>

#include <libmp3splt/mp3splt.h>

#endif

