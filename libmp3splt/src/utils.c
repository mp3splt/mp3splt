/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2009 Alexandru Munteanu - io_fx@yahoo.fr
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

#include "splt.h"

#include <dirent.h>

#ifdef __WIN32__
#include <windows.h>
#include <direct.h>
#endif

extern short global_debug;

/****************************/
/* some prototypes */

static void splt_u_cut_extension(char *str);

/****************************/
/* utils for conversion */

//converts string s in hundredth
//and returns seconds
//returns -1 if it cannot convert (usually we hope it can:)
long splt_u_convert_hundreths (const char *s)
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
  if (input <= 0.0)
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
off_t splt_u_flength(splt_state *state, FILE *in, const char *filename, int *error)
{
  struct stat info;
  if (fstat(fileno(in), &info)==-1)
  {
    splt_t_set_strerror_msg(state);
    splt_t_set_error_data(state, filename);
    *error = SPLT_ERROR_CANNOT_OPEN_FILE;
    return -1;
  }
  return info.st_size;
}

/*****************************************************/
/* utils manipulating strings (including filenames) */

int splt_u_is_illegal_char(char c, int ignore_dirchar)
{
/*#ifdef __WIN32__
  char windows_illegal_characters[] =
  { '\\', '/', ':', '*', '?', '"', "<", ">", "|", '\r' };
#elif defined(__MACOS__)
  char mac_os_illegal_characters[] = { ':' };
#elif defined(__MACOSX__)
  char mac_osx_illegal_characters[] = { ':' };
#else
  char _nix_illegal_characters[] = { '/' };
#endif*/

  if ((ignore_dirchar) && (c == SPLT_DIRCHAR))
  {
    return SPLT_FALSE;
  }

  //for the sake of filename portability, we take the the windows illegal
  //characters (will be changed upon feature request)
  if ((c == '\\') || (c == '/') || (c == ':') || (c == '*') ||
      (c == '?') || (c == '"') || (c == '<') ||
      (c == '>') || (c == '|') || (c == '\r'))
  {
    return SPLT_TRUE;
  }

  return SPLT_FALSE;
}

//cleans the string of weird characters like ? " | : > < * \ \r
void splt_u_cleanstring_(splt_state *state, char *s, int *error, int ignore_dirchar)
{
  int i = 0, j=0;
  char *copy = NULL;
  if (s)
  {
    copy = strdup(s);
    if (copy)
    {
      for (i=0; i<=strlen(copy); i++)
      {
        if (! splt_u_is_illegal_char(copy[i], ignore_dirchar))
        {
          s[j++] = copy[i];
        }
        else
        {
          s[j++] = '_';
        }
      }
      free(copy);
      copy = NULL;

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
    }
    else
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    }
  }
}

void splt_u_cleanstring(splt_state *state, char *s, int *error)
{
  splt_u_cleanstring_(state, s, error, SPLT_FALSE);
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
const char *splt_u_get_real_name(const char *filename)
{
  char *c = NULL;
  while ((c = strchr(filename, SPLT_DIRCHAR)) !=NULL)
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
static char *splt_u_get_mins_secs_filename(const char *filename, splt_state *state,
    long split_begin, long split_end, int i, int *error)
{
  int number_of_splits = 0;
  splt_point *points = 
    splt_t_get_splitpoints(state, &number_of_splits);

  char *fname = NULL, *fname2 = NULL;
  int fname2_malloc_number = 0,fname_malloc_number = 0;

  fname2_malloc_number = fname_malloc_number = strlen(filename) + 256;

  long old_split_end = split_end;

  if((fname = malloc(fname_malloc_number*sizeof(char))) != NULL)
  {
    memset(fname,'\0',fname_malloc_number*sizeof(char));
    if((fname2 = malloc(fname2_malloc_number*sizeof(char))) != NULL)
    {
      memset(fname2,'\0',fname2_malloc_number*sizeof(char));
      long hundr = 0, secs = 0, mins = 0;
      long hundr2 = 0, secs2 = 0, mins2 = 0;
      splt_t_get_mins_secs_hundr_from_splitpoint(split_begin, &mins, &secs, &hundr);
      splt_t_get_mins_secs_hundr_from_splitpoint(split_end, &mins2, &secs2, &hundr2);

      //if we have this splitpoint
      if (splt_t_splitpoint_exists(state, i))
      {
        if (points[i].name != NULL)
        {
          char temp[3] = { '\0' };
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
        if ((points[i].name == NULL) || (strcmp(points[i].name,"") == 0))
        {
          snprintf(fname,strlen(filename),"%s", filename);
          splt_u_cut_extension(fname);
        }
      }
      else
      {
        splt_u_error(SPLT_IERROR_INT,__func__, i, NULL);
      }

      //we put EOF if LONG_MAX
      if (old_split_end == LONG_MAX)
      {
        snprintf(fname2,fname2_malloc_number,
            "%s_%ldm_%lds_%ldh__EOF", 
            fname, mins, secs, hundr);
      }
      else
      {
        snprintf(fname2,fname2_malloc_number,
            "%s_%ldm_%lds_%ldh__%ldm_%lds_%ldh", 
            fname, mins, secs, hundr, mins2, secs2, hundr2);
      }

      //put the extension according to the file type
      const char *extension = splt_p_get_extension(state, error);
      if (*error >= 0)
      {
        strcat(fname2, extension);
      }
    }
    else
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    }
    //freeing memory
    if (fname)
    {
      free(fname);
      fname = NULL;
    }
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
  char *filename = splt_t_get_filename_to_split(state);
  if (filename == NULL)
  {
    return;
  }

  int get_error = SPLT_OK;
  int current_split = splt_t_get_current_split(state);
  long split_begin = splt_t_get_splitpoint_value(state, current_split, &get_error);
  if (get_error < 0) { *error = get_error; return; }
  long split_end = splt_t_get_splitpoint_value(state, current_split+1, &get_error);
  if (get_error < 0) { *error = get_error; return; }

  char *filename2 = strdup(filename);
  if (!filename2)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return;
  }
  else
  {
    //filename of the new created file
    char *fname = NULL;

    //get the filename without the path
    const char *ptr = splt_u_get_real_name(filename2);
    if (ptr)
    {
      fname = splt_u_get_mins_secs_filename(ptr, state,
          split_begin, split_end, current_split, error);

      if (*error >= 0)
      {
        splt_u_cut_extension(fname);
        splt_t_set_splitpoint_name(state, current_split, fname);
      }

      if (fname)
      {
        free(fname);
        fname = NULL;
      }
    }

    //free some memory
    if (filename2)
    {
      free(filename2);
      filename2 = NULL;
    }
  }
}

//the result must be freed
//returns the new_filename_path + filename + extension or NULL if error
char *splt_u_get_fname_with_path_and_extension(splt_state *state, int *error)
{
  char *output_fname_with_path = NULL;
  char *new_filename_path = splt_t_get_new_filename_path(state);
  int current_split = splt_t_get_current_split(state);
  char *output_fname = splt_t_get_splitpoint_name(state, current_split, error);
  int malloc_number = strlen(new_filename_path) + 10;
  if (output_fname)
  {
    malloc_number += strlen(output_fname);
  }

  //if we don't output to stdout
  if (output_fname && (strcmp(output_fname,"-") != 0))
  {
    if ((output_fname_with_path = malloc(malloc_number)) != NULL)
    {
      //we put the full output filename (with the path)
      //construct full filename with path
      const char *extension = splt_p_get_extension(state, error);
      if (*error >= 0)
      {
        if (new_filename_path[0] == '\0')
        {
          snprintf(output_fname_with_path, malloc_number,
              "%s%s", output_fname, extension);
        }
        else
        {
          snprintf(output_fname_with_path, malloc_number,
              "%s%c%s%s",new_filename_path, SPLT_DIRCHAR,
              output_fname, extension);
        }
      }
      else
      {
        if (output_fname_with_path)
        {
          free(output_fname_with_path);
          output_fname_with_path = NULL;
        }
        return NULL;
      }
    }
    else
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      return NULL;
    }

    char *filename = splt_t_get_filename_to_split(state);

    //if the output file exists
    int is_file = splt_check_is_file(state, output_fname_with_path);
    if (is_file)
    {
      //if input and output are the same file
      if (splt_check_is_the_same_file(state,filename, output_fname_with_path, error))
      {
        splt_t_set_error_data(state,filename);
        *error = SPLT_ERROR_INPUT_OUTPUT_SAME_FILE;
      }
      else
      {
        //if no error from the check_is_the_same..
        if (*error >= 0)
        {
          //TODO
          //warning if a file already exists
        }
      }
    }

    return output_fname_with_path;
  }
  else
  {
    char *returned_result = NULL;
    if (output_fname)
    {
      returned_result = strdup(output_fname);
    }
    else
    {
      returned_result = strdup("-");
    }
    if (returned_result)
    {
      return returned_result;
    }
    else
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      return NULL;
    }
  }
}

/****************************/
/* utils for splitpoints */

//cut extension from 'str'
static void splt_u_cut_extension(char *str)
{
  char *point = strrchr(str , '.');
  if (point)
  {
    *point = '\0';
  }
}

