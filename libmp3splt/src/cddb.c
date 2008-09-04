/**********************************************************
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2008 Alexandru Munteanu - io_fx@yahoo.fr
 *
 *********************************************************/

/**********************************************************
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
 * 02111-1307, USA.
 *********************************************************/

#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <math.h>

#ifdef __WIN32__
#include <conio.h>
#include <winsock.h>
#else
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#endif

#include "splt.h"

/******************************************/
/* common cddb and cue */

//used by cddb and cue functions
//the cddb and cue functions does not fill in all the tags, this
//function completes the fill
//and calls another function that sets the output filenames
static void splt_tag_put_filenames_from_tags(splt_state *state,
    int tracks, int *error)
{
  int i = 0;
  char *artist0 = NULL;
  artist0 = strdup(splt_t_get_tags_char_field(state, 0, SPLT_TAGS_ARTIST));
  if (artist0 == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    goto function_end;
  }

  char *album0 = NULL;
  album0 = strdup(splt_t_get_tags_char_field(state, 0, SPLT_TAGS_ALBUM));
  if (album0 == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    goto function_end;
  }

  char *year0 = NULL;
  if (splt_t_get_tags_char_field(state, 0, SPLT_TAGS_YEAR))
  {
    year0 = strdup(splt_t_get_tags_char_field(state, 0, SPLT_TAGS_YEAR));
    if (year0 == NULL)
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      goto function_end;
    }
  }

  char *performer = NULL;
  unsigned char genre0 = splt_t_get_tags_uchar_field(state, 0, SPLT_TAGS_GENRE);
  int tags_error = SPLT_OK;

  //if we have the defaults for the output,
  int output_filenames = 
    splt_t_get_int_option(state, SPLT_OPT_OUTPUT_FILENAMES);
  if (output_filenames == SPLT_OUTPUT_DEFAULT)
  {
    //we put the default output if we have the default output
    int err = splt_t_new_oformat(state, SPLT_DEFAULT_OUTPUT);
    if (err < 0) { *error = err; goto function_end; }

    //we put the real performer in the artist
    for (i = 0; i < tracks;i++)
    {
      performer = splt_t_get_tags_char_field(state, i, SPLT_TAGS_PERFORMER);
      //we put performer if found
      if ((performer != NULL) && (performer[0] != '\0'))
      {
        tags_error = splt_t_set_tags_char_field(state, i, SPLT_TAGS_ARTIST,
              performer);
        if (tags_error != SPLT_OK)
        {
          *error = tags_error;
          goto function_end;
        }
      }
      else
      {
        //we put the artist0
        tags_error = splt_t_set_tags_char_field(state, i, SPLT_TAGS_ARTIST, artist0);
        if (tags_error != SPLT_OK)
        {
          *error = tags_error;
          goto function_end;
        }
      }

      //we put the same album, year and genre everywhere
      if (i != 0)
      {
        tags_error = splt_t_set_tags_char_field(state, i, SPLT_TAGS_ALBUM, album0);
        if (tags_error != SPLT_OK)
        {
          *error = tags_error;
          goto function_end;
        }
        tags_error = splt_t_set_tags_char_field(state, i, SPLT_TAGS_YEAR, year0);
        if (tags_error != SPLT_OK)
        {
          *error = tags_error;
          goto function_end;
        }
        tags_error = splt_t_set_tags_uchar_field(state, i, SPLT_TAGS_GENRE, genre0);
        if (tags_error != SPLT_OK)
        {
          *error = tags_error;
          goto function_end;
        }
      }
    }
  }
  else
  {
    //we put the same artist, genre, album and year everywhere
    for(i = 1;i<tracks;i++)
    {
      tags_error = splt_t_set_tags_uchar_field(state, i, SPLT_TAGS_GENRE, genre0);
      if (tags_error != SPLT_OK) { *error = tags_error; goto function_end; }
      tags_error = splt_t_set_tags_char_field(state, i, SPLT_TAGS_ARTIST, artist0);
      if (tags_error != SPLT_OK) { *error = tags_error; goto function_end; }
      tags_error = splt_t_set_tags_char_field(state, i, SPLT_TAGS_ALBUM, album0);
      if (tags_error != SPLT_OK) { *error = tags_error; goto function_end; }
      tags_error = splt_t_set_tags_char_field(state, i, SPLT_TAGS_YEAR, year0);
      if (tags_error != SPLT_OK) { *error = tags_error; goto function_end; }
      tags_error = splt_t_set_tags_uchar_field(state, i, SPLT_TAGS_GENRE, genre0);
      if (tags_error != SPLT_OK) { *error = tags_error; goto function_end; }
    }
  }

  if (*error >= 0)
  {
    int err_format = SPLT_OK;

    if (splt_t_get_oformat(state) != NULL)
    {
      //we put the outputted filename
      char *old_format = strdup(splt_t_get_oformat(state));
      if (old_format != NULL)
      {
        splt_t_set_oformat(state, old_format,&err_format);
        free(old_format);
        old_format = NULL;
      }
      else
      {
        *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
        goto function_end;
      }
    }

    if (err_format >= 0)
    {
      //we set the current split to 0
      splt_t_set_current_split(state,0);
      do {
        int filename_error = SPLT_OK;
        //get output format filename
        filename_error = splt_u_put_output_format_filename(state);
        if (filename_error != SPLT_OK)
        {
          *error = filename_error;
          break;
        }
        splt_t_current_split_next(state);
      } while (splt_t_get_current_split(state) < tracks);
    }
    else
    {
      *error = err_format;
    }
  }

function_end:
  //free some memory
  if (artist0)
  {
    free(artist0);
    artist0 = NULL;
  }
  if (album0)
  {
    free(album0);
    album0 = NULL;
  }
  if (year0)
  {
    free(year0);
    year0 = NULL;
  }
}

/***********************/
/* cue */

//-if we have a string "test", this functions cuts out the two "
//and cleans the string of other unwanted characters
//-then puts the information in the tags
//-returns possible error
static int splt_cue_set_value(splt_state *state, char *in,
    int index, int tag_field)
{
  int error = SPLT_OK;

  char *ptr_b = in, *ptr_e = NULL;

  //-find begin
  //while we don't have spaces any more
  while (*ptr_b == ' ')
  {
    ptr_b++;
  }
  //if we have a '"', then skip quote
  if (*ptr_b == '"')
  {
    ptr_b++;
  }
  
  //-find and and put '\0'
  //go to the end of line
  ptr_e = strchr(ptr_b+1,'\n');
  //move back ignoring spaces
  while (*ptr_e == ' ')
  {
    ptr_e--;
  }
  //if we have a '"', then skip quote
  if (*ptr_e == '"')
  {
    ptr_e--;
  }
  *ptr_e = '\0';

  char *out = NULL;
  if ((out = malloc(strlen(ptr_b)+1)) == NULL)
  {
    error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  }
  else
  {
    strncpy(out, ptr_b, (strlen(ptr_b)+1));
    splt_u_cleanstring(state, out, &error);
    if (error >= 0)
    {
      int tags_err = SPLT_OK;

      //put Artist + Album info to client
      char *client_infos = malloc(sizeof(char) * (strlen(out)+30));
      if (client_infos == NULL)
      {
        error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      }
      else
      {
        if (tag_field == SPLT_TAGS_ARTIST)
        {
          snprintf(client_infos,strlen(out)+30,"\n  Artist: %s\n", out);
          splt_t_put_message_to_client(state, client_infos);
        }
        else if (tag_field == SPLT_TAGS_ALBUM)
        {
          snprintf(client_infos,strlen(out)+30,"  Album: %s\n", out);
          splt_t_put_message_to_client(state, client_infos);
        }
        free(client_infos);
        client_infos = NULL;

        tags_err = splt_t_set_tags_char_field(state, index, tag_field, out);
        if (tags_err != SPLT_OK)
        {
          error = tags_err;
        }
      }
    }
    free(out);
    out = NULL;
  }

  return error;
}

