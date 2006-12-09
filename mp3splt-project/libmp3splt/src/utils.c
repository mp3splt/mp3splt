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

#include <sys/stat.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include <dirent.h>

#include "splt.h"

extern short global_debug;

/****************************/
/* utils for conversion */

//converts string s in hundredth
//and returns seconds
//returns -1 if it cannot convert (usually we hope it can :)
long splt_u_convert_hundreths (char *s)
{
  long minutes=0, seconds=0, hundredths=0, i;
  long hun;
  
  for(i=0; i<strlen(s); i++) // Some checking
    if ((s[i]<0x30 || s[i] > 0x39) && (s[i]!='.'))
      return -1;
  
  if (sscanf(s, "%ld.%ld.%ld", &minutes, &seconds, &hundredths)<2)
    return -1;
  
  if ((minutes < 0) || (seconds < 0) || (hundredths < 0))
    return -1;

  if ((seconds > 59) || (hundredths > 99))
    return -1;
  
  if (s[strlen(s)-2]=='.')
    hundredths *= 10;
  
  hun = hundredths;
  hun += (minutes*60 + seconds) * 100;
  
  return hun;
}

//convert to decibels
float splt_u_convert2dB(double input)
{
  float level;
  if (input<=0.0)
    {
      level = -96.0;
    }
  else 
    {
      level = 20 * log10(input);
    }
  
  return level;
}

//convert from decibels
double splt_u_convertfromdB(float input)
{
  double amp;
  if (input<-96.0)
    {
      amp = 0.0;
    }
  else 
    {
      amp = pow(10.0, input/20.0);
    }
  
  return amp;
}

/****************************/
/* utils for file infos */

//returns -1 is the file is damaged
//0 otherwise
int splt_u_getword (FILE *in, off_t offset, int mode, 
                    unsigned long *headw)
{
  int i;
  *headw = 0;
  
  if (fseeko(in, offset, mode)==-1)
    {
      return -1;
    }
  
  for (i=0; i<4; i++)
    {
      if (feof(in)) 
        {
          return -1;
        }
      *headw = *headw << 8;
      *headw |= fgetc(in);
    }
  
  return 0;
}

//returns the file length
off_t splt_u_flength (FILE *in)
{
  struct stat info;
  if (fstat(fileno(in), &info)==-1)
    {
      return -1;
    }
  return info.st_size;
}

/*****************************************************/
/* utils manipulating strings (including filenames) */

//cleans the string of weird characters like ? " | : > < * \ \r
char *splt_u_cleanstring (char *s)
{
  int i, j=0;
  char *copy;
  copy = strdup(s);
  for (i=0; i<=strlen(copy); i++)
    {
      if ((copy[i]!='\\')&&(copy[i]!='/')&&(copy[i]!='?')
          &&(copy[i]!='*')&&(copy[i]!=':')&&(copy[i]!='"')
          &&(copy[i]!='>')&&(copy[i]!='<')&&(copy[i]!='|')
          &&(copy[i]!='\r'))
        {
          s[j++] = copy[i];
        }
    }
  free(copy);
  
  // Trim string. I will never stop to be surprised about cddb strings dirtiness! ;-)
  for (i=strlen(s)-1; i >= 0; i--) 
    {
      if (s[i]==' ')
        {
          s[i] = '\0';
        }
      else 
        {
          break;
        }
    }
  
  return s;
}

//cuts spaces from the begin
char *splt_u_cut_spaces_from_begin(char *c)
{
  //cut spaces from the begin
  if (*c == ' ')
    {
      while (*c == ' ')
        {
          c++;
        }
    }
  
  return c;
}

//cuts spaces at the end
char *splt_u_cut_spaces_at_the_end(char *c)
{
  //we cut spaces at the end
  while (*c == ' ')
    {
      *c = '\0';
      c--;
    }
  
  return c;
}

//finding the real name of the file, without the path
char *splt_u_get_real_name(char *filename)
{
  char *c = NULL;
  while ((c = strchr(filename, SPLT_DIRCHAR))
         !=NULL)
    {
      filename = c + 1;
    }
  
  return filename;
}

