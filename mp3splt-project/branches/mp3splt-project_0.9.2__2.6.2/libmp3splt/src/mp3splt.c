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

All user-accessible functions

The functions that are actually meant to be called by the user. If
this was C++ it would be the "public" section of our object.

Actually this whole library seems to be written with object orientied
programming in mind: All functions expect the address of the object as
the first parameter etc. Actually changing the library to be c++
should therefore not be too hard a task...
 */

#include <sys/stat.h>
#include <string.h>

#include <ltdl.h>

#include "splt.h"

const char splt_id3v1_genres[SPLT_ID3V1_NUMBER_OF_GENRES][25] = {
  {"Blues"},
  {"Classic Rock"}, {"Country"}, {"Dance"}, 
  {"Disco"},{"Funk"},{"Grunge"},{"Hip-Hop"},{"Jazz"},
  {"Metal"},{"New Age"},{"Oldies"}, {"Other"}, {"Pop"},
  {"R&B"}, {"Rap"}, {"Reggae"}, {"Rock"}, {"Techno"},
  {"Industrial"}, {"Alternative"}, {"Ska"}, {"Death metal"},
  {"Pranks"}, {"Soundtrack"}, {"Euro-Techno"},
  {"Ambient"}, {"Trip-hop"}, {"Vocal"}, {"Jazz+Funk"},
  {"Fusion"}, {"Trance"}, {"Classical"}, {"Instrumental"},
  {"Acid"}, {"House"}, {"Game"}, {"Sound clip"}, {"Gospel"},
  {"Noise"}, {"Alt. Rock"}, {"Bass"}, {"Soul"}, {"Punk"}, 
  {"Space"}, {"Meditative"}, {"Instrumental pop"}, 
  {"Instrumental rock"}, {"Ethnic"}, {"Gothic"},{"Darkwave"},
  {"Techno-Industrial"},{"Electronic"},{"Pop-Folk"},{"Eurodance"},
  {"Dream"},{"Southern Rock"},{"Comedy"}, {"Cult"},{"Gangsta"},
  {"Top 40"},{"Christian Rap"},{"Pop/Funk"}, {"Jungle"},
  {"Native American"},{"Cabaret"},{"New Wave"}, {"Psychedelic"},
  {"Rave"},{"Showtunes"},{"Trailer"}, {"Lo-Fi"},{"Tribal"},
  {"Acid Punk"},{"Acid Jazz"}, {"Polka"}, {"Retro"},
  {"Musical"},{"Rock & Roll"},{"Hard Rock"},

  {"Folk"}, {"Folk-Rock"}, {"National Folk"}, {"Swing"},
  {"Fast Fusion"}, {"Bebob"}, {"Latin"}, {"Revival"},
  {"Celtic"}, {"Bluegrass"}, {"Avantgarde"}, {"Gothic Rock"},
  {"Progressive Rock"}, {"Psychedelic Rock"}, {"Symphonic Rock"},
  {"Slow Rock"}, {"Big Band"}, {"Chorus"}, {"Easy Listening"},
  {"Acoustic"}, {"Humour"}, {"Speech"}, {"Chanson"}, {"Opera"},
  {"Chamber Music"}, {"Sonata"}, {"Symphony"}, {"Booty Bass"},
  {"Primus"}, {"Porn Groove"}, {"Satire"}, {"Slow Jam"},
  {"Club"}, {"Tango"}, {"Samba"}, {"Folklore"}, {"Ballad"},
  {"Power Ballad"}, {"Rhythmic Soul"}, {"Freestyle"}, {"Duet"},
  {"Punk Rock"}, {"Drum Solo"}, {"A capella"}, {"Euro-House"},
  {"Dance Hall"},

  {"misc"},
};

int global_debug = SPLT_FALSE;

/*! 
\defgroup API The API of libmp3splt

This group contains all functions of libmp3splt that are meant to be
accessed by the user

@{
*/

/*! Initialisation

 creates and returns the new central splt_state structure this
 library keeps all its data in. 

 The next step after initialization is to do a
 mp3splt_find_plugins() to find any plugins;

 At the end of the program all data we allocate here is freed by
 calling mp3splt_free_state()

 \param error Contains the error code if an error occours.
*/
splt_state *mp3splt_new_state(splt_code *error)
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

#ifdef ENABLE_NLS
 #ifndef __WIN32__
    bindtextdomain(MP3SPLT_LIB_GETTEXT_DOMAIN, LOCALEDIR);
    bind_textdomain_codeset(MP3SPLT_LIB_GETTEXT_DOMAIN, nl_langinfo(CODESET));
 #else
    bind_textdomain_codeset(MP3SPLT_LIB_GETTEXT_DOMAIN, "UTF-8");
 #endif
#endif

    state = splt_t_new_state(state, err);
  }

  return state;
}

/*! find plugins and initialise them

\return The error code if necessary
*/
splt_code mp3splt_find_plugins(splt_state *state)
{
  return splt_p_find_get_plugins_data(state);
}

/*! this function frees all variables malloc'ed by the library

call this function ONLY at the end of the program
and don't forget to call it.

\param error The code of any error that might have occoured 
\param state the splt_state structure that keeps all data for 
 our library
*/
splt_code mp3splt_free_state(splt_state *state)
{
  if (!state)
  {
    return SPLT_ERROR_STATE_NULL;
  }

  if (!splt_o_library_locked(state))
  {
    splt_o_lock_library(state);
    splt_t_free_state(state);
  }
  else
  {
    return SPLT_ERROR_LIBRARY_LOCKED;
  }

  return SPLT_OK;
}

