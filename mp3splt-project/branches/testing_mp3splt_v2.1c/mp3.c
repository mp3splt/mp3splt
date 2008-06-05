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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#include "splt.h"
#include "mp3.h"

#if !HAVE_FSEEKO
#define fseeko fseek
#define ftello ftell
#endif

const int tabsel_123[2][3][16] = {
  { {128,32,64,96,128,160,192,224,256,288,320,352,384,416,448,},
    {128,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,},
    {128,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320,} },

  { {128,32,48,56,64,80,96,112,128,144,160,176,192,224,256,},
    {128,8,16,24,32,40,48,56,64,80,96,112,128,144,160,},
    {128,8,16,24,32,40,48,56,64,80,96,112,128,144,160,} }
};

const char categories[GENRENUM][10] = {
	{"blues"},
	{"classical"},
	{"country"},
	{"data"},
	{"folk"},
	{"jazz"},
	{"newage"},
	{"reggae"},
	{"rock"},
	{"soundtrack"},
	{"misc"},
};

const char unsigned id3genre[GENRENUM] = {0x00, 0x20, 0x02, 0xFF, 0x50, 0x08, 0x0a, 0x10, 0x11, 0x18, 0xFF};

// Fill out ID3 buffer
char *id3 (char *id, char *title, char *artist, char *album, char *year, unsigned char genre, char *comment, unsigned char track)
{
	char buffer[30];
	int j=3,i;
	strncpy(id, TAG, 3);

	memset(buffer, '\0', 30);
	if (title!=NULL) strncpy(buffer, title, 30);
	for (i=0; i<30; i++) id[j++]=buffer[i];

	memset(buffer, '\0', 30);
	if (artist!=NULL) strncpy(buffer, artist, 30);
	for (i=0; i<30; i++) id[j++]=buffer[i];

	memset(buffer, '\0', 30);
	if (album!=NULL) strncpy(buffer, album, 30);
	for (i=0; i<30; i++) id[j++]=buffer[i];

	memset(buffer, '\0', 30);
	if (year!=NULL) strncpy(buffer, year, 4);
	for (i=0; i<4; i++) id[j++]=buffer[i];

	memset(buffer, '\0', 30);
	if (comment!=NULL) strncpy(buffer, comment, 30);
	for (i=0; i<30; i++) id[j++]=buffer[i];
	if (track!=0x00)
		id[j-1] = (char) track;
	id[j]=(char) genre;

	return id;
}

unsigned char getgenre (char *s)
{
	int i;
	for (i=0; i<GENRENUM; i++) {
		if (strncmp(s, categories[i], strlen(s))==0)
			return id3genre[i];
	}
	return 0xFF;
}

void checksync (mp3_state *state)
{
	char junk[32];
	fprintf(stderr, "\nWarning: Too many sync errors! This may not be a mp3 file. Continue? (y/n) ");
	fgets(junk, 31, stdin);
	if (junk[0]=='y')
		state->syncdetect = 0;
	else error("Aborted.",125);
}

// Calculates bitrate
int c_bitrate (unsigned long head)
{
    if ((head & 0xffe00000) != 0xffe00000) return 0;
    if (!((head>>17)&3)) return 0;
    if (((head>>12)&0xf) == 0xf) return 0;
    if (!((head >> 12) & 0xf)) return 0;
    if (((head>>10)&0x3) == 0x3 ) return 0;
    if (((head >> 19) & 1)==1 && ((head>>17)&3)==3 && ((head>>16)&1)==1) return 0;
    if ((head & 0xffff0000) == 0xfffe0000) return 0;

    return ((head>>12)&0xf);
}

struct header makehead (unsigned long headword, struct mp3 mp3f, struct header head, off_t ptr)
{
	head.ptr = ptr;
	head.bitrate = tabsel_123[1 - mp3f.mpgid][mp3f.layer-1][c_bitrate(headword)];
	head.padding = ((headword>>9)&0x1);
	head.framesize = (head.bitrate*144000)/(mp3f.freq<<(1 - mp3f.mpgid)) + head.padding;
	return head;
}

