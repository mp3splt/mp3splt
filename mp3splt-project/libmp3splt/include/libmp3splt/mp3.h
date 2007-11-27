/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2006 Munteanu Alexandru - io_alex_2002@yahoo.fr
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

#ifndef NO_ID3TAG
#include <id3tag.h>
#endif

/****************************/
/* mp3 utils */

void splt_mp3_state_free(splt_state *state);

/****************************/
/* mp3 tags */

void splt_mp3_get_original_tags(char *filename,splt_state *state,
                                int *tag_error);

char *splt_mp3_get_tags(char *filename,splt_state *state,
                        int *error);

/****************************/
/* mp3 infos */

splt_mp3_state *splt_mp3_info(FILE *file_input, splt_state *state,
                              int framemode, int *error);

/****************************/
/* mp3 split */

int splt_mp3_simple_split (splt_state *state, char *filename,
                           off_t begin, off_t end, char *id3buffer);
void splt_mp3_split (char *filename, splt_state *state, char *id3,
                     double fbegin_sec, double fend_sec, int *error);

/****************************/
/* mp3 scan for silence */

int splt_mp3_silence(splt_mp3_state *mp3state, 
                     int channels, mad_fixed_t threshold);
int splt_mp3_scan_silence (splt_state *sp_state, off_t begin, 
                           unsigned long length, float threshold, float min, short output);

/****************************/
/* mp3 syncerror */

void splt_mp3_syncerror_search (splt_state *state, int *error);

/****************************/
/* mp3 dewrap */

void splt_mp3_dewrap (FILE *file_input, int listonly, char *dir,
                      int *error, splt_state *state);

/****************************/
/* mp3 constants */

/*
  Frame per second:
  Each MPEG1 frame decodes to 1152 PCM
  samples, 576 with MPEG2.
  32000/1152 = 27.77778 = 16000/576
  44100/1152 = 38.28125 = 22050/576
  48000/1152 = 41.66667 = 24000/576
*/

#define SPLT_MP3_TAG "TAG"
#define SPLT_MP3_GENRENUM 82
#define SPLT_MP3_PCM 1152
#define SPLT_MP3_BYTE 8

#define SPLT_MP3_XING_MAGIC 0x58696E67
#define SPLT_MP3_INFO_MAGIC 0x496E666F

#define SPLT_MP3_XING_FRAMES 0x00000001L
#define SPLT_MP3_XING_BYTES  0x00000002L

#define SPLT_MP3_ID3_ARTIST 1
#define SPLT_MP3_ID3_ALBUM 2
#define SPLT_MP3_ID3_TITLE 3
#define SPLT_MP3_ID3_YEAR 4
#define SPLT_MP3_ID3_GENRE 5
#define SPLT_MP3_ID3_TRACK 6
#define SPLT_MP3_ID3_COMMENT 7

//#define SPLT_MP3_CRCLEN 4
#define SPLT_MP3_ABWINDEXOFFSET 0x539
#define SPLT_MP3_ABWLEN 0x1f5
#define SPLT_MP3_INDEXVERSION 1
#define SPLT_MP3_READBSIZE 1024
