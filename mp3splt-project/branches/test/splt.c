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
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "mp3.h"
#include "splt.h"

#if !HAVE_FSEEKO
#define fseeko fseek
#define ftello ftell
#endif

float c_seconds (char *s)
{
	int minutes=0, seconds=0, hundredths=0, i;
	float secs;

	for(i=0; i<strlen(s); i++) // Some checking
		if ((s[i]<0x30 || s[i] > 0x39) && (s[i]!='.'))
			return -1.f;

	if (sscanf(s, "%d.%d.%d", &minutes, &seconds, &hundredths)<2)
		return -1.f;

	if ((minutes < 0) || (seconds < 0) || (hundredths < 0))
		return -1.f;

	if ((seconds > 59) || (hundredths > 99))
		return -1.f;

	if (s[strlen(s)-2]=='.')
		hundredths *= 10;

	secs = (float) (hundredths / 100.f);
	secs += (float) (minutes*60 + seconds);

	return secs;
}

int getword (FILE *in, off_t offset, int mode, unsigned long *headw)
{
	int i;
	*headw = 0;

	if (fseeko(in, offset, mode)==-1)
		return -1;
	for (i=0; i<4; i++) {
		if (feof(in)) return -1;
		*headw = *headw << 8;
		*headw |= fgetc(in);
	}
	return 0;
}

off_t flength (FILE *in)
{
	struct stat info;
	if (fstat(fileno(in), &info)==-1)
		return -1;
	return info.st_size;
}

int split (unsigned char *filename, FILE *file_input, off_t begin, off_t end, int xing, unsigned char *xingbuffer, char *id3buffer)
{
	FILE *file_output;
	off_t position;
	unsigned char buffer[READBSIZE];
	int readed;

	if (end != -1)
		if (begin >= end) return -1;

	position = ftello(file_input); // Save current position

	if (fseeko(file_input, begin, SEEK_SET)==-1)
		return -1;

	if (strcmp(filename, "-")==0)
		file_output=stdout;
	else
	if (!(file_output=fopen(filename, "wb+"))) {
		fprintf (stderr, "\n");
		perror(filename);
		exit(1);
	}

	if (xing!=0)
		fwrite(xingbuffer, 1, xing, file_output);

       while (!feof(file_input)) {
		readed = READBSIZE;
 		if (end!=-1) {
			if (begin>=end) break;
			if ((end-begin) < READBSIZE)
			readed = end-begin;
		}
		if ((readed = fread(buffer, 1, readed, file_input))==-1)
			break;
		if (fwrite(buffer, 1, readed, file_output)==-1)
			return -5;
		begin+=readed;
	}

	if (id3buffer!=NULL) {
		if (fseeko(file_output, getid3v1(file_output), SEEK_END)!=-1)
			fwrite(id3buffer, 1, 128, file_output);
	}

	fclose(file_output);

	if (fseeko(file_input, position, SEEK_SET)==-1)
		return -2;

	if (begin!=end)
		return -3;

	return 0;
}

void error (char *s, int erron)
{
	if (strlen(s)>0)
		fprintf (stderr, "Error: %s\n", s);
	exit(erron);
}

char *strtoupper(char *s)
{
	int i;
	char *out = strdup(s);
	if (out == NULL) {
		perror("strdup");
		exit(1);
	}
	for (i=0; i<strlen(s); i++)
		out[i] = toupper(s[i]);
	return out;
}

char *trackstring(int number)
{
	char *track = NULL;
	if (number != 0) {
		int len = 0, i;
		len = ((int) (log10((double) (number)))) + 1;
		if ((track = malloc(len + 1))==NULL)
			return NULL;
		memset(track, 0, len + 1);
		for (i=len-1; i >= 0; i--) {
			track[i] = ((number%10) | 0x30);
			number /= 10;
		}
	}
	return track;
}

void order_splitpoints(float *ssplitpoints, int len)
{
	int i, j;
	float key;
	for (j=1; j < len; j++) {
		key = ssplitpoints[j];
		i = j -1;
		while ((i >= 0) && (ssplitpoints[i] > key)) {
			ssplitpoints[i+1] = ssplitpoints[i];
			i--;
		}
		ssplitpoints[i+1] = key;
	}
}

float silence_position(struct ssplit *temp, float off)
{
	float position = (temp->end_position - temp->begin_position);
	position = temp->begin_position + (position*off);
	return position;
}

