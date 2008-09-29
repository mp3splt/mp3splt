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

#include <time.h>
#include <string.h>
#include <math.h>
#include <locale.h>

#ifdef __WIN32__
#include <io.h>
#include <fcntl.h>
#endif

#include "ogg.h"

/**
 * Notes :
 * - if ogg_stream_init() returns -1, then the stream was incorrectly
 *   initialized => coding error
 * - some 'vorbis_synthesis_*' functions return OV_EINVAL for bad argument
 *   but we don't check that error code because this is not supposed to
 *   happend; if it happends, it's a coding error
 **/

/****************************/
/* some function prototypes */

splt_ogg_state *splt_ogg_info(FILE *in, splt_state *state, int *error);
int splt_ogg_scan_silence(splt_state *state, short seconds, 
    float threshold, float min,
    short output, ogg_page *page, ogg_int64_t granpos, int *error);

/****************************/
/* ogg constants */

//ogg genre list
const char *splt_ogg_genre_list[] = {
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

/****************************/
/* ogg utils */

FILE *splt_ogg_open_file_read(splt_state *state,
    const char *filename, int *error)
{
  FILE *file_input = NULL;

  if (strcmp(filename,"o-") == 0)
  {
    file_input = stdin;
#ifdef __WIN32__
    _setmode(fileno(file_input), _O_BINARY);
#endif
  }
  else
  {
    //we open the file
    file_input = fopen(filename, "rb");
    if (file_input == NULL)
    {
      splt_t_set_strerror_msg(state);
      splt_t_set_error_data(state,filename);
      *error = SPLT_ERROR_CANNOT_OPEN_FILE;
    }
  }

  return file_input;
}

//gets the mp3 info and puts it in the state
void splt_ogg_get_info(splt_state *state, FILE *file_input, int *error)
{
  //checks if valid ogg file
  state->codec = splt_ogg_info(file_input, state, error);

  //if error
  if ((*error < 0) || (state->codec == NULL))
  {
    return;
  }
  else
  {
    //put file infos to client 
    if (! splt_t_messages_locked(state))
    {
      splt_ogg_state *oggstate = state->codec;
      //ogg infos
      char ogg_infos[2048] = { '\0' };
      snprintf(ogg_infos, 2048, " info: Ogg Vorbis Stream - %ld - %ld Kb/s - %d channels",
          oggstate->vd->vi->rate, oggstate->vd->vi->bitrate_nominal/1024,
          oggstate->vd->vi->channels);
      //total time
      char total_time[256] = { '\0' };
      int total_seconds = (int) splt_t_get_total_time(state) / 100;
      int minutes = total_seconds / 60;
      int seconds = total_seconds % 60;
      snprintf(total_time,256," - Total time: %dm.%02ds", minutes, seconds%60);
      //all infos together
      char all_infos[3072] = { '\0' };
      snprintf(all_infos,3072,"%s%s\n",ogg_infos,total_time);
      splt_t_put_message_to_client(state, all_infos);
    }
  }
}

static char *splt_ogg_trackstring(int number)
{
  char *track = NULL;

  if (number != 0)
  {
    int len = 0, i;
    len = ((int) (log10((double) (number)))) + 1;

    if ((track = malloc(len + 1))==NULL)
    {
      return NULL;
    }
    memset(track, 0, len + 1);
    for (i=len-1; i >= 0; i--)
    {
      track[i] = ((number%10) | 0x30);
      number /= 10;
    }
  }

  return track;
}

//for the moment we only omit invalid character
static char *splt_ogg_checkutf(char *s)
{
  int i, j=0;
  for (i=0; i < strlen(s); i++)
  {
    if (s[i]<0x7F)
    {
      s[j++] = s[i];
    }
  }
  s[j] = '\0';

  return s;
}

//saves a packet
static splt_v_packet *splt_ogg_save_packet(ogg_packet *packet, int *error)
{
  splt_v_packet *p = NULL;

  //if we have no header, we will have bytes < 0
  p = malloc(sizeof(splt_v_packet));
  if (!p)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return p;
  }

  p->length = packet->bytes;
  p->packet = malloc(p->length);
  if (! p->packet)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    free(p);
    p = NULL;
    return p;
  }
  memcpy(p->packet, packet->packet, p->length);

  return p;
}

//frees a packet
static void splt_ogg_free_packet(splt_v_packet *p)
{
  if(p)
  {
    if(p->packet)
    {
      free(p->packet);
      p->packet = NULL;
    }
    free(p);
    p = NULL;
  }
}

static long splt_ogg_get_blocksize(splt_ogg_state *oggstate, 
    vorbis_info *vi, ogg_packet *op)
{
  //if this < 0, there is a problem
  int this = vorbis_packet_blocksize(vi, op);
  int ret = (this+oggstate->prevW)/4;

  oggstate->prevW = this;

  return ret;
}

static int splt_ogg_update_sync(splt_state *state, ogg_sync_state *sync_in,
    FILE *f, int *error)
{
  char *buffer = ogg_sync_buffer(sync_in, SPLT_OGG_BUFSIZE);
  if (!buffer)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return -1;
  }
  int bytes = fread(buffer,1,SPLT_OGG_BUFSIZE,f);

  if (ogg_sync_wrote(sync_in, bytes) != 0)
  {
    splt_t_set_error_data(state, splt_t_get_filename_to_split(state));
    *error = SPLT_ERROR_INVALID;
    return -1;
  }

  return bytes;
}

/* Returns 0 for success, or -1 on failure. */
static int splt_ogg_write_pages_to_file(splt_state *state, 
    ogg_stream_state *stream, FILE *file, int flush, int *error,
    const char *output_fname)
{
  ogg_page page;

  if(flush)
  {
    while(ogg_stream_flush(stream, &page))
    {
      if(fwrite(page.header, 1, page.header_len, file) < page.header_len)
      {
        goto write_error;
      }
      if(fwrite(page.body, 1, page.body_len, file) < page.body_len)
      {
        goto write_error;
      }
    }
  }
  else
  {
    while(ogg_stream_pageout(stream, &page))
    {
      if (fwrite(page.header,1,page.header_len, file) < page.header_len)
      {
        goto write_error;
      }
      if (fwrite(page.body,1,page.body_len, file) < page.body_len)
      {
        goto write_error;
      }
    }
  }

  return 0;

write_error:
  splt_t_set_strerror_msg(state);
  splt_t_set_error_data(state, output_fname);
  *error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
  return -1;
}

