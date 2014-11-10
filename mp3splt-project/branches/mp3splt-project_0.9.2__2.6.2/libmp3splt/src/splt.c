/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2014 Alexandru Munteanu - m@ioalex.net
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 *
 *********************************************************/

/*! \file

Actually split the input file
*/

#include <sys/stat.h>
#include <string.h>
#include <math.h>

#include "splt.h"

static splt_code splt_s_open_full_log_filename(splt_state *state);
static void splt_s_close_full_log_filename(splt_state *state);

/****************************/
/*! normal split */

static long splt_s_real_split(double splt_beg, double splt_end, 
    int save_end_point, int *error, splt_state *state)
{
  char *final_fname = splt_su_get_fname_with_path_and_extension(state,error);
  long new_end_point = splt_co_time_to_long_ceil(splt_end);

  if (*error >= 0)
  {
    double new_sec_end_point = 
      splt_p_split(state, final_fname, splt_beg, splt_end, error, save_end_point);

    if (new_sec_end_point == -1.0)
    {
      new_end_point = LONG_MAX;
    }
    else
    {
      new_end_point = splt_co_time_to_long_ceil(new_sec_end_point);
    }

    if (*error >= 0)
    {
      splt_c_update_progress(state,1.0,1.0,1,1,1);

      int err = SPLT_OK;
      err = splt_c_put_split_file(state, final_fname);
      if (err < 0) { *error = err; }
    }
  }

  if (final_fname)
  {
    free(final_fname);
    final_fname = NULL;
  }

  return new_end_point;
}

//! Extract the file portion between two split points
static long splt_s_split(splt_state *state, int first_splitpoint,
    int second_splitpoint, int *error)
{
  int get_error = SPLT_OK;
  long split_begin = splt_sp_get_splitpoint_value(state, first_splitpoint, &get_error);
  long split_end = splt_sp_get_splitpoint_value(state, second_splitpoint, &get_error);

  long new_end_point = split_end;

  int save_end_point = SPLT_TRUE;
  if (splt_sp_get_splitpoint_type(state, second_splitpoint, &get_error) == SPLT_SKIPPOINT ||
      splt_o_get_long_option(state, SPLT_OPT_OVERLAP_TIME) > 0)
  {
    save_end_point = SPLT_FALSE;
  }

  if (get_error == SPLT_OK)
  {
    //if no error
    if (*error >= 0)
    {
      //if the first splitpoint different than the end point
      if (split_begin != split_end)
      {
        //convert to float for hundredth
        // 34.6  --> 34 seconds and 6 hundredth
        double splt_beg = split_begin / 100;
        splt_beg += ((split_begin % 100) / 100.);
        double splt_end = 0;

        //LONG_MAX == EOF
        if (split_end == LONG_MAX)
        {
          splt_end = -1;
        }
        else
        {
          splt_end = split_end / 100;
          splt_end += ((split_end % 100) / 100.);
        }

        new_end_point = splt_s_real_split(splt_beg, splt_end, save_end_point, error, state);
      }
      else
      {
        splt_e_set_error_data_from_splitpoint(state, split_begin);
        *error = SPLT_ERROR_EQUAL_SPLITPOINTS;
      }
    }
  }
  else
  {
    *error = get_error;
  }

  return new_end_point;
}