// Finds first header from start_pos. Returns -1 if no header is found
off_t findhead (mp3_state *state, off_t start)
{
	if (getword(state->file_input, start, SEEK_SET, &state->headw) == -1)
		return -1;
	if (feof(state->file_input)) return -1;
	while (!(c_bitrate(state->headw))) {
		if (feof(state->file_input)) return -1;
		state->headw <<= 8;
		state->headw |= fgetc(state->file_input);
		start++;
	}
	return start;
}

// Finds first valid header from start. Will work with high probabilty, i hope :)
off_t findvalidhead (mp3_state *state, off_t start)
{
	off_t begin;
	struct header h;

	begin = findhead(state, start);
	do {
		start = begin;
		if (start == -1) break;
		h = makehead (state->headw, state->mp3file, h, start);
		begin = findhead(state, (start + 1));
	} while (begin!=(start + h.framesize));

	return start;
}

int getid3v1 (FILE *file_input)
{
	if (fseeko(file_input, (off_t) -128, SEEK_END)==-1)
		return 0;
	if (fgetc(file_input)=='T')
	   if (fgetc(file_input)=='A')
	      if (fgetc(file_input)=='G')
			return -128;
	return 0;
}

// Check if there is a ID3v2. If found, it returns offset of mp3 data.
off_t getid3v2 (FILE *in, off_t start)
{
	unsigned long oword = 0;
	if (fseeko(in, start, SEEK_SET)==-1)
		return 0;
	if (fgetc(in)=='I')
		if (fgetc(in)=='D')
			if (fgetc(in)=='3') {
				int i;
				if (fseeko(in, (off_t) 3, SEEK_CUR)==-1)
					return 0;
				for (i=0; i<4; i++)
					oword = (oword << 7) | fgetc(in);
				return (off_t) (oword);
			}

	return 0;
}

int xing_info_off(mp3_state *state)
{
	unsigned long headw = 0;
	int i;

	for (i=0; i<state->mp3file.xing; i++)
	{
		if ((headw==XING_MAGIC) || (headw==INFO_MAGIC)) // "Xing" or "Info"
			return i;
		headw <<= 8;
		headw |= state->mp3file.xingbuffer[i];
	}

	return 0;
}

int get_frame(mp3_state *state)
{
	if(state->stream.buffer==NULL || state->stream.error==MAD_ERROR_BUFLEN)
	{
		size_t readSize, remaining;
		unsigned char *readStart;

		if (feof(state->file_input))
			return -2;

		if(state->stream.next_frame!=NULL)
		{
			remaining = state->stream.bufend - state->stream.next_frame;
			memmove(state->inputBuffer, state->stream.next_frame, remaining);
			readStart = state->inputBuffer + remaining;
			readSize = MAD_BSIZE - remaining;
		}
		else {
			readSize=MAD_BSIZE;
			readStart=state->inputBuffer;
			remaining=0;
		}
		readSize=fread(readStart, 1, readSize, state->file_input);
		if (readSize <= 0)
			return -2;
		state->buf_len = readSize + remaining;
		state->bytes += readSize;
		mad_stream_buffer(&state->stream, state->inputBuffer, readSize+remaining);
		state->stream.error = MAD_ERROR_NONE;
	}

	return mad_frame_decode(&state->frame,&state->stream);
}

int get_valid_frame(mp3_state *state)
{
	int ok = 0;
	do
	{
		int ret = get_frame(state);
		if(ret)
		{
			if (ret == -2)
				return -1;
			if (state->stream.error == MAD_ERROR_LOSTSYNC)
			{
				state->syncerrors++;
				if ((state->syncdetect)&&(state->syncerrors>MAXSYNC))
					checksync(state);
			}
			if(MAD_RECOVERABLE(state->stream.error))
				continue;
			else
				if(state->stream.error==MAD_ERROR_BUFLEN)
					continue;
				else
					break;
		}
		else
		{
			state->data_ptr = (unsigned char *) state->stream.this_frame;
			if(state->stream.next_frame!=NULL)
				state->data_len = (int) (state->stream.next_frame - state->stream.this_frame);
			ok = 1;
		}

	} while (!ok);

	return ok;
}

