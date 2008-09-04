/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2006 Alexandru Munteanu - io_alex_2002@yahoo.fr
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

/**
 * creates and returns the new mp3splt state
 * \param error A possible error.
 */
splt_state *mp3splt_new_state(int *error)
{
  splt_state *state = NULL;

  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

  if (lt_dlinit() != 0)
  {
    *err = SPLT_ERROR_CANNOT_INIT_LIBLTDL;
  }
  else
  {
    state = splt_t_new_state(state, err);
  }

  return state;
}

//find plugins and initialise them
//-returns possible error
int mp3splt_find_plugins(splt_state *state)
{
  return splt_p_find_get_plugins_data(state);
}

//this function frees the left variables in the library
//call this function ONLY at the end of the program
//and don't forget to call it
//Returns possible error in *error
void mp3splt_free_state(splt_state *state, int *error)
{  
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

  if (state != NULL)
  {
    if (!splt_t_library_locked(state))
    {
      splt_t_lock_library(state);

      splt_t_free_state(state);
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
/* Set path                         */

//puts the path of the split
//-where the split file will be
int mp3splt_set_path_of_split(splt_state *state, const char *path)
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

//sets the m3u filename
int mp3splt_set_m3u_filename(splt_state *state, const char *filename)
{
  int error = SPLT_OK;

  if (state != NULL)
  {
    if (!splt_t_library_locked(state))
    {
      splt_t_lock_library(state);

      error = splt_t_set_m3u_filename(state, filename);

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

//sets the m3u filename
int mp3splt_set_silence_log_filename(splt_state *state, const char *filename)
{
  int error = SPLT_OK;

  if (state != NULL)
  {
    if (!splt_t_library_locked(state))
    {
      splt_t_lock_library(state);

      error = splt_t_set_silence_log_fname(state, filename);

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


//puts the filename to split in the state
int mp3splt_set_filename_to_split(splt_state *state, const char *filename)
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
    void (*message_cb)(const char *))
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

//sets the function that sends the split filename after a split
//returns possible error
int mp3splt_set_split_filename_function(splt_state *state,
    void (*file_cb)(const char *,int b))
{
  int error = SPLT_OK;

  if (state != NULL)
  {
    state->split.file_split = file_cb;
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

int mp3splt_set_silence_level_function(splt_state *state,
  void (*get_silence_cb)(float level, void *user_data),
  void *data)
{
  int error = SPLT_OK;

  if (state != NULL)
  {
    state->split.get_silence_level = get_silence_cb;
    state->split.silence_level_client_data = data;
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
//if split_value is LONG_MAX, we put the end of the song (EOF)
int mp3splt_append_splitpoint(splt_state *state,
    long split_value, const char *name)
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
const splt_point *mp3splt_get_splitpoints(splt_state *state,
    int *splitpoints_number, int *error)
{
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

  if (state != NULL)
  {
    return splt_t_get_splitpoints(state, splitpoints_number);
  }
  else
  {
    *err = SPLT_ERROR_STATE_NULL;
    return 0;
  }
}

//erase all the splitpoints
//returns possible errors
void mp3splt_erase_all_splitpoints(splt_state *state, int *error)
{
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

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
      *err = SPLT_ERROR_LIBRARY_LOCKED;
    }
  }
  else
  {
    *err = SPLT_ERROR_STATE_NULL;
  }
}

/************************************/
/* Tags                             */

//append tags
int mp3splt_append_tags(splt_state *state, 
    const char *title, const char *artist,
    const char *album, const char *performer,
    const char *year, const char *comment,
    int track, unsigned char genre)
{
  int error = SPLT_OK;

  if (state != NULL)
  {
    if (!splt_t_library_locked(state))
    {
      splt_t_lock_library(state);

      error = splt_t_append_tags(state, title, artist,
          album, performer, year, comment, track, genre);

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
const splt_tags *mp3splt_get_tags(splt_state *state,
    int *tags_number, int *error)
{
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

  if (state != NULL)
  {
    return splt_t_get_tags(state,tags_number);
  }
  else
  {
    *err = SPLT_ERROR_STATE_NULL;
    return NULL;
  }
}

//puts tags from a string
int mp3splt_put_tags_from_string(splt_state *state, const char *tags, int *error)
{
  int ambigous = SPLT_FALSE;
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

  if (state != NULL)
  {
    if (!splt_t_library_locked(state))
    {
      splt_t_lock_library(state);

      ambigous = splt_u_put_tags_from_string(state, tags, err);

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

  return ambigous;
}

//erase all the tags
void mp3splt_erase_all_tags(splt_state *state, int *error)
{
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

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
      *err = SPLT_ERROR_LIBRARY_LOCKED;
    }
  }
  else
  {
    *err = SPLT_ERROR_STATE_NULL;
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
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

  if (state != NULL)
  {
    return splt_t_get_int_option(state, option_name);
  }
  else
  {
    *err = SPLT_ERROR_STATE_NULL;
    return 0;
  }
}

//get a float option
float mp3splt_get_float_option(splt_state *state, int option_name,
    int *error)
{
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

  if (state != NULL)
  {
    return splt_t_get_float_option(state, option_name);
  }
  else
  {
    *err = SPLT_ERROR_STATE_NULL;
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

      //if we have stdin, enable NOT SEEKABLE
      if (splt_t_is_stdin(state))
      {
        splt_t_set_int_option(state, SPLT_OPT_INPUT_NOT_SEEKABLE, SPLT_TRUE);
      }

      state->cancel_split = SPLT_FALSE;

      //we set default internal options
      splt_t_set_default_iopts(state);
 
      //the new filename path
      char *new_filename_path = NULL;
      char *fname_to_split = splt_t_get_filename_to_split(state);

      splt_u_print_debug("Original filename to split is ",0, fname_to_split);

      //we put the real splitnumber in the splitnumber variable
      //that could be changed (see splitnumber in mp3splt.h)
      state->split.splitnumber = state->split.real_splitnumber;

      //we put splitnumber in the state, we will use it later
      splt_t_set_current_split(state,0);

      //we check if the filename is a real file
      int is_file = splt_check_is_file(state, fname_to_split);
      if (!is_file)
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

        int split_type = splt_t_get_int_option(state, SPLT_OPT_SPLIT_MODE);

        //normal split checks
        if (split_type == SPLT_OPTION_NORMAL_MODE)
        {
          //check if we have at least 2 splitpoints
          if (splt_t_get_splitnumber(state) < 2)
          {
            error = SPLT_ERROR_SPLITPOINTS;
            goto function_end;
          }

          //we check if the splitpoints are in order
          splt_check_if_splitpoints_in_order(state, &error);
          if (error < 0) { goto function_end; }
        }

        //we put the new filename path in the state
        splt_t_set_new_filename_path(state, new_filename_path, &error);
        if (error < 0) { goto function_end; }

        //we create output directory if it does not exists
        error = splt_u_create_directory(state, new_filename_path);
        if (error < 0) { goto function_end; }

        //check means the test is ok
        splt_check_if_new_filename_path_correct(state, new_filename_path, &error);
        if (error < 0) { goto function_end; }

        //we check if mp3 or ogg
        splt_check_file_type(state, &error);
        if (error < 0) { goto function_end; }

        //print the new m3u fname
        char *m3u_fname_with_path = splt_t_get_m3u_file_with_path(state, &error);
        if (error < 0) { goto function_end; }
        if (m3u_fname_with_path)
        {
          int malloc_size = strlen(m3u_fname_with_path) + 200;
          char *mess = malloc(sizeof(char) * (strlen(m3u_fname_with_path) + 200));
          if (!mess) { error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY; goto function_end; }
          snprintf(mess, malloc_size, " M3U file '%s' will be created.\n",
              m3u_fname_with_path);
          splt_t_put_message_to_client(state, mess);
          free(m3u_fname_with_path);
          m3u_fname_with_path = NULL;
        }

        //init the plugin for split
        splt_p_init(state, &error);
        if (error < 0) { goto function_end; }

        splt_u_print_debug("Setting original tags...",0,NULL);
        //we retrieve the original tags from the file
        //and save them for future use
        // (for use with the output options)
        splt_t_get_original_tags(state,&error);
        if (error < 0)
        {
          splt_p_end(state);
          goto function_end;
        }

        splt_u_print_debug("parse type of split...",0,NULL);

        char message[1024] = { '\0' };
        //print Working with auto adjust if necessary
        if (splt_t_get_int_option(state, SPLT_OPT_AUTO_ADJUST)
            && !  splt_t_get_int_option(state, SPLT_OPT_QUIET_MODE))
        {
          if ((split_type != SPLT_OPTION_WRAP_MODE)
              && (split_type != SPLT_OPTION_SILENCE_MODE)
              && (split_type != SPLT_OPTION_ERROR_MODE))
          {
            snprintf(message, 2048, " Working with SILENCE AUTO-ADJUST (Threshold:"
                " %.1f dB Gap: %d sec Offset: %.2f)\n",
                splt_t_get_float_option(state, SPLT_OPT_PARAM_THRESHOLD),
                splt_t_get_int_option(state, SPLT_OPT_PARAM_GAP),
                splt_t_get_float_option(state, SPLT_OPT_PARAM_OFFSET));

            splt_t_put_message_to_client(state, message);
          }
        }

        //the type of the split
        switch (split_type)
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
            //here we also have error_mode and frame_mode 

            if (split_type == SPLT_OPTION_NORMAL_MODE)
            {
              //if we don't have STDIN
              if (! splt_t_is_stdin(state))
              {
                //total time of the song
                splt_check_splitpts_inf_song_length(state, &error);
                if (error < 0) { goto function_end; }
              }
            }

            //do the effective multiple split
            splt_s_multiple_split(state, &error);
            break;
        }

        //ends the 'init' of the plugin for the split
        splt_p_end(state);

function_end:
        //free memory
        free(new_filename_path);
        new_filename_path = NULL;
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
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

  if (state != NULL)
  {
    splt_t_set_stop_split(state, SPLT_TRUE);
  }
  else
  {
    *err = SPLT_ERROR_STATE_NULL;
  }
}

/************************************/
/*    Cddb and Cue functions        */

//we get the cue splitpoints from the file
//returns possible error in err
void mp3splt_put_cue_splitpoints_from_file(splt_state *state,
    const char *file, int *error)
{
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

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
    const char *file, int *error)
{
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

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
const splt_freedb_results *mp3splt_get_freedb_search(splt_state *state,
    //our search
    const char *search_string,
    //possible errors
    int *error,
    //the type of the search
    //usually SPLT_SEARCH_TYPE_FREEDB2
    int search_type,
    //if strlen(search_server) == 0, we put the default
    //or null
    const char search_server[256],
    //if port=-1, we use 80
    int port)
{
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

  if (state != NULL)
  {
    //we copy the search string, in order not to modify the original one
    char *search = strdup(search_string);
    if (search != NULL)
    {
      //puts the results in "search_results"
      //for the moment, 1 means search freedb2.org
      *err = splt_freedb_process_search(state, search,
          search_type, search_server, port);

      free(search);
      search = NULL;

      return state->fdb.search_results;
    }
    else
    {
      *err = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      return NULL;
    }
  }
  else
  {
    *err = SPLT_ERROR_STATE_NULL;
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
//
//cddb_get_type specifies the type of the get 
// -it can be SPLT_FREEDB_GET_FILE_TYPE_CDDB_CGI (that works for both
//  freedb and freedb2 at the moment - 18_10_06)
//  or SPLT_FREEDB_GET_FILE_TYPE_CDDB (that only work for freedb at
//  the moment - 18_10_06)
void mp3splt_write_freedb_file_result(splt_state *state, int disc_id,
    const char *cddb_file, int *error, int cddb_get_type,
    const char cddb_get_server[256], int port)
{
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

  if (state != NULL)
  {
    if (!splt_t_library_locked(state))
    {
      splt_t_lock_library(state);

      char *freedb_file_content = NULL;
      freedb_file_content =
        splt_freedb_get_file(state, disc_id, err,
            cddb_get_type,cddb_get_server,port);

      //if no error, write file
      if (*err == SPLT_FREEDB_FILE_OK)
      {
        //we write the result to the file
        FILE *output = NULL;
        if (!(output = fopen(cddb_file, "w")))
        {
          splt_t_set_strerror_msg(state);
          splt_t_set_error_data(state,cddb_file);
          *err = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
        }
        else
        {
          fprintf(output,"%s",freedb_file_content);
          if (fclose(output) != 0)
          {
            splt_t_set_strerror_msg(state);
            splt_t_set_error_data(state, cddb_file);
            *error = SPLT_ERROR_CANNOT_CLOSE_FILE;
          }
          output = NULL;
        }
      }

      //free some memory
      if (freedb_file_content)
      {
        free(freedb_file_content);
        freedb_file_content = NULL;
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
    const char *format_string, int *error)
{
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

  if (state != NULL)
  {
    if (!splt_t_library_locked(state))
    {
      splt_t_lock_library(state);

      splt_t_set_oformat(state, format_string, err);  

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
/* Other utilities                  */

//returns the syncerrors found
//puts error in the error variable
const splt_syncerrors *mp3splt_get_syncerrors(splt_state *state,
    int *error)
{
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

  if (state != NULL)
  {
    if (!splt_t_library_locked(state))
    {
      splt_t_lock_library(state);

      //we check the format of the filename
      splt_check_file_type(state, err);

      if (*err >= 0)
      {
        splt_t_lock_messages(state);
        splt_p_init(state, err);
        if (*err >= 0)
        {
          splt_t_unlock_messages(state);
          splt_p_search_syncerrors(state, err);
          splt_p_end(state);
        }
        else
        {
          splt_t_unlock_messages(state);
        }
      }

      splt_t_unlock_library(state);

      if (*err < 0)
      {
        return NULL;
      }
    }
    else
    {
      *err = SPLT_ERROR_LIBRARY_LOCKED;
      return NULL;
    }

    return state->serrors;
  }
  else
  {
    *err = SPLT_ERROR_STATE_NULL;
    return NULL;
  }
}

//puts possible error in the error variable
const splt_wrap *mp3splt_get_wrap_files(splt_state *state,
    int *error)
{
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

  if (state != NULL)
  {
    if (!splt_t_library_locked(state))
    {
      splt_t_lock_library(state);

      //we check the format of the filename
      splt_check_file_type(state, err);

      int old_split_mode = splt_t_get_int_option(state, SPLT_OPT_SPLIT_MODE);
      splt_t_set_int_option(state, SPLT_OPT_SPLIT_MODE, SPLT_OPTION_WRAP_MODE);
      if (*err >= 0)
      {
        splt_t_lock_messages(state);
        splt_p_init(state, err);
        if (*err >= 0)
        {
          splt_t_unlock_messages(state);
          splt_p_dewrap(state, SPLT_TRUE, NULL, err);
          splt_p_end(state);
        }
        else
        {
          splt_t_unlock_messages(state);
        }
      }
      splt_t_set_int_option(state, SPLT_OPT_SPLIT_MODE, old_split_mode);

      splt_t_unlock_library(state);
    }
    else
    {
      *err = SPLT_ERROR_LIBRARY_LOCKED;
    }

    return state->wrap;
  }
  else
  {
    *err = SPLT_ERROR_STATE_NULL;
    return NULL;
  }
}

//count how many silence splitpoints we have with silence detection
int mp3splt_count_silence_points(splt_state *state, int *error)
{
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

  int found_splitpoints = -1;

  if (state != NULL)
  {
    if (!splt_t_library_locked(state))
    {
      splt_t_lock_library(state);

      splt_check_file_type(state, err);

      if (err >= 0)
      {
        splt_p_init(state, err);
        if (err >= 0)
        {
          found_splitpoints = splt_s_set_silence_splitpoints(state, SPLT_FALSE, err);
          //the set_silence_splitpoints returns us the
          //number of tracks, not splitpoints
          found_splitpoints--;
          splt_p_end(state);
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

  return found_splitpoints;
}

//returns libmp3splt version, max 20 chars
void mp3splt_get_version(char *version)
{
  snprintf(version,20,"%s",SPLT_PACKAGE_VERSION);
}

//result must be freed
char *mp3splt_get_strerror(splt_state *state, int error_code)
{
  return splt_u_strerror(state, error_code);
}

