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

#include <sys/stat.h>
#include <string.h>

#include "splt.h"

#ifdef __WIN32__
#include "windows.h"
#include "io.h"
#endif

/****************************/
/* splitpoints checks */

//checks if the splitpoints are inferior to the length of the song
//remove all the splitpoint superior than the biggest one superior
//to the end of file
void splt_check_splitpts_inf_song_length(splt_state *state, int *error)
{
  int i = 0;
  int found_max_splitpoint = SPLT_FALSE;
  int initial_splitpoints = state->split.splitnumber;

  int err = SPLT_OK;

  long splitpoint_value = 0;
  long total_time = 0;
  //check if splitpoints > total time
  for(i = 0; i < initial_splitpoints; i++)
  {
    splitpoint_value = splt_t_get_splitpoint_value(state,i,&err);
    if (err != SPLT_OK)
    {
      total_time = splt_t_get_total_time(state);
      //if the value is superior to the total time
      if (splitpoint_value > total_time)
      {
        found_max_splitpoint = SPLT_TRUE;
        splt_t_set_splitnumber(state, i+1);
        //we have an error only if different from the
        //total time
        *error = SPLT_SPLITPOINT_BIGGER_THAN_LENGTH;
        splt_t_set_error_data_from_splitpoint(state, splitpoint_value);
        splt_t_set_splitpoint_value(state,i,total_time);
        break;
      }
    }
    else
    {
      *error = err;
      break;
    }
  }
}

//checks if splitpoints are in order : 1, 5, 6 ...
//and not 1, 5, 4 ... (4<5 -> error)
void splt_check_if_splitpoints_in_order(splt_state *state, int *error)
{
  int i = 0;

  int err = SPLT_OK;

  int splitnumber = splt_t_get_splitnumber(state);
  for(i = 0; i < (splitnumber-1); i++)
  {
    long splitpoint_value = splt_t_get_splitpoint_value(state,i,&err);
    if (err != SPLT_OK) { *error = err; return; }
    long next_splitpoint_value = splt_t_get_splitpoint_value(state,i+1,&err);
    if (err != SPLT_OK) { *error = err; return; }

    //if we don't have EOF for the second value <=> != total_time
    if (splitpoint_value != splt_t_get_total_time(state))
    {
      //check if first splitpoint is positive
      if (splitpoint_value < 0)
      {
        splt_t_set_error_data_from_splitpoint(state, splitpoint_value);
        *error = SPLT_ERROR_NEGATIVE_SPLITPOINT;
        return;
      }

      //we take the total time and assign it to split_value
      if (splitpoint_value == LONG_MAX)
      {
        splt_t_set_splitpoint_value(state,i,splt_t_get_total_time(state));
      }

      //check if splitpoints not in order
      if (splitpoint_value > next_splitpoint_value) 
      {
        splt_t_set_error_data_from_splitpoints(state,
            splitpoint_value, next_splitpoint_value);
        *error = SPLT_ERROR_SPLITPOINTS_NOT_IN_ORDER;
        return;
      }
      else
      {
        //check if two consecutive splitpoints are equals
        if (splitpoint_value == next_splitpoint_value)
        {
          splt_t_set_error_data_from_splitpoint(state, splitpoint_value);
          *error = SPLT_ERROR_EQUAL_SPLITPOINTS;
          return;
        }
      }
    }
  }
}

/****************************/
/* path check */

//checks if the filename path is correct
void splt_check_if_new_filename_path_correct(splt_state *state,
    const char *new_filename_path, int *error)
{
  splt_u_print_debug("We check if the new filename path is correct ",0,new_filename_path);

  char current_directory[4] = { '\0' };
  snprintf(current_directory,4,"%c%c",'.',SPLT_DIRCHAR);

  //if the path equals "" or .DIRCHAR, then dont do the directory check,
  //we will split in the current directory
  if ((strcmp(new_filename_path, "") != 0) &&
      (strcmp(new_filename_path, current_directory) != 0))
  {
    //used to see if the file exists
    struct stat buffer;
    int         status;

    //-1 means error
    if((status = stat(new_filename_path, &buffer)) == -1)
    {
      splt_t_set_strerror_msg(state);
      splt_t_set_error_data(state, new_filename_path);
      *error = SPLT_ERROR_INCORRECT_PATH;
    }
    else
    {
      //if it is a directory
      if (S_ISDIR(buffer.st_mode) != 0)
      {
        //no error
        return;
      }
      else
      {
        splt_t_set_strerr_msg(state,"Directory does not exists");
        splt_t_set_error_data(state, new_filename_path);
        *error = SPLT_ERROR_INCORRECT_PATH;
      }
    }
  }
}