static void splt_ogg_submit_headers_to_stream(ogg_stream_state *stream, 
    splt_ogg_state *oggstate)
{
  int i;
  for(i=0;i<3;i++)
  {
    ogg_packet p;
    p.bytes = oggstate->headers[i]->length;
    p.packet = oggstate->headers[i]->packet;
    p.b_o_s = ((i==0)?1:0);
    p.e_o_s = 0;
    p.granulepos=0;

    ogg_stream_packetin(stream, &p);
  }
}

/* Full cleanup of internal state and vorbis/ogg structures */
static void splt_ogg_v_free(splt_ogg_state *oggstate)
{
  if(oggstate)
  {
    if(oggstate->packets)
    {
      splt_ogg_free_packet(oggstate->packets[0]);
      splt_ogg_free_packet(oggstate->packets[1]);
      free(oggstate->packets);
      oggstate->packets = NULL;
    }
    if(oggstate->headers)
    {
      int i;
      for(i=0; i < 3; i++)
      {
        splt_ogg_free_packet(oggstate->headers[i]);
      }
      free(oggstate->headers);
      oggstate->headers;
    }
    vorbis_comment_clear(&oggstate->vc);
    if(oggstate->vb)
    {
      vorbis_block_clear(oggstate->vb);
      free(oggstate->vb);
      oggstate->vb = NULL;
    }
    if(oggstate->vd)
    {
      vorbis_dsp_clear(oggstate->vd);
      free(oggstate->vd);
      oggstate->vd = NULL;
    }
    //only free the input if different from stdin
    if(oggstate->stream_in && oggstate->in != stdin)
    {
      ogg_stream_clear(oggstate->stream_in);
      free(oggstate->stream_in);
      oggstate->stream_in = NULL;
    }
    if(oggstate->sync_in)
    {
      ogg_sync_clear(oggstate->sync_in);
      free(oggstate->sync_in);
      oggstate->sync_in = NULL;
    }
    if (oggstate->vi)
    {
      vorbis_info_clear(oggstate->vi);
      free(oggstate->vi);
      oggstate->vi = NULL;
    }
    free(oggstate);
    oggstate = NULL;
  }
}

static splt_ogg_state *splt_ogg_v_new(int *error)
{
  splt_ogg_state *oggstate = NULL;

  if ((oggstate = malloc(sizeof(splt_ogg_state)))==NULL)
  {
    goto error;
  }
  memset(oggstate, 0, sizeof(splt_ogg_state));
  if ((oggstate->sync_in = malloc(sizeof(ogg_sync_state)))==NULL)
  {
    goto error;
  }
  if ((oggstate->stream_in = malloc(sizeof(ogg_stream_state)))==NULL)
  {
    goto error;
  }
  if ((oggstate->vd = malloc(sizeof(vorbis_dsp_state)))==NULL)
  {
    goto error;
  }
  if ((oggstate->vi = malloc(sizeof(vorbis_info)))==NULL)
  {
    goto error;
  }
  if ((oggstate->vb = malloc(sizeof(vorbis_block)))==NULL)
  {
    goto error;
  }
  if ((oggstate->headers = malloc(sizeof(splt_v_packet)*3))==NULL)
  {
    goto error;
  }
  memset(oggstate->headers, 0, sizeof(splt_v_packet)*3);
  if ((oggstate->packets = malloc(sizeof(splt_v_packet)*2))==NULL)
  {
    goto error;
  }
  memset(oggstate->packets, 0, sizeof(splt_v_packet)*2);

  return oggstate;

error:
  *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  splt_ogg_v_free(oggstate);
  return NULL;
}

//frees the splt_ogg_state structure,
//used in the splt_t_state_free() function
void splt_ogg_state_free(splt_state *state)
{
  splt_ogg_state *oggstate = state->codec;
  if (oggstate)
  {
    ov_clear(&oggstate->vf);
    splt_ogg_v_free(oggstate);
    state->codec = NULL;
  }
}

/****************************/
/* ogg tags */

//puts tags in vc
//TODO: what happens if 'vorbis_comment_add_tag(..)' fails ?
//- ask vorbis developers
static vorbis_comment *splt_ogg_v_comment (vorbis_comment *vc, char *artist,
    char *album, char *title, 
    char *tracknum, char *date, char *genre, char *comment, int *error)
{
  if (title!=NULL)
  {
    vorbis_comment_add_tag(vc, "title", splt_ogg_checkutf(title));
  }
  if (artist!=NULL)
  {
    vorbis_comment_add_tag(vc, "artist", splt_ogg_checkutf(artist));
  }
  if (album!=NULL)
  {
    vorbis_comment_add_tag(vc, "album", splt_ogg_checkutf(album));
  }
  if (date!=NULL)
  {
    if (strlen(date)>0)
    {
      vorbis_comment_add_tag(vc, "date", date);
    }
  }
  if (genre!=NULL)
  {
    vorbis_comment_add_tag(vc, "genre", genre);
  }
  if (tracknum!=NULL)
  {
    vorbis_comment_add_tag(vc, "tracknumber", tracknum);
  }
  if (comment!=NULL)
  {
    vorbis_comment_add_tag(vc, "comment", comment);
  }

  return vc;
}

