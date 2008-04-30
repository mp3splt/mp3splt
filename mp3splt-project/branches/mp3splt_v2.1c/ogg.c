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

/*
 *   NOTE: The major part of the code below is original from:
 *
 *  	vcut 1.6 - (c) 2000-2001 Michael Smith <msmith@labyrinth.net.au>
 *
 *   included in vorbis tools (http://www.xiph.org)
 */

#ifndef NO_OGG

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <ogg/ogg.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#include <locale.h>

#include "ogg.h"
#include "mp3.h"
#include "splt.h"

#if !HAVE_FSEEKO
#define fseeko fseek
#define ftello ftell
#endif

#include <limits.h>

static v_packet *save_packet(ogg_packet *packet)
{
	v_packet *p = malloc(sizeof(v_packet));

	p->length = packet->bytes;
	p->packet = malloc(p->length);
	memcpy(p->packet, packet->packet, p->length);

	return p;
}

static void free_packet(v_packet *p)
{
	if(p)
	{
		if(p->packet)
			free(p->packet);
		free(p);
	}
}

static long get_blocksize(ogg_state *s, vorbis_info *vi, ogg_packet *op)
{
	int this = vorbis_packet_blocksize(vi, op);
	int ret = (this+s->prevW)/4;

	s->prevW = this;
	return ret;
}

static int update_sync(ogg_sync_state *sync_in, FILE *f)
{
	unsigned char *buffer = ogg_sync_buffer(sync_in, BUFSIZE);
	int bytes = fread(buffer,1,BUFSIZE,f);
	ogg_sync_wrote(sync_in, bytes);
	return bytes;
}

/* Returns 0 for success, or -1 on failure. */
static int write_pages_to_file(ogg_stream_state *stream, FILE *file, int flush)
{
	ogg_page page;

	if(flush)
	{
		while(ogg_stream_flush(stream, &page))
		{
			if(fwrite(page.header,1,page.header_len, file) != page.header_len)
				return -1;
			if(fwrite(page.body,1,page.body_len, file) != page.body_len)
				return -1;
		}
	}
	else
	{
		while(ogg_stream_pageout(stream, &page))
		{
			if(fwrite(page.header,1,page.header_len, file) != page.header_len)
				return -1;
			if(fwrite(page.body,1,page.body_len, file) != page.body_len)
				return -1;
		}
	}

	return 0;
}


/* Read stream until we get to the appropriate cut point.
 *
 * We need to do the following:
 *   - Save the final two packets in the stream to temporary buffers.
 *     These two packets then become the first two packets in the 2nd stream
 *     (we need two packets because of the overlap-add nature of vorbis).
 *   - For each packet, buffer it (it could be the 2nd last packet, we don't
 *     know yet (but we could optimise this decision based on known maximum
 *     block sizes, and call get_blocksize(), because this updates internal
 *     state needed for sample-accurate block size calculations.
 */
static int find_begin_cutpoint(ogg_state *s, FILE *in, ogg_int64_t cutpoint)
{
	int eos=0;
	ogg_page page;
	ogg_packet packet;
	ogg_int64_t granpos, prevgranpos;
	int result;

	granpos = prevgranpos = 0;
	
	while(!eos)
	{
		while(!eos)
		{
			int result = ogg_sync_pageout(s->sync_in, &page);
			if(result==0) break;
			else 
			if(result>0)
			{
				granpos = ogg_page_granulepos(&page);
				ogg_stream_pagein(s->stream_in, &page);
				
				if(granpos < cutpoint)
				{
					while(1)
					{
						result=ogg_stream_packetout(s->stream_in, &packet);
						/* throw away result, but update state */
						get_blocksize(s,s->vd->vi,&packet);

						if(result==0) break;
						else 
						if(result!=-1)
						{
							/* We need to save the last packet in the first
							 * stream - but we don't know when we're going
							 * to get there. So we have to keep every packet
							 * just in case.
							 */
							if(s->packets[0])
								free_packet(s->packets[0]);
							s->packets[0] = save_packet(&packet);
						}
					}
					prevgranpos = granpos;
				}
				else
					eos=1; /* First stream ends somewhere in this page.
							  We break of out this loop here. */

				if(ogg_page_eos(&page))
				{
					eos=1;
				}
			}
		}
		if(!eos)
		{
			if(update_sync(s->sync_in, in)==0)
			{
				eos=1;
			}
		}
	}

	/* Now, check to see if we reached a real EOS */
	if(granpos < cutpoint)
		return -1; // Cutpoint is out of file

	while((result = ogg_stream_packetout(s->stream_in, &packet))!=0)
	{
		int bs;

		bs = get_blocksize(s, s->vd->vi, &packet);
		prevgranpos += bs;

		if(prevgranpos > cutpoint)
		{
			s->packets[1] = save_packet(&packet);
			break;
		}
		if(s->packets[0])
			free_packet(s->packets[0]);
		s->packets[0] = save_packet(&packet);
	}

	/* Remaining samples in first packet */
	s->initialgranpos = prevgranpos - cutpoint;
	s->cutpoint_begin = cutpoint;

	return 0;
}

