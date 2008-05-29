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

#ifndef NO_OGG
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>
#endif

#include "getopt.h"

#include "mp3.h"
#include "wrap.h"
#include "ogg.h"
#include "splt.h"
#include "mp3splt.h"
#include "cddb.h"

const char *chan[] = {
	"Mono",
	"Dual Mono",
	"Joint Stereo",
	"Stereo",
	"?"
};

const char *genre_list[] = {
"Blues",                 "Classic Rock",          "Country",
"Dance",                 "Disco",                 "Funk",
"Grunge",                "Hip-Hop",               "Jazz",
"Metal",                 "New Age",               "Oldies",
"Other",                 "Pop",                   "R&B",
"Rap",                   "Reggae",                "Rock",
"Techno",                "Industrial",            "Alternative",
"Ska",                   "Death Metal",           "Pranks",
"Soundtrack",            "Euro-Techno",           "Ambient",
"Trip-Hop",              "Vocal",                 "Jazz+Funk",
"Fusion",                "Trance",                "Classical",
"Instrumental",          "Acid",                  "House",
"Game",                  "Sound Clip",            "Gospel",
"Noise",                 "Alt. Rock",             "Bass",
"Soul",                  "Punk",                  "Space",
"Meditative",            "Instrum. Pop",          "Instrum. Rock",
"Ethnic",                "Gothic",                "Darkwave",
"Techno-Indust.",        "Electronic",            "Pop-Folk",
"Eurodance",             "Dream",                 "Southern Rock",
"Comedy",                "Cult",                  "Gangsta",
"Top 40",                "Christian Rap",         "Pop/Funk",
"Jungle",                "Native American",       "Cabaret",
"New Wave",              "Psychadelic",           "Rave",
"Showtunes",             "Trailer",               "Lo-Fi",
"Tribal",                "Acid Punk",             "Acid Jazz",
"Polka",                 "Retro",                 "Musical",
"Rock & Roll",           "Hard Rock",             "Folk",
"Folk/Rock",             "National Folk",         "Swing",
"Fusion",                "Bebob",                 "Latin",
"Revival",               "Celtic",                "Bluegrass",
"Avantgarde",            "Gothic Rock",           "Progress. Rock",
"Psychadel. Rock",       "Symphonic Rock",        "Slow Rock",
"Big Band",              "Chorus",                "Easy Listening",
"Acoustic",              "Humour",                "Speech",
"Chanson",               "Opera",                 "Chamber Music",
"Sonata",                "Symphony",              "Booty Bass",
"Primus",                "Porn Groove",           "Satire",
"Slow Jam",              "Club",                  "Tango",
"Samba",                 "Folklore",              "Ballad",
"Power Ballad",          "Rhythmic Soul",         "Freestyle",
"Duet",                  "Punk Rock",             "Drum Solo",
"A Capella",             "Euro-House",            "Dance Hall",
"Goa",                   "Drum & Bass",           "Club-House",
"Hardcore",              "Terror",                "Indie",
"BritPop",               "Negerpunk",             "Polsk Punk",
"Beat",                  "Christian Gangsta Rap", "Heavy Metal",
"Black Metal",           "Crossover",             "Contemporary Christian",
"Christian Rock",        "Merengue",              "Salsa",
"Trash Metal",           "Anime",                 "Jpop",
"Synthpop"};

splt_state *splt_state_reset_mo(splt_state *state)
{
	if (state)
	{
		if (state->mstate) {
			fclose(state->mstate->file_input);
			mp3_state_free(state->mstate);
		}
#ifndef NO_OGG
		else if (state->ostate) { // If file has been opened with ov_open we should close it using ov_clear()
			ov_clear(&state->ostate->vf);
			v_free(state->ostate);
		}
#endif
}
	return state;
}

splt_state *splt_state_reset(splt_state *state)
{
	if (state)
	{
		splt_state_reset_mo(state);
		memset(state, 0x0, sizeof(splt_state));
		state->id.genre = 0xFF;
		state->id.year[0]='\0';
	}
	return state;
}

splt_state *splt_state_new(splt_state *state)
{
	if ((state = (splt_state *) malloc(sizeof(splt_state)))==NULL) {
		perror("malloc");
		exit(1);
	}
	memset(state, 0x0, sizeof(splt_state));
	return state;
}

void splt_state_free(splt_state *state)
{
	if (state!=NULL) {
		splt_state_reset(state);
		free(state);
	}
}

void splt_abort(char *msg, splt_state *state,int erron)
{
	splt_state_free(state);
	error(msg,erron);
}