//!splits the file with multiple points
void splt_s_multiple_split(splt_state *state, int *error)
{
  int split_type = splt_o_get_int_option(state, SPLT_OPT_SPLIT_MODE);
  int err = SPLT_OK;

  splt_of_set_oformat_digits(state);

  if (split_type == SPLT_OPTION_NORMAL_MODE)
  {
    splt_c_put_info_message_to_client(state, _(" info: starting normal split\n"));
  }

  splt_u_print_overlap_time(state);

  int get_error = SPLT_OK;

  splt_array *new_end_points = splt_array_new();

  int i = 0;
  int number_of_splitpoints = splt_t_get_splitnumber(state);

  int save_end_point = SPLT_TRUE;
  if (splt_o_get_long_option(state, SPLT_OPT_OVERLAP_TIME) > 0)
  {
    save_end_point = SPLT_FALSE;
  }

  while (i  < number_of_splitpoints - 1)
  {
    splt_t_set_current_split(state, i);

    if (!splt_t_split_is_canceled(state))
    {
      get_error = SPLT_OK;

      int first_splitpoint_type = splt_sp_get_splitpoint_type(state, i, &get_error);
      if (first_splitpoint_type == SPLT_SKIPPOINT)
      {
        splt_d_print_debug(state, "SKIP splitpoint at _%d_\n", i);
        i++;
        continue;
      }

      splt_tu_auto_increment_tracknumber(state);

      long saved_end_point = splt_sp_get_splitpoint_value(state, i+1, &get_error);
      splt_sp_overlap_time(state, i + 1);

      err = splt_u_finish_tags_and_put_output_format_filename(state, i);
      if (err < 0) { *error = err; goto end; }

      int end_point_index = i+1;
      long new_end_point = splt_s_split(state, i, end_point_index, error);

      if (save_end_point)
      {
        splt_il_pair *index_end_point = splt_il_pair_new(end_point_index, new_end_point);
        splt_array_append(new_end_points, (void *)index_end_point);
      }

      splt_sp_set_splitpoint_value(state, i + 1, saved_end_point);

      if ((*error < 0) || (*error == SPLT_OK_SPLIT_EOF))
      {
        break;
      }
    }
    else
    {
      *error = SPLT_SPLIT_CANCELLED;
      goto end;
    }

    i++;
  }

end:
  for (i = 0;i < splt_array_length(new_end_points);i++)
  {
    splt_il_pair *index_end_point = (splt_il_pair *) splt_array_get(new_end_points, i);

    splt_sp_set_splitpoint_value(state,
        splt_il_pair_first(index_end_point),
        splt_il_pair_second(index_end_point));

    splt_il_pair_free(&index_end_point);
  }

  splt_array_free(&new_end_points);
}

void splt_s_normal_split(splt_state *state, int *error)
{
  int output_filenames = splt_o_get_int_option(state,SPLT_OPT_OUTPUT_FILENAMES);
  if (output_filenames == SPLT_OUTPUT_DEFAULT)
  {
    splt_of_set_oformat(state, SPLT_DEFAULT_OUTPUT, error, SPLT_TRUE);
    if (*error < 0) { return; }
  }

  splt_s_multiple_split(state, error);
}

//!the sync error mode
void splt_s_error_split(splt_state *state, int *error)
{
  splt_c_put_info_message_to_client(state, _(" info: starting error mode split\n"));
  char *final_fname = NULL;

  //we detect sync errors
  splt_p_search_syncerrors(state, error);

  //automatically set progress callback to 100% after
  //the error detection
  splt_c_update_progress(state,1.0,1.0,1,1,1);

  int err = SPLT_OK;

  //if no error
  if (*error >= 0)
  {
    //we put the number of sync errors
    splt_t_set_splitnumber(state, state->serrors->serrors_points_num - 1);

    splt_of_set_oformat_digits(state);

    if (splt_o_get_int_option(state,SPLT_OPT_OUTPUT_FILENAMES) == SPLT_OUTPUT_DEFAULT)
    {
      splt_of_set_oformat(state, SPLT_DEFAULT_SYNCERROR_OUTPUT, &err, SPLT_TRUE);
      if (err < 0) { *error = err; goto bloc_end; }
    }

    //we split all sync errors
    int i = 0;
    for (i = 0; i < state->serrors->serrors_points_num - 1; i++)
    {
      //if we don't cancel the split
      if (!splt_t_split_is_canceled(state))
      {
        //we put the current file to split
        splt_t_set_current_split(state, i);

        splt_tu_auto_increment_tracknumber(state);

        //we append a splitpoint
        int err = splt_sp_append_splitpoint(state, 0, "", SPLT_SPLITPOINT);
        if (err < 0) { *error = err; goto bloc_end; }

        err = splt_u_finish_tags_and_put_output_format_filename(state, -1);
        if (err < 0) { *error = err; goto bloc_end; }

        final_fname = splt_su_get_fname_with_path_and_extension(state, error);

        if (*error >= 0)
        {
          splt_io_create_output_dirs_if_necessary(state, final_fname, error);
          if (error < 0)
          {
            goto bloc_end;
          }

          //we split with the detected splitpoints
          int split_result = splt_p_simple_split(state, final_fname, 
              (off_t) state->serrors->serrors_points[i], 
              (off_t) state->serrors->serrors_points[i+1]);

          //automatically set progress callback to 100%
          splt_c_update_progress(state,1.0,1.0,1,1,1);

          if (split_result >= 0)
          {
            *error = SPLT_SYNC_OK;
          }
          else
          {
            *error = split_result;
          }

          //if the split has been a success
          if (*error == SPLT_SYNC_OK)
          {
            err = splt_c_put_split_file(state, final_fname);
            if (err < 0) { *error = err; goto bloc_end; }
          }
        }
        else
        {
          //error
          goto bloc_end;
        }

        //free some memory
        if (final_fname)
        {
          free(final_fname);
          final_fname = NULL;
        }
      }
      //if we cancel the split
      else
      {
        *error = SPLT_SPLIT_CANCELLED;
        goto bloc_end;
      }
    }
  }
  else
  {
    if (*error >= 0 && err < 0)
    {
      *error = err;
    }
  }

bloc_end:
  //free possible unfreed 'final_fname'
  if (final_fname)
  {
    free(final_fname);
    final_fname = NULL;
  }
}