/* Process second stream.
 *
 * We need to do more packet manipulation here, because we need to calculate
 * a new granulepos for every packet, since the old ones are now invalid.
 * Start by placing the modified first and second packets into the stream.
 * Then just proceed through the stream modifying packno and granulepos for
 * each packet, using the granulepos which we track block-by-block.
 */
static int find_end_cutpoint(ogg_state *s, ogg_stream_state *stream, FILE *in, FILE *f, ogg_int64_t cutpoint, short adjust, float threshold)
{
	ogg_packet packet;
	ogg_page page;
	int eos=0;
	int result;
	ogg_int64_t page_granpos = 0, current_granpos = 0, prev_granpos = 0;
	ogg_int64_t packetnum=0; /* Should this start from 0 or 3 ? */

	if(s->packets[0] && s->packets[1]) { // Check if we have the 2 packet, begin can be 0!
		packet.bytes = s->packets[0]->length;
		packet.packet = s->packets[0]->packet;
		packet.b_o_s = 0;
		packet.e_o_s = 0;
		packet.granulepos = 0;
		packet.packetno = packetnum++;
		ogg_stream_packetin(stream,&packet);

		packet.bytes = s->packets[1]->length;
		packet.packet = s->packets[1]->packet;
		packet.b_o_s = 0;
		packet.e_o_s = 0;
		packet.granulepos = s->initialgranpos;
		packet.packetno = packetnum++;
		ogg_stream_packetin(stream,&packet);

		if(ogg_stream_flush(stream, &page)!=0)
		{
			fwrite(page.header,1,page.header_len,f);
			fwrite(page.body,1,page.body_len,f);
		}

		while(ogg_stream_flush(stream, &page)!=0)
		{
			/* Might this happen for _really_ high bitrate modes, if we're
			* spectacularly unlucky? Doubt it, but let's check for it just
			* in case.
			*/
			fprintf(stderr, _("Warning: First audio packet didn't fit into page. File may not decode correctly\n"));
			fwrite(page.header,1,page.header_len,f);
			fwrite(page.body,1,page.body_len,f);
		}
	}
	else s->initialgranpos = 0;

	current_granpos = s->initialgranpos;

	while(!eos)
	{
		while(!eos)
		{
			result=ogg_sync_pageout(s->sync_in, &page);
			if(result==0) break;
			else
			if(result!=-1)
			{
				page_granpos = ogg_page_granulepos(&page) - s->cutpoint_begin;
				if(ogg_page_eos(&page)) eos=1;
				ogg_stream_pagein(s->stream_in, &page);

				if ((cutpoint == 0) || (page_granpos < cutpoint))
				{
					while(1)
					{
						result = ogg_stream_packetout(s->stream_in, &packet);
						if(result==0) break;
						else
						if(result!=-1)
						{
							int bs = get_blocksize(s, s->vd->vi, &packet);
							current_granpos += bs;
							if(s->packets[0]) /* We need to save packet to optimize following split process */
								free_packet(s->packets[0]);
							s->packets[0] = save_packet(&packet);
							if(current_granpos > page_granpos)
								current_granpos = page_granpos;
							packet.granulepos = current_granpos;
							packet.packetno = packetnum++;
							ogg_stream_packetin(stream, &packet);
							if(write_pages_to_file(stream,f, 0))
								return -1;

						}
					}
					prev_granpos = page_granpos;
				}
				else {
					if (adjust) {
						if (ogg_scan_silence(s, (2 * adjust), threshold, 0.f, 0, &page, current_granpos) > 0)
							cutpoint = (ogg_int64_t) (silence_position(s->silence_list, s->off) * s->vd->vi->rate);
						else
							cutpoint = (ogg_int64_t) (cutpoint + (adjust * s->vd->vi->rate));
						ssplit_free(&s->silence_list);
						adjust=0;
					}
					else eos=1; /* We reached the second cutpoint */
				}
				if(ogg_page_eos(&page))
				{
					eos=1;
				}
			}
		}
		if(!eos)
		{
			if(update_sync(s->sync_in, in)==0)
			{
				eos=1;
			}
		}
	}

	if ((cutpoint == 0) || (page_granpos < cutpoint)) // End of file. We stop here
	{
		if(write_pages_to_file(stream,f, 0))
			return -1;
		s->end = -1; // No more data available. Next processes aborted
		return 0;
	}

	while((result = ogg_stream_packetout(s->stream_in, &packet))!=0)
	{
		int bs;
		bs = get_blocksize(s, s->vd->vi, &packet);
		prev_granpos += bs;
		
		if(prev_granpos >= cutpoint)
		{
			s->packets[1] = save_packet(&packet);
			packet.granulepos = cutpoint; /* Set it! This 'truncates' the final packet, as needed. */
			packet.e_o_s = 1;
			ogg_stream_packetin(stream, &packet);
			break;
		}

		if(s->packets[0])
			free_packet(s->packets[0]);
		s->packets[0] = save_packet(&packet);

		ogg_stream_packetin(stream, &packet);
		if(write_pages_to_file(stream,f, 0))
			return -1;
	}

	if(write_pages_to_file(stream,f, 0))
		return -1;

	s->initialgranpos = prev_granpos - cutpoint;
	s->end = 1;
	s->cutpoint_begin += cutpoint;

	return 0;
}