//cuts the extension of a splitpoint
int splt_u_cut_splitpoint_extension(splt_state *state, int index)
{
  int change_error = SPLT_OK;

  if (splt_t_splitpoint_exists(state,index))
  {
    int get_error = SPLT_OK;
    char *temp_name = splt_t_get_splitpoint_name(state, index, &get_error);

    if (get_error != SPLT_OK)
    {
      return get_error;
    }
    else
    {
      if (temp_name)
      {
        char *new_name = strdup(temp_name);
        if (new_name == NULL)
        {
          return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
        }
        else
        {
          splt_u_cut_extension(new_name);
          change_error = splt_t_set_splitpoint_name(state,index, new_name);
          free(new_name);
          new_name = NULL;
        }
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
//-return must be freed
static char *splt_u_parse_tag_word(const char *cur_pos,
    const char *end_paranthesis, int *ambiguous, int *error)
{
  char *word = NULL;
  char *word_end = NULL;
  char *word_end2 = NULL;
  const char *equal_sign = NULL;

  if ((word_end = strchr(cur_pos,',')))
  {
    if ((word_end2 = strchr(cur_pos,']')) < word_end)
    {
      word_end = word_end2;
      if ((strchr(word_end+1,']') && !strchr(word_end+1,'['))
          || (strchr(word_end+1,']') < strchr(word_end+1,'[')))
      {
        *ambiguous = SPLT_TRUE;
      }
    }

    if (*word_end == ',')
    {
      if (*(word_end+1) != '@')
      {
        *ambiguous = SPLT_TRUE;
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
        word = malloc((string_length+1)*sizeof(char));
        memset(word,'\0',(string_length+1)*sizeof(char));
        if (word)
        {
          memcpy(word,equal_sign+1,string_length);
          word[string_length] = '\0';
        }
        else
        {
          *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
          return NULL;
        }
      }
      else
      {
        *ambiguous = SPLT_TRUE;
      }
    }
    else
    {
      *ambiguous = SPLT_TRUE;
    }
  }

  cur_pos = word_end;

  return word;
}

//we put the custom tags
//returns if ambiguous or not
int splt_u_put_tags_from_string(splt_state *state, const char *tags, int *error)
{
  if (tags != NULL)
  {
    const char *cur_pos = NULL;
    int all_tags = SPLT_FALSE;
    int we_had_all_tags = SPLT_FALSE;

    cur_pos = tags;

    int ambiguous = SPLT_FALSE;
    const char *end_paranthesis = NULL;
    //we search for tags
    if (!strchr(cur_pos,'['))
    {
      ambiguous = SPLT_TRUE;
    }

    //tags that we put to all the others: %[@t=tag1,@b=album1]
    char *all_artist = NULL;
    char *all_album = NULL;
    char *all_title = NULL;
    char *all_performer = NULL;
    char *all_year = NULL;
    char *all_comment = NULL;
    int all_tracknumber = -1;
    int auto_incremented_tracknumber = -1;
    unsigned char all_genre = 12;
    //when using 'N', auto increment the track number
    short auto_increment_tracknumber = SPLT_FALSE;
    short first_time_auto_increment_tracknumber = SPLT_TRUE;

    int tags_appended = 0;
    int get_out_from_while = SPLT_FALSE;

    while ((cur_pos = strchr(cur_pos,'[')))
    {
      //if we set the tags for all the files
      if (cur_pos != tags)
      {
        //if we have % before [
        if (*(cur_pos-1) == '%')
        {
          splt_t_set_int_option(state,SPLT_OPT_ALL_REMAINING_TAGS_LIKE_X, tags_appended);
          all_tags = SPLT_TRUE;
          //if we had all tags, remove them
          if (we_had_all_tags)
          {
            if (all_title) { free(all_title); all_title = NULL; }
            if (all_artist) { free(all_artist); all_artist = NULL; }
            if (all_album) { free(all_album); all_album = NULL; }
            if (all_performer) { free(all_performer); all_performer = NULL; }
            if (all_year) { free(all_year); all_year = NULL; }
            if (all_comment) { free(all_comment); all_comment = NULL; }
          }
        }
      }

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
        ambiguous = SPLT_TRUE;
      }
      else
      {
        if ((*(end_paranthesis+1) != '[') &&
            (*(end_paranthesis+1) != '%') &&
            (*(end_paranthesis+1) != '\0'))
        {
          ambiguous = SPLT_TRUE;
        }
      }

      char *tag = NULL;
      int original_tags = SPLT_FALSE;
      while ((tag = strchr(cur_pos-1,'@')))
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

        const char *old_pos = cur_pos;
        //we take the artist, performer,...
        if (*(cur_pos-1) == '@')
        {
          switch (*cur_pos)
          {
            case 'o':
              //if we have twice @o
              if (original_tags)
              {
                ambiguous = SPLT_TRUE;
              }
              //if we have other thing than @o, or @o]
              //then ambiguous
              if ((*(cur_pos+1) != ',') &&
                  (*(cur_pos+1) != ']'))
              {
                ambiguous = SPLT_TRUE;
              }

              //if we don't have STDIN
              if (! splt_t_is_stdin(state))
              {
                int err = SPLT_OK;
                splt_t_lock_messages(state);
                splt_check_file_type(state, &err);
                if (err < 0)
                {
                  *error = err;
                  splt_t_unlock_messages(state);
                  get_out_from_while = SPLT_TRUE;
                  goto end_while;
                }
                splt_t_unlock_messages(state);

                splt_t_lock_messages(state);
                splt_p_init(state, &err);
                if (err >= 0)
                {
                  splt_t_get_original_tags(state, &err);
                  if (err < 0) 
                  {
                    *error = err;
                    splt_t_unlock_messages(state);
                    get_out_from_while = SPLT_TRUE;
                    goto end_while;
                  }
                  splt_p_end(state, &err);
                  err = splt_t_append_original_tags(state);
                  if (err < 0)
                  {
                    *error = err;
                    splt_t_unlock_messages(state);
                    get_out_from_while = SPLT_TRUE;
                    goto end_while;
                  }
                  //if we have all_tags, get out the last tags
                  if (all_tags)
                  {
                    //and copy them
                    splt_tags last_tags = splt_t_get_last_tags(state);
                    //free previous all tags
                    if (all_title) { free(all_title); all_title = NULL; }
                    if (all_artist) { free(all_artist); all_artist = NULL; }
                    if (all_album) { free(all_album); all_album = NULL; }
                    if (all_performer) { free(all_performer); all_performer = NULL; }
                    if (all_year) { free(all_year); all_year = NULL; }
                    if (all_comment) { free(all_comment); all_comment = NULL; }
                    //set new all tags
                    if (last_tags.title != NULL)
                    {
                      all_title = strdup(last_tags.title);
                    }
                    if (last_tags.artist != NULL)
                    {
                      all_artist = strdup(last_tags.artist);
                    }
                    if (last_tags.album != NULL)
                    {
                      all_album = strdup(last_tags.album);
                    }
                    if (last_tags.performer != NULL)
                    {
                      all_performer = strdup(last_tags.performer);
                    }
                    if (last_tags.year != NULL)
                    {
                      all_year = strdup(last_tags.year);
                    }
                    if (last_tags.comment != NULL)
                    {
                      all_comment = strdup(last_tags.comment);
                    }
                    all_genre = last_tags.genre;
                    all_tracknumber = last_tags.track;
                  }
                  original_tags = SPLT_TRUE;
                  splt_t_unlock_messages(state);
                }
                else
                {
                  *error = err;
                  splt_t_unlock_messages(state);
                  get_out_from_while = SPLT_TRUE;
                  goto end_while;
                }
              }

              //if we have a @a,@p,.. before @n
              //then ambiguous
              if ((artist != NULL) || (performer != NULL) ||
                  (album != NULL) || (title != NULL) ||
                  (comment != NULL) || (year != NULL) ||
                  (tracknumber != NULL))
              {
                ambiguous = SPLT_TRUE;
              }
              break;
            case 'a':
              artist = splt_u_parse_tag_word(cur_pos,end_paranthesis,&ambiguous, error);
              if (*error < 0) { get_out_from_while = SPLT_TRUE; goto end_while; }
              if (artist != NULL)
              {
                //copy the artist for all tags
                if (all_tags)
                {
                  if (all_artist) { free(all_artist); all_artist = NULL; }
                  all_artist = strdup(artist);
                }
                cur_pos += strlen(artist)+2;
                s_artist++;
              }
              else
              {
                cur_pos++;
              }
              break;
            case 'p':
              performer = splt_u_parse_tag_word(cur_pos,end_paranthesis,&ambiguous, error);
              if (*error < 0) { get_out_from_while = SPLT_TRUE; goto end_while; }
              if (performer != NULL)
              {
                //copy the performer for all tags
                if (all_tags)
                {
                  if (all_performer) { free(all_performer); all_performer = NULL; }
                  all_performer = strdup(performer);
                }
                cur_pos += strlen(performer)+2;
                s_performer++;
              }
              else
              {
                cur_pos++;
              }
              break;
            case 'b':
              album = splt_u_parse_tag_word(cur_pos,end_paranthesis,&ambiguous,error);
              if (*error < 0) { get_out_from_while = SPLT_TRUE; goto end_while; }
              if (album != NULL)
              {
                //copy the album for all tags
                if (all_tags)
                {
                  if (all_album) { free(all_album); all_album = NULL; }
                  all_album = strdup(album);
                }
                cur_pos += strlen(album)+2;
                s_album++;
              }
              else
              {
                cur_pos++;
              }
              break;
            case 't':
              title = splt_u_parse_tag_word(cur_pos,end_paranthesis,&ambiguous,error);
              if (*error < 0) { get_out_from_while = SPLT_TRUE; goto end_while; }
              if (title != NULL)
              {
                //copy the title for all tags
                if (all_tags)
                {
                  if (all_title) { free(all_title); all_title = NULL; }
                  all_title = strdup(title);
                }

                cur_pos += strlen(title)+2;
                s_title++;
              }
              else
              {
                cur_pos++;
              }
              break;
            case 'c':
              comment = splt_u_parse_tag_word(cur_pos,end_paranthesis,&ambiguous,error);
              if (*error < 0) { get_out_from_while = SPLT_TRUE; goto end_while; }
              if (comment != NULL)
              {
                //copy the comment for all tags
                if (all_tags)
                {
                  if (all_comment) { free(all_comment); all_comment = NULL; }
                  all_comment = strdup(comment);
                }

                cur_pos += strlen(comment)+2;
                s_comment++;
              }
              else
              {
                cur_pos++;
              }
              break;
            case 'y':
              year = splt_u_parse_tag_word(cur_pos,end_paranthesis,&ambiguous,error);
              if (*error < 0) { get_out_from_while = SPLT_TRUE; goto end_while; }
              if (year != NULL)
              {
                //copy the year for all tags
                if (all_tags)
                {
                  if (all_year) { free(all_year); all_year = NULL; }
                  all_year = strdup(year);
                }

                cur_pos += strlen(year)+2;
                s_year++;
              }
              else
              {
                cur_pos++;
              }
              break;
            case 'n':
              tracknumber = splt_u_parse_tag_word(cur_pos,end_paranthesis,&ambiguous,error);
              if (*error < 0) { get_out_from_while = SPLT_TRUE; goto end_while; }
              if (tracknumber != NULL)
              {
                cur_pos += strlen(tracknumber)+2;
                s_tracknumber++;
              }
              else
              {
                cur_pos++;
              }
              if (auto_increment_tracknumber)
              {
                first_time_auto_increment_tracknumber = SPLT_TRUE;
              }
              break;
            case 'N':
              tracknumber = splt_u_parse_tag_word(cur_pos,end_paranthesis, &ambiguous,error);
              if (*error < 0) { get_out_from_while = SPLT_TRUE; goto end_while; }
              splt_t_set_int_option(state,SPLT_OPT_AUTO_INCREMENT_TRACKNUMBER_TAGS, SPLT_TRUE);
              if (auto_increment_tracknumber)
              {
                first_time_auto_increment_tracknumber = SPLT_TRUE;
              }
              else if (all_tags)
              {
                auto_increment_tracknumber = SPLT_TRUE;
                first_time_auto_increment_tracknumber = SPLT_TRUE;
              }
              break;
            default:
              ambiguous = SPLT_TRUE;
              break;
          }
        }

        if (cur_pos <= old_pos)
        {
          cur_pos++;
        }
      }

      int track = -1;
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
            ambiguous = SPLT_TRUE;
          }
        }
        if (is_number)
        {
          track = atoi(tracknumber);

          //copy the tracknumber for all tags
          if (all_tags || auto_increment_tracknumber)
          {
            all_tracknumber = track;
          }
        }
      }

      //
      if (auto_increment_tracknumber)
      {
        if (first_time_auto_increment_tracknumber)
        {
          auto_incremented_tracknumber = all_tracknumber;
          first_time_auto_increment_tracknumber = SPLT_FALSE;
        }
        track = auto_incremented_tracknumber++;
      }

      if ((s_title > 1) || (s_artist > 1)
          || (s_album > 1) || (s_performer > 1)
          || (s_year > 1) || (s_comment > 1)
          || (s_tracknumber > 1))
      {
        ambiguous = SPLT_TRUE;
      }

      //if we don't have already set the original tags,
      //we set the tags
      if (!original_tags)
      {
        if (track == -1)
        {
          track = 0;
        }

        //we put the tags
        int err = splt_t_append_tags(state, title, artist,
            album, performer, year, comment, track, genre);
        if (err < 0)
        {
          *error = err;
          get_out_from_while = SPLT_TRUE;
        }
      }
      else
      {
        //we put the tags
        int err = splt_t_append_only_non_null_previous_tags(state, title, artist,
            album, performer, year, comment, track, genre);
        if (err < 0)
        {
          *error = err;
          get_out_from_while = SPLT_TRUE;
        }
      }

      //if we have all tags, set them
      if (we_had_all_tags && !original_tags)
      {
        int index = state->split.real_tagsnumber - 1;
        if (!title)
        {
          splt_t_set_tags_char_field(state, index, SPLT_TAGS_TITLE, all_title);
        }
        if (!artist)
        {
          splt_t_set_tags_char_field(state, index, SPLT_TAGS_ARTIST, all_artist);
        }
        if (!album)
        {
          splt_t_set_tags_char_field(state, index, SPLT_TAGS_ALBUM, all_album);
        }
        if (!performer)
        {
          splt_t_set_tags_char_field(state, index, SPLT_TAGS_PERFORMER, all_performer);
        }
        if (!year)
        {
          splt_t_set_tags_char_field(state, index, SPLT_TAGS_YEAR, all_year);
        }
        if (!comment)
        {
          splt_t_set_tags_char_field(state, index, SPLT_TAGS_COMMENT, all_comment);
        }
        if (!tracknumber && ! auto_incremented_tracknumber)
        {
          splt_t_set_tags_int_field(state, index, SPLT_TAGS_TRACK, all_tracknumber);
        }
        if (genre == 12)
        {
          splt_t_set_tags_uchar_field(state, index, SPLT_TAGS_GENRE, all_genre);
        }
      }

end_while:
      //we free the memory
      if (title) { free(title); title = NULL; }
      if (artist) { free(artist); artist = NULL; }
      if (album) { free(album); album = NULL; }
      if (performer) { free(performer); performer = NULL; }
      if (year) { free(year); year = NULL; }
      if (comment) { free(comment); comment = NULL; }
      if (tracknumber) { free(tracknumber); tracknumber = NULL; }

      //we get out from while when error
      if (get_out_from_while)
      {
        break;
      }

      //set all tags for the next
      if (all_tags)
      {
        we_had_all_tags = SPLT_TRUE;
        all_tags = SPLT_FALSE;
      }

      tags_appended++;
    }

    //free all tags memory
    if (all_title) { free(all_title); all_title = NULL; }
    if (all_artist) { free(all_artist); all_artist = NULL; }
    if (all_album) { free(all_album); all_album = NULL; }
    if (all_performer) { free(all_performer); all_performer = NULL; }
    if (all_year) { free(all_year); all_year = NULL; }
    if (all_comment) { free(all_comment); all_comment = NULL; }

    return ambiguous;
  }

  return SPLT_FALSE;
}