/************************************/
/*! time and length split           */

static double splt_s_get_real_end_time_splitpoint(splt_state *state, 
    int current_split, long total_time)
{
  long overlapped_time = splt_sp_overlap_time(state, current_split+1);
  double overlapped_end = -1;
  if (overlapped_time == LONG_MAX)
  {
    return overlapped_end;
  }

  overlapped_end = (double) ((double) overlapped_time / 100.0);

  if (total_time <= 0)
  {
    return overlapped_end;
  }

  long minimum_length = splt_o_get_long_option(state, SPLT_OPT_TIME_MINIMUM_THEORETICAL_LENGTH);
  long remaining_time = total_time - overlapped_time;
  if (remaining_time > 0 && remaining_time < minimum_length)
  {
    splt_sp_set_splitpoint_value(state, current_split + 1, total_time);
    return -1.0;
  }

  return overlapped_end;
}

static void splt_s_split_by_time(splt_state *state, int *error,
    double split_time_length, int number_of_files)
{
  char *final_fname = NULL;
  int j=0, tracks=1;
  double begin = 0.f;
  double end = split_time_length;
  long total_time = splt_t_get_total_time(state);

  if (split_time_length >= 0)
  {
    splt_u_print_overlap_time(state);

    int err = SPLT_OK;

    int temp_int = number_of_files + 1;
    if (total_time > 0 && number_of_files == -1)
    {
      temp_int = (int)floor(((total_time / 100.0) / (split_time_length))+1) + 1;
    }
    splt_t_set_splitnumber(state, temp_int);

    splt_of_set_oformat_digits(state);

    //if we have the default output
    int output_filenames = splt_o_get_int_option(state, SPLT_OPT_OUTPUT_FILENAMES);
    if (output_filenames == SPLT_OUTPUT_DEFAULT)
    {
      splt_of_set_oformat(state, SPLT_DEFAULT_OUTPUT, &err, SPLT_TRUE);
      if (err < 0) { *error = err; return; }
    }

    //we append a splitpoint
    err = splt_sp_append_splitpoint(state, 0, "", SPLT_SPLITPOINT);
    if (err >= 0)
    { 
      int save_end_point = SPLT_TRUE;
      if (splt_o_get_long_option(state, SPLT_OPT_OVERLAP_TIME) > 0)
      {
        save_end_point = SPLT_FALSE;
      }

      splt_array *new_end_points = splt_array_new();

      do {
        if (!splt_t_split_is_canceled(state))
        {
          err = splt_sp_append_splitpoint(state, 0, "", SPLT_SPLITPOINT);
          if (err < 0) { *error = err; break; }

          splt_t_set_current_split(state, tracks-1);

          splt_tu_auto_increment_tracknumber(state);

          int current_split = splt_t_get_current_split(state);

          splt_sp_set_splitpoint_value(state, current_split,
              splt_co_time_to_long_ceil(begin));
          long end_splitpoint = splt_co_time_to_long_ceil(end);
          splt_sp_set_splitpoint_value(state, current_split+1, end_splitpoint);

          double real_end_splitpoint =
            splt_s_get_real_end_time_splitpoint(state, current_split, total_time);

          err = splt_u_finish_tags_and_put_output_format_filename(state, -1);
          if (err < 0) { *error = err; break; }

          final_fname = splt_su_get_fname_with_path_and_extension(state,&err);
          if (err < 0) { *error = err; break; }

          double new_sec_end_point = splt_p_split(state, final_fname,
              begin, real_end_splitpoint, error, save_end_point);

          if (save_end_point)
          {
            long new_end_point = 0;
            if (new_sec_end_point == -1.0)
            {
              new_end_point = LONG_MAX;
            }
            else
            {
              new_end_point = splt_co_time_to_long_ceil(new_sec_end_point);
            }

            int end_point_index = current_split + 1;
            splt_il_pair *index_end_point = splt_il_pair_new(end_point_index, new_end_point);
            splt_array_append(new_end_points, (void *) index_end_point);
          }

          //if no error for the split, put the split file
          if (*error >= 0)
          {
            err = splt_c_put_split_file(state, final_fname);
            if (err < 0) { *error = err; break; }
          }

          //set new splitpoints
          begin = end;
          end += split_time_length;
          tracks++;

          //get out if error
          if ((*error == SPLT_MIGHT_BE_VBR) ||
              (*error == SPLT_OK_SPLIT_EOF) ||
              (*error < 0))
          {
            tracks = 0;
          }

          if (*error==SPLT_ERROR_BEGIN_OUT_OF_FILE)
          {
            j--;
          }

          if (final_fname)
          {
            //free memory
            free(final_fname);
            final_fname = NULL;
          }
        }
        else
        {
          *error = SPLT_SPLIT_CANCELLED;
          break;
        }

      } while (j++<tracks);

      if (final_fname)
      {
        free(final_fname);
        final_fname = NULL;
      }

      int i = 0;
      for (i = 0;i < splt_array_length(new_end_points);i++)
      {
        splt_il_pair *index_end_point = (splt_il_pair *) splt_array_get(new_end_points, i);

        splt_sp_set_splitpoint_value(state,
            splt_il_pair_first(index_end_point),
            splt_il_pair_second(index_end_point));

        splt_il_pair_free(&index_end_point);
      }

      splt_array_free(&new_end_points);
    }
    else
    {
      *error = err;
    }

    //we put the time split error
    switch (*error)
    {
/*      case SPLT_MIGHT_BE_VBR: 
        *error = SPLT_TIME_SPLIT_OK;
        break; */

      case SPLT_OK_SPLIT: 
        *error = SPLT_TIME_SPLIT_OK;
        break;
      case SPLT_OK_SPLIT_EOF: 
        *error = SPLT_TIME_SPLIT_OK;
        break;
      case SPLT_ERROR_BEGIN_OUT_OF_FILE: 
        *error = SPLT_TIME_SPLIT_OK;
        break;
      default:
        break;
    }
  }
  else
  {
    *error = SPLT_ERROR_NEGATIVE_TIME_SPLIT;
  }
}