//returns the filename of the new file to be created, the two integer
//parameters are for the 2 splitpoints
//Warning !! the first splitpoint begins at 0 !!!
//i is the current split, to find the current name in 
//state->fn[i]
//error is the possible error
//result must be freed
static char *splt_u_get_mins_secs_filename(char *filename, 
                                           splt_state *state,
                                           long split_begin,
                                           long split_end,
                                           int i, int *error)
{
  int number_of_splits = 0;
  splt_point *points = 
    splt_t_get_splitpoints(state, &number_of_splits);
  
  //calculating minutes and seconds to modify filename
  char mins_char[16], secs_char[16], hundr_secs_char[16],
    mins_char2[16], secs_char2[16], hundr_secs_char2[16];
  
  char *fname = NULL, *fname2 = NULL;
  int fname2_malloc_number = 0,fname_malloc_number = 0;
  
  fname2_malloc_number = fname_malloc_number = 
    strlen(filename) + 256;
  
  if((fname = calloc(fname_malloc_number*sizeof(char),1))
     != NULL)
    {
      if((fname2 = calloc(fname2_malloc_number*sizeof(char),1))
         != NULL)
        {
          //hundreds of seconds
          snprintf(hundr_secs_char,16, "%ld", split_begin % 100);
          snprintf(hundr_secs_char2,16, "%ld", split_end % 100);
          //transform to seconds
          split_begin = split_begin / 100;
          split_end = split_end / 100;
          //calculating seconds
          snprintf(secs_char,16, "%ld", split_begin % 60);
          snprintf(secs_char2,16, "%ld", split_end % 60);
          //calculating minutes
          snprintf(mins_char,16, "%ld", split_begin / 60);
          snprintf(mins_char2,16, "%ld", split_end / 60);
          
          //if we have this splitpoint
          if (splt_t_splitpoint_exists(state, i))
            {
              if (points[i].name != NULL)
                {
                  char temp[3];
                  //transform " " to "\ "
                  int j;
                  for (j = 0; j < strlen(points[i].name); j++)
                    {
                      if ((state->split.points[i].name[j] == ' '))
                        {
                          strcat(fname, " ");
                        }
                      else
			{
			  if ((state->split.points[i].name[j] == '\\') ||
			      (state->split.points[i].name[j] == '/'))
			    {
			      strcat(fname, "-");
			    }
			  else
			    {
			      snprintf(temp,2,"%c", state->split.points[i].name[j]);
			      strcat(fname,temp);
			    }
			}
                    }
                }
              
              //if fn[i] is "", we put the same name as the
              //original file
              if ((points[i].name == NULL)
                  || (strcmp(points[i].name,"") == 0))
                {
                  snprintf(fname,strlen(filename),"%s", filename);
                  //we cut the extension of the original file
		  char *temp = strrchr(fname,'.');
		  *temp='\0';
                }
            }
          else
            {
              splt_u_error(SPLT_IERROR_INT,__func__, i, NULL);
            }
          
          snprintf(fname2,fname2_malloc_number,
                   "%s_%sm_%ss_%sh__%sm_%ss_%sh", 
                   fname, mins_char, secs_char, hundr_secs_char,
                   mins_char2, secs_char2,hundr_secs_char2);
          
          //put the extension according to the file type
          if (splt_t_get_file_format(state) == SPLT_MP3_FORMAT)
            {
              strcat(fname2, SPLT_MP3EXT);
            }
#ifndef NO_OGG
          else 
            {
              strcat(fname2, SPLT_OGGEXT);
            }
#endif
        }
      else
        {
          *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
        }
      //freeing memory
      free(fname);
    }
  else
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    }
  
  return fname2;
}

//puts the complete filename with the correct path
//error is the possible error
void splt_u_set_complete_mins_secs_filename(splt_state *state, int *error)
{
  int current_split = splt_t_get_current_split(state);
  int get_error = SPLT_OK;
  int split_begin = 
    splt_t_get_splitpoint_value(state, current_split, &get_error);
  int split_end = 
    splt_t_get_splitpoint_value(state, current_split+1, &get_error);
  char *filename = splt_t_get_filename_to_split(state);
  char *filename2 = strdup(filename);
  
  //filename of the new created file
  char *fname = NULL;
  
  //get the filename without the path
  char *filename3 = strdup(splt_u_get_real_name(filename2));
  fname = splt_u_get_mins_secs_filename(filename3, state,
                                        split_begin, split_end,
                                        current_split,error);
  //free memory
  if (filename2)
    {
      free(filename2);
    }
  if (filename3)
    {
      free(filename3);
    }
  
  if ((*error >= 0) && (get_error == SPLT_OK))
    {
      //we cut the extension
      char *fname2 = strdup(fname);
      fname2[strlen(fname2)-4]='\0';
      //we put the filename in the state
      splt_t_set_splitpoint_name(state, current_split, fname2);
      free(fname2);
    }
  else
    //if error, put NULL ?
    {
      //we put the name in the state
      int change_error = SPLT_OK;
      change_error = splt_t_set_splitpoint_name(state,current_split,NULL);
      if (change_error != SPLT_OK)
        {
          *error = change_error;
        }
    }
  
  free(fname);
}