unsigned char *get_out_filename(splt_state *state, unsigned char *filename, int num, char digits, int cddboption)
{
	char temp[256], fm[256];
	int i;
	memset(filename, 0, 512);
	for (i=0; i<OUTNUM; i++) {
		memset(temp, 0, 256);
		memset(fm, 0, 256);
		if (strlen(state->format[i])==0) break;
		if (state->format[i][0]=='%')
		{
			temp[0]='%';
			temp[1]='s';
			switch (state->format[i][1])
			{
				case 'a':
							if (cddboption)
							{
								sprintf(temp+2, state->format[i]+2);
								sprintf(fm, temp, state->id.artist);
							}
							break;
				case 'b':
							if (cddboption)
							{
								sprintf(temp+2, state->format[i]+2);
								sprintf(fm, temp, state->performer[0]);
							}
							break;
				case 't':
							if (cddboption)
							{
								sprintf(temp+2, state->format[i]+2);
								sprintf(fm, temp, state->fn[num]);
							}
							break;
				case 'p':
							if (cddboption)
							{
								sprintf(temp+2, state->format[i]+2);
								sprintf(fm, temp, state->performer[num]);
							}
							break;
				case 'n':
							temp[1]='0';
							temp[2]=digits;
							temp[3]='d';
							sprintf(temp+4, state->format[i]+2);
							sprintf(fm, temp, num);
							break;
			}
		}
		else strncpy(fm, state->format[i], MAXOLEN);
		strncat(filename, fm, strlen(fm));
	}
	return filename;
}