void ssplit_free (struct ssplit **silence_list)
{
	struct ssplit *temp, *saved;
	temp = *silence_list;
	while (temp != NULL) {
		saved = temp->next;
		free(temp);
		temp = saved;
	}
	*silence_list = NULL;
}

int ssplit_new(struct ssplit **silence_list, float begin_position, float end_position, int len)
{
	struct ssplit *temp;
	struct ssplit *s_new;

	if ((s_new = (struct ssplit *) malloc(sizeof(struct ssplit)))==NULL)
		return -1;
	s_new->len = len;
	s_new->begin_position = begin_position;
	s_new->end_position = end_position;
	s_new->next = NULL;

	temp = *silence_list;
	if (temp == NULL)
		*silence_list = s_new; // No elements
	else {
		if (temp->len < len) {
				s_new->next = temp;
				*silence_list = s_new;
			}
		else  {
			if (temp->next == NULL)
				temp->next = s_new;
			else {
				while (temp != NULL) {
					if (temp->next != NULL) {
						if (temp->next->len < len) {			/* We build an ordered list by len to keep most probable silence points */
							s_new->next = temp->next;
							temp->next = s_new;
							break;
						}
					}
					else {
							temp->next = s_new;
							break;
					}
					temp = temp->next;
				}
			}
		}
	}

	return 0;
}

 int parse_ssplit_file (struct ssplit **silence_list, FILE *log)
{
	char line[512];
	int found = 0;
	while(fgets(line, 512, log)!=NULL) {
		int len;
		float begin_position, end_position;
		if (sscanf(line, "%f\t%f\t%d", &begin_position, &end_position, &len) == 3) {
			ssplit_new(silence_list, begin_position, end_position, len);
			found++;
		}
	}
	return found;
}

char **rmopt (char **argv, int offset, int tot)
{
	char **first = &argv[1];
	while (offset < tot) {
		*first = argv[offset];
		first++;
		offset++;
	}
	
	return argv;
}

int dot_pos(char *s, int zpad)
{
	int pos = zpad;
	char *p;

	if ((p=strchr(s, '.'))!=NULL)
		zpad = (int) (p-s);
	
	if (zpad > pos)
		pos = zpad;
	
	return pos;
}

char *zero_pad (char *s, char *out, int zpad)
{
	int minutes=0, seconds=0, hundredths=0;
	char format[32];
	
	sscanf(s, "%d.%d.%d", &minutes, &seconds, &hundredths);
	
	if (s[strlen(s)-2]=='.') 
		hundredths *= 10;
		
	sprintf(format, "%%0%dd.%%02d", zpad);
	sprintf(out, format, minutes, seconds);
	if (hundredths > 0)
		sprintf(out, "%s.%02d", out, hundredths);

	return out;
}

char *zero_pad_float (float f, char *out)
{
	int minutes=0, seconds=0, hundredths=0;

	minutes = (int) (f/60);
	seconds = (int) (f);
	seconds %=60;
	hundredths = (int) (f*100);
	hundredths %= 100;

	sprintf(out, "%03d.%02d", minutes, seconds);

	if (hundredths > 0)
		sprintf(out, "%s.%02d", out, hundredths);

	return out;
}

/*
  Will output a string with a sequence of char that can be:
	a -> artist
	b -> album title
	t -> song title
	n -> track number
*/
int parse_outformat(char *s, char format[OUTNUM][MAXOLEN], int cddboption)
{
	char *ptrs, *ptre;
	int i=0, amb=1, len=0;

	for (i=0; i<strlen(s); i++) {
		if (s[i]=='+') s[i]=' ';
		else if (s[i]==VARCHAR) s[i]='%';
	}

	cleanstring(s);

	ptrs = s;
	i=0;
	ptre=strchr(ptrs+1, '%');
	if (s[0]!='%'){
		if (ptre==NULL)
			len=strlen(ptrs);
		else len = ptre-ptrs;
		if (len > MAXOLEN)
			len = MAXOLEN;
		strncpy(format[i++], ptrs, len);
	}
	else ptre=s;
	if (ptre==NULL)
		return 1;
	ptrs = ptre;

	while (((ptre=strchr(ptrs+1, '%'))!=NULL) && (i < OUTNUM))
	{
		char cf = *(ptrs+1);

		len = ptre-ptrs;
		if (len > MAXOLEN)
			len = MAXOLEN;

		switch (cf) {
			case 'a':
						break;
			case 'b':
						break;
			case 't':
						if (!cddboption)
							break;
			case 'n':
						amb = 0;
						break;
			case 'p':
						break;
			default:
					fprintf(stderr, "Error: illegal variable '@%c' in output format. Allowed are: abtnp\n", cf);
					return -1;
		}
		strncpy(format[i++], ptrs, len);
		ptrs = ptre;
	}

	strncpy(format[i], ptrs, strlen(ptrs));

	if ((ptrs[1]=='t') && (cddboption))
		amb =0;

	if (ptrs[1]=='n')
		amb =0;

	return amb;
}