/*******************************/
/* utils for the output format */

int splt_u_parse_outformat(char *s, splt_state *state)
{
  char *ptrs = NULL, *ptre = NULL;
  int i=0, amb = SPLT_OUTPUT_FORMAT_AMBIGUOUS, len=0;

  for (i=0; i<strlen(s); i++)
  {
    if (s[i]=='+') 
    {
      s[i]=' ';
    }
    else 
    {
      if (s[i] == SPLT_VARCHAR) 
      {
        s[i]='%';
      }
    }
  }

  ptrs = s;
  i = 0;
  ptre = strchr(ptrs+1, '%');
  if (s[0] != '%')
  {
    if (ptre==NULL)
    {
      len = strlen(ptrs);
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
    ptre = s;
  }

  //if stdout, NOT ambiguous
  if (splt_t_is_stdout(state))
  {
    return SPLT_OUTPUT_FORMAT_OK;
  }

  char err[2] = { '\0' };

  if (ptre == NULL)
  {
    splt_t_set_error_data(state, err);
    return SPLT_OUTPUT_FORMAT_AMBIGUOUS;
  }
  ptrs = ptre;

  while (((ptre = strchr(ptrs+1, '%')) != NULL) && (i < SPLT_OUTNUM))
  {
    char cf = *(ptrs+1);

    len = ptre-ptrs;
    if (len > SPLT_MAXOLEN)
    {
      len = SPLT_MAXOLEN;
    }

    switch (cf)
    {
      case 's':
      case 'S':
      case 'm':
      case 'M':
      case 'h':
      case 'H':
      case 'a':
      case 'A':
      case 'b':
      case 'f':
      case 'p':
        break;
      case 't':
      case 'l':
      case 'L':
      case 'u':
      case 'U':
      case 'n':
      case 'N':
        amb = SPLT_OUTPUT_FORMAT_OK;
        break;
      default:
        err[0] = cf;
        splt_t_set_error_data(state, err);
        return SPLT_OUTPUT_FORMAT_ERROR;
    }

    strncpy(state->oformat.format[i++], ptrs, len);
    ptrs = ptre;
  }

  strncpy(state->oformat.format[i], ptrs, strlen(ptrs));

  if (ptrs[1]=='t')
  {
    amb = SPLT_OUTPUT_FORMAT_OK;
  }

  if (ptrs[1]=='n')
  {
    amb = SPLT_OUTPUT_FORMAT_OK;
  }

  return amb;
}

static const char *splt_u_get_format_ptr(const char *format, char *temp)
{
  int format_length = strlen(format);
  const char *format_ptr = format;

  if ((format_length > 2) && isdigit(format[2]))
  {
    temp[2] = format[2];
    format_ptr = format + 1;
  }

  return format_ptr;
}

static int splt_u_get_requested_num_of_digits(splt_state *state, const char *format,
    int *requested_num_of_digits, int is_alpha)
{
  int format_length = strlen(format);
  int number_of_digits = 0;
  if (is_alpha)
  {
    number_of_digits = state->oformat.output_alpha_format_digits;
  }
  else
  {
    number_of_digits = splt_t_get_oformat_number_of_digits_as_int(state);
  }
  int max_number_of_digits = number_of_digits;
  *requested_num_of_digits = number_of_digits;

  if ((format_length > 2) && isdigit(format[2]))
  {
    *requested_num_of_digits = format[2] - '0';
  }

  if (*requested_num_of_digits > number_of_digits)
  {
    max_number_of_digits = *requested_num_of_digits;
  }

  return max_number_of_digits;
}

/*
 * Encode track number as 'A', 'B', ... 'Z', 'AA, 'AB', ...
 *
 * This is not simply "base 26"; note that the lowest 'digit' is from 'A' to
 * 'Z' (base 26), but all higher digits are 'A' to 'Z' plus 'nothing', i.e.,
 * base 27. In other words, since after 'Z' comes 'AA', we cannot use 'AA'
 * as the padded version of track number 1 ('A').
 *
 * This means that there are two distinct work modes:
 * - The normal encoding is as described above.
 * - When the user has specified the number of digits (padding), we encode
 *   the track number as simple base-26: 'AAA', 'AAB', ... 'AAZ', 'ABA',
 *   'ABB', ...
 */
static void splt_u_alpha_track(splt_state *state, int nfield,
    char *fm, int fm_length, int number_of_digits, int tracknumber)
{
  char *format = state->oformat.format[nfield];
  int lowercase = (toupper(format[1]) == 'L');
  char a = lowercase ? 'a' : 'A';
  int zerobased = tracknumber - 1;
  int i = 1, min_digits = state->oformat.output_alpha_format_digits;

  if (number_of_digits > 1)
  {
    /* Padding required => simple base-26 encoding */
    if (number_of_digits < min_digits)
      number_of_digits = min_digits;
    for (i = 1; i <= number_of_digits; ++ i, zerobased /= 26)
    {
      int digit = (zerobased % 26);
      fm[number_of_digits - i] = a + digit;
    }
  }
  else
  {
    /* No padding: First letter base-26, others base-27 */
    number_of_digits = min_digits;

    /* Start with the first, base-26 'digit' */
    fm[number_of_digits - 1] = a + (zerobased % 26);

    /* Now handle all other digits */
    zerobased /= 26;
    for (i = 2; i <= number_of_digits; ++ i, zerobased /= 27)
    {
      int digit = (zerobased % 27);
      fm[number_of_digits - i] = a + digit - 1;
    }
  }

  int offset = 0;
  if ((strlen(format) > 2) && isdigit(format[2]))
  {
    offset = 1;
  }
  snprintf(fm + number_of_digits, fm_length - number_of_digits,
      "%s", format + 2 + offset);
}

void splt_u_create_output_dirs_if_necessary(splt_state *state,
    const char *output_filename, int *error)
{
  if (splt_t_get_int_option(state, SPLT_OPT_CREATE_DIRS_FROM_FILENAMES))
  {
    char *only_dirs = strdup(output_filename);
    if (! only_dirs)
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      return;
    }

    char *dir_char = strrchr(only_dirs, SPLT_DIRCHAR);
    if (dir_char != NULL)
    {
      *dir_char = '\0';
      int err = splt_u_create_directories(state, only_dirs);
      if (err < 0) { *error = err; }
    }

    free(only_dirs);
    only_dirs = NULL;
  }
}

