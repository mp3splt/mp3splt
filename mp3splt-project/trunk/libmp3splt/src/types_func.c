/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2012 Alexandru Munteanu - io_fx@yahoo.fr
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

/*! \file

Set global parameters like the name of the file that has to be 
split.
 */

#include <string.h>

#include "splt.h"

static void splt_t_set_default_state_values(splt_state *state, int *error)
{
  state->split.tags = NULL;
  splt_tu_reset_tags(splt_tu_get_tags_like_x(state));
  state->split.points = NULL;
  state->fname_to_split = NULL;
  state->path_of_split = NULL;
  state->m3u_filename = NULL;
  state->input_fname_regex = NULL;
  state->default_comment_tag = NULL;
  state->default_genre_tag = NULL;
  state->silence_log_fname = NULL;

  state->split.real_tagsnumber = 0;
  state->split.real_splitnumber = 0;
  state->split.splitnumber = 0;
  state->split.current_split_file_number = 1;
  state->split.get_silence_level = NULL;
  state->split.put_message = NULL;
  state->split.file_split = NULL;
  state->split.p_bar->progress_text_max_char = 40;
  snprintf(state->split.p_bar->filename_shorted,512, "%s","");
  state->split.p_bar->percent_progress = 0;
  state->split.p_bar->current_split = 0;
  state->split.p_bar->max_splits = 0;
  state->split.p_bar->progress_type = SPLT_PROGRESS_PREPARE;
  state->split.p_bar->silence_found_tracks = 0;
  state->split.p_bar->silence_db_level = 0;
  state->split.p_bar->user_data = 0;
  state->split.p_bar->progress = NULL;
  state->cancel_split = SPLT_FALSE;

  splt_original_tags *original_tags = &state->original_tags;
  splt_tu_reset_tags(&original_tags->tags);
  original_tags->all_original_tags = NULL;

  splt_w_set_wrap_default_values(state);
  splt_se_set_sync_errors_default_values(state);
  if (splt_of_set_default_values(state) < 0) { return; }
  splt_e_set_errors_default_values(state);
  splt_fu_set_default_values(state);
  splt_o_set_options_default_values(state);
  splt_o_set_ioptions_default_values(state);
  splt_p_set_default_values(state);
}

splt_state *splt_t_new_state(splt_state *state, int *error)
{
  if ((state =malloc(sizeof(splt_state))) ==NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return NULL;
  }

  memset(state, 0x0, sizeof(splt_state));
  if ((state->wrap = malloc(sizeof(splt_wrap))) == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    free(state);
    return NULL;
  }
  memset(state->wrap, 0x0, sizeof(state->wrap));

  if ((state->serrors = malloc(sizeof(splt_syncerrors))) == NULL)
  {
    free(state->wrap);
    free(state);
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return NULL;
  }
  memset(state->serrors, 0x0, sizeof(state->serrors));

  if ((state->split.p_bar = malloc(sizeof(splt_progress))) == NULL)
  {
    free(state->wrap);
    free(state->serrors);
    free(state);
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return NULL;
  }

  if ((state->plug = malloc(sizeof(splt_plugins))) == NULL)
  {
    free(state->wrap);
    free(state->serrors);
    free(state->split.p_bar);
    free(state);
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return NULL;
  }

  state->current_plugin = -1;

  splt_t_set_default_state_values(state, error);

  return state;
}

static void splt_t_free_state_struct(splt_state *state)
{
  if (state)
  {
    if (state->fname_to_split)
    {
      free(state->fname_to_split);
      state->fname_to_split = NULL;
    }
    if (state->path_of_split)
    {
      free(state->path_of_split);
      state->path_of_split = NULL;
    }
    if (state->m3u_filename)
    {
      free(state->m3u_filename);
      state->m3u_filename = NULL;
    }
    if (state->input_fname_regex)
    {
      free(state->input_fname_regex);
      state->input_fname_regex = NULL;
    }
    if (state->default_comment_tag)
    {
      free(state->default_comment_tag);
      state->default_comment_tag = NULL;
    }
    if (state->default_genre_tag)
    {
      free(state->default_genre_tag);
      state->default_genre_tag = NULL;
    }
    if (state->silence_log_fname)
    {
      free(state->silence_log_fname);
      state->silence_log_fname = NULL;
    }
    if (state->wrap)
    {
      free(state->wrap);
      state->wrap = NULL;
    }
    if (state->serrors)
    {
      free(state->serrors);
      state->serrors = NULL;
    }
    if (state->plug)
    {
      free(state->plug);
      state->plug = NULL;
    }

    free(state);
    state = NULL;
  }
}