mp3_state *mp3info(FILE *file_input, mp3_state *state, int quietoption, int framemode)
{
	int prev = -1, len;

	if ((state = (mp3_state *) malloc (sizeof(mp3_state)))==NULL) {
		perror("malloc");
		exit(1);
	}

	memset(state, 0x0, sizeof(mp3_state));

	if (quietoption)
		state->syncdetect = 0;
	else state->syncdetect = 1;
	state->frames = 1;
	state->end = 0;
	state->first = 1;
	state->syncerrors = 0;
	state->file_input = file_input;
	state->framemode = framemode;
	state->headw = 0;
	state->mp3file.xing = 0;
	state->mp3file.xing_offset = 0;
	state->mp3file.xingbuffer = NULL;
	state->mp3file.len = flength(file_input);
	state->total_time = 0;
	state->data_ptr = NULL;
	state->data_len = 0;
	state->buf_len = 0;
	state->bytes = 0;

	state->silence_list = NULL;

	mad_stream_init(&state->stream);
	mad_frame_init(&state->frame);
	mad_synth_init(&state->synth);
	mad_timer_reset(&state->timer);

	do
	{
		int ret = get_frame(state);

		if (ret==-2)
			return NULL;

		if ((prev == 0) && ((ret == 0) || (state->stream.error==MAD_ERROR_BUFLEN)))
			break;

		if (ret == 0)
		{
			state->data_ptr = (unsigned char *) state->stream.this_frame;
			if(state->stream.next_frame!=NULL)
				state->data_len = (int) (state->stream.next_frame - state->stream.this_frame);
			if (state->stream.anc_bitlen > 64)
			{
				int tag = 0;
				struct mad_bitptr ptr = state->stream.anc_ptr;
				struct mad_bitptr start = ptr;
				unsigned long xing_word = mad_bit_read(&ptr, 32);
				if ((xing_word==XING_MAGIC) || (xing_word==INFO_MAGIC))
					tag = 1;
				/* Handle misplaced Xing header in mp3 files with CRC */
				else if (xing_word == ((XING_MAGIC << 16) & 0xffffffffL) || xing_word == ((INFO_MAGIC << 16) & 0xffffffffL))
				{
					ptr = start;
					mad_bit_skip(&ptr, 16);
					tag = 1;
				}
				if (tag)
				{
					xing_word = mad_bit_read(&ptr, 32);
					if (xing_word & XING_FRAMES)
					{
						mad_timer_t total;
						state->frames = mad_bit_read(&ptr, 32);
						total = state->frame.header.duration;
						mad_timer_multiply(&total, state->frames);
						state->total_time = mad_timer_count(total, MAD_UNITS_SECONDS);
					}
					if (xing_word & XING_BYTES) {
						if (state->mp3file.len == 0)
							state->mp3file.len = mad_bit_read(&ptr, 32);
					}
					state->mp3file.xing = state->data_len;
					if ((state->mp3file.xingbuffer = (unsigned char *) malloc(state->mp3file.xing))==NULL) {
						perror("malloc");
						exit(1);
					}
					memcpy(state->mp3file.xingbuffer, state->data_ptr, state->mp3file.xing);
					state->mp3file.xing_offset = xing_info_off(state);
					if (!quietoption && !state->framemode)
						fprintf (stderr, "Warning: found Xing or Info header, mp3 may be VBR. Switching to Frame mode...\n");
					state->framemode = 1;
					continue;
				}
			}
		}

		prev = ret;

	} while (1);

	len = (int) (state->buf_len - (state->data_ptr - state->inputBuffer));

	if (len < 0)
		return NULL;

	state->mp3file.firsth = (off_t) (state->bytes - len);
	state->bytes = state->mp3file.firsth;
	state->headw = (unsigned long) ((state->data_ptr[0] << 24) | (state->data_ptr[1] << 16) | (state->data_ptr[2] << 8) | (state->data_ptr[3]));
	state->mp3file.mpgid = (int) ((state->headw >> 19)&1);
	state->mp3file.layer = state->frame.header.layer;
	state->mp3file.freq = state->frame.header.samplerate;
	state->mp3file.bitrate = state->frame.header.bitrate/BYTE;
	state->mp3file.firsthead = makehead(state->headw, state->mp3file, state->mp3file.firsthead, state->mp3file.firsth);
	state->mp3file.fps = (float) (state->mp3file.freq*(2-state->mp3file.mpgid));
	state->mp3file.fps /= PCM;

	switch(state->frame.header.mode)
	{
		case MAD_MODE_SINGLE_CHANNEL:
		state->mp3file.channels = 0;
		break;
		case MAD_MODE_DUAL_CHANNEL:
		state->mp3file.channels = 1;
		break;
		case MAD_MODE_JOINT_STEREO:
		state->mp3file.channels = 2;
		break;
		case MAD_MODE_STEREO:
		state->mp3file.channels = 3;
		break;
		default:
		state->mp3file.channels = 4;
		break;
	}

	if (!state->framemode)
	{
		if (state->mp3file.len > 0)
			state->total_time = (unsigned long) ((state->mp3file.len - state->mp3file.firsth) / state->mp3file.bitrate);
	}

	return state;
}