//writes the current filename according to the output_filename
int splt_u_put_output_format_filename(splt_state *state)
{
  int error = SPLT_OK;

  int output_filenames = splt_t_get_int_option(state, SPLT_OPT_OUTPUT_FILENAMES);
  if (output_filenames == SPLT_OUTPUT_CUSTOM)
  {
    return error;
  }

  char *temp = NULL;
  char *fm = NULL;
  int i = 0;
  char *output_filename = NULL;
  int output_filename_size = 0;

  char *title = NULL;
  char *artist = NULL;
  char *album = NULL;
  char *performer = NULL;
  char *artist_or_performer = NULL;
  char *original_filename = NULL;

  int old_current_split = splt_t_get_current_split_file_number(state) - 1;
  int current_split = old_current_split;

  long mins = -1;
  long secs = -1;
  long hundr = -1;
  long point_value = splt_t_get_splitpoint_value(state, old_current_split, &error);
  splt_u_get_mins_secs_hundr(point_value, &mins, &secs, &hundr);
  long next_mins = -1;
  long next_secs = -1;
  long next_hundr = -1;
  if (splt_t_splitpoint_exists(state, old_current_split + 1))
  {
    point_value = splt_t_get_splitpoint_value(state, old_current_split +1, &error);
    long total_time = splt_t_get_total_time(state);
    if (point_value > total_time)
    {
      point_value = total_time;
    }
    splt_u_get_mins_secs_hundr(point_value, &next_mins, &next_secs, &next_hundr);
  }

  int fm_length = 0;

  //if we get the tags from the first file
  int remaining_tags_like_x = 
    splt_t_get_int_option(state,SPLT_OPT_ALL_REMAINING_TAGS_LIKE_X);
  if ((current_split >= state->split.real_tagsnumber) &&
      (remaining_tags_like_x != -1))
  {
    current_split = remaining_tags_like_x;
  }

  splt_u_print_debug(state,"The output format is ",0,state->oformat.format_string);

  long mMsShH_value = -1;

  for (i = 0; i < SPLT_OUTNUM; i++)
  {
    if (strlen(state->oformat.format[i]) == 0)
    {
      break;
    }
    //if we have some % in the format (@ has been converted to %)
    if (state->oformat.format[i][0] == '%')
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

      temp[0] = '%';
      temp[1] = 's';
      char char_variable = state->oformat.format[i][1];
      switch (char_variable)
      {
        case 's':
          mMsShH_value = secs;
          goto put_value;
        case 'S':
          mMsShH_value = next_secs;
          goto put_value;
        case 'm':
          mMsShH_value = mins;
          goto put_value;
        case 'M':
          mMsShH_value = next_mins;
          goto put_value;
        case 'h':
          mMsShH_value = hundr;
          goto put_value;
        case 'H':
          mMsShH_value = next_hundr;
put_value:
          temp[1] = '0';
          temp[2] = '2';
          temp[3] = 'l';
          temp[4] = 'd';

          if (mMsShH_value != -1)
          {
            const char *format = NULL;
            int offset = 5;

            //don't print out @h or @H if 0 for default output
            if ((strcmp(state->oformat.format_string, SPLT_DEFAULT_OUTPUT) == 0) &&
                (mMsShH_value == 0) &&
                (char_variable == 'h' || char_variable == 'H'))
            {
              if (char_variable == 'h')
              {
                format = state->oformat.format[i]+2;
                offset = 0;
              }
              else
              {
                output_filename[strlen(output_filename)-1] = '\0';
                break;
              }
            }
            else
            {
              format = splt_u_get_format_ptr(state->oformat.format[i], temp);
            }

            int requested_num_of_digits = 0;
            int max_number_of_digits = splt_u_get_requested_num_of_digits(state,
                state->oformat.format[i], &requested_num_of_digits, SPLT_FALSE);

            snprintf(temp + offset, temp_len, format + 2);

            fm_length = strlen(temp) + 1 + max_number_of_digits;
            if ((fm = malloc(fm_length * sizeof(char))) == NULL)
            {
              error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
              goto end;
            }

            snprintf(fm, fm_length, temp, mMsShH_value);
          }
          break;
        case 'A':
          if (splt_t_tags_exists(state,current_split))
          {
            artist_or_performer =
              splt_t_get_tags_char_field(state,current_split, SPLT_TAGS_PERFORMER);
            splt_u_cleanstring(state, artist_or_performer, &error);
            if (error < 0) { goto end; };

            if (artist_or_performer == NULL || artist_or_performer[0] == '\0')
            {
              artist_or_performer = 
                splt_t_get_tags_char_field(state,current_split, SPLT_TAGS_ARTIST);
              splt_u_cleanstring(state, artist_or_performer, &error);
              if (error < 0) { goto end; };
            }
          }
          else
          {
            artist_or_performer = NULL;
          }

          //
          if (artist_or_performer != NULL)
          {
            snprintf(temp+2,temp_len, state->oformat.format[i]+2);

            int artist_length = 0;
            artist_length = strlen(artist_or_performer);
            fm_length = strlen(temp) + artist_length + 1;
          }
          else
          {
            snprintf(temp,temp_len, state->oformat.format[i]+2);
            fm_length = strlen(temp) + 1;
          }

          if ((fm = malloc(fm_length * sizeof(char))) == NULL)
          {
            error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
            goto end;
          }

          //
          if (artist_or_performer != NULL)
          {
            snprintf(fm, fm_length, temp, artist_or_performer);
          }
          else
          {
            snprintf(fm, fm_length, temp);
          }

          break;
        case 'a':
          if (splt_t_tags_exists(state,current_split))
          {
            //we get the artist
            artist =
              splt_t_get_tags_char_field(state,current_split, SPLT_TAGS_ARTIST);
            splt_u_cleanstring(state, artist, &error);
            if (error < 0) { goto end; };
          }
          else
          {
            artist = NULL;
          }

          //
          if (artist != NULL)
          {
            snprintf(temp+2,temp_len, state->oformat.format[i]+2);

            int artist_length = 0;
            artist_length = strlen(artist);
            fm_length = strlen(temp) + artist_length + 1;
          }
          else
          {
            snprintf(temp,temp_len, state->oformat.format[i]+2);
            fm_length = strlen(temp) + 1;
          }

          if ((fm = malloc(fm_length * sizeof(char))) == NULL)
          {
            error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
            goto end;
          }

          //
          if (artist != NULL)
          {
            snprintf(fm, fm_length, temp, artist);
          }
          else
          {
            snprintf(fm, fm_length, temp);
          }
          break;
        case 'b':
          if (splt_t_tags_exists(state,current_split))
          {
            //we get the album
            album =
              splt_t_get_tags_char_field(state,current_split, SPLT_TAGS_ALBUM);
            splt_u_cleanstring(state, album, &error);
            if (error < 0) { goto end; };
          }
          else
          {
            album = NULL;
          }

          //
          if (album != NULL)
          {
            int album_length = 0;
            album_length = strlen(album);
            snprintf(temp+2, temp_len, state->oformat.format[i]+2);

            fm_length = strlen(temp) + album_length + 1;
          }
          else
          {
            snprintf(temp,temp_len, state->oformat.format[i]+2);
            fm_length = strlen(temp) + 1;
          }

          if ((fm = malloc(fm_length * sizeof(char))) == NULL)
          {
            error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
            goto end;
          }

          //
          if (album != NULL)
          {
            snprintf(fm, fm_length, temp, album);
          }
          else
          {
            snprintf(fm, fm_length, "%s", temp);
          }
          break;
        case 't':
          if (splt_t_tags_exists(state,current_split))
          {
            //we get the title
            title = splt_t_get_tags_char_field(state,current_split, SPLT_TAGS_TITLE);
            splt_u_cleanstring(state, title, &error);
            if (error < 0) { goto end; };
          }
          else
          {
            title = NULL;
          }

          //
          if (title != NULL)
          {
            int title_length = 0;
            title_length = strlen(title);
            snprintf(temp+2, temp_len, state->oformat.format[i]+2);

            fm_length = strlen(temp) + title_length + 1;
          }
          else
          {
            snprintf(temp,temp_len, state->oformat.format[i]+2);
            fm_length = strlen(temp) + 1;
          }

          if ((fm = malloc(fm_length * sizeof(char))) == NULL)
          {
            error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
            goto end;
          }

          //
          if (title != NULL)
          {
            snprintf(fm, fm_length, temp, title);
          }
          else
          {
            snprintf(fm, fm_length, temp);
          }
          break;
        case 'p':
          if (splt_t_tags_exists(state,current_split))
          {
            //we get the performer
            performer = splt_t_get_tags_char_field(state,current_split, SPLT_TAGS_PERFORMER);
            splt_u_cleanstring(state, performer, &error);
            if (error < 0) { goto end; };
          }
          else
          {
            performer = NULL;
          }

          //
          if (performer != NULL)
          {
            int performer_length = 0;
            performer_length = strlen(performer);
            snprintf(temp+2, temp_len, state->oformat.format[i]+2);

            fm_length = strlen(temp) + performer_length + 1;
          }
          else
          {
            snprintf(temp,temp_len, state->oformat.format[i]+2);
            fm_length = strlen(temp) + 1;
          }

          if ((fm = malloc(fm_length * sizeof(char))) == NULL)
          {
            error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
            goto end;
          }

          if (performer != NULL)
          {
            snprintf(fm, fm_length, temp, performer);
          }
          else
          {
            snprintf(fm, fm_length, temp);
          }
          break;
        case 'l':
        case 'L':
        case 'u':
        case 'U':
        case 'n':
        case 'N':
          temp[1] = '0';
          temp[2] = splt_t_get_oformat_number_of_digits_as_char(state);
          temp[3] = 'd';

          int tracknumber = old_current_split + 1;

          //if not time split, or normal split, or silence split or error,
          //we put the track number from the tags
          int split_mode = splt_t_get_int_option(state,SPLT_OPT_SPLIT_MODE);
          if ((isupper(state->oformat.format[i][1])) ||
              ((split_mode != SPLT_OPTION_TIME_MODE) &&
               (split_mode != SPLT_OPTION_NORMAL_MODE) &&
               (split_mode != SPLT_OPTION_SILENCE_MODE) &&
               (split_mode != SPLT_OPTION_ERROR_MODE)))
          {
            if (splt_t_tags_exists(state,current_split))
            {
              int tags_track = splt_t_get_tags_int_field(state,
                  current_split,SPLT_TAGS_TRACK);
              if (tags_track > 0)
              {
                tracknumber = tags_track;
              }
            }
          }

          int requested_num_of_digits = 0;
          int max_num_of_digits = splt_u_get_requested_num_of_digits(state,
              state->oformat.format[i], &requested_num_of_digits, SPLT_FALSE);

          int alpha_requested_num_of_digits = 0;
          int alpha_max_num_of_digits = splt_u_get_requested_num_of_digits(state,
              state->oformat.format[i], &alpha_requested_num_of_digits, SPLT_TRUE);

          int is_numeric = toupper(state->oformat.format[i][1]) == 'N';
          if (is_numeric)
          {
            const char *format = splt_u_get_format_ptr(state->oformat.format[i], temp);

            snprintf(temp + 4, temp_len, format + 2);
            fm_length = strlen(temp) + 1 + max_num_of_digits;
          }
          else
          {
            fm_length = strlen(state->oformat.format[i]) + 1 + alpha_max_num_of_digits;
          }

          if ((fm = malloc(fm_length * sizeof(char))) == NULL)
          {
            error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
            goto end;
          }
          memset(fm, '\0', fm_length);

          if (is_numeric)
          {
            snprintf(fm, fm_length, temp, tracknumber);
          }
          else
          {
            splt_u_alpha_track(state, i, fm, fm_length,
                alpha_requested_num_of_digits, tracknumber);
          }
          break;
        case 'f':
          if (splt_t_get_filename_to_split(state) != NULL)
          {
            //we get the filename
            original_filename = strdup(splt_u_get_real_name(splt_t_get_filename_to_split(state)));
            if (original_filename)
            {
              snprintf(temp+2,temp_len, state->oformat.format[i]+2);

              //we cut extension
              splt_u_cut_extension(original_filename);

              int filename_length = strlen(original_filename);

              fm_length = strlen(temp) + filename_length;
              if ((fm = malloc(fm_length * sizeof(char))) == NULL)
              {
                error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
                goto end;
              }

              snprintf(fm, fm_length, temp, original_filename);
              free(original_filename);
              original_filename = NULL;
            }
            else
            {
              error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
              goto end;
            }
          }
          break;
      }
    }
    else
    {
      fm_length = SPLT_MAXOLEN;
      if ((fm = malloc(fm_length * sizeof(char))) == NULL)
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
      if ((output_filename = malloc((1+fm_size)*sizeof(char))) == NULL)
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
      if ((output_filename = realloc(output_filename, output_filename_size
              * sizeof(char))) == NULL)
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

  splt_u_print_debug(state,"The new output filename is ",0,output_filename);
  int cur_splt = splt_t_get_current_split(state);
  int name_error =
    splt_t_set_splitpoint_name(state, cur_splt, output_filename);
  if (name_error != SPLT_OK) { error = name_error; }

end:
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
      fflush(stderr);
      break;
    case SPLT_IERROR_SET_ORIGINAL_TAGS:
      fprintf(stderr,
          "libmp3splt: cannot set original file tags, "
          "libmp3splt not compiled with libid3tag\n");
      fflush(stderr);
      break;
    case SPLT_IERROR_CHAR:
      fprintf(stderr,
          "libmp3splt: error in %s with message '%s'\n",function,arg_char);
      fflush(stderr);
      break;
    default:
      fprintf(stderr,
          "libmp3splt: unknown error in %s\n", function);
      fflush(stderr);
      break;
  }
}