unsigned char *cleanstring (unsigned char *s)
{
	int i, j=0;
	char *copy;
	copy = (char *)strdup(s);
	for (i=0; i<=strlen(copy); i++)
		if ((copy[i]!='\\')&&(copy[i]!='/')&&(copy[i]!='?')&&(copy[i]!='*')&&(copy[i]!=':')&&(copy[i]!='"')&&(copy[i]!='>')&&(copy[i]!='<')&&(copy[i]!='|')&&(copy[i]!='\r'))
			s[j++] = copy[i];
	free(copy);
	for (i=strlen(s)-1; i >= 0; i--) // Trim string. I will never stop to be surprised about cddb strings dirtiness! ;-)
		if (s[i]==' ')
			s[i] = '\0';
		else break;
	return s;
}

int parse_arg(char *arg, float *th, int *gap, int *nt, float *off, int *rm, float *min)
{
	char *ptr;
	int found=0;

	if ((gap!=NULL) && ((ptr=strstr(arg, "gap"))!=NULL))
	{
		if ((ptr=strchr(ptr, '='))!=NULL)
		{
			if (sscanf(ptr+1, "%d", gap)==1)
				found++;
			else fprintf(stderr, "Warning: bad gap argument. It will be ignored!\n");
		}
	}
	if ((th!=NULL) && ((ptr=strstr(arg, "th"))!=NULL))
	{
		if ((ptr=strchr(ptr, '='))!=NULL)
		{
			if (sscanf(ptr+1, "%f", th)==1)
				found++;
			else fprintf(stderr, "Warning: bad threshold argument. It will be ignored!\n");
		}
	}
	if ((nt!=NULL) && ((ptr=strstr(arg, "nt"))!=NULL))
	{
		if ((ptr=strchr(ptr, '='))!=NULL)
		{
			if (sscanf(ptr+1, "%d", nt)==1)
				found++;
			else fprintf(stderr, "Warning: bad tracknumber argument. It will be ignored!\n");
		}
	}
	if (rm!=NULL)
	{
		if ((ptr=strstr(arg, "rm"))!=NULL) {
			found++;
			*rm = 1;
		}
	}
	if ((off!=NULL) && ((ptr=strstr(arg, "off"))!=NULL))
	{
		if ((ptr=strchr(ptr, '='))!=NULL)
		{
			if (sscanf(ptr+1, "%f", off)==1)
				found++;
			else fprintf(stderr, "Warning: bad offset argument. It will be ignored!\n");
		}
	}
	if ((min!=NULL) && ((ptr=strstr(arg, "min"))!=NULL))
	{
		if ((ptr=strchr(ptr, '='))!=NULL)
		{
			if (sscanf(ptr+1, "%f", min)==1)
				found++;
			else fprintf(stderr, "Warning: bad minimum silence length argument. It will be ignored!\n");
		}
	}
	return found;
}

char *check_ext(char *filename, int ogg)
{
	if (strcmp(filename, "-")!=0)
	{
		if (ogg)
		{
			if ((strstr(filename, OGGEXT)==NULL) && (strstr(filename, OGGEXTU)==NULL))
				strncat(filename, OGGEXT, 4);
		}
		else
		{
			if ((strstr(filename, MP3EXT)==NULL) && (strstr(filename, MP3EXTU)==NULL))
				strncat(filename, MP3EXT, 4);
		}
	}
	return filename;
}

float convert2dB(double input)
{
	float level;
	if (input<=0.0)
		level = -96.0;
	else level = 20 * log10(input);
	return level;
}

double convertfromdB(float input)
{
	double amp;
	if (input<-96.0)
		amp = 0.0;
	else amp = pow(10.0, input/20.0);
	return amp;
}
