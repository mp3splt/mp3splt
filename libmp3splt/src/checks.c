/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2011 Alexandru Munteanu - io_fx@yahoo.fr
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

static void close_files(splt_state *state, const char *file1, FILE **f1,
    const char *file2, FILE **f2, int *error);

void splt_check_points_inf_song_length_and_convert_negatives(splt_state *state, int *error)
{
  if (splt_io_input_is_stdin(state))
  {
    return;
  }

  int splitnumber = splt_t_get_splitnumber(state);
  if (splitnumber < 1)
  {
    return;
  }

  int err = SPLT_OK;
  long total_time = splt_t_get_total_time(state);
  if (total_time == 0)
  {
    return;
  }

  int i = 0;
  for (i = 0; i < splitnumber; i++)
  {
    long splitpoint_value = splt_sp_get_splitpoint_value(state, i, &err);
    if (err < SPLT_OK) { *error = err; return; }

    if (splitpoint_value < 0)
    {
      splitpoint_value = total_time + splitpoint_value;
      splt_sp_set_splitpoint_value(state, i, splitpoint_value);
    }

    /*if (splitpoint_value == LONG_MAX)
    {
      splitpoint_value = total_time;
      splt_sp_set_splitpoint_value(state, i, total_time);
      continue;
    }

    if (splitpoint_value > total_time)
    {
      splt_t_set_splitnumber(state, i+1);

      *error = SPLT_SPLITPOINT_BIGGER_THAN_LENGTH;
      splt_e_set_error_data_from_splitpoint(state, splitpoint_value);
      splt_sp_set_splitpoint_value(state, i, total_time);

      break;
    }*/
  }
}

void splt_check_if_points_in_order(splt_state *state, int *error)
{
  int splitnumber = splt_t_get_splitnumber(state);
  if (splitnumber < 1)
  {
    return;
  }

  int err = SPLT_OK;
  int i = 0;
  for (i = 0; i < (splitnumber-1); i++)
  {
    long splitpoint_value = splt_sp_get_splitpoint_value(state, i, &err);
    if (err != SPLT_OK) { *error = err; return; }
    long next_splitpoint_value = splt_sp_get_splitpoint_value(state, i+1, &err);
    if (err != SPLT_OK) { *error = err; return; }

    if (splitpoint_value < 0)
    {
      splt_e_set_error_data_from_splitpoint(state, splitpoint_value);
      *error = SPLT_ERROR_NEGATIVE_SPLITPOINT;
      return;
    }

    if (next_splitpoint_value < 0)
    {
      splt_e_set_error_data_from_splitpoint(state, next_splitpoint_value);
      *error = SPLT_ERROR_NEGATIVE_SPLITPOINT;
      return;
    }

    if (splitpoint_value > next_splitpoint_value) 
    {
      splt_e_set_error_data_from_splitpoints(state, splitpoint_value, next_splitpoint_value);
      *error = SPLT_ERROR_SPLITPOINTS_NOT_IN_ORDER;
      return;
    }

    if (splitpoint_value == next_splitpoint_value)
    {
      splt_e_set_error_data_from_splitpoint(state, splitpoint_value);
      *error = SPLT_ERROR_EQUAL_SPLITPOINTS;
      return;
    }
  }
}

void splt_check_if_fname_path_is_correct(splt_state *state,
    const char *fname_path, int *error)
{
  splt_d_print_debug(state,"Check if the new filename path is correct _%s_\n",fname_path);

  char current_directory[4] = { '\0' };
  snprintf(current_directory, 4, "%c%c", '.', SPLT_DIRCHAR);

  if ((strcmp(fname_path, "") != 0) &&
      (strcmp(fname_path, current_directory) != 0))
  {
    if (!splt_io_check_if_directory(fname_path))
    {
      splt_e_set_strerr_msg_with_data(state,
          _("directory does not exists"), fname_path);
      *error = SPLT_ERROR_INCORRECT_PATH;
    }
  }
}

char *splt_check_put_dir_of_cur_song(const char *filename,
    const char *path, int *error)
{
  int err = SPLT_OK;
  char *filename_path = NULL;

  if (path == NULL || path[0] == '\0')
  {
    err = splt_su_copy(filename, &filename_path);
    if (err < 0) { *error = err; return NULL; }

    char *c = strrchr(filename_path, SPLT_DIRCHAR);
    if (c == NULL)
    {
      filename_path[0] = '\0';
      return filename_path;
    }

    *(c+1) = '\0';
    return filename_path;
  }

  err = splt_su_copy(path, &filename_path);
  if (err < 0) { *error = err; return NULL; }

  return filename_path;
}

int splt_check_compatible_options(splt_state *state)
{
  return SPLT_TRUE;
}

