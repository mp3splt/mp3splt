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

#ifndef DATA_MANAGER_H

#include <libmp3splt/mp3splt.h>

#include "options_manager.h"

typedef struct
{
  double level_sum;
  unsigned long number_of_levels;
  //if set to FALSE, don't show the average silence level
  int print_silence_level;
} silence_level;

typedef struct
{
  //command line options
  options *opt;
  //the libmp3splt state
  splt_state *state;
  //for computing the average silence level
  silence_level *sl;
  //the filenames parsed from the arguments
  char **filenames;
  int number_of_filenames;
  //the splitpoints parsed from the arguments
  long *splitpoints;
  int number_of_splitpoints;
  //command line arguments: on windows, we need to
  //keep the ones transformed to utf8 and free them later
  char **argv;
  int argc;
  int printed_value_length;
} main_data;


main_data *create_main_struct(int argc, char **orig_argv);
void free_main_struct(main_data **d);

void append_filename(main_data *data, const char *str);
void append_splitpoint(main_data *data, long value);

#define DATA_MANAGER_H
#endif

