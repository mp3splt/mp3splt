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

#include "splt.h"

short global_debug = SPLT_FALSE;

/************************************/
/* Initialisation and free          */

//create new state
//error is the possible error
splt_state *mp3splt_new_state(int *error)
{
  splt_state *state = NULL;
  
  //if error is null
  if (error == NULL)
    {
      int err = SPLT_OK;
      state = splt_t_new_state(state,&err);
    }
  else
    {
      state = splt_t_new_state(state,error);
    }
  
  return state;
}

//this function frees the left variables in the library
//call this function ONLY at the end of the program
//and don't forget to call it
//Returns possible error in *error
void mp3splt_free_state(splt_state *state, int *error)
{  
  if (state != NULL)
    {
      if (!splt_t_library_locked(state))
        {
          splt_t_lock_library(state);
          
          splt_t_free_state(state);
        }
      else
        {
          *error = SPLT_ERROR_LIBRARY_LOCKED;
        }
    }
  else
    {
      *error = SPLT_ERROR_STATE_NULL;
    }
}

/************************************/
/* Check functions                  */

//returns if the library has id3tag support or not
short mp3splt_has_id3tag()
{
#ifdef NO_ID3TAG
  return SPLT_FALSE;
#else
  return SPLT_TRUE;
#endif
}

//returns if the library has ogg support or not
short mp3splt_has_ogg()
{
#ifdef NO_OGG
  return SPLT_FALSE;
#else
  return SPLT_TRUE;
#endif
}

/************************************/
/* Set path                         */

//puts the path of the split
//-where the splitted file will be
int mp3splt_set_path_of_split(splt_state *state, char *path)
{
  int error = SPLT_OK;
  
  if (state != NULL)
    {
      if (!splt_t_library_locked(state))
        {
          splt_t_lock_library(state);

          error = splt_t_set_path_of_split(state, path);

          splt_t_unlock_library(state);
        }
      else
        {
          error = SPLT_ERROR_LIBRARY_LOCKED;
        }
    }
  else
    {
      error = SPLT_ERROR_STATE_NULL;
    }
  
  return error;
}

/************************************/
/* Set filename                     */

//puts the filename to split in the state
int mp3splt_set_filename_to_split(splt_state *state,char *filename)
{
  int error = SPLT_OK;

  if (state != NULL)
    {
      if (!splt_t_library_locked(state))
        {
          splt_t_lock_library(state);
      
          error = splt_t_set_filename_to_split(state, filename);

          splt_t_unlock_library(state);
        }
      else
        {
          error = SPLT_ERROR_LIBRARY_LOCKED;
        }
    }
  else
    {
      error = SPLT_ERROR_STATE_NULL;
    }
  
  return error;
}

/************************************/
/* Set callback functions           */

//sets the function that sends messages to the client
//returns possible error
int mp3splt_set_message_function(splt_state *state,
                                 void (*message_cb)(int))
{
  int error = SPLT_OK;
  
  if (state != NULL)
    {
      state->split.put_message = message_cb;
    }
  else
    {
      error = SPLT_ERROR_STATE_NULL;
    }
  
  return error;
}

//sets the function that sends the splitted filename after a split
//returns possible error
int mp3splt_set_splitted_filename_function(splt_state *state,
                                           void (*file_cb)(char *,int b))
{
  int error = SPLT_OK;
  
  if (state != NULL)
    {
      state->split.file_splitted = file_cb;
    }
  else
    {
      error = SPLT_ERROR_STATE_NULL;
    }
  
  return error;
}

//sets the function that sends progress messages to the client
int mp3splt_set_progress_function(splt_state *state,
                                  void (*progress_cb)(splt_progress *p_bar))
{
  int error = SPLT_OK;
  
  if (state != NULL)
    {
      state->split.p_bar->progress = progress_cb;
    }
  else
    {
      error = SPLT_ERROR_STATE_NULL;
    }
  
  return error;
}

/************************************/
/* Splitpoints                      */

//puts a splitpoint in the state with an eventual file name
//split_value is which splitpoint hundreths of seconds
int mp3splt_append_splitpoint(splt_state *state,
                              long split_value, char *name)
{
  int error = SPLT_OK;
  
  if (state != NULL)
    {
      if (!splt_t_library_locked(state))
        {
          splt_t_lock_library(state);
          
          error = splt_t_append_splitpoint(state, split_value, name);
          
          splt_t_unlock_library(state);
        }
      else
        {
          error = SPLT_ERROR_LIBRARY_LOCKED;
        }
    }
  else
    {
      error = SPLT_ERROR_STATE_NULL;
    }
  
  return error;
}