int mp3split (unsigned char *filename, mp3_state *state, char *id3, float fbegin_sec, float fend_sec, short adjustoption, short seekable, float threshold)
{
	short status = -1, eof=0, check_bitrate=0;

	if (!seekable)
	{
		FILE *file_output = NULL;
		short writing = 0, finished=0;
		unsigned long fbegin=0;
		off_t wrote = 0;
		int len = 0;

		if (strcmp(filename, "-")==0)
			file_output=stdout;
		else
		if (!(file_output=fopen(filename, "wb+"))) {
			fprintf (stderr, "\n");
			perror(filename);
			exit(1);
		}

		if (state->framemode)
		{
			unsigned long begin_c, end_c, time;
			begin_c = (unsigned long) (fbegin_sec * 100.f);
			if (fend_sec > 0)
				end_c = (unsigned long) (fend_sec * 100.f);
			else end_c = 0;
			time = 0;

			do
			{
				if (!writing && (time >= begin_c))
				{
					writing = 1;
					fbegin = state->frames;

					if (state->mp3file.xing > 0)
					{
						wrote = fwrite(state->mp3file.xingbuffer, 1, state->mp3file.xing, file_output);
						if (wrote <= 0)
							return -5;
					}
				}
				if (writing)
				{
					if (state->data_len > 0)
					{
						len = fwrite(state->data_ptr, 1, state->data_len, file_output);
						if (len <= 0)
							return -5;
						wrote = (off_t) (wrote + len);
						state->data_len = 0;
					}
					if ((end_c > 0) && (time > end_c))
						finished = 1;
					if (eof || finished)
					{
						finished = 1;
						status = 0;
						break;
					}
				}
				switch (get_valid_frame(state))
				{
					case 1:
						mad_timer_add(&state->timer, state->frame.header.duration);
						state->frames++;
						time = mad_timer_count(state->timer, MAD_UNITS_CENTISECONDS);
						break;
					case 0:
						break;
					case -1:
						eof = 1;
						break;
				}
			} while (!finished);
		}
		else
		{
			off_t begin = 0, end = 0;
			if (fend_sec != -1)
				end = (off_t) (fend_sec * state->mp3file.bitrate + state->mp3file.firsth);

			if (state->end == 0)
			{
				begin = (off_t) (fbegin_sec * state->mp3file.bitrate + state->mp3file.firsth);

				if ((state->bytes == begin) && (state->data_len > 0))
				{
					len = (int) (state->inputBuffer + state->buf_len - state->data_ptr);
					if (len < 0)
						return -6;
					if (fwrite(state->data_ptr, 1, len, file_output) < 0)
						return -5;
					wrote = (off_t) (wrote + len);
					state->data_len = 0;
				}
				else
				{
					while (state->bytes < begin)
					{
						off_t to_read;
						if (feof(state->file_input))
							return -1;
						to_read = (begin - state->bytes);
						if (to_read > MAD_BSIZE)
							to_read = MAD_BSIZE;
						if ((state->data_len = fread(state->inputBuffer, 1, to_read, state->file_input))<=0)
							return -1;
						state->bytes+=state->data_len;
					}
					mad_stream_init(&state->stream);
					mad_frame_init(&state->frame);
					switch (get_valid_frame(state))
					{
						case 1:
							len = (int) (state->inputBuffer + state->buf_len - state->data_ptr);
							if (len < 0)
								return -6;
							if (fwrite(state->data_ptr, 1, len, file_output) < 0)
								return -5;
							wrote = (off_t) (wrote + len);
							state->data_len = 0;
							break;
						case 0:
							break;
						case -1:
							eof = 1;
							break;
					}
				}
			}
			else
			{
				len = (int) (state->inputBuffer + state->buf_len - state->data_ptr);
				if (len < 0)
					return -6;
				if (fwrite(state->data_ptr, 1, len, file_output) < 0)
					return -5;
				wrote = (off_t) (wrote + len);
				state->data_len = 0;
				begin = state->end;
			}

			while (!eof)
			{
				off_t to_read = MAD_BSIZE;
				if (end > 0)
				{
					to_read = (end - state->bytes);
					if (to_read <= 0)
					{
						status = 0;
						break;
					}
					if (to_read > MAD_BSIZE)
						to_read = MAD_BSIZE;
				}
				if (feof(state->file_input) || ((state->data_len = fread(state->inputBuffer, 1, to_read, state->file_input))<=0))
				{
					eof = 1;
					status = 0;
					break;
				}
				if (fwrite(state->inputBuffer, 1, state->data_len, file_output)<0)
				{
					status = -5;
					break;
				}
				state->bytes+=state->data_len;
			}

			state->end = end;

			if (!eof)
			{
				mad_stream_init(&state->stream);
				mad_frame_init(&state->frame);
				switch (get_valid_frame(state))
				{
					case 1:
						len = (int) (state->data_ptr - state->inputBuffer);
						if (len < 0)
							return -6;
						if (fwrite(state->inputBuffer, 1, len, file_output) < 0)
							return -5;
						break;
					case 0:
						break;
					case -1:
						eof = 1;
						break;
				}
			}

		}

		if (file_output)
		{
			if (id3)
			{
				fseeko(file_output, getid3v1(file_output), SEEK_END);
				fwrite(id3, 1, 128, file_output);
			}
			if (state->mp3file.xing > 0)
			{
				if (fseeko(file_output, state->mp3file.xing_offset+4, SEEK_SET)!=-1)
				{
					unsigned long headw = (unsigned long) (state->frames - fbegin + 1); // Frames
					fputc((headw >> 24) & 0xFF, file_output);
					fputc((headw >> 16) & 0xFF, file_output);
					fputc((headw >> 8) & 0xFF, file_output);
					fputc((headw >> 0) & 0xFF, file_output);
					headw = (unsigned long) (wrote); // Bytes
					fputc((headw >> 24) & 0xFF, file_output);
					fputc((headw >> 16) & 0xFF, file_output);
					fputc((headw >> 8) & 0xFF, file_output);
					fputc((headw >> 0) & 0xFF, file_output);
				}
				else fprintf(stderr, "Warning: can't seek outputfile to write Xing informations, they may be wrong.\n");
			}
			fclose(file_output);
		}
	}
	else
	{
		off_t begin = 0, end = -1;
 		if (state->framemode)
		{
			unsigned long fbegin, fend, adjust;
			fbegin = fend = adjust = 0;
			fbegin = fbegin_sec * state->mp3file.fps;
			if (fend_sec != -1)
			{
				if (adjustoption) {
					if (fend_sec != -1) {
						float adj = (float) (adjustoption);
						float len = (fend_sec - fbegin_sec);
						if (adj > len)
							adj = len;
						if (fend_sec > adj)
							fend_sec -= adj;
						adjust = (unsigned long) (adj * 100.f);
					}
					else adjust=0;
				}
				fend = fend_sec * state->mp3file.fps;
			}
			else fend = 0xFFFFFFFF;

			if (state->end == 0) {
				if (state->first)
				{
					state->h.ptr = state->mp3file.firsthead.ptr;
					state->h.framesize = state->mp3file.firsthead.framesize;
					begin = state->mp3file.firsthead.ptr;
					state->first = 0;
				}
				// Finds begin by counting frames
				while (state->frames < fbegin) {
					begin = findhead(state, state->h.ptr + state->h.framesize);
					if (begin==-1)
						return -1;
					if ((begin!=state->h.ptr + state->h.framesize)&&(state->syncerrors>=0)) state->syncerrors++;
					if ((state->syncdetect)&&(state->syncerrors>MAXSYNC))
						checksync(state);
					state->h = makehead (state->headw, state->mp3file, state->h, begin);
          state->frames++;
				}
			}
			else
      {
				begin = state->end;
        fprintf(stdout,"\n\nbegin = %ld\n",begin);
        fflush(stdout);
      }

			if (state->mp3file.len > 0)
				if (begin >= state->mp3file.len) // If we can check, we just do that :)
					return -1;

			// Finds end by counting frames
			while (state->frames <= fend) {
        state->frames++;
				end = findhead(state, state->h.ptr + state->h.framesize);
				if (end == -1) {
					end = state->h.ptr + state->h.framesize; // Last valid offset
					eof=1;
					break;
				}
        fprintf(stdout,"alex frame = %d\n",state->frames);
        fprintf(stdout,"alex_end = %ld\n",end);
        fflush(stdout);

				if ((end!=state->h.ptr + state->h.framesize)&&(state->syncerrors>=0)) state->syncerrors++;
				if ((state->syncdetect)&&(state->syncerrors>MAXSYNC))
					checksync(state);
				state->h = makehead (state->headw, state->mp3file, state->h, end);
				if ((adjust) && (state->frames >= fend)) {
					if (mp3_scan_silence(state, end, 2 * adjust, threshold, 0.f, 0) > 0)
						adjust = (unsigned long) (silence_position(state->silence_list, state->off) * state->mp3file.fps);
					else
						adjust = (unsigned long) (adjustoption * state->mp3file.fps);
					fend += adjust;
					end = findhead(state, end);
					ssplit_free(&state->silence_list);
					adjust=0;
				}
			}

			state->end = end + state->h.framesize;
      fprintf(stdout,"alex = %ld\n", end);
      fprintf(stdout,"alex = %ld\n", state->h.framesize);
      fprintf(stdout,"alex = %ld\n", state->end);
      fflush(stdout);

			if (state->mp3file.xing > 0) {
				unsigned long headw;
				headw = (unsigned long) (state->frames - fbegin + 1); // Frames
				state->mp3file.xingbuffer[state->mp3file.xing_offset+4] = (headw >> 24) & 0xFF;
				state->mp3file.xingbuffer[state->mp3file.xing_offset+5] = (headw >> 16) & 0xFF;
				state->mp3file.xingbuffer[state->mp3file.xing_offset+6] = (headw >> 8) & 0xFF;
				state->mp3file.xingbuffer[state->mp3file.xing_offset+7] = headw  & 0xFF;
				if (end == -1)
					end = state->mp3file.len;
				headw = (unsigned long) (end - begin + state->mp3file.xing); // Bytes
				state->mp3file.xingbuffer[state->mp3file.xing_offset+8] = (headw >> 24) & 0xFF;
				state->mp3file.xingbuffer[state->mp3file.xing_offset+9] = (headw >> 16) & 0xFF;
				state->mp3file.xingbuffer[state->mp3file.xing_offset+10] = (headw >> 8) & 0xFF;
				state->mp3file.xingbuffer[state->mp3file.xing_offset+11] = headw  & 0xFF;
			}
		}
		else
		{
			// If file is CBR we can trust bitrate to find position
			if (state->end == 0) {
				begin = (off_t) (fbegin_sec * state->mp3file.bitrate + state->mp3file.firsth);
				// Finds first valid header. Mantain clean files.
				begin = findvalidhead (state, begin);
				if (begin==-1)
					return -1;
				if (tabsel_123[1 - state->mp3file.mpgid][state->mp3file.layer-1][c_bitrate(state->headw)] != state->mp3file.firsthead.bitrate)
					check_bitrate = 1;
			}
			else begin = state->end;
			if (fend_sec != -1) {
				end = (off_t) (fend_sec * state->mp3file.bitrate + state->mp3file.firsth);
				end = findvalidhead (state, end); // We take the complete frame
				if (tabsel_123[1 - state->mp3file.mpgid][state->mp3file.layer-1][c_bitrate(state->headw)] != state->mp3file.firsthead.bitrate)
					check_bitrate = 1;
			}
			state->end = end;
		}
		status = split (filename, state->file_input, begin, end, state->mp3file.xing, state->mp3file.xingbuffer, id3);
	}

	if (check_bitrate)
		return -4;

	if (status== 0 && eof) return -3;

	return status;
}

