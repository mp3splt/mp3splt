/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2011 Alexandru Munteanu - io_fx@yahoo.fr
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

#ifndef FREEDB_H

//structure for the socket connection
typedef struct {
  short proxy;
  char hostname[512];
  int port;
  char *auth;
} splt_addr;

typedef struct {
  int err;
  char *file;
  int stop_on_dot;
} splt_get_file;

int splt_freedb_process_search(splt_state *state, char *search, 
    int search_type, const char *cddb_get_server,
    int port);
char *splt_freedb_get_file(splt_state *state, int i, int *error,
    int get_type, const char *cddb_get_server, int port);

//global freedb, ports and buffersize
#define SPLT_FREEDB_BUFFERSIZE 8192

#define SPLT_FREEDB2_SITE "tracktype.org"
#define SPLT_FREEDB_SITE "freedb.org"

//cddb protocol
#define SPLT_FREEDB_HELLO "CDDB HELLO nouser mp3splt.sf.net "SPLT_PACKAGE_NAME" "SPLT_PACKAGE_VERSION"\n"
#define SPLT_FREEDB_GET_FILE "CDDB READ %s %s\n"

//cddb.cgi
#define SPLT_FREEDB2_SEARCH "GET %s?cmd=cddb+album+%s"SPLT_FREEDB_HELLO_PROTO
#define SPLT_FREEDB_HELLO_PROTO "&hello=nouser+mp3splt.sf.net+"SPLT_PACKAGE_NAME"+"SPLT_PACKAGE_VERSION"&proto=5"
#define SPLT_FREEDB_CDDB_CGI_GET_FILE "GET %s?cmd=cddb+read+%s+%s"SPLT_FREEDB_HELLO_PROTO

//proxy stuff
//#define PROXYCONFIG ".mp3splt"
//#define PROXYDLG "HTTP/1.0\nUserAgent: "PACKAGE_NAME"/"PACKAGE_VERSION"\n"
//#define SPLT_AUTH "Proxy-Authorization: Basic "

#define FREEDB_H

#endif