//returns a list containing all the splitpoints
splt_point *mp3splt_get_splitpoints(splt_state *state,
                                    int *splitpoints_number,
                                    int *error)
{
  if (state != NULL)
    {
      return splt_t_get_splitpoints(state,splitpoints_number);
    }
  else
    {
      *error = SPLT_ERROR_STATE_NULL;
      return 0;
    }
}

//erase all the splitpoints
//returns possible errors
void mp3splt_erase_all_splitpoints(splt_state *state, 
                                   int *error)
{
  if (state != NULL)
    {
      if (!splt_t_library_locked(state))
        {
          splt_t_lock_library(state);
      
          splt_t_free_splitpoints(state);
      
          splt_t_unlock_library(state);
        }
      else
        {
          *error = SPLT_ERROR_LIBRARY_LOCKED;
        }
    }
  else
    {
      *error = SPLT_ERROR_STATE_NULL;
    }
}

/************************************/
/* Tags                             */

//append tags
int mp3splt_append_tags(splt_state *state, 
                        char *title, char *artist,
                        char *album, char *performer,
                        char *year, char *comment,
                        int track, unsigned char genre)
{
  int error = SPLT_OK;
  
  if (state != NULL)
    {
      if (!splt_t_library_locked(state))
        {
          splt_t_lock_library(state);
      
          error = splt_t_append_tags(state, title, artist,
                                     album, performer, year, comment,
                                     track, genre);
      
          splt_t_unlock_library(state);
        }
      else
        {
          error = SPLT_ERROR_LIBRARY_LOCKED;
        }
    }
  else
    {
      error = SPLT_ERROR_STATE_NULL;
    }
  
  return error;
}

//returns a list containing all the tags
splt_tags *mp3splt_get_tags(splt_state *state,
                            int *tags_number, 
                            int *error)
{
  if (state != NULL)
    {
      return splt_t_get_tags(state,tags_number);
    }
  else
    {
      *error = SPLT_ERROR_STATE_NULL;
      return NULL;
    }
}

//puts tags from a string
int mp3splt_put_tags_from_string(splt_state *state, 
                                 char *tags)
{
  int error = SPLT_OK;
  
  if (state != NULL)
    {
      if (!splt_t_library_locked(state))
        {
          splt_t_lock_library(state);
      
          error = splt_u_put_tags_from_string(state,tags);
      
          splt_t_unlock_library(state);
        }
      else
        {
          error = SPLT_ERROR_LIBRARY_LOCKED;
        }
    }
  else
    {
      error = SPLT_ERROR_STATE_NULL;
    }
  
  return error;
}

//erase all the tags
void mp3splt_erase_all_tags(splt_state *state,
                            int *error)
{
  if (state != NULL)
    {
      if (!splt_t_library_locked(state))
        {
          splt_t_lock_library(state);
          
          splt_t_free_tags(state);
          
          splt_t_unlock_library(state);
        }
      else
        {
          *error = SPLT_ERROR_LIBRARY_LOCKED;
        }
    }
  else
    {
      *error = SPLT_ERROR_STATE_NULL;
    }
}

/************************************/
/* Options                          */

//set an int option
//returns possible error
int mp3splt_set_int_option(splt_state *state, 
                           int option_name, int value)
{
  int error = SPLT_OK;
  
  if (state != NULL)
    {
      if (!splt_t_library_locked(state))
        {
          splt_t_lock_library(state);
      
          splt_t_set_int_option(state, option_name, value);
      
          splt_t_unlock_library(state);
        }
      else
        {
          error = SPLT_ERROR_LIBRARY_LOCKED;
        }
    }
  else
    {
      error = SPLT_ERROR_STATE_NULL;
    }
  
  return error;
}

//set a float option
//returns possible error
int mp3splt_set_float_option(splt_state *state, 
                             int option_name, float value)
{
  int error = SPLT_OK;
  
  if (state != NULL)
    {
      if (!splt_t_library_locked(state))
        {
          splt_t_lock_library(state);
      
          splt_t_set_float_option(state, option_name, value);

          splt_t_unlock_library(state);
        }
      else
        {
          error = SPLT_ERROR_LIBRARY_LOCKED;
        }
    }
  else
    {
      error = SPLT_ERROR_STATE_NULL;
    }
  
  return error;
}

//get a int option
int mp3splt_get_int_option(splt_state *state, int option_name,
                           int *error)
{
  if (state != NULL)
    {
      return splt_t_get_int_option(state, option_name);
    }
  else
    {
      *error = SPLT_ERROR_STATE_NULL;
      return 0;
    }
}