int main (int argc, char *argv[]) {

	FILE *file_input = NULL;
	unsigned char filename[512];
	int tracks = 0, stracks=0, i = 0, j = 0, k = 0, option, filenum=1, autowrap=0, gap=DEFAULT_GAP, rm=0;
	short framemode=0, wrapoption=0, cddboption=0, listoption=0, errsyncoption = 0, outputdir = 0, timeoption=0, paramoption=0;
	short noid3=0, quietoption=0, ogg=0, status = 0, silenceoption = 0, zpad=0, outformat=0, adjustoption=0, seekable=1;
	char *ptr = NULL, id3buffer[128], *arg=NULL, *darg=NULL, *oarg=NULL, *parg=NULL;
	char zstart[32], zend[32], digits='3';
	float time_len=0.f, begin=0.f, end=0.f, off=DEFAULTSILOFF, threshold=DEFAULTTS, min=0.f;
	splt_state *state=NULL;

	fprintf (stderr, NAME" "VER" "YEAR" by "AUTHOR" "EMAIL"\n");
	fprintf (stderr, "THIS SOFTWARE COMES WITH ABSOLUTELY NO WARRANTY! USE AT YOUR OWN RISK!\n");

	while ((option=getopt(argc, argv, "fkwleqnasc:d:o:t:p:"))!=-1) {
	  switch (option) {
		case 'f': framemode=1;
			break;
		case 'k': seekable = 0;
			break;
		case 'w': wrapoption=1;
			break;
		case 'l': listoption=1;
			wrapoption = 1;
			break;
		case 'e': errsyncoption=1;
			break;
		case 'q': quietoption = 1;
			break;
		case 'n': noid3=1;
			break;
		case 'a': adjustoption=1;
			framemode=1;
			break;
		case 's': silenceoption=1;
			framemode=1;
			break;
		case 'c': cddboption=1;
			arg = optarg;
			break;
		case 'd': outputdir=1;
		  	darg = optarg;
			break;
		case 'o': outformat=1;
			oarg = optarg;
			break;
		case 't': timeoption=1;
			arg = optarg;
			break;
		case 'p': paramoption=1;
			parg = optarg;
			break;
		default: error("Run without arguments for HELP. Read man page for complete documentation",1);
	  }
	}

	if (optind > 1)
	{
		argv = rmopt(argv, optind, argc);
		argc -= optind-1;
	}

	if ((argc<4)&&(!wrapoption)&&(!cddboption)&&(!errsyncoption)&&(!silenceoption)&&(!timeoption))
	{
	   fprintf (stderr, "USAGE (Please read man page for complete documentation)\n");
	   fprintf (stderr, "      mp3splt [OPTIONS] FILE... [BEGIN_TIME] [END_TIME...]\n");
	   fprintf (stderr, "      TIME FORMAT: min.sec[.0-99], even if minutes are over 59. \n");
	   fprintf (stderr, "OPTIONS\n");
	   fprintf (stderr, " -w   Splits wrapped files created with Mp3Wrap or AlbumWrap.\n");
	   fprintf (stderr, " -l   Lists the tracks from file without extraction. (Only for wrapped mp3)\n");
	   fprintf (stderr, " -e   Error mode: split mp3 with sync error detection. (For concatenated mp3)\n");
	   fprintf (stderr, " -f   Frame mode (mp3 only): process all frames. For higher precision and VBR.\n");
	   fprintf (stderr, " -c + file.cddb, file.cue or \"query\". Get splitpoints and filenames from a\n");
	   fprintf (stderr, "      .cddb or .cue file or from Internet (\"query\"). Use -a to auto-adjust.\n");
	   fprintf (stderr, " -t + TIME: to split files every fixed time len. (TIME format same as above). \n");
	   fprintf (stderr, " -s   Silence detection: automatically find splitpoint. (Use -p for arguments)\n");
	   fprintf (stderr, " -a   Auto-Adjust splitpoints with silence detection. (Use -p for arguments)\n");
	   fprintf (stderr, " -p + PARAMETERS (th, nt, off, min, rm, gap): user arguments for -s and -a.\n");
	   fprintf (stderr, " -o + FORMAT: output filename pattern. Can contain those variables:\n");
	   fprintf (stderr, "      @a: artist, @p: performer (only CUE), @b: album, @t: title, @n: number\n");
	   fprintf (stderr, " -d + DIRNAME: to put all output files in the directory DIRNAME.\n");
	   fprintf (stderr, " -k   Consider input not seekable (slower). Default when input is STDIN (-).\n");
	   fprintf (stderr, " -n   No Tag: does not write ID3v1 or vorbis comment. If you need clean files.\n");
	   fprintf (stderr, " -q   Quiet mode: do not prompt for anything and print less messages.\n");
	   exit (1);
	}
	else
	{
		if (wrapoption && ((argc<2) || cddboption))
			error("usage is 'mp3splt -w FILE...'",1);

		if (errsyncoption && ((argc<2) || cddboption || wrapoption || silenceoption || timeoption))
			error("usage is 'mp3splt -e FILE...'",1);

		if (cddboption && ((argc<2) || silenceoption || timeoption))
			error("usage is 'mp3splt -c SOURCE FILE...''",1);

		if (silenceoption && ((argc<2) || wrapoption))
			error("usage is 'mp3splt -s [th=THRESHOLD,nt=NUMBER,off=OFFSET,rm] FILE...'",1);

		if (timeoption && ((argc<2) || wrapoption))
			error("usage is 'mp3splt -t TIME FILE...'",1);
	}

	filenum = argc - 1;

	if (!wrapoption) {
		if (!state)
			state = splt_state_new(state);

		splt_state_reset(state);
		memset (filename, '\0', 512);

		if (cddboption) {
			if ((strstr(arg, ".cue")!=NULL)||(strstr(arg, ".CUE")!=NULL)) {
				strncpy(filename, arg, 511);
				tracks = get_cue_splitpoints(filename, state);
			}
			else {
				if (strcmp(arg, "query")==0) {
					fprintf (stderr, "CDDB QUERY. Insert album and artist informations to find cd.\n");
#ifdef _WIN32
					i = search_freedb(state, argv[0]);
#else
					i = search_freedb(state);
#endif
					if (i==0) {
						strncpy(filename, CDDBFILE, strlen(CDDBFILE));
						fprintf (stderr, "Now starting to split file...\n\n");
					}
					else {
						switch(i) {
							case -1: fprintf (stderr, "Error: No cd found in this search!\n");
								splt_abort("",state,104);
								break;
							case -2: fprintf (stderr, "Error: An error occurred while reading Freedb informations\n");
								splt_abort("",state,105);
								break;
							case -3: fprintf (stderr, "Error: Selected Cd not found on "FREEDB". Try another cd.\n");
								splt_abort("",state,106);
								break;
							case -4: fprintf (stderr, "Error: Bad communication or server cowardly refused to give informations!\n");
								splt_abort("",state,107);
								break;
							case -5: fprintf (stderr, "Error: An error occurred while trying to read server answer.\n");
								splt_abort("",state,108);
								break;
							default:
								splt_abort("",state,109);
								break;
						}
						//splt_abort("", state);
					}
				}
				else strncpy(filename, arg, 511);
				tracks = get_cddb_splitpoints(filename, state);
			}
			if (tracks<=0){
				fprintf (stderr, "%s: this is not a valid cddb or cue file, try another.\n", filename);
				splt_abort("", state,110);
			}
			strncpy(state->performer[0], state->id.album, 128);
			cleanstring(state->performer[0]);
		}
		else tracks=0;

		if (!cddboption && !errsyncoption && !silenceoption && !timeoption)
		{
			int timeptr;
			for (i=2; i < argc; i++)
				if (c_seconds(argv[i])!=-1)
					break;

			filenum = i - 1;
			timeptr = i;

			if ((argc-filenum-1)<1)
				splt_abort("no valid splitpoints specified!", state,111);

			for (i=timeptr; (i<argc) && (tracks < MAXTRACKS); i++)
			{
				state->splitpoints[tracks] = c_seconds(argv[i]);
				if (state->splitpoints[tracks] == -1.f)
				{
					if (strcmp(argv[i], EOF_STRING)!=0)
					{
						fprintf (stderr, "Warning: omitting '%s' (bad time format)\n", argv[i]);
						break;
					}
				}
				else
				if  ((tracks > 0) && (state->splitpoints[tracks] < state->splitpoints[tracks-1]))
				{
					fprintf (stderr, "Warning: omitting '%s' (not enough seconds)\n", argv[i]);
					break;
				}
				zpad = dot_pos(argv[i], zpad);
				tracks++;
			}

			tracks = tracks-1;

			if (tracks < 1)
				splt_abort("no valid splitpoints found!", state,112);

		}

		if (outformat)
		{
			i = parse_outformat(oarg, state->format, cddboption);
			switch (i) {
				case -1: splt_abort("", state,127);
						break;
				case 1: if ((tracks > 1) || (silenceoption) || (errsyncoption) || (timeoption))
						{
							if (strcmp(oarg, "-")!=0)
								splt_abort("your output format may be ambiguous! (@t or @n missing)", state,113);
						}
					     break;
				case 0: break;
			}
			if (tracks > 1)
			{
				i = (int) (log10((double) (tracks)));
				digits = (char) ((i+1) | 0x30);
			}
		}

		if (paramoption) {
			if (parse_arg(parg, &threshold, &gap, &stracks, &off, &rm, &min) < 1)
				splt_abort("bad argument for -p option. No valid value was recognized!", state,114);
		}

		if (silenceoption || adjustoption) {
			if  ((threshold<-96.f) || (threshold>0.f) || (adjustoption < 0))
				splt_abort("bad threshold (float between -96 and 0 dB) or gap (positive int)", state,116);
			if  ((off<-2.f) || (off>2.f))
				splt_abort("bad offset value (float between -1 and 1)", state,117);
			if (min<0.f)
				splt_abort("bad minimum silence value (positive float)", state,118);
			if (adjustoption)
				adjustoption = (short) (gap);
		}
	}

	if (outputdir)
#ifdef _WIN32
		mkdir(darg);
#else
		mkdir(darg, 0755);
#endif

	for (k=0; k < filenum; k++)
	{
		ogg = i = 0;

		if (strcmp(argv[k+1], "-")==0)
		{
			fprintf (stderr, "Warning: stdin \"-\" is supposed to be mp3 stream. Use \"o-\" for ogg.\n");
			file_input = stdin;
			seekable = 0;
		}
		else if (strcmp(argv[k+1], "o-")==0)
		{
			fprintf (stderr, "Warning: stdin \"o-\" is supposed to be ogg stream. Use \"-\" for mp3.\n");
			file_input = stdin;
			ogg = 1;
			seekable = 0;
		}
		else if (!(file_input=fopen(argv[k+1], "rb"))) {
			perror(argv[k+1]);
			exit(1);
		}

		if ((!seekable) && (silenceoption || adjustoption || errsyncoption || wrapoption))
			splt_abort("can't use STDIN or -k option with -s -a -e -w (input must be seekable)", state,119);

		if (filenum > 1)
			fprintf(stderr, "%s:\n", argv[k+1]);

		if (!wrapoption)
		{
			ptr = strtoupper(argv[k+1]); // Assure that any combination of these string will be found. You should not modify them!
			if ((strstr(ptr, OGGEXTU))!=NULL)
				ogg = 1;

			autowrap = 0;
			splt_state_reset_mo(state);
			if (((strstr(ptr, WRAP)!=NULL)||(strstr(ptr, ALBW)!=NULL))&&quietoption==0) {
				fprintf (stderr, "\nWARNING: "WRAP" or "ALBW" string found in filename. Switching to wrap mode...\n");
				fprintf (stderr, "\t Remove string if program gives errors or if you want standard mode.\n\n");
				autowrap = 1;
			}
			free(ptr);
		}

		if (wrapoption || autowrap) {
			if (dewrap(file_input, listoption, quietoption, darg)!=0) {
				fflush(stdout);
				error("file is not wrapped or might be damaged! Try -e option.",102);
			}
			else if (listoption) {
					fprintf(stdout, "\n\n");
					fflush(stdout);
				}
				else fprintf (stderr, "\n\nAll files have been splitted correctly. Visit http://mp3wrap.sourceforge.net!\n");

			fclose(file_input);
		}
		else
		{
			begin = end = 0.f;

			if (errsyncoption && ogg)
				error("can't use error option with ogg!",103);

			if (ogg) {

#ifdef NO_OGG
				error("program was compiled without ogg support. Can't split file.",102);
#else
				if (!(state->ostate = ogginfo(file_input, state->ostate)))
					splt_abort("input does not appear to be a valid ogg vorbis bitstream", state,120);

				if (file_input != stdin) {
					i = (int) ov_time_total(&state->ostate->vf, -1); // Total Seconds
					j = (int) (i / 60); // Minutes
				}
				fprintf(stderr, "Ogg Vorbis Stream - %ld - %ld Kb/s - %d channels", state->ostate->vd->vi->rate,
					state->ostate->vd->vi->bitrate_nominal/1024, state->ostate->vd->vi->channels);

				if (noid3 || cddboption)
					vorbis_comment_clear(&state->ostate->vc);

				if (state->ostate->vc.comments>0)
					noid3 = 1;

				state->ostate->off = off;
#endif
			}
			else {
				struct mp3 *mfile;

				if (!(state->mstate = mp3info(file_input, state->mstate, quietoption, framemode)))
					splt_abort("input does not appear to be a valid mp3 stream", state,121);

				mfile = &state->mstate->mp3file; // For short lines... ;-)

				fprintf (stderr, "MPEG %d Layer %d - %d Hz - %s", (2-mfile->mpgid), mfile->layer, mfile->freq, chan[mfile->channels]);
				if (state->mstate->framemode)
				{
					fprintf (stderr, " - FRAME MODE");
					if (!seekable) fprintf (stderr, " NS");
				}
				else fprintf (stderr, " - %d Kb/s", mfile->bitrate*BYTE/1000);

				if (state->mstate->total_time > 0)
				{
					i = (int)(state->mstate->total_time);
					j = (int)(i/60);
					state->mstate->frames = 1;
				}
				state->mstate->off = off;
			}

			if (i!=0) fprintf (stderr, " - Total time: %dm.%02ds", j, i%60);
			fprintf (stderr, "\n");

			if (errsyncoption)
			{
				off_t splitpoints[MAXTRACKS+1];
				i = syncerror_split(state->mstate, splitpoints, quietoption);

				if (i!=0)
					status = 1;
				switch (i) {
					case -1: splt_abort("an error occurred while processing file!", state,122);
							break;
					case 0: break;
					case 1: splt_abort("sorry, no sync errors found. Can't split file.", state,123);
							break;
					case 2: splt_abort("too many sync errors! Are you sure this is a valid mp3 stream?", state,124);
							break;
					case 3: fprintf(stderr, "Aborted.\n");
							splt_abort("", state,125);
							break;
				}

				for (i = 0; i < state->mstate->syncerrors; i++) {
					int ret;
					if (outformat)
						get_out_filename(state, filename, i+1, digits, 0);
					else sprintf(filename, "Track %02d", i+1);
					check_ext(filename, ogg);
					if (outputdir)
					{
						char temp[512];
						strncpy(temp, filename, 512);
						sprintf(filename, "%s%c%s", darg, DIRCHAR, temp);
					}
					if ((file_input != stdin) && (strcmp(argv[k+1], filename)==0))
						splt_abort("input and output are the same file.", state,126);
					fprintf (stderr, "\n %3ld %% -> Splitting %s... ", ((i+1)*100)/state->mstate->syncerrors, filename);

          fprintf(stdout,"%ld -> %ld\n",splitpoints[i],splitpoints[i+1]);
          fflush(stdout);

					ret = split(filename, state->mstate->file_input, splitpoints[i], splitpoints[i+1], 0, NULL, NULL);
					if ((ret==0) || (ret==-3))
						fprintf (stderr, "OK");
					else
					{
						fprintf (stderr, "FAILED\n");
						splt_abort("", state,127);
					}
				}
				fprintf(stderr, "\n\n");
			}
			else
			if (silenceoption) {
				FILE *log;
				int found = 0;
				struct ssplit *temp, **list;

				if (!ogg)
					list = &state->mstate->silence_list;
#ifndef NO_OGG
				else list = &state->ostate->silence_list;
#endif
				if ((log=fopen(SSPLITLOG, "r"))) {
					short error = 0;
					if (fgets(filename, 512, log)!=NULL) {
						filename[strlen(filename)-1]='\0';
						if (strcmp(filename, argv[k+1])!=0)
							error = 1;
					}
					else error = 1;
					if (fgets(filename, 512, log)!=NULL) {
						float t, m;
						filename[strlen(filename)-1]='\0';
						i=sscanf(filename, "%f\t%f", &t, &m);
						if ((i < 2) || (threshold != t) || (min != m))
							error=1;
					}
					else error = 1;
					if (error) {
						fclose(log);
						log = NULL;
					}
				}

				if (!quietoption)
				{
					fprintf(stderr, "Silence split type: ");
					if (stracks>0) fprintf(stderr, "User");
					else fprintf(stderr, "Auto");
					fprintf(stderr, " mode (Th: %.1f dB, Off: %.2f, Min: %.2f, Remove: ", threshold, off, min);
					if (rm)
						fprintf(stderr, "YES");
					else fprintf(stderr, "NO");
					fprintf(stderr, ")\n");
				}

				if (log == NULL) {
					fprintf(stderr, "\t\t\t\tDetecting silence positions, please wait...\r");
					if (!ogg)
						found = mp3_scan_silence(state->mstate, state->mstate->mp3file.firsthead.ptr, 0, threshold, min, 1);
#ifndef NO_OGG
					else found = ogg_scan_silence(state->ostate, 0, threshold, min, 1, NULL, 0);
#endif
					if (found > 0) {
						if (!(log=fopen(SSPLITLOG, "w")))
							fprintf(stderr, "Warning: can't write "SSPLITLOG" file to save informations!\n");
						else {
							temp = *list;
							fprintf(log, "%s\n", argv[k+1]);
							fprintf(log, "%.2f\t%.2f\n", threshold, min);
							while (temp != NULL) {
								fprintf(log, "%f\t%f\t%d\n", temp->begin_position, temp->end_position, temp->len);
								temp = temp->next;
							}
							fclose(log);
						}
					}
				}
				else {
					fprintf(stderr, SSPLITLOG" file found! Reading silence points from file to save time ;)\n");
					found = parse_ssplit_file(list, log);
					fclose(log);
				}

				if (found <= 0) {
					float level;
					if (!ogg) level =state->mstate->avg_level/state->mstate->n_stat;
#ifndef NO_OGG
					else level = state->ostate->avg_level/state->ostate->n_stat;
#endif
					fprintf(stderr, "Error: sorry, no silence points found in this file! Average Level: %.1f dB\n", level);
					status=401;
				}

				if (!status) {
					int order;
					fprintf (stderr, "Total silence points found: %d. ", found);
					found++;
					if ((stracks > 0)&&(stracks < MAXTRACKS)) {
						if (stracks < found)
							found = stracks;
					}
					fprintf(stderr, "Selected %d tracks...\n", found);
					state->splitpoints[0] = 0.f;
					temp = *list;
					for (i = 1; i < found; i++) {
						if (temp == NULL) {
							found = i;
							break;
						}
						if (rm)
						{
							state->splitpoints[2*i-1] = temp->begin_position;
							state->splitpoints[2*i] = temp->end_position;
						}
						else
						{
							state->splitpoints[i] = silence_position(temp, off);
						}
						temp = temp->next;
					}

					if (rm)
						order = (found-1)*2+1;
					else order = found;

					order_splitpoints(state->splitpoints, order);
					state->splitpoints[order] = -1.f;

					for (i = 0; i < found; i++) {
						float beg_pos, end_pos;

						if (outformat)
							get_out_filename(state, filename, i+1, digits, 0);
						else sprintf(filename, "Track %02d", i+1);
						check_ext(filename, ogg);
						if (outputdir)
						{
							char temp[512];
							strncpy(temp, filename, 512);
							sprintf(filename, "%s%c%s", darg, DIRCHAR, temp);
						}
						if ((file_input != stdin) && (strcmp(argv[k+1], filename)==0))
							splt_abort("input and output are the same file.", state,128);
						fprintf (stderr, "\n %3d %% -> Splitting %s... ", (int)((i+1)*100)/found, filename);

						if (rm)
						{
							beg_pos = state->splitpoints[2*i];
							end_pos = state->splitpoints[2*i+1];
							if (!ogg) state->mstate->end = 0;
#ifndef NO_OGG
							else state->ostate->end = 0;
#endif
						}
						else
						{
							beg_pos = state->splitpoints[i];
							end_pos = state->splitpoints[i+1];
						}

						if (!ogg) j = mp3split (filename, state->mstate, NULL, beg_pos, end_pos, 0, seekable, 0);
#ifndef NO_OGG
						else j = oggsplit(filename, state->ostate, beg_pos, end_pos, seekable, 0, 0);
#endif
						switch (j)
						{
							case 0:
							case -4:
								fprintf (stderr, "OK");
								break;
							case -3:
								fprintf (stderr, "OK (EOF)");
								break;
							default:
								fprintf (stderr, "FAILED\nError: an error occurred!");
								status=1; //TODO 
								break;
						}
					}
					fprintf (stderr, "\n\n");
				}
			}
			else {
				if (!quietoption && cddboption) {
					fprintf (stderr, "Reading informations from %s...\n", filename);
					fprintf (stderr, "Tracks: %d \n", tracks);
					fprintf (stderr, "Artist: %s\n", state->id.artist);
					fprintf (stderr, "Album: %s\n\n", state->id.album);
				}

				if (timeoption)
				{
					if ((time_len = c_seconds(arg))==-1)
					{
						fprintf(stderr, "Error: bad time expression '%s'. Must be min.sec, read man page for details.\n", arg);
						splt_abort("", state,129);
					}
					end=time_len;
					tracks=2;
				}

				if (adjustoption && !quietoption) {
					fprintf (stderr, "Working with SILENCE AUTO-ADJUST (Threshold: %.1f dB Gap: %d sec Offset: %.2f)\n", threshold, adjustoption, off);
					if (cddboption) fprintf(stderr, "\n");
				}

				j = 1;

				do 	{

					memset (filename, '\0', 512);
					ptr = argv[k+1];

					while (strchr(ptr, DIRCHAR)!=NULL) // Will create file where we are (PWD)
						ptr = strchr(ptr, DIRCHAR) + 1;

					if (outformat)
						get_out_filename(state, filename, j, digits, cddboption);

					if (!cddboption) {
						if (timeoption)
						{
							zero_pad_float(begin, zstart);
							zero_pad_float(end, zend);
						}
						else
						{
							zero_pad(argv[k+1+j], zstart, zpad);
							if ((k+2+j) < argc)
							{
								if (state->splitpoints[j]!=-1.f)
									zero_pad(argv[k+2+j], zend, zpad);
								else
									strncpy(zend, EOF_STRING"\0", strlen(EOF_STRING)+1);
							}
						}
						if (!outformat)
						{
							if (file_input!=stdin)
							{
								if ((strlen(ptr)>4) && (((strstr(ptr, MP3EXT))!=NULL) || (ogg)))
									strncpy (filename, ptr, strlen(ptr)-4);
								else strncpy (filename, ptr, 511);
								sprintf (filename, "%s"SEP"%s"SEP"%s", filename, zstart, zend);
							}
							else sprintf (filename, "%s"SEP"%s", zstart, zend);
						}
					}
					else {
						memset(state->id.title, 0x00, 31);
						strncpy(state->id.title, state->fn[j], 30);
						if (!outformat)
							sprintf (filename, "%s - %02d - %s", state->id.artist, j, state->fn[j]);
					}

					check_ext(filename, ogg);

					if (!noid3) {
						if (!cddboption) {
							strncpy(state->id.title, ptr, 30);
							strncpy(state->id.album, "Splitted by "NAME" v. "VER, 30);
							strncpy(state->id.artist, "From \0", 6);
							strncat(state->id.artist, zstart, 31-strlen(state->id.artist));
							strncat(state->id.artist, " To \0", 31-strlen(state->id.artist));
							strncat(state->id.artist, zend, 31-strlen(state->id.artist));
						}

						state->id.title[30]='\0';
						state->id.artist[30]='\0';
						state->id.album[30]='\0';

						if (tracks>1) i=j;
						else i=0x00;

						ptr = NULL;

						if (state->performer[j][0]=='\0') strncpy(state->performer[j], state->id.artist, 128);

						if (ogg) {
#ifndef NO_OGG
							char *gptr;
							if (state->id.genre == 0xFF) // Out of genre list
								gptr = NULL;
							else gptr = (char *) genre_list[(int) state->id.genre];
							ptr = trackstring(i);
							vorbis_comment_init(&state->ostate->vc);
							v_comment(&state->ostate->vc, state->performer[j], state->id.album, state->id.title, ptr, state->id.year, gptr, WEBSITE);
							if (ptr!=NULL) free(ptr);
#endif
						}
						else ptr = id3(id3buffer, state->id.title, state->performer[j], state->id.album, state->id.year, state->id.genre, WEBSITE, i);
					}
					else
					{
						ptr = NULL;
					}

					if (tracks > 1 && !timeoption)
						fprintf (stderr, " %3d %% -> ", (j*100)/tracks);

					if (outputdir)
					{
						char temp[512];
						strncpy(temp, filename, 512);
						sprintf(filename, "%s%c%s", darg, DIRCHAR, temp);
					}

					if ((file_input != stdin) && (strcmp(argv[k+1], filename)==0))
						splt_abort("input and output are the same file.", state,130);

					if (!cddboption) fprintf (stderr, "Splitting ");
					fprintf (stderr, "%s... ", filename);
					fflush(stderr);

					if (!timeoption) {
						begin = state->splitpoints[j-1];
						end = state->splitpoints[j];
					}

					if (!ogg) i = mp3split (filename, state->mstate, ptr, begin, end, adjustoption, seekable, threshold);
#ifndef NO_OGG
					else i = oggsplit(filename, state->ostate, begin, end, seekable, adjustoption, threshold);
#endif
					if (timeoption)
					{
						begin=end;
						end+=time_len;
						tracks++;
					}

					switch (i) {
						case 0:	fprintf (stderr, "OK\n");
								break;
						case -1: fprintf (stderr, "FAILED!\n");
								if (cddboption)
									splt_abort("file is damaged or this is not the right cddb!", state,131);
								else splt_abort("begin is out of file or not enough seconds!", state,132);
								break;
						case -2:	fprintf (stderr, "OK\nWarning: an error occurred while seeking input file. Trying to continue...\n");
								break;
						case -3:	fprintf (stderr, "OK (EOF)\n");
								if (cddboption && (j != tracks))
									splt_abort("file is damaged or this is not the right cddb!", state,131);
								else tracks=0; // End of file reached, we should stop ;)
								break;
						case -4:	fprintf (stderr, "OK (WARNING)\nWarning: mp3 might be VBR. You should use -f option.\n");
								tracks=0;
								break;
						case -5:	fprintf(stderr, "FAILED!\nWarning: can't write to outputfile! Trying to continue...\n");
								break;
						case -6:	fprintf(stderr, "FAILED!\nWarning: an error occurred while reading file. Please report this to author.\n");
								break;
					}

				}  while (j++<tracks);

				if (!quietoption) {
					if (cddboption) {
						fprintf (stderr, "\nAll files successfully splitted! ");
					}
					if (!ogg)
						if (state->mstate->framemode)
							fprintf (stderr, "Processed %lu frames - Sync errors: %lu", state->mstate->frames, state->mstate->syncerrors);

					if (cddboption)
					{
						if (!adjustoption) {
							fprintf (stderr, "\n\n+-----------------------------------------------------------------------------+\n");
							fprintf (stderr, "|NOTE: When you use cddb/cue, splitted files might be not very precise due to:|\n");
							fprintf (stderr, "|1) Who extracts CD tracks might use \"Remove silence\" option. This means that |\n");
							fprintf (stderr, "|   the large mp3 file is shorter than CD Total time. Never use this option.  |\n");
							fprintf (stderr, "|2) Who burns CD might add extra pause seconds between tracks.  Never do it.  |\n");
							fprintf (stderr, "|3) Encoders might add some padding frames so  that  file is longer than CD.  |\n");
							fprintf (stderr, "|4) There are several entries of the same cd on CDDB, find the best for yours.|\n");
							fprintf (stderr, "|   Usually you can find the correct splitpoints for your mp3, so good luck!  |\n");
							fprintf (stderr, "+-----------------------------------------------------------------------------+\n");
							fprintf (stderr, "| TRY TO ADJUST SPLITS POINT WITH -a OPTION. Read man page for more details!  |\n");
							fprintf (stderr, "+-----------------------------------------------------------------------------+\n\n");
						}
						else fprintf (stderr, "\n");
					}
					else if (!ogg)
							if (state->mstate->framemode) fprintf (stderr, "\n");
				}
			}
		}
	}

	splt_state_free(state);

	return status;
}