//error = possible error that we return
//gets the cue splitpoints from a file and puts them in the state
//returns the number of tracks found
int splt_cue_put_splitpoints(const char *file, splt_state *state, int *error)
{
  if (file == NULL)
  { 
    *error = SPLT_INVALID_CUE_FILE;
    return 0;
  }

  //clear previous splitpoints
  splt_t_free_splitpoints_tags(state);

  //default no error
  *error = SPLT_CUE_OK;
  
  char *client_infos = malloc(sizeof(char) * (strlen(file)+200));
  //put information to client
  if (client_infos == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return 0;
  }
  snprintf(client_infos, strlen(file)+200,
      " reading informations from CUE file %s ...\n",file);
  splt_t_put_message_to_client(state, client_infos);
  free(client_infos);
  client_infos = NULL;

  int append_error = SPLT_OK;
  //our file
  FILE *file_input = NULL;
  //the line we read from the file
  char line[2048] = { '\0' };
  char *ptr = NULL, *dot = NULL;
  ptr = dot = NULL;
  //tracks = number of tracks found
  int tracks = -1, time_for_track = SPLT_TRUE;
  //counter, counts if we have at least one INDEX on the file
  int type = SPLT_CUE_NOTHING, performer = SPLT_FALSE,
    title = SPLT_FALSE, counter = 0;
  int temp_error = SPLT_OK;
  int tags_error = SPLT_OK;
  
  //default genre
  tags_error = splt_t_set_tags_uchar_field(state, 0, SPLT_TAGS_GENRE, 12);
  if (tags_error != SPLT_OK)
  {
    *error = tags_error;
    return tracks;
  }
  
  //if we cannot open the file
  if (!(file_input=fopen(file, "r")))
  {
    splt_t_set_strerror_msg(state);
    splt_t_set_error_data(state,file);
    *error = SPLT_ERROR_CANNOT_OPEN_FILE;
    return tracks;
  }
  //if we can open the file
  else
  {
    //put pointer at the beginning of the file
    if (fseek(file_input, 0, SEEK_SET) == 0)
    {
      //contains the start of the content of the line;
      //-for example for the '      PERFORMER abcd' line,
      //it will point to ' abcd'
      char *line_content = NULL;

      //we read the file line by line
      while (fgets(line, 2048, file_input)!=NULL)
      {
        type = SPLT_CUE_NOTHING;

        //we read strings from file TRACK,TITLE,AUDIO,PERFORMER,INDEX
        if (((line_content = strstr(line, "TRACK"))!=NULL)
            && (strstr(line, "AUDIO")!=NULL))
        {
          line_content += 5;
          type = SPLT_CUE_TRACK;
        }
        else if ((line_content = strstr(line, "TITLE")) != NULL)
        {
          line_content += 5;
          type = SPLT_CUE_TITLE;
        }
        else if ((line_content = strstr(line, "PERFORMER")) != NULL)
        {
          type = SPLT_CUE_PERFORMER;
          line_content += 9;
        }
        else if ((ptr=strstr(line, "INDEX 01"))!=NULL)
        {
          type = SPLT_CUE_INDEX;
        }

        //we clean previous data
        splt_t_clean_one_split_data(state, tracks);

        //we analyse the previous strings found
        switch (type)
        {
          //nothing found
          case SPLT_CUE_NOTHING:
            break;
          //TRACK and AUDIO found
          case SPLT_CUE_TRACK:
            if (tracks==-1) 
            {
              tracks = 0;
            }
            if (time_for_track) 
            {
              performer = SPLT_FALSE;
              title = SPLT_FALSE;
              tracks++;
            }
            else
            {
              splt_t_set_error_data(state,file);
              *error = SPLT_INVALID_CUE_FILE;
              goto function_end;
            }
            time_for_track = SPLT_FALSE;
            break;
          //TITLE found
          case SPLT_CUE_TITLE:
            if (tracks == -1)
            {
              if ((temp_error = splt_cue_set_value(state, line_content, 
                      0, SPLT_TAGS_ALBUM)) != SPLT_OK)
              {
                *error = temp_error;
                goto function_end;
              }
            }
            else
            {
              if (tracks > 0)
              {
                if ((temp_error = splt_cue_set_value(state, line_content,
                        tracks-1, SPLT_TAGS_TITLE)) != SPLT_OK)
                {
                  *error = temp_error;
                  goto function_end;
                }
              }
              title = SPLT_TRUE;
            }
            break;
          //PERFORMER found
          case SPLT_CUE_PERFORMER:        
            if (tracks == -1)
            {
              //we always have one artist in a cue file, we
              //put the performers if more than one artist
              if ((temp_error = splt_cue_set_value(state, line_content,
                      0, SPLT_TAGS_ARTIST)) != SPLT_OK)
              {
                *error = temp_error;
                goto function_end;
              }
            }
            else
            {
              if (tracks>0)
              {
                if ((temp_error = splt_cue_set_value(state, line_content,
                        tracks-1, SPLT_TAGS_PERFORMER)) != SPLT_OK)
                {
                  *error = temp_error;
                  goto function_end;
                }
              }
              performer = SPLT_TRUE;
            }
            break;
          //INDEX 01 found
          case SPLT_CUE_INDEX:
            line[strlen(line)-1]='\0';
            ptr += 9;
            if ((dot = strchr(ptr, ':'))==NULL)
            {
              splt_t_set_error_data(state,file);
              *error = SPLT_INVALID_CUE_FILE;
              goto function_end;
            }
            else
            {
              //we replace : with . for the sscanf
              ptr[dot-ptr] = ptr[dot-ptr+3] = '.';
              //we clean the string for unwanted characters
              splt_u_cleanstring(state, ptr, error);
              if (*error < 0) { goto function_end; }

              //we convert to hundreths of seconds and put splitpoints
              if (tracks>0)
              {
                long hundr_seconds = splt_u_convert_hundreths(ptr);
                if (hundr_seconds==-1)
                {
                  splt_t_set_error_data(state,file);
                  *error = SPLT_INVALID_CUE_FILE;
                  goto function_end;
                }

                //we append the splitpoint
                append_error =
                  splt_t_append_splitpoint(state, hundr_seconds, NULL);
                if (append_error != SPLT_OK)
                {
                  *error = append_error;
                  goto function_end;
                }

                time_for_track = SPLT_TRUE;
                counter++;
              }
            }
            break;
          default:
            break;
        }
      }
      //we append the last splitpoint
      append_error = splt_t_append_splitpoint(state, LONG_MAX,
          "description here");
    }
    else
    {
      splt_t_set_strerror_msg(state);
      splt_t_set_error_data(state,file);
      *error = SPLT_ERROR_SEEKING_FILE;
      goto function_end;
    }

    //if we don't find INDEX on the file, error
    if (counter == 0)
    {
      splt_t_set_error_data(state,file);
      *error = SPLT_INVALID_CUE_FILE;
      goto function_end;
    }

    //if no time for track,
    if (!time_for_track) 
    {
      tracks--;
    }

    splt_tag_put_filenames_from_tags(state,tracks,error);

function_end:
    if (fclose(file_input) != 0)
    {
      splt_t_set_strerror_msg(state);
      splt_t_set_error_data(state, file);
      *error = SPLT_ERROR_CANNOT_CLOSE_FILE;
    }
    file_input = NULL;
  }
  
  //put number of tracks
  char tracks_info[64] = { '\0' };
  snprintf(tracks_info, 64, "  Tracks: %d\n\n",tracks);
  splt_t_put_message_to_client(state, tracks_info);

  //we return the number of tracks found
  return tracks;
}

