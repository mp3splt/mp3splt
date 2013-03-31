/**
 * libmp3splt 
 *
 *        Utility for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2004 M. Trotta - <matteo.trotta@lib.unimib.it>
 * Copyright (c) 2005-2013 Alexandru Munteanu <m@ioalex.net>
 *
 * http://mp3splt.sourceforge.net
 **/

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

#ifndef _MP3SPLT_SOCKET_MANAGER_H

#if defined(__BEOS__) && (IPPROTO_UDP==1)
// net_server has a weird order for IPPROTO_
#else
 #ifndef __WIN32__
  #define closesocket close
 #endif
#endif

typedef struct {
  int error;
#ifdef __WIN32__
  SOCKET fd;
#else
  int fd;
#endif
  char *hostname;
} splt_socket_handler;

typedef struct {
  int (*functor)(const char *received_line, int line_number, void *user_data);
  void *user_data;
  int processing_headers;
  int num_lines_to_skip;
  int line_number_after_headers;
  int line_number;
} splt_sm_functor_decorator;

splt_socket_handler *splt_sm_socket_handler_new(int *error);
void splt_sm_socket_handler_free(splt_socket_handler **sh);

void splt_sm_connect(splt_socket_handler *sh, const char *hostname, int port,
    splt_state *state);
void splt_sm_send_http_message(splt_socket_handler *sh, const char *message,
    splt_state *state);

void splt_sm_receive_and_process_without_headers(splt_socket_handler *sh, splt_state *state,
    int (*process_functor)(const char *received_line, int line_number, void *user_data),
    void *user_data, int number_of_lines_to_skip_after_headers);

char *splt_sm_receive_and_process_without_headers_with_recv(splt_socket_handler *sh, 
    splt_state *state,
    ssize_t (*recv_func)(int fd, void *buf, size_t len, int flags),
    int (*process_functor)(const char *received_line, int line_number, void *user_data),
    void *user_data, int number_of_lines_to_skip_after_headers);

void splt_sm_receive_and_process(splt_socket_handler *sh, splt_state *state,
    int (*process_functor)(const char *received_line, int line_number, void *user_data),
    void *user_data);

char *splt_sm_receive_and_process_with_recv(splt_socket_handler *sh, splt_state *state,
    ssize_t (*recv_func)(int fd, void *buf, size_t len, int flags),
    int (*process_functor)(const char *received_line, int line_number, void *user_data),
    void *user_data);

void splt_sm_close(splt_socket_handler *sh, splt_state *state);

#define _MP3SPLT_SOCKET_MANAGER_H
#endif