//macro used only in the following function splt_ogg_get_original_tags
#define OGG_VERIFY_ERROR() \
if (err != SPLT_OK) \
{ \
*tag_error = err; \
return; \
};
//get the original ogg tags and put them in the state
void splt_ogg_get_original_tags(const char *filename,
    splt_state *state, int *tag_error)
{
  splt_ogg_state *oggstate = state->codec;

  vorbis_comment *vc_local = NULL;
  vc_local = ov_comment(&oggstate->vf,-1);
  int err = SPLT_OK;

  char *a = NULL,*t = NULL,*al = NULL,*da = NULL, *g = NULL,*tr = NULL,
       *com = NULL;

  int size = 0;
  a = vorbis_comment_query(vc_local, "artist",0);
  if (a != NULL)
  {
    size = strlen(a);
    err = splt_t_set_original_tags_field(state,SPLT_TAGS_ARTIST, 0,a,0x0,size);
    OGG_VERIFY_ERROR();
  }

  t = vorbis_comment_query(vc_local, "title",0);
  if (t != NULL)
  {
    size = strlen(t);
    err = splt_t_set_original_tags_field(state,SPLT_TAGS_TITLE, 0,t,0x0,size);
    OGG_VERIFY_ERROR();
  }

  al = vorbis_comment_query(vc_local, "album",0);
  if (al != NULL)
  {
    size = strlen(al);
    err = splt_t_set_original_tags_field(state,SPLT_TAGS_ALBUM, 0,al,0x0,size);
    OGG_VERIFY_ERROR();
  }

  da = vorbis_comment_query(vc_local, "date",0);
  if (da != NULL)
  {
    size = strlen(da);
    err = splt_t_set_original_tags_field(state,SPLT_TAGS_YEAR, 0,da,0x0,size);
    OGG_VERIFY_ERROR();
  }

  g = vorbis_comment_query(vc_local, "genre",0);
  if (g != NULL)
  {
    size = strlen(g);
    err = splt_t_set_original_tags_field(state,SPLT_TAGS_GENRE, 0, g, 0x0,size);
    OGG_VERIFY_ERROR();
  }

  tr = vorbis_comment_query(vc_local, "tracknumber",0);
  if (tr != NULL)
  {
    size = strlen(tr);
    err = splt_t_set_original_tags_field(state,SPLT_TAGS_TRACK, 0,tr, 0x0,size);
    OGG_VERIFY_ERROR();
  }

  com = vorbis_comment_query(vc_local, "comment",0);
  if (com != NULL)
  {
    size = strlen(com);
    err = splt_t_set_original_tags_field(state,SPLT_TAGS_COMMENT, 0,com,0x0,size);
    OGG_VERIFY_ERROR();
  }
}

//we take the tags from the state and put them in the
//new file
//TODO: why we don't set the 'genre' and 'track' ?
static void splt_ogg_put_original_tags(splt_state *state, int *error)
{
  splt_ogg_state *oggstate = state->codec;
  char *a = NULL,*t = NULL,*al = NULL,*da = NULL,
       /**g,*tr,*/*com = NULL;

  a = state->original_tags.artist;
  t = state->original_tags.title;
  al = state->original_tags.album;
  da = state->original_tags.year;
  //g = (char *)state->original_tags.genre;
  //tr = (char *)state->original_tags.track;
  com = state->original_tags.comment;

  vorbis_comment_clear(&oggstate->vc);
  vorbis_comment_init(&oggstate->vc);
  splt_ogg_v_comment(&oggstate->vc, a,al,t,
  /*tr*/NULL,da,/*g*/NULL,com, error);
}

//puts the ogg tags
void splt_ogg_put_tags(splt_state *state, int *error)
{
  splt_ogg_state *oggstate = state->codec;

  //clean_original_id3(state);
  //if we put the original tags
  vorbis_comment_clear(&oggstate->vc);

  if (splt_t_get_int_option(state, SPLT_OPT_TAGS) == SPLT_TAGS_ORIGINAL_FILE)
  {
    splt_ogg_put_original_tags(state, error);
    return;
  }
  else
  {
    //if we put current tags (cddb,cue,...)
    if (splt_t_get_int_option(state, SPLT_OPT_TAGS) == SPLT_CURRENT_TAGS)
    {
      int current_split = splt_t_get_current_split(state);

      //if we set all the tags like the x one
      int remaining_tags_like_x = splt_t_get_int_option(state,SPLT_OPT_ALL_REMAINING_TAGS_LIKE_X); 
      if ((current_split >= state->split.real_tagsnumber)
          && (remaining_tags_like_x != -1))
      {
        current_split = remaining_tags_like_x;
      }

      //only if the tags exists for the current split
      if (splt_t_tags_exists(state,current_split))
      {
        char *title = NULL;
        char *artist = NULL;

        title = splt_t_get_tags_char_field(state,current_split,
              SPLT_TAGS_TITLE);
        artist = splt_t_get_tags_char_field(state,current_split,
              SPLT_TAGS_ARTIST);

        //only if we have the artist or the title
        if (((artist != NULL) && (artist[0] != '\0'))
            || ((title != NULL) && (title[0] != '\0')))
        {
          int tags_number = 0;
          splt_tags *tags = 
            splt_t_get_tags(state, &tags_number);

          if (splt_t_tags_exists(state, current_split))
          {
            char *track_string = NULL;
            if (tags[current_split].track > 0)
            {
              track_string = 
                splt_ogg_trackstring(tags[current_split].track);
            }
            else
            {
              track_string = 
                splt_ogg_trackstring(current_split+1);
            }

            if (track_string)
            {
              splt_ogg_v_comment(&oggstate->vc,
                  tags[current_split].artist,
                  tags[current_split].album,
                  tags[current_split].title,
                  track_string,
                  tags[current_split].year,
                  (char *)splt_ogg_genre_list[(int) 
                  tags[current_split].genre],
                  tags[current_split].comment, error);

              free(track_string);
              track_string = NULL;
            }
            else
            {
              *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
              return;
            }
          }
        }
      }
    }
    //no tags
  }
}

/****************************/
/* ogg infos */