/***********************/
/* cddb */

//error = possible error that we return
//puts the cddb splitpoints from the file in the state
//returns number of tracks
//see freedb file format documentation on freedb.org
//-file must not be NULL
int splt_cddb_put_splitpoints (const char *file, splt_state *state, int *error)
{
  //clear previous splitpoints
  splt_t_free_splitpoints_tags(state);

  //default no error
  *error = SPLT_CDDB_OK;

  char *client_infos = malloc(sizeof(char) * (strlen(file)+200));
  //put information to client
  if (client_infos == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return 0;
  }
  snprintf(client_infos, strlen(file)+200,
      " reading informations from CDDB file %s ...\n",file);
  splt_t_put_message_to_client(state, client_infos);
  free(client_infos);
  client_infos = NULL;

  //our file
  FILE *file_input = NULL;
  //the line we read
  char line[2048] = { '\0' };
  char prev[10] = { '\0' };
  //performer_title_split is where we split if we have
  //performer / title on cddb
  char *number = NULL, *c = NULL;
  //for the performer
  char *c2 = NULL;
  //tracks is the number of tracks
  int tracks = 0, i = 0, j = 0;
  int append_error = SPLT_OK;
  int change_error = SPLT_OK;
  int get_error = SPLT_OK;
  int tags_error = SPLT_OK;
  //temporary variables
  long split1 = 0, split2 = 0;

  char *artist = NULL;
  char *album = NULL;
  char *perfor = NULL;

  //we open the file
  if (!(file_input=fopen(file, "r")))
  {
    splt_t_set_strerror_msg(state);
    splt_t_set_error_data(state,file);
    *error = SPLT_ERROR_CANNOT_OPEN_FILE;
    return tracks;
  }
  else
  //if we can open the file
  {
    //we go at the beggining of the file
    if(fseek(file_input, 0, SEEK_SET) == 0)
    {
      //we search for the string "Track frame offset"
      do {
        if ((fgets(line, 2048, file_input))==NULL)
        {
          splt_t_set_error_data(state,file);
          *error = SPLT_INVALID_CDDB_FILE;
          goto function_end;
        }
        number = strstr(line, "Track frame offset");
      } while (number == NULL);

      memset(prev, 0, 10);

      //we read the track offsets
      do {
        if ((fgets(line, 2048, file_input))==NULL)
        {
          splt_t_set_error_data(state,file);
          *error = SPLT_INVALID_CDDB_FILE;
          goto function_end;
        }
        line[strlen(line)-1] = '\0';
        if (strstr(line, "Disc length") != NULL)
        {
          break;
        }
        i = 0;
        while ((isdigit(line[i])==0) && (line[i]!='\0'))
        {
          i++;
          number = line + i;
        }
        if (number == (line + strlen(line))) 
        {
          break;
        }
        else
        //we put the offsets in the splitpoint table
        {
          double temp = 0;
          temp = atof(number);

          //we append the splitpoint
          //in cddb_offset*100
          //we convert them lower to seconds
          append_error =
            splt_t_append_splitpoint(state, temp * 100, NULL);
  
          if (append_error != SPLT_OK)
          {
            *error = append_error;
            goto function_end;
          }
          else
          {
            //we count the tracks
            tracks++;
          }
        }
      } while (1);

      //we check parse output format
      //setting the number of tracks is important
      splt_t_set_splitnumber(state,tracks);

      //we clean the old data from the state
      splt_t_clean_split_data(state,tracks);

      //if we have Disc length right after
      //the splitpoints
      if (strstr(line,"Disc length") != NULL)
      {
        number = strstr(line, "Disc length");
      }
      else
      {
        //we find out "Disc length"
        do {
          if ((fgets(line, 1024, file_input))==NULL)
          {
            splt_t_set_error_data(state,file);
            *error = SPLT_INVALID_CDDB_FILE;
            goto function_end;
          }
          number = strstr(line, "Disc length");
        } while (number == NULL);
      }

      //we get the max disc length (in seconds)
      i = 0;
      while ((isdigit(line[i])==0) && (line[i]!='\0'))
      {
        i++;
        number = line + i;
      }

      //if no error, ?
      if (*error != SPLT_INVALID_CDDB_FILE)
      {
        double temp2 = 0;
        temp2 = atof(number);

        //we append the splitpoint (in seconds*100)
        append_error = splt_t_append_splitpoint(state, temp2 * 100, NULL);
        if (append_error != SPLT_OK)
        {
          *error = append_error;
          goto function_end;
        }
      }
      else
      {
        //we append 0 as splitpoint
        append_error = splt_t_append_splitpoint(state, 0, NULL);
        if (append_error != SPLT_OK)
        {
          *error = append_error;
          goto function_end;
        }
      }

      split2 = splt_t_get_splitpoint_value(state, 0, &get_error);
      if (get_error != SPLT_OK)
      {
        *error = get_error;
        goto function_end;
      }

      //we convert the points previously found
      for (i=tracks-1; i>=0; i--)
      {
        split1 = splt_t_get_splitpoint_value(state, i, &get_error);
        if (get_error != SPLT_OK)
        {
          *error = get_error;
          goto function_end;
        }

        //we remove the cddb_offset of the first splitpoint
        //and we divide by 75 (cddb specs)
        long difference = split1 - split2;
        float real_value = (float) difference / 75.f;
        change_error =
          splt_t_set_splitpoint_value(state, i, (long) ceilf(real_value));

        if (change_error != SPLT_OK)
        {
          *error = change_error;
          goto function_end;
        }
      }

      j=0;
      //if we have found the title of the song or not
      int title = SPLT_FALSE;
      //if we have performers or not
      int performer = SPLT_FALSE;
      do {
        title = 0;
        char temp[10];
        memset(temp, 0, 10);
        if ((fgets(line, 2048, file_input))==NULL)
        {
          splt_t_set_error_data(state,file);
          *error = SPLT_INVALID_CDDB_FILE;
          goto function_end;
        }
        line[strlen(line)-1] = '\0';
        if (strlen(line)>0)
        {
          if (line[strlen(line)-1]=='\r')
          {
            line[strlen(line)-1]='\0';
          }
        }

        //some cddb files have Year and Genre before TTITLE
        if (strstr(line, "YEAR") != NULL)
        {
          tags_error = splt_t_set_tags_char_field(state, 0, SPLT_TAGS_YEAR, line+6);
          if (tags_error != SPLT_OK)
          {
            *error = tags_error;
            goto function_end;
          }
        }
        else if (strstr(line, "GENRE") != NULL)
        {
          char a[4];
          strncpy(a,line+6,3);
          //this tag doesn't work correctly because GENRE is not a number
          tags_error = splt_t_set_tags_uchar_field(state, 0, SPLT_TAGS_GENRE,
              (unsigned char) atoi(a));
          if (tags_error != SPLT_OK)
          {
            *error = tags_error;
            goto function_end;
          }
        }

        //we search for the title of the disc
        if (j==0)
        {
          if (strstr(line, "DTITLE")==NULL)
          {
            continue;
          }
        }
        else
        {
          //we search for the title of the tracks
          if (strstr(line, "TTITLE")==NULL)
          {
            continue;
          }
          else
          {
            title = SPLT_TRUE;
          }
        }

        //if we don't have '=', invalid file
        if ((number=strchr(line, '='))==NULL) 
        {
          splt_t_set_error_data(state,file);
          *error = SPLT_INVALID_CDDB_FILE;
          goto function_end;
        }

        //we read the string after the = or after the /
        //the / sign is found on the DTITLE=Artist / Disc
        //we put the string in number variable
        //used for the filename
        if (j>0)
        {
          int len = number-line;
          if (len>10) 
          {
            len = 10;
          }
          strncpy(temp, line, len);
          if ((c = strchr(number, '/'))!=NULL) 
          {
            //if found '/' in TITLE, separate performer / title
            if (title)
            {
              c2 = strchr(number+1,'/');
              *c2 = '\0';
              c2 = splt_u_cut_spaces_at_the_end(c2-1);

              //we put performer
              performer = SPLT_TRUE;
              tags_error = 
                splt_t_set_tags_char_field(state, j-1, SPLT_TAGS_PERFORMER,
                    number+1);

              if (tags_error != SPLT_OK)
              {
                *error = tags_error;
                goto function_end;
              }
            }
            else
            {
              tags_error = 
                splt_t_set_tags_char_field(state, j-1, SPLT_TAGS_PERFORMER,
                    NULL);
              if (tags_error != SPLT_OK)
              {
                *error = tags_error;
                goto function_end;
              }
            }

            perfor = splt_t_get_tags_char_field(state, j-1, SPLT_TAGS_PERFORMER);
            splt_u_cleanstring(state, perfor, error);
            if (*error < 0) { goto function_end; }
            number = c+1;
          }

          splt_u_cleanstring(state, number, error);
          if (*error < 0) { goto function_end; }
        }

        //we limit number to 512?
        if (strlen(++number)>512) 
        {
          number[512]='\0';
        }

        //if what we read contains Data or Track
        if ((j>0)&&(strstr(number, "Data")!=NULL) && 
            (strstr(number, "Track")!=NULL)) 
        {
          split1 = 
            splt_t_get_splitpoint_value(state, j, &get_error);
          if (get_error != SPLT_OK)
          {
            *error = get_error;
            goto function_end;
          }

          change_error = 
            splt_t_set_splitpoint_value(state,j-1, split1);

          if (change_error != SPLT_OK)
          {
            *error = change_error;
            goto function_end;
          }
          tracks -= 1;
        }
        else
        {
          //otherwise, we put the title
          if ((j>0)&&(strcmp(temp, prev)==0))
          {
            tags_error = splt_t_set_tags_char_field(state, j-1, SPLT_TAGS_TITLE,
                number);
            if (tags_error != SPLT_OK)
            {
              *error =tags_error;
              goto function_end;
            }
          }
          else
          {
            //we treat DTITLE
            if (j == 0)
            {
              //we put the artist
              i=0;
              while ((number[i]!='/') && 
                  (number[i]!='\0')&&(i<127)) 
              {
                i++;
              }

              char ttemp[i+1];
              int i_temp = 0;
              for (i_temp = 0; i_temp < i;i_temp++)
              {
                ttemp[i_temp] = number[i_temp];
              }
              ttemp[i] = '\0';
              tags_error = splt_t_set_tags_char_field(state, 0, SPLT_TAGS_ARTIST,
                  ttemp);
              if (tags_error != SPLT_OK)
              {
                *error =tags_error;
                goto function_end;
              }

              //we cut the space at the end of the artist
              artist = splt_t_get_tags_char_field(state,0, SPLT_TAGS_ARTIST);
              int k = strlen(artist)-1;
              while (artist[k] == ' ')
              {
                artist[k] = '\0';
                k--;
                if (k < 0)
                {
                  break;
                }
              }
              splt_u_cleanstring(state, artist, error);
              if (*error < 0) { goto function_end; }

              //put artist info to client
              client_infos = malloc(sizeof(char) * (strlen(artist)+30));
              if (client_infos == NULL)
              {
                *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
                goto function_end;
              }
              snprintf(client_infos,strlen(artist)+30,"\n  Artist: %s\n", artist);
              splt_t_put_message_to_client(state, client_infos);
              free(client_infos);
              client_infos = NULL;

              //we put the album
              i += 1;
              number = splt_u_cut_spaces_from_begin(number+i);

              tags_error = splt_t_set_tags_char_field(state, 0, SPLT_TAGS_ALBUM, 
                  number);
              if (tags_error != SPLT_OK)
              {
                *error =tags_error;
                goto function_end;
              }

              album = splt_t_get_tags_char_field(state,0, SPLT_TAGS_ALBUM);
              splt_u_cleanstring(state, album, error);
              if (*error < 0) { goto function_end; }

              //put album info to client
              client_infos = malloc(sizeof(char) * (strlen(album)+30));
              if (client_infos == NULL)
              {
                *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
                goto function_end;
              }
              snprintf(client_infos,strlen(album)+30,"  Album: %s\n", album);
              splt_t_put_message_to_client(state, client_infos);
              free(client_infos);
              client_infos = NULL;
            }
            else
            {
              char *t = number;
              //if we don't have = before, it means
              //we have a performer with a  perf/title
              if (*(number-1) != '=')
              {
                t = number-1;
              }
              t = splt_u_cut_spaces_from_begin(t);
              tags_error = splt_t_set_tags_char_field(state, j-1,
                  SPLT_TAGS_TITLE, t);
              if (tags_error != SPLT_OK)
              {
                *error =tags_error;
                goto function_end;
              }
            }
            j++;
          }
        }
        strncpy(prev, temp, 10);

      } while (j<=tracks);

      //we search for
      //YEAR (the year) and ID3 genre
      tags_error = splt_t_set_tags_uchar_field(state, 0, SPLT_TAGS_GENRE, 12);
      if (tags_error != SPLT_OK)
      {
        *error =tags_error;
        goto function_end;
      }

      while ((fgets(line, 2048, file_input))!=NULL)
      {
        line[strlen(line)-1] = '\0';
        if (strlen(line)>0)
        {
          if (line[strlen(line)-1]=='\r') 
          {
            line[strlen(line)-1]='\0';
          }
        }
        if (strstr(line, "EXTD")==NULL) 
        {
          continue;
        }
        else 
        {
          if ((number=strchr(line, '='))==NULL) 
          {
            break;
          }
          else 
          {
            if ((c=strstr(number, "YEAR"))!=NULL)
            {
              tags_error = splt_t_set_tags_char_field(state, 0, SPLT_TAGS_YEAR, c+6);
              if (tags_error != SPLT_OK)
              {
                *error = tags_error;
                goto function_end;
              }
            }
            if ((c=strstr(number, "ID3G"))!=NULL) 
            {
              strncpy(line, c+6, 3);
              tags_error = splt_t_set_tags_uchar_field(state, 0, SPLT_TAGS_GENRE,
                    (unsigned char) atoi(line));
              if (tags_error != SPLT_OK)
              {
                *error = tags_error;
                goto function_end;
              }
            }
            break;
          }
        }
      }

      splt_tag_put_filenames_from_tags(state,tracks,error);
    }
    else
    {
      splt_t_set_strerror_msg(state);
      splt_t_set_error_data(state,file);
      *error = SPLT_ERROR_SEEKING_FILE;
      goto function_end;
    }

function_end:
    if (fclose(file_input) != 0)
    {
      splt_t_set_strerror_msg(state);
      splt_t_set_error_data(state, file);
      *error = SPLT_ERROR_CANNOT_CLOSE_FILE;
    }
    file_input = NULL;
  }

  //if we have tracks, put the number of tracks to the user
  if (*error >= 0)
  {
    char tracks_info[64] = { '\0' };
    snprintf(tracks_info, 64, "  Tracks: %d\n\n",tracks);
    splt_t_put_message_to_client(state, tracks_info);
  }

  return tracks;
}