off_t adjustsync(mp3_state *state, off_t begin, off_t end)
{
	off_t position;
	position = begin;
	if (fseeko(state->file_input, position, SEEK_SET)==-1)
		return (off_t) (-1);

	while (position++ < end) { // First we search for ID3v1
		if (fgetc(state->file_input)=='T') {
			if (fgetc(state->file_input)=='A') {
				if (fgetc(state->file_input)=='G')
					return (position + 127);
				else position++;
			}
			fseeko(state->file_input, -1, SEEK_CUR);
		}
	}

	position = begin;
	if (fseeko(state->file_input, position, SEEK_SET)==-1)
		return (off_t) (-1);

	while (position++ < end) { // Now we search for ID3v2
		if (fgetc(state->file_input)=='I') {
			if (fgetc(state->file_input)=='D') {
				if (fgetc(state->file_input)=='3')
					return (position - 1);
				else position++;
			}
			fseeko(state->file_input, -1, SEEK_CUR);
		}
	}

	return end;
}

int syncerror_split (mp3_state *state, off_t *splitpoints, int quiet)
{
	off_t offset = 0;
	char junk[32];

	state->h.ptr = state->mp3file.firsthead.ptr;
	state->h.framesize = state->mp3file.firsthead.framesize;
	splitpoints[0] = 0;

	fprintf(stderr, "Processing file to detect possible split points, please wait...\n");

	while (state->syncerrors < MAXTRACKS) {
		offset = findhead(state, state->h.ptr + state->h.framesize);
		if (offset==-1)
			break;
		if (offset!=state->h.ptr + state->h.framesize) {
			state->syncerrors++;
			splitpoints[state->syncerrors] = adjustsync(state, state->h.ptr, offset);
			if (splitpoints[state->syncerrors] == -1)
				return -1;
			offset = findvalidhead(state, splitpoints[state->syncerrors]);
			if (getword(state->file_input, offset, SEEK_SET, &state->headw) == -1)
				return -1;
		}
		state->h = makehead (state->headw, state->mp3file, state->h, offset);
	}

	if (state->syncerrors == 0)
		return 1;
	if (state->syncerrors == MAXTRACKS)
		return 2;

	splitpoints[++state->syncerrors] = -1;

	fprintf(stderr, "Total tracks found: %ld\n", state->syncerrors);
	if (!quiet) {
		fprintf(stderr, "Is this a reasonable number of tracks for this file? (y/n) ");
		fgets(junk, 31, stdin);
		if (junk[0]=='n')
			return 3;
	}

	return 0;
}