static void submit_headers_to_stream(ogg_stream_state *stream, ogg_state *s)
{
	int i;
	for(i=0;i<3;i++)
	{
		ogg_packet p;
		p.bytes = s->headers[i]->length;
		p.packet = s->headers[i]->packet;
		p.b_o_s = ((i==0)?1:0);
		p.e_o_s = 0;
		p.granulepos=0;

		ogg_stream_packetin(stream, &p);
	}
}

/* Pull out and save the 3 header packets from the input file.
 */

static int process_headers(ogg_state *s)
{
	ogg_page page;
	ogg_packet packet;
	int bytes;
	int i;
	unsigned char *buffer;

	ogg_sync_init(s->sync_in);

	vorbis_info_init(s->vd->vi);
	vorbis_comment_init(&s->vc);

	while (ogg_sync_pageout(s->sync_in, &page)!=1)
	{
		buffer = ogg_sync_buffer(s->sync_in, BUFSIZE);
		bytes = fread(buffer, 1, BUFSIZE, s->in);
		if (bytes <= 0)
		{
			return -1;
		}
		ogg_sync_wrote(s->sync_in, bytes);
	}

	s->serial = ogg_page_serialno(&page);

	ogg_stream_init(s->stream_in, s->serial);

	if(ogg_stream_pagein(s->stream_in, &page) <0)
	{
		return -1;
	}

	if(ogg_stream_packetout(s->stream_in, &packet)!=1){
		return -1;
	}

	if(vorbis_synthesis_headerin(s->vd->vi, &s->vc, &packet)<0)
	{
		return -1;
	}

	s->headers[0] = save_packet(&packet);

	i=0;
	while(i<2)
	{
		while(i<2) {
			int res = ogg_sync_pageout(s->sync_in, &page);
			if(res==0)break;
			if(res==1)
			{
				ogg_stream_pagein(s->stream_in, &page);
				while(i<2)
				{
					res = ogg_stream_packetout(s->stream_in, &packet);
					if(res==0)break;
					if(res<0)
					{
						return -1;
					}
					s->headers[i+1] = save_packet(&packet);
					vorbis_synthesis_headerin(s->vd->vi,&s->vc,&packet);
					i++;
				}
			}
		}
		buffer=ogg_sync_buffer(s->sync_in, BUFSIZE);
		bytes=fread(buffer,1,BUFSIZE,s->in);
		if(bytes==0 && i<2)
		{
			return -1;
		}
		ogg_sync_wrote(s->sync_in, bytes);
	}

	return 0;
}