//if the new_filename_path is "", we put the directory of
//the current song
//return NULL means 'cannot allocate memory'
//result must be freed
char *splt_check_put_dir_of_cur_song(const char *filename,
    const char *the_filename_path)
{
  char *orig_filename = strdup(filename);
  if (!orig_filename)
  {
    return NULL;
  }

  char *c = NULL;
  char *filename_path = NULL;
  int length_malloc = strlen(orig_filename) + 1;
  if (length_malloc < 5)
  {
    length_malloc = 8;
  }

  if((filename_path = malloc(length_malloc)) == NULL)
  {
    free(orig_filename);
    orig_filename = NULL;
    return NULL;
  }

  int cur_dir = SPLT_FALSE;
  //if new filename path is null
  if (the_filename_path == NULL)
  {
    cur_dir = SPLT_TRUE;
  }
  else
  {
    //if new_filename_path is ""
    if (the_filename_path[0] == '\0')
    {
      cur_dir = SPLT_TRUE;
    }
  }

  //if we split in the current directory
  if (cur_dir)
  {
    snprintf(filename_path,length_malloc, "%s", orig_filename);
    c = strrchr(filename_path,SPLT_DIRCHAR);
    //if we found a dirchar, erase everythig after to get the path
    if (c != NULL)
    {
      *c = '\0';
    }
    //otherwise we set the path as ""
    else
    {
      filename_path[0] = '\0';
    }

    free(orig_filename);
    orig_filename = NULL;
    return filename_path;
  }

  //free memory
  if (filename_path)
  {
    free(filename_path);
    filename_path = NULL;
  }
  if (orig_filename)
  {
    free(orig_filename);
    orig_filename = NULL;
  }

  char *new_filename_path = strdup(the_filename_path);
  if (new_filename_path == NULL)
  {
    return NULL;
  }

  if (new_filename_path != NULL)
  {
    size_t path_len = strlen(new_filename_path);
    //erase the last char directory
    //-for windows manage c:\ because the gtk dir returns us "c:\"
    //and the normal directories without the "\"
    //-for unix, erase '/'
    if (new_filename_path[path_len-1] == SPLT_DIRCHAR)
    {
      new_filename_path[path_len-1] = '\0';
    }
  }

  return new_filename_path;
}

/****************************/
/* options check */

//checks if the options are compatible. 
//the implementation of this function is not essential
int splt_check_compatible_options(splt_state *state)
{
  
  return SPLT_TRUE;
}

//checks if we have incorrect options and sets the correct options if
//we need so (this function is quite important)
void splt_check_set_correct_options(splt_state *state)
{
  splt_u_print_debug("We check and set correct options.. ",0,NULL);

  int split_mode = splt_t_get_int_option(state,SPLT_OPT_SPLIT_MODE);

  //if we have the silence option or the adjustoption
  //we set the default values if something is not ok
  if (( split_mode == SPLT_OPTION_SILENCE_MODE)
      || splt_t_get_int_option(state,SPLT_OPT_AUTO_ADJUST))
  {
    //if we have the adjust option or the silence option,
    //enable frame mode by default
    splt_t_set_int_option(state,SPLT_OPT_FRAME_MODE, SPLT_TRUE);

    //for the autoadjust or silence mode
    if  ((splt_t_get_float_option(state,SPLT_OPT_PARAM_THRESHOLD) < -96.f) || 
        (splt_t_get_float_option(state,SPLT_OPT_PARAM_THRESHOLD) > 0.f))
    {
      splt_t_set_float_option(state,SPLT_OPT_PARAM_THRESHOLD,
          SPLT_DEFAULT_PARAM_THRESHOLD);
    }
    if  ((splt_t_get_float_option(state,SPLT_OPT_PARAM_OFFSET) < -2.f) || 
        (splt_t_get_float_option(state,SPLT_OPT_PARAM_OFFSET) > 2.f))
    {
      splt_t_set_float_option(state,SPLT_OPT_PARAM_OFFSET,
          SPLT_DEFAULT_PARAM_OFFSET);
    }
    //for the adjust mode
    if (splt_t_get_int_option(state,SPLT_OPT_PARAM_GAP) < 0)
    {
      splt_t_set_int_option(state,SPLT_OPT_PARAM_GAP, SPLT_DEFAULT_PARAM_GAP);
    }
    //for the silence mode
    if (splt_t_get_float_option(state,SPLT_OPT_PARAM_MIN_LENGTH) < 0.f)
    {
      splt_t_set_float_option(state,SPLT_OPT_PARAM_MIN_LENGTH,
          SPLT_DEFAULT_PARAM_MINIMUM_LENGTH);
      //disable auto adjust mode
      splt_t_set_int_option(state,SPLT_OPT_AUTO_ADJUST,
          SPLT_FALSE);
    }
  }

  //if we don't have adjust, set gap to 0
  if (!splt_t_get_int_option(state,SPLT_OPT_AUTO_ADJUST))
  {
    splt_t_set_int_option(state,SPLT_OPT_PARAM_GAP, 0);
  }

  //if seekable and (silence or adjust or wrap or err sync)
  if ((splt_t_get_int_option(state,SPLT_OPT_INPUT_NOT_SEEKABLE)) &&
      (splt_t_get_int_option(state,SPLT_OPT_AUTO_ADJUST) ||
       (split_mode == SPLT_OPTION_SILENCE_MODE) ||
       (split_mode == SPLT_OPTION_ERROR_MODE) ||
       (split_mode == SPLT_OPTION_WRAP_MODE)))
  {
    splt_t_set_int_option(state,SPLT_OPT_INPUT_NOT_SEEKABLE, SPLT_FALSE);
  }
}

