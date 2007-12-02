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

//see mp3splt_types.h for error codes and structures
#include "mp3splt_types.h"

/**
 * @file mp3splt.h
 * 
 * @brief main functions
 * 
 * This file contains the main functions
 */

/************************************/
/* Initialisation and free          */

/**
 * @brief Creates a new state structure
 *
 * Creates a new state structure, needed by libmp3splt
 */
splt_state *mp3splt_new_state(int *error);

//this function frees the left variables in the library
//don't forget to call this function ONLY at the end of the program
//returns possible error
void mp3splt_free_state(splt_state *state, int *error);

/************************************/
/* Check functions                  */

/**
 * returns SPLT_TRUE if the library has been compiled with libid3tag 
 * and SPLT_FALSE if the library has not been compiled with libid3tag
 */
short mp3splt_has_id3tag();
/**
 * returns SPLT_TRUE if the library has been compiled with ogg
 * and SPLT_FALSE if the library has not been compiled with ogg
 */
short mp3splt_has_ogg();

/************************************/
/* Set path                         */

//puts the path for the new splitted files
//returns possible error
int mp3splt_set_path_of_split(splt_state *state, char *path);

/************************************/
/* Set filename                     */

//put the filename to split
//returns possible error
int mp3splt_set_filename_to_split(splt_state *state, char *filename);
int mp3splt_set_m3u_filename(splt_state *state, char *filename);

/************************************/
/* Set callback functions           */

int mp3splt_set_message_function(splt_state *state,
    void (*put_message)(int));
int mp3splt_set_splitted_filename_function(splt_state *state,
    void (*file_cb)(char *,int));
int mp3splt_set_progress_function(splt_state *state,
    void (*progress_cb)(splt_progress *p_bar));

/************************************/
/* Splitpoints                      */

//puts a splitpoint
//returns possible error
int mp3splt_append_splitpoint(splt_state *state,
    long split_value, char *name);

//returns a pointer to all the current splitpoints
splt_point *mp3splt_get_splitpoints(splt_state *state,
    int *splitpoints_number,
    int *error);

//erase all the splitpoints
void mp3splt_erase_all_splitpoints(splt_state *state,
    int *error);

/************************************/
/* Tags                             */

//puts a tag
int mp3splt_append_tags(splt_state *state, 
    char *title, char *artist,
    char *album, char *performer,
    char *year, char *comment,
    int track, unsigned char genre);

//returns a pointer to all the current tags
splt_tags *mp3splt_get_tags(splt_state *state,
    int *tags_number,
    int *error);

//puts tags from a string
int mp3splt_put_tags_from_string(splt_state *state, 
    char *tags);

void mp3splt_erase_all_tags(splt_state *state,
    int *error);

/************************************/
/* Options                          */

int mp3splt_set_int_option(splt_state *state, int option_name,
    int value);

int mp3splt_set_float_option(splt_state *state, int option_name,
    float value);

int mp3splt_get_int_option(splt_state *state, int option_name,
    int *error);

float mp3splt_get_float_option(splt_state *state, int option_name,
    int *error);

/************************************/
/* Split functions                  */

//split a ogg or mp3 file
//returns possible error
int mp3splt_split(splt_state *state);

//cancel split function
//returns possible error
void mp3splt_stop_split(splt_state *state,
    int *error);

/************************************/
/*    Cddb and Cue functions        */

//get the cue splitpoints from a file and puts them in the state
void mp3splt_put_cue_splitpoints_from_file(splt_state *state,
    char *cue_file,
    int *error);

//read cddb splitpoints from file and puts them in the state
void mp3splt_put_cddb_splitpoints_from_file(splt_state *state,
    char *cddb_file,
    int *error);

/************************************/
/*    Freedb functions              */

//returns the freedb results and possible eerror
/**
 * @brief test
 */
splt_freedb_results *mp3splt_get_freedb_search(splt_state *state,
    char *searched_string,
    int *error,
    int search_type,
    char search_server[256],
    int port);

void mp3splt_write_freedb_file_result(splt_state *state,
    int disc_id,
    char *cddb_file,
    int *error,
    int cddb_get_type,
    char cddb_get_server[256],
    int port);

//string s is freed, call with strdup for example
void mp3splt_set_oformat(splt_state *state,
    char *format_string,
    int *error);

/************************************/
/* Other utilities                  */

//returns the version of libmp3splt
void mp3splt_get_version(char *version);

//returns the number of syncerrors
//puts possible error in error variable
splt_syncerrors *mp3splt_get_syncerrors(splt_state *state,
    int *error);

//returns the wrapped files found
splt_wrap *mp3splt_get_wrap_files(splt_state *state,
    int *error);

//count how many silence splitpoints we have with silence detection
int mp3splt_count_silence_points(splt_state *state, int *error);