//the result must be freed
//returns the new_filename_path + filename + extension
char *splt_u_get_fname_with_path_and_extension(splt_state *state,
                                               int *error)
{
  char *output_fname_with_path = NULL;
  char *new_filename_path = splt_t_get_new_filename_path(state);
  int current_split = splt_t_get_current_split(state);
  char *output_fname = splt_t_get_splitpoint_name(state, current_split,
                                                  error);
  int malloc_number = strlen(output_fname)+
    strlen(new_filename_path) + 10;
  int file_format = splt_t_get_file_format(state);
  
  if ((output_fname_with_path = malloc(malloc_number)) != NULL)
    {
      //we put the full output filename (with the path)
      //construct full filename with path
      switch (file_format) 
        {
        case SPLT_MP3_FORMAT:
          if (strcmp(new_filename_path,"") == 0)
            {
              snprintf(output_fname_with_path, malloc_number,
                       "%s"SPLT_MP3EXT,output_fname);
            }
          else
            {
              snprintf(output_fname_with_path, malloc_number,
                       "%s%c%s"SPLT_MP3EXT,new_filename_path,
                       SPLT_DIRCHAR, output_fname);
            }
          break;
        case SPLT_OGG_FORMAT:
          if (strcmp(new_filename_path,"") == 0)
            {
              snprintf(output_fname_with_path, malloc_number,
                       "%s"SPLT_OGGEXT,output_fname);
            }
          else
            {
              snprintf(output_fname_with_path, malloc_number,
                       "%s%c%s"SPLT_OGGEXT,new_filename_path,
                       SPLT_DIRCHAR, output_fname);
            }
          break;
        default:
          *error = SPLT_ERROR_INVALID_FORMAT;
          break;
        }
    }
  else
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      return NULL;
    }
  
  char *filename = splt_t_get_filename_to_split(state);
  
  //if the output file exists
  if (splt_check_is_file(output_fname_with_path))
    {
      //if input and output are the same file
      if (splt_check_is_the_same_file(filename,output_fname_with_path,
                                      error))
        {
          *error = SPLT_ERROR_INPUT_OUTPUT_SAME_FILE;
        }
      else
        {
          //if no error from the check_is_the_same..
          if (*error >= 0)
            {
              //TODO
              //warning that a file already exists
            }
        }
    }
  
  return output_fname_with_path;
}

/****************************/
/* utils for splitpoints */

//cuts the extension of a splitpoint
int splt_u_cut_splitpoint_extension(splt_state *state, int index)
{
  int change_error = SPLT_OK;
  
  if (splt_t_splitpoint_exists(state,index))
    {
      int get_error = SPLT_OK;
      char *temp_name =
        splt_t_get_splitpoint_name(state,index,&get_error);
      
      if (get_error != SPLT_OK)
        {
          return get_error;
        }
      else
        {
          char *new_name = strdup(temp_name);
          if (new_name == NULL)
            {
              return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
            }
          else
            {
              new_name[strlen(new_name)-4] = '\0';
              change_error = splt_t_set_splitpoint_name(state,index,
                                                        new_name);
              free(new_name);
            }
        }
    }
  
  return change_error;
}

//order the splitpoints (used in the silence split)
//only works for the values, the names are not ordered!
void splt_u_order_splitpoints(splt_state *state, int len)
{
  long temp = 0;
  
  int err = SPLT_OK;
  
  int i, j;
  float key;
  for (j=1; j < len; j++)
    {
      key = splt_t_get_splitpoint_value(state,j,&err);
      i = j -1;
      while ((i >= 0) && 
             (splt_t_get_splitpoint_value(state,i,&err) > key))
        {
          temp = splt_t_get_splitpoint_value(state,i,&err);
          splt_t_set_splitpoint_value(state,i+1,temp);
          i--;
        }
      splt_t_set_splitpoint_value(state,i+1,key);
    }
}

