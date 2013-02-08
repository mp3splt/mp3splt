/**********************************************************
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2013 Alexandru Munteanu - m@ioalex.net
 *
 *********************************************************/

/**********************************************************
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
 *********************************************************/

#include "silence_processors.h"

static void write_to_full_log(splt_state *state, double time, float level, int shots, int found,
    double begin_position, double end_position);

splt_scan_silence_data *splt_scan_silence_data_new(splt_state *state, short first, 
    float min, int shots, short set_new_length)
{
  splt_scan_silence_data *ssd = malloc(sizeof(splt_scan_silence_data));
  if (!ssd)
  {
    return NULL;
  }

  ssd->state = state;
  ssd->first = first;
  ssd->min = min;
  ssd->number_of_shots = shots;
  ssd->set_new_length = set_new_length;

  ssd->flush = SPLT_FALSE;
  ssd->silence_begin = 0;
  ssd->silence_end = 0;
  ssd->len = 0;
  ssd->found = 0;
  ssd->shot = shots;
  ssd->silence_begin_was_found = SPLT_FALSE;
  ssd->continue_after_silence = SPLT_FALSE;
  ssd->previous_time = 0;

  return ssd;
}

void splt_free_scan_silence_data(splt_scan_silence_data **ssd)
{
  if (!ssd || !*ssd)
  {
    return;
  }

  free(*ssd);
  *ssd = NULL;
}

short splt_scan_silence_processor(double time, float level, int silence_was_found,
    short must_flush, splt_scan_silence_data *ssd, int *found_silence_points, int *error)
{
  if (time < 0) { return SPLT_TRUE; }

  short stop = SPLT_FALSE;

  if (must_flush)
  {
    ssd->flush = SPLT_TRUE;
    stop = SPLT_TRUE;
  }

  if (!ssd->flush && silence_was_found)
  {
    if (ssd->len == 0)
    {
      ssd->silence_begin = time;
    }

    if (ssd->first == SPLT_FALSE) 
    {
      ssd->len++;
    }

    if (ssd->shot < ssd->number_of_shots)
    {
      ssd->shot += 2;
    }

    ssd->silence_end = time;

    *found_silence_points = ssd->found;

    write_to_full_log(ssd->state, time, level, ssd->shot, ssd->found, -1, -1);
    return stop;
  }

  double begin_position = -1;
  double end_position = -1;

  if (ssd->len > SPLT_DEFAULTSILLEN)
  {
    if (ssd->flush || (ssd->shot <= 0))
    {
      begin_position = ssd->silence_begin;
      end_position = ssd->silence_end;

      if (ssd->set_new_length)
      {
        ssd->len = (int) (ssd->silence_end * 100.0 - ssd->silence_begin * 100.0);
      }

      if ((end_position - begin_position - ssd->min) >= 0.f)
      {
        if (splt_siu_ssplit_new(&ssd->state->silence_list,
              begin_position, end_position, ssd->len, error) == -1)
        {
          ssd->found = -1;
          *found_silence_points = ssd->found;

          write_to_full_log(ssd->state, time, level, ssd->shot, ssd->found, begin_position, end_position);
          return SPLT_TRUE;
        }

        ssd->found++;
      }

      ssd->len = 0;
      ssd->shot = ssd->number_of_shots;
    }
  }
  else 
  {
    ssd->len = 0;
  }

  if (ssd->flush)
  {
    write_to_full_log(ssd->state, time, level, ssd->shot, ssd->found, begin_position, end_position);
    return -1;
  }

  if (ssd->first && (ssd->shot <= 0))
  {
    ssd->first = SPLT_FALSE;
  }

  if (ssd->shot > 0) 
  {
    ssd->shot--;
  }

  if (ssd->found >= SPLT_MAXSILENCE) 
  {
    stop = SPLT_TRUE;
  }

  *found_silence_points = ssd->found;

  write_to_full_log(ssd->state, time, level, ssd->shot, ssd->found, begin_position, end_position);
  return stop;
}

static void write_to_full_log(splt_state *state, double time, float level, int shots, int found,
    double begin_position, double end_position)
{
  FILE *full_log_file_descriptor = splt_t_get_silence_full_log_file_descriptor(state);
  if (!full_log_file_descriptor)
  {
    return;
  }

  if (begin_position > 0 && end_position > 0)
  {
    fprintf(full_log_file_descriptor, "0\t%lf\t%f\t%d\t%d\t%lf\t%lf\n", time, level, shots, found,
        begin_position, end_position);
    return;
  }

  fprintf(full_log_file_descriptor, "0\t%lf\t%f\t%d\t%d\t\t\n", time, level, shots, found);
}