/*! function used with the -t option (time split)

create an indefinite number of smaller files with a fixed time
length specified by options.split_time in seconds
*/
void splt_s_time_split(splt_state *state, int *error)
{
  splt_c_put_info_message_to_client(state, _(" info: starting time mode split\n"));

  long split_time_length = splt_o_get_long_option(state, SPLT_OPT_SPLIT_TIME);
  if (split_time_length == 0)
  {
    *error = SPLT_ERROR_TIME_SPLIT_VALUE_INVALID;
    return;
  }

  splt_s_split_by_time(state, error, split_time_length / 100.0, -1);
}

/*! function used with the -L option (length split)

split into X files
X is defined by SPLT_OPT_LENGTH_SPLIT_FILE_NUMBER
*/
void splt_s_equal_length_split(splt_state *state, int *error)
{
  splt_c_put_info_message_to_client(state, _(" info: starting 'split in equal tracks' mode\n"));

  double total_time = splt_t_get_total_time_as_double_secs(state);
  if (total_time > 0)
  {
    int number_of_files =
      splt_o_get_int_option(state, SPLT_OPT_LENGTH_SPLIT_FILE_NUMBER);

    if (number_of_files > 0)
    {
      double split_time_length = total_time / number_of_files;
      splt_s_split_by_time(state, error, split_time_length, number_of_files);
    }
    else
    {
      *error = SPLT_ERROR_LENGTH_SPLIT_VALUE_INVALID;
      return;
    }
  }
  else
  {
    *error = SPLT_ERROR_CANNOT_GET_TOTAL_TIME;
    return;
  }

  if (*error == SPLT_TIME_SPLIT_OK)
  {
    *error = SPLT_LENGTH_SPLIT_OK;
  }
}

