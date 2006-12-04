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
/* splt utils */

//get the mp3 info and put it in the state
//error is the possible error
splt_state *splt_s_get_mp3_info(splt_state *state, 
                                FILE *file_input, int *error)
{
  //checks if valid mp3 file
  //before last argument, if framemode or not
  //last argument if we put messages to clients or not
  state->mstate = 
    splt_mp3_info(file_input, state, 
                  splt_t_get_int_option(state,SPLT_OPT_MP3_FRAME_MODE),
                  error);
  //if error
  if ((*error < 0) || 
      (state->mstate == NULL))
    {
      return NULL;
    }
  
  return state;
}

#ifndef NO_OGG
//get ogg info and put it in the state
//error is the possible error
splt_state *splt_s_get_ogg_info(splt_state *state, 
                                FILE *file_input, int *error)
{
  //checks if valid ogg file
  state->ostate = 
    splt_ogg_info(file_input, state->ostate, error);
  
  //if error
  if ((*error < 0) ||
      (state->ostate == NULL))
    {
      return NULL;
    }
  
  return state;
}
#endif

//put the total time in state->split.total_time
void splt_s_put_total_time(splt_state *state, int *error)
{
  FILE *file_input;
  char *filename = splt_t_get_filename_to_split(state);
  
  //if we can open the file
  if ((file_input = fopen(filename, "rb")) != NULL)
    {
#ifndef NO_OGG
      //if the file is ogg
      if (splt_t_get_file_format(state) == SPLT_OGG_FORMAT)
        {
          OggVorbis_File ogg_file;
          
          //if we can open file
          if(ov_open(file_input, &ogg_file, NULL, 0) >= 0)
            {
              long temp =
                ov_time_total(&ogg_file, -1) * 100;
              splt_t_set_total_time(state,temp);
              ov_clear(&ogg_file);
            }
          else
            {
              *error = SPLT_ERROR_CANNOT_OPEN_FILE;
            }
        }
      else
#endif
        {
	  //if the file is mp3
	  if (splt_t_get_file_format(state) == SPLT_MP3_FORMAT)
	    {
	      splt_t_lock_messages(state);
          
	      //if we can read the file
	      //get the mp3 info => this puts the total time in the
	      //state
	      if((state = splt_s_get_mp3_info(state, file_input,error))
		 != NULL)
		{
		  //we free the mp3 state after the check
		  splt_mp3_state_free(state);
		}
          
	      splt_t_unlock_messages(state);
          
	      fclose(file_input);
	    }
	  else
	    {
	      //todo
	    }
        }
    }
  else
    {
      *error = SPLT_ERROR_CANNOT_OPEN_FILE;
    }
}

/****************************/
/* splt normal split */