void splt_check_set_correct_options(splt_state *state)
{
  splt_d_print_debug(state,"Check and set correct options...\n");

  int split_mode = splt_o_get_int_option(state,SPLT_OPT_SPLIT_MODE);

  if ((split_mode == SPLT_OPTION_SILENCE_MODE)
      || (split_mode == SPLT_OPTION_TRIM_SILENCE_MODE)
      || splt_o_get_int_option(state,SPLT_OPT_AUTO_ADJUST))
  {
    splt_o_set_int_option(state, SPLT_OPT_FRAME_MODE, SPLT_OPT_FRAME_MODE);

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

    if (splt_o_get_float_option(state, SPLT_OPT_PARAM_MIN_TRACK_LENGTH) < 0.f)
    {
      splt_o_set_float_option(state, SPLT_OPT_PARAM_MIN_TRACK_LENGTH,
          SPLT_DEFAULT_PARAM_MINIMUM_TRACK_LENGTH);
    }
  }

  if (!splt_o_get_int_option(state,SPLT_OPT_AUTO_ADJUST))
  {
    splt_o_set_int_option(state,SPLT_OPT_PARAM_GAP, 0);
  }

  if ((splt_o_get_int_option(state,SPLT_OPT_INPUT_NOT_SEEKABLE)) &&
      (splt_o_get_int_option(state,SPLT_OPT_AUTO_ADJUST) ||
       (split_mode == SPLT_OPTION_SILENCE_MODE) ||
       (split_mode == SPLT_OPTION_TRIM_SILENCE_MODE) ||
       (split_mode == SPLT_OPTION_ERROR_MODE) ||
       (split_mode == SPLT_OPTION_WRAP_MODE)))
  {
    splt_o_set_int_option(state, SPLT_OPT_INPUT_NOT_SEEKABLE, SPLT_FALSE);
  }
}

void splt_check_file_type(splt_state *state, int *error)
{
  int err = SPLT_OK;

  splt_d_print_debug(state,"Detecting file format...\n");
  const char *filename = splt_t_get_filename_to_split(state);

  splt_d_print_debug(state,"Checking the format of _%s_\n", filename);

  splt_plugins *pl = state->plug;
  int plugin_found = SPLT_FALSE;
  int i = 0;
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
    if ((test = splt_io_fopen(filename,"r")) == NULL)
    {
      splt_e_set_strerror_msg_with_data(state, filename);
      *error = SPLT_ERROR_CANNOT_OPEN_FILE;
      return;
    }

    if (fclose(test) != 0)
    {
      splt_e_set_strerror_msg_with_data(state, filename);
      *error = SPLT_ERROR_CANNOT_CLOSE_FILE;
      return;
    }
  }
}

int splt_check_is_the_same_file(splt_state *state, const char *file1,
    const char *file2, int *error)
{
  if (file1 == NULL || file2 == NULL)
  {
    return SPLT_FALSE;
  }

  FILE *file1_ = NULL;
  FILE *file2_ = NULL;

  if (file1[strlen(file1) - 1] == '-')
  {
    return SPLT_FALSE;
  }

  splt_d_print_debug(state,"Checking if _%s_ is like _%s_ \n", file1, file2);

  int is_file1 = splt_io_check_if_file(state, file1);
  int is_file2 = splt_io_check_if_file(state, file2);
  if (!is_file1 || !is_file2)
  {
    return SPLT_FALSE;
  }

  if ((file1_ = splt_io_fopen(file1,"r")) == NULL)
  {
    goto end;
  }

  if ((file2_ = splt_io_fopen(file2,"r")) == NULL)
  {
    goto end;
  }

  //mingw for windows returns 0 as inode and 0 as device
  //when using the fstat function.
#ifdef __WIN32__
  BY_HANDLE_FILE_INFORMATION handle_info_file1;
  BY_HANDLE_FILE_INFORMATION handle_info_file2;
  int file1_d = fileno(file1_);
  int file2_d = fileno(file2_);

  if (!GetFileInformationByHandle((HANDLE)_get_osfhandle(file1_d), &handle_info_file1))
  {
    goto end;
  }
  if (!GetFileInformationByHandle((HANDLE)_get_osfhandle(file2_d), &handle_info_file2))
  {
    goto end;
  }

  if ((handle_info_file1.nFileIndexHigh == handle_info_file2.nFileIndexHigh)&&
      (handle_info_file1.nFileIndexLow == handle_info_file2.nFileIndexLow))
  {
    close_files(state, file1, &file1_,file2, &file2_,error);
    return SPLT_TRUE;
  }
#else
  int file1_d = fileno(file1_);
  struct stat file1_stat;
  if (fstat(file1_d, &file1_stat) != 0)
  {
    goto end;
  }

  int file2_d = fileno(file2_);
  struct stat file2_stat;
  if (fstat(file2_d,&file2_stat) != 0)
  {
    goto end;
  }

  if ((file1_stat.st_ino == file2_stat.st_ino) &&
      (file1_stat.st_dev == file2_stat.st_dev))
  {
    close_files(state, file1, &file1_,file2, &file2_,error);
    return SPLT_TRUE;
  }
#endif

end:
  close_files(state, file1, &file1_,file2, &file2_,error);

  return SPLT_FALSE;
}

static void close_files(splt_state *state, const char *file1, FILE **f1,
    const char *file2, FILE **f2, int *error)
{
  if (*f1)
  {
    if (fclose(*f1) != 0)
    {
      splt_e_set_strerror_msg_with_data(state, file1);
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
      splt_e_set_strerror_msg_with_data(state, file2);
      *error = SPLT_ERROR_CANNOT_CLOSE_FILE;
    }
    else
    {
      *f2 = NULL;
    }
  }
}

