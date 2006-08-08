/**********************************************************
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <matteo.trotta@lib.unimib.it>
 * Copyright (c) 2005-2006 Munteanu Alexandru - io_alex_2002@yahoo.fr
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
void splt_tag_put_filenames_from_tags(splt_state *state,
                                      int tracks, int *error)
{
  int i;
  char *artist0;
  if (splt_t_get_tags_char_field(state, 0, 
                                 SPLT_TAGS_ARTIST) != NULL)
    {
      artist0 = 
        strdup(splt_t_get_tags_char_field(state, 0, 
                                          SPLT_TAGS_ARTIST));
    }
  else
    {
      artist0 = NULL;
    }
  
  char *album0 = splt_t_get_tags_char_field(state, 0,
                                            SPLT_TAGS_ALBUM);
  char *year0 = splt_t_get_tags_char_field(state, 0,
                                           SPLT_TAGS_YEAR);
  char *performer = NULL;
  unsigned char genre0 = splt_t_get_tags_uchar_field(state, 0,
                                                     SPLT_TAGS_GENRE);
  int tags_error = SPLT_OK;
  
  //if we have the defaults for the output,
  if (splt_t_get_int_option(state, SPLT_OPT_OUTPUT_DEFAULT))
    {
      //we put the default output if we have the default output
      splt_t_new_oformat(state, SPLT_DEFAULT_OUTPUT);
      
      //we put the real performer in the artist
      for(i = 0; i < tracks;i++)
        {
          performer = 
            splt_t_get_tags_char_field(state, i,
                                       SPLT_TAGS_PERFORMER);
          //we put performer if found
          if ((performer != NULL) && (performer[0] != '\0'))
            {
              tags_error = 
                splt_t_set_tags_char_field(state, i, SPLT_TAGS_ARTIST,
                                           performer);
              if (tags_error != SPLT_OK)
                {
                  *error = tags_error;
                }
            }
          else
            {
              //we put the artist0
              tags_error = splt_t_set_tags_char_field(state, i, SPLT_TAGS_ARTIST,
                                                      artist0);
            }
          
          //we put the same album, year and genre everywhere
          if (i != 0)
            {
              tags_error = splt_t_set_tags_char_field(state, i, SPLT_TAGS_ALBUM,
                                                      album0);
              tags_error = splt_t_set_tags_char_field(state, i, SPLT_TAGS_YEAR,
                                                      year0);
              tags_error = splt_t_set_tags_uchar_field(state, i, SPLT_TAGS_GENRE,
                                                       genre0);
              if (tags_error != SPLT_OK)
                {
                  *error = tags_error;
                }
            }
        }
    }
  else
    {
      //we put the same artist, genre, album and year everywhere
      for(i = 1;i<tracks;i++)
        {
          tags_error = splt_t_set_tags_uchar_field(state, i, SPLT_TAGS_GENRE,
                                                   genre0);
          tags_error = splt_t_set_tags_char_field(state, i, SPLT_TAGS_ARTIST,
                                                  artist0);
          tags_error = splt_t_set_tags_char_field(state, i, SPLT_TAGS_ALBUM,
                                                  album0);
          tags_error = splt_t_set_tags_char_field(state, i, SPLT_TAGS_YEAR,
                                                  year0);
          tags_error = splt_t_set_tags_uchar_field(state, i, SPLT_TAGS_GENRE,
                                                   genre0);
          if (tags_error != SPLT_OK)
            {
              *error = tags_error;
            }
        }
    }
  
  if (*error >= 0)
    {
      int err_format = SPLT_OK;
      
      if (splt_t_get_oformat(state) != NULL)
        {
          //we put the outputted filename
          char *old_format = strdup(splt_t_get_oformat(state));
          splt_t_set_oformat(state, old_format,&err_format);
          free(old_format);
        }
      
      if (err_format >= 0)
        {
          //we set the current split to 0
          splt_t_set_current_split(state,0);
          do {      
            int filename_error = SPLT_OK;
            //get output format filename
            filename_error = splt_u_put_output_filename(state);
            if (filename_error != SPLT_OK)
              {
                *error = filename_error;
                break;
              }
            splt_t_current_split_next(state);
          } while (splt_t_get_current_split(state)<tracks);
        }
      else
        {
          *error = err_format;
        }
    }
  
  if (artist0)
    {
      free(artist0);
    }
}

/***********************/
/* cue */