//get a float option
float mp3splt_get_float_option(splt_state *state, int option_name,
                               int *error)
{
  if (state != NULL)
    {
      return splt_t_get_float_option(state, option_name);
    }
  else
    {
      *error = SPLT_ERROR_STATE_NULL;
      return 0;
    }
}

/************************************/
/* Split functions                  */

//main function, split the file
//splitnumber = how many splits
//returns possible error
int mp3splt_split(splt_state *state)
{
  //the result of the split
  int error = SPLT_OK;

  if (state != NULL)
    {
      if (!splt_t_library_locked(state))
        {
          splt_t_lock_library(state);
          
          splt_u_print_debug("Starting to split file...",0,NULL);
          
          state->cancel_split = SPLT_FALSE;
          
          //we set default internal options
          splt_t_set_default_iopts(state);
          
          splt_u_print_debug("Setting original tags...",0,NULL);
          
          //we retrieve the original tags from the file
          //and save them for future use
          // (for use with the output options)
          splt_t_get_original_tags(state,&error);
          
          if (error < 0)
            {
              splt_t_unlock_library(state);
              return error;
            }
          
          //the new filename path
          char *new_filename_path = NULL;
          char *fname_to_split = 
            splt_t_get_filename_to_split(state);
          
          splt_u_print_debug("Original filename to split is ",0,
                             fname_to_split);
          
          //we put the real splitnumber in the splitnumber variable
          //that could be changed (see splitnumber in mp3splt_types.h)
          state->split.splitnumber = state->split.real_splitnumber;

          //we put splitnumber in the state, we will use it later
          splt_t_set_current_split(state,0);

          //we check if the filename is a real file
          if (!splt_check_is_file(fname_to_split))
            {
              error = SPLT_ERROR_INEXISTENT_FILE;
              splt_t_unlock_library(state);
              return error;
            }
          
          //if the new_filename_path is "", we put the directory of
          //the current song
          new_filename_path =
            splt_check_put_dir_of_cur_song(fname_to_split,
                                           splt_t_get_path_of_split(state));
          
          //we use strdup in the check_put_dir
          //if strdup fails,
          if (new_filename_path == NULL)
            {
              error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
              splt_t_unlock_library(state);
              return error;
            }
          else
            {
              //checks and sets correct options
              splt_check_set_correct_options(state);
              
              //if we have compatible options
              //this function is optional,
              if (!splt_check_compatible_options(state))
                {
                  error = SPLT_ERROR_INCOMPATIBLE_OPTIONS;
                  goto function_end;
                }
              
              splt_check_if_mp3_or_ogg(state, &error);
              
              if (error < 0)
                {
                  goto function_end;
                }
              
              //we put the new filename path in the state
              splt_t_set_new_filename_path(state, new_filename_path,
                                           &error);
              
              if (error < 0)
                {
                  goto function_end;
                }
              
              //we create output directory if it does not exists
              splt_u_create_directory(new_filename_path);
              
              //check means the test is ok
              splt_check_if_new_filename_path_correct(new_filename_path,
                                                      &error);
              
              if (error < 0)
                {
                  goto function_end;
                }
              
              splt_s_put_total_time(state, &error);
              
              if (error < 0)
                {
                  goto function_end;
                }
              
              //the type of the split
              switch (splt_t_get_int_option(state, SPLT_OPT_SPLIT_MODE))
                {
                case SPLT_OPTION_WRAP_MODE:
                  splt_s_wrap_split(state, &error);
                  break;
                case SPLT_OPTION_SILENCE_MODE:
                  splt_s_silence_split(state, &error);
                  break; 
                case SPLT_OPTION_TIME_MODE:
                  splt_s_time_split(state, &error);
                  break;
                default:
                  //this is the normal split or error mode split:
                  //here we also have mp3_error_mode and
                  //mp3_frame_mode 
                  //if we dont have error mode, check if we have
                  //at least 2 splitpoints
                  if (splt_t_get_int_option(state, SPLT_OPT_SPLIT_MODE)
                      != SPLT_OPTION_MP3_ERROR_MODE)
                    {
                      //check if we have at least 2 splitpoints
                      if (splt_t_get_splitnumber(state) < 2)
                        {
                          error = SPLT_ERROR_SPLITPOINTS;
                          goto function_end;
                        }
                    }

                  //we check if the splitpoints are in order
                  splt_check_if_splitpoints_in_order(state, &error);

                  if (error < 0)
                    {
                      goto function_end;
                    }

                  //total time of the song
                  splt_check_splitpts_inf_song_length(state, &error);

                  //if no error, continue
                  if (error < 0)
                    {
                      goto function_end;
                    }
                  
                  //do the effective multiple split
                  splt_s_multiple_split(state, &error);
                  break;
                }
              
            function_end:
              //free memory
              free(new_filename_path);
            }
          
          splt_t_unlock_library(state);
        }
      else
        {
          error = SPLT_ERROR_LIBRARY_LOCKED;
        }
    }
  else
    {
      error = SPLT_ERROR_STATE_NULL;
    }
  
  return error;
}

