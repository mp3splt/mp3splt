/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2008 Alexandru Munteanu - io_fx@yahoo.fr
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

#ifndef MP3SPLT_SPLT_H

#include "mp3splt.h"
#include "types_func.h"
#include "cddb.h"
#include "utils.h"
#include "checks.h"
#include "plugins.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <limits.h>

#if !HAVE_FSEEKO
#define fseeko fseek
#define ftello ftell
#endif

/****************************/
/* splt utils */

splt_state *splt_s_get_mp3_info(splt_state *state, FILE *file_input,
    int *error);
splt_state *splt_s_get_ogg_info(splt_state *state, FILE *file_input,
    int *error);
void splt_s_put_total_time(splt_state *state, int *error);

/**********************************/
/* splt normal or syncerror split */

void splt_s_multiple_split(splt_state *state, int *error);

/************************************/
/* splt time split */

void splt_s_time_split(splt_state *state, int *error);

/************************************/
/* splt silence detection and split */

int splt_s_set_silence_splitpoints(splt_state *state,
    int write_tracks, int *error);
void splt_s_silence_split(splt_state *state, int *error);

/****************************/
/* splt wrap split */

void splt_s_wrap_split(splt_state *state, int *error);

/* other stuff :/ */

#define SPLT_DEFAULT_PROGRESS_RATE 350
#define SPLT_DEFAULT_PROGRESS_RATE2 50

#define SPLT_DEFAULTSILLEN 10
#define SPLT_DEFAULTSHOT 25

#define SPLT_VARCHAR '@'

#define SPLT_OGGEXT ".ogg"

//max number of splitpoints 
//for silence detection and for syncerrors
#define SPLT_MAXSYNC INT_MAX
#define SPLT_MAXSILENCE INT_MAX

/* libmp3splt internals */
#define SPLT_IERROR_INT -1
#define SPLT_IERROR_SET_ORIGINAL_TAGS -2
#define SPLT_IERROR_CHAR -3

//tags options
#define SPLT_TAGS_TITLE 0
#define SPLT_TAGS_ARTIST 1
#define SPLT_TAGS_ALBUM 2
#define SPLT_TAGS_YEAR 3
#define SPLT_TAGS_COMMENT 4
#define SPLT_TAGS_TRACK 5
#define SPLT_TAGS_GENRE 6
#define SPLT_TAGS_PERFORMER 7

#define MP3SPLT_SPLT_H

#endif