ogg_state *ogginfo(FILE *in, ogg_state *state)
{
	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);

	state = v_new();

	if (state == NULL) {
		perror("malloc");
		exit(1);
	}

	state->in = in;
	state->end = 0;

	if (state->in != stdin)
	{
		if(ov_open(state->in, &state->vf, NULL, 0) < 0)
			return NULL;
		rewind(state->in);
	}

	/* Read headers in, and save them */
	if(process_headers(state))
		return NULL;

	if (state->in != stdin)
		state->len = (ogg_int64_t) ((state->vd->vi->rate)*(ov_time_total(&state->vf, -1)));

	state->cutpoint_begin = 0;
	vorbis_synthesis_init(state->vd,state->vd->vi);
	vorbis_block_init(state->vd,state->vb);

	srand(time(NULL));

	return state;
}

int oggsplit(unsigned char *filename, ogg_state *state, float sec_begin, float sec_end, short seekable, short adjust, float threshold)
{
	ogg_stream_state stream_out;
	ogg_packet header_comm;
	ogg_int64_t begin, end = 0, cutpoint = 0;

	begin = (ogg_int64_t) (sec_begin * state->vd->vi->rate);

	if (sec_end != -1.f)
	{
		if (sec_begin >= sec_end)
			return -1;
		if (adjust) {
			if (sec_end != -1) {
				float gap = (float) adjust;
				if (sec_end > gap)
					sec_end -= gap;
				if (sec_end < sec_begin)
					sec_end = sec_begin;
			}
			else adjust = 0;
		}
		end = (ogg_int64_t) (sec_end * state->vd->vi->rate);
		cutpoint = end - begin;
	}

	if (state->end == 0) {					// First time we run this, no packets already saved.
		if(find_begin_cutpoint(state, state->in, begin)) 	// We must do this before. If an error occurs, we don't want to create empty files!
			return -1;
	}

	if (strcmp(filename, "-")==0)
		state->out = stdout;
	else
	if (!(state->out=fopen(filename, "wb"))) {
		fprintf (stderr, "\n");
		perror(filename);
		exit(1);
	}

	ogg_stream_init(&stream_out, rand()); /* gets random serial number*/

	vorbis_commentheader_out(&state->vc, &header_comm);

	if (state->headers[1])
		free_packet(state->headers[1]);
	state->headers[1] = save_packet(&header_comm);

	submit_headers_to_stream(&stream_out, state);

	if(write_pages_to_file(&stream_out, state->out, 1))
		return -1;

	if(find_end_cutpoint(state, &stream_out, state->in, state->out, cutpoint, adjust, threshold))
		return -1;

	ogg_stream_clear(&stream_out);

	fclose(state->out);

	if (state->end == -1) return -3; // End of file, no more data available

	return 0;
}

int ogg_silence(ogg_state *state, vorbis_dsp_state *vd, float threshold)
{
	float **pcm, sample;
	int samples, silence = 1;


	while((samples=vorbis_synthesis_pcmout(vd,&pcm))>0)
	{
		if (silence) {
			int i, j;
			for (i=0; i < state->vd->vi->channels; i++) {
				float  *mono=pcm[i];
				if (!silence) break;
				for(j=0; j<samples; j++)
				{
					sample = fabs(mono[j]);
					state->temp_level = state->temp_level *0.999 + sample*0.001;
					if (sample > threshold)
						silence = 0;
				}
			}
		}
		vorbis_synthesis_read(vd, samples);
	}
	return silence;
}