/****************************/
/* file type check */

//checks if mp3 file or ogg file
//returns possible error
void splt_check_file_type(splt_state *state, int *error)
{
  int err = SPLT_OK;

  splt_u_print_debug("Detecting file format...",0,NULL);
  char *filename = splt_t_get_filename_to_split(state);

  splt_u_print_debug("Checking the format of",0,filename);

  //parse each plugin until we find out a plugin for the file
  splt_plugins *pl = state->plug;
  int i = 0;
  int plugin_found = SPLT_FALSE;
  for (i = 0;i < pl->number_of_plugins_found;i++)
  {
    splt_t_set_current_plugin(state, i);
    err = SPLT_OK;
    if (splt_p_check_plugin_is_for_file(state, &err))
    {
      if (err == SPLT_OK)
      {
        //here, plugin found
        plugin_found = SPLT_TRUE;
        break;
      }
    }
  }
  if (! plugin_found)
  {
    splt_t_set_error_data(state, filename);
    *error = SPLT_ERROR_NO_PLUGIN_FOUND_FOR_FILE;
    splt_u_print_debug("No plugin found !",0,NULL);

    //if no plugin was found,
    //verify if the file is a real file
    splt_u_print_debug("Verify if the file is a file",0,filename);

    FILE *test = NULL;
    if ((test = fopen(filename,"r")) != NULL)
    {
      if (fclose(test) != 0)
      {
        splt_t_set_strerror_msg(state);
        splt_t_set_error_data(state, filename);
        *error = SPLT_ERROR_CANNOT_CLOSE_FILE;
        return;
      }
      else
      {
        test = NULL;
      }
    }
    else
    {
      splt_t_set_strerror_msg(state);
      splt_t_set_error_data(state,filename);
      *error = SPLT_ERROR_CANNOT_OPEN_FILE;
      return;
    }
  }
  else
  {
    err = SPLT_OK;
    const char *temp = splt_p_get_name(state,&err);
    if (err != SPLT_OK)
    {
      char infos[2048] = { '\0' };
      snprintf(infos,2047," info: file matches the plugin '%s'\n",temp);
      splt_t_put_message_to_client(state, infos);
    }
    else
    {
      *error = err;
      return;
    }
  }
}

//check if its a file
//-we are not interested in errors
int splt_check_is_file(splt_state *state, const char *fname)
{
  struct stat buffer;
  int status = 0;

  if (fname == NULL)
  {
    return SPLT_FALSE;
  }
  else
  {
    //stdin : consider as file
    if (fname[strlen(fname)-1] == '-')
    {
      return SPLT_TRUE;
    }
    //not stdin :
    status = stat(fname, &buffer);
    if (status == 0)
    {
      //if it is a file
      if (S_ISREG(buffer.st_mode))
      {
        return SPLT_TRUE;
      }
      else
      {
        splt_t_set_strerror_msg(state);
        splt_t_set_error_data(state, fname);
        return SPLT_FALSE;
      }
    }
    else
    {
      splt_t_set_strerror_msg(state);
      splt_t_set_error_data(state, fname);
      return SPLT_FALSE;
    }
  }
}