void splt_t_free_state(splt_state *state)
{
  if (state)
  {
    splt_tu_free_original_tags(state);
    splt_of_free_oformat(state);
    splt_w_wrap_free(state);
    splt_se_serrors_free(state);
    splt_fu_freedb_free_search(state);
    splt_t_free_splitpoints_tags(state);
    splt_o_iopts_free(state);
    splt_p_free_plugins(state);
    if (state->split.p_bar)
    {
      free(state->split.p_bar);
      state->split.p_bar = NULL;
    }
    splt_e_free_errors(state);
    splt_t_free_state_struct(state);
  }
}

void splt_t_set_total_time(splt_state *state, long value)
{
  splt_d_print_debug(state,"Setting total time to _%ld_\n", value);

  if (value >= 0)
  {
    state->split.total_time = value;
  }
  else
  {
    splt_e_error(SPLT_IERROR_INT,__func__, value, NULL);
  }
}

long splt_t_get_total_time(splt_state *state)
{
  return state->split.total_time;
}

double splt_t_get_total_time_as_double_secs(splt_state *state)
{
  long total_time = splt_t_get_total_time(state);

  double total = total_time / 100;
  total += ((total_time % 100) / 100.);

  return total;
}

void splt_t_set_new_filename_path(splt_state *state, 
    const char *new_filename_path, int *error)
{
  int err = SPLT_OK;

  splt_internal *iopts = &state->iopts;
  err = splt_su_copy(new_filename_path, &iopts->new_filename_path);
  if (err < 0) { *error = err; }
}

char *splt_t_get_new_filename_path(splt_state *state)
{
  return state->iopts.new_filename_path;
}

int splt_t_set_path_of_split(splt_state *state, const char *path_of_split)
{
  splt_d_print_debug(state,"Setting path of split to _%s_\n", path_of_split);
  int err = splt_su_copy(path_of_split, &state->path_of_split);

  if (state->path_of_split == NULL)
  {
    return err;
  }

#ifdef __WIN32__
  if (state->path_of_split[strlen(state->path_of_split)-1] == SPLT_DIRCHAR)
  {
    if (! splt_w32_str_is_drive_root_directory(state->path_of_split))
    {
      splt_su_str_cut_last_char(state->path_of_split);
    }
  }
#endif

  return err;
}

char *splt_t_get_path_of_split(splt_state *state)
{
  return state->path_of_split;
}

int splt_t_set_m3u_filename(splt_state *state, const char *filename)
{
  splt_d_print_debug(state,"Setting m3u filename to _%s_\n", filename);
  return splt_su_copy(filename, &state->m3u_filename);
}

char *splt_t_get_m3u_filename(splt_state *state)
{
  return state->m3u_filename;
}

int splt_t_set_input_filename_regex(splt_state *state, const char *regex)
{
  splt_d_print_debug(state, "Setting input filename regex to _%s_\n", regex);
  return splt_su_copy(regex, &state->input_fname_regex);
}

char *splt_t_get_input_filename_regex(splt_state *state)
{
  return state->input_fname_regex;
}

int splt_t_set_default_comment_tag(splt_state *state, const char *default_comment)
{
  splt_d_print_debug(state,"Setting default comment tag to _%s_\n", default_comment);
  return splt_su_copy(default_comment, &state->default_comment_tag);
}

int splt_t_set_default_genre_tag(splt_state *state, const char *default_genre)
{
  splt_d_print_debug(state,"Setting default genre tag to _%s_\n", default_genre);
  return splt_su_copy(default_genre, &state->default_genre_tag);
}

char *splt_t_get_default_comment_tag(splt_state *state)
{
  return state->default_comment_tag;
}

char *splt_t_get_default_genre_tag(splt_state *state)
{
  return state->default_genre_tag;
}

char *splt_t_get_m3u_file_with_path(splt_state *state, int *error)
{
  char *m3u_file = splt_t_get_m3u_filename(state);
  return splt_su_get_file_with_output_path(state, m3u_file, error);
}

