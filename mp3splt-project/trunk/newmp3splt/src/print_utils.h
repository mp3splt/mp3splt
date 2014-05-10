/*
 * Mp3Splt -- Utility for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2014 Alexandru Munteanu - <m@ioalex.net>
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

#ifndef PRINT_UTILS_H

#include <libmp3splt/mp3splt.h>

#include "data_manager.h"

void print_message(const char *m);
void print_warning(const char *w);
void print_error(const char *e);
void put_library_message(const char *message, splt_message_type mess_type, void *data);
void put_split_file(const char *file, void *data);
void put_progress_bar(splt_progress *p_bar, void *data);
void print_version(FILE *std);
void print_authors(FILE *std);
void print_no_warranty(FILE *std);
void print_version_authors(FILE *std);

void print_error_exit(const char *m, main_data *data);
void print_message_exit(const char *m, main_data *data);
void process_confirmation_error(int conf, main_data *data);
void show_small_help_exit(main_data *data);

#define PRINT_UTILS_H
#endif

