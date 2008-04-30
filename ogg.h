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

#ifndef NO_OGG

#ifndef _MP3SPLT_OGG_H
#define _MP3SPLT_OGG_H

#ifdef ENABLE_NLS
#include <libintl.h>
#define _(X) gettext(X)
#else
#define _(X) (X)
#define textdomain(X)
#define bindtextdomain(X, Y)
#endif
#ifdef gettext_noop
#define N_(X) gettext_noop(X)
#else
#define N_(X) (X)
#endif

typedef struct {
	int length;
	unsigned char *packet;
} v_packet;

typedef struct {
	ogg_sync_state *sync_in;
	ogg_stream_state *stream_in;
	vorbis_dsp_state *vd;
	vorbis_block *vb;
	int prevW;
	ogg_int64_t initialgranpos;
	ogg_int64_t len;
	ogg_int64_t cutpoint_begin;
	struct ssplit *silence_list;
	unsigned int serial;
	v_packet **packets; /* 2 */
	v_packet **headers; /* 3 */
	OggVorbis_File vf;
	vorbis_comment vc;
	FILE *in,*out;
	short end;
	float off;
	double avg_level;
	float temp_level;
	unsigned long n_stat;
} ogg_state;

ogg_state *ogginfo(FILE *in, ogg_state *state);

int oggsplit(unsigned char *filename, ogg_state *s, float sec_begin, float sec_end, short seekable, short adjust, float threshold);

ogg_state *v_new(void);

void v_free(ogg_state *state);

vorbis_comment *v_comment(vorbis_comment *vc, char *artist, char *album, char *title, char *tracknum, char *date, char *genre, char *comment);

int ogg_scan_silence (ogg_state *state, short seconds, float threshold, float min, short output, ogg_page *page, ogg_int64_t granpos);

#endif

#endif