int mp3_silence(mp3_state *state, int channels, mad_fixed_t threshold)
{
	int i, j;
	mad_fixed_t sample;

	for (j=0; j<channels; j++) {
		for(i=0; i<state->synth.pcm.length; i++)
		{
			sample = mad_f_abs(state->synth.pcm.samples[j][i]);
			state->temp_level = state->temp_level *0.999 + sample*0.001;
			if (sample > threshold)
				return 0;
		}
	}
	return 1;
}

int mp3_scan_silence (mp3_state *state, off_t begin, unsigned long length, float threshold, float min, short output)
{
	int len = 0, found = 0, shot;
	short first, flush = 0, stop = 0;
	unsigned long count=0, silence_begin = 0, silence_end = 0, time;
	off_t pos;
	mad_fixed_t th;

	pos = begin;
	th = mad_f_tofixed(convertfromdB(threshold));

	if (fseeko(state->file_input, begin, SEEK_SET)==-1)
		return -1;

	first = output;
	shot = DEFAULTSHOT;

	mad_stream_init(&state->stream);
	mad_frame_init(&state->frame);
	mad_synth_init(&state->synth);
	mad_timer_reset(&state->timer);
	state->temp_level = 0.0;
	state->avg_level = 0.0;
	state->n_stat = 0;

	if (output) fprintf(stderr, "[  0 %%] S: 00 Level: -0.0  dB\r");

	do
	{
		switch (get_valid_frame(state))
		{
			case 1:
					mad_timer_add(&state->timer, state->frame.header.duration);
					mad_synth_frame(&state->synth,&state->frame);
					time = mad_timer_count(state->timer, MAD_UNITS_CENTISECONDS);
					if (length > 0)
						if (time >= length)
						{
							flush = 1;
							stop = 1;
						}

					if ((!flush) && (mp3_silence(state, MAD_NCHANNELS(&state->frame.header), th))) {
						if (len == 0) silence_begin = time;
						if (first == 0) len++;
						if (shot < DEFAULTSHOT)
							shot+=2;
						silence_end = time;
					}
					else {
						if (len > DEFAULTSILLEN) {
							if ((flush) || (shot <= 0))
							{
								float begin_position, end_position;
								begin_position = (float) (silence_begin / 100.f);
								end_position = (float) (silence_end / 100.f);
								if ((end_position - begin_position - min) >= 0.f)
								{
									ssplit_new(&state->silence_list, begin_position, end_position, len);
									found++;
								}
								len = 0;
								shot = DEFAULTSHOT;
							}
						}
						else len = 0;
						if ((first) && (shot <= 0))
							first = 0;
						if (shot > 0) shot--;
					}

					if ((output) && (state->mp3file.len > 0))
					{
						pos = ftello(state->file_input);
						if (count++ > MP3_STAT) {
							float level = convert2dB(mad_f_todouble(state->temp_level));
							state->avg_level += level;
							state->n_stat++;
							fprintf(stderr, "[%3d %%] S: %02d Level: %+.1f\r", (int)(pos/(state->mp3file.len/100)), found, level);
							count = 0;
						}
					}
					break;;
			case 0:
				break;
			case -1:
				stop = 1;
				break;
		}
	} while (!stop && (found < MAXTRACKS));

	if (output) fprintf(stderr, "[100 %%]\n");

	mad_synth_finish(&state->synth);
	mad_frame_finish(&state->frame);
	mad_stream_finish(&state->stream);

	return found;
}

void mp3_state_free (mp3_state *state)
{
	if (state->mp3file.xingbuffer)
		free(state->mp3file.xingbuffer);
	ssplit_free(&state->silence_list);
	free(state);
}