//! Set the output path
splt_code mp3splt_set_path_of_split(splt_state *state, const char *path)
{
  int error = SPLT_OK;

  if (state != NULL)
  {
    if (!splt_o_library_locked(state))
    {
      splt_o_lock_library(state);

      error = splt_t_set_path_of_split(state, path);

      splt_o_unlock_library(state);
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

/*! Set the m3u filename

\param state the splt_state structure that keeps all our data
\param filename Constains the file name that is to be set
 */
splt_code mp3splt_set_m3u_filename(splt_state *state, const char *filename)
{
  int error = SPLT_OK;

  if (state != NULL)
  {
    if (!splt_o_library_locked(state))
    {
      splt_o_lock_library(state);

      error = splt_t_set_m3u_filename(state, filename);

      splt_o_unlock_library(state);
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

splt_code mp3splt_set_silence_log_filename(splt_state *state, const char *filename)
{
  if (state == NULL)
  {
    return SPLT_ERROR_STATE_NULL;
  }

  if (splt_o_library_locked(state))
  {
    return SPLT_ERROR_LIBRARY_LOCKED;
  }

  splt_o_lock_library(state);

  int error = splt_t_set_silence_log_fname(state, filename);

  splt_o_unlock_library(state);

  return error;
}

splt_code mp3splt_set_silence_full_log_filename(splt_state *state, const char *filename)
{
  if (state == NULL)
  {
    return SPLT_ERROR_STATE_NULL;
  }

  if (splt_o_library_locked(state))
  {
    return SPLT_ERROR_LIBRARY_LOCKED;
  }

  splt_o_lock_library(state);

  int error = splt_t_set_silence_full_log_fname(state, filename);

  splt_o_unlock_library(state);

  return error;
}

/*! \brief Reads out the name of the file that is about to be split

  
 */
const char *mp3splt_get_filename_to_split(splt_state *state)
{
  return splt_t_get_filename_to_split(state); 
}

/*! \brief Sets the name of the file that is about to be split

\param state The central data structure this library keeps its data in
\param filename The filename we want to save in state
*/
splt_code mp3splt_set_filename_to_split(splt_state *state, const char *filename)
{
  int error = SPLT_OK;

  if (state != NULL)
  {
    if (!splt_o_library_locked(state))
    {
      splt_o_lock_library(state);

      error = splt_t_set_filename_to_split(state, filename);

      splt_o_unlock_library(state);
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

splt_code mp3splt_set_input_filename_regex(splt_state *state, const char *regex)
{
  int error = SPLT_OK;

  if (state != NULL)
  {
    if (!splt_o_library_locked(state))
    {
      splt_o_lock_library(state);

      error = splt_t_set_input_filename_regex(state, regex);

      splt_o_unlock_library(state);
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

splt_code mp3splt_set_default_comment_tag(splt_state *state, const char *default_comment)
{
  int error = SPLT_OK;

  if (state != NULL)
  {
    if (!splt_o_library_locked(state))
    {
      splt_o_lock_library(state);

      error = splt_t_set_default_comment_tag(state, default_comment);

      splt_o_unlock_library(state);
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

splt_code mp3splt_set_default_genre_tag(splt_state *state, const char *default_genre_tag)
{
  int error = SPLT_OK;

  if (state != NULL)
  {
    if (!splt_o_library_locked(state))
    {
      splt_o_lock_library(state);

      error = splt_t_set_default_genre_tag(state, default_genre_tag);

      splt_o_unlock_library(state);
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

/*! Register the callback that is called for messages to the client

\param state The central structure this library keeps all its data in
\param message_cb The callback function
\return The error code if any error occours
*/
splt_code mp3splt_set_message_function(splt_state *state, 
    void (*message_cb)(const char *, splt_message_type, void *), 
    void *cb_data)
{
  int error = SPLT_OK;

  if (state != NULL)
  {
    state->split.put_message = message_cb;
    state->split.put_message_cb_data = cb_data;
  }
  else
  {
    error = SPLT_ERROR_STATE_NULL;
  }

  return error;
}

/*! Register the callback called with the split filename after split

\param state The central structure this library keeps all its data in
\param file_cb The callback function
\return The error code if any error occours
*/
splt_code mp3splt_set_split_filename_function(splt_state *state,
    void (*file_cb)(const char *, void *), void *cb_data)
{
  int error = SPLT_OK;

  if (state != NULL)
  {
    state->split.file_split = file_cb;
    state->split.file_split_cb_data = cb_data;
  }
  else
  {
    error = SPLT_ERROR_STATE_NULL;
  }

  return error;
}

splt_code mp3splt_set_pretend_to_split_write_function(splt_state *state,
   void (*write_cb)(const void *ptr, size_t size, size_t nmemb, void *cb_data),
   void *cb_data)
{
  if (state == NULL)
  {
    return SPLT_ERROR_STATE_NULL;
  }

  state->split.write_cb = write_cb;
  state->split.write_cb_data = cb_data;

  return SPLT_OK;
}

/*! Register the callback called with progress messages

\param state The central structure this library keeps all its data in
\param progress_cb The callback function
\return The error code if any error occours
*/
splt_code mp3splt_set_progress_function(splt_state *state,
    void (*progress_cb)(splt_progress *p_bar, void *), void *cb_data)
{
  int error = SPLT_OK;

  if (state != NULL)
  {
    state->split.p_bar->progress = progress_cb;
    state->split.p_bar->progress_cb_data = cb_data;
  }
  else
  {
    error = SPLT_ERROR_STATE_NULL;
  }

  return error;
}

int mp3splt_progress_get_type(const splt_progress *p_bar)
{
  return p_bar->progress_type;
}

char *mp3splt_progress_get_filename_shorted(const splt_progress *p_bar)
{
  if (p_bar->filename_shorted)
  {
    return strdup(p_bar->filename_shorted);
  }

  return NULL;
}

int mp3splt_progress_get_current_split(const splt_progress *p_bar)
{
  return p_bar->current_split;
}

int mp3splt_progress_get_max_splits(const splt_progress *p_bar)
{
  return p_bar->max_splits;
}

int mp3splt_progress_get_silence_found_tracks(const splt_progress *p_bar)
{
  return p_bar->silence_found_tracks;
}

float mp3splt_progress_get_silence_db_level(const splt_progress *p_bar)
{
  return p_bar->silence_db_level;
}

float mp3splt_progress_get_percent_progress(const splt_progress *p_bar)
{
  return p_bar->percent_progress;
}

/*! Register the callback for the function that calculates silence
    levels 

\param state The central structure this library keeps all its data in
\param get_silence_cb The callback function
\return The error code if any error occours
*/
splt_code mp3splt_set_silence_level_function(splt_state *state,
    void (*get_silence_cb)(long time, float level, void *user_data),
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

splt_point *mp3splt_point_new(long splitpoint_value, splt_code *error)
{
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

  splt_point *point = malloc(sizeof(splt_point));
  if (point == NULL)
  {
    *err = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return NULL;
  }

  point->value = splitpoint_value;
  point->name = NULL;
  point->type = SPLT_SPLITPOINT;

  return point;
}

splt_code mp3splt_point_set_name(splt_point *splitpoint, const char *name)
{
  if (splitpoint == NULL || name == NULL)
  {
    return SPLT_OK;
  }

  if (splitpoint->name)
  {
    free(splitpoint->name);
  }

  splitpoint->name = strdup(name);
  if (splitpoint->name == NULL)
  {
    return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  }

  return SPLT_OK;
}

splt_code mp3splt_point_set_type(splt_point *splitpoint, splt_type_of_splitpoint type)
{
  if (splitpoint == NULL)
  {
    return SPLT_OK;
  }

  splitpoint->type = type;

  return SPLT_OK;
}

/*! puts a splitpoint in the state 

\param state The central data structure this library keeps all its
data in
\param name The file name if we want to set it, else NULL
\param split_value The time of this splitpoint in hundreths of
seconds. If this walue is LONG_MAX we put the splitpoint to the end of
the song (EOF)
*/
splt_code mp3splt_append_splitpoint(splt_state *state, splt_point *splitpoint)
{
  int error = SPLT_OK;

  if (state != NULL)
  {
    if (!splt_o_library_locked(state))
    {
      splt_o_lock_library(state);

      error = splt_sp_append_splitpoint(state, 
          splitpoint->value, splitpoint->name, splitpoint->type);

      splt_sp_free_one_splitpoint(splitpoint);

      splt_o_unlock_library(state);
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

/*!returns a list containing all the splitpoints

\param state The structure this library keeps all its data in
\param splitpoints_number Will be set to the number of splitpoints by
this function
\param error Is set to the error code if any error occours
 */
splt_points *mp3splt_get_splitpoints(splt_state *state, splt_code *error)
{
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

  if (state == NULL)
  {
    *err = SPLT_ERROR_STATE_NULL;
    return NULL;
  }

  return splt_sp_get_splitpoints(state);
}

void mp3splt_points_init_iterator(splt_points *splitpoints)
{
  if (splitpoints == NULL)
  {
    return;
  }

  splitpoints->iterator_counter = 0;
}

const splt_point *mp3splt_points_next(splt_points *splitpoints)
{
  if (splitpoints == NULL)
  {
    return NULL;
  }

  if (splitpoints->iterator_counter < 0 || 
      splitpoints->iterator_counter >= splitpoints->real_splitnumber)
  {
    splitpoints->iterator_counter++;
    return NULL;
  }

  splt_point *point = &splitpoints->points[splitpoints->iterator_counter];

  splitpoints->iterator_counter++;

  return point;
}

long mp3splt_point_get_value(const splt_point *point)
{
  return point->value;
}

splt_type_of_splitpoint mp3splt_point_get_type(const splt_point *point)
{
  return point->type;
}

char *mp3splt_point_get_name(const splt_point *point)
{
  if (point->name)
  {
    return strdup(point->name);
  }

  return NULL;
}

/*! erase all the splitpoints

\param state The structure this library keeps all its data in
\param error Is set to the error code if any error occours
*/
splt_code mp3splt_erase_all_splitpoints(splt_state *state)
{
  if (state == NULL)
  {
    return SPLT_ERROR_STATE_NULL;
  }

  if (splt_o_library_locked(state))
  {
    return SPLT_ERROR_LIBRARY_LOCKED;
  }

  splt_o_lock_library(state);
  splt_sp_free_splitpoints(state);
  splt_o_unlock_library(state);

  return SPLT_OK;
}

/************************************/
/* Tags                             */

splt_tags *mp3splt_tags_new(splt_code *error)
{
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

  return splt_tu_new_tags(err);
}

splt_code mp3splt_tags_set(splt_tags *tags, ...)
{
  if (tags == NULL)
  {
    return SPLT_OK;
  }

  int error = SPLT_OK;

  va_list ap;
  va_start(ap, tags);

  splt_tag_key current_key = 0;
  while ((current_key = va_arg(ap, splt_tag_key)))
  {
    const char *current_value = va_arg(ap, const char *);
    error = splt_tu_set_char_field_on_tag(tags, current_key, current_value);
    if (error < 0) { break; }
  }

  va_end(ap);

  return error;
}

char *mp3splt_tags_get(splt_tags *tags, splt_tag_key key)
{
  if (key == SPLT_TAGS_TRACK)
  {
    const int *track = splt_tu_get_tags_value(tags, SPLT_TAGS_TRACK);
    char *track_str = malloc(sizeof(char) * 24);
    if (!track_str) { return NULL; }
    snprintf(track_str, 24, "%d", *track);
    return track_str;
  }

  const char *value = splt_tu_get_tags_value(tags, key);
  if (value == NULL)
  {
    return NULL;
  }

  return strdup(value);
}

splt_code mp3splt_append_tags(splt_state *state, splt_tags *tags)
{
  int error = SPLT_OK;

  if (state == NULL)
  {
    return SPLT_ERROR_STATE_NULL;
  }

  if (splt_o_library_locked(state))
  {
    return SPLT_ERROR_LIBRARY_LOCKED;
  }

  splt_o_lock_library(state);

  splt_tu_append_tags_to_state(state, tags, SPLT_TRUE, SPLT_FALSE, SPLT_TRUE, &error);
  mp3splt_free_one_tag(tags);

  splt_o_unlock_library(state);

  return error;
}

//!returns a list containing all the tags
splt_tags_group *mp3splt_get_tags_group(splt_state *state, splt_code *error)
{
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

  if (state != NULL)
  {
    return splt_tu_get_tags_group(state);
  }
  else
  {
    *err = SPLT_ERROR_STATE_NULL;
    return NULL;
  }
}

splt_code mp3splt_remove_tags_of_skippoints(splt_state *state)
{
  return splt_tu_remove_tags_of_skippoints(state);
}

void mp3splt_tags_group_init_iterator(splt_tags_group *tags_group)
{
  if (tags_group == NULL)
  {
    return;
  }

  tags_group->iterator_counter = 0;
}

splt_tags *mp3splt_tags_group_next(splt_tags_group *tags_group)
{
  if (tags_group == NULL)
  {
    return NULL;
  }

  if (tags_group->iterator_counter < 0 || 
      tags_group->iterator_counter >= tags_group->real_tagsnumber)
  {
    tags_group->iterator_counter++;
    return NULL;
  }

  return &tags_group->tags[tags_group->iterator_counter++];
}

splt_code mp3splt_read_original_tags(splt_state *state)
{
  int error = SPLT_OK;

  if (state == NULL)
  {
    return SPLT_ERROR_STATE_NULL;
  }

  if (splt_o_library_locked(state))
  {
    return SPLT_ERROR_LIBRARY_LOCKED;
  }

  splt_o_lock_library(state);

  splt_check_file_type_and_set_plugin(state, SPLT_FALSE, SPLT_FALSE, &error);
  if (error < 0) { goto end; }

  splt_o_lock_messages(state);

  splt_p_init(state, &error);
  if (error < 0) { goto end; }

  splt_tu_get_original_tags(state, &error);
  if (error < 0) { goto end; }

  splt_p_end(state, &error);
  if (error < 0) { goto end; }

end:
  splt_o_unlock_messages(state);
  splt_o_unlock_library(state);

  return error;
}

//!puts tags from a string
int mp3splt_put_tags_from_string(splt_state *state, const char *tags, splt_code *error)
{
  int ambigous = SPLT_FALSE;
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

  if (state != NULL)
  {
    if (!splt_o_library_locked(state))
    {
      splt_o_lock_library(state);

      ambigous = splt_tp_put_tags_from_string(state, tags, err);

      splt_o_unlock_library(state);
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

//!erase all the tags
splt_code mp3splt_erase_all_tags(splt_state *state)
{
  if (state == NULL)
  {
    return SPLT_ERROR_STATE_NULL;
  }

  if (splt_o_library_locked(state))
  {
    return SPLT_ERROR_LIBRARY_LOCKED;
  }

  splt_o_lock_library(state);
  splt_tu_free_tags(state);
  splt_o_unlock_library(state);

  return SPLT_OK;
}

/************************************/
/* Options                          */

static int mp3splt_set_option(splt_state *state, splt_options option_name, void *value)
{
  int error = SPLT_OK;

  if (state != NULL)
  {
    if (!splt_o_library_locked(state))
    {
      splt_o_lock_library(state);

      splt_o_set_option(state, option_name, value);

      splt_o_unlock_library(state);
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

splt_code mp3splt_set_int_option(splt_state *state, splt_options option_name, int value)
{
  return mp3splt_set_option(state, option_name, &value);
}

splt_code mp3splt_set_long_option(splt_state *state, splt_options option_name, long value)
{
  return mp3splt_set_option(state, option_name, &value);
}

splt_code mp3splt_set_float_option(splt_state *state, splt_options option_name, float value)
{
  return mp3splt_set_option(state, option_name, &value);
}

int mp3splt_get_int_option(splt_state *state, splt_options option_name, splt_code *error)
{
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

  if (state != NULL)
  {
    return splt_o_get_int_option(state, option_name);
  }
  else
  {
    *err = SPLT_ERROR_STATE_NULL;
    return 0;
  }
}

long mp3splt_get_long_option(splt_state *state, splt_options option_name, splt_code *error)
{
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

  if (state != NULL)
  {
    return splt_o_get_long_option(state, option_name);
  }
  else
  {
    *err = SPLT_ERROR_STATE_NULL;
    return 0;
  }
}

float mp3splt_get_float_option(splt_state *state, splt_options option_name, splt_code *error)
{
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

  if (state != NULL)
  {
    return splt_o_get_float_option(state, option_name);
  }
  else
  {
    *err = SPLT_ERROR_STATE_NULL;
    return 0;
  }
}

/************************************/
/* Split functions                  */

/*! main function: Call it and it splits the file for you

state-> splitnumber tells how many splits to do
\return the error code if any error has occoured
*/
splt_code mp3splt_split(splt_state *state)
{
  int error = SPLT_OK;

  if (state != NULL)
  {
    if (!splt_o_library_locked(state))
    {
      splt_o_lock_library(state);

      splt_d_print_debug(state,"Starting to split file ...\n");

      if (splt_o_get_int_option(state, SPLT_OPT_HANDLE_BIT_RESERVOIR))
      {
        splt_c_put_warning_message_to_client(state,
            _(" warning: bit reservoir handling for gapless playback is still experimental\n"));

        if (splt_o_get_long_option(state, SPLT_OPT_OVERLAP_TIME) > 0)
        {
          splt_c_put_warning_message_to_client(state, 
              _(" warning: bit reservoir is not compatible with overlap option\n"));
        }

        if (splt_o_get_int_option(state, SPLT_OPT_AUTO_ADJUST))
        {
          splt_c_put_warning_message_to_client(state, 
              _(" warning: bit reservoir is not compatible with auto adjust option\n"));
        }

        if (splt_o_get_int_option(state, SPLT_OPT_INPUT_NOT_SEEKABLE))
        {
          splt_c_put_warning_message_to_client(state, 
              _(" warning: bit reservoir is not compatible with input not seekable\n"));
        }

        int supported_split_mode = SPLT_TRUE;
        int split_mode = splt_o_get_int_option(state, SPLT_OPT_SPLIT_MODE);
        if ((split_mode == SPLT_OPTION_SILENCE_MODE) || (split_mode == SPLT_OPTION_TRIM_SILENCE_MODE))
        {
          supported_split_mode = SPLT_FALSE;
        }

        if (!supported_split_mode)
        {
          splt_c_put_warning_message_to_client(state, 
              _(" warning: bit reservoir is not compatible with silence detection or trimming\n"));
        }

        int with_xing = splt_o_get_int_option(state, SPLT_OPT_XING);
        if (!with_xing)
        {
          splt_c_put_warning_message_to_client(state, 
              _(" warning: bit reservoir is not compatible with 'no xing'\n"));
        }

        int with_frame_mode = splt_o_get_int_option(state, SPLT_OPT_FRAME_MODE);
        if (!with_frame_mode)
        {
          splt_c_put_warning_message_to_client(state, 
              _(" warning: please enable frame mode to make bit reservoir work\n"));
        }
      }

      char *new_filename_path = NULL;
      char *fname_to_split = splt_t_get_filename_to_split(state);

      splt_d_print_debug(state,"Original filename/path to split is _%s_\n", fname_to_split);

      if (splt_io_input_is_stdin(state))
      {
        splt_o_set_int_option(state, SPLT_OPT_INPUT_NOT_SEEKABLE, SPLT_TRUE);
      }

      splt_t_set_stop_split(state, SPLT_FALSE);

      splt_o_set_default_iopts(state);

      //we put the real splitnumber in the splitnumber variable
      //that could be changed (see splitnumber in mp3splt.h)
      if (!state->split.points)
      {
        state->split.splitnumber = 0;
      }
      else
      {
        state->split.splitnumber = state->split.points->real_splitnumber;
      }

      splt_t_set_current_split(state,0);

      if (!splt_io_check_if_file(state, fname_to_split))
      {
        error = SPLT_ERROR_INEXISTENT_FILE;
        splt_o_unlock_library(state);
        return error;
      }

      //if the new_filename_path is "", we put the directory of
      //the current song
      new_filename_path = splt_check_put_dir_of_cur_song(fname_to_split,
          splt_t_get_path_of_split(state), &error);
      if (error < 0)
      {
        splt_o_unlock_library(state);
        return error;
      }

#ifndef __WIN32__
      char *linked_fname = splt_io_get_linked_fname(fname_to_split, NULL);
      if (linked_fname)
      {
        splt_c_put_info_message_to_client(state, 
            _(" info: resolving linked filename to '%s'\n"), linked_fname);

        splt_t_set_filename_to_split(state, linked_fname);
        fname_to_split = splt_t_get_filename_to_split(state);

        free(linked_fname);
        linked_fname = NULL;
      }
#endif

      //checks and sets correct options
      splt_check_set_correct_options(state);

      //if we have compatible options
      //this function is optional,
      if (! splt_check_compatible_options(state))
      {
        error = SPLT_ERROR_INCOMPATIBLE_OPTIONS;
        goto function_end;
      }

      int split_type = splt_o_get_int_option(state, SPLT_OPT_SPLIT_MODE);

      splt_t_set_new_filename_path(state, new_filename_path, &error);
      if (error < 0) { goto function_end; }

      splt_d_print_debug(state, "new fname path = _%s_\n", new_filename_path); 

      error = splt_io_create_directories(state, new_filename_path);
      if (error < 0) { goto function_end; }

      splt_check_if_fname_path_is_correct(state, new_filename_path, &error);
      if (error < 0) { goto function_end; }

      //we check if mp3 or ogg
      splt_check_file_type_and_set_plugin(state, SPLT_FALSE, SPLT_TRUE, &error);
      if (error < 0) { goto function_end; }

      int tags_option = splt_o_get_int_option(state, SPLT_OPT_TAGS);
      if (tags_option == SPLT_TAGS_ORIGINAL_FILE)
      {
        splt_tp_put_tags_from_string(state, SPLT_ORIGINAL_TAGS_DEFAULT, &error);
        if (error < 0) { goto function_end; }
      }
      else if (tags_option == SPLT_TAGS_FROM_FILENAME_REGEX)
      {
        int regex_error = SPLT_OK;
        splt_tp_put_tags_from_filename(state, &regex_error);
        if (regex_error < 0) { error = regex_error; goto function_end; }
      }

      const char *plugin_name = splt_p_get_name(state,&error);
      if (error < 0) { goto function_end; }

      splt_c_put_info_message_to_client(state, 
          _(" info: file matches the plugin '%s'\n"), plugin_name);

      //print the new m3u fname
      char *m3u_fname_with_path = splt_t_get_m3u_file_with_path(state, &error);
      if (error < 0) { goto function_end; }
      if (m3u_fname_with_path)
      {
        splt_c_put_info_message_to_client(state, 
            _(" M3U file '%s' will be created.\n"), m3u_fname_with_path);

        free(m3u_fname_with_path);
        m3u_fname_with_path = NULL;
      }

      //init the plugin for split
      splt_p_init(state, &error);
      if (error < 0) { goto function_end; }

      splt_d_print_debug(state,"Parse type of split ...\n");

      if (splt_o_get_int_option(state, SPLT_OPT_AUTO_ADJUST)
          && !splt_o_get_int_option(state, SPLT_OPT_QUIET_MODE))
      {
        if ((split_type != SPLT_OPTION_WRAP_MODE)
            && (split_type != SPLT_OPTION_SILENCE_MODE)
            && (split_type != SPLT_OPTION_TRIM_SILENCE_MODE)
            && (split_type != SPLT_OPTION_ERROR_MODE))
        {
          splt_c_put_info_message_to_client(state, 
              _(" Working with SILENCE AUTO-ADJUST (Threshold:"
                " %.1f dB Gap: %d sec Offset: %.2f Min: %.2f sec)\n"),
              splt_o_get_float_option(state, SPLT_OPT_PARAM_THRESHOLD),
              splt_o_get_int_option(state, SPLT_OPT_PARAM_GAP),
              splt_o_get_float_option(state, SPLT_OPT_PARAM_OFFSET),
              splt_o_get_float_option(state, SPLT_OPT_PARAM_MIN_LENGTH));
        }
      }

      switch (split_type)
      {
        case SPLT_OPTION_WRAP_MODE:
          splt_s_wrap_split(state, &error);
          break;
        case SPLT_OPTION_SILENCE_MODE:
          splt_s_silence_split(state, &error);
          break; 
        case SPLT_OPTION_TRIM_SILENCE_MODE:
          splt_s_trim_silence_split(state, &error);
          break;
        case SPLT_OPTION_TIME_MODE:
          splt_s_time_split(state, &error);
          break;
        case SPLT_OPTION_LENGTH_MODE:
          splt_s_equal_length_split(state, &error);
          break;
        case SPLT_OPTION_ERROR_MODE:
          splt_s_error_split(state, &error);
          break;
        default:
          if (split_type == SPLT_OPTION_NORMAL_MODE)
          {
            if (! splt_o_get_int_option(state, SPLT_OPT_PRETEND_TO_SPLIT))
            {
              //check if we have at least 2 splitpoints
              if (splt_t_get_splitnumber(state) < 2)
              {
                error = SPLT_ERROR_SPLITPOINTS;
                goto function_end;
              }
            }

            splt_check_points_inf_song_length_and_convert_negatives(state, &error);
            if (error < 0) { goto function_end; }

            splt_check_if_points_in_order(state, &error);
            if (error < 0) { goto function_end; }
          }

          splt_s_normal_split(state, &error);
          break;
      }

      //ends the 'init' of the plugin for the split
      splt_p_end(state, &error);

function_end:
      if (new_filename_path)
      {
        free(new_filename_path);
        new_filename_path = NULL;
      }

      splt_o_unlock_library(state);
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

/*! cancels the current split
\return The error code if any error occours
*/
splt_code mp3splt_stop_split(splt_state *state)
{
  if (state == NULL)
  {
    return SPLT_ERROR_STATE_NULL;
  }

  splt_t_set_stop_split(state, SPLT_TRUE);

  return SPLT_OK;
}

/************************************/
/*    Cddb and Cue functions        */

/*! Fetch the splitpoints from an input file.
*/
splt_code mp3splt_import(splt_state *state, splt_import_type type, const char *file)
{
  if (state == NULL)
  {
    return SPLT_ERROR_STATE_NULL;
  }

  if (splt_o_library_locked(state))
  {
    return SPLT_ERROR_LIBRARY_LOCKED;
  }

  splt_o_lock_library(state);
  int err = SPLT_OK;

  if (type == CUE_IMPORT)
  {
    splt_cue_put_splitpoints(file, state, &err);
  }
  else if (type == CDDB_IMPORT)
  {
    splt_cddb_put_splitpoints(file, state, &err);
  }
  else if (type == AUDACITY_LABELS_IMPORT)
  {
    splt_audacity_put_splitpoints(file, state, &err);
  }
  else if (type == PLUGIN_INTERNAL_IMPORT)
  {
    char *old_filename_to_split = strdup(mp3splt_get_filename_to_split(state));
    if (old_filename_to_split == NULL)
    {
      err = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      goto end;
    }

    err = splt_t_set_filename_to_split(state, file);
    if (err < 0) { goto end2; }

    splt_check_file_type_and_set_plugin(state, SPLT_TRUE, SPLT_FALSE, &err);
    if (err >= 0)
    {
      splt_t_free_splitpoints_tags(state);
      splt_p_import_internal_sheets(state, &err);
    }

    splt_t_set_filename_to_split(state, old_filename_to_split);

end2:
    free(old_filename_to_split);
  }

end:
  splt_o_unlock_library(state);

  return err;
}

/************************************/
/*    Freedb functions              */

splt_code mp3splt_use_proxy(splt_state *state, const char *proxy_address, int proxy_port)
{
  return splt_pr_use_proxy(state, proxy_address, proxy_port);
}

splt_code mp3splt_use_base64_authentification(splt_state *state, 
    const char *base64_authentification)
{
  return splt_pr_use_base64_authentification(state, base64_authentification);
}

char *mp3splt_encode_in_base64(splt_state *state, const char *input, int *error)
{
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

  char *input_as_base64 = splt_pr_base64((unsigned char *)input);
  if (input_as_base64 == NULL)
  {
    *err = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return NULL;
  }

  return input_as_base64;
}

void mp3splt_clear_proxy(splt_state *state)
{
  splt_pr_free(state);
}

/*!Do a freedb search

After dong the search continue by calling
mp3splt_write_freedb_file_result().
\param state The central structure this library keeps all its data in
\param search_string The string that is to be searched for
\param error The error code this action returns in
\param search_type the type of the search. Usually set to
SPLT_SEARCH_TYPE_FREEDB2
\param search_server The URL of the search server or NULL to select
the default
\param port The port on the server. -1 means default (Which should be
80). 
 */
splt_freedb_results *mp3splt_get_freedb_search(splt_state *state,
    const char *search_string,
    splt_code *error,
    int search_type,
    const char search_server[256],
    int port)
{
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

  if (search_string == NULL)
  {
    *err = SPLT_FREEDB_NO_CD_FOUND;
    return NULL;
  }
 
  if (state != NULL)
  {
    //we copy the search string, in order not to modify the original one
    char *search = strdup(search_string);
    if (search != NULL)
    {
      *err = splt_freedb_process_search(state, search, search_type,
          search_server, port);

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

void mp3splt_freedb_init_iterator(splt_freedb_results *freedb_results)
{
  if (freedb_results == NULL)
  {
    return;
  }

  freedb_results->iterator_counter = 0;
}

const splt_freedb_one_result *mp3splt_freedb_next(splt_freedb_results *freedb_results)
{
  if (freedb_results == NULL)
  {
    return NULL;
  }

  if (freedb_results->iterator_counter < 0 || 
      freedb_results->iterator_counter >= freedb_results->number)
  {
    freedb_results->iterator_counter++;
    return NULL;
  }

  splt_freedb_one_result *result = &freedb_results->results[freedb_results->iterator_counter];

  freedb_results->iterator_counter++;

  return result;
}

int mp3splt_freedb_get_id(const splt_freedb_one_result *result)
{
  return result->id;
}

const char *mp3splt_freedb_get_name(const splt_freedb_one_result *result)
{
  return result->name;
}

int mp3splt_freedb_get_number_of_revisions(const splt_freedb_one_result *result)
{
  return result->revision_number;
}

/*! returns the content of a cddb file

must only be called \emph{after} running get_freedb_search
otherwise, it will fail (seg fault!?)

you need to write the cddb entry to the disk in 
a cddb file to use it be able to use it

\attention result is malloc'ed and must be freed by the caller after
use.
*/
splt_code mp3splt_write_freedb_file_result(splt_state *state, int disc_id,
    const char *cddb_file, int cddb_get_type,
    const char cddb_get_server[256], int port)
{
  int erro = SPLT_OK;
  int *err = &erro;

  if (state != NULL)
  {
    if (!splt_o_library_locked(state))
    {
      splt_o_lock_library(state);

      char *freedb_file_content = NULL;
      freedb_file_content = splt_freedb_get_file(state, disc_id, err,
          cddb_get_type, cddb_get_server, port);

      //if no error, write file
      if (*err == SPLT_FREEDB_FILE_OK)
      {
        if (! splt_o_get_int_option(state, SPLT_OPT_PRETEND_TO_SPLIT))
        {
          //we write the result to the file
          FILE *output = NULL;
          if (!(output = splt_io_fopen(cddb_file, "w")))
          {
            splt_e_set_strerror_msg_with_data(state, cddb_file);
            *err = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
          }
          else
          {
            fprintf(output,"%s",freedb_file_content);
            if (fclose(output) != 0)
            {
              splt_e_set_strerror_msg_with_data(state, cddb_file);
              *err = SPLT_ERROR_CANNOT_CLOSE_FILE;
            }
            output = NULL;
          }
        }
      }

      //free some memory
      if (freedb_file_content)
      {
        free(freedb_file_content);
        freedb_file_content = NULL;
      }

      splt_o_unlock_library(state);
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

  return *err;
}

/*! Export our split points to a cue file
*/
splt_code mp3splt_export(splt_state *state, splt_export_type type,
    const char *out_file, int stop_at_total_time)
{
  if (state == NULL)
  {
    return SPLT_ERROR_STATE_NULL;
  }

  if (splt_o_library_locked(state))
  {
    return SPLT_ERROR_LIBRARY_LOCKED;
  }

  splt_o_lock_library(state);
  int err = SPLT_OK;

  if (type == CUE_EXPORT)
  {
    splt_cue_export_to_file(state, out_file, stop_at_total_time, &err);
  }

  splt_o_unlock_library(state);

  return err;
}

//! Sets the output format
splt_code mp3splt_set_oformat(splt_state *state, const char *format_string)
{
  if (state == NULL)
  {
    return SPLT_ERROR_STATE_NULL;
  }

  if (splt_o_library_locked(state))
  {
    return SPLT_ERROR_LIBRARY_LOCKED;
  }

  splt_o_lock_library(state);

  int err = SPLT_OK;
  splt_of_set_oformat(state, format_string, &err, SPLT_FALSE);

  splt_o_unlock_library(state);

  return err;
}

/************************************/
/* Other utilities                  */

/*! Split a file that has been generated by mp3wrap

  \param state The splt_state structure containing the split points
  \param error The error code
*/
splt_wrap *mp3splt_get_wrap_files(splt_state *state, splt_code *error)
{
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

  if (state != NULL)
  {
    if (!splt_o_library_locked(state))
    {
      splt_o_lock_library(state);

      //we check the format of the filename
      splt_check_file_type_and_set_plugin(state, SPLT_FALSE, SPLT_FALSE, err);

      int old_split_mode = splt_o_get_int_option(state, SPLT_OPT_SPLIT_MODE);
      splt_o_set_int_option(state, SPLT_OPT_SPLIT_MODE, SPLT_OPTION_WRAP_MODE);
      if (*err >= 0)
      {
        splt_o_lock_messages(state);
        splt_p_init(state, err);
        if (*err >= 0)
        {
          splt_o_unlock_messages(state);
          splt_p_dewrap(state, SPLT_TRUE, NULL, err);
          splt_p_end(state, err);
        }
        else
        {
          splt_o_unlock_messages(state);
        }
      }
      splt_o_set_int_option(state, SPLT_OPT_SPLIT_MODE, old_split_mode);

      splt_o_unlock_library(state);
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

void mp3splt_wrap_init_iterator(splt_wrap *wrap)
{
  if (!wrap)
  {
    return;
  }

  wrap->iterator_counter = 0;
}

const splt_one_wrap *mp3splt_wrap_next(splt_wrap *wrap)
{
  if (!wrap)
  {
    return NULL;
  }

  if (wrap->iterator_counter < 0 || 
      wrap->iterator_counter >= wrap->wrap_files_num)
  {
    wrap->iterator_counter++;
    return NULL;
  }

  splt_one_wrap *one_wrap = (splt_one_wrap *) wrap->wrap_files[wrap->iterator_counter];

  wrap->iterator_counter++;

  return one_wrap;
}

char *mp3splt_wrap_get_wrapped_file(const splt_one_wrap *one_wrap)
{
  if (!one_wrap)
  {
    return NULL;
  }

  return strdup((const char *)one_wrap);
}

//!set the silence splitpoints without actually splitting
int mp3splt_set_silence_points(splt_state *state, splt_code *error)
{
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

  int silence_mode = SPLT_OPTION_SILENCE_MODE;
  mp3splt_set_option(state, SPLT_OPT_SPLIT_MODE, &silence_mode);

  int found_splitpoints = -1;

  if (state != NULL)
  {
    if (!splt_o_library_locked(state))
    {
      splt_o_lock_library(state);

      splt_t_set_stop_split(state, SPLT_FALSE);

      splt_check_file_type_and_set_plugin(state, SPLT_FALSE, SPLT_FALSE, err);

      if (*err >= 0)
      {
        splt_p_init(state, err);
        if (*err >= 0)
        {
          found_splitpoints = splt_s_set_silence_splitpoints(state, err);
          splt_p_end(state, err);
        }
      }

      splt_o_unlock_library(state);
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

//!set the trim silence splitpoints without actually splitting
splt_code mp3splt_set_trim_silence_points(splt_state *state)
{
  int erro = SPLT_OK;
  int *err = &erro;

  int silence_mode = SPLT_OPTION_TRIM_SILENCE_MODE;
  mp3splt_set_option(state, SPLT_OPT_SPLIT_MODE, &silence_mode);

  if (state != NULL)
  {
    if (!splt_o_library_locked(state))
    {
      splt_o_lock_library(state);

      splt_t_set_stop_split(state, SPLT_FALSE);

      splt_check_file_type_and_set_plugin(state, SPLT_FALSE, SPLT_FALSE, err);

      if (*err >= 0)
      {
        splt_p_init(state, err);
        if (*err >= 0)
        {
          splt_s_set_trim_silence_splitpoints(state, err);
          splt_p_end(state, err);
        }
      }

      splt_o_unlock_library(state);
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

  return *err;
}

//!returns libmp3splt version; result must be freed
char *mp3splt_get_version()
{
  return strdup(SPLT_PACKAGE_VERSION);
}

/*! Returns the error string that matches an error code

\param state The splt_state The central structure containing all data
for our library
\param error The error code

\attention The resulting string is malloc'ed and must be freed by the
caller after use.
*/

char *mp3splt_get_strerror(splt_state *state, splt_code error_code)
{
  return splt_e_strerror(state, error_code);
}

/*! Tell where to scan for plug-ins

\param state The splt_state The central structure containing all data
for our library
\return The error code in case that anything goes wrong, else SPLT_OK
*/
splt_code mp3splt_append_plugins_scan_dir(splt_state *state, const char *dir)
{
  return splt_p_append_plugin_scan_dir(state, dir);
}

#ifdef __WIN32__
/*! Converts a string from win32 to utf8

\param The UTF16 string from Windows 
\result The same string in UTF8
\attention The resulting string is malloc'ed and must be freed by the
caller after use.
*/
char *mp3splt_win32_utf16_to_utf8(const wchar_t *source)
{
  return splt_w32_utf16_to_utf8(source);
}
#endif

char **mp3splt_find_filenames(splt_state *state, const char *filename,
    int *num_of_files_found, splt_code *error)
{
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

  char **found_files = NULL;

  if (state != NULL)
  {
    if (!splt_o_library_locked(state))
    {
      splt_o_lock_library(state);

      *num_of_files_found = 0;

      if (splt_io_check_if_file(state, filename))
      {
        if (splt_p_file_is_supported_by_plugins(state, filename))
        {
          found_files = malloc(sizeof(char *));
          if (!found_files)
          {
            *err = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
            return NULL;
          }

          int fname_size = strlen(filename) + 1;
          found_files[0] = malloc(sizeof(char) * fname_size);
          memset(found_files[0], '\0', fname_size);

          if (!found_files[0])
          {
            free(found_files);
            return NULL;
          }

          strncat(found_files[0], filename, fname_size);
          *num_of_files_found = 1;
        }
      }
      else
      {
        char *dir = strdup(filename);
        if (dir == NULL)
        {
          *err = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
          return NULL;
        }

        if (dir[strlen(dir)-1] == SPLT_DIRCHAR)
        {
          dir[strlen(dir)-1] = '\0';
        }

        splt_io_find_filenames(state, dir, &found_files, num_of_files_found, err);

        if (dir)
        {
          free(dir);
          dir = NULL;
        }
      }

      splt_o_unlock_library(state);
    }
    else
    {
      *err = SPLT_ERROR_LIBRARY_LOCKED;
    }
  }
  else
  {
    *err = SPLT_ERROR_STATE_NULL;
    return NULL;
  }

  return found_files;
}

//@}

//! Checks if a name points to a directory
int mp3splt_check_if_directory(const char *fname)
{
  return splt_io_check_if_directory(fname);
}

void mp3splt_free_one_tag(splt_tags *tags)
{
  splt_tu_free_one_tags(&tags);
}

splt_tags *mp3splt_parse_filename_regex(splt_state *state, splt_code *error)
{
  splt_tags *tags = NULL;
  int erro = SPLT_OK;
  int *err = &erro;
  if (error != NULL) { err = error; }

  if (state != NULL)
  {
    if (!splt_o_library_locked(state))
    {
      splt_o_lock_library(state);

#ifndef NO_PCRE
      tags = splt_fr_parse_from_state(state, error);
#else
      splt_c_put_warning_message_to_client(state,
          _(" warning: cannot set tags from filename regular expression - compiled without pcre support\n"));
      *error = SPLT_REGEX_UNAVAILABLE;
#endif

      splt_o_unlock_library(state);
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

  return tags;
}