static short splt_detect_where_begin_silence_ends(double time, float level, int silence_was_found,
    short must_flush, splt_scan_silence_data *ssd, int *found_silence_points, int *error)
{
  if (silence_was_found)
  {
    if (ssd->shot < ssd->number_of_shots)
    {
      ssd->shot += 2;
    }

    ssd->silence_end = time;
  }

  if (ssd->shot <= 0)
  {
    double silence_end = ssd->silence_end;

    float min_length = splt_o_get_float_option(ssd->state, SPLT_OPT_PARAM_MIN_LENGTH);
    if (min_length > 0)
    {
      if (silence_end > min_length)
      {
        silence_end -= min_length;
      }
      else
      {
        silence_end = 0;
      }

      long mins, secs, hundr;
      splt_co_get_mins_secs_hundr(splt_co_time_to_long(silence_end), &mins, &secs, &hundr);
      splt_c_put_info_message_to_client(ssd->state,
          _(" info: trim begin split at %ldm_%.2lds_%.2ldh\n"), mins, secs, hundr);
    }
 
    if (splt_siu_ssplit_new(&ssd->state->silence_list, silence_end, silence_end, 0, error) == -1)
    {
      return SPLT_TRUE;
    }

    ssd->found++;
    ssd->silence_begin_was_found = SPLT_TRUE;
    ssd->shot = ssd->number_of_shots;
  }

  if (ssd->shot > 0)
  {
    ssd->shot--;
  }

  return SPLT_FALSE;
}

static short splt_detect_where_end_silence_begins(double time, float level, int silence_was_found, 
    short must_flush, splt_scan_silence_data *ssd, int *found_silence_points, int *error)
{
  if (time < 0)
  {
    double silence_begin = ssd->silence_begin;

    float min_length = splt_o_get_float_option(ssd->state, SPLT_OPT_PARAM_MIN_LENGTH);
    if (min_length > 0)
    {
      if (ssd->previous_time - silence_begin > min_length)
      {
        silence_begin += min_length;
      }
      else
      {
        silence_begin = ssd->previous_time;
      }

      long mins, secs, hundr;
      splt_co_get_mins_secs_hundr(splt_co_time_to_long(silence_begin), &mins, &secs, &hundr);
      splt_c_put_info_message_to_client(ssd->state,
          _(" info: trim end split at %ldm_%.2lds_%.2ldh\n"), mins, secs, hundr);
    }

    if (splt_siu_ssplit_new(&ssd->state->silence_list, silence_begin, silence_begin, 0, error) == -1)
    {
      return SPLT_TRUE;
    }

    ssd->found++;

    return SPLT_TRUE;
  }

  ssd->previous_time = time;

  if (silence_was_found)
  {
    if (ssd->len == 0)
    {
      ssd->silence_begin = time;
      ssd->continue_after_silence = SPLT_FALSE;
    }

    if (ssd->first == SPLT_FALSE) 
    {
      ssd->len++;
    }

    if (ssd->shot < ssd->number_of_shots)
    {
      ssd->shot += 2;
    }

    return SPLT_FALSE;
  }
  else if (ssd->continue_after_silence)
  {
    ssd->silence_begin = time;
  }

  if (ssd->len > SPLT_DEFAULTSILLEN)
  {
    if (ssd->shot <= 0)
    {
      ssd->len = 0;
      ssd->shot = ssd->number_of_shots;
      ssd->continue_after_silence = SPLT_TRUE;
    }
  }
  else
  {
    ssd->len = 0;
  }

  if (ssd->first && (ssd->shot <= 0))
  {
    ssd->first = SPLT_FALSE;
  }

  if (ssd->shot > 0)
  {
    ssd->shot--;
  }

  return SPLT_FALSE;
}

short splt_trim_silence_processor(double time, float level, int silence_was_found, 
    short must_flush, splt_scan_silence_data *ssd, int *found_silence_points, int *error)
{
  if (!ssd->silence_begin_was_found)
  {
    splt_detect_where_begin_silence_ends(time, level, silence_was_found, must_flush, ssd, found_silence_points, error);
  }
  else 
  {
    splt_detect_where_end_silence_begins(time, level, silence_was_found, must_flush, ssd, found_silence_points, error);
  }

  return SPLT_FALSE;
}