int ogg_scan_silence (ogg_state *state, short seconds, float threshold, float min, short output, ogg_page *page, ogg_int64_t granpos)
{
	ogg_page og;
	ogg_packet op;
	ogg_sync_state oy;
	ogg_stream_state os;
	vorbis_dsp_state vd;
	vorbis_block vb;
	ogg_int64_t end_position, begin_position, pos, end, begin, page_granpos;
	int eos=0, found = 0, shot, result = 0, len = 0 ;
	short first, flush = 0;
	unsigned long count = 0;
	off_t position = ftello(state->in); // Some backups
	int saveW = state->prevW;
	float th = convertfromdB(threshold);

	ogg_sync_init(&oy);
	ogg_stream_init(&os, state->serial);

	if (page){ // We still have a page to process
		memcpy(&og, page, sizeof(ogg_page));
		result = 1;
	}

	end_position = begin_position = pos = granpos;
	vorbis_synthesis_init(&vd, state->vd->vi);
	vorbis_block_init(&vd, &vb);

	if (seconds > 0)
		end = (ogg_int64_t) (seconds * state->vd->vi->rate);
	else end = 0;

	begin = 0;
	first = output;
	shot = DEFAULTSHOT;

	state->temp_level = 0.0;
	state->avg_level = 0.0;
	state->n_stat = 0.0;

	if (output) fprintf(stderr, "[  0 %%] S: 00 Level: -0.0  dB\r");

	while (!eos)
	{
		while(!eos)
		{
			if (result==0) break;
			if(result>0)
			{
				if (ogg_page_eos(&og)) eos=1;
				page_granpos = ogg_page_granulepos(&og) - state->cutpoint_begin;
				if (pos == 0) pos = page_granpos;
				ogg_stream_pagein(&os, &og);
				while(1)
				{
					result=ogg_stream_packetout(&os, &op);
					if(result==0) break; /* need more data */
					if(result>0)
					{
						int bs = get_blocksize(state, state->vd->vi, &op);
						pos += bs;
						if (pos > page_granpos)
							pos = page_granpos;
						begin += bs;
						if(vorbis_synthesis(&vb, &op)==0) {
							vorbis_synthesis_blockin(&vd, &vb);
							if ((!flush) && (ogg_silence(state, &vd, th))) {
								if (len == 0) begin_position = pos;
								if (first == 0) len++;
								if (shot < DEFAULTSHOT)
									shot+=2;
								end_position = pos;
							}
							else {
								if (len > DEFAULTSILLEN) {
									if ((flush) || (shot <= 0))
									{
										float b_position, e_position;
										double temp;
										temp = (double) begin_position;
										temp /= state->vd->vi->rate;
										b_position = (float) temp;
										temp = (double) end_position;
										temp /= state->vd->vi->rate;
										e_position = (float) temp;
										if ((e_position - b_position - min) >= 0.f)
										{
											ssplit_new(&state->silence_list, b_position, e_position, len);
											found++;
										}
										len = 0;
										shot = DEFAULTSHOT;
									}
								} else len = 0;
								if (flush) {
									eos = 1;
									break;
								}
								if ((first) && (shot <= 0))
									first = 0;
								if (shot > 0) shot--;
							}
						}
					}
					if (end)
						if (begin > end)
							flush = 1;
					if (found >= MAXTRACKS) eos = 1;
				}
			}
			result=ogg_sync_pageout(&oy, &og);
		}
		if(!eos){
			if(update_sync(&oy, state->in)==0)
			{
				eos=1;
			}
			result=ogg_sync_pageout(&oy, &og);
			if ((output) && ((count++ > OGG_STAT) && (state->len > 0)))
			{
				float level = convert2dB(state->temp_level);
				state->avg_level += level;
				state->n_stat++;
				fprintf(stderr, "[%3d %%] S: %02d Level: %+.1f\r", (int)(pos/(state->len/100)), found, level);
				count = 0;
			}
		}
	}

	if (output) fprintf(stderr, "[100 %%]\n");

    ogg_stream_clear(&os);

	vorbis_block_clear(&vb);
	vorbis_dsp_clear(&vd);
	ogg_sync_clear(&oy);

	state->prevW = saveW;
	fseeko(state->in, position, SEEK_SET);

	return found;
}