/****************************/
/* utils miscellaneous */

//get silence position depending of the offset
float splt_u_silence_position(struct splt_ssplit *temp, float off)
{
  float length_of_silence = (temp->end_position - temp->begin_position);
  float position = temp->begin_position + (length_of_silence * off);

  return position;
}

//returns a string error message from the 'error_code'
//-return result must be freed
char *splt_u_strerror(splt_state *state, int error_code)
{
  int max_error_size = 4096;
  char *error_msg = malloc(sizeof(char) * max_error_size);
  if (error_msg)
  {
    memset(error_msg,'\0',4096);

    switch (error_code)
    {
      //
      case SPLT_MIGHT_BE_VBR:
        snprintf(error_msg,max_error_size,
            _(" warning: might be VBR, use frame mode"));
        break;
      case SPLT_SYNC_OK:
        snprintf(error_msg,max_error_size, _(" error mode ok"));
        break;
      case SPLT_ERR_SYNC:
        snprintf(error_msg,max_error_size, _(" error: unknown sync error"));
        break;
      case SPLT_ERR_NO_SYNC_FOUND:
        snprintf(error_msg,max_error_size, _(" no sync errors found"));
        break;
      case SPLT_ERR_TOO_MANY_SYNC_ERR:
        snprintf(error_msg,max_error_size, _(" sync error: too many sync errors"));
        break;
        //
      case SPLT_FREEDB_MAX_CD_REACHED:
        snprintf(error_msg,max_error_size, _(" maximum number of found CD reached"));
        break;
      case SPLT_CUE_OK:
        snprintf(error_msg,max_error_size, _(" cue file processed"));
        break;
      case SPLT_CDDB_OK:
        snprintf(error_msg,max_error_size, _(" cddb file processed"));
        break;
      case SPLT_FREEDB_FILE_OK:
        snprintf(error_msg,max_error_size, _(" freedb file downloaded"));
        break;
      case SPLT_FREEDB_OK:
        snprintf(error_msg,max_error_size, _(" freedb search processed"));
        break;
        //
      case SPLT_FREEDB_ERROR_INITIALISE_SOCKET:
        snprintf(error_msg,max_error_size, 
            _(" freedb error: cannot initialise socket (%s)"),
            state->err.strerror_msg);
        break;
      case SPLT_FREEDB_ERROR_CANNOT_GET_HOST:
        snprintf(error_msg,max_error_size, 
            _(" freedb error: cannot get host '%s' by name (%s)"),
            state->err.error_data, state->err.strerror_msg);
        break;
      case SPLT_FREEDB_ERROR_CANNOT_OPEN_SOCKET:
        snprintf(error_msg,max_error_size, _(" freedb error: cannot open socket"));
        break;
      case SPLT_FREEDB_ERROR_CANNOT_CONNECT:
        snprintf(error_msg,max_error_size, 
            _(" freedb error: cannot connect to host '%s' (%s)"),
            state->err.error_data, state->err.strerror_msg);
        break;
      case SPLT_FREEDB_ERROR_CANNOT_SEND_MESSAGE:
        snprintf(error_msg,max_error_size, 
            _(" freedb error: cannot send message to host '%s' (%s)"),
            state->err.error_data, state->err.strerror_msg);
        break;
      case SPLT_FREEDB_ERROR_INVALID_SERVER_ANSWER:
        snprintf(error_msg,max_error_size, _(" freedb error: invalid server answer"));
        break;
      case SPLT_FREEDB_ERROR_SITE_201:
        snprintf(error_msg,max_error_size, _(" freedb error: site returned code 201"));
        break;
      case SPLT_FREEDB_ERROR_SITE_200:
        snprintf(error_msg,max_error_size, _(" freedb error: site returned code 200"));
        break;
      case SPLT_FREEDB_ERROR_BAD_COMMUNICATION:
        snprintf(error_msg,max_error_size, _(" freedb error: bad communication with site"));
        break;
      case SPLT_FREEDB_ERROR_GETTING_INFOS:
        snprintf(error_msg,max_error_size, _(" freedb error: could not get infos from site '%s'"),
            state->err.error_data);
        break;
      case SPLT_FREEDB_NO_CD_FOUND:
        snprintf(error_msg,max_error_size, _(" no CD found for this search"));
        break;
      case SPLT_FREEDB_ERROR_CANNOT_RECV_MESSAGE:
        snprintf(error_msg,max_error_size,
            _(" freedb error: cannot receive message from server '%s' (%s)"),
            state->err.error_data, state->err.strerror_msg);
        break;
      case SPLT_INVALID_CUE_FILE:
        snprintf(error_msg,max_error_size, _(" cue error: invalid cue file '%s'"),
            state->err.error_data);
        break;
      case SPLT_INVALID_CDDB_FILE:
        snprintf(error_msg,max_error_size, _(" cddb error: invalid cddb file '%s'"),
            state->err.error_data);
        break;
      case SPLT_FREEDB_NO_SUCH_CD_IN_DATABASE:
        snprintf(error_msg,max_error_size, _(" freedb error: No such CD entry in database"));
        break;
      case SPLT_FREEDB_ERROR_SITE:
        snprintf(error_msg,max_error_size, _(" freedb error: site returned an unknown error"));
        break;
        //
      case SPLT_DEWRAP_OK:
        snprintf(error_msg,max_error_size, _(" wrap split ok"));
        break;
        //
      case SPLT_DEWRAP_ERR_FILE_LENGTH:
        snprintf(error_msg,max_error_size, _(" wrap error: incorrect file length"));
        break;
      case SPLT_DEWRAP_ERR_VERSION_OLD:
        snprintf(error_msg,max_error_size,
            _(" wrap error: libmp3splt version too old for this wrap file"));
        break;
      case SPLT_DEWRAP_ERR_NO_FILE_OR_BAD_INDEX:
        snprintf(error_msg,max_error_size,
            _(" wrap error: no file found or bad index"));
        break;
      case SPLT_DEWRAP_ERR_FILE_DAMAGED_INCOMPLETE:
        snprintf(error_msg,max_error_size,
            _(" wrap error: file '%s' damaged or incomplete"),
            state->err.error_data);
        break;
      case SPLT_DEWRAP_ERR_FILE_NOT_WRAPED_DAMAGED:
        snprintf(error_msg,max_error_size,
            _(" wrap error: maybe not a wrapped file or wrap file damaged"));
        break;
        //
      case SPLT_OK_SPLIT_EOF:
        snprintf(error_msg,max_error_size,_(" file split (EOF)"));
        break;
      case SPLT_NO_SILENCE_SPLITPOINTS_FOUND:
        snprintf(error_msg,max_error_size, _(" no silence splitpoints found"));
        break;
      case SPLT_TIME_SPLIT_OK:
        snprintf(error_msg,max_error_size, _(" time split ok"));
        break;
      case SPLT_SILENCE_OK:
        snprintf(error_msg,max_error_size, _(" silence split ok"));
        break;
      case SPLT_SPLITPOINT_BIGGER_THAN_LENGTH:
        snprintf(error_msg,max_error_size,
            _(" file split, splitpoints bigger than length"));
        break;
      case SPLT_OK_SPLIT:
        snprintf(error_msg,max_error_size, _(" file split"));
        break;
      case SPLT_OK:
        break;
      case SPLT_ERROR_SPLITPOINTS:
        snprintf(error_msg,max_error_size, _(" error: not enough splitpoints (<2)"));
        break;
      case SPLT_ERROR_CANNOT_OPEN_FILE:
        snprintf(error_msg,max_error_size,
            _(" error: cannot open file '%s': %s"),
            state->err.error_data, state->err.strerror_msg);
        break;
      case SPLT_ERROR_CANNOT_CLOSE_FILE:
        snprintf(error_msg,max_error_size,
            _(" error: cannot close file '%s': %s"),
            state->err.error_data, state->err.strerror_msg);
        break;
      case SPLT_ERROR_INVALID:
        ;
        int err = SPLT_OK;
        const char *plugin_name = splt_p_get_name(state,&err);
        snprintf(error_msg,max_error_size,
            _(" error: invalid input file '%s' for '%s' plugin"),
            state->err.error_data, plugin_name);
        break;
      case SPLT_ERROR_EQUAL_SPLITPOINTS:
        snprintf(error_msg,max_error_size,
            _(" error: splitpoints are equal (%s)"),
            state->err.error_data);
        break;
      case SPLT_ERROR_TIME_SPLIT_VALUE_INVALID:
        snprintf(error_msg,max_error_size, _(" error: invalid time split value"));
        break;
      case SPLT_ERROR_SPLITPOINTS_NOT_IN_ORDER:
        snprintf(error_msg,max_error_size,
            _(" error: the splitpoints are not in order (%s)"),
            state->err.error_data);
        break;
      case SPLT_ERROR_NEGATIVE_SPLITPOINT:
        snprintf(error_msg,max_error_size, _(" error: negative splitpoint (%s)"),
            state->err.error_data);
        break;
      case SPLT_ERROR_INCORRECT_PATH:
        snprintf(error_msg,max_error_size,
            _(" error: bad destination folder '%s' (%s)"),
            state->err.error_data, state->err.strerror_msg);
        break;
      case SPLT_ERROR_INCOMPATIBLE_OPTIONS:
        snprintf(error_msg,max_error_size, _(" error: incompatible options"));
        break;
      case SPLT_ERROR_INPUT_OUTPUT_SAME_FILE:
        snprintf(error_msg,max_error_size,
            _(" input and output are the same file ('%s')"),
            state->err.error_data);
        break;
      case SPLT_ERROR_CANNOT_ALLOCATE_MEMORY:
        snprintf(error_msg,max_error_size, _(" error: cannot allocate memory"));
        break;
      case SPLT_ERROR_CANNOT_OPEN_DEST_FILE:
        snprintf(error_msg,max_error_size,
            _(" error: cannot open destination file '%s': %s"),
            state->err.error_data,state->err.strerror_msg);
        break;
      case SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE:
        snprintf(error_msg,max_error_size,
            _(" error: cannot write to output file '%s'"),
            state->err.error_data);
        break;
      case SPLT_ERROR_WHILE_READING_FILE:
        snprintf(error_msg,max_error_size, _(" error: error while reading file '%s': %s"),
            state->err.error_data, state->err.strerror_msg);
        break;
      case SPLT_ERROR_SEEKING_FILE:
        snprintf(error_msg,max_error_size, _(" error: cannot seek file '%s'"),
            state->err.error_data);
        break;
      case SPLT_ERROR_BEGIN_OUT_OF_FILE:
        snprintf(error_msg,max_error_size, _(" error: begin point out of file"));
        break;
      case SPLT_ERROR_INEXISTENT_FILE:
        snprintf(error_msg,max_error_size, _(" error: inexistent file '%s': %s"),
            state->err.error_data,state->err.strerror_msg);
        break;
      case SPLT_SPLIT_CANCELLED:
        snprintf(error_msg,max_error_size, _(" split process cancelled"));
        break;
      case SPLT_ERROR_LIBRARY_LOCKED: 
        snprintf(error_msg,max_error_size, _(" error: library locked"));
        break;
      case SPLT_ERROR_STATE_NULL:
        snprintf(error_msg,max_error_size,
            _(" error: the state has not been initialized with 'mp3splt_new_state'"));
        break;
      case SPLT_ERROR_NEGATIVE_TIME_SPLIT:
        snprintf(error_msg,max_error_size, _(" error: negative time split"));
        break;
      case SPLT_ERROR_CANNOT_CREATE_DIRECTORY:
        snprintf(error_msg,max_error_size, _(" error: cannot create directory '%s'"),
            state->err.error_data);
        break;
      case SPLT_ERROR_NO_PLUGIN_FOUND:
        snprintf(error_msg,max_error_size, _(" error: no plugin found"));
        break;
      case SPLT_ERROR_CANNOT_INIT_LIBLTDL:
        snprintf(error_msg,max_error_size, _(" error: cannot initiate libltdl"));
        break;
      case SPLT_ERROR_CRC_FAILED:
        snprintf(error_msg,max_error_size, _(" error: CRC failed"));
        break;
      case SPLT_ERROR_NO_PLUGIN_FOUND_FOR_FILE:
        snprintf(error_msg,max_error_size,
            _(" error: no plugin matches the file '%s'"),
            state->err.error_data);
        break;
        //
      case SPLT_OUTPUT_FORMAT_OK:
        break;
      case SPLT_OUTPUT_FORMAT_AMBIGUOUS:
        snprintf(error_msg,max_error_size,
            _(" warning: output format ambiguous (@t or @n missing)"));
        break;
        //
      case SPLT_OUTPUT_FORMAT_ERROR:
        if (state->err.error_data != NULL && strlen(state->err.error_data) == 1)
        {
          snprintf(error_msg,max_error_size,
              _(" error: illegal variable '@%s' in output format"),
              state->err.error_data);
        }
        else
        {
          snprintf(error_msg,max_error_size, _(" error: invalid output format"));
        }
        break;
        //
      case SPLT_ERROR_INEXISTENT_SPLITPOINT:
        snprintf(error_msg,max_error_size, _(" error: inexistent splitpoint"));
        break;
        //
      case SPLT_ERROR_PLUGIN_ERROR:
        snprintf(error_msg,max_error_size, _(" plugin error: '%s'"),
            state->err.error_data);
        break;
        //
      case SPLT_PLUGIN_ERROR_UNSUPPORTED_FEATURE:
        ;
        splt_plugins *pl = state->plug;
        int current_plugin = splt_t_get_current_plugin(state);
        snprintf(error_msg,max_error_size, _(" error: unsupported feature for the plugin '%s'"),
            pl->data[current_plugin].info.name);
        break;
    }

    if (error_msg[0] == '\0')
    {
      free(error_msg);
      error_msg = NULL;
    }
  }
  else
  {
    //if not enough memory
    splt_u_error(SPLT_IERROR_CHAR,__func__, 0, _("not enough memory"));
  }

  return error_msg;
}