//close two filehandles
static void close_files(splt_state *state, const char *file1, FILE **f1,
    const char *file2, FILE **f2, int *error)
{
  if (*f1)
  {
    if (fclose(*f1) != 0)
    {
      splt_t_set_strerror_msg(state);
      splt_t_set_error_data(state, file1);
      *error = SPLT_ERROR_CANNOT_CLOSE_FILE;
    }
    else
    {
      *f1 = NULL;
    }
  }
  if (*f2)
  {
    if (fclose(*f2) != 0)
    {
      splt_t_set_strerror_msg(state);
      splt_t_set_error_data(state, file2);
      *error = SPLT_ERROR_CANNOT_CLOSE_FILE;
    }
    else
    {
      *f2 = NULL;
    }
  }
}

//check if file1 = file2
//-we are not interested in getting the errors of fopen or fstat
int splt_check_is_the_same_file(splt_state *state, const char *file1,
    const char *file2, int *error)
{
  FILE *file1_ = NULL;
  FILE *file2_ = NULL;

  //stdin
  if (file1[strlen(file1)-1] == '-')
  {
    return SPLT_FALSE;
  }

  splt_u_print_debug("Checking if this file :",0,file1);
  splt_u_print_debug("is like this file :",0,file2);
 
  int is_file1 = splt_check_is_file(state, file1);
  int is_file2 = splt_check_is_file(state, file2);
  if (is_file1 && is_file2)
  {
    //file1
    if ((file1_ = fopen(file1,"r")) == NULL)
    {
      goto end_error;
    }
    else
    {
      int file1_d = fileno(file1_);
      struct stat file1_stat;
      if (fstat(file1_d,&file1_stat) == 0)
      {
        //file2
        if ((file2_ = fopen(file2,"r")) == NULL)
        {
          goto end_error;
        }
        else
        {
          //mingw for windows returns 0 as inode and 0 as device
          //when using the fstat function.
#ifdef __WIN32__
          //file information structures
          BY_HANDLE_FILE_INFORMATION handle_info_file1;
          BY_HANDLE_FILE_INFORMATION handle_info_file2;
          int file1_d = fileno(file1_);
          int file2_d = fileno(file2_);

          //we get the file information for the file1
          if (!GetFileInformationByHandle((HANDLE)_get_osfhandle(file1_d), &handle_info_file1))
          {
            goto end_error;
          }
          //we get the file information for the file2
          if (!GetFileInformationByHandle((HANDLE)_get_osfhandle(file2_d), &handle_info_file2))
          {
            goto end_error;
          }
          //if the files have the same indexes, we have the same files
          if ((handle_info_file1.nFileIndexHigh == handle_info_file2.nFileIndexHigh)&&
              (handle_info_file1.nFileIndexLow == handle_info_file2.nFileIndexLow))
          {
            close_files(state, file1, &file1_,file2, &file2_,error);
            return SPLT_TRUE;
          }
#else
          int file2_d = fileno(file2_);
          struct stat file2_stat;
          if (fstat(file2_d,&file2_stat) == 0)
          {
            //compare file1 with file2
            if ((file1_stat.st_ino == file2_stat.st_ino) &&
                (file1_stat.st_dev == file2_stat.st_dev))
            {
              close_files(state, file1, &file1_,file2, &file2_,error);
              return SPLT_TRUE;
            }
            else
            {
              close_files(state, file1, &file1_,file2, &file2_,error);
              return SPLT_FALSE;
            }
          }
          else
          {
            goto end_error;
          }
#endif
          //close the second file
          if (file2_)
          {
            if (fclose(file2_) != 0)
            {
              splt_t_set_strerror_msg(state);
              splt_t_set_error_data(state, file2);
              *error = SPLT_ERROR_CANNOT_CLOSE_FILE;
            }
            else
            {
              file2_ = NULL;
            }
          }
        }
      }
      else
      {
        goto end_error;
      }
      //close the first file
      if (file1_)
      {
        if (fclose(file1_) != 0)
        {
          splt_t_set_strerror_msg(state);
          splt_t_set_error_data(state, file1);
          *error = SPLT_ERROR_CANNOT_CLOSE_FILE;
        }
        else
        {
          file1_ = NULL;
        }
      }
    }
  }
  else
  {
    return SPLT_FALSE;
  }

end:
  return SPLT_FALSE;

end_error:
  /*splt_t_set_strerror_msg(state);
  splt_t_set_error_data(state,file1);
  *error = SPLT_ERROR_CANNOT_OPEN_FILE;*/
  close_files(state, file1, &file1_,file2, &file2_,error);
  return SPLT_FALSE;
}