/****************************/
/* utils for the tags       */

//parse the word, returns a allocated string with the recognised word
char *splt_u_parse_tag_word(char *cur_pos, char *end_paranthesis,
                            int *ambigous)
{
  char *word = NULL;
  char *word_end = NULL;
  char *word_end2 = NULL;
  char *equal_sign = NULL;
  if ((word_end = strchr(cur_pos,',')))
    {
      if ((word_end2 = strchr(cur_pos,']')) < word_end)
        {
          word_end = word_end2;
          if ((strchr(word_end+1,']') && !strchr(word_end+1,'['))
              || (strchr(word_end+1,']') < strchr(word_end+1,'[')))
            {
              *ambigous = SPLT_TRUE;
            }
        }
      
      if (*word_end == ',')
        {
          if (*(word_end+1) != '@')
            {
              *ambigous = SPLT_TRUE;
            }
        }
    }
  else
    {
      word_end = strchr(cur_pos,']');
    }
  
  if (word_end <= end_paranthesis)
    {
      if (*(cur_pos+1) == '=')
        {
          equal_sign = cur_pos+1;
          int string_length = word_end-(equal_sign+1);
          if (string_length > 0)
            {
              word = malloc(string_length*sizeof(char));
              memcpy(word,equal_sign+1,string_length);
              word[string_length] = '\0';
            }
          else
            {
              *ambigous = SPLT_TRUE;
            }
        }
      else
        {
          *ambigous = SPLT_TRUE;
        }
    }
  
  cur_pos = word_end;
  
  return word;
}