//Pull out and save the 3 header packets from the input file.
//-returns -1 if error and error is set in '*error'
static int splt_ogg_process_headers(splt_ogg_state *oggstate, int *error)
{
  ogg_page page;
  ogg_packet packet;
  int bytes = 0;
  int i = 0;
  char *buffer = NULL;
  oggstate->header_page_number = 0;

  ogg_sync_init(oggstate->sync_in);

  //TODO: how to handle alloc memory problem ?
  vorbis_info_init(oggstate->vi);
  vorbis_comment_init(&oggstate->vc);

  //we read while we don't have a page anymore
  int result = 0;
  while ((result = ogg_sync_pageout(oggstate->sync_in, &page))!=1)
  {
    buffer = ogg_sync_buffer(oggstate->sync_in, SPLT_OGG_BUFSIZE);
    if (buffer == NULL)
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      return -1;
    }
    bytes = fread(buffer, 1, SPLT_OGG_BUFSIZE, oggstate->in);
    if (bytes <= 0)
    {
      goto error_invalid_file;
    }
    if (ogg_sync_wrote(oggstate->sync_in, bytes) != 0)
    {
      goto error_invalid_file;
    }
  }

  oggstate->serial = ogg_page_serialno(&page);
  //TODO: how to handle alloc memory problem ?
  ogg_stream_init(oggstate->stream_in, oggstate->serial);
  if(ogg_stream_pagein(oggstate->stream_in, &page) < 0)
  {
    goto error_invalid_file;
  }
  //TODO: ogg doc says "usually this will not be a fatal error"
  if(ogg_stream_packetout(oggstate->stream_in, &packet)!=1)
  {
    goto error_invalid_file;
  }
  //if bad header
  if(vorbis_synthesis_headerin(oggstate->vi, &oggstate->vc, &packet) < 0)
  {
    goto error_invalid_file;
  }
  int packet_err = SPLT_OK;
  oggstate->headers[0] = splt_ogg_save_packet(&packet, &packet_err);
  if (packet_err < 0)
  { 
    goto error;
  }

  i=0;
  while(i<2)
  {
    while(i<2)
    {
      int res = ogg_sync_pageout(oggstate->sync_in, &page);
      //res == -1 is NOT a fatal error
      if(res==0)
      {
        break;
      }
      if(res==1)
      {
        //we count header page numbers
        long page_number = ogg_page_pageno(&page);
        oggstate->header_page_number=page_number;

        if (ogg_stream_pagein(oggstate->stream_in, &page) < 0)
        {
          goto error_invalid_file;
        }
        while(i<2)
        {
          res = ogg_stream_packetout(oggstate->stream_in, &packet);
          if(res==0)
          {
            break;
          }
          //TODO: ogg doc says "usually this will not be a fatal error"
          if(res<0)
          {
            goto error_invalid_file;
          }
          oggstate->headers[i+1] = splt_ogg_save_packet(&packet, &packet_err);
          if (packet_err < 0)
          {
            goto error;
          }
          //if bad header
          if (vorbis_synthesis_headerin(oggstate->vi,&oggstate->vc,&packet) < 0)
          {
            goto error_invalid_file;
          }
          i++;
        }
      }
    }

    buffer=ogg_sync_buffer(oggstate->sync_in, SPLT_OGG_BUFSIZE);
    if (buffer == NULL)
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      goto error;
    }
    bytes=fread(buffer,1,SPLT_OGG_BUFSIZE,oggstate->in);

    if(bytes == 0 && i < 2)
    {
      goto error_invalid_file;
    }
    if (ogg_sync_wrote(oggstate->sync_in, bytes) != 0)
    {
      *error = SPLT_ERROR_INVALID;
      goto error;
    }
  }             

  return 0;

error_invalid_file:
  *error = SPLT_ERROR_INVALID;
error:
  return -1;
}

//returns ogg info
splt_ogg_state *splt_ogg_info(FILE *in, splt_state *state, int *error)
{
  splt_ogg_state *oggstate = state->codec;

  oggstate = splt_ogg_v_new(error);
  if (oggstate == NULL) { return NULL; }

  char *filename = splt_t_get_filename_to_split(state);

  oggstate->in = in;
  oggstate->end = 0;

  //open the file
  if (oggstate->in != stdin)
  {
    int ret = ov_open(oggstate->in, &oggstate->vf, NULL, 0);
    if(ret < 0)
    {
      splt_t_set_error_data(state,filename);
      switch (ret)
      {
        case OV_EREAD:
          *error = SPLT_ERROR_WHILE_READING_FILE;
          break;
        default:
          *error = SPLT_ERROR_INVALID;
          break;
      }
      splt_ogg_v_free(oggstate);
      return NULL;
    }
    //go at the start of the file
    rewind(oggstate->in);
  }

  /* Read headers in, and save them */
  if(splt_ogg_process_headers(oggstate, error) == -1)
  {
    if (*error == SPLT_ERROR_INVALID)
    {
      splt_t_set_error_data(state,filename);
    }
    splt_ogg_v_free(oggstate);
    return NULL;
  }

  if (oggstate->in != stdin)
  {
    //read total time
    long total_time = ov_time_total(&oggstate->vf, -1) * 100;
    splt_t_set_total_time(state,total_time);
    oggstate->len = (ogg_int64_t) (oggstate->vi->rate*total_time);
  }

  oggstate->cutpoint_begin = 0;
  //TODO: what to do if no memory ?
  //- we must free memory from 'vorbis_synthesis_init' ?
  vorbis_synthesis_init(oggstate->vd,oggstate->vi);
  vorbis_block_init(oggstate->vd,oggstate->vb);

  srand(time(NULL));

  return oggstate;
}