int splt_s_set_trim_silence_splitpoints(splt_state *state, int *error)
{
  splt_d_print_debug(state, "Search and set trim silence splitpoints...\n");

  int found = 0;
  struct splt_ssplit *temp = NULL;
  int append_error = SPLT_OK;

  if (! splt_o_get_int_option(state,SPLT_OPT_QUIET_MODE))
  {
    splt_c_put_info_message_to_client(state, 
        _(" Trim silence split - Th: %.1f dB, Min: %.2f sec\n"),
        splt_o_get_float_option(state, SPLT_OPT_PARAM_THRESHOLD),
        splt_o_get_float_option(state, SPLT_OPT_PARAM_MIN_LENGTH));
  }

  if (state->split.get_silence_level)
  {
    state->split.get_silence_level(0, INT_MAX, state->split.silence_level_client_data);
  }
  found = splt_p_scan_trim_silence(state, error);
  if (*error < 0) { goto end; }

  if (splt_t_split_is_canceled(state))
  {
    *error = SPLT_SPLIT_CANCELLED;
    goto end;
  }

  temp = state->silence_list;
  int i;
  long previous = 0;
  for (i = 1; i < found + 1; i++)
  {
    if (temp == NULL)
    {
      found = i;
      break;
    }

    long temp_silence_pos = splt_siu_silence_position(temp, 0) * 100;

    if (i > 1 && temp_silence_pos < previous)
    {
      temp_silence_pos = LONG_MAX;
    }

    append_error = splt_sp_append_splitpoint(state, temp_silence_pos, NULL, SPLT_SPLITPOINT);
    if (append_error != SPLT_OK) { *error = append_error; found = i; break; }

    temp = temp->next;

    previous = temp_silence_pos;
  }

end:
  splt_siu_ssplit_free(&state->silence_list);
  splt_t_set_splitnumber(state, found);

  return found;
}