//we put the custom tags
//returns if ambigous or not
int splt_u_put_tags_from_string(splt_state *state, char *tags)
{
  if (tags != NULL)
    {
      char *cur_pos = NULL;
      int all_tags = SPLT_FALSE;

      cur_pos = tags;

      //if we set the tags for all the files
      if (*(cur_pos) == '%')
        {
          splt_t_set_int_option(state,SPLT_OPT_ALL_TAGS_LIKE_FIRST_ONE,
                                SPLT_TRUE);
          cur_pos++;
          all_tags = SPLT_TRUE;
        }

      int ambigous = SPLT_FALSE;
      char *end_paranthesis = NULL;
      //we search for tags
      if (!strchr(cur_pos,'['))
        {
          ambigous = SPLT_TRUE;
        }

      while((cur_pos = strchr(cur_pos,'[')))
        {
          char *title = NULL;
          char *artist = NULL;
          char *album = NULL;
          char *performer = NULL;
          char *year = NULL;
          char *comment = NULL;
          char *tracknumber = NULL;
          //means "other"
          unsigned char genre = 12;

          //how many we have found in one [..]  
          short s_title = 0;
          short s_artist = 0;
          short s_album = 0;
          short s_performer = 0;
          short s_year = 0;
          short s_comment = 0;
          short s_tracknumber = 0;

          cur_pos++;

          end_paranthesis = strchr(cur_pos,']');
          if (!end_paranthesis)
            {
              ambigous = SPLT_TRUE;
            }
          else
            {
              if ((*(end_paranthesis+1) != '[')
                  && (*(end_paranthesis+1) != '\0'))
                {
                  ambigous = SPLT_TRUE;
                }
            }

          char *tag = NULL;
          while((tag = strchr(cur_pos-1,'@')))
            {
              //if the current position is superior or equal
              if (tag >= end_paranthesis)
                {
                  break;
                }
              else
                {
                  cur_pos = tag+1;
                }

              char *old_pos = cur_pos;
              //we take the artist, performer,...
              if (*(cur_pos-1) == '@')
                {
                  switch (*cur_pos)
                    {
                    case 'a':
                      artist = 
                        splt_u_parse_tag_word(cur_pos,end_paranthesis,&ambigous);
                      if (artist != NULL)
                        {
                          cur_pos += strlen(artist)+2;
                          s_artist++;
                        }
                      else
                        {
                          cur_pos++;
                        }
                      break;
                    case 'p':
                      performer = 
                        splt_u_parse_tag_word(cur_pos,end_paranthesis,&ambigous);
                      if (performer != NULL)
                        {
                          cur_pos += strlen(performer)+2;
                          s_performer++;
                        }
                      else
                        {
                          cur_pos++;
                        }
                      break;
                    case 'b':
                      album =
                        splt_u_parse_tag_word(cur_pos,end_paranthesis,&ambigous);
                      if (album != NULL)
                        {
                          cur_pos += strlen(album)+2;
                          s_album++;
                        }
                      else
                        {
                          cur_pos++;
                        }
                      break;
                    case 't':
                      title =
                        splt_u_parse_tag_word(cur_pos,end_paranthesis,&ambigous);
                      if (title != NULL)
                        {
                          cur_pos += strlen(title)+2;
                          s_title++;
                        }
                      else
                        {
                          cur_pos++;
                        }
                      break;
                    case 'c':
                      comment =
                        splt_u_parse_tag_word(cur_pos,end_paranthesis,&ambigous);
                      if (comment != NULL)
                        {
                          cur_pos += strlen(comment)+2;
                          s_comment++;
                        }
                      else
                        {
                          cur_pos++;
                        }
                      break;
                    case 'y':
                      year =
                        splt_u_parse_tag_word(cur_pos,end_paranthesis,&ambigous);
                      if (year != NULL)
                        {
                          cur_pos += strlen(year)+2;
                          s_year++;
                        }
                      else
                        {
                          cur_pos++;
                        }
                      break;
                    case 'n':
                      tracknumber = 
                        splt_u_parse_tag_word(cur_pos,end_paranthesis,&ambigous);
                      if (tracknumber != NULL)
                        {
                          cur_pos += strlen(tracknumber)+2;
                          s_tracknumber++;
                        }
                      else
                        {
                          cur_pos++;
                        }
                      break;
                    default:
                      ambigous = SPLT_TRUE;
                      break;
                    }
                }

              if (cur_pos <= old_pos)
                {
                  cur_pos++;
                }
            }

          int track = 0;
          //we check that we really have the tracknumber as integer
          if (tracknumber)
            {
              int is_number = SPLT_TRUE;
              int i = 0;
              for (i = 0;i < strlen(tracknumber);i++)
                {
                  if (!isdigit(tracknumber[i]))
                    {
                      is_number = SPLT_FALSE;
                      ambigous = SPLT_TRUE;
                    }
                }
              if (is_number)
                {
                  track = atoi(tracknumber);
                }
            }

          if ((s_title > 1) || (s_artist > 1)
              || (s_album > 1) || (s_performer > 1)
              || (s_year > 1) || (s_comment > 1)
              || (s_tracknumber > 1))
            {
              ambigous = SPLT_TRUE;
            }

          //we put the tags
          splt_t_append_tags(state,title,artist,
                             album, performer, year, comment,
                             track, genre);

          //we free the memory
          if (title)
            {
              free(title);
            }
          if (artist)
            {
              free(artist);
            }
          if (album)
            {
              free(album);
            }
          if (performer)
            {
              free(performer);
            }
          if (year)
            {
              free(year);
            }
          if (comment)
            {
              free(comment);
            }
          if (tracknumber)
            {
              free(tracknumber);
            }

          //if we put all tags, we break
          if (all_tags)
            {
              break;
            }
        }

      return ambigous;
    }

  return SPLT_FALSE;
}

/*******************************/
/* utils for the output format */

