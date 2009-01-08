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

#ifndef _MP3SPLT_SPLT_H
#define _MP3SPLT_SPLT_H

#define MAXTRACKS 1000
#define READBSIZE 1024
#define SSPLITLOG "mp3splt.log"
#define DEFAULTSILLEN 10
#define DEFAULTSILOFF 0.8
#define DEFAULTSHOT 25
#define DEFAULTTS -48.0

#define MAXOLEN 255
#define OUTNUM  10

#ifdef _WIN32
#define DIRCHAR '\\'
#define NDIRCHAR '/'
#else
#define DIRCHAR '/'
#define NDIRCHAR '\\'
#endif

#define VARCHAR '@'

#define MP3EXT ".mp3"
#define MP3EXTU ".MP3"
#define OGGEXTU ".OGG"
#define OGGEXT ".ogg"

#define MP3_STAT 200
#define OGG_STAT 50

#define BUFSIZE 4096

struct ssplit {
	float begin_position;
	float end_position;
	int len;
	struct ssplit *next;
};

float c_seconds (char *s);

int getword (FILE *in, off_t offset, int mode, unsigned long *headw);

off_t flength (FILE *in);

int split (unsigned char *filename, FILE *file_input, off_t begin, off_t end, int xing, unsigned char *xingbuffer, char *id3buffer);

void error (char *s,int erron);

char *strtoupper(char *s);

char *trackstring(int number);

void order_splitpoints(float *ssplitpoints, int len);

float silence_position(struct ssplit *temp, float off);

void ssplit_free (struct ssplit **silence_list);

int ssplit_new(struct ssplit **silence_list, float begin_position, float end_position, int len);

int parse_ssplit_file (struct ssplit **silence_list, FILE *log);

char **rmopt (char **argv, int offset, int tot);

int dot_pos(char *s, int zpad);

char *zero_pad (char *s, char *out, int zpad);

char *zero_pad_float (float f, char *out);

int parse_outformat(char *s, char format[OUTNUM][MAXOLEN], int cddboption);

unsigned char *cleanstring (unsigned char *s);

int parse_arg(char *arg, float *th, int *gap, int *nt, float *off, int *rm, float *min);

char *check_ext(char *filename, int ogg);

float convert2dB(double input);

double convertfromdB(float input);

#endif