/************************************/
/*! Split with split points setermined by silence detection

Sets the silence splitpoints in state->split.splitpoints

\return the number of split points found or the number of tracks
specified in the options  
*/
int splt_s_set_silence_splitpoints(splt_state *state, int *error)
{
  splt_d_print_debug(state,"Search and set silence splitpoints...\n");

  int found = 0;
  int splitpoints_appended = 0;
  struct splt_ssplit *temp = NULL;
  int append_error = SPLT_OK;

  float offset = splt_o_get_float_option(state,SPLT_OPT_PARAM_OFFSET);
  int number_tracks = splt_o_get_int_option(state, SPLT_OPT_PARAM_NUMBER_TRACKS);

  int we_read_silence_from_logs = SPLT_FALSE;

  FILE *log_file = NULL;
  char *log_fname = splt_t_get_silence_log_fname(state);
  if (splt_o_get_int_option(state, SPLT_OPT_ENABLE_SILENCE_LOG))
  {
    if ((log_file = splt_io_fopen(log_fname, "r")))
    {
      char *log_silence_fname = splt_io_readline(log_file, error);
      if (*error < 0)
      {
        if (log_silence_fname)
        {
          free(log_silence_fname);
          log_silence_fname = NULL;
        }
        fclose(log_file);
        return found;
      }

      if (log_silence_fname && log_silence_fname[0] != '\0')
      {
        log_silence_fname[strlen(log_silence_fname)-1] = '\0';
        if (strcmp(log_silence_fname, splt_t_get_filename_to_split(state)) == 0)
        {
          we_read_silence_from_logs = SPLT_TRUE;
          float threshold = SPLT_DEFAULT_PARAM_THRESHOLD;
          float min = SPLT_DEFAULT_PARAM_MINIMUM_LENGTH;
          int shots = SPLT_DEFAULT_PARAM_SHOTS;
          int i = fscanf(log_file, "%f\t%f\t%d", &threshold, &min, &shots);

          //compatibility with older versions; allow missing shots
          if (i == 2) { shots = SPLT_DEFAULT_PARAM_SHOTS; }

          if ((i < 2) ||
              (threshold != splt_o_get_float_option(state, SPLT_OPT_PARAM_THRESHOLD)) ||
              (splt_o_get_float_option(state, SPLT_OPT_PARAM_MIN_LENGTH) != min) ||
              (splt_o_get_int_option(state, SPLT_OPT_PARAM_SHOTS) != shots))
          {
            we_read_silence_from_logs = SPLT_FALSE;
          }
          else
          {
            splt_o_set_float_option(state, SPLT_OPT_PARAM_THRESHOLD, threshold);
            splt_o_set_float_option(state, SPLT_OPT_PARAM_MIN_LENGTH, min);
            splt_o_set_int_option(state, SPLT_OPT_PARAM_SHOTS, shots);
          }
        }

        free(log_silence_fname);
        log_silence_fname = NULL;
      }

      if (!we_read_silence_from_logs && log_file)
      {
        fclose(log_file);
        log_file = NULL;
      }
    }
  }

  char remove_str[128] = { '\0' };
  if (splt_o_get_int_option(state, SPLT_OPT_PARAM_REMOVE_SILENCE))
  {
    snprintf(remove_str, 128, "%s(%.2f-%.2f)", _ ("YES"),
        splt_o_get_float_option(state, SPLT_OPT_KEEP_SILENCE_LEFT),
        splt_o_get_float_option(state, SPLT_OPT_KEEP_SILENCE_RIGHT));
  }
  else
  {
    snprintf(remove_str, 128 ,_("NO"));
  }

  char auto_user_str[128] = { '\0' };
  if (splt_o_get_int_option(state, SPLT_OPT_PARAM_NUMBER_TRACKS) > 0)
  {
    snprintf(auto_user_str,128,_("User"));
  }
  else
  {
    snprintf(auto_user_str,128,_("Auto"));
  }

  if (! splt_o_get_int_option(state, SPLT_OPT_QUIET_MODE))
  {
    char *other_options = NULL;

    if (splt_o_get_float_option(state, SPLT_OPT_PARAM_MIN_TRACK_JOIN) > 0)
    {
      char *min_track_join = 
        splt_su_get_formatted_message(state, ", %s: %.2f", "Min track join",
            splt_o_get_float_option(state, SPLT_OPT_PARAM_MIN_TRACK_JOIN));

      int err = splt_su_append_str(&other_options, min_track_join, NULL);
      if (err < 0) { *error = err; goto end; }

      free(min_track_join);
    }

    splt_c_put_info_message_to_client(state,
        _(" Silence split type: %s mode (Th: %.1f dB,"
          " Off: %.2f, Min: %.2f, Remove: %s, Min track: %.2f, Shots: %d%s)\n"),
        auto_user_str,
        splt_o_get_float_option(state, SPLT_OPT_PARAM_THRESHOLD),
        splt_o_get_float_option(state, SPLT_OPT_PARAM_OFFSET),
        splt_o_get_float_option(state, SPLT_OPT_PARAM_MIN_LENGTH),
        remove_str,
        splt_o_get_float_option(state, SPLT_OPT_PARAM_MIN_TRACK_LENGTH),
        splt_o_get_int_option(state, SPLT_OPT_PARAM_SHOTS), 
        other_options == NULL ? "" : other_options);

    if (other_options)
    {
      free(other_options);
    }
  }
 
  short read_silence_from_logs = SPLT_FALSE;
  if (we_read_silence_from_logs)
  {
    if (state->split.get_silence_level)
    {
      state->split.get_silence_level(0, INT_MAX, state->split.silence_level_client_data);
    }

    splt_c_put_info_message_to_client(state, 
        _(" Found silence log file '%s' ! Reading"
          " silence points from file to save time ;)\n"), log_fname);

    found = splt_siu_parse_ssplit_file(state, log_file, error);
    if (log_file)
    {
      fclose(log_file);
      log_file = NULL;
    }

    read_silence_from_logs = SPLT_TRUE;
  }
  else
  {
    if (state->split.get_silence_level)
    {
      state->split.get_silence_level(0, INT_MAX, state->split.silence_level_client_data);
    }

    int err = splt_s_open_full_log_filename(state);
    if (err < 0) { *error = err; goto end; }

    found = splt_p_scan_silence(state, error);

    splt_s_close_full_log_filename(state);
  }

  //if no error
  if (*error >= 0)
  {
    if (!read_silence_from_logs)
    {
      splt_c_put_info_message_to_client(state, "\n");
    }

    splt_c_put_info_message_to_client(state, _(" Total silence points found: %d."), found);

    if (found > 0)
    {
      int selected_tracks = found + 1;
      int param_number_of_tracks = splt_o_get_int_option(state, SPLT_OPT_PARAM_NUMBER_TRACKS);
      if (param_number_of_tracks > 0)
      {
        selected_tracks = param_number_of_tracks;
      }

      splt_c_put_info_message_to_client(state, 
          _(" (Selected %d tracks)\n"), selected_tracks);
    }
    else
    {
      splt_c_put_info_message_to_client(state, "\n");
    }

    //we set the number of tracks
    if (!splt_t_split_is_canceled(state))
    {
      found++;
      if ((number_tracks > 0) &&
          (number_tracks < SPLT_MAXSILENCE))
      {
        if (number_tracks < found)
        {
          found = number_tracks;
        }
      }

      temp = state->silence_list;

      int i;

      for (i = 1; i < found; i++)
      {
        if (temp == NULL)
        {
          found = i;
          break;
        }

        if (i == 1)
        {
          append_error = splt_sp_append_splitpoint(state, 0, NULL, SPLT_SPLITPOINT);
          if (append_error < 0) { *error = append_error; found = i; break;}

          splitpoints_appended++;
        }

        if (splt_o_get_int_option(state, SPLT_OPT_PARAM_REMOVE_SILENCE))
        {
          long end_track_point = splt_co_time_to_long(temp->begin_position);
          long end_track_point_after_silence = splt_co_time_to_long(temp->end_position);
          long silence_length = end_track_point_after_silence - end_track_point;

          float keep_silence_left = splt_o_get_float_option(state, SPLT_OPT_KEEP_SILENCE_LEFT);
          float keep_silence_right = splt_o_get_float_option(state, SPLT_OPT_KEEP_SILENCE_RIGHT);
          long keep_silence_total = splt_co_time_to_long(keep_silence_left + keep_silence_right);

          if (silence_length > keep_silence_total)
          {
            long adjusted_silence_left = splt_co_time_to_long(temp->begin_position + keep_silence_right);
            long adjusted_silence_right = splt_co_time_to_long(temp->end_position - keep_silence_left);

            append_error = splt_sp_append_splitpoint(state, adjusted_silence_left, NULL, SPLT_SKIPPOINT);
            if (append_error < 0) { *error = append_error; found = i; break;}
            append_error =
              splt_sp_append_splitpoint(state, adjusted_silence_right, NULL, SPLT_SPLITPOINT);
            if (append_error < 0) { *error = append_error; found = i; break;}

            splitpoints_appended += 2;
          }
          else
          {
            float offset = keep_silence_right / (keep_silence_left + keep_silence_right);
            long end_track_point = splt_co_time_to_long(splt_siu_silence_position(temp, offset));
            append_error = splt_sp_append_splitpoint(state, end_track_point, NULL, SPLT_SPLITPOINT);
            if (append_error < 0) { *error = append_error; found = i; break;}
            splitpoints_appended++;
          }
        }
        else
        {
          long end_track_point = splt_co_time_to_long(splt_siu_silence_position(temp, offset));
          append_error = splt_sp_append_splitpoint(state, end_track_point, NULL, SPLT_SPLITPOINT);
          if (append_error != SPLT_OK) { *error = append_error; found = i; break; }

          splitpoints_appended++;
        }

        temp = temp->next;
      }

      splt_d_print_debug(state,"Order splitpoints...\n");
      splt_sp_order_splitpoints(state, splitpoints_appended);

      if (*error >= 0)
      {
        long total_time = splt_t_get_total_time(state);
        if (total_time <= 0)
        {
          total_time = LONG_MAX;
        }

        //last splitpoint, end of file
        append_error =
          splt_sp_append_splitpoint(state, total_time, NULL, SPLT_SPLITPOINT);
        if (append_error != SPLT_OK) { *error = append_error; }
      }

      splt_sp_join_minimum_tracks_splitpoints(state, error);
      if (*error < 0) { goto end; }

      splt_sp_skip_minimum_track_length_splitpoints(state, error);
    }
    else
    {
      *error = SPLT_SPLIT_CANCELLED;
    }

    //if splitpoints are found
    if ((*error >= 0) && (found > 0) && !we_read_silence_from_logs)
    {
      //if we write the silence points log file
      if (splt_o_get_int_option(state, SPLT_OPT_ENABLE_SILENCE_LOG))
      {
        char *fname = splt_t_get_silence_log_fname(state);

        splt_c_put_info_message_to_client(state, _(" Writing silence log file '%s' ...\n"), fname);

        if (! splt_o_get_int_option(state, SPLT_OPT_PRETEND_TO_SPLIT))
        {
          FILE *log_file = NULL;
          if (!(log_file = splt_io_fopen(fname, "w")))
          {
            splt_e_set_strerror_msg_with_data(state, fname);
            *error = SPLT_ERROR_CANNOT_OPEN_FILE;
          }
          else
          {
            //do the effective write
            struct splt_ssplit *temp = state->silence_list;
            fprintf(log_file, "%s\n", splt_t_get_filename_to_split(state));
            fprintf(log_file, "%.2f\t%.2f\t%d\n", 
                splt_o_get_float_option(state, SPLT_OPT_PARAM_THRESHOLD),
                splt_o_get_float_option(state, SPLT_OPT_PARAM_MIN_LENGTH),
                splt_o_get_int_option(state, SPLT_OPT_PARAM_SHOTS));
            while (temp != NULL)
            {
              fprintf(log_file, "%f\t%f\t%ld\n",
                  temp->begin_position, temp->end_position, temp->len);
              temp = temp->next;
            }
            fflush(log_file);
            if (log_file)
            {
              fclose(log_file);
              log_file = NULL;
            }
            temp = NULL;
          }
        }
      }
    }
  }

end:
  splt_siu_ssplit_free(&state->silence_list);
  splt_t_set_splitnumber(state, splitpoints_appended + 1);

  return found;
}