//parse the output format to see if correct
//use strdup on s when calling this function
int splt_u_parse_outformat(char *s, splt_state *state)
{
  char *ptrs, *ptre;
  int i=0, amb=SPLT_OUTPUT_FORMAT_AMBIGUOUS, len=0;

  for (i=0; i<strlen(s); i++)
    {
      if (s[i]=='+') 
        {
          s[i]=' ';
        }
      else 
        {
          if (s[i]==SPLT_VARCHAR) 
            {
              s[i]='%';
            }
        }
    }
  
  splt_u_cleanstring(s);
  ptrs = s;
  i=0;
  ptre=strchr(ptrs+1, '%');
  if (s[0]!='%')
    {
      if (ptre==NULL)
        {
          len=strlen(ptrs);
        }
      else
        {
          len = ptre-ptrs;
        }
      if (len > SPLT_MAXOLEN)
        {
          len = SPLT_MAXOLEN;
        }
      strncpy(state->oformat.format[i++], ptrs, len);
    }
  else
    {
      ptre=s;
    }
  
  if (ptre==NULL)
    {
      return SPLT_OUTPUT_FORMAT_AMBIGUOUS;
    }
  ptrs = ptre;
  
  while (((ptre=strchr(ptrs+1, '%'))!=NULL) && 
         (i < SPLT_OUTNUM))
    {
      char cf = *(ptrs+1);
      
      len = ptre-ptrs;
      if (len > SPLT_MAXOLEN)
        {
          len = SPLT_MAXOLEN;
        }
      
      switch (cf)
        {
        case 'a':
          break;
        case 'b':
          break;
        case 't':
          break;
        case 'n':
          amb = SPLT_OUTPUT_FORMAT_OK;
          break;
        case 'f':
          break;
        case 'p':
          break;
        default:
          return SPLT_OUTPUT_FORMAT_ERROR;
        }
      
      strncpy(state->oformat.format[i++], ptrs, len);
      ptrs = ptre;
    }
  
  strncpy(state->oformat.format[i], ptrs, strlen(ptrs));
  
  if (ptrs[1]=='t')
    amb = SPLT_OUTPUT_FORMAT_OK;
  
  if (ptrs[1]=='n')
    amb = SPLT_OUTPUT_FORMAT_OK;
  
  return amb;
}

