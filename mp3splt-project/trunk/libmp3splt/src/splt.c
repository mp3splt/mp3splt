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

#include "splt.h"

/****************************/
/* splt normal split */

//the real split of the file
static void splt_s_real_split(splt_state *state, int *error)
{
  int split_error = *error;
  double splt_beg = splt_t_get_i_begin_point(state);
  double splt_end = splt_t_get_i_end_point(state);
  char *final_fname = NULL;
  final_fname =
    splt_u_get_fname_with_path_and_extension(state,error);

  if (error >= 0)
  {
    splt_p_split(state, final_fname, splt_beg, splt_end, error);

    //if no error
    if (*error >= 0)
    {
      //automatically set progress callback to 100%
      splt_t_update_progress(state,1.0,1.0,1,1,1);

      //we put the splitted file
      splt_t_put_splitted_file(state, final_fname);
    }
  }

  if (final_fname)
  {
    //free memory
    free(final_fname);
  }
}

//simple split with only 2 splitpoints
//i is the current splitpoint
//(used to get the filename)
//error in error parameter
static void splt_s_split(splt_state *state, int *error)
{
  //begin splitpoint
  double splt_beg = 0;
  //end splitpoint
  double splt_end = 0;

  //we get the splitpoints to split
  int get_error = SPLT_OK;
  int current_split = splt_t_get_current_split(state);
  long split_begin = 0,split_end = 0;
  split_begin = 
    splt_t_get_splitpoint_value(state, current_split, &get_error);
  split_end = 
    splt_t_get_splitpoint_value(state, current_split+1, &get_error);

  if (get_error == SPLT_OK)
  {
    //if we put mins and secs
    if (splt_t_get_int_option(state,
          SPLT_OPT_OUTPUT_FILENAMES)
        == SPLT_OUTPUT_MINS_SECS)
    {
      splt_u_set_complete_mins_secs_filename(state,error);
    }

    //if no error
    if (*error >= 0)
    {
      //if the first splitpoint different than the end point
      if (split_begin != split_end)
      {
        //convert to float for hundredth
        // 34.6  --> 34 seconds and 6 hundredth
        splt_beg = split_begin / 100;
        splt_beg += ((split_begin % 100) / 100.);
        //we will transmit to the plugins the -1.f value
        //if we have LONG_MAX (== EOF)
        if (split_end == LONG_MAX)
        {
          splt_end = -1.f;
        }
        else
        {
          splt_end = split_end / 100;
          splt_end += ((split_end % 100) / 100.);
        }

        splt_t_set_i_begin_point(state,splt_beg);
        splt_t_set_i_end_point(state,splt_end);

        //we do the real split
        splt_s_real_split(state, error);
      }
    }
  }
  else
  {
    *error = get_error;
  }
}