/*! Do the silence split

\param error The code of a eventual error that has occoured
*/ 
void splt_s_silence_split(splt_state *state, int *error)
{
  splt_d_print_debug(state,"Starting silence split ...\n");

  //print some useful infos to the client
  splt_c_put_info_message_to_client(state, _(" info: starting silence mode split\n"));

  int found = splt_s_set_silence_splitpoints(state, error);

  //if no error
  if (*error >= 0)
  {
    //if we have found splitpoints, write the silence tracks
    if (found > 1)
    {
      splt_d_print_debug(state,"Writing silence tracks...\n");

      //set the default silence output
      int output_filenames = splt_o_get_int_option(state,SPLT_OPT_OUTPUT_FILENAMES);
      if (output_filenames == SPLT_OUTPUT_DEFAULT)
      {
        splt_of_set_oformat(state, SPLT_DEFAULT_SILENCE_OUTPUT, error, SPLT_TRUE);
        if (*error < 0) { return; }
      }

      splt_s_multiple_split(state, error);

      //we put the silence split errors
      switch (*error)
      {
        case SPLT_OK_SPLIT:
          *error = SPLT_SILENCE_OK;
          break;
        case SPLT_OK_SPLIT_EOF:
          *error = SPLT_SILENCE_OK;
          break;
        default:
          break;
      }
    }
    else
    {
      *error = SPLT_NO_SILENCE_SPLITPOINTS_FOUND;
    }
  }
}