//debug messages
void splt_u_print_debug(splt_state *state, const char *message,
    double optional, const char *optional2)
{
  if (global_debug)
  {
    int mess_size = 1024;
    if (message)
    {
      mess_size += strlen(message);
    }
    if (optional2)
    {
      mess_size += strlen(optional2);
    }
    char *mess = malloc(sizeof(char) * mess_size);

    if (optional != 0)
    {
      if (optional2 != NULL)
      {
        snprintf(mess, mess_size, "%s %f _%s_\n",message, optional, optional2);
      }
      else
      {
        snprintf(mess, mess_size, "%s %f\n",message, optional);
      }
    }
    else
    {
      if (optional2 != NULL)
      {
        snprintf(mess, mess_size, "%s _%s_\n",message, optional2);
      }
      else
      {
        snprintf(mess, mess_size, "%s\n",message);
      }
    }

    if (state)
    {
      splt_t_put_debug_message_to_client(state, mess);
    }
    else
    {
      fprintf(stdout,"%s",mess);
      fflush(stdout);
    }

    free(mess);
    mess = NULL;
  }
}

//convert to float for hundredth
// 3460 -> 34.6  | 34 seconds and 6 hundredth
double splt_u_get_double_pos(long split)
{
  double pos = 0;
  pos = split / 100;
  pos += ((split % 100) / 100.);

  return pos;
}

