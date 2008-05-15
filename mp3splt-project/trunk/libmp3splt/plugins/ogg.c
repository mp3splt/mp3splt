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

#include <time.h>
#include <string.h>
#include <math.h>
#include <locale.h>

#include "ogg.h"

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

//gets the mp3 info and puts it in the state
splt_state *splt_ogg_get_info(splt_state *state, FILE *file_input, int *error)
{
  //checks if valid ogg file
  state->codec = splt_ogg_info(file_input, state->codec, error);

  //if error
  if ((*error < 0) ||
      (state->codec == NULL))
  {
    return NULL;
  }
  else
  {
    if (! splt_t_messages_locked(state))
    {
      splt_ogg_state *oggstate = (splt_ogg_state *) state->codec;
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

  return state;
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
static splt_v_packet *splt_ogg_save_packet(ogg_packet *packet)
{
  splt_v_packet *p = NULL;

  //if we have no header, we will have bytes < 0
  p = malloc(sizeof(splt_v_packet));

  p->length = packet->bytes;
  p->packet = malloc(p->length);
  memcpy(p->packet, packet->packet, p->length);

  return p;
}

//frees a packet
static void splt_ogg_free_packet(splt_v_packet *p)
{
  if(p)
  {
    if(p->packet)
      free(p->packet);
    free(p);
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

static int splt_ogg_update_sync(ogg_sync_state *sync_in, FILE *f)
{
  char *buffer = ogg_sync_buffer(sync_in, SPLT_OGG_BUFSIZE);
  int bytes = fread(buffer,1,SPLT_OGG_BUFSIZE,f);

  ogg_sync_wrote(sync_in, bytes);

  return bytes;
}

/* Returns 0 for success, or -1 on failure. */
static int splt_ogg_write_pages_to_file(ogg_stream_state *stream, 
    FILE *file, int flush)
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
      if(oggstate->packets[0])
        splt_ogg_free_packet(oggstate->packets[0]);
      if(oggstate->packets[1])
        splt_ogg_free_packet(oggstate->packets[1]);
      free(oggstate->packets);
    }
    if(oggstate->headers)
    {
      int i;
      for(i=0; i < 3; i++)
        if(oggstate->headers[i])
          splt_ogg_free_packet(oggstate->headers[i]);
      free(oggstate->headers);
    }
    vorbis_comment_clear(&oggstate->vc);
    if(oggstate->vb)
    {
      vorbis_block_clear(oggstate->vb);
      free(oggstate->vb);
    }
    if(oggstate->vd)
    {
      vorbis_dsp_clear(oggstate->vd);
      free(oggstate->vd);
    }
    if(oggstate->stream_in)
    {
      ogg_stream_clear(oggstate->stream_in);
      free(oggstate->stream_in);
    }
    if(oggstate->sync_in)
    {
      ogg_sync_clear(oggstate->sync_in);
      free(oggstate->sync_in);
    }
    if (oggstate->vi)
    {
      vorbis_info_clear(oggstate->vi);
      free(oggstate->vi);
    }
    free(oggstate);
  }
}

static splt_ogg_state *splt_ogg_v_new(void)
{
  splt_ogg_state *oggstate;

  if ((oggstate = malloc(sizeof(splt_ogg_state)))==NULL)
    return NULL;
  memset(oggstate, 0, sizeof(splt_ogg_state));
  if ((oggstate->sync_in = malloc(sizeof(ogg_sync_state)))==NULL)
  {
    splt_ogg_v_free(oggstate);
    return NULL;
  }
  if ((oggstate->stream_in = malloc(sizeof(ogg_stream_state)))==NULL)
  {
    splt_ogg_v_free(oggstate);
    return NULL;
  }
  if ((oggstate->vd = malloc(sizeof(vorbis_dsp_state)))==NULL)
  {
    splt_ogg_v_free(oggstate);
    return NULL;
  }
  if ((oggstate->vi = malloc(sizeof(vorbis_info)))==NULL)
  {
    splt_ogg_v_free(oggstate);
    return NULL;
  }
  if ((oggstate->vb = malloc(sizeof(vorbis_block)))==NULL)
  {
    splt_ogg_v_free(oggstate);
    return NULL;
  }
  if ((oggstate->headers = malloc(sizeof(splt_v_packet)*3))==NULL)
  {
    splt_ogg_v_free(oggstate);
    return NULL;
  }
  memset(oggstate->headers, 0, sizeof(splt_v_packet)*3);
  if ((oggstate->packets = malloc(sizeof(splt_v_packet)*2))==NULL)
  {
    splt_ogg_v_free(oggstate);
    return NULL;
  }
  memset(oggstate->packets, 0, sizeof(splt_v_packet)*2);

  return oggstate;
}

//frees the splt_ogg_state structure,
//used in the splt_t_state_free() function
void splt_ogg_state_free(splt_state *state)
{
  splt_ogg_state *oggstate = (splt_ogg_state *) state->codec;
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
static vorbis_comment *splt_ogg_v_comment (vorbis_comment *vc, char *artist, char *album, char *title, 
    char *tracknum, char *date, char *genre, char *comment)
{
  if (title!=NULL)
    vorbis_comment_add_tag(vc, "title",
        splt_ogg_checkutf(title));
  if (artist!=NULL)
    vorbis_comment_add_tag(vc, "artist",
        splt_ogg_checkutf(artist));
  if (album!=NULL)
    vorbis_comment_add_tag(vc, "album",
        splt_ogg_checkutf(album));
  if (date!=NULL)
    if (strlen(date)>0)
      vorbis_comment_add_tag(vc, "date", date);
  if (genre!=NULL)
    vorbis_comment_add_tag(vc, "genre", genre);
  if (tracknum!=NULL)
    vorbis_comment_add_tag(vc, "tracknumber", tracknum);
  if (comment!=NULL)
    vorbis_comment_add_tag(vc, "comment", comment);

  return vc;
}

//get the original ogg tags and put them in the state
void splt_ogg_get_original_tags(char *filename,
    splt_state *state, int *tag_error)
{
  FILE *file_input;

  //if we can open the file
  if ((file_input = fopen(filename, "rb")) != NULL)
  {
    splt_t_lock_messages(state);
    if(splt_ogg_get_info(state, file_input, tag_error) != NULL)
    {
      splt_t_unlock_messages(state);
      splt_ogg_state *oggstate = (splt_ogg_state *) state->codec;

      vorbis_comment *vc_local;
      vc_local = ov_comment(&oggstate->vf,-1);

      char *a = NULL,*t = NULL,*al = NULL,*da = NULL,
           *g = NULL,*tr = NULL,*com = NULL;

      int size = 0;
      a = vorbis_comment_query(vc_local, "artist",0);
      if (a != NULL)
      {
        size = strlen(a);
      }
      else
      {
        size = 0;
      }
      splt_t_set_original_tags_field(state,SPLT_TAGS_ARTIST,
          0,a,0x0,size);
      t = vorbis_comment_query(vc_local, "title",0);
      if (t != NULL)
      {
        size = strlen(t);
      }
      else
      {
        size = 0;
      }
      splt_t_set_original_tags_field(state,SPLT_TAGS_TITLE,
          0,t,0x0,size);

      al = vorbis_comment_query(vc_local, "album",0);
      if (al != NULL)
      {
        size = strlen(al);
      }
      else
      {
        size = 0;
      }
      splt_t_set_original_tags_field(state,SPLT_TAGS_ALBUM,
          0,al,0x0,size);
      da = vorbis_comment_query(vc_local, "date",0);
      if (da != NULL)
      {
        size = strlen(da);
      }
      else
      {
        size = 0;
      }
      splt_t_set_original_tags_field(state,SPLT_TAGS_YEAR,
          0,da,0x0,size);
      g = vorbis_comment_query(vc_local, "genre",0);
      if (g != NULL)
      {
        size = strlen(g);
      }
      else
      {
        size = 0;
      }
      splt_t_set_original_tags_field(state,SPLT_TAGS_GENRE,
          0, g, 0x0,size);
      tr = vorbis_comment_query(vc_local, "tracknumber",0);
      if (tr != NULL)
      {
        size = strlen(tr);
      }
      else
      {
        size = 0;
      }
      splt_t_set_original_tags_field(state,SPLT_TAGS_TRACK,
          0,tr, 0x0,size);
      com = vorbis_comment_query(vc_local, "comment",0);
      if (com != NULL)
      {
        size = strlen(com);
      }
      else
      {
        size = 0;
      }
      splt_t_set_original_tags_field(state,SPLT_TAGS_COMMENT,
          0,com,0x0,size);

      splt_ogg_state_free(state);
    }
    else
    {
      splt_t_unlock_messages(state);
      fclose(file_input);
    }
  }
  else
  {
    *tag_error = SPLT_ERROR_CANNOT_OPEN_FILE;
  }
}

//we take the tags from the state and put them in the
//new file
static void splt_ogg_put_original_tags(splt_state *state)
{
  splt_ogg_state *oggstate = (splt_ogg_state *) state->codec;
  char *a,*t,*al,*da,/**g,*tr,*/*com;

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
  /*tr*/NULL,da,/*g*/NULL,com);
}

//puts the ogg tags
void splt_ogg_put_tags(splt_state *state, int *error)
{
  splt_ogg_state *oggstate = (splt_ogg_state *) state->codec;

  //clean_original_id3(state);
  //if we put the original tags
  vorbis_comment_clear(&oggstate->vc);

  if (splt_t_get_int_option(state, SPLT_OPT_TAGS) == SPLT_TAGS_ORIGINAL_FILE)
  {
    splt_ogg_put_original_tags(state);
  }
  else
  {
    //if we put current tags (cddb,cue,...)
    if (splt_t_get_int_option(state, SPLT_OPT_TAGS) == SPLT_CURRENT_TAGS)
    {
      int current_split = splt_t_get_current_split(state);

      //if we set all the tags like the x one
      int tags_after_x_like_x = splt_t_get_int_option(state,SPLT_OPT_ALL_TAGS_LIKE_X_AFTER_X); 
      if ((current_split >= tags_after_x_like_x)
          && (tags_after_x_like_x != -1))
      {
        current_split = tags_after_x_like_x;
      }

      //only if the tags exists for the current split
      if (splt_t_tags_exists(state,current_split))
      {
        char *title = NULL;
        char *artist = NULL;

        title = 
          splt_t_get_tags_char_field(state,current_split,
              SPLT_TAGS_TITLE);
        artist = 
          splt_t_get_tags_char_field(state,current_split,
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

            splt_ogg_v_comment(&oggstate->vc,
                tags[current_split].artist,
                tags[current_split].album,
                tags[current_split].title,
                track_string,
                tags[current_split].year,
                (char *)splt_ogg_genre_list[(int) 
                tags[current_split].genre],
                tags[current_split].comment);
            //free memory
            if (track_string)
            {
              free(track_string);
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
static int splt_ogg_process_headers(splt_ogg_state *oggstate)
{
  ogg_page page;
  ogg_packet packet;
  int bytes;
  int i;
  char *buffer;
  oggstate->header_page_number = 0;

  ogg_sync_init(oggstate->sync_in);

  vorbis_info_init(oggstate->vi);
  vorbis_comment_init(&oggstate->vc);

  //we read while we don't have a page anymore
  int result = 0;
  while ((result=ogg_sync_pageout(oggstate->sync_in, &page))!=1)
  {
    buffer = ogg_sync_buffer(oggstate->sync_in, SPLT_OGG_BUFSIZE);
    if (buffer == NULL)
    {
      return -1;
    }
    bytes = fread(buffer, 1, SPLT_OGG_BUFSIZE, oggstate->in);
    if (bytes <= 0)
    {
      return -1;
    }
    if (ogg_sync_wrote(oggstate->sync_in, bytes) == -1)
    {
      return -1;
    }
  }

  oggstate->serial = ogg_page_serialno(&page);
  ogg_stream_init(oggstate->stream_in, oggstate->serial);
  if(ogg_stream_pagein(oggstate->stream_in, &page) <0)
  {
    return -1;
  }
  if(ogg_stream_packetout(oggstate->stream_in, &packet)!=1)
  {
    return -1;
  }
  if(vorbis_synthesis_headerin(oggstate->vi, &oggstate->vc, &packet)<0)
  {
    return -1;
  }
  oggstate->headers[0] = splt_ogg_save_packet(&packet);

  i=0;
  while(i<2)
  {
    while(i<2)
    {
      int res = ogg_sync_pageout(oggstate->sync_in, &page);
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
          return -1;
        }
        while(i<2)
        {
          res = ogg_stream_packetout(oggstate->stream_in, &packet);
          if(res==0)
          {
            break;
          }
          if(res<0)
          {
            return -1;
          }
          oggstate->headers[i+1] = splt_ogg_save_packet(&packet);
          if (vorbis_synthesis_headerin(oggstate->vi,&oggstate->vc,&packet) < 0)
          {
            return -1;
          }
          i++;
        }
      }
    }

    buffer=ogg_sync_buffer(oggstate->sync_in, SPLT_OGG_BUFSIZE);
    if (buffer == NULL)
    {
      return -1;
    }
    bytes=fread(buffer,1,SPLT_OGG_BUFSIZE,oggstate->in);

    if(bytes==0 && i<2)
    {
      return -1;
    }
    if (ogg_sync_wrote(oggstate->sync_in, bytes) == -1)
    {
      return -1;
    }

  }             

  return 0;
}

//returns ogg info
splt_ogg_state *splt_ogg_info(FILE *in, splt_ogg_state *oggstate,int *error)
{
  oggstate = splt_ogg_v_new();

  if (oggstate == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return NULL;
  }

  oggstate->in = in;
  oggstate->end = 0;

  if (oggstate->in != stdin)
  {
    if(ov_open(oggstate->in, &oggstate->vf, NULL, 0) < 0)
    {
      *error = SPLT_ERROR_INVALID;
      splt_ogg_v_free(oggstate);
      return NULL;
    }
    rewind(oggstate->in);
  }

  /* Read headers in, and save them */
  if(splt_ogg_process_headers(oggstate) == -1)
  {
    *error = SPLT_ERROR_INVALID;
    splt_ogg_v_free(oggstate);
    return NULL;
  }

  if (oggstate->in != stdin)
  {
    oggstate->len = (ogg_int64_t) ((oggstate->vi->rate)*(ov_time_total(&oggstate->vf, -1)));
  }

  oggstate->cutpoint_begin = 0;
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
static int splt_ogg_find_begin_cutpoint(splt_ogg_state *oggstate, 
    FILE *in, ogg_int64_t cutpoint)
{
  int eos=0;
  ogg_page page;
  ogg_packet packet;
  ogg_int64_t granpos, prevgranpos;
  int result;

  granpos = prevgranpos = 0;

  short is_stream = SPLT_FALSE;
  //if we are at the first header
  short first_time = SPLT_TRUE;
  while(!eos)
  {
    while(!eos)
    {
      int result = ogg_sync_pageout(oggstate->sync_in, &page);

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
            break;
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

              if(result==0) 
              {
                break;
              }
              else 
              {
                if(result!=-1)
                {
                  /* We need to save the last packet in the first
                   * stream - but we don't know when we're going
                   * to get there. So we have to keep every packet
                   * just in case.
                   */
                  if(oggstate->packets[0])
                  {
                    splt_ogg_free_packet(oggstate->packets[0]);
                  }
                  oggstate->packets[0] = splt_ogg_save_packet(&packet);
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
      if(splt_ogg_update_sync(oggstate->sync_in, in)==0)
      {
        eos=1;
      }
    }
  }

  /* Now, check to see if we reached a real EOS */
  if(granpos < cutpoint)
  {
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
        oggstate->packets[1] = splt_ogg_save_packet(&packet);
        break;
      }

      if(oggstate->packets[0])
      {
        splt_ogg_free_packet(oggstate->packets[0]);
      }
      oggstate->packets[0] = splt_ogg_save_packet(&packet);
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
static int splt_ogg_find_end_cutpoint(splt_state *state, ogg_stream_state *stream, FILE *in, FILE *f, 
    ogg_int64_t cutpoint, short adjust, float threshold)
{
  splt_t_put_progress_text(state,SPLT_PROGRESS_CREATE);

  //for the progress
  int progress_adjust = 1;

  splt_ogg_state *oggstate = state->codec;

  ogg_packet packet;
  ogg_page page;
  int eos=0;
  int result;
  ogg_int64_t page_granpos = 0, current_granpos = 0, prev_granpos = 0;
  ogg_int64_t packetnum=0; /* Should this start from 0 or 3 ? */

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
      fwrite(page.header,1,page.header_len,f);
      fwrite(page.body,1,page.body_len,f);
    }

    while(ogg_stream_flush(stream, &page)!=0)
    {
      /* Might this happen for _really_ high bitrate modes, if we're
       * spectacularly unlucky? Doubt it, but let's check for it just
       * in case.
       */
      //fprintf(stderr, "Warning: First audio packet didn't fit into page. File may not decode correctly\n");
      fwrite(page.header,1,page.header_len,f);
      fwrite(page.body,1,page.body_len,f);
    }
  }
  else 
  {
    oggstate->initialgranpos = 0;
  }

  current_granpos = oggstate->initialgranpos;

  while(!eos)
  {
    while(!eos)
    {
      result=ogg_sync_pageout(oggstate->sync_in, &page);

      if(result==0)
      {
        break;
      }
      else
      {
        if(result!=-1)
        {
          page_granpos = ogg_page_granulepos(&page) - oggstate->cutpoint_begin;

          if(ogg_page_eos(&page)) 
          {
            eos=1;
          }

          ogg_stream_pagein(oggstate->stream_in, &page);

          if ((cutpoint == 0) || (page_granpos < cutpoint))
          {
            while(1)
            {
              result = ogg_stream_packetout(oggstate->stream_in, &packet);
              if(result==0) 
              {
                break;
              }
              else
              {
                if(result!=-1)
                {
                  int bs = splt_ogg_get_blocksize(oggstate, oggstate->vi, &packet);
                  current_granpos += bs;
                  if(oggstate->packets[0]) /* We need to save packet to optimize following split process */
                  {
                    splt_ogg_free_packet(oggstate->packets[0]);
                  }
                  oggstate->packets[0] = splt_ogg_save_packet(&packet);
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
                  if(splt_ogg_write_pages_to_file(stream,f, 0))
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
                    (2 * adjust), threshold, 0.f, 0, &page, current_granpos) > 0)
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
              splt_t_put_progress_text(state,SPLT_PROGRESS_CREATE);
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
      if(splt_ogg_update_sync(oggstate->sync_in, in)==0)
      {
        eos=1;
      }
    }
  }

  if ((cutpoint == 0) || (page_granpos < cutpoint)) // End of file. We stop here
  {
    if(splt_ogg_write_pages_to_file(stream,f, 0))
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
        if (oggstate->packets[1])
        {
          splt_ogg_free_packet(oggstate->packets[1]);
        }
        oggstate->packets[1] = splt_ogg_save_packet(&packet);
        packet.granulepos = cutpoint; /* Set it! This 'truncates' the final packet, as needed. */
        packet.e_o_s = 1;
        ogg_stream_packetin(stream, &packet);
        break;
      }

      if(oggstate->packets[0])
      {
        splt_ogg_free_packet(oggstate->packets[0]);
      }
      oggstate->packets[0] = splt_ogg_save_packet(&packet);

      ogg_stream_packetin(stream, &packet);
      if(splt_ogg_write_pages_to_file(stream,f, 0))
      {
        return -1;
      }
    }
  }

  if(splt_ogg_write_pages_to_file(stream,f, 0))
  {
    return -1;
  }

  oggstate->initialgranpos = prev_granpos - cutpoint;
  oggstate->end = 1;
  oggstate->cutpoint_begin += cutpoint;

  return 0;
}

//splits ogg
void splt_ogg_split(char *filename, splt_state *state, double
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

  if (sec_end != -1.f)
  {
    if (sec_begin >= sec_end)
    {
      *error = SPLT_ERROR_BEGIN_OUT_OF_FILE;
      return;
    }

    if (adjust)
    {
      if (sec_end != -1)
      {
        float gap = (float) adjust;
        if (sec_end > gap)
          sec_end -= gap;
        if (sec_end < sec_begin)
          sec_end = sec_begin;
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
    int result = splt_ogg_find_begin_cutpoint(oggstate, oggstate->in, begin);
    //if error
    if (result < 0)
    {
      if (result == -1)
      {
        *error = SPLT_ERROR_BEGIN_OUT_OF_FILE;
      }
      else
      {
        splt_u_print_debug("Invalid ogg file in find_begin_cutpoint",0,NULL);
        *error = SPLT_ERROR_INVALID;
      }
      return;
    }
  }

  if (strcmp(filename, "-")==0)
  {
    oggstate->out = stdout;
  }
  else
  {
    if (!(oggstate->out=fopen(filename, "wb")))
    {
      *error = SPLT_ERROR_CANNOT_OPEN_DEST_FILE;
      return;
    }
  }

  /* gets random serial number*/
  ogg_stream_init(&stream_out, rand());

  //vorbis memory leak ?
  vorbis_commentheader_out(&oggstate->vc, &header_comm);
  if (oggstate->headers[1])
  {
    splt_ogg_free_packet(oggstate->headers[1]);
  }

  oggstate->headers[1] = splt_ogg_save_packet(&header_comm);
  //we clear the packet
  ogg_packet_clear(&header_comm);
  vorbis_comment_clear(&oggstate->vc);

  splt_ogg_submit_headers_to_stream(&stream_out, oggstate);

  if(splt_ogg_write_pages_to_file(&stream_out, oggstate->out, 1))
  {
    *error = SPLT_ERROR_BEGIN_OUT_OF_FILE;
    ogg_stream_clear(&stream_out);
    fclose(oggstate->out);
    return;
  }

  //find end cutpoint
  int result = splt_ogg_find_end_cutpoint(state, &stream_out, oggstate->in, 
      oggstate->out, cutpoint, adjust, threshold);
  //if errors
  if(result < 0)
  {
    if (result == -1)
    {
      *error = SPLT_ERROR_BEGIN_OUT_OF_FILE;
    }
    else
    {
      *error = SPLT_ERROR_INVALID;
      splt_u_print_debug("Invalid ogg file in find_end_cutpoint",0,NULL);
    }
    ogg_stream_clear(&stream_out);
    fclose(oggstate->out);
    return;
  }

  ogg_stream_clear(&stream_out);
  fclose(oggstate->out);

  if (oggstate->end == -1) 
  {
    *error = SPLT_OK_SPLITTED_EOF;
    return;
  }

  *error = SPLT_OK_SPLITTED;
}

/****************************/
/* ogg scan for silence */

//used by scan_silence
static int splt_ogg_silence(splt_ogg_state *oggstate, vorbis_dsp_state *vd, float threshold)
{
  float **pcm, sample;
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
    ogg_page *page, ogg_int64_t granpos)
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
  oggstate->avg_level = 0.0;
  oggstate->n_stat = 0.0;

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
            if(vorbis_synthesis(&vb, &op)==0)
            {
              vorbis_synthesis_blockin(&vd, &vb);
              if ((!flush) && (splt_ogg_silence(oggstate, &vd, th))) 
              {
                if (len == 0) 
                  begin_position = pos;
                if (first == 0) 
                  len++;
                if (shot < SPLT_DEFAULTSHOT)
                  shot+=2;
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
                      splt_t_ssplit_new(&state->silence_list, b_position, e_position, len);
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
      result=ogg_sync_pageout(&oy, &og);
    }
    if(!eos)
    {
      if(splt_ogg_update_sync(&oy, oggstate->in)==0)
      {
        eos=1;
      }

      result=ogg_sync_pageout(&oy, &og);

      //progress bar
      if (splt_t_get_int_option(state,SPLT_OPT_SPLIT_MODE)
          == SPLT_OPTION_SILENCE_MODE)
      {
        float level = splt_u_convert2dB(oggstate->temp_level);
        state->split.p_bar->silence_db_level = level;
        state->split.p_bar->silence_found_tracks = found;

        //if we have cancelled the split
        if (splt_t_split_is_canceled(state))
        {
          eos = 1;
        }
        splt_t_update_progress(state,(float)pos,
            (float)(oggstate->len),
            1,0,SPLT_DEFAULT_PROGRESS_RATE2);
      }
      else
      {
        splt_t_update_progress(state,(float)begin,
            (float)end,
            2,0.5,SPLT_DEFAULT_PROGRESS_RATE2);
      }
    }
  }

  ogg_stream_clear(&os);

  vorbis_block_clear(&vb);
  vorbis_dsp_clear(&vd);
  ogg_sync_clear(&oy);

  oggstate->prevW = saveW;
  fseeko(oggstate->in, position, SEEK_SET);

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
  int is_ogg = SPLT_FALSE;
  OggVorbis_File ogg_file;

  FILE *file_input = NULL;

  if((file_input = fopen(filename, "rb")) == NULL)
  {
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
      fclose(file_input);
      file_input = NULL;
    }
  }

  return is_ogg;
}

void splt_pl_set_total_time(splt_state *state, int *error)
{
  FILE *file_input = NULL;
  char *filename = splt_t_get_filename_to_split(state);

  //if we can open the file
  if ((file_input = fopen(filename, "rb")) != NULL)
  {
    OggVorbis_File ogg_file;

    //if we can open file
    if(ov_open(file_input, &ogg_file, NULL, 0) >= 0)
    {
      long temp = ov_time_total(&ogg_file, -1) * 100;
      splt_t_set_total_time(state,temp);
      ov_clear(&ogg_file);
    }
    else
    {
      *error = SPLT_ERROR_CANNOT_OPEN_FILE;
    }
  }
  else
  {
    *error = SPLT_ERROR_CANNOT_OPEN_FILE;
  }
}

void splt_pl_init_split(splt_state *state, int *error)
{
  FILE *file_input = NULL;
  char *filename = splt_t_get_filename_to_split(state);

  //if we can open the file
  if ((file_input = fopen(filename, "rb")) != NULL)
  {
    if (splt_t_get_int_option(state, SPLT_OPT_SPLIT_MODE) ==
        SPLT_OPTION_SILENCE_MODE)
    {
      splt_t_lock_messages(state);
    }
    splt_ogg_get_info(state, file_input, error);
    splt_t_unlock_messages(state);
    if (*error >= 0)
    {
      splt_ogg_state *oggstate = (splt_ogg_state *) state->codec;
      oggstate->off = splt_t_get_float_option(state,SPLT_OPT_PARAM_OFFSET);
    }
    else
    {
      fclose(file_input);
    }
  }
  else
  {
    *error = SPLT_ERROR_CANNOT_OPEN_FILE;
  }
}

void splt_pl_end_split(splt_state *state)
{
  splt_ogg_state *oggstate = (splt_ogg_state *) state->codec;
  splt_ogg_state_free(state);
}

void splt_pl_split(splt_state *state, char *final_fname,
    double begin_point, double end_point, int *error) 
{
  splt_ogg_state *oggstate = (splt_ogg_state *) state->codec;

  splt_ogg_put_tags(state, error);

  //effective ogg split
  splt_ogg_split(final_fname, state,
      begin_point, end_point,
      !state->options.option_input_not_seekable,
      state->options.parameter_gap,
      state->options.parameter_threshold, error);
}

int splt_pl_scan_silence(splt_state *state, int *error)
{
  char *filename = splt_t_get_filename_to_split(state);
  float offset =
    splt_t_get_float_option(state,SPLT_OPT_PARAM_OFFSET);
  float threshold = 
    splt_t_get_float_option(state, SPLT_OPT_PARAM_THRESHOLD);
  float min_length =
    splt_t_get_float_option(state, SPLT_OPT_PARAM_MIN_LENGTH);
  int found = 0;
  FILE *file_input = NULL;

  //open the file
  if ((file_input = fopen(filename, "rb")))
  {
    if(splt_ogg_get_info(state, file_input, error) != NULL)
    {
      splt_ogg_state *oggstate = (splt_ogg_state *) state->codec;
      oggstate->off = offset;

      found = splt_ogg_scan_silence(state, 0, threshold,
            min_length, 1, NULL, 0);

      splt_ogg_state_free(state);
    }
    else
    {
      *error = SPLT_ERROR_INVALID;
      fclose(file_input);
    }
    //we don't need to close the file_input becase ov_clear() does it
    //when we call splt_ogg_state_free(..)
    /*fclose(file_input);
    file_input = NULL;*/
  }
  else
  {
    *error = SPLT_ERROR_WHILE_READING_FILE;
  }

  return found;
}

void splt_pl_set_original_tags(splt_state *state, int *error)
{
  char *filename = splt_t_get_filename_to_split(state);

  splt_u_print_debug("Putting ogg original tags...\n",0,NULL);
  splt_ogg_get_original_tags(filename, state, error);
}