//if we have a string "test", this functions cuts out the two "
//and cleans the string of other unwanted characters
static int splt_cue_set_value(splt_state *state, char *in, 
                              int index, int tag_field)
{
  int error = SPLT_OK;
  
  char *ptr_b, *ptr_e;
  ptr_b = strchr(in, '"');
  if (ptr_b==NULL)
    {
      return SPLT_INVALID_CUE_FILE;
    }
  ptr_e = strchr(ptr_b+1, '"');
  if (ptr_e==NULL)
    {
      return SPLT_INVALID_CUE_FILE;
    }
  *ptr_e='\0';
  
  char *out;
  if ((out = malloc(sizeof(char) * (strlen(ptr_b)+1)))
      == NULL)
    {
      error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    }
  else
    {
      strncpy(out, ptr_b, (strlen(ptr_b)+1));
      splt_u_cleanstring(out);
      int tags_err = SPLT_OK;
      tags_err = splt_t_set_tags_char_field(state, index, tag_field, out);
      if (tags_err != SPLT_OK)
        {
          error = tags_err;
        }
      free(out);
    }
  
  return error;
}

//error = possible error that we return
//gets the cue splitpoints from a file and puts them in the state
//returns the number of tracks found
int splt_cue_put_splitpoints(char *file, splt_state *state, 
                             int *error)
{
  //clear previous splitpoints
  splt_t_free_splitpoints_tags(state);

  //default no error
  *error = SPLT_FREEDB_CUE_OK;
  
  int append_error = SPLT_OK;
  //our file
  FILE *file_input;
  //the line we read from the file
  char line[1024];
  char *ptr, *dot;
  ptr = dot = NULL;
  //tracks = number of tracks found
  int tracks = -1, time_for_track = SPLT_TRUE;
  //counter, counts if we have at least one INDEX on the file
  int type, performer = SPLT_FALSE,
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
  
  //we open the file
  if (!(file_input=fopen(file, "r")))
    {
      *error = SPLT_CUE_ERROR_CANNOT_OPEN_FILE_READING;
      return tracks;
    }
  else
    //if we can open the file
    {
      //put pointer at the beginning of the file
      if (fseek (file_input, 0, SEEK_SET) == 0)
        {
          //we read the file line by line
          while (fgets(line, 1024, file_input)!=NULL)
            {
              type = SPLT_CUE_NOTHING;
              
              //we read strings from file TRACK,TITLE,AUDIO,PERFORMER,
              //INDEX
              if ((strstr(line, "TRACK")!=NULL)&&(strstr(line, "AUDIO")!=NULL))
                {
                  type = SPLT_CUE_TRACK;
                }
              else if (strstr(line, "TITLE")!=NULL)
                {
                  type = SPLT_CUE_TITLE;
                }
              else if (strstr(line, "PERFORMER")!=NULL)
                {
                  type = SPLT_CUE_PERFORMER;
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
                case SPLT_CUE_NOTHING:break;
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
                      *error = SPLT_INVALID_CUE_FILE;
                      goto function_end;
                    }
                  time_for_track = SPLT_FALSE;
                  break;
                  //TITLE found
                case SPLT_CUE_TITLE:
                  if (tracks == -1)
                    {
                      if ((temp_error = splt_cue_set_value(state, line, 0, SPLT_TAGS_ALBUM)) 
                          != SPLT_OK)
                        {
                          *error = temp_error;
                          goto function_end;
                        }
                    }
                  else
                    {
                      if (tracks > 0)
                        {
                          if ((temp_error = splt_cue_set_value(state, line, tracks-1, SPLT_TAGS_TITLE)) 
                              != SPLT_OK)
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
                      if ((temp_error = splt_cue_set_value(state, line, 0, SPLT_TAGS_ARTIST)) 
                          != SPLT_OK)
                        {
                          *error = temp_error;
                          goto function_end;
                        }
                    }
                  else
                    {
                      if (tracks>0)
                        {
                          if ((temp_error = splt_cue_set_value(state, line, tracks-1, SPLT_TAGS_PERFORMER)) 
                              != SPLT_OK)
                            {
                              *error = temp_error;
                              goto function_end;
                            }
                        }
                      performer = SPLT_TRUE;
                    }
                  break;
                  //INDEX 01 found
                  //we get the 
                case SPLT_CUE_INDEX:
                  line[strlen(line)-1]='\0';
                  ptr += 9;
                  if ((dot = strchr(ptr, ':'))==NULL)
                    {
                      *error = SPLT_INVALID_CUE_FILE;
                      goto function_end;
                    }
                  else
                    {
                      //we replace : with . for the sscanf
                      ptr[dot-ptr] = ptr[dot-ptr+3] = '.';
                      //we clean the string for unwanted characters
                      splt_u_cleanstring(ptr);
                      //we convert to hundreths of seconds and put splitpoints
                      if (tracks>0)
                        {
                          long hundr_seconds = splt_u_convert_hundreths(ptr);
                          if (hundr_seconds==-1)
                            {
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
          append_error =
            splt_t_append_splitpoint(state, LONG_MAX,
                                     "description here");
        }
      else
        {
          *error = SPLT_ERROR_SEEKING_FILE;
          goto function_end;
        }
      
      //if we don't find INDEX on the file, error
      if (counter == 0)
        {
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
      fclose(file_input);
    }
  
  //we return the number of tracks found
  return tracks;
}

/***********************/
/* cddb */

//error = possible error that we return
//puts the cddb splitpoints from the file in the state
//returns number of tracks
//see freedb file format documentation on freedb.org
int splt_cddb_put_splitpoints (char *file, splt_state *state,
                               int *error)
{
  //clear previous splitpoints
  splt_t_free_splitpoints_tags(state);
  
  //default no error
  *error = SPLT_FREEDB_CDDB_OK;
  
  //our file
  FILE *file_input;
  //the line we read
  char line[1024];
  char prev[10];
  //performer_title_split is where we split if we have
  //performer / title on cddb
  char *number = NULL, *c;
  //for the performer
  char *c2;
  //tracks is the number of tracks
  int tracks = 0, i, j;
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
      *error = SPLT_CDDB_ERROR_CANNOT_OPEN_FILE_READING;
      return tracks;
    }
  else
    //if we can open the file
    {
      //we go at the beggining of the file
      if(fseek (file_input, 0, SEEK_SET) == 0)
        {
          //we search for the string "Track frame offset"
          do {
            if ((fgets(line, 1024, file_input))==NULL)
              {
                *error = SPLT_INVALID_CDDB_FILE;
                goto function_end;
              }
            number = strstr(line, "Track frame offset");
          } while (number == NULL);
          
          memset(prev, 0, 10);
          
          //we read the track offsets
          do {
            if ((fgets(line, 1024, file_input))==NULL)
              {
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
                double temp;
                temp = atof(number);
                
                //we append the splitpoint
                append_error =
                  splt_t_append_splitpoint(state, temp * 100, NULL);
                //we count the tracks
                tracks++;
                
                if (append_error != SPLT_OK)
                  {
                    *error = append_error;
                    goto function_end;
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
                    *error = SPLT_INVALID_CDDB_FILE;
                    goto function_end;
                  }
                number = strstr(line, "Disc length");
              } while (number == NULL);
            }
          
          //we get the max disc length (in seconds)
          i = 0;
          while ((isdigit(line[i])==0) && (line[i]!='\0')) {
            i++;
            number = line + i;
          }
          
          //if no error, ?
          if (*error != SPLT_INVALID_CDDB_FILE)
            {
              double temp2;
              temp2 = atof(number);
              
              //we append the splitpoint
              append_error =
                splt_t_append_splitpoint(state, temp2 * 100, NULL);
              if (append_error != SPLT_OK)
                {
                  *error = append_error;
                  goto function_end;
                }
            }
          else
            {
              //we append the splitpoint
              append_error =
                splt_t_append_splitpoint(state, 0, NULL);
              if (append_error != SPLT_OK)
                {
                  *error = append_error;
                  goto function_end;
                }
            }
          
          //we convert the points previously found
          for (i=tracks-1; i>=0; i--) 
            {
              split1 = 
                splt_t_get_splitpoint_value(state, i, &get_error);
              split2 = 
                splt_t_get_splitpoint_value(state, 0, &get_error);
              if (get_error != SPLT_OK)
                {
                  *error = get_error;
                  goto function_end;
                }
              
              change_error = 
                splt_t_set_splitpoint_value(state,
                                            i, split1 - split2);
              if (change_error != SPLT_OK)
                {
                  *error = change_error;
                  goto function_end;
                }
              
              change_error = 
                splt_t_set_splitpoint_value(state,
                                            i, split1/75);
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
            if ((fgets(line, 1024, file_input))==NULL)
              {
                *error = SPLT_INVALID_CDDB_FILE;
                goto function_end;
              }
            line[strlen(line)-1] = '\0';
            if (strlen(line)>0)
              {
                if (line[strlen(line)-1]=='\r')
                  line[strlen(line)-1]='\0';
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
            
            if ((number=strchr(line, '='))==NULL) 
              {
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
                if (len>10) len = 10;
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
                    
                    perfor = splt_t_get_tags_char_field(state, j-1, 
                                                        SPLT_TAGS_PERFORMER);
                    splt_u_cleanstring(perfor);
                    number = c+1;
                  }
                
                number = splt_u_cleanstring (number);
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
                    //strncat(state->tags.id[j-1].title,
                    //number, 128-strlen(state->tags.id[j-1].title));
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
                        int i_temp;
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
                        artist = 
                          splt_t_get_tags_char_field(state,0, SPLT_TAGS_ARTIST);
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
                        splt_u_cleanstring(artist);
                        
                        //we put the album
                        i += 2;
                        number = splt_u_cut_spaces_from_begin(number+i);
                        
                        tags_error = splt_t_set_tags_char_field(state, 0, SPLT_TAGS_ALBUM, 
                                                                number);
                        if (tags_error != SPLT_OK)
                          {
                            *error =tags_error;
                            goto function_end;
                          }
                        
                        album = 
                          splt_t_get_tags_char_field(state,0, SPLT_TAGS_ALBUM);
                        splt_u_cleanstring(album);
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
                        //strncpy(state->tags.id[j-1].title, t, 128);
                      }
                    j++;
                  }
              }
            strncpy(prev, temp, 10);
            
          } while (j<=tracks);
          
          //we search for
          //YEAR (the year) and ID3 genre
          tags_error = splt_t_set_tags_uchar_field(state, 0, SPLT_TAGS_GENRE,
                                                   12);
          if (tags_error != SPLT_OK)
            {
              *error =tags_error;
              goto function_end;
            }
          
          while ((fgets(line, 1024, file_input))!=NULL)
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
                          tags_error = 
                            splt_t_set_tags_char_field(state, 0, SPLT_TAGS_YEAR,
                                                       c+6);
                          if (tags_error != SPLT_OK)
                            {
                              *error = tags_error;
                              goto function_end;
                            }
                        }
                      if ((c=strstr(number, "ID3G"))!=NULL) 
                        {
                          strncpy(line, c+6, 3);
                          tags_error = 
                            splt_t_set_tags_uchar_field(state, 0, SPLT_TAGS_GENRE,
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
          *error = SPLT_ERROR_SEEKING_FILE;
          goto function_end;
        }
      
    function_end:
      fclose(file_input);
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

//we analyse the buffer for the CDs results
static int splt_freedb_analyse_cd_buffer (char *buf, int size, splt_state *state,
                                          int *error)
{
  char *c = NULL, *add = NULL;
  int i = 0;
  
  //temporary string
  char str_temp[3] = "";
  //temporary character
  char t;
  //the name of the album
  char *album_name = NULL;
  
  //30 is a safe value for preventing buffer exceed
  if (size<30)
    {
      return 0; 
    }
  
  add = buf+size;
  buf = strstr(buf, "cat=");
  if (buf==NULL)
    {
      return 0;
    }
  else {
    do {
      i=0;
      buf += 4;
      
      //-1 = continue with the next buffer
      if ((c=strchr(buf, '&'))==NULL)
        return -1;
      
      if (c==buf)
        continue;
      
      //we set the category and the disc id
      splt_t_freedb_set_disc(state,splt_t_freedb_get_found_cds(state),
                             buf,c+4,c-buf);
      buf=c+4;
      buf = buf + SPLT_DISCIDLEN + 2;
      
      //if buf is NULL, get out
      if (buf == NULL)
        {
          return -1;
        }
      else
        {
          //-1 = continue with the next buffer
          if ((c=strchr(buf, '<'))==NULL)
            {
              return -1;
            }
          else 
            {
              if (c == buf)
                {
                  if ((buf=strchr(buf, '>'))==NULL)
                    return -1;
                  buf++;
                  if ((c=strchr(buf, '<'))==NULL)
                    return -1;
                  i=-1;
                }
            }
        }
      
      album_name = calloc(2,1);
      //we count the characters for the realloc
      int count = 2;
      //we get the name of the album
      while ( (buf<c) && (buf<add) )
        {
          t = *buf;
          buf++;count++;
          
          sprintf(str_temp,"%c",t);
          strcat(album_name, str_temp);
          album_name = realloc(album_name, count);
        }
      
      //here we have in album_name the name of the current album      
      splt_t_freedb_append_result(state, album_name, i);
      
      //we free the local variable
      //for the album name
      if (album_name)
        {
          free(album_name);
        }
      
      splt_t_freedb_found_cds_next(state);
      
    } while ((buf<(add-30)) && 
             (splt_t_freedb_get_found_cds(state) < SPLT_MAXCD) && 
             ((buf=strstr(buf, "cat="))!=NULL));
    
    return splt_t_freedb_get_found_cds(state);
  }
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

static splt_addr splt_freedb_useproxy(FILE *in, splt_addr dest)
{
  char line[270];
  //char *ptr;
        
  dest.proxy=0;
  memset(dest.hostname, 0, 256);
  memset(line, 0, 270);
        
  if (in != NULL) {
    /*
    //proxy NOT IMPLEMENTED YET
            
    fseek(in, 0, SEEK_SET);

    if (fgets(line, 266, in)!=NULL) {
    if (strstr(line, "PROXYADDR")!=NULL) {
    line[strlen(line)-1]='\0';
    if ((ptr = strchr(line, '='))!=NULL) {
    ptr++;
    strncpy(dest.hostname, ptr, 255);
    }

    if (fgets(line, 266, in)!=NULL) {
    if (strstr(line, "PROXYPORT")!=NULL) {
    line[strlen(line)-1]='\0';
    if ((ptr = strchr(line, '='))!=NULL) {
    ptr++;
    dest.port = atoi (ptr);
    dest.proxy=1;
    }
    fprintf (stderr, "Using Proxy: %s on Port %d\n", dest.hostname, dest.port);
    if (fgets(line, 266, in)!=NULL) {
    if (strstr(line, "PROXYAUTH")!=NULL) {
    line[strlen(line)-1]='\0';
    if ((ptr = strchr(line, '='))!=NULL) {
    ptr++;
    if (ptr[0]=='1') {
    if (fgets(line, 266, in)!=NULL) {
    dest.auth = malloc(strlen(line)+1);
    if (dest.auth==NULL) {
    perror("malloc");
    exit(1);
    }
    memset(dest.auth, 0x0, strlen(line)+1);
    strncpy(dest.auth, line, strlen(line));
    }
    else {
    login(line);
    dest.auth = b64(line, strlen(line));
    memset(line, 0x00, strlen(line));
    }
    }
    }
    }
    }
    else dest.auth = NULL;
    }
    }
    }
    }
    */
  }
        
  if (!dest.proxy) 
    {
      strncpy(dest.hostname, SPLT_FREEDB_SITE, 255);
      dest.port = SPLT_FREEDB_PORT1;
    }
        
  return dest;
}

//search the freedb according to "search"
//returns possible errors
//we have possible errors in result
int splt_freedb_process_search(splt_state *state, char *search)
{
  //possible error that we will return
  int error = SPLT_FREEDB_OK;
  //socket and internet structures
  struct sockaddr_in host;
  struct hostent *h;
  splt_addr dest;
  //e is used for the end of the buffer
  //c is used for the buffer read
  char *c, *e=NULL;
  int i, tot=0;
  //the message delivered to the server
  char *message;
  //the buffer that we are using to read incoming transmission
  char buffer[SPLT_FREEDB_BUFFERSIZE];
  
  //fd = socket identifier
#ifdef __WIN32__
  long winsockinit;
  WSADATA winsock;
  SOCKET fd;
  winsockinit = WSAStartup(0x0101,&winsock);
#else
  int fd;
#endif
  
  //transform " " to "+"
  int string_length = strlen(search);
  for (i = 0; i < string_length; i++)
    {
      if (search[i] == ' ')
        {
          search[i] = '+';
        }
    }
  
  //null because we dont use proxy for now
  dest = splt_freedb_useproxy(NULL, dest);
  
  //we get the hostname of freedb
  if((h=gethostbyname(dest.hostname))==NULL)
    {
      error = SPLT_FREEDB_ERROR_CANNOT_GET_HOST;
      return error;
    }
  else
    {
      //we prepare socket
      memset(&host, 0x0, sizeof(host));
      host.sin_family=AF_INET;
      host.sin_addr.s_addr=((struct in_addr *) (h->h_addr)) ->s_addr;
      host.sin_port=htons(dest.port);
      
      //initialize socket
      if((fd=socket(AF_INET, SOCK_STREAM, 0))==-1)
        {
          error = SPLT_FREEDB_ERROR_INITIALISE_SOCKET;
          return error;
        }
      else
        {
          //make connection
          if ((connect(fd, (void *)&host, sizeof(host))) < 0)
            {
              error = SPLT_FREEDB_ERROR_CANNOT_CONNECT;
              closesocket(fd);
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
              int malloc_number = strlen(SPLT_FREEDB_SEARCH)+strlen(search)+5;
              if((message = malloc(malloc_number)) == NULL)
                {
                  error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
                }
              else
                {
                  snprintf(message,malloc_number,
                           "GET "SPLT_FREEDB_SEARCH"\n", search);
                  
                  //message sent
                  if((send(fd, message, strlen(message), 0))==-1)
                    {
                      error = SPLT_FREEDB_ERROR_CANNOT_SEND_MESSAGE;
                      goto function_end;
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
                                  error = SPLT_FREEDB_ERROR_CANNOT_RECV_MESSAGE;
                                  goto function_end;
                                }
                              tot += i;
                              buffer[tot]='\0';
                              c += i;
                            } while ((i>0)&&(tot<SPLT_FREEDB_BUFFERSIZE-1)
                                     &&((e=strstr(buffer, "</html>"))==NULL));
                            
                            //we analyse the buffer
                            tot = splt_freedb_analyse_cd_buffer(buffer, tot, state,&error);
                            if (error < 0)
                              {
                                goto function_end;
                              }
                            
                            if (tot == -1) continue;
                            if (tot == -2) break;
                            
                          } while ((i>0)&&(e==NULL)&&
                                   (splt_t_freedb_get_found_cds(state)<SPLT_MAXCD));
                          
                          //no cd found
                          if (splt_t_freedb_get_found_cds(state)==0) 
                            {
                              error = SPLT_FREEDB_NO_CD_FOUND;
                              goto function_end;
                            }
                          //erroror occured while getting freedb infos
                          if (splt_t_freedb_get_found_cds(state)==-1) 
                            {
                              error = SPLT_FREEDB_ERROR_GETTING_INFOS;
                              goto function_end;
                            }
                          //max cd number reached
                          if (splt_t_freedb_get_found_cds(state)==SPLT_MAXCD) 
                            {
                              error = SPLT_FREEDB_MAX_CD_REACHED;
                              goto function_end;
                            }
                        }
                    }
                function_end:
                  //free memory
                  free(message);
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
char *splt_freedb_get_file(splt_state *state, int i, int *error)
{
  //possible error that we will return
  *error = SPLT_FREEDB_FILE_OK;
  //the freedb file that we will return
  char *output = NULL;
  
  //socket and internet structures
  struct sockaddr_in host;
  struct hostent *h;
  splt_addr dest;
  char *message;
  int tot=0;
  //the buffer that we are using to read incoming transmission
  char buffer[SPLT_FREEDB_BUFFERSIZE];
  //e is used for the end of the buffer
  //c is used for the buffer read
  char *c, *e=NULL;
  
  //fd = socket identifier
#ifdef __WIN32__
  long winsockinit;
  WSADATA winsock;
  SOCKET fd;
  winsockinit = WSAStartup(0x0101,&winsock);
#else
  int fd;
#endif
  
  //NULL because we dont use proxy for now
  dest = splt_freedb_useproxy(NULL, dest);
  
  //we get the hostname of freedb
  if((h=gethostbyname(dest.hostname))==NULL)
    {
      *error = SPLT_FREEDB_ERROR_CANNOT_GET_HOST;
      return NULL;
    }
  else
    {
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
      //        else {
      char *cd_category = 
        splt_t_freedb_get_disc_category(state, i);
      char *cd_id = 
        splt_t_freedb_get_disc_id(state, i);
      
      int malloc_number = strlen(cd_category)+strlen(cd_id) + 20;
      message = malloc(malloc_number);
      if (message != NULL)
        {
          snprintf(message, malloc_number,
                   "CDDB READ %s %s\n", cd_category, cd_id);
          host.sin_port=htons(SPLT_FREEDB_PORT2);
          
          //open socket
          if((fd=socket(AF_INET, SOCK_STREAM, 0))==-1)
            {
              *error = SPLT_FREEDB_ERROR_CANNOT_OPEN_SOCKET;
              free(message);
              return NULL;
            }
          else
            {
              //connect to host
              if ((connect(fd, (void *)&host, sizeof(host)))==-1)
                {
                  *error = SPLT_FREEDB_ERROR_CANNOT_CONNECT;
                  goto bloc_end;
                }
              else
                {
                  //possible errors + proxy
                  if (!dest.proxy) {
                    i=recv(fd, buffer, SPLT_FREEDB_BUFFERSIZE-1, 0);
                    if (i == -1)
                      {
                        *error = SPLT_FREEDB_ERROR_CANNOT_RECV_MESSAGE;
                        goto bloc_end;
                      }
                    buffer[i]='\0';
                    
                    if (strncmp(buffer,"201",3)!=0)  
                      {
                        *error = SPLT_FREEDB_ERROR_SITE_201;
                        goto bloc_end;
                      }
                    
                    if((send(fd, SPLT_FREEDB_HELLO, strlen(SPLT_FREEDB_HELLO), 0))==-1)
                      {
                        *error = SPLT_FREEDB_ERROR_CANNOT_SEND_MESSAGE;
                        goto bloc_end;
                      }
                    i=recv(fd, buffer, SPLT_FREEDB_BUFFERSIZE-1, 0);
                    if (i == -1)
                      {
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
                            break;
                          }
                        
                        //if errors
                        if (tot == 0)
                          {
                            if ((strncmp(buffer,"50",2) == 0)
                                || (strncmp(buffer,"40",2) == 0))
                              {
                                *error = SPLT_FREEDB_ERROR_SITE;
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
                              *error = SPLT_FREEDB_ERROR_CANNOT_SEND_MESSAGE;
                              goto bloc_end;
                            }
                        }
                    }
                }
              
            bloc_end:
              free(message);
              closesocket(fd);
            }
          
          //if we don't have an error
          if (*error == SPLT_FREEDB_FILE_OK)
            {
              if (tot==0) 
                {
                  *error = SPLT_FREEDB_ERROR_BAD_COMMUNICATION;
                  return NULL;
                }
          
              if (e!=NULL)
                {
                  buffer[e-buffer+1]='\0';
                }
              
              //if invalid server answer
              if ((strstr(buffer, "database entry follows"))==NULL)
                {
                  if ((c = strchr (buffer, '\n'))!=NULL)
                    {
                      buffer[c-buffer]='\0';
                    }
                  *error = SPLT_FREEDB_ERROR_INVALID_SERVER_ANSWER;
                  return NULL;
                }
              else
                {
                  if ((c = strchr (buffer, '#'))==NULL)
                    {
                      output = NULL;
                    }
                  else
                    {
                      output = malloc(sizeof(char)*(strlen(c)+1));
                      if (output != NULL)
                        {
                          sprintf (output,c);
                        }
                      else
                        {
                          *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
                          return NULL;
                        }
                    }
                }
            }
        }
      else
        {
          *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
          return NULL;
        }
    }
  
  return output;
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