void splt_u_get_mins_secs_hundr(long split_hundr, long *mins, long *secs, long *hundr)
{
  long h = split_hundr % 100;
  long split_hundr_without_h = split_hundr / 100;
  long m = split_hundr_without_h / 60;
  long s = split_hundr_without_h % 60;
  if (mins)
  {
    *mins = m;
  }
  if (secs)
  {
    *secs = s;
  }
  if (hundr)
  {
    *hundr = h;
  }
}

int splt_u_parse_ssplit_file(splt_state *state, FILE *log_file, int *error)
{
  char line[512] = { '\0' };
  int found = 0;

  while(fgets(line, 512, log_file)!=NULL)
  {
    int len = 0;
    float begin_position = 0, end_position = 0;
    if (sscanf(line, "%f\t%f\t%d", &begin_position, &end_position, &len) == 3)
    {
      splt_t_ssplit_new(&state->silence_list, begin_position, end_position, len, error);
      if (*error < 0)
      {
        break;
      }
      found++;
    }
  }

  return found;
}

//create recursive directories
int splt_u_create_directories(splt_state *state, const char *dir)
{
  int result = SPLT_OK;
  const char *ptr = NULL;
  if (dir[0] == '\0')
  {
    return result;
  }
  char *junk = malloc(sizeof(char) * (strlen(dir)+100));
  if (!junk)
  {
    return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  }
  
  splt_u_print_debug(state,"Creating directory ...",0,dir);
  
  ptr = dir;
#ifdef __WIN32__
  int first_time = SPLT_TRUE;
#endif
  while ((ptr = strchr(ptr, SPLT_DIRCHAR))!=NULL)
  {
		//handle C:DIRCHAR on windows
#ifdef __WIN32__
    if (first_time && (strlen(dir) > 2)
        && (dir[1] == ':') && (dir[2] == SPLT_DIRCHAR))
    {
      ptr++;
    }
    first_time = SPLT_FALSE;
#endif
    strncpy(junk, dir, ptr-dir);
    junk[ptr-dir] = '\0';
    ptr++;

    if (junk[0] != '\0')
    {
      if (! splt_u_check_if_directory(junk))
      {
        splt_u_print_debug(state,"directory ...",0, junk);

        if (result < 0) { goto end; }

        //don't create output directories if we pretend to split
        if (! splt_t_get_int_option(state, SPLT_OPT_PRETEND_TO_SPLIT))
        {
          if ((splt_u_mkdir(junk)) == -1)
          {
            splt_t_set_strerror_msg(state);
            splt_t_set_error_data(state,junk);
            result = SPLT_ERROR_CANNOT_CREATE_DIRECTORY;
            goto end;
          }
        }
      }
    }
  }

  if (dir)
  {
    //we have created all the directories except the last one
    char *last_dir = strdup(dir);
    if (!last_dir)
    {
      result = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      goto end;
    }

    if (! splt_u_check_if_directory(last_dir))
    {
      splt_u_print_debug(state,"final directory ...",0, last_dir);

      if ((splt_u_mkdir(last_dir)) == -1)
      {
        splt_t_set_strerror_msg(state);
        splt_t_set_error_data(state,last_dir);
        result = SPLT_ERROR_CANNOT_CREATE_DIRECTORY;
      }
    }

    if (last_dir)
    {
      free(last_dir);
      last_dir = NULL;
    }
  }

end:
  if (junk)
  {
    free(junk);
    junk = NULL;
  }
  
  return result;
}

void splt_u_print(char *mess)
{
	fprintf(stdout,"mess = _%s_ \n",mess);
	fflush(stdout);
}

//check if its a directory
int splt_u_check_if_directory(char *fname)
{
  if (fname == NULL)
  {
    return SPLT_FALSE;
  }
  else
  {
    mode_t st_mode;
    int status = splt_u_stat(fname, &st_mode, NULL);
    if (status == 0)
    {
      //if it is a directory
      if (S_ISDIR(st_mode))
      {
        return SPLT_TRUE;
      }
      else
      {
        return SPLT_FALSE;
      }
    }
    else
    {
      return SPLT_FALSE;
    }
  }

  return SPLT_FALSE;
}

#ifdef __WIN32__
static wchar_t *splt_u_win32_encoding_to_utf16(UINT encoding, const char *source)
{
  wchar_t *dest = NULL;

  int converted_size = MultiByteToWideChar(encoding, 0, source, -1, NULL, 0);
  if (converted_size > 0)
  {
    dest = malloc(sizeof(wchar_t) * converted_size);
    if (dest)
    {
      MultiByteToWideChar(encoding, 0, source, -1, dest, converted_size);
    }
  }

  return dest;
}

static char *splt_u_win32_utf16_to_encoding(UINT encoding, const wchar_t *source)
{
  char *dest = NULL;

  int converted_size = WideCharToMultiByte(encoding, 0, source, -1, NULL, 0, NULL, NULL);
  if (converted_size > 0)
  {
    dest = malloc(sizeof(char *) * converted_size);
    if (dest)
    {
      WideCharToMultiByte(encoding, 0, source, -1, dest, converted_size, NULL, NULL);
    }
  }

  return dest;
}

wchar_t *splt_u_win32_utf8_to_utf16(const char *source)
{
  return splt_u_win32_encoding_to_utf16(CP_UTF8, source);
}

char *splt_u_win32_utf16_to_utf8(const wchar_t *source)
{
  return splt_u_win32_utf16_to_encoding(CP_UTF8, source);
}

static int splt_u_win32_check_if_encoding_is_utf8(const char *source)
{
  int is_utf8 = SPLT_FALSE;

  if (source)
  {
    wchar_t *source_wchar = splt_u_win32_utf8_to_utf16(source);
    if (source_wchar)
    {
      char *source2 = splt_u_win32_utf16_to_utf8(source_wchar);
      if (source2)
      {
        if (strcmp(source, source2) == 0)
        {
          is_utf8 = SPLT_TRUE;
        }

        free(source2);
        source2 = NULL;
      }

      free(source_wchar);
      source_wchar = NULL;
    }
  }

  return is_utf8;
}
#endif

//windows filenames have to be converted to utf16
FILE *splt_u_fopen(const char *filename, const char *mode)
{
#ifdef __WIN32__
  if (splt_u_win32_check_if_encoding_is_utf8(filename))
  {
    wchar_t *wfilename = splt_u_win32_utf8_to_utf16(filename);
    wchar_t *wmode = splt_u_win32_utf8_to_utf16(mode);

    FILE *file = _wfopen(wfilename, wmode);

    if (wfilename)
    {
      free(wfilename);
      wfilename = NULL;
    }

    if (wmode)
    {
      free(wmode);
      wmode = NULL;
    }

    return file;
  }
  else
#endif
  {
    return fopen(filename, mode);
  }
}

int splt_u_mkdir(const char *path)
{
#ifdef __WIN32__
  if (splt_u_win32_check_if_encoding_is_utf8(path))
  {
    wchar_t *wpath = splt_u_win32_utf8_to_utf16(path);

    int ret = _wmkdir(wpath);

    if (wpath)
    {
      free(wpath);
      wpath = NULL;
    }

    return ret;
  }
  else
  {
    return mkdir(path);
  }
#else
  return mkdir(path, 0755);
#endif
}