/****************************/
/* ogg split */

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
static int splt_ogg_find_begin_cutpoint(splt_state *state, splt_ogg_state *oggstate, 
    FILE *in, ogg_int64_t cutpoint, int *error, const char *filename)
{
  int eos=0;
  ogg_page page;
  ogg_packet packet;
  ogg_int64_t granpos, prevgranpos;
  int result;

  granpos = prevgranpos = 0;

  int packet_err = SPLT_OK;
  short is_stream = SPLT_FALSE;
  //if we are at the first header
  short first_time = SPLT_TRUE;
  while(!eos)
  {
    while(!eos)
    {
      int result = ogg_sync_pageout(oggstate->sync_in, &page);
      //result == -1 is NOT a fatal error
      if(result==0) 
      {
        break;
      }
      else 
      {
        //result==1 means that we have a good page
        if(result>0)
        {
          //for streams recorded in the middle
          //we add the current granpos
          if (first_time)
          {
            granpos = ogg_page_granulepos(&page);
            //we take the page number
            long page_number = ogg_page_pageno(&page);
            //if the page number > header_page+1, probably a stream
            if (page_number > (oggstate->header_page_number+1))
            {
              is_stream = SPLT_TRUE;
              cutpoint += granpos;
            }
            first_time = SPLT_FALSE;
          }
          else
          {
            //find the granule pos, that we will compare with
            //our cutpoint
            granpos = ogg_page_granulepos(&page);
          }

          //-1 means failure
          if (ogg_stream_pagein(oggstate->stream_in, &page) == -1)
          {
            *error = SPLT_ERROR_INVALID;
            return -1;
          }

          //for a broken ogg file with no
          //header, we have granpos > cutpoint the first time
          if(granpos < cutpoint)
          {
            while(1)
            {
              result=ogg_stream_packetout(oggstate->stream_in, &packet);
              /* throw away result, but update state */
              splt_ogg_get_blocksize(oggstate,oggstate->vi,&packet);

              //result == -1 is not a fatal error
              if(result==0) 
              {
                break;
              }
              else 
              {
                if(result != -1)
                {
                  /* We need to save the last packet in the first
                   * stream - but we don't know when we're going
                   * to get there. So we have to keep every packet
                   * just in case.
                   */
                  splt_ogg_free_packet(oggstate->packets[0]);
                  oggstate->packets[0] = splt_ogg_save_packet(&packet, &packet_err);
                  if (packet_err < 0) { return -1; }
                }
              }
            }

            prevgranpos = granpos;
          }
          else
          {
            eos=1; /* First stream ends somewhere in this page.
            //We break of out this loop here. */
          }

          if(ogg_page_eos(&page))
          {
            eos=1;
          }
        }
      }
    }
    if(!eos)
    {
      int sync_bytes = splt_ogg_update_sync(state, oggstate->sync_in, in, error);
      if (sync_bytes == 0)
      {
        eos=1;
      }
      else if (sync_bytes == -1)
      {
        return -1;
      }
    }
  }

  /* Now, check to see if we reached a real EOS */
  if(granpos < cutpoint)
  {
    *error = SPLT_ERROR_BEGIN_OUT_OF_FILE;
    return -1; // Cutpoint is out of file
  }

  while((result = ogg_stream_packetout(oggstate->stream_in, &packet))
      !=0)
  {
    //if == -1, we are out of sync; not a fatal error
    if (result != -1)
    {
      int bs;

      bs = splt_ogg_get_blocksize(oggstate, oggstate->vi, &packet);
      prevgranpos += bs;

      if(prevgranpos > cutpoint)
      {
        oggstate->packets[1] = splt_ogg_save_packet(&packet, &packet_err);
        if (packet_err < 0) { return -1; }
        break;
      }

      splt_ogg_free_packet(oggstate->packets[0]);
      oggstate->packets[0] = splt_ogg_save_packet(&packet, &packet_err);
      if (packet_err < 0) { return -1; }
    }
  }

  /* Remaining samples in first packet */
  oggstate->initialgranpos = prevgranpos - cutpoint;
  oggstate->cutpoint_begin = cutpoint;

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
static int splt_ogg_find_end_cutpoint(splt_state *state, ogg_stream_state *stream,
    FILE *in, FILE *f, ogg_int64_t cutpoint, short adjust, float threshold,
    int *error, const char *output_fname)
{
  splt_t_put_progress_text(state,SPLT_PROGRESS_CREATE);

  //for the progress
  int progress_adjust = 1;

  splt_ogg_state *oggstate = state->codec;

  ogg_packet packet;
  ogg_page page;
  int eos=0;
  int result = 0;
  ogg_int64_t page_granpos = 0, current_granpos = 0, prev_granpos = 0;
  ogg_int64_t packetnum=0; /* Should this start from 0 or 3 ? */

  char *filename = splt_t_get_filename_to_split(state);

  if(oggstate->packets[0] && oggstate->packets[1])
  { // Check if we have the 2 packet, begin can be 0!
    packet.bytes = oggstate->packets[0]->length;
    packet.packet = oggstate->packets[0]->packet;
    packet.b_o_s = 0;
    packet.e_o_s = 0;
    packet.granulepos = 0;
    packet.packetno = packetnum++;
    ogg_stream_packetin(stream,&packet);

    packet.bytes = oggstate->packets[1]->length;
    packet.packet = oggstate->packets[1]->packet;
    packet.b_o_s = 0;
    packet.e_o_s = 0;
    packet.granulepos = oggstate->initialgranpos;
    packet.packetno = packetnum++;
    ogg_stream_packetin(stream,&packet);

    if(ogg_stream_flush(stream, &page)!=0)
    {
      if (fwrite(page.header,1,page.header_len,f) < page.header_len)
      {
        goto write_error;
      }
      if (fwrite(page.body,1,page.body_len,f) < page.body_len)
      {
        goto write_error;
      }
    }

    while(ogg_stream_flush(stream, &page) != 0)
    {
      /* Might this happen for _really_ high bitrate modes, if we're
       * spectacularly unlucky? Doubt it, but let's check for it just
       * in case.
       */
      //fprintf(stderr, "Warning: First audio packet didn't fit into page. File may not decode correctly\n");
      if (fwrite(page.header,1,page.header_len,f) < page.header_len)
      {
        goto write_error;
      }
      if (fwrite(page.body,1,page.body_len,f) < page.body_len)
      {
        goto write_error;
      }
    }
  }
  else 
  {
    oggstate->initialgranpos = 0;
  }

  current_granpos = oggstate->initialgranpos;
  int packet_err = SPLT_OK;

  while(!eos)
  {
    while(!eos)
    {
      result = ogg_sync_pageout(oggstate->sync_in, &page);

      //result == -1 is NOT a fatal error
      if(result==0)
      {
        break;
      }
      else
      {
        if(result != -1)
        {
          page_granpos = ogg_page_granulepos(&page) - oggstate->cutpoint_begin;

          if(ogg_page_eos(&page)) 
          {
            eos=1;
          }

          if (ogg_stream_pagein(oggstate->stream_in, &page) == -1)
          {
            *error = SPLT_ERROR_INVALID;
            return -1;
          }

          if ((cutpoint == 0) || (page_granpos < cutpoint))
          {
            while(1)
            {
              result = ogg_stream_packetout(oggstate->stream_in, &packet);
              //result == -1 is not a fatal error
              if(result==0) 
              {
                break;
              }
              else
              {
                if(result != -1)
                {
                  int bs = splt_ogg_get_blocksize(oggstate, oggstate->vi, &packet);
                  current_granpos += bs;
                  /* We need to save packet to optimize following split process */
                  splt_ogg_free_packet(oggstate->packets[0]);
                  oggstate->packets[0] = splt_ogg_save_packet(&packet, &packet_err);
                  if (packet_err < 0) { return -1; }
                  if(current_granpos > page_granpos)
                  {
                    current_granpos = page_granpos;
                  }
                  packet.granulepos = current_granpos;
                  packet.packetno = packetnum++;

                  //progress
                  if ((splt_t_get_int_option(state,SPLT_OPT_SPLIT_MODE)
                        == SPLT_OPTION_SILENCE_MODE) ||
                      (!splt_t_get_int_option(state,SPLT_OPT_AUTO_ADJUST)))
                  {
                    splt_t_update_progress(state, (float)page_granpos,
                        (float)cutpoint,
                        1,0,SPLT_DEFAULT_PROGRESS_RATE);
                  }
                  else
                  {
                    splt_t_update_progress(state, (float)page_granpos,
                        (float)cutpoint,
                        2,0,SPLT_DEFAULT_PROGRESS_RATE);
                  }

                  ogg_stream_packetin(stream, &packet);
                  if (splt_ogg_write_pages_to_file(state, stream,f, 0,
                        error, output_fname))
                  {
                    return -1;
                  }
                }
              }
            }

            prev_granpos = page_granpos;
          }
          else 
          {
            if (adjust)
            {
              if (splt_ogg_scan_silence(state,
                    (2 * adjust), threshold, 0.f, 0, &page, current_granpos, error) > 0)
              {
                cutpoint = (splt_u_silence_position(state->silence_list, 
                      oggstate->off) * oggstate->vi->rate);
              }
              else
              {
                cutpoint = (cutpoint + (adjust * oggstate->vi->rate));
              }

              splt_t_ssplit_free(&state->silence_list);
              adjust=0;
              progress_adjust = 0;
              splt_t_put_progress_text(state, SPLT_PROGRESS_CREATE);

              //check error of 'splt_ogg_scan_silence'
              if (*error < 0) { return -1; }
            }
            else 
            {
              eos=1; /* We reached the second cutpoint */
            }
          }
          if(ogg_page_eos(&page))
          {
            eos=1;
          }
        }
      }
    }
    if(!eos)
    {
      int sync_bytes = splt_ogg_update_sync(state, oggstate->sync_in, in, error);
      if (sync_bytes == 0)
      {
        eos=1;
      }
      else if (sync_bytes == -1)
      {
        return -1;
      }
    }
  }

  if ((cutpoint == 0) || (page_granpos < cutpoint)) // End of file. We stop here
  {
    if(splt_ogg_write_pages_to_file(state, stream,f, 0, error, output_fname))
    {
      return -1;
    }
    oggstate->end = -1; // No more data available. Next processes aborted
    return 0;
  }

  while((result = ogg_stream_packetout(oggstate->stream_in, &packet))
      !=0)
  {
    //if == -1, we are out of sync; not a fatal error
    if (result != -1)
    {
      int bs;
      bs = splt_ogg_get_blocksize(oggstate, oggstate->vi, &packet);
      prev_granpos += bs;

      if(prev_granpos >= cutpoint)
      {
        //we free possible previous packets
        splt_ogg_free_packet(oggstate->packets[1]);
        oggstate->packets[1] = splt_ogg_save_packet(&packet, &packet_err);
        if (packet_err < 0) { return -1; }
        packet.granulepos = cutpoint; /* Set it! This 'truncates' the final packet, as needed. */
        packet.e_o_s = 1;
        ogg_stream_packetin(stream, &packet);
        break;
      }

      splt_ogg_free_packet(oggstate->packets[0]);
      oggstate->packets[0] = splt_ogg_save_packet(&packet, &packet_err);
      if (packet_err < 0) { return -1; }

      ogg_stream_packetin(stream, &packet);
      if(splt_ogg_write_pages_to_file(state, stream,f, 0, error, output_fname))
      {
        return -1;
      }
    }
  }

  if(splt_ogg_write_pages_to_file(state, stream,f, 0, error, output_fname))
  {
    return -1;
  }

  oggstate->initialgranpos = prev_granpos - cutpoint;
  oggstate->end = 1;
  oggstate->cutpoint_begin += cutpoint;

  return 0;

write_error:
  splt_t_set_strerror_msg(state);
  splt_t_set_error_data(state, output_fname);
  *error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
  return -1;
}

//splits ogg
void splt_ogg_split(const char *output_fname, splt_state *state, double
    sec_begin, double sec_end, short seekable, 
    short adjust, float threshold, int *error)
{
  //we do the next split
  splt_t_current_split_next(state);
  splt_ogg_state *oggstate = state->codec;

  ogg_stream_state stream_out;
  ogg_packet header_comm;
  ogg_int64_t begin,end = 0, cutpoint = 0;

  begin = (ogg_int64_t) (sec_begin * oggstate->vi->rate);

  char *filename = splt_t_get_filename_to_split(state);

  if (sec_end != -1.f)
  {
    if (adjust)
    {
      if (sec_end != -1)
      {
        float gap = (float) adjust;
        if (sec_end > gap)
        {
          sec_end -= gap;
        }
        if (sec_end < sec_begin)
        {
          sec_end = sec_begin;
        }
      }
      else 
      {
        adjust = 0;
      }
    }
    end = (ogg_int64_t) (sec_end * oggstate->vi->rate);
    cutpoint = end - begin;
  }

  // First time we run this, no packets already saved.
  if (oggstate->end == 0)
  {
    // We must do this before. If an error occurs, we don't want to create empty files!
    //we find the begin cutpoint
    if (splt_ogg_find_begin_cutpoint(state,
          oggstate, oggstate->in, begin, error, filename) < 0)
    {
      return;
    }
  }

  //- means stdout
  if (strcmp(output_fname, "-")==0)
  {
    oggstate->out = stdout;
#ifdef __WIN32__
    _setmode(fileno(oggstate->out), _O_BINARY);
#endif
  }
  else
  {
    if (!(oggstate->out = fopen(output_fname, "wb")))
    {
      splt_t_set_strerror_msg(state);
      splt_t_set_error_data(state, output_fname);
      *error = SPLT_ERROR_CANNOT_OPEN_DEST_FILE;
      return;
    }
  }

  /* gets random serial number*/
  ogg_stream_init(&stream_out, rand());

  //vorbis memory leak ?
  vorbis_commentheader_out(&oggstate->vc, &header_comm);

  int packet_err = SPLT_OK;
  splt_ogg_free_packet(oggstate->headers[1]);
  oggstate->headers[1] = splt_ogg_save_packet(&header_comm, &packet_err);
  //we clear the packet
  ogg_packet_clear(&header_comm);
  vorbis_comment_clear(&oggstate->vc);
  //check error of 'splt_ogg_save_packet'
  if (packet_err < 0)
  {
    *error = packet_err;
    ogg_stream_clear(&stream_out);
    return;
  }

  splt_ogg_submit_headers_to_stream(&stream_out, oggstate);

  if (splt_ogg_write_pages_to_file(state, &stream_out, oggstate->out, 1,
        error, output_fname) == -1)
  {
    goto end;
  }

  //find end cutpoint and get error
  int result = splt_ogg_find_end_cutpoint(state, &stream_out, oggstate->in, 
      oggstate->out, cutpoint, adjust, threshold, error, output_fname);

end:
  ogg_stream_clear(&stream_out);
  if (oggstate->out != stdout)
  {
    if (fclose(oggstate->out) != 0)
    {
      splt_t_set_strerror_msg(state);
      splt_t_set_error_data(state, output_fname);
      *error = SPLT_ERROR_CANNOT_CLOSE_FILE;
    }
  }
  oggstate->out = NULL;

  if (*error >= 0)
  {
    if (oggstate->end == -1) 
    {
      *error = SPLT_OK_SPLIT_EOF;
      return;
    }

    *error = SPLT_OK_SPLIT;
  }
}

/****************************/
/* ogg scan for silence */

//used by scan_silence
static int splt_ogg_silence(splt_ogg_state *oggstate, vorbis_dsp_state *vd, float threshold)
{
  float **pcm = NULL, sample;
  int samples, silence = 1;

  while((samples=vorbis_synthesis_pcmout(vd,&pcm))>0)
  {
    if (silence) 
    {
      int i, j;
      for (i=0; i < oggstate->vi->channels; i++)
      {
        float  *mono=pcm[i];
        if (!silence) 
        {
          break;
        }
        for(j=0; j<samples; j++)
        {
          sample = fabs(mono[j]);
          oggstate->temp_level = oggstate->temp_level *0.999 + sample*0.001;
          if (sample > threshold)
          {
            silence = 0;
          }
        }
      }
    }
    vorbis_synthesis_read(vd, samples);
  }

  return silence;
}

//scans for silence
int splt_ogg_scan_silence(splt_state *state, short seconds,
    float threshold, float min, short output, 
    ogg_page *page, ogg_int64_t granpos, int *error)
{
  splt_t_put_progress_text(state,SPLT_PROGRESS_SCAN_SILENCE);
  splt_ogg_state *oggstate = state->codec;

  ogg_page og;
  ogg_packet op;
  ogg_sync_state oy;
  ogg_stream_state os;
  vorbis_dsp_state vd;
  vorbis_block vb;
  ogg_int64_t end_position, begin_position, pos, end, begin, page_granpos;
  int eos=0, found = 0, shot, result = 0, len = 0 ;
  short first, flush = 0;
  off_t position = ftello(oggstate->in); // Some backups
  int saveW = oggstate->prevW;
  float th = splt_u_convertfromdB(threshold);

  ogg_sync_init(&oy);
  ogg_stream_init(&os, oggstate->serial);

  char *filename = splt_t_get_filename_to_split(state);

  // We still have a page to process
  if (page)
  {
    memcpy(&og, page, sizeof(ogg_page));
    result = 1;
  }

  end_position = begin_position = pos = granpos;
  vorbis_synthesis_init(&vd, oggstate->vi);
  vorbis_block_init(&vd, &vb);

  if (seconds > 0)
  {
    end = (ogg_int64_t) (seconds * oggstate->vi->rate);
  }
  else 
  {
    end = 0;
  }

  begin = 0;
  first = output;
  shot = SPLT_DEFAULTSHOT;

  oggstate->temp_level = 0.0;

  while (!eos)
  {
    while(!eos)
    {
      if (result==0) 
      {
        break;
      }
      if(result>0)
      {
        if (ogg_page_eos(&og)) 
        {
          eos=1;
        }
        page_granpos = ogg_page_granulepos(&og) - oggstate->cutpoint_begin;
        if (pos == 0) 
        {
          pos = page_granpos;
        }
        ogg_stream_pagein(&os, &og);
        while(1)
        {
          result=ogg_stream_packetout(&os, &op);
          /* need more data */
          if(result==0) 
          {
            break;
          }
          if(result>0)
          {
            int bs = splt_ogg_get_blocksize(oggstate, oggstate->vi, &op);
            pos += bs;                      
            if (pos > page_granpos)
            {
              pos = page_granpos;
            }
            begin += bs;
            if (vorbis_synthesis(&vb, &op) == 0)
            {
              vorbis_synthesis_blockin(&vd, &vb);
              if ((!flush) && (splt_ogg_silence(oggstate, &vd, th))) 
              {
                if (len == 0) 
                {
                  begin_position = pos;
                }
                if (first == 0) 
                {
                  len++;
                }
                if (shot < SPLT_DEFAULTSHOT)
                {
                  shot+=2;
                }
                end_position = pos;
              }
              else 
              {
                if (len > SPLT_DEFAULTSILLEN)
                {
                  if ((flush) || (shot <= 0))
                  {
                    float b_position, e_position;
                    double temp;
                    temp = (double) begin_position;
                    temp /= oggstate->vi->rate;
                    b_position = (float) temp;
                    temp = (double) end_position;
                    temp /= oggstate->vi->rate;
                    e_position = (float) temp;
                    if ((e_position - b_position - min) >= 0.f)
                    {
                      int err = SPLT_OK;
                      if (splt_t_ssplit_new(&state->silence_list, b_position, e_position, len, &err) == -1)
                      {
                        found = -1;
                        goto function_end;
                      }
                      found++;
                    }
                    len = 0;
                    shot = SPLT_DEFAULTSHOT;
                  }
                } 
                else 
                {
                  len = 0;
                }
                if (flush)
                {
                  eos = 1;
                  break;
                }
                if ((first) && (shot <= 0))
                {
                  first = 0;
                }
                if (shot > 0) 
                {
                  shot--;
                }
              }
            }
            else
            {
              *error = SPLT_ERROR_INVALID;
              splt_t_set_error_data(state,filename);
              found = -1;
              goto function_end;
            }
          }
          if (end)
          {
            if (begin > end)
            {
              flush = 1;
            }
          }
          if (found >= SPLT_MAXSILENCE) 
          {
            eos = 1;
          }
        }
      }
      result = ogg_sync_pageout(&oy, &og);
      //result == -1 is NOT a fatal error
    }
    if(!eos)
    {
      int sync_bytes = splt_ogg_update_sync(state, &oy, oggstate->in, error);
      if (sync_bytes == 0)
      {
        eos=1;
      }
      else if (sync_bytes == -1)
      {
        return -1;
      }

      result = ogg_sync_pageout(&oy, &og);
      //result == -1 is NOT a fatal error

      //progress bar
      if (splt_t_get_int_option(state,SPLT_OPT_SPLIT_MODE) == SPLT_OPTION_SILENCE_MODE)
      {
        float level = splt_u_convert2dB(oggstate->temp_level);
        if (state->split.get_silence_level)
        {
          state->split.get_silence_level(level, state->split.silence_level_client_data);
        }
        state->split.p_bar->silence_db_level = level;
        state->split.p_bar->silence_found_tracks = found;

        //if we have cancelled the split
        if (splt_t_split_is_canceled(state))
        {
          eos = 1;
        }
        splt_t_update_progress(state,(float)pos * 100,
            (float)(oggstate->len),
            1,0,SPLT_DEFAULT_PROGRESS_RATE2);
      }
      else
      {
        splt_t_update_progress(state,(float)begin,
            (float)end, 2,0.5,SPLT_DEFAULT_PROGRESS_RATE2);
      }
    }
  }

function_end:

  ogg_stream_clear(&os);

  vorbis_block_clear(&vb);
  vorbis_dsp_clear(&vd);
  ogg_sync_clear(&oy);

  oggstate->prevW = saveW;
  if (fseeko(oggstate->in, position, SEEK_SET) == -1)
  {
    splt_t_set_strerror_msg(state);
    splt_t_set_error_data(state, filename);
    *error = SPLT_ERROR_SEEKING_FILE;
    found = -1;
  }

  return found;
}

/****************************/
/* External plugin API */

//returns the plugin infos (name, version, extension)
//-alloced data in splt_plugin_info will be freed at the end of the program
void splt_pl_set_plugin_info(splt_plugin_info *info, int *error)
{
  float plugin_version = 0.1;

  //set plugin version
  info->version = plugin_version;

  //set plugin name
  info->name = malloc(sizeof(char) * 40);
  if (info->name != NULL)
  {
    snprintf(info->name, 39, "ogg vorbis (libvorbis)");
  }
  else
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return;
  }

  //set plugin extension
  info->extension = malloc(sizeof(char) * (strlen(SPLT_OGGEXT)+2));
  if (info->extension != NULL)
  {
    snprintf(info->extension, strlen(SPLT_OGGEXT)+1, SPLT_OGGEXT);
  }
  else
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return;
  }
}

