/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2010 Alexandru Munteanu - io_fx@yahoo.fr
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
    splitpoint_value = splt_sp_get_splitpoint_value(state,i,&err);
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
        splt_e_set_error_data_from_splitpoint(state, splitpoint_value);
        splt_sp_set_splitpoint_value(state,i,total_time);
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

//checks if splitpoints are in order: 1, 5, 6 ...
//and not 1, 5, 4 ... (4<5 -> error)
void splt_check_if_splitpoints_in_order(splt_state *state, int *error)
{
  int i = 0;

  int err = SPLT_OK;

  int splitnumber = splt_t_get_splitnumber(state);
  for(i = 0; i < (splitnumber-1); i++)
  {
    long splitpoint_value = splt_sp_get_splitpoint_value(state,i,&err);
    if (err != SPLT_OK) { *error = err; return; }
    long next_splitpoint_value = splt_sp_get_splitpoint_value(state,i+1,&err);
    if (err != SPLT_OK) { *error = err; return; }

    //if we don't have EOF for the second value <=> != total_time
    if (splitpoint_value != splt_t_get_total_time(state))
    {
      //check if first splitpoint is positive
      if (splitpoint_value < 0)
      {
        splt_e_set_error_data_from_splitpoint(state, splitpoint_value);
        *error = SPLT_ERROR_NEGATIVE_SPLITPOINT;
        return;
      }

      //we take the total time and assign it to split_value
      if (splitpoint_value == LONG_MAX)
      {
        splt_sp_set_splitpoint_value(state, i, splt_t_get_total_time(state));
      }

      //check if splitpoints not in order
      if (splitpoint_value > next_splitpoint_value) 
      {
        splt_e_set_error_data_from_splitpoints(state,
            splitpoint_value, next_splitpoint_value);
        *error = SPLT_ERROR_SPLITPOINTS_NOT_IN_ORDER;
        return;
      }
      else
      {
        //check if two consecutive splitpoints are equals
        if (splitpoint_value == next_splitpoint_value)
        {
          splt_e_set_error_data_from_splitpoint(state, splitpoint_value);
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
  splt_d_print_debug(state,"Check if the new filename path is correct _%s_\n",new_filename_path);

  char current_directory[4] = { '\0' };
  snprintf(current_directory,4,"%c%c",'.',SPLT_DIRCHAR);

  //if the path equals "" or .DIRCHAR, then dont do the directory check,
  //we will split in the current directory
  if ((strcmp(new_filename_path, "") != 0) &&
      (strcmp(new_filename_path, current_directory) != 0))
  {
    if (!splt_io_check_if_directory(new_filename_path))
    {
      splt_e_set_strerr_msg(state, _("directory does not exists"));
      splt_e_set_error_data(state, new_filename_path);
      *error = SPLT_ERROR_INCORRECT_PATH;
    }
  }
}

//if the new_filename_path is "", we put the directory of
//the current song
//return NULL means 'cannot allocate memory'
//result must be freed
char *splt_check_put_dir_of_cur_song(const char *filename,
    const char *the_filename_path, int *error)
{
  //if we split in the current directory
  if (the_filename_path == NULL || the_filename_path[0] == '\0')
  {
    char *c = NULL;
    char *filename_path = NULL;
    int length_malloc = strlen(filename) + 1;
    if (length_malloc < 5)
    {
      length_malloc = 8;
    }

    if ((filename_path = malloc(length_malloc)) == NULL)
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      return NULL;
    }

    snprintf(filename_path,length_malloc, "%s", filename);
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

    return filename_path;
  }
  else
  {
    char *new_filename_path = strdup(the_filename_path);
    if (new_filename_path == NULL)
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      return NULL;
    }
    else
    {
#ifdef __WIN32__
      size_t path_len = strlen(new_filename_path);
      //erase the last char directory
      //-for windows manage c:\ because the gtk dir returns us "c:\"
      //and the normal directories without the "\"
      if (path_len > 3)
      {
        if (new_filename_path[path_len-1] == SPLT_DIRCHAR)
        {
          new_filename_path[path_len-1] = '\0';
        }
      }
#endif
    }

    return new_filename_path;
  }
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
  splt_d_print_debug(state,"Check and set correct options...\n");

  int split_mode = splt_o_get_int_option(state,SPLT_OPT_SPLIT_MODE);

  //if we have the silence option or the adjustoption
  //we set the default values if something is not ok
  if (( split_mode == SPLT_OPTION_SILENCE_MODE)
      || splt_o_get_int_option(state,SPLT_OPT_AUTO_ADJUST))
  {
    splt_o_set_int_option(state, SPLT_OPT_FRAME_MODE, SPLT_OPT_FRAME_MODE);

    //for the autoadjust or silence mode
    if  ((splt_o_get_float_option(state,SPLT_OPT_PARAM_THRESHOLD) < -96.f) || 
        (splt_o_get_float_option(state,SPLT_OPT_PARAM_THRESHOLD) > 0.f))
    {
      splt_o_set_float_option(state,SPLT_OPT_PARAM_THRESHOLD, SPLT_DEFAULT_PARAM_THRESHOLD);
    }
    if  ((splt_o_get_float_option(state,SPLT_OPT_PARAM_OFFSET) < -2.f) || 
        (splt_o_get_float_option(state,SPLT_OPT_PARAM_OFFSET) > 2.f))
    {
      splt_o_set_float_option(state,SPLT_OPT_PARAM_OFFSET, SPLT_DEFAULT_PARAM_OFFSET);
    }
    if (splt_o_get_int_option(state,SPLT_OPT_PARAM_GAP) < 0)
    {
      splt_o_set_int_option(state,SPLT_OPT_PARAM_GAP, SPLT_DEFAULT_PARAM_GAP);
    }
    if (splt_o_get_float_option(state,SPLT_OPT_PARAM_MIN_LENGTH) < 0.f)
    {
      splt_o_set_float_option(state, SPLT_OPT_PARAM_MIN_LENGTH, SPLT_DEFAULT_PARAM_MINIMUM_LENGTH);
      splt_o_set_int_option(state, SPLT_OPT_AUTO_ADJUST, SPLT_FALSE);
    }
  }

  //if we don't have adjust, set gap to 0
  if (!splt_o_get_int_option(state,SPLT_OPT_AUTO_ADJUST))
  {
    splt_o_set_int_option(state,SPLT_OPT_PARAM_GAP, 0);
  }

  //if seekable and (silence or adjust or wrap or err sync)
  if ((splt_o_get_int_option(state,SPLT_OPT_INPUT_NOT_SEEKABLE)) &&
      (splt_o_get_int_option(state,SPLT_OPT_AUTO_ADJUST) ||
       (split_mode == SPLT_OPTION_SILENCE_MODE) ||
       (split_mode == SPLT_OPTION_ERROR_MODE) ||
       (split_mode == SPLT_OPTION_WRAP_MODE)))
  {
    splt_o_set_int_option(state, SPLT_OPT_INPUT_NOT_SEEKABLE, SPLT_FALSE);
  }
}

/****************************/
/* file type check */

//checks if mp3 file or ogg file
//returns possible error
void splt_check_file_type(splt_state *state, int *error)
{
  int err = SPLT_OK;

  splt_d_print_debug(state,"Detecting file format...\n");
  const char *filename = splt_t_get_filename_to_split(state);

  splt_d_print_debug(state,"Checking the format of _%s_\n", filename);

  //parse each plugin until we find out a plugin for the file
  splt_plugins *pl = state->plug;
  int i = 0;
  int plugin_found = SPLT_FALSE;
  for (i = 0;i < pl->number_of_plugins_found;i++)
  {
    splt_p_set_current_plugin(state, i);
    err = SPLT_OK;

    if (splt_o_get_int_option(state, SPLT_OPT_INPUT_NOT_SEEKABLE) &&
        ! splt_io_input_is_stdin(state))
    {
      const char *extension = splt_p_get_extension(state, &err);
      const char *upper_extension = splt_p_get_extension(state, &err);
      if (err == SPLT_OK)
      {
        if (splt_su_str_ends_with(filename, extension) ||
            splt_su_str_ends_with(filename, upper_extension))
        {
          plugin_found = SPLT_TRUE;
          break;
        }
      }
    }
    else
    {
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
  }

  if (! plugin_found)
  {
    splt_e_set_error_data(state, filename);
    *error = SPLT_ERROR_NO_PLUGIN_FOUND_FOR_FILE;
    splt_d_print_debug(state,"No plugin found !\n");
    splt_d_print_debug(state,"Verifying if the file _%s_ is a file ...\n", filename);

    FILE *test = NULL;
    if ((test = splt_io_fopen(filename,"r")) != NULL)
    {
      if (fclose(test) != 0)
      {
        splt_e_set_strerror_msg(state);
        splt_e_set_error_data(state, filename);
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
      splt_e_set_strerror_msg(state);
      splt_e_set_error_data(state,filename);
      *error = SPLT_ERROR_CANNOT_OPEN_FILE;
      return;
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
      splt_e_set_strerror_msg(state);
      splt_e_set_error_data(state, file1);
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
      splt_e_set_strerror_msg(state);
      splt_e_set_error_data(state, file2);
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

  splt_d_print_debug(state,"Checking if _%s_ is like _%s_ \n", file1, file2);
 
  int is_file1 = splt_io_check_if_file(state, file1);
  int is_file2 = splt_io_check_if_file(state, file2);
  if (is_file1 && is_file2)
  {
    //file1
    if ((file1_ = splt_io_fopen(file1,"r")) == NULL)
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
        if ((file2_ = splt_io_fopen(file2,"r")) == NULL)
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
              splt_e_set_strerror_msg(state);
              splt_e_set_error_data(state, file2);
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
          splt_e_set_strerror_msg(state);
          splt_e_set_error_data(state, file1);
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

end_error:
  close_files(state, file1, &file1_,file2, &file2_,error);

  return SPLT_FALSE;
}