//writes the current filename according to the output_filename
int splt_u_put_output_format_filename(splt_state *state)
{
  int error = SPLT_OK;
  
  char *temp = NULL;
  char *fm = NULL;
  int i = 0;
  char *output_filename = NULL;
  int output_filename_size = 0;
  
  char *title = NULL;
  char *artist = NULL;
  char *album = NULL;
  char *performer = NULL;
  char *original_filename = NULL;
  
  int old_current_split =
    splt_t_get_current_split(state);
  
  int current_split = old_current_split;
  
  int fm_length = 0;
  
  //if we get the tags from the first file
  if (splt_t_get_int_option(state,SPLT_OPT_ALL_TAGS_LIKE_FIRST_ONE)
      == SPLT_TRUE)
    {
      current_split = 0;
    }
  
  splt_u_print_debug("The output format is ",0,state->oformat.format_string);
  
  for (i=0; i<SPLT_OUTNUM; i++)
    {
      if (strlen(state->oformat.format[i])==0)
        {
          break;
        }
      //if we have some % in the format (@ has been converted to %)
      if (state->oformat.format[i][0]=='%')
        {
          //we allocate memory for the temp variable
          if (temp)
            {
              free(temp);
              temp = NULL;
            }
          
          int temp_len = strlen(state->oformat.format[i])+10;
          if ((temp = malloc(temp_len * sizeof(char))) == NULL)
            {
              error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
              goto end;
            }
          memset(temp, 0x0, temp_len);
          
          temp[0]='%';
          temp[1]='s';
          switch (state->oformat.format[i][1])
            {
            case 'a':
              if (splt_t_tags_exists(state,current_split))
                {
                  //we get the artist
                  artist =
                    splt_t_get_tags_char_field(state,current_split,
                                               SPLT_TAGS_ARTIST);
                }
              else
                {
                  artist = NULL;
                }
              
              if (artist != NULL)
                {
                  snprintf(temp+2,temp_len, state->oformat.format[i]+2);
                  
                  int artist_length = 0;
                  artist_length = strlen(artist);
                  fm_length = strlen(temp)+artist_length;
                  if ((fm = malloc(fm_length
                                   * sizeof(char))) == NULL)
                    {
                      error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
                      goto end;
                    }
                  
                  snprintf(fm, fm_length, temp, artist);
                }
              break;
            case 'b':
              if (splt_t_tags_exists(state,current_split))
                {
                  //we get the album
                  album =
                    splt_t_get_tags_char_field(state,current_split, SPLT_TAGS_ALBUM);
                }
              else
                {
                  album = NULL;
                }
              
              if (album != NULL)
                {
                  int album_length = 0;
                  album_length = strlen(album);
                  snprintf(temp+2, temp_len, state->oformat.format[i]+2);
                  
                  fm_length = strlen(temp)+album_length;
                  if ((fm = malloc(fm_length
                                   * sizeof(char))) == NULL)
                    {
                      error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
                      goto end;
                    }
                  
                  snprintf(fm, fm_length, temp, album);
                }               
              break;
            case 't':
              if (splt_t_tags_exists(state,current_split))
                {
                  //we get the title
                  title =
                    splt_t_get_tags_char_field(state,current_split,
                                               SPLT_TAGS_TITLE);
                }
              else
                {
                  title = NULL;
                }
              
              if (title != NULL)
                {
                  int title_length = 0;
                  title_length = strlen(title);
                  snprintf(temp+2, temp_len, state->oformat.format[i]+2);

                  fm_length = strlen(temp)+title_length;
                  if ((fm = malloc(fm_length
                                   * sizeof(char))) == NULL)
                    {
                      error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
                      goto end;
                    }
                  
                  snprintf(fm, fm_length, temp, title);
                }
              break;
            case 'p':
              if (splt_t_tags_exists(state,current_split))
                {
                  //we get the performer
                  performer =
                    splt_t_get_tags_char_field(state,current_split,
                                               SPLT_TAGS_PERFORMER);
                }
              else
                {
                  performer = NULL;
                }
              
              if (performer != NULL)
                {
                  int performer_length = 0;
                  performer_length = strlen(performer);
                  snprintf(temp+2, temp_len, state->oformat.format[i]+2);

                  fm_length = strlen(temp)+performer_length;
                  if ((fm = malloc(fm_length
                                   * sizeof(char))) == NULL)
                    {
                      error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
                      goto end;
                    }
              
                  snprintf(fm, fm_length, temp, performer);
                }
              break;
            case 'n':
              temp[1]='0';
              temp[2]=state->oformat.output_format_digits;
              temp[3]='d';
              
              //we set the track number
              int tracknumber = old_current_split+1;
              
              //if not time split, or normal split, or silence split or error,
              //we put the track number from the tags
              int split_mode = splt_t_get_int_option(state,SPLT_OPT_SPLIT_MODE);
              if ((split_mode != SPLT_OPTION_TIME_MODE) &&
                  (split_mode != SPLT_OPTION_NORMAL_MODE) &&
                  (split_mode != SPLT_OPTION_SILENCE_MODE) &&
                  (split_mode != SPLT_OPTION_MP3_ERROR_MODE))
                {
                  if (splt_t_tags_exists(state,current_split))
                    {
                      int tags_track = 
                        splt_t_get_tags_int_field(state,
                                                  current_split,SPLT_TAGS_TRACK);
                      if (tags_track > 0)
                        {
                          tracknumber = tags_track;
                        }
                    }
                }
              snprintf(temp+4, temp_len, state->oformat.format[i]+2);
              
              fm_length = strlen(temp);
              if ((fm = malloc(fm_length * sizeof(char)))
                  == NULL)
                {
                  error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
                  goto end;
                }
              
              snprintf(fm, fm_length, temp, tracknumber);
              break;
            case 'f':
              if (splt_t_get_filename_to_split(state) != NULL)
                {
                  //we get the filename
                  original_filename = strdup(splt_u_get_real_name(splt_t_get_filename_to_split(state)));
                  snprintf(temp+2,temp_len, state->oformat.format[i]+2);
                  
                  //we cut extension
                  original_filename[strlen(original_filename)-4] = '\0';
                  
                  int filename_length = strlen(original_filename);
                  
                  fm_length = strlen(temp) + filename_length;
                  if ((fm = malloc(fm_length
                                   * sizeof(char))) == NULL)
                    {
                      error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
                      goto end;
                    }
                  
                  snprintf(fm, fm_length, temp, original_filename);
                  free(original_filename);
                  original_filename = NULL;
                }
              break;
            }
        }
      else
        {
          fm_length = SPLT_MAXOLEN;
          if ((fm = malloc(fm_length * sizeof(char)))
              == NULL)
            {
              error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
              goto end;
            }
          
          strncpy(fm, state->oformat.format[i], SPLT_MAXOLEN);
        }
      
      int fm_size = 7;
      if (fm != NULL)
        {
          fm_size = strlen(fm);
        }
      
      //allocate memory for the output filename
      if (!output_filename)
        {
          if ((output_filename = malloc((1+fm_size)*sizeof(char)))
              == NULL)
            {
              error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
              goto end;
            }
          output_filename_size = fm_size;
          output_filename[0] = '\0';
        }
      else
        {
          output_filename_size += fm_size+1;
          if ((output_filename = realloc(output_filename,
                                         output_filename_size
                                         * sizeof(char)))
              == NULL)
            {
              error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
              goto end;
            }
        }
      
      if (fm != NULL)
        {
          strcat(output_filename, fm);
        }
      
      //we free fm
      if (fm)
        {
          free(fm);
          fm = NULL;
        }
    }
  
  //we change the splitpoint name
  int name_error = SPLT_OK;
  int cur_splt = splt_t_get_current_split(state);
  
  splt_u_print_debug("The new output filename is ",0,output_filename);
  
  name_error = 
    splt_t_set_splitpoint_name(state,
                               cur_splt,
                               output_filename);
  
  if (name_error != SPLT_OK)
    {
      error = name_error;
    }
  
 end:
  //free memory
  if (output_filename)
    {
      free(output_filename);
      output_filename = NULL;
    }
  if (fm)
    {
      free(fm);
      fm = NULL;
    }
  if (temp)
    {
      free(temp);
      temp = NULL;
    }
  
  return error;
}