/***********************/
/* freedb */

#if defined(__BEOS__) && !defined (HAS_GETPASS)
//used for proxy (proxy not implemented)
//#warning Faking getpass() !!!
//char *getpass(char *p)
//{
//      char *ret;
//      ret = malloc(30);
//      if (!ret)
//              return NULL;
//      puts(p);
//      fgets(ret,30,stdin);
//      return ret;
//}
#endif
#if defined(__BEOS__) && (IPPROTO_UDP==1)
// net_server has a weird order for IPPROTO_
#else
#define closesocket close
#endif

//used the base64 algorithm - for proxy (proxy not implemented)
//
// Base64 Algorithm: Base64.java v. 1.3.6 by Robert Harder
// Ported and optimized for C by Matteo Trotta
//
//const char alphabet [] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
//char *encode3to4 (unsigned char *source, int srcoffset, int num, char *destination, int destoffset)
//{
//
//    int inbuff=(num>0?(source[srcoffset]<<16):0)|(num>1?(source[srcoffset+1]<<8):0)|(num>2?(source[srcoffset+2]):0);
//    switch(num)
//    {
//      case 3:
//          destination[destoffset] = alphabet[(inbuff>>18)];
//          destination[destoffset+1] = alphabet[(inbuff>>12)&0x3f];
//          destination[destoffset+2] = alphabet[(inbuff>>6)&0x3f];
//          destination[destoffset+3] = alphabet[(inbuff)&0x3f];
//          return destination;
//
//      case 2:
//          destination[destoffset] = alphabet[(inbuff>>18)];
//          destination[destoffset+1] = alphabet[(inbuff>>12)&0x3f];
//          destination[destoffset+2] = alphabet[(inbuff>>6)&0x3f];
//          destination[destoffset+3] = '=';
//          return destination;
//
//      case 1:
//          destination[destoffset] = alphabet[(inbuff>>18)];
//          destination[destoffset+1] = alphabet[(inbuff>>12)&0x3f];
//          destination[destoffset+2] = '=';
//          destination[destoffset+3] = '=';
//          return destination;
//      default:
//          return destination;
//    }
//}