//check if file is ogg vorbis
int splt_pl_check_plugin_is_for_file(splt_state *state, int *error)
{
  char *filename = splt_t_get_filename_to_split(state);

  //o- means stdin ogg format
  if (strcmp(filename,"o-") == 0)
  {
    return SPLT_TRUE;
  }

  int is_ogg = SPLT_FALSE;
  OggVorbis_File ogg_file;

  FILE *file_input = NULL;

  if ((file_input = fopen(filename, "rb")) == NULL)
  {
    splt_t_set_strerror_msg(state);
    splt_t_set_error_data(state,filename);
    *error = SPLT_ERROR_CANNOT_OPEN_FILE;
  }
  else
  {
    //check if the file is ogg vorbis
    if ((ov_test(file_input, &ogg_file, NULL, 0) + 1) == 1)
    {
      is_ogg = SPLT_TRUE;
      ov_clear(&ogg_file);
    }
    else
    {
      if (file_input != stdin)
      {
        if (fclose(file_input) != 0)
        {
          splt_t_set_strerror_msg(state);
          splt_t_set_error_data(state, filename);
          *error = SPLT_ERROR_CANNOT_CLOSE_FILE;
        }
      }
      file_input = NULL;
    }
  }

  return is_ogg;
}

void splt_pl_init(splt_state *state, int *error)
{
  FILE *file_input = NULL;
  char *filename = splt_t_get_filename_to_split(state);

  //if we can open the file
  if ((file_input = splt_ogg_open_file_read(state, filename, error)) != NULL)
  {
    splt_ogg_get_info(state, file_input, error);
    if (*error >= 0)
    {
      splt_ogg_state *oggstate = state->codec;
      oggstate->off = splt_t_get_float_option(state,SPLT_OPT_PARAM_OFFSET);
    }
  }
}