//splits the file with multiple points
void splt_s_multiple_split(splt_state *state, int *error)
{
  int i = 0;
  char *final_fname = NULL;

  int err = SPLT_OK;

  //if we have the sync error mode
  //we do a different split than the normal split
  if (splt_t_get_int_option(state, SPLT_OPT_SPLIT_MODE) == SPLT_OPTION_ERROR_MODE)
  {
    splt_t_put_message_to_client(state, " info: starting error mode split\n");

    //we detect sync errors
    splt_p_search_syncerrors(state, error);

    //automatically set progress callback to 100% after
    //the error detection
    splt_t_update_progress(state,1.0,1.0,1,1,1);

    //if we have the default output, we put the
    //default
    err = SPLT_OK;
    if (splt_t_get_int_option(state,SPLT_OPT_OUTPUT_FILENAMES)
        == SPLT_OUTPUT_DEFAULT)
    {
      splt_t_set_oformat(state, SPLT_DEFAULT_SYNCERROR_OUTPUT,&err);
    }

    //if no error
    if (*error >= 0 && err >= 0)
    {
      //we put the number of sync errors
      splt_t_set_splitnumber(state, state->serrors->serrors_points_num - 1);

      //we split all sync errors
      for (i = 0; i < state->serrors->serrors_points_num - 1; i++)
      {
        //if we don't cancel the split
        if (!splt_t_split_is_canceled(state))
        {
          //we put the current file to split
          splt_t_set_current_split(state, i);

          //we append a splitpoint
          splt_t_append_splitpoint(state, 0, "");

          //if we have to put the original tags
          if (splt_t_get_int_option(state, SPLT_OPT_TAGS) == SPLT_TAGS_ORIGINAL_FILE)
          {
            //we put the original tags to the current split
            splt_t_append_original_tags(state);
          }

          //we put the output filename
          splt_u_put_output_format_filename(state);

          //we get the final fname
          if (final_fname)
          {
            free(final_fname);
            final_fname = NULL;
          }
          final_fname =
            splt_u_get_fname_with_path_and_extension(state,error);

          if(*error >= 0)
          {
            //we split with the detected splitpoints
            int split_result = splt_p_simple_split(state, final_fname, 
                (off_t) state->serrors->serrors_points[i], 
                (off_t) state->serrors->serrors_points[i+1]);

            //automatically set progress callback to 100%
            splt_t_update_progress(state,1.0,1.0,1,1,1);

            if (split_result >= 0)
            {
              *error = SPLT_SYNC_OK;
            }

            //if the split has been a success
            if (*error == SPLT_SYNC_OK)
            {
              splt_t_put_splitted_file(state, final_fname);
            }
          }
          else
          {
            //error
            goto bloc_end;
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
      if (err < 0)
      {
        *error = err;
      }
    }

bloc_end:
    if (final_fname)
    {
      free(final_fname);
      final_fname = NULL;
    }
  }
  else
    //if we don't have the sync error mode
    //we do a normal split
  {
    splt_t_put_message_to_client(state, " info: starting normal split\n");

    int get_error = SPLT_OK;

    if (get_error >= 0)
    {
      //for every 2 splitpoints, split
      for(i = 0; i < (splt_t_get_splitnumber(state) - 1); i++)
      {
        //we put the current file to split
        splt_t_set_current_split(state, i);

        //if we don't cancel the split
        if (!splt_t_split_is_canceled(state))
        {
          get_error = SPLT_OK;

          //we look what option we have for the output filenames
          int output_filenames =
            splt_t_get_int_option(state,
                SPLT_OPT_OUTPUT_FILENAMES);
          //if we have to put the original tags
          if (splt_t_get_int_option(state, SPLT_OPT_TAGS) 
              == SPLT_TAGS_ORIGINAL_FILE)
          {
            //we put the original tags to the current split
            splt_t_append_original_tags(state);
          }
          //we put the output filrnames
          if (output_filenames == SPLT_OUTPUT_FORMAT)
          {
            splt_u_put_output_format_filename(state);
          }

          char *temp_name = 
            splt_t_get_splitpoint_name(state,i,&get_error);
          if (get_error == SPLT_OK)
          {
            //if we have no filename, we put
            //option mins secs
            if ((temp_name == NULL) || (temp_name[0] == '\0'))
            {
              splt_t_set_int_option(state, SPLT_OPT_OUTPUT_FILENAMES,
                  SPLT_OUTPUT_MINS_SECS);
            }

            splt_s_split(state, error);

            //get out if error
            if ((*error==SPLT_ERROR_BEGIN_OUT_OF_FILE)||
                (*error==SPLT_ERROR_INVALID)||
                (*error==SPLT_OK_SPLITTED_EOF))
            {
              break;
            }
          }
          else
          {
            *error = get_error;
          }
        }
        else
        {
          *error = SPLT_SPLIT_CANCELLED;
        }
      }
    }
    else
    {
      *error = get_error;
    }
  }
}

/************************************/
/* splt time split */

//function used with the -t option (time split
//create an indefinite number of smaller files with a fixed time
//length specified by options.split_time in seconds
void splt_s_time_split(splt_state *state, int *error)
{
  char *filename = splt_t_get_filename_to_split(state);

  splt_u_print_debug("Starting time split...",0,NULL);

  splt_t_put_message_to_client(state, " info: starting time mode split\n");

  char *final_fname = NULL;
  //if the state has an error
  int j=0,tracks=1;
  //the begin split
  double begin = 0.f;
  double end = (double)
    splt_t_get_float_option(state, SPLT_OPT_SPLIT_TIME);

  //if no state error
  if (state->options.split_time >= 0)
  {
    int err = SPLT_OK;

    int temp_int = 
      (int)floor(((splt_t_get_total_time(state)/100.0) /
            (state->options.split_time))+1) + 1;
    splt_t_set_splitnumber(state,temp_int);

    splt_t_set_oformat_digits(state);

    //we append a splitpoint
    splt_t_append_splitpoint(state, 0, "");

    if (err >= 0)
    {
      //while we have tracks
      do {
        //if we don't cancel the split
        if (!splt_t_split_is_canceled(state))
        {
          //we append a splitpoint
          splt_t_append_splitpoint(state, 0, "");

          //if we have to put the original tags
          if (splt_t_get_int_option(state, SPLT_OPT_TAGS) == SPLT_TAGS_ORIGINAL_FILE)
          {
            //we put the original tags to the current split
            splt_t_append_original_tags(state);
          }

          //we set the current split
          splt_t_set_current_split(state, tracks-1);

          int current_split = splt_t_get_current_split(state);
          int output_filenames = 
            splt_t_get_int_option(state, SPLT_OPT_OUTPUT_FILENAMES);
          //if we have the default output or the mins_secs
          if ((output_filenames == SPLT_OUTPUT_DEFAULT)
              || (output_filenames == SPLT_OUTPUT_MINS_SECS))
          {
            //we put the splitpoints values in the state
            splt_t_set_splitpoint_value(state,
                current_split,(long)(begin*100));
            long end_splitpoint = end*100;
            if (end_splitpoint > splt_t_get_total_time(state))
            {
              end_splitpoint = splt_t_get_total_time(state);
            }
            splt_t_set_splitpoint_value(state,
                current_split+1,end_splitpoint);
            //we put the filename with mins_secs
            splt_u_set_complete_mins_secs_filename(state, &err);
          }
          else
          {
            //if we have the output format
            if (output_filenames == SPLT_OUTPUT_FORMAT)
            {
              //if we have the output option
              //we put the output filename
              splt_u_put_output_format_filename(state);
            }
          }

          //we get the final fname
          final_fname =
            splt_u_get_fname_with_path_and_extension(state,&err);

          //if error
          if (err < 0)
          {
            *error = err;
            break;
          }

          splt_p_split(state, final_fname, begin, end, error);

          //if no error for the split, put the splitted file
          if (*error >= 0)
          {
            splt_t_put_splitted_file(state, final_fname);
          }

          //set new splitpoints
          begin = end;
          end += splt_t_get_float_option(state,SPLT_OPT_SPLIT_TIME);
          tracks++;

          //get out if error
          if ((*error==SPLT_ERROR_BEGIN_OUT_OF_FILE)||
              (*error==SPLT_MIGHT_BE_VBR)||
              (*error==SPLT_ERROR_INVALID)||
              (*error==SPLT_OK_SPLITTED_EOF))
          {
            tracks = 0;
          }
          if (*error==SPLT_ERROR_BEGIN_OUT_OF_FILE)
          {
            j--;
          }

          //free memory
          free(final_fname);
          final_fname = NULL;
        }
        else
        {
          *error = SPLT_SPLIT_CANCELLED;
          break;
        }

      } while (j++<tracks);
    }

    //free memory
    if (final_fname)
    {
      free(final_fname);
      final_fname = NULL;
    }

    //we put the time split error
    switch (*error)
    {
      case SPLT_MIGHT_BE_VBR : 
        *error = SPLT_TIME_SPLIT_OK;
        break;
      case SPLT_OK_SPLITTED: 
        *error = SPLT_TIME_SPLIT_OK;
        break;
      case SPLT_OK_SPLITTED_EOF : 
        *error = SPLT_TIME_SPLIT_OK;
        break;
      case SPLT_ERROR_BEGIN_OUT_OF_FILE : 
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

/************************************/
/* splt silence detection and split */

//returns the number of silence splits found
//or the number of tracks specified in the options
//sets the silence splitpoints in state->split.splitpoints
//write_tracks is a boolean, SPLT_TRUE if we call the function
//write_silence_tracks and SPLT_FALSE otherwise
int splt_s_set_silence_splitpoints(splt_state *state,
    int write_tracks, int *error)
{
  splt_u_print_debug("We search and set silence splitpoints...",0,NULL);

  //found is the number of silence splits found
  int found = 0;
  //checks if the state is null
  int state_null = SPLT_FALSE;
  struct splt_ssplit *temp = NULL;
  int append_error = SPLT_OK;
  //we get some options
  float offset =
    splt_t_get_float_option(state,SPLT_OPT_PARAM_OFFSET);
  int number_tracks =
    splt_t_get_int_option(state, SPLT_OPT_PARAM_NUMBER_TRACKS);

  found = splt_p_scan_silence(state, error);

  //if no error
  if (*error >= 0)
  {
    //put client infos
    char client_infos[512] = { '\0' };
    snprintf(client_infos,512,"\n Total silence points found: %d.\n",found);
    splt_t_put_message_to_client(state,client_infos);

    //we set the number of tracks
    int order = 0;
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

      //put first splitpoint
      append_error = splt_t_append_splitpoint(state, 0, NULL);
      if (append_error != SPLT_OK)
      {
        *error = append_error;
      }
      else
      {
        temp = state->silence_list;
        int i;

        //we take all splitpoints found and we remove silence 
        //if needed
        for (i = 1; i < found; i++)
        {
          if (temp == NULL)
          {
            found = i;
            break;
          }
          if (splt_t_get_int_option(state, SPLT_OPT_PARAM_REMOVE_SILENCE))
          {
            //we append 2 splitpoints
            splt_t_append_splitpoint(state, 0, NULL);
            splt_t_append_splitpoint(state, 0, NULL);
            //we set the values
            splt_t_set_splitpoint_value(state,
                2*i-1,temp->begin_position * 100);
            splt_t_set_splitpoint_value(state,
                2*i,temp->end_position * 100);
          }
          else
          {
            long temp_silence_pos = splt_u_silence_position(temp, offset) *100;
            append_error = splt_t_append_splitpoint(state, temp_silence_pos, NULL);
            if (append_error != SPLT_OK)
            {
              *error = append_error;
              break;
            }
          }
          temp = temp->next;
        }

        //we order the splitpoints
        if (splt_t_get_int_option(state, SPLT_OPT_PARAM_REMOVE_SILENCE))
        {
          order = (found-1)*2+1;
        }
        else 
        {
          order = found;
        }

        splt_u_print_debug("We order splitpoints...",0,NULL);
        splt_u_order_splitpoints(state, order);

        //last splitpoint, end of file
        append_error = splt_t_append_splitpoint(state,
            splt_t_get_total_time(state), NULL);
        if (append_error != SPLT_OK)
        {
          *error = append_error;
        }
      }
    }
    else
    {
      *error = SPLT_SPLIT_CANCELLED;
    }
  }
  splt_t_ssplit_free(&state->silence_list);

  return found;
}

//write the silence tracks
//warning, this function must always be called after
//set_silence_splitpoints
//we have possible error in error
void splt_s_write_silence_tracks(int found, splt_state *state, int *error)
{
  char *filename = splt_t_get_filename_to_split(state);

  splt_u_print_debug("Writing silence tracks...",0,NULL);

  char *final_fname = NULL;
  int i = 0;

  //begin splitpoint and end splitpoint
  double beg_pos = 0, end_pos = 0;
  int get_error = SPLT_OK;

  //we put the number of tracks found
  splt_t_set_splitnumber(state, found+1);

  //if we have the default output, we put the
  //default
  int err = SPLT_OK;
  int output_filenames = splt_t_get_int_option(state,SPLT_OPT_OUTPUT_FILENAMES);

  if (err >= 0)
  {
    //if we have the default output
    if (output_filenames == SPLT_OUTPUT_DEFAULT)
    {
      splt_t_set_oformat(state,SPLT_DEFAULT_SILENCE_OUTPUT,&err);
    }

    //if no error
    if (err >= 0)
    {
      //we write all found tracks
      for (i = 0; i < found; i++)
      {
        if (!splt_t_split_is_canceled(state))
        {
          //if we have to put the original tags
          if (splt_t_get_int_option(state, SPLT_OPT_TAGS) == SPLT_TAGS_ORIGINAL_FILE)
          {
            //we put the original tags to the current split
            splt_t_append_original_tags(state);
          }

          //we put the output filename
          splt_u_put_output_format_filename(state);

          //we get the output filename+path+extension
          final_fname = 
            splt_u_get_fname_with_path_and_extension(state, &err);

          if(err >= 0)
          {
            long split_begin = 0,split_end = 0;
            if (splt_t_get_int_option(state, SPLT_OPT_PARAM_REMOVE_SILENCE))
            {
              split_begin = splt_t_get_splitpoint_value(state,2*i,&get_error);
              split_end = splt_t_get_splitpoint_value(state,2*i+1,&get_error);
            }
            else
            {
              split_begin = splt_t_get_splitpoint_value(state,i,&get_error);
              split_end = splt_t_get_splitpoint_value(state,i+1,&get_error);
            }

            //if we have an error
            if (get_error != SPLT_OK)
            {
              *error = get_error;
            }
            else
            {
              beg_pos = splt_u_get_double_pos(split_begin);
              end_pos = splt_u_get_double_pos(split_end);

              splt_p_split(state, final_fname, beg_pos, end_pos, error);

              //put the splitted file if no error
              if (*error >= 0)
              {
                splt_t_put_splitted_file(state, final_fname);
              }

              //we put the silence split errors
              switch (*error)
              {
                case SPLT_MIGHT_BE_VBR:
                  *error = SPLT_SILENCE_OK;
                  break;
                case SPLT_OK_SPLITTED:
                  *error = SPLT_SILENCE_OK;
                  break;
                case SPLT_OK_SPLITTED_EOF:
                  *error = SPLT_SILENCE_OK;
                  break;
                default:
                  goto function_end;
                  break;
              }
            }
          }
          else
          {
            goto function_end;
          }
        }
        else
        {
          *error = SPLT_SPLIT_CANCELLED;
          goto function_end;
        }
      }
    }
  }
  else
  {
    *error = err;
  }

function_end:
  err = SPLT_OK;
  if (*error >= 0 && err < 0)
  {
    *error = err;
  }
  //free memory
  if (final_fname)
  {
    free(final_fname);
    final_fname = NULL;
  }
}

//do the silence split
//possible error in error
void splt_s_silence_split(splt_state *state, int *error)
{
  splt_u_print_debug("Starting silence split ...",0,NULL);

  splt_t_put_message_to_client(state, " info: starting silence mode split\n");

  int found = 0;
  found = splt_s_set_silence_splitpoints(state, SPLT_TRUE, error);

  //if no error
  if(*error >= 0)
  {
    //if we have found splitpoints, write the silence tracks
    if (found > 1)
    {
      splt_s_write_silence_tracks(found,state,error);
    }
    else
    {
      *error = SPLT_NO_SILENCE_SPLITPOINTS_FOUND;
    }
  }
}

/****************************/
/* splt wrap split */

//do the wrap split
void splt_s_wrap_split(splt_state *state, int *error)
{
  char *new_filename_path = splt_t_get_new_filename_path(state);
  char *filename = splt_t_get_filename_to_split(state);

  splt_u_print_debug("We begin wrap split for the file ...",0,filename);

  splt_t_put_message_to_client(state, " info: starting wrap mode split\n");

  splt_p_dewrap(state, SPLT_FALSE, new_filename_path, error);
}