ogg_state *v_new(void)
{
	ogg_state *s;

	if ((s = (ogg_state *) malloc(sizeof(ogg_state)))==NULL)
		return NULL;

	memset(s, 0, sizeof(ogg_state));

	if ((s->sync_in = (ogg_sync_state *) malloc(sizeof(ogg_sync_state)))==NULL)
		return NULL;
	if ((s->stream_in = (ogg_stream_state *) malloc(sizeof(ogg_stream_state)))==NULL)
		return NULL;
	if ((s->vd = (vorbis_dsp_state *) malloc(sizeof(vorbis_dsp_state)))==NULL)
		return NULL;
	if ((s->vd->vi = (vorbis_info *) malloc(sizeof(vorbis_info)))==NULL)
		return NULL;
	if ((s->vb = (vorbis_block *) malloc(sizeof(vorbis_block)))==NULL)
		return NULL;

	if ((s->headers = malloc(sizeof(v_packet)*3))==NULL)
		return NULL;
	memset(s->headers, 0, sizeof(v_packet)*3);
	
	if ((s->packets = malloc(sizeof(v_packet)*2))==NULL)
		return NULL;
	memset(s->packets, 0, sizeof(v_packet)*2);
	
	return s;
}

/* Full cleanup of internal state and vorbis/ogg structures */
void v_free(ogg_state *s)
{
	if(s)
	{
		if(s->packets)
		{
			if(s->packets[0])
				free_packet(s->packets[0]);
			if(s->packets[1])
				free_packet(s->packets[1]);
			free(s->packets);
		}
		if(s->headers)
		{
			int i;
			for(i=0; i < 3; i++)
				if(s->headers[i])
					free_packet(s->headers[i]);
			free(s->headers);
		}

		if(s->vb)
		{
			vorbis_block_clear(s->vb);
			free(s->vb);
		}
		if(s->vd)
		{
			vorbis_dsp_clear(s->vd);
			free(s->vd);
		}
		if(s->stream_in)
		{
			ogg_stream_clear(s->stream_in);
			free(s->stream_in);
		}
		if(s->sync_in)
		{
			ogg_sync_clear(s->sync_in);
			free(s->sync_in);
		}
		ssplit_free(&s->silence_list);
		free(s);
	}
}

static char *checkutf(unsigned char *s) // For the moment we only omit invalid character
{
	int i, j=0;
	for (i=0; i < strlen(s); i++) {
		if (s[i]<0x7F)
			s[j++] = s[i];
	}
	s[j] = '\0';
	return s;
}

vorbis_comment *v_comment (vorbis_comment *vc, char *artist, char *album, char *title, 
								char *tracknum, char *date, char *genre, char *comment)
{
	if (title!=NULL)
		vorbis_comment_add_tag(vc, "title", checkutf(title));
	if (artist!=NULL)
		vorbis_comment_add_tag(vc, "artist", checkutf(artist));
	if (album!=NULL)
		vorbis_comment_add_tag(vc, "album", checkutf(album));
	if (date!=NULL)
		if (strlen(date)>0)
			vorbis_comment_add_tag(vc, "date", date);
	if (genre!=NULL)
		vorbis_comment_add_tag(vc, "genre", genre);
	if (tracknum!=NULL)
		vorbis_comment_add_tag(vc, "tracknumber", tracknum);
	if (comment!=NULL)
		vorbis_comment_add_tag(vc, "", comment);
	
	return vc;
}

#endif