int splt_u_stat(const char *path, mode_t *st_mode, off_t *st_size)
{
#ifdef __WIN32__
  if (splt_u_win32_check_if_encoding_is_utf8(path))
  {
    struct _stat buf;
    wchar_t *wpath = splt_u_win32_utf8_to_utf16(path);

    int ret = _wstat(wpath, &buf);

    if (wpath)
    {
      free(wpath);
      wpath = NULL;
    }

    if (st_mode != NULL)
    {
      *st_mode = buf.st_mode;
    }

    if (st_size != NULL)
    {
      *st_size = buf.st_size;
    }

    return ret;
  }
  else
#endif
  {
    struct stat buf;

#ifdef __WIN32__
    int ret = stat(path, &buf);
#else
    int ret = lstat(path, &buf);
#endif

    if (st_mode != NULL)
    {
      *st_mode = buf.st_mode;
    }

    if (st_size != NULL)
    {
      *st_size = buf.st_size;
    }

    return ret;
  }
}

char *splt_u_safe_strdup(char *input, int *error)
{
  if (input == NULL)
  {
    return NULL;
  }
  else
  {
    char *dup_input = strdup(input);
    if (dup_input != NULL)
    {
      return dup_input;
    }
    else
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      return NULL;
    }
  }
}

void splt_u_print_overlap_time(splt_state *state)
{
  long overlap_time = splt_t_get_long_option(state, SPLT_OPT_OVERLAP_TIME);
  if (overlap_time > 0)
  {
    char message[1024] = { '\0' };
    long mins = -1;
    long secs = -1;
    long hundr = -1;
    splt_u_get_mins_secs_hundr(overlap_time, &mins, &secs, &hundr);
    snprintf(message, 1024,
        _(" info: overlapping split files with %ld.%ld.%ld\n"),
        mins, secs, hundr);
    splt_t_put_info_message_to_client(state, message);
  }
}

long splt_u_overlap_time(splt_state *state, int splitpoint_index)
{
  int error = SPLT_OK;
  long split_value = splt_t_get_splitpoint_value(state, splitpoint_index, &error);
  long overlap_time = splt_t_get_long_option(state, SPLT_OPT_OVERLAP_TIME);
  if ((overlap_time > 0) && (split_value != LONG_MAX))
  {
    long total_time = splt_t_get_total_time(state);
    long overlapped_split_value = split_value + overlap_time;
    if (overlapped_split_value > total_time)
    {
      overlapped_split_value = total_time;
    }
    splt_t_set_splitpoint_value(state, splitpoint_index, overlapped_split_value);

    return overlapped_split_value;
  }

  return split_value;
}

//mingw does not provide BSD functions 'scandir' and 'alphasort'
#ifdef __WIN32__

//-returns -1 for not enough memory, -2 for other errors
//-a positive (or 0) number if success
int scandir(const char *dir, struct _wdirent ***namelist,
		int(*filter)(const struct _wdirent *),
		int(*compar)(const struct _wdirent **, const struct _wdirent **))
{
  struct _wdirent **files = NULL;
  struct _wdirent *file = NULL;
  _WDIR *directory = NULL;
  int number_of_files = 0;

  wchar_t *wdir = splt_u_win32_utf8_to_utf16(dir);
  directory = _wopendir(wdir);
  if (wdir) { free(wdir); wdir = NULL; }
  if (directory == NULL)
  {
    return -2;
  }

  int free_memory = 0;
  int we_have_error = 0;

  while ((file = _wreaddir(directory)))
  {
    if ((filter == NULL) || (filter(file)))
    {
      if (files == NULL)
      {
        files = malloc((sizeof *files));
      }
      else
      {
        files = realloc(files, (sizeof *files) * (number_of_files + 1));
      }
      if (files == NULL)
      {
        free_memory = 1;
        we_have_error = 1;
        break;
      }

      files[number_of_files] = malloc(sizeof(struct _wdirent));
      if (files[number_of_files] == NULL)
      {
        free_memory = 1;
        we_have_error = 1;
        break;
      }

      *files[number_of_files] = *file;
      number_of_files++;
    }
  }

  //we should have a valid 'namelist' argument
  if (namelist)
  {
    *namelist = files;
  }
  else
  {
    free_memory = 1;
  }

  //-free memory if error
  if (free_memory)
  {
    while (number_of_files--)
    {
      if (files[number_of_files])
      {
        free(files[number_of_files]);
        files[number_of_files] = NULL;
      }
    }
    free(files);
    files = NULL;
  }

  if (_wclosedir(directory) == -1)
  {
    return -2;
  }

  qsort(*namelist, number_of_files, sizeof **namelist,
      (int (*)(const void *, const void *)) compar);

  if (we_have_error)
  {
    return -1;
  }

  return number_of_files;
}

int alphasort(const struct _wdirent **a, const struct _wdirent **b)
{
  char *name_a = splt_u_win32_utf16_to_utf8((*a)->d_name);
  char *name_b = splt_u_win32_utf16_to_utf8((*b)->d_name);

  int ret = strcoll(name_a, name_b);

  if (name_a)
  {
    free(name_a);
    name_a = NULL;
  }

  if (name_b)
  {
    free(name_b);
    name_b = NULL;
  }

  return ret;
}

#endif

//result must be freed
char *splt_u_str_to_upper(const char *str, int *error)
{
  int i = 0;

  char *result = strdup(str);
  if (result == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return NULL;
  }

  for (i = 0;i < strlen(str);i++)
  {
    result[i] = toupper(str[i]);
  }

  return result;
}

int splt_u_file_is_supported_by_plugins(splt_state *state, const char *fname)
{
  splt_plugins *pl = state->plug;

  int fname_length = strlen(fname);
  if (fname_length > 3)
  {
    char *ptr_to_compare = strrchr(fname, '.');
    if (ptr_to_compare)
    {
      int i = 0;
      for (i = 0;i < pl->number_of_plugins_found;i++)
      {
        char *pl_extension = pl->data[i].info.extension;
        char *pl_upper_extension = pl->data[i].info.upper_extension;

        if ((strcmp(ptr_to_compare, pl_extension) == 0) ||
            (strcmp(ptr_to_compare, pl_upper_extension) == 0))
        {
          return SPLT_TRUE;
        }
      }
    }
  }

  return SPLT_FALSE;
}

//recursive function to go through directories
void splt_u_find_filenames(splt_state *state, const char *directory,
    char ***found_files, int *number_of_found_files, int *error)
{
#ifdef __WIN32__
  struct _wdirent **files = NULL;
#else
  struct dirent **files = NULL;
#endif

  if (! (splt_check_is_directory_and_not_symlink(directory) ||
        splt_check_is_file_and_not_symlink(state, directory)))
  {
    return;
  }


  int num_of_files = scandir(directory, &files, NULL, alphasort);
  int new_number_of_files = num_of_files;

  if (files == NULL) { return; }

  while (new_number_of_files-- > 0)
  {
#ifdef __WIN32__
    char *fname =
      splt_u_win32_utf16_to_utf8(files[new_number_of_files]->d_name);
#else
    char *fname = files[new_number_of_files]->d_name;
#endif

    int fname_size = strlen(fname);

    //avoid seg fault with strcmp(fname, "..")
    if (fname_size < 3)
    {
      continue;
    }

    if ((*error >= 0) &&
        (strcmp(fname, ".") != 0) && (strcmp(fname, "..") != 0))
    {
      int path_with_fname_size = fname_size + strlen(directory) + 2;
      char *path_with_fname = malloc(sizeof(char) * path_with_fname_size);
      if (path_with_fname == NULL)
      {
        *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
        free(files[new_number_of_files]);
        files[new_number_of_files] = NULL;
        continue;
      }

      snprintf(path_with_fname, path_with_fname_size,
          "%s%c%s", directory, SPLT_DIRCHAR, fname);

      if (splt_check_is_file_and_not_symlink(state, path_with_fname))
      {
        if (splt_u_file_is_supported_by_plugins(state, fname))
        {
          if (!(*found_files))
          {
            (*found_files) = malloc(sizeof(char *));
          }
          else
          {
            (*found_files) = realloc((*found_files),
                sizeof(char *) * ((*number_of_found_files) + 1));
          }
          if (*found_files == NULL)
          {
            *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
            goto end;
          }

          int fname_size = strlen(path_with_fname) + 1;
          (*found_files)[(*number_of_found_files)] = malloc(sizeof(char) * fname_size);
          if ((*found_files)[(*number_of_found_files)] == NULL)
          {
            *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
            goto end;
          }

          memset((*found_files)[(*number_of_found_files)], '\0', fname_size);
          strncat((*found_files)[*(number_of_found_files)], path_with_fname, fname_size);
          (*number_of_found_files)++;
        }
      }
      else if (splt_check_is_directory_and_not_symlink(path_with_fname))
      {
        splt_u_find_filenames(state, path_with_fname, found_files,
            number_of_found_files, error);
      }

end:
      if (path_with_fname)
      {
        free(path_with_fname);
        path_with_fname = NULL;
      }
    }

    free(files[new_number_of_files]);
    files[new_number_of_files] = NULL;
  }

  if (files)
  {
    free(files);
    files = NULL;
  }
}

char *splt_u_get_artist_or_performer_ptr(splt_state *state, int current_split)
{
  int tags_number = 0;
  splt_tags *tags = splt_t_get_tags(state, &tags_number);
  char *artist_or_performer = tags[current_split].artist;

  if ((tags[current_split].performer != NULL) &&
      (tags[current_split].performer[0] != '\0'))
  {
    artist_or_performer = tags[current_split].performer;
  }

  return artist_or_performer;
}