int splt_t_set_silence_log_fname(splt_state *state, const char *filename)
{
  splt_d_print_debug(state,"Setting silence log fname to _%s_\n", filename);
  return splt_su_copy(filename, &state->silence_log_fname);
}

char *splt_t_get_silence_log_fname(splt_state *state)
{
  return state->silence_log_fname;
}

//! Sets the name of the file that has to be split.
int splt_t_set_filename_to_split(splt_state *state, const char *filename)
{
  splt_d_print_debug(state,"Setting filename to split to _%s_\n", filename);
  return splt_su_copy(filename, &state->fname_to_split);
}

//! returns the name of the file that has to be split.
char * splt_t_get_filename_to_split(splt_state *state)
{
  return state->fname_to_split;
}

static void splt_t_set_current_split_file_number(splt_state *state, int index)
{
  state->split.current_split_file_number = index;
}

static void splt_t_set_current_split_file_number_next(splt_state *state)
{
  splt_t_set_current_split_file_number(state, state->split.current_split_file_number+1);
}

void splt_t_set_current_split(splt_state *state, int index)
{
  if (index >= 0)
  {
    int err = SPLT_OK;
	  if (index == 0)
	  {
      if (splt_sp_splitpoint_exists(state, index) &&
          splt_sp_get_splitpoint_type(state, index, &err) == SPLT_SKIPPOINT)
      {
        splt_t_set_current_split_file_number(state, 0);
      }
      else
      {
        splt_t_set_current_split_file_number(state, 1);
      }
	  }
	  else
	  {
		  if (splt_sp_splitpoint_exists(state, index))
		  {
			  if (splt_sp_get_splitpoint_type(state, index, &err) != SPLT_SKIPPOINT)
			  {
				  splt_t_set_current_split_file_number_next(state);
			  }
		  }
		  else
		  {
			  splt_t_set_current_split_file_number_next(state);
		  }
	  }

	  state->split.current_split = index;
  }
  else
  {
    splt_e_error(SPLT_IERROR_INT, __func__,index, NULL);
  }
}

void splt_t_current_split_next(splt_state *state)
{
  splt_t_set_current_split(state, splt_t_get_current_split(state) + 1);
}

int splt_t_get_current_split(splt_state *state)
{
  return state->split.current_split;
}

int splt_t_get_current_split_file_number(splt_state *state)
{
  return state->split.current_split_file_number;
}

void splt_t_set_splitnumber(splt_state *state, int number)
{
  if (number >= 0)
  {
    state->split.splitnumber = number;
  }
  else
  {
    splt_e_error(SPLT_IERROR_INT,__func__, number, NULL);
  }
}

int splt_t_get_splitnumber(splt_state *state)
{
  return state->split.splitnumber;
}

void splt_t_free_splitpoints_tags(splt_state *state)
{
  splt_sp_free_splitpoints(state);
  splt_tu_free_tags(state);
}

void splt_t_clean_one_split_data(splt_state *state, int num)
{
  if (splt_tu_tags_exists(state,num))
  {
    splt_tu_set_tags_field(state,num, SPLT_TAGS_YEAR, NULL);
    splt_tu_set_tags_field(state,num, SPLT_TAGS_ARTIST, NULL);
    splt_tu_set_tags_field(state,num, SPLT_TAGS_ALBUM, NULL);
    splt_tu_set_tags_field(state,num, SPLT_TAGS_TITLE, NULL);
    splt_tu_set_tags_field(state,num, SPLT_TAGS_COMMENT, NULL);
    splt_tu_set_tags_field(state,num, SPLT_TAGS_PERFORMER, NULL);
  }

  if (splt_sp_splitpoint_exists(state, num))
  {
    splt_sp_set_splitpoint_name(state, num, NULL);
  }
}

void splt_t_clean_split_data(splt_state *state,int tracks)
{
  splt_t_set_current_split(state,0);
  do {
    splt_t_clean_one_split_data(state,state->split.current_split);
    splt_t_current_split_next(state);
  } while (splt_t_get_current_split(state) < tracks);
}

int splt_t_split_is_canceled(splt_state *state)
{
  return state->cancel_split;
}

void splt_t_set_stop_split(splt_state *state, int bool_value)
{
  state->cancel_split = bool_value;
}