//used for proxy (proxy not implemented)
//char *b64 (unsigned char *source, int len)
//{
//      char *out;
//      int d, e=0;
//      d = ((len*4/3)+((len%3)>0?4:0));
//      
//      out = malloc(d+1);
//      
//      memset(out, 0x00, d+1);
//      for(d=0;d<(len-2);d+=3,e+=4)
//              out = encode3to4(source, d, 3, out, e);
//      if(d<len)
//              out = encode3to4(source, d, len-d, out, e);
//
//      return out;
//}
// End of Base64 Algorithm

//we analyse the freedb2 buffer for the CDs results
static int splt_freedb2_analyse_cd_buffer (char *buf, int size,
    splt_state *state, int *error)
{
  //temporary pointer
  char *temp = buf, *temp2 = NULL;

  //we replace the \r with \n
  while ((temp = strchr(temp,'\r')) != NULL)
  {
    *temp = '\n';
  }

  temp = NULL;
  do
  {
    //genre
    buf = strchr(buf, '\n');

    if (buf != NULL)
    {
      buf += 1;
      buf++;

      //disc id
      temp = strchr(buf, ' ');
      if (temp != NULL)
      {
        temp++;

        //artist / album
        //temp2 is the end of the line
        temp2 = strchr(temp+8,'\n');
        if (temp2 != NULL)
        {
          temp2++;

          //we set the category and the disc id
          splt_t_freedb_set_disc(state,splt_t_freedb_get_found_cds(state), 
              temp,buf,temp-buf);

          char *full_artist_album = malloc(temp2-(temp+8)-1);
          if (full_artist_album)
          {
            int max_chars = temp2-(temp+8)-1;
            snprintf(full_artist_album,max_chars,"%s",temp+9);
            //snprintf seems buggy
#ifdef __WIN32__					
            full_artist_album[max_chars-1] = '\0';
#endif
            splt_u_print_debug("Setting the full artist album name ",0,full_artist_album);

            //i!=-1 means that it's not a revision
            int i=0;
            int err = SPLT_OK;
            //here we have in album_name the name of the current album      
            err = splt_t_freedb_append_result(state, full_artist_album, i);
            if (err < 0)
            {
              if (full_artist_album)
              {
                free(full_artist_album);
                full_artist_album = NULL;
              }
              *error = err;
              return -2;
            }

            //free memory
            free(full_artist_album);
            full_artist_album = NULL;
          }
          else
          {
            *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
            return -2;
          }

          //next cd
          splt_t_freedb_found_cds_next(state);
        }
        else
        {
          return -1;
        }
      }
    }
    else
    {
      return 0;
    }

  } while (((strstr(buf,"/"))!= NULL) &&
      ((strchr(buf,'\n'))!= NULL) &&
      (splt_t_freedb_get_found_cds(state) < SPLT_MAXCD));

  return splt_t_freedb_get_found_cds(state);
}

//char *login (char *s)
//{
//      char *pass, junk[130];
//      fprintf (stdout, "Username: ");
//      fgets(junk, 128, stdin);
//      junk[strlen(junk)-1]='\0';
//      pass = getpass("Password: ");
//      sprintf (s, "%s:%s", junk, pass);
//      memset (pass, 0x00, strlen(pass));
//      free(pass);
//      return s;
//}

static splt_addr splt_freedb_useproxy(splt_proxy *proxy, splt_addr dest,
    const char search_server[256], int port)
{
  dest.proxy=0;
  memset(dest.hostname, 0, 256);
  //memset(line, 0, 270);

  //if (proxy->use_proxy)
  if (proxy)
  {
    /*
    //TODO
    strncpy(dest.hostname, proxy->hostname, 255);
    dest.port = proxy->port;
    dest.proxy = proxy->use_proxy;

    fprintf(stderr, "Using Proxy: %s on Port %d\n", dest.hostname, dest.port);

    dest.auth = malloc(strlen(line)+1);
    if (dest.auth==NULL)
    {
      perror("malloc");
      exit(1);
    }
    memset(dest.auth, 0x0, strlen(line)+1);
    strncpy(dest.auth, line, strlen(line));
    //dest.auth = b64(line, strlen(line));*/
  }

  if (!dest.proxy) 
  {
    //we put the hostname
    if (strlen(search_server) == 0)
    {
      //by default we use freedb2.org
      strncpy(dest.hostname, SPLT_FREEDB2_SITE, 255);
    }
    else
    {
      strncpy(dest.hostname, search_server, 255);
    }

    //we put the port
    if (port == -1)
    {
      //by default we put the port 80
      //to use it with cddb.cgi
      dest.port = SPLT_FREEDB_CDDB_CGI_PORT;
    }
    else
    {
      dest.port = port;
    }      
  }

  return dest;
}

