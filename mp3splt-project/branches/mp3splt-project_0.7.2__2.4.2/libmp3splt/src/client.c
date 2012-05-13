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

#include <string.h>

#include "splt.h"

static void splt_c_put_message_to_client(splt_state *state, const char *message,
    splt_message_type mess_type);
static int splt_c_append_to_m3u_file(splt_state *state, const char *filename);
static void splt_c_set_filename_shorted_from_current_point_name(splt_state *state);

int splt_c_put_split_file(splt_state *state, const char *filename)
{
  int error = SPLT_OK;

  if (state->split.file_split != NULL)
  {
    state->split.file_split(filename, state->split.p_bar->user_data);
    error = splt_c_append_to_m3u_file(state, filename);
  }
  else
  {
    //splt_e_error(SPLT_IERROR_INT,__func__, -500, NULL);
  }

  return error;
}

void splt_c_put_progress_text(splt_state *state, int type)
{
  splt_progress *p_bar = state->split.p_bar;
  if (p_bar->progress == NULL) { return; }

  splt_c_set_filename_shorted_from_current_point_name(state);

  p_bar->current_split = splt_t_get_current_split_file_number(state);
  p_bar->max_splits = state->split.splitnumber - 1;
  p_bar->progress_type = type;
}

void splt_c_put_info_message_to_client(splt_state *state, const char *message, ...)
{
  va_list ap;
  char *mess = NULL;

  va_start(ap, message);
  mess = splt_su_format_messagev(state, message, ap);
  va_end(ap);

  if (mess)
  {
    splt_c_put_message_to_client(state, mess, SPLT_MESSAGE_INFO);

    free(mess);
    mess = NULL;
  }
}

void splt_c_put_debug_message_to_client(splt_state *state, const char *message, ...)
{
  va_list ap;
  char *mess = NULL;

  va_start(ap, message);
  mess = splt_su_format_messagev(state, message, ap);
  va_end(ap);

  if (mess)
  {
    splt_c_put_message_to_client(state, mess, SPLT_MESSAGE_DEBUG);

    free(mess);
    mess = NULL;
  }
}

void splt_c_update_progress(splt_state *state, double current_point,
    double total_points, int progress_stage,
    float progress_start, int refresh_rate)
{
  splt_progress *p_bar = state->split.p_bar;
  if (p_bar->progress == NULL) { return; }

  if (splt_o_get_iopt(state, SPLT_INTERNAL_PROGRESS_RATE) > refresh_rate)
  {
    p_bar->percent_progress = (float) (current_point / total_points);

    p_bar->percent_progress = p_bar->percent_progress / progress_stage + progress_start;

    if (p_bar->percent_progress < 0)
    {
      p_bar->percent_progress = 0;
    }
    if (p_bar->percent_progress > 1)
    {
      p_bar->percent_progress = 1;
    }

    p_bar->progress(p_bar);
    splt_o_set_iopt(state, SPLT_INTERNAL_PROGRESS_RATE, 0);
  }
  else
  {
    splt_o_set_iopt(state, SPLT_INTERNAL_PROGRESS_RATE,
        splt_o_get_iopt(state, SPLT_INTERNAL_PROGRESS_RATE) + 1);
  }
}

static void splt_c_put_message_to_client(splt_state *state, const char *message,
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

static int splt_c_append_to_m3u_file(splt_state *state, const char *filename)
{
  int err = SPLT_OK;

  if (splt_o_get_int_option(state, SPLT_OPT_PRETEND_TO_SPLIT))
  {
    return err;
  }

  char *new_m3u_file = splt_t_get_m3u_file_with_path(state, &err); 
  if (err < 0 || !new_m3u_file) { return err; }

  FILE *file_input = NULL;
  if ((file_input = splt_io_fopen(new_m3u_file, "a+")) != NULL)
  {
    fprintf(file_input, "%s\n", splt_su_get_fname_without_path(filename));

    if (fclose(file_input) != 0)
    {
      splt_e_set_strerror_msg_with_data(state, new_m3u_file);
      err = SPLT_ERROR_CANNOT_CLOSE_FILE;
    }
  }
  else
  {
    splt_e_set_strerror_msg_with_data(state, new_m3u_file);
    err = SPLT_ERROR_CANNOT_OPEN_FILE;
  }

  free(new_m3u_file);
  new_m3u_file = NULL;

  return err;
}

static void splt_c_set_filename_shorted_from_current_point_name(splt_state *state)
{
  int err = SPLT_OK;
  splt_progress *p_bar = state->split.p_bar;

  char filename_shorted[512] = { '\0' };

  int curr_split = splt_t_get_current_split(state);
  const char *point_name = splt_sp_get_splitpoint_name(state, curr_split, &err);
  if (point_name != NULL)
  {
    const char *extension = splt_p_get_extension(state, &err);
    if (err >= 0)
    {
      size_t max_size = p_bar->progress_text_max_char;
      if (max_size >= 512) { max_size = 511; }

      snprintf(filename_shorted, max_size, "%s%s", point_name, extension);

      if (strlen(point_name) > max_size)
      {
        size_t size = strlen(filename_shorted);
        filename_shorted[size-1] = '.';
        filename_shorted[size-2] = '.';
        filename_shorted[size-3] = '.';
      }
    }
  }

  snprintf(p_bar->filename_shorted, 512, "%s", filename_shorted);
}