void splt_pl_end(splt_state *state, int *error)
{
  splt_ogg_state *oggstate = state->codec;
  splt_ogg_state_free(state);
}

void splt_pl_split(splt_state *state, const char *final_fname,
    double begin_point, double end_point, int *error) 
{
  splt_ogg_state *oggstate = state->codec;

  splt_ogg_put_tags(state, error);

  if (*error >= 0)
  {
    //effective ogg split
    splt_ogg_split(final_fname, state,
        begin_point, end_point,
        !state->options.option_input_not_seekable,
        state->options.parameter_gap,
        state->options.parameter_threshold, error);
  }
}

int splt_pl_scan_silence(splt_state *state, int *error)
{
  float offset = splt_t_get_float_option(state,SPLT_OPT_PARAM_OFFSET);
  float threshold = splt_t_get_float_option(state, SPLT_OPT_PARAM_THRESHOLD);
  float min_length = splt_t_get_float_option(state, SPLT_OPT_PARAM_MIN_LENGTH);
  int found = 0;

  splt_ogg_state *oggstate = state->codec;
  oggstate->off = offset;

  found = splt_ogg_scan_silence(state, 0, threshold, min_length, 1, NULL, 0, error);
  if (*error < 0) { return -1; }

  return found;
}

void splt_pl_set_original_tags(splt_state *state, int *error)
{
  char *filename = splt_t_get_filename_to_split(state);

  splt_u_print_debug("Putting ogg original tags...",0,NULL);
  splt_ogg_get_original_tags(filename, state, error);
}