//search the freedb according to "search"
//returns possible errors
//we have possible errors in result
//search_type can be SPLT_FREEDB_SEARCH_TYPE_CDDB_CGI
// - it is the search type to perform on the server
//search_server is the server on which to search for,
// if search_server == NULL, it will be freedb2.org by default
//port is the port where to connect to the server; by default is 80
int splt_freedb_process_search(splt_state *state, char *search,
                               int search_type, const char search_server[256],
                               int port)
{
  //we take the cgi path of the search_server
  //if we have one
  char cgi_path[256] = { '\0' };
  if (search_type == SPLT_FREEDB_SEARCH_TYPE_CDDB_CGI)
  {
    char *temp = strchr(search_server,'/');
    if (temp != NULL)
    {
      snprintf(cgi_path,255,"%s",temp);
      *temp = '\0';
    }
  }
  //default cgi path
  if (strlen(search_server) == 0)
  {
    snprintf(cgi_path,255,"%s","/~cddb/cddb.cgi");
  }

  //possible error that we will return
  int error = SPLT_FREEDB_OK;
  //socket and internet structures
  struct sockaddr_in host;
  struct hostent *h = NULL;
  splt_addr dest;
  //e is used for the end of the buffer
  //c is used for the buffer read
  char *c = NULL, *e=NULL;
  int i = 0, tot=0;
  //the message delivered to the server
  char *message = NULL;
  //the buffer that we are using to read incoming transmission
  char buffer[SPLT_FREEDB_BUFFERSIZE] = { '\0' };

  //fd = socket identifier
#ifdef __WIN32__
  long winsockinit;
  WSADATA winsock;
  SOCKET fd;
  winsockinit = WSAStartup(0x0101,&winsock);
  if (winsockinit != 0)
  {
    splt_t_clean_strerror_msg(state);
    error = SPLT_FREEDB_ERROR_INITIALISE_SOCKET;
    return error;
  }
#else
  int fd;
#endif

  //transform ' ' to '+'
  int string_length = strlen(search);
  for (i = 0; i < string_length; i++)
  {
    if (search[i] == ' ')
    {
      search[i] = '+';
    }
  }

  //dest = splt_freedb_useproxy(&state->proxy, dest, search_server, port);
  dest = splt_freedb_useproxy(NULL, dest, search_server, port);

  //we get the hostname of freedb
  if((h=gethostbyname(dest.hostname))==NULL)
  {
    splt_t_set_strherror_msg(state);
    error = SPLT_FREEDB_ERROR_CANNOT_GET_HOST;
    splt_t_set_error_data(state,dest.hostname);
#ifdef __WIN32__
    WSACleanup();
#endif
    return error;
  }
  else
  {
    splt_t_set_error_data(state,dest.hostname);

    //we prepare socket
    memset(&host, 0x0, sizeof(host));
    host.sin_family=AF_INET;
    host.sin_addr.s_addr=((struct in_addr *) (h->h_addr)) ->s_addr;
    host.sin_port=htons(dest.port);

    //initialize socket
    if((fd=socket(AF_INET, SOCK_STREAM, 0))==-1)
    {
      splt_t_set_strerror_msg(state);
      error = SPLT_FREEDB_ERROR_INITIALISE_SOCKET;
#ifdef __WIN32__
      WSACleanup();
#endif
      return error;
    }
    else
    {
      //make connection
      if ((connect(fd, (void *)&host, sizeof(host))) < 0)
      {
        splt_t_set_strerror_msg(state);
        error = SPLT_FREEDB_ERROR_CANNOT_CONNECT;
        closesocket(fd);
#ifdef __WIN32__
        WSACleanup();
#endif
        return error;
      }
      else
      {
        //prepare message to send
        //proxy not supported for now
        //if (dest.proxy) {
        //                sprintf(message,
        //                "GET http://www.freedb.org"SPLT_SEARCH" "PROXYDLG, search);
        //                if (dest.auth!=NULL)
        //                sprintf (message, "%s"AUTH"%s\n", message, dest.auth);
        //                strncat(message, "\n", 1);
        //                }
        //                else 
        int malloc_number = 0;
        //freedb2 search
        if (search_type == SPLT_FREEDB_SEARCH_TYPE_CDDB_CGI)
        {
          malloc_number = strlen(search)+
            strlen(SPLT_FREEDB2_SEARCH)+strlen(cgi_path)+3;

          //we allocate the memory for the query string
          if ((message = malloc(malloc_number)) == NULL)
          {
            error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
          }
          else
          {
            //we write the search query
            snprintf(message, malloc_number,
                SPLT_FREEDB2_SEARCH,cgi_path,search);

            //message sent
            if((send(fd, message, strlen(message), 0))==-1)
            {
              splt_t_set_strerror_msg(state);
              error = SPLT_FREEDB_ERROR_CANNOT_SEND_MESSAGE;
            }
            else
            {
              memset(buffer, 0x00, SPLT_FREEDB_BUFFERSIZE);

              //we free previous search
              splt_t_freedb_free_search(state);

              int init_err = SPLT_OK;
              //create cdstate..
              init_err = splt_t_freedb_init_search(state);

              if (init_err == SPLT_OK)
              {
                //we read what we receive from the server
                do {
                  tot=0;
                  c = buffer;

                  do {
                    i = recv(fd, c, SPLT_FREEDB_BUFFERSIZE-(c-buffer)-1, 0);
                    if (i == -1) 
                    {
                      splt_t_set_strerror_msg(state);
                      error = SPLT_FREEDB_ERROR_CANNOT_RECV_MESSAGE;
                      goto function_end1;
                    }
                    tot += i;
                    buffer[tot]='\0';
                    c += i;
                  } while ((i>0)&&(tot<SPLT_FREEDB_BUFFERSIZE-1)
                      &&((e=strstr(buffer, "\n."))==NULL));

                  //we analyse the buffer
                  tot = splt_freedb2_analyse_cd_buffer(buffer, tot, state,&error);
                  if (error < 0)
                  {
                    goto function_end1;
                  }

                  if (tot == -1) continue;
                  if (tot == -2) break;

                } while ((i>0)&&(e==NULL)&&
                    (splt_t_freedb_get_found_cds(state)<SPLT_MAXCD));

                //no cd found
                if (splt_t_freedb_get_found_cds(state)==0) 
                {
                  error = SPLT_FREEDB_NO_CD_FOUND;
                  goto function_end1;
                }
                //erroror occured while getting freedb infos
                if (splt_t_freedb_get_found_cds(state)==-1) 
                {
                  error = SPLT_FREEDB_ERROR_GETTING_INFOS;
                  goto function_end1;
                }
                //max cd number reached
                if (splt_t_freedb_get_found_cds(state)==SPLT_MAXCD) 
                {
                  error = SPLT_FREEDB_MAX_CD_REACHED;
                  goto function_end1;
                }
              }
              else
              {
                error = init_err;
                goto function_end1;
              }
            }

function_end1:
            //free memory
            free(message);
            message = NULL;
#ifdef __WIN32__
            WSACleanup();
#endif
          }
        }
        //we will put the new web html freedb search
        /* TODO when freedb.org releases the web search */
        else 
        {
          error = SPLT_FREEDB_ERROR_GETTING_INFOS;
#ifdef __WIN32__
          WSACleanup();
#endif
          closesocket(fd);
          return error;
          /*if (search_type == SPLT_FREEDB_SEARCH_TYPE_CDDB)
            {
            }*/
        }
      }
      closesocket(fd);
    }
  }

  return error;
}