/****************************/
/* debug errors */

//prints an error message
//if such error messages appear, there are coding errors
void splt_u_error(int error_type, const char *function,
                int arg_int, char *arg_char)
{
  switch (error_type)
    {
    case SPLT_IERROR_INT:
      fprintf(stderr,
              "libmp3splt: error in %s with value %d\n",
              function, arg_int);
      break;
    case SPLT_IERROR_SET_ORIGINAL_TAGS:
      fprintf(stderr,
              "libmp3splt: cannot set original file tags, "
              "libmp3splt not compiled with libid3tag\n");
      break;
    default:
      fprintf(stderr,
              "libmp3splt: unknown error in %s\n", function);
      break;
    }
}

/****************************/
/* utils miscellaneous */

//get silence position depending of the offset
float splt_u_silence_position(struct splt_ssplit *temp, float off)
{
  float position = (temp->end_position - 
                    temp->begin_position);

  position = temp->begin_position + (position*off);

  return position;
}

//debug messages
void splt_u_print_debug(char *message,double optional,
                        char *optional2)
{
  if (global_debug)
    {
      if (optional != 0)
        {
          if (optional2 != NULL)
            {
              fprintf(stdout,"%s %f _%s_\n",message,optional,
                      optional2);
            }
          else
            {
              fprintf(stdout,"%s %f\n",message,optional);
            }
        }
      else
        {
          if (optional2 != NULL)
            {
              fprintf(stdout,"%s _%s_\n",message, optional2);
            }
          else
            {
              fprintf(stdout,"%s\n",message);
            }
        }
      fflush(stdout);
    }
}

//convert to float for hundredth
// 3460 -> 34.6  | 34 seconds and 6 hundredth
double splt_u_get_double_pos(long split)
{
  double pos;
  pos = split / 100;
  pos += ((split % 100) / 100.);

  return pos;
}

//create recursive directories
int splt_u_create_directory(char *dir)
{
  int result = SPLT_OK;
  char *ptr = NULL;
  char *junk = malloc(sizeof(char) * (strlen(dir)+100));
  DIR *d;
  
  splt_u_print_debug("Creating directory ...",0,dir);
  
  ptr = dir;
  while ((ptr = strchr(ptr, SPLT_DIRCHAR))!=NULL)
    {
      ptr++;
      strncpy(junk, dir, ptr-dir);
      junk[ptr-dir] = '\0';
      
      splt_u_print_debug("directory ...",0, junk);
      
      if (!(d = opendir(junk)))
        {
#ifdef _WIN32                                
          if ((mkdir(junk))==-1)
            {
#else
          if ((mkdir(junk, 0755))==-1)
            {
#endif
              result = SPLT_ERROR_CANNOT_CREATE_DIRECTORY;
              break;
            }
        }
      else
        {
          closedir(d);
        }
    }
      
  //we have created all the directories except the last one
  if (!(d = opendir(dir)))
    {
      splt_u_print_debug("final directory ...",0, dir);

#ifdef _WIN32
      if ((mkdir(dir))==-1)
        {
 #else
      if ((mkdir(dir, 0755))==-1)
        {
#endif
          result = SPLT_ERROR_CANNOT_CREATE_DIRECTORY;
        }
    }
  else
    {
      closedir(d);
    }
  
  free(junk);
  
  return result;
}