//cancels the current split
//returns possible errors
void mp3splt_stop_split(splt_state *state, int *error)
{
  if (state != NULL)
    {
      if (!splt_t_library_locked(state))
        {
          splt_t_lock_library(state);
      
          splt_t_set_stop_split(state, SPLT_TRUE);
      
          splt_t_unlock_library(state);
        }
      else
        {
          *error = SPLT_ERROR_LIBRARY_LOCKED;
        }
    }
  else
    {
      *error = SPLT_ERROR_STATE_NULL;
    }
}

/************************************/
/*    Cddb and Cue functions        */

//we get the cue splitpoints from the file
//returns possible error in err
void mp3splt_put_cue_splitpoints_from_file(splt_state *state,
                                           char *file, int *err)
{
  if (state != NULL)
    {
      if (!splt_t_library_locked(state))
        {
          splt_t_lock_library(state);
          
          splt_cue_put_splitpoints(file, state, err);
          
          splt_t_unlock_library(state);
        }
      else
        {
          *err = SPLT_ERROR_LIBRARY_LOCKED;
        }
    }
  else
    {
      *err = SPLT_ERROR_STATE_NULL;
    }
}

//we get the cddb splitpoints from the file
void mp3splt_put_cddb_splitpoints_from_file(splt_state *state,
                                            char *file, int *err)
{
  if (state != NULL)
    {
      if (!splt_t_library_locked(state))
        {
          splt_t_lock_library(state);
      
          splt_cddb_put_splitpoints(file, state, err);
      
          splt_t_unlock_library(state);
        }
      else
        {
          *err = SPLT_ERROR_LIBRARY_LOCKED;
        }
    }
  else
    {
      *err = SPLT_ERROR_STATE_NULL;
    }
}

/************************************/
/*    Freedb functions              */

//get freedb search
splt_freedb_results *mp3splt_get_freedb_search(splt_state *state,
                                               //our search
                                               char *search_string,
                                               //possible errors
                                               int *error)
{
  if (state != NULL)
    {
      //puts the results in "search_results"
      *error = splt_freedb_process_search(state, search_string);
      return state->fdb.search_results;
    }
  else
    {
      *error = SPLT_ERROR_STATE_NULL;
      return NULL;
    }
}

//must be called after get_freedb_search,
//otherwise, it will fail (seg fault!?)
//result must be freed
//returns the content of a cddb file
//you need to write it on the disk in 
//a cddb file to use it
//we return possible errors in err
//the cddb_file is the file to write
void mp3splt_write_freedb_file_result(splt_state *state, int disc_id,
                                      char *cddb_file, int *err)
{
  if (state != NULL)
    {
      if (!splt_t_library_locked(state))
        {
          splt_t_lock_library(state);
      
          char *freedb_file_content;
          freedb_file_content =
            splt_freedb_get_file(state, disc_id, err);
      
          //if no error, write file
          if (*err == SPLT_FREEDB_FILE_OK)
            {
              //we write the result to the file
              FILE *output;
              if (!(output = fopen(cddb_file, "w")))
                {
                  fprintf(stdout,"here1");
                  fflush(stdout);
                  *err = SPLT_CANNOT_WRITE_CDDB_FILE;
                }
              else
                {
                  fprintf(output,"%s",freedb_file_content);
                  fclose(output);
                  free(freedb_file_content);
                }
            }
          
          splt_t_unlock_library(state);
        }
      else
        {
          *err = SPLT_ERROR_LIBRARY_LOCKED;
        }
    }
  else
    {
      *err = SPLT_ERROR_STATE_NULL;
    }
}

//puts output format
void mp3splt_set_oformat(splt_state *state,
                         char *format_string, int *error)
{
  if (state != NULL)
    {
      if (!splt_t_library_locked(state))
        {
          splt_t_lock_library(state);
          
          splt_t_set_oformat(state, format_string, error);  
          
          splt_t_unlock_library(state);
        }
      else
        {
          *error = SPLT_ERROR_LIBRARY_LOCKED;
        }
    }
  else
    {
      *error = SPLT_ERROR_STATE_NULL;
    }
}

