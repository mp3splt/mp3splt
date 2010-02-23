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

#include <string.h>

#include "splt.h"

static void splt_c_put_message_to_client(splt_state *state, char *message,
    splt_message_type mess_type);

int splt_c_put_split_file(splt_state *state, const char *filename)
{
  int error = SPLT_OK;

  if (state->split.file_split != NULL)
  {
    state->split.file_split(filename,state->split.p_bar->user_data);

    if (! splt_o_get_int_option(state, SPLT_OPT_PRETEND_TO_SPLIT))
    {
      char *new_m3u_file = splt_t_get_m3u_file_with_path(state, &error); 
      if (error < 0) { return error; }
      if (new_m3u_file)
      {
        FILE *file_input = NULL;
        if ((file_input = splt_io_fopen(new_m3u_file, "a+")) != NULL)
        {
          //we don't care about the path of the split filename
          fprintf(file_input,"%s\n", splt_su_get_fname_without_path(filename));
          if (fclose(file_input) != 0)
          {
            splt_e_set_strerror_msg(state);
            splt_e_set_error_data(state, new_m3u_file);
            error = SPLT_ERROR_CANNOT_CLOSE_FILE;
          }
        }
        else
        {
          splt_e_set_strerror_msg(state);
          splt_e_set_error_data(state, new_m3u_file);
          error = SPLT_ERROR_CANNOT_OPEN_FILE;
        }
        free(new_m3u_file);
        new_m3u_file = NULL;
      }
    }
  }
  else
  {
    //splt_e_error(SPLT_IERROR_INT,__func__, -500, NULL);
  }

  return error;
}

void splt_c_put_progress_text(splt_state *state, int type)
{
  char filename_shorted[512] = "";

  int err = SPLT_OK;

  if (state->split.p_bar->progress != NULL)
  {
    char *point_name = NULL;
    int curr_split = splt_t_get_current_split(state);
    point_name = splt_sp_get_splitpoint_name(state, curr_split,&err);

    if (point_name != NULL)
    {
      const char *extension = splt_p_get_extension(state, &err);
      if (err >= 0)
      {
        snprintf(filename_shorted,
            state->split.p_bar->progress_text_max_char,"%s%s",
            point_name,extension);

        if (strlen(point_name) > state->split.p_bar->progress_text_max_char)
        {
          filename_shorted[strlen(filename_shorted)-1] = '.';
          filename_shorted[strlen(filename_shorted)-2] = '.';
          filename_shorted[strlen(filename_shorted)-3] = '.';
        }
      }
    }

    snprintf(state->split.p_bar->filename_shorted, 512,"%s", filename_shorted);

    state->split.p_bar->current_split = splt_t_get_current_split_file_number(state);
    state->split.p_bar->max_splits = state->split.splitnumber-1;
    state->split.p_bar->progress_type = type;
  }
}

void splt_c_put_info_message_to_client(splt_state *state, char *message)
{
  splt_c_put_message_to_client(state, message, SPLT_MESSAGE_INFO);
}

void splt_c_put_debug_message_to_client(splt_state *state, char *message)
{
  splt_c_put_message_to_client(state, message, SPLT_MESSAGE_DEBUG);
}

void splt_c_update_progress(splt_state *state, double current_point,
    double total_points, int progress_stage,
    float progress_start, int refresh_rate)
{
  if (state->split.p_bar->progress != NULL)
  {
    if (splt_o_get_iopt(state, SPLT_INTERNAL_PROGRESS_RATE) > refresh_rate)
    {
      state->split.p_bar->percent_progress = (float) (current_point / total_points);

      state->split.p_bar->percent_progress = 
        state->split.p_bar->percent_progress / progress_stage + progress_start;

      if (state->split.p_bar->percent_progress < 0)
      {
        state->split.p_bar->percent_progress = 0;
      }
      if (state->split.p_bar->percent_progress > 1)
      {
        state->split.p_bar->percent_progress = 1;
      }

      state->split.p_bar->progress(state->split.p_bar);
      splt_o_set_iopt(state, SPLT_INTERNAL_PROGRESS_RATE, 0);
    }
    else
    {
      splt_o_set_iopt(state, SPLT_INTERNAL_PROGRESS_RATE,
          splt_o_get_iopt(state, SPLT_INTERNAL_PROGRESS_RATE)+1);
    }
  }
}

static void splt_c_put_message_to_client(splt_state *state, char *message,
    splt_message_type mess_type)
{
  if (!splt_o_messages_locked(state))
  {
    if (state->split.put_message != NULL)
    {
      state->split.put_message(message, mess_type);
    }
    else
    {
      //splt_e_error(SPLT_IERROR_INT,__func__, -500, NULL);
    }
  }
}

