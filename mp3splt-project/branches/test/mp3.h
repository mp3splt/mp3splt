/*
 * Mp3Splt -- Utility for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <matteo.trotta@lib.unimib.it>
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

#ifndef _MP3SPLT_MP3_H
#define _MP3SPLT_MP3_H

#define TAG "TAG"
#define GENRENUM 11
#define PCM 1152
#define MAXSYNC MAXTRACKS
#define BYTE 8

#define XING_MAGIC 0x58696E67
#define INFO_MAGIC 0x496E666F

#define XING_FRAMES 0x00000001L
#define XING_BYTES  0x00000002L

#define MAD_BSIZE 4032

#include "mad.h"

/*
	Frame per second:
	Each MPEG1 frame decodes to 1152 PCM
	samples, 576 with MPEG2.
	32000/1152 = 27.77778 = 16000/576
	44100/1152 = 38.28125 = 22050/576
	48000/1152 = 41.66667 = 24000/576
*/

// Struct that will contain header's useful infos
struct header {
     off_t ptr;	// Offset of header
     int bitrate;
     int padding;
     int framesize;
};

// Struct that will contains infos on mp3 and an header struct of first valid header
struct mp3 {
  int mpgid;	// 0 or 1
  int layer;	// 1, 2, or 3
  int channels;
  int freq;
  int bitrate;
  float fps;
  int xing;
  char *xingbuffer;
  off_t xing_offset;
  off_t len;
  off_t firsth;

  struct header firsthead;
};

struct id3 {
  char title[31];
  char artist[31];
  char album[31];
  char year[5];
  unsigned char genre;
};

typedef struct {
  FILE *file_input;
  struct header h;
  short framemode;
  unsigned long frames;
  int syncdetect;
  unsigned long syncerrors;
  off_t end;
  off_t bytes;
  int first;
  unsigned long headw;
  struct mp3 mp3file;
  float off;
  struct ssplit *silence_list;
  unsigned char inputBuffer[MAD_BSIZE];
  struct mad_stream stream;
  struct mad_frame frame;
  struct mad_synth synth;
  mad_timer_t timer;
  unsigned long total_time;
  unsigned char *data_ptr;
  int data_len;
  int buf_len;
  double avg_level;
  mad_fixed_t temp_level;
  unsigned long n_stat;
} mp3_state;

int c_bitrate (unsigned long head);

struct header makehead (unsigned long headword, struct mp3 mp3f, struct header head, off_t ptr);

off_t findhead (mp3_state *state, off_t start);

off_t findvalidhead (mp3_state *state, off_t start);

int getid3v1 (FILE *file_input);

off_t getid3v2 (FILE *in, off_t start);

char *id3 (char *id, char *title, char *artist, char *album, char *year, unsigned char genre, char *comment, unsigned char track);

unsigned char getgenre (char *s);

void checksync (mp3_state *state);

int xing_info_off(mp3_state *state);

int mp3split (unsigned char *filename, mp3_state *state, char *id3, float fbegin_sec, float fend_sec, short adjustoption, short seekable, float threshold);

mp3_state *mp3info (FILE *file_input, mp3_state *state, int quietoption, int framemode);

void mp3_state_free (mp3_state *state);

int syncerror_split (mp3_state *state, off_t *splitpoints, int quiet);

int mp3_scan_silence (mp3_state *state, off_t begin, unsigned long length, float threshold, float min, short output);

#endif