//the real split of the file
static void splt_s_real_simple_split(splt_state *state,
                                     int *error)
{
  FILE *file_input;
  int split_error = *error;
  char *filename = splt_t_get_filename_to_split(state);
  double splt_beg = splt_t_get_i_begin_point(state);
  double splt_end = splt_t_get_i_end_point(state);
  char *final_fname = NULL;
  final_fname =
    splt_u_get_fname_with_path_and_extension(state,error);
  
  if (error >= 0)
    {
      //if we can open the file
      if ((file_input = fopen(filename, "rb")) != NULL)
        {
#ifndef NO_OGG
          //if the file is ogg
          if (splt_t_get_file_format(state) == SPLT_OGG_FORMAT)
            {
              if((state = splt_s_get_ogg_info(state, file_input,error))
                 != NULL)
                {
                  state->ostate->off = 
                    splt_t_get_float_option(state,SPLT_OPT_PARAM_OFFSET);
                  
                  splt_ogg_put_tags(state, &split_error);
                  
                  if (split_error >= 0)
                    {
                      //effective ogg split
                      splt_ogg_split(final_fname, state,
                                     splt_beg, splt_end,
                                     !state->options.option_input_not_seekable,
                                     state->options.parameter_gap,
                                     state->options.parameter_threshold,
                                     &split_error);
                    }
              
                  splt_ogg_state_free(state);
                }
              else
                {
                  fclose(file_input);
                }
            }
          else
#endif
            //if the file is mp3
            {
              if((state = splt_s_get_mp3_info(state, file_input, error))
                 != NULL)
                {     
                  state->mstate->off = 
                    splt_t_get_float_option(state,SPLT_OPT_PARAM_OFFSET);
              
                  //we initialise frames to 1
                  if (splt_t_get_total_time(state) > 0)
                    {
                      state->mstate->frames = 1;
                    }
              
                  //put id3v1 tags
                  char *id3_data = NULL;
                  id3_data = splt_mp3_get_tags(filename,
                                               state, &split_error);
                      
                  if (split_error >= 0)
                    {
                      //effective mp3 split
                      splt_mp3_split(final_fname, state, id3_data,
                                     splt_beg, splt_end, &split_error);
                      if (id3_data)
                        {
                          free(id3_data);
                        }
                    }
                      
                  //we free the mp3 state after the check
                  splt_mp3_state_free (state);
                }
          
              fclose(file_input);
            }
          
          //if no error
          if (*error >= 0)
            {
              //automatically set progress callback to 100%
              splt_t_update_progress(state,1.0,1.0,1,1,1);
                  
              //we treat the split error
              if (split_error < 0)
                {
                  *error = split_error;
                }
              else
                {
                  //we put the splitted file
                  splt_t_put_splitted_file(state, final_fname);
                  
                  if (*error != SPLT_SPLITPOINT_BIGGER_THAN_LENGTH)
                    {
                      *error = split_error;
                    }
                }
            }
        }
      else
        {
          *error = SPLT_ERROR_CANNOT_OPEN_FILE;
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
static void splt_s_simple_split(splt_state *state, int *error)
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
                                SPLT_OPT_MINS_SECS))
        {
          splt_u_set_complete_new_filename(state,error);
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
	      splt_end = split_end / 100;
	      splt_end += ((split_end % 100) / 100.);
                  
	      splt_t_set_i_begin_point(state,splt_beg);
	      splt_t_set_i_end_point(state,splt_end);
              
	      //we do the real split, if mp3 or ogg..
	      splt_s_real_simple_split(state, error);
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
  int split_result;
  
  //if we have the sync error mode
  //we do a different split than the normal split
  if (splt_t_get_int_option(state, SPLT_OPT_SPLIT_MODE)
      == SPLT_OPTION_MP3_ERROR_MODE)
    {
      splt_t_put_message_to_client(state, SPLT_MESS_START_ERROR_SPLIT);
      
      //mp3,ogg, or..
      int file_format = splt_t_get_file_format(state);
  
#ifndef NO_OGG
      if (file_format == SPLT_OGG_FORMAT)
        {
          *error = SPLT_ERROR_CANNOT_SYNC_OGG;
        }
      else
#endif
        {
          char *filename = splt_t_get_filename_to_split(state);
          FILE *file_input;
          //we try to open the file
          if ((file_input = fopen(filename, "rb")) != NULL)
            {
              //we get the infos for the file
              if((state = splt_s_get_mp3_info(state, file_input, error))
                 != NULL)
                {
                  //we detect sync errors
                  splt_mp3_syncerror_search(state, error);
                  //automatically set progress callback to 100% after
                  //the error detection
                  splt_t_update_progress(state,1.0,1.0,1,1,1);
                  
                  //if we have the default output, we put the
                  //default
                  int err = SPLT_OK;
                  if (splt_t_get_int_option(state,SPLT_OPT_OUTPUT_DEFAULT))
                    {
                      splt_t_set_oformat(state,
                                         SPLT_DEFAULT_SYNCERROR_OUTPUT,&err);
                    }
                  
                  //if no error
                  if (*error >= 0)
                    {
                      //we put the number of sync errors
                      splt_t_set_splitnumber(state, state->mstate->syncerrors+1);
                      
                      //we split all sync errors
                      for (i = 0; i < state->mstate->syncerrors; i++)
                        {
                          //if we don't cancel the split
                          if (!splt_t_split_is_canceled(state))
                            {
                              if (i == 0)
                                {
                                  //we put the current file to split
                                  splt_t_set_current_split(state, i);
                                }
                              
                              //we append a splitpoint
                              splt_t_append_splitpoint(state, 0, "");
                              
                              //we put the output filename
                              splt_u_put_output_filename(state);
                              
                              //we get the final fname
                              final_fname =
                                splt_u_get_fname_with_path_and_extension(state,error);
                              
                              if(*error >= 0)
                                {
                                  splt_t_current_split_next(state);
                                  
                                  //we split with the detected splitpoints
                                  split_result =
                                    splt_mp3_simple_split(state,final_fname, state->serrors->serrors_points[i],
                                                          state->serrors->serrors_points[i+1], NULL);
                                  
                                  //automatically set progress callback to 100%
                                  splt_t_update_progress(state,1.0,1.0,1,1,1);
                                  
                                  //if the split has been a success
                                  if (split_result >= 0)
                                    {
                                      splt_t_put_splitted_file(state, final_fname);
                                    }
                                  
                                  //we put the real error
                                  switch (split_result)
                                    {
                                    case SPLT_OK_SPLITTED_MP3:
                                      *error = SPLT_MP3_SYNC_OK;
                                      break;
                                    case SPLT_ERROR_BEGIN_OUT_OF_FILE:
                                      *error = SPLT_MP3_SYNC_OK;
                                      break;
                                    default:
                                      *error = SPLT_MP3_ERR_SYNC;
                                      goto bloc_end;
                                      break;
                                    }
                                }
                              else
                                {
                                  //error
                                  goto bloc_end;
                                }
                              //free memory
                              free(final_fname);
                              final_fname = NULL;
                            }
                          //if we cancel the split
                          else
                            {
                              *error = SPLT_SPLIT_CANCELLED;
                              goto bloc_end;
                            }
                        }
                    }
                  
                bloc_end:
                  if (final_fname)
                    {
                      free(final_fname);
                    }
                  //we put the syncerrors to 0
                  state->mstate->syncerrors = 0;
                  //we free the mp3 state after the split
                  splt_mp3_state_free (state);
                }
              fclose(file_input);
            }
          else
            {
              *error = SPLT_ERROR_CANNOT_OPEN_FILE;
            }
        }
    }
  else
    //if we don't have the sync error mode
    //we do a normal split
    {
      splt_t_put_message_to_client(state,SPLT_MESS_START_NORMAL_SPLIT);
      
      int old_option_mins = 
        splt_t_get_int_option(state,SPLT_OPT_MINS_SECS);
      int get_error = SPLT_OK;
      
      //for every 2 splitpoints, split
      for(i = 0; i < (splt_t_get_splitnumber(state) - 1); i++)
        {
          //we put the current file to split
          splt_t_set_current_split(state, i);
          
          //if we don't cancel the split
          if (!splt_t_split_is_canceled(state))
            {
              get_error = SPLT_OK;
              
              //if we put the output filename
              /*if (!old_option_mins)
                {
                  splt_u_put_output_filename(state);
                  }*/
              
              char *temp_name = 
                splt_t_get_splitpoint_name(state,i,&get_error);
              
              if (get_error == SPLT_OK)
                {
                  //if we have no filename, we put
                  //option mins secs
                  if ((temp_name == NULL)
                      || (temp_name[0] == '\0'))
                    {
                      splt_t_set_int_option(state, SPLT_OPT_MINS_SECS,
                                            SPLT_TRUE);
                    }
                  else
                    {
                      splt_t_set_int_option(state, SPLT_OPT_MINS_SECS,
                                            old_option_mins);
                    }
                  
                  splt_s_simple_split(state, error);
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
  
  splt_t_put_message_to_client(state,SPLT_MESS_START_TIME_SPLIT);
  
  char *final_fname = NULL;
  //if the state has an error
  int state_null = SPLT_FALSE;
  FILE *file_input;
  int j=0,tracks=1;
  //the begin split
  double begin = 0.f;
  double end = (double)
    splt_t_get_float_option(state, SPLT_OPT_SPLIT_TIME);
  //mp3,ogg, or..
  int file_format = splt_t_get_file_format(state);
  
  //we check if we can open the file in check_if_mp3_or_ogg
  //we open the file
  if ((file_input = fopen(filename, "rb")) != NULL)
    {
      //if the file is mp3 (not ogg)
      if(file_format == SPLT_MP3_FORMAT)
        {
          //we get the infos for the file
          if((state = splt_s_get_mp3_info(state, file_input, error))
             == NULL)
            {
              state_null = SPLT_TRUE;
            }
          
          //for the progress bar
          struct stat file_statistics;
          if(stat(filename, &file_statistics) == 0)
            {
              state->mstate->end2 = file_statistics.st_size;
            }
          else
            {
              *error = SPLT_ERROR_CANNOT_OPEN_FILE;
            }
        }
#ifndef NO_OGG
      else
        //if ogg
        {
          //we get the infos for the file
          if((state = splt_s_get_ogg_info(state, file_input,error))
             == NULL)
            {
              state_null = SPLT_TRUE;
            }
        }
#endif

      //if no state error
      if (!state_null)
        {
          if (state->options.split_time >= 0)
            {
              int err = SPLT_OK;
              
              //initialise frames to 1 after the get_mp3_info
              if (file_format == SPLT_MP3_FORMAT)
                {
                  if (splt_t_get_total_time(state) > 0)
                    {
                      state->mstate->frames = 1;
                    }
                }
              
              int temp_int = 
                (int)floor(((state->split.total_time/100.0) /
                            (state->options.split_time))+1) + 1;
              splt_t_set_splitnumber(state,temp_int);
              
              splt_t_set_oformat_digits(state);
              
              //we append a splitpoint
              splt_t_append_splitpoint(state, 0, "");
              
              //while we have tracks
              do {
                //if we don't cancel the split
                if (!splt_t_split_is_canceled(state))
                  {
                    //we append a splitpoint
                    splt_t_append_splitpoint(state, 0, "");
                    
                    //if we have to put the original tags
                    if (splt_t_get_int_option(state, SPLT_OPT_TAGS) 
                        == SPLT_TAGS_ORIGINAL_FILE)
                      {
                        //we put the original tags to the current split
                        splt_t_append_original_tags(state);
                      }
                    
                    //we set the current split
                    splt_t_set_current_split(state, tracks-1);
                    
                    int current_split = splt_t_get_current_split(state);
                    //if we have the default output
                    if (splt_t_get_int_option(state,
                                              SPLT_OPT_OUTPUT_DEFAULT))
                      {
                        //we put the splitpoints values in the state
                        splt_t_set_splitpoint_value(state,
                                                    current_split,(long)(begin*100));
                        splt_t_set_splitpoint_value(state,
                                                    current_split+1,(long)(end*100));
                        int old_min_value =
                          splt_t_get_int_option(state, SPLT_OPT_MINS_SECS);
                        //we put the names as mins_secs
                        splt_t_set_int_option(state, SPLT_OPT_MINS_SECS,
                                              SPLT_TRUE);
                        //we put the filename
                        splt_u_set_complete_new_filename(state, &err);
                        //we put the names as mins_secs
                        splt_t_set_int_option(state, SPLT_OPT_MINS_SECS,
                                              old_min_value);
                      }
                    else
                      {
                        //if we have the output option
                        //we put the output filename
                        splt_u_put_output_filename(state);
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
                    
                    if (file_format == SPLT_MP3_FORMAT) 
                      {
                        if (*error >= 0)
                          {
                            //put id3
                            char *id3_data = NULL;
                            id3_data = splt_mp3_get_tags(filename,
                                                         state, error);
                            if (*error >= 0)
                              {
                                //we put the end of the file - we need this for the progress bar
                                splt_mp3_split (final_fname, state,
                                                id3_data, begin, end,
                                                error);
                                if (id3_data)
                                  {
                                    free(id3_data);
                                  }
                              }
                          }
                        else
                          {
                            break;
                          }
                      }
#ifndef NO_OGG
                    else
                      {
                        splt_ogg_put_tags(state,error);

                        splt_ogg_split(final_fname, 
                                       state, begin, end, 
                                       !state->options.option_input_not_seekable,
                                       state->options.parameter_gap,
                                       state->options.parameter_threshold,
                                       error);
                      }
#endif
                    //if no error for the split, put the splitted file
                    if (*error >= 0)
                      {
                        splt_t_put_splitted_file(state, 
                                                 final_fname);
                      }
                    
                    //set new splitpoints
                    begin=end;
                    end+= splt_t_get_float_option(state,SPLT_OPT_SPLIT_TIME);
                    tracks++;
                        
                    //get out
                    if ((*error==SPLT_ERROR_BEGIN_OUT_OF_FILE)||
                        (*error==SPLT_MP3_MIGHT_BE_VBR)||
                        (*error==SPLT_OK_SPLITTED_MP3_EOF)||
                        (*error==SPLT_OK_SPLITTED_OGG_EOF))
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
              }  while (j++<tracks);
              
              //free memory
              if (final_fname)
                {
                  free(final_fname);
                }
              
              //we put the mp3 time split error
              switch (*error)
                {
                case SPLT_ERROR_CANNOT_ALLOCATE_MEMORY:
                  *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
                  break;
                case SPLT_MP3_MIGHT_BE_VBR : 
                  *error = SPLT_TIME_SPLIT_OK;
                  break;
                case SPLT_OK_SPLITTED_OGG : 
                  *error = SPLT_TIME_SPLIT_OK;
                  break;
                case SPLT_OK_SPLITTED_MP3 : 
                  *error = SPLT_TIME_SPLIT_OK;
                  break;
                case SPLT_OK_SPLITTED_OGG_EOF : 
                  *error = SPLT_TIME_SPLIT_OK;
                  break;
                case SPLT_OK_SPLITTED_MP3_EOF : 
                  *error = SPLT_TIME_SPLIT_OK;
                  break;
                case SPLT_ERROR_BEGIN_OUT_OF_FILE : 
                  *error = SPLT_TIME_SPLIT_OK;
                  break;
                case SPLT_SPLIT_CANCELLED :
                  *error = SPLT_SPLIT_CANCELLED;
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
      
      //we free the mp3 state after the check
      if(file_format == SPLT_MP3_FORMAT)
        {
          splt_mp3_state_free (state);
        }
#ifndef NO_OGG
      else
        {
          splt_ogg_state_free(state);
        }
#endif
      fclose(file_input);
    }
  else
    {
      *error = SPLT_ERROR_CANNOT_OPEN_FILE;
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
  
  char *filename = splt_t_get_filename_to_split(state);
  //found is the number of silence splits found
  int found = 0;
  //checks if the state is null
  int state_null = SPLT_FALSE;
  struct splt_ssplit *temp = NULL, **list = NULL;
  FILE *file_input;
  int append_error = SPLT_OK;
  //we get some options
  float offset =
    splt_t_get_float_option(state,SPLT_OPT_PARAM_OFFSET);
  float threshold = 
    splt_t_get_float_option(state, SPLT_OPT_PARAM_THRESHOLD);
  float min_length =
    splt_t_get_float_option(state, SPLT_OPT_PARAM_MIN_LENGTH);
  int number_tracks =
    splt_t_get_int_option(state, SPLT_OPT_PARAM_NUMBER_TRACKS);
  
  //we try to open the file
  if((file_input = fopen(filename, "rb")))
    {
      //we get information and scan for silence
      if(splt_t_get_file_format(state) == SPLT_MP3_FORMAT)
        {
          if((state = splt_s_get_mp3_info(state, file_input, error))
             != NULL)
            {
              state->mstate->off = offset;
              
              list = &state->mstate->silence_list;
              found = 
                splt_mp3_scan_silence(state, state->mstate->mp3file.firsthead.ptr, 0,
                                      threshold, min_length, 1);
            }
          else
            {
              state_null = SPLT_TRUE;
              *error = SPLT_ERROR_INVALID_MP3;
            }
        }
#ifndef NO_OGG
      else
        {
          if((state = splt_s_get_ogg_info(state, file_input,error))
             != NULL)
            {
              state->ostate->off = offset;
              
              list = &state->ostate->silence_list;
              found = 
                splt_ogg_scan_silence(state, 0, threshold,
                                      min_length, 1, NULL, 0);
            }
          else
            {
              state_null = SPLT_TRUE;
              *error = SPLT_ERROR_INVALID_OGG;
            }
        }
#endif
      //if we have a state not NULL
      if(!state_null)
        {
          //we set the number of tracks
          int order = 0;
          if (!splt_t_split_is_canceled(state))
            {
              found++;
              if ((number_tracks > 0)&&
                  (number_tracks < SPLT_MAXSILENCE))
                {
                  if (number_tracks < found)
                    {
                      found = number_tracks;
                    }
                }
              
              //put first splitpoint
              append_error = 
                splt_t_append_splitpoint(state, 0, NULL);
              if (append_error != SPLT_OK)
                {
                  *error = append_error;
                }
              else
                {
                  temp = *list;
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
                          long temp_silence_pos = 
                            splt_u_silence_position(temp, offset) *100;
                          append_error =
                            splt_t_append_splitpoint(state, temp_silence_pos,
                                                     NULL);
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
                  append_error = 
                    splt_t_append_splitpoint(state,
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
          
          //free memory if we don't call the write_silence_tracks function
          if (!write_tracks)
            {
              //we free the state
              if (splt_t_get_file_format(state) == SPLT_MP3_FORMAT)
                {
                  fclose(state->mstate->file_input);
                  splt_mp3_state_free (state);
                }
#ifndef NO_OGG
              else
                {
                  splt_ogg_state_free(state);
                }
#endif
            }
        }
      else
        {
          fclose(file_input);
        }
    }
  else
    {
      *error = SPLT_ERROR_WHILE_READING_FILE;
    }
  
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
  //mp3, ogg, or..
  int file_format = splt_t_get_file_format(state);
  
  //put frames to 1 if necessary
  if(file_format == SPLT_MP3_FORMAT)
    {
      if (splt_t_get_total_time(state) > 0)
        {
          state->mstate->frames = 1;
        }
    }
      
  //we put the number of tracks found
  splt_t_set_splitnumber(state, found+1);
  
  //if we have the default output, we put the
  //default
  int err = SPLT_OK;
  if (splt_t_get_int_option(state,SPLT_OPT_OUTPUT_DEFAULT))
    {
      splt_t_set_oformat(state,"Track @n",&err);
    }
  
  //if no error
  if (err >= 0)
    {
      //we write all found tracks
      for (i = 0; i < found; i++)
        {
          if (!splt_t_split_is_canceled(state))
            {
              //we put the output filename
              splt_u_put_output_filename(state);
              
              //we get the output filename+path+extension
              final_fname = 
                splt_u_get_fname_with_path_and_extension(state, &err);
              
              if(err >= 0)
                {
                  long split_begin = 0,split_end = 0;
                  if (splt_t_get_int_option(state, SPLT_OPT_PARAM_REMOVE_SILENCE))
                    {
                      split_begin = 
                        splt_t_get_splitpoint_value(state,2*i,&get_error);
                      split_end = 
                        splt_t_get_splitpoint_value(state,2*i+1,&get_error);
                      
                      if (file_format == SPLT_MP3_FORMAT) 
                        {
                          state->mstate->end = 0;
                        }
#ifndef NO_OGG
                      else 
                        {
                          state->ostate->end = 0;
                        }
#endif
                    }
                  else
                    {
                      split_begin = 
                        splt_t_get_splitpoint_value(state,i,&get_error);
                      split_end = 
                        splt_t_get_splitpoint_value(state,i+1,&get_error);
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
                      
                      //effective silence split
                      if (file_format == SPLT_MP3_FORMAT)
                        {
                          //put id3
                          char *id3_data = NULL;
                          id3_data = splt_mp3_get_tags(filename,
                                                       state,error);
                          
                          if (*error >= 0)
                            {
                              /*splt_t_set_int_option(state,
                                SPLT_OPT_PARAM_GAP, 0);
                                splt_t_set_float_option(state,
                                SPLT_OPT_PARAM_THRESHOLD, 0);*/
                              splt_mp3_split (final_fname, state,
                                              id3_data, beg_pos, end_pos,
                                              error);
                              if (id3_data)
                                {
                                  free(id3_data);
                                }
                            }
                        }
#ifndef NO_OGG
                      else 
                        {
                          splt_ogg_put_tags(state,error);
                          
                          splt_ogg_split(final_fname,
                                         state, beg_pos, end_pos, 
                                         !state->options.option_input_not_seekable, 
                                         0, 0, error);
                        }
#endif
                      //put the splitted file if no error
                      if (*error >= 0)
                        {
                          splt_t_put_splitted_file(state, final_fname);
                        }
                      
                      //we put the silence split errors
                      switch (*error)
                        {
                        case SPLT_MP3_MIGHT_BE_VBR:
                          *error = SPLT_SILENCE_OK;
                          break;
                        case SPLT_OK_SPLITTED_MP3:
                          *error = SPLT_SILENCE_OK;
                          break;
                        case SPLT_OK_SPLITTED_OGG:
                          *error = SPLT_SILENCE_OK;
                          break;
                        case SPLT_OK_SPLITTED_MP3_EOF:
                          *error = SPLT_SILENCE_OK;
                          break;
                        case SPLT_OK_SPLITTED_OGG_EOF:
                          *error = SPLT_SILENCE_OK;
                          break;
                        default:
                          *error = SPLT_ERROR_SILENCE;
                          goto function_end;
                          break;
                        }
                    }
                }
              else
                {
                  goto function_end;
                }
              //free memory
              free(final_fname);
              final_fname = NULL;
            }
          else
            {
              *error = SPLT_SPLIT_CANCELLED;
              goto function_end;
            }
        }
    }
  
 function_end:
  //free memory
  if (final_fname)
    {
      free(final_fname);
    }
  //free the state
  if (file_format == SPLT_MP3_FORMAT)
    {
      splt_mp3_state_free (state);
    }
#ifndef NO_OGG
  else
    {
      splt_ogg_state_free(state);
    }
#endif
}

//do the silence split
//possible error in error
void splt_s_silence_split(splt_state *state, int *error)
{
  splt_u_print_debug("Starting silence split ...",0,NULL);
  
  splt_t_put_message_to_client(state,SPLT_MESS_START_SILENCE_SPLIT);
  
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
          //free the state
          if (splt_t_get_file_format(state) == SPLT_MP3_FORMAT)
            {
              fclose(state->mstate->file_input);
              splt_mp3_state_free (state);
            }
#ifndef NO_OGG
          else
            {
              splt_ogg_state_free(state);
            }
#endif
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
  
  splt_t_put_message_to_client(state,SPLT_MESS_START_WRAP_SPLIT);
  
  //only for mp3
  if(splt_t_get_file_format(state) == SPLT_MP3_FORMAT)
    {
      //we free possibly previous wrapped files
      splt_t_wrap_free(state);
      
      FILE *file_input;
      //we check if we can open the file in check_if_mp3_or_ogg
      //open the file read binary
      if ((file_input = fopen(filename, "rb")) != NULL)
        {
          //SPLT_FALSE means don't list tracks
          //we get the result error or confirmation
          splt_mp3_dewrap(file_input,SPLT_FALSE, new_filename_path, 
                          error, state);
          //close the file
          fclose(file_input);
        }
      else
        {
          *error = SPLT_ERROR_CANNOT_OPEN_FILE;
        }
    }
  else
    {
      *error = SPLT_ERROR_WRAP_NOT_IMPLEMENTED;
    }
}
