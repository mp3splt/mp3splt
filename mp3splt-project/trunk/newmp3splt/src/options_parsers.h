/*
 * Mp3Splt -- Utility for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2012 Alexandru Munteanu - <m@ioalex.net>
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


#ifndef OPTIONS_PARSER_H

#include "options_manager.h"

int parse_silence_options(char *arg, float *th, int *gap,
    int *nt, float *off, int *rm, float *min, float *min_track_length, int *shots,
    float *min_track_join, float *keep_silence_left, float *keep_silence_right);
int parse_query_arg(options *opt, const char *query);
regex_options *parse_tags_from_fname_regex_options(const char *parameters, int *error);

#define OPTIONS_PARSER_H
#endif