/************************************/
/* Other utilities                  */

//returns the number of syncerrors found
//puts error in the error variable
splt_syncerrors *mp3splt_get_syncerrors(splt_state *state,
                                        int *error)
{
  if (state != NULL)
    {
      if (!splt_t_library_locked(state))
        {
          splt_t_lock_library(state);
      
          //if the file is mp3 (not ogg)
          if(splt_t_get_file_format(state) == SPLT_MP3_FORMAT)
            {
              FILE *file_input;
              char *filename = splt_t_get_filename_to_split(state);
              
              //we open the file
              if ((file_input = fopen(filename, "rb")) != NULL)
                {
                  //we get the infos for the file
                  if((state = splt_s_get_mp3_info(state, file_input,
                                                  error)) != NULL)
                    {
                      //we detect sync errors
                      splt_mp3_syncerror_search(state, error);
                      //we put the syncerrors to 0
                      state->mstate->syncerrors = 0;
                      //we free the mp3 state after the check
                      splt_mp3_state_free(state);
                    }
                  fclose(file_input);
                }
              else
                {
                  *error = SPLT_ERROR_CANNOT_OPEN_FILE;
                  splt_t_unlock_library(state);
                  return NULL;
                }
            }
          else
            {
#ifndef NO_OGG
              *error = SPLT_ERROR_CANNOT_SYNC_OGG;
              splt_t_unlock_library(state);
              return NULL;
#endif
            }
          
          splt_t_unlock_library(state);
        }
      else
        {
          *error = SPLT_ERROR_LIBRARY_LOCKED;
        }

      return state->serrors;
    }
  else
    {
      *error = SPLT_ERROR_STATE_NULL;
      return NULL;
    }
}

//puts possible error in the error variable
splt_wrap *mp3splt_get_wrap_files(splt_state *state,
                                  int *error)
{
  if (state != NULL)
    {
      if (!splt_t_library_locked(state))
        {
          splt_t_lock_library(state);
          
          FILE *file_input;
          char *filename = splt_t_get_filename_to_split(state);
          
          //we try to open the file read binary
          if((file_input = fopen(filename, "rb")) != NULL)
            {
              //we check the format of the filename
              splt_check_if_mp3_or_ogg(state, error);
              //if the filename is mp3,
              if(splt_t_get_file_format(state) == SPLT_MP3_FORMAT)
                {
                  //TRUE means we only get the files
                  //first NULL because no directory
                  splt_mp3_dewrap(file_input, SPLT_TRUE, 
                                  NULL, error, state);
                }
              else
                {
                  if (splt_t_get_file_format(state) !=
                      SPLT_INVALID_FORMAT)
                    {
                      *error = SPLT_ERROR_WRAP_NOT_IMPLEMENTED;
                    }
                  splt_t_unlock_library(state);
                  fclose(file_input);
                  return NULL;
                }
              
              //close the file
              fclose(file_input);
            }
          else
            {
              *error = SPLT_ERROR_CANNOT_OPEN_FILE;
              splt_t_unlock_library(state);
              return NULL;
            }
          
          splt_t_unlock_library(state);
        }
      else
        {
          *error = SPLT_ERROR_LIBRARY_LOCKED;
        }
      
      return state->wrap;
    }
  else
    {
      *error = SPLT_ERROR_STATE_NULL;
      return NULL;
    }
}

//count how many silence splitpoints we have with silence detection
int mp3splt_count_silence_points(splt_state *state, int *error)
{
  int found_splitpoints = -1;
  
  if (state != NULL)
    {
      if (!splt_t_library_locked(state))
        {
          splt_t_lock_library(state);
          
          splt_check_if_mp3_or_ogg(state, error);
          
          if (error >= 0)
            {
              found_splitpoints = 
                splt_s_set_silence_splitpoints(state,
                                               SPLT_FALSE, error);
              //the set_silence_splitpoints returns us the
              //number of tracks, not splitpoints
              found_splitpoints--;
            }
          
          splt_t_unlock_library(state);
        }
      else
        {
          *error = SPLT_ERROR_LIBRARY_LOCKED;
        }
    }
  else
    {
      *error = SPLT_ERROR_STATE_NULL;
    }
  
  return found_splitpoints;
}

//returns libmp3splt version, max 20 chars
void mp3splt_get_version(char *version)
{
  snprintf(version,20,"%s",SPLT_PACKAGE_VERSION);
}

//checks the version
void mp3splt_v()
{}