//must only be called after process_freedb_search
//returns the cddb file content corresponding to the last search, for
//the disc_id i (parameter of the function)
//we return possible error in err
//result must be freed
//cddb_get_server is the server from where we get the cddb file
// -if it's null, we will use freedb2.org
//the port is 80 by default, is the port where to connect to the server
//get_type specifies the type of the get 
// -it can be SPLT_FREEDB_GET_FILE_TYPE_CDDB_CGI (that works for both
//  freedb and freedb2 at the moment - 18_10_06)
//  or SPLT_FREEDB_GET_FILE_TYPE_CDDB (that only work for freedb at
//  the moment - 18_10_06)
//
//TODO: see when we don't have a valid port or get_type
char *splt_freedb_get_file(splt_state *state, int i, int *error,
    int get_type, const char cddb_get_server[256], int port)
{
  //we take the cgi path of the search_server
  //if we have one
  char cgi_path[256] = { '\0' };
  if (get_type == SPLT_FREEDB_GET_FILE_TYPE_CDDB_CGI)
  {
    char *temp = strchr(cddb_get_server,'/');
    if (temp != NULL)
    {
      snprintf(cgi_path,256,"%s",temp);
      *temp = '\0';
    }
  }
  //default cgi path
  if (strlen(cddb_get_server) == 0)
  {
    snprintf(cgi_path,255,"%s","/~cddb/cddb.cgi");
  }

  //possible error that we will return
  *error = SPLT_FREEDB_FILE_OK;
  //the freedb file that we will return
  char *output = NULL;

  //socket and internet structures
  struct sockaddr_in host;
  struct hostent *h;
  splt_addr dest;
  char *message = NULL;
  int tot=0;
  //the buffer that we are using to read incoming transmission
  char buffer[SPLT_FREEDB_BUFFERSIZE] = { '\0' };
  //e is used for the end of the buffer
  //c is used for the buffer read
  char *c = NULL, *e=NULL;

  //fd = socket identifier
#ifdef __WIN32__
  long winsockinit;
  WSADATA winsock;
  SOCKET fd;
  winsockinit = WSAStartup(0x0101,&winsock);
  if (winsockinit != 0)
  {
    splt_t_clean_strerror_msg(state);
    error = SPLT_FREEDB_ERROR_INITIALISE_SOCKET;
    return error;
  }
#else
  int fd = 0;
#endif

  //dest = splt_freedb_useproxy(&state->proxy, dest, cddb_get_server, port);
  dest = splt_freedb_useproxy(NULL, dest, cddb_get_server, port);

  //we get the hostname of freedb
  if((h=gethostbyname(dest.hostname))==NULL)
  {
    splt_t_set_strherror_msg(state);
    *error = SPLT_FREEDB_ERROR_CANNOT_GET_HOST;
    splt_t_set_error_data(state,dest.hostname);
#ifdef __WIN32__
    WSACleanup();
#endif
    return NULL;
  }
  else
  {
    splt_t_set_error_data(state,dest.hostname);

    //we prepare socket
    memset(&host, 0x0, sizeof(host));
    host.sin_family=AF_INET;
    host.sin_addr.s_addr=((struct in_addr *) (h->h_addr)) ->s_addr;
    host.sin_port=htons(dest.port);

    //prepare message to send
    //proxy not yet supported
    //      if (dest.proxy) {
    //        sprintf(message, "GET "FREEDBHTTP"cmd=cddb+read+%s+%s&hello=nouser+mp3splt.net+"PACKAGE_NAME"+"PACKAGE_VERSION"&proto=5 "PROXYDLG, 
    //        cdstate->discs[i].category, cdstate->discs[i].discid);
    //        if (dest.auth!=NULL) {
    //        sprintf (message, "%s"AUTH"%s\n", message, dest.auth);
    //        memset(dest.auth, 0x00, strlen(dest.auth));
    //        free(dest.auth);
    //        }
    //        strncat(message, "\n", 1);
    //        }
    //        else 
    const char *cd_category = splt_t_freedb_get_disc_category(state, i);
    const char *cd_id = splt_t_freedb_get_disc_id(state, i);

    int malloc_number = 0;
    if (get_type == SPLT_FREEDB_GET_FILE_TYPE_CDDB)
    {
      malloc_number = strlen(cd_category)+strlen(cd_id)+
        strlen(SPLT_FREEDB_GET_FILE);
    }
    else
    {
      //if (get_type == SPLT_FREEDB_GET_FILE_TYPE_CDDB_CGI)
      malloc_number = strlen(cd_category) + strlen(cd_id) +
        strlen(SPLT_FREEDB_CDDB_CGI_GET_FILE) + strlen(cgi_path);
    }
    message = malloc(malloc_number);
    if (message != NULL)
    {
      //CDDB protocol (usually port 8880)
      if (get_type == SPLT_FREEDB_GET_FILE_TYPE_CDDB)
      {
        snprintf(message, malloc_number, SPLT_FREEDB_GET_FILE, cd_category, cd_id);

        //open socket
        if((fd=socket(AF_INET, SOCK_STREAM, 0))==-1)
        {
          splt_t_set_strerror_msg(state);
          *error = SPLT_FREEDB_ERROR_CANNOT_OPEN_SOCKET;
          free(message);
          goto end_function;
        }
        else
        {
          //connect to host
          if ((connect(fd, (void *)&host, sizeof(host)))==-1)
          {
            splt_t_set_strerror_msg(state);
            *error = SPLT_FREEDB_ERROR_CANNOT_CONNECT;
            goto bloc_end;
          }
          else
          {
            //possible errors + proxy
            if (!dest.proxy) 
            {
              i=recv(fd, buffer, SPLT_FREEDB_BUFFERSIZE-1, 0);
              if (i == -1)
              {
                splt_t_set_strerror_msg(state);
                *error = SPLT_FREEDB_ERROR_CANNOT_RECV_MESSAGE;
                goto bloc_end;
              }
              buffer[i]='\0';

              if (strncmp(buffer,"201",3)!=0)  
              {
                *error = SPLT_FREEDB_ERROR_SITE_201;
                goto bloc_end;
              }

              //send hello message
              if((send(fd, SPLT_FREEDB_HELLO, strlen(SPLT_FREEDB_HELLO), 0))==-1)
              {
                splt_t_set_strerror_msg(state);
                *error = SPLT_FREEDB_ERROR_CANNOT_SEND_MESSAGE;
                goto bloc_end;
              }
              i=recv(fd, buffer, SPLT_FREEDB_BUFFERSIZE-1, 0);

              if (i == -1)
              {
                splt_t_set_strerror_msg(state);
                *error = SPLT_FREEDB_ERROR_CANNOT_RECV_MESSAGE;
                goto bloc_end;
              }
              buffer[i]='\0';

              if (strncmp(buffer,"200",3)!=0)  
              {
                *error = SPLT_FREEDB_ERROR_SITE_200;
                goto bloc_end;
              }
            }

            //we send the message
            if((send(fd, message, strlen(message), 0))==-1)
            {
              splt_t_set_strerror_msg(state);
              *error = SPLT_FREEDB_ERROR_CANNOT_SEND_MESSAGE;
              goto bloc_end;
            }
            else
            {
              memset(buffer, 0x00, SPLT_FREEDB_BUFFERSIZE);
              c = buffer;
              tot=0;

              //we read 
              do {
                i = recv(fd, c, SPLT_FREEDB_BUFFERSIZE-(c-buffer)-1, 0);
                if (i == -1)
                {
                  splt_t_set_strerror_msg(state);
                  *error = SPLT_FREEDB_ERROR_CANNOT_RECV_MESSAGE;
                  goto bloc_end;
                }

                //if errors
                if (tot == 0)
                {
                  if ((strncmp(buffer,"50",2) == 0)
                      || (strncmp(buffer,"40",2) == 0))
                  {
                    //if "No such CD entry in database"
                    if (strncmp(buffer,"401",3) == 0)
                    {
                      *error = SPLT_FREEDB_NO_SUCH_CD_IN_DATABASE;
                    }
                    else
                    {
                      *error = SPLT_FREEDB_ERROR_SITE;
                    }
                    goto bloc_end;
                  }
                }

                tot += i;
                buffer[tot]='\0';
                c += i;
              } while ((i>0)&&(tot<SPLT_FREEDB_BUFFERSIZE-1)&&
                  ((e=strstr(buffer, "\n."))==NULL));

              //we quit
              if (!dest.proxy)
              {
                if((send(fd, "quit\n", 5, 0))==-1)
                {
                  splt_t_set_strerror_msg(state);
                  *error = SPLT_FREEDB_ERROR_CANNOT_SEND_MESSAGE;
                  goto bloc_end;
                }
              }
            }
          }

bloc_end:
          free(message);
          message = NULL;
          closesocket(fd);
        }

        //if we don't have an error
        if (*error >= 0)
        {
          if (tot==0) 
          {
            *error = SPLT_FREEDB_ERROR_BAD_COMMUNICATION;
            goto end_function;
          }

          if (e!=NULL)
          {
            buffer[e-buffer+1]='\0';
          }

          //if invalid server answer
          if ((strstr(buffer, "database entry follows"))==NULL)
          {
            if ((c = strchr(buffer, '\n'))!=NULL)
            {
              buffer[c-buffer]='\0';
            }
            *error = SPLT_FREEDB_ERROR_INVALID_SERVER_ANSWER;
            goto end_function;
          }
          else
          {
            if ((c = strchr(buffer, '#'))==NULL)
            {
              output = NULL;
              *error = SPLT_FREEDB_ERROR_BAD_COMMUNICATION;
              goto end_function;
            }
            else
            {
              output = malloc(strlen(c)+1);
              if (output != NULL)
              {
                sprintf(output,c);
#ifdef __WIN32__
                WSACleanup();
#endif
                return output;
              }
              else
              {
                *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
                goto end_function;
              }
            }
          }
        }
        else
        {
          goto end_function;
        }
      }
      //cddb.cgi script (usually port 80)
      else
      {
        if (get_type == SPLT_FREEDB_GET_FILE_TYPE_CDDB_CGI)
        {
          snprintf(message, malloc_number, SPLT_FREEDB_CDDB_CGI_GET_FILE, 
              cgi_path, cd_category, cd_id);

          //open socket
          if((fd=socket(AF_INET, SOCK_STREAM, 0))==-1)
          {
            splt_t_set_strerror_msg(state);
            *error = SPLT_FREEDB_ERROR_CANNOT_OPEN_SOCKET;
            free(message);
            goto end_function;
          }
          else
          {
            //connect to host
            if ((connect(fd, (void *)&host, sizeof(host)))==-1)
            {
              splt_t_set_strerror_msg(state);
              *error = SPLT_FREEDB_ERROR_CANNOT_CONNECT;
              goto bloc_end2;
            }
            else
            {
              //we send the message
              if((send(fd, message, strlen(message), 0))==-1)
              {
                splt_t_set_strerror_msg(state);
                *error = SPLT_FREEDB_ERROR_CANNOT_SEND_MESSAGE;
                goto bloc_end2;
              }
              else
              {
                memset(buffer, 0x00, SPLT_FREEDB_BUFFERSIZE);
                c = buffer;
                tot=0;

                //we read 
                //we read what we receive from the server
                do {
                  tot=0;
                  c = buffer;

                  do {
                    i = recv(fd, c, SPLT_FREEDB_BUFFERSIZE-(c-buffer)-1, 0);
                    if (i == -1) 
                    {
                      splt_t_set_strerror_msg(state);
                      *error = SPLT_FREEDB_ERROR_CANNOT_RECV_MESSAGE;
                      goto bloc_end2;
                    }

                    //if errors
                    if (tot == 0)
                    {
                      if ((strncmp(buffer,"50",2) == 0)
                          || (strncmp(buffer,"40",2) == 0))
                      {
                        //if "No such CD entry in database"
                        if (strncmp(buffer,"401",3) == 0)
                        {
                          *error = SPLT_FREEDB_NO_SUCH_CD_IN_DATABASE;
                        }
                        else
                        {
                          *error = SPLT_FREEDB_ERROR_SITE;
                        }
                        goto bloc_end2;
                      }
                    }

                    tot += i;
                    buffer[tot]='\0';
                    c += i;
                  } while ((i>0)&&(tot<SPLT_FREEDB_BUFFERSIZE-1)
                      &&((e=strstr(buffer, "\n."))==NULL));

                  if (error < 0)
                  {
                    goto bloc_end2;
                  }

                } while ((i>0)&&(e==NULL));
              }
            }

bloc_end2:
            free(message);
            message = NULL;
            closesocket(fd);

            //if we don't have an error
            if (*error >= 0)
            {
              if (tot==0) 
              {
                *error = SPLT_FREEDB_ERROR_BAD_COMMUNICATION;
                goto end_function;
              }

              if ((c = strchr (buffer, '#'))==NULL)
              {
                output = NULL;
                *error = SPLT_FREEDB_ERROR_BAD_COMMUNICATION;
                goto end_function;
              }
              else
              {
                output = malloc(strlen(c)+1);
                if (output != NULL)
                {
                  //we write the output
                  sprintf(output,c);
#ifdef __WIN32__
                  WSACleanup();
#endif
                  return output;
                }
                else
                {
                  *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
                  goto end_function;
                }
              }
            }
            else
            {
              goto end_function;
            }
          }
        }
        else
        {
          //invalid get file type
          *error = SPLT_FREEDB_ERROR_GETTING_INFOS;
          goto end_function;
        }
      }
    }
    else
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      goto end_function;
    }

end_function:
    ;
#ifdef __WIN32__
    WSACleanup();
#endif
    return NULL;
  }
}

  //deprecated, and not in use
  //but may useful for the implementation of the proxy
  /*int search_freedb (splt_state *state)
    {
    char *c, *e=NULL;
    FILE *output = NULL;
    struct sockaddr_in host;
    struct hostent *h;
    struct splt_addr dest;

    if ((c=getenv("HOME"))!=NULL) sprintf(message, "%s/"PROXYCONFIG, c);
    else strncpy(message, PROXYCONFIG, strlen(PROXYCONFIG));

    if (!(output=fopen(message, "r"))) {
    if (!(output=fopen(message, "w+"))) {
    fprintf(stderr, "\nWARNING Can't open config file ");
    perror(message);
    }
    else {
    fprintf (stderr, "Will you use a proxy? (y/n): ");
    fgets(junk, 200, stdin);
    if (junk[0]=='y') {
    fprintf (stderr, "Proxy Address: ");
    fgets(junk, 200, stdin);
    fprintf (output, "PROXYADDR=%s", junk);
    fprintf (stderr, "Proxy Port: ");
    fgets(junk, 200, stdin);
    fprintf (output, "PROXYPORT=%s", junk);
    fprintf (stderr, "Need authentication? (y/n): ");
    fgets(junk, 200, stdin);
    if (junk[0]=='y') {
    fprintf (output, "PROXYAUTH=1\n");
    fprintf (stderr, "Would you like to save password (insecure)? (y/n): ");
    fgets(junk, 200, stdin);
    if (junk[0]=='y') {
    login (message);
    e = b64(message, strlen(message));
    fprintf (output, "%s\n", e);
    memset(message, 0x00, strlen(message));
    memset(e, 0x00, strlen(e));
    free(e);
    }
    }
    }
    }
    }
        
    if (splt_t_freedb_get_found_cds(state)<=0) {
    if (dest.proxy) {
    if (strstr(buffer, "HTTP/1.0")!=NULL) {
    if ((c = strchr (buffer, '\n'))!=NULL)
    buffer[c-buffer]='\0';
    fprintf (stderr, "Proxy Reply: %s\n", buffer);
    }
    }
    }
    return 0;
    }*/