/*! Do the trim silence split

\param error The code of a eventual error that has occoured
*/ 
void splt_s_trim_silence_split(splt_state *state, int *error)
{
  splt_c_put_info_message_to_client(state, _(" info: starting trim using silence mode split\n"));

  int found = splt_s_set_trim_silence_splitpoints(state, error);
  if (*error < 0) { return; }

  if (found < 1)
  {
    *error = SPLT_NO_SILENCE_SPLITPOINTS_FOUND;
    return;
  }

  splt_d_print_debug(state,"Writing tracks...\n");

  int output_filenames = splt_o_get_int_option(state,SPLT_OPT_OUTPUT_FILENAMES);
  if (output_filenames == SPLT_OUTPUT_DEFAULT)
  {
    splt_of_set_oformat(state, SPLT_DEFAULT_TRIM_SILENCE_OUTPUT, error, SPLT_TRUE);
    if (*error < 0) { return; }
  }

  splt_s_multiple_split(state, error);

  switch (*error)
  {
    case SPLT_OK_SPLIT:
      *error = SPLT_TRIM_SILENCE_OK;
      break;
    case SPLT_OK_SPLIT_EOF:
      *error = SPLT_TRIM_SILENCE_OK;
      break;
    default:
      break;
  }
}

static splt_code splt_s_open_full_log_filename(splt_state *state)
{
  char *fname = splt_t_get_silence_full_log_fname(state);
  if (!fname || fname[0] == '\0')
  {
    return SPLT_OK;
  }

  state->full_log_file_descriptor = splt_io_fopen(fname, "w");
  if (!state->full_log_file_descriptor)
  {
    splt_e_set_strerror_msg_with_data(state, fname);
    return SPLT_ERROR_CANNOT_OPEN_FILE;
  }

  return SPLT_OK;
}

static void splt_s_close_full_log_filename(splt_state *state)
{
  if (!state->full_log_file_descriptor)
  {
    return;
  }

  fflush(state->full_log_file_descriptor);
  fclose(state->full_log_file_descriptor);
  state->full_log_file_descriptor = NULL;
}

/****************************/
/*! Automatically split a file that has been created by mp3wrap

do the wrap split
*/
void splt_s_wrap_split(splt_state *state, int *error)
{
  char *new_filename_path = splt_t_get_new_filename_path(state);
  char *filename = splt_t_get_filename_to_split(state);

  splt_d_print_debug(state,"Begin wrap split for the file _%s_\n", filename);

  splt_c_put_info_message_to_client(state, _(" info: starting wrap mode split\n"));

  splt_p_dewrap(state, SPLT_FALSE, new_filename_path, error);
}

