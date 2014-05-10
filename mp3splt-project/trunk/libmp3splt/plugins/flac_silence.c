/**********************************************************
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2014 Alexandru Munteanu - m@ioalex.net
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

#include <stdio.h>
#include <math.h>

#include "flac_silence.h"

static void splt_flac_scan_silence_and_process(splt_state *state, off_t start_offset,
    float max_threshold, unsigned long length, 
    short process_silence(double time, float level, int silence_was_found, short must_flush,
      splt_scan_silence_data *ssd, int *found, int *error),
    splt_scan_silence_data *ssd, int *error);

int splt_flac_scan_silence(splt_state *state, off_t start_offset, unsigned long length,
    float threshold, float min, int shots, short output, int *error,
    short silence_processor(double time, float level, int silence_was_found, short must_flush,
      splt_scan_silence_data *ssd, int *found, int *error))
{
  splt_scan_silence_data *ssd = splt_scan_silence_data_new(state, output, min, shots, SPLT_TRUE);
  if (ssd == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return -1;
  }

  splt_flac_scan_silence_and_process(state, start_offset, threshold, length, silence_processor, ssd, error);

  int found = ssd->found;

  splt_free_scan_silence_data(&ssd);

  if (*error < 0)
  {
    found = -1;
  }

  return found;
}

static splt_flac_silence_data *splt_flac_silence_data_new(splt_state *state, 
    splt_flac_state *flacstate) {
  splt_flac_silence_data *silence_data = malloc(sizeof(splt_flac_silence_data));
  if (silence_data == NULL) { return NULL; }

  silence_data->error = SPLT_OK;
  silence_data->state = state;
  silence_data->flacstate = flacstate;
  silence_data->time = 0;
  silence_data->silence_found = 1;
  silence_data->threshold = 0;

  return silence_data;
}

static void splt_flac_silence_data_free(splt_flac_silence_data *silence_data)
{
  if (!silence_data) { return; }
  free(silence_data);
}

static FLAC__StreamDecoderWriteStatus splt_flac_write_callback(const FLAC__StreamDecoder *decoder, 
    const FLAC__Frame *frame, const FLAC__int32 * const buffer[], void *client_data)
{
  splt_flac_silence_data *silence_data = (splt_flac_silence_data *) client_data;
  splt_flac_state *flacstate = silence_data->flacstate;

  double number;
  if (frame->header.number_type == FLAC__FRAME_NUMBER_TYPE_SAMPLE_NUMBER)
  {
    number = (double) frame->header.number.sample_number;
  }
  else
  {
    number = (double) frame->header.number.frame_number;
  }

  double time = (double) number / (double) frame->header.sample_rate;
  silence_data->time = time;

  silence_data->silence_found = 1;

  size_t i, j;
  for (i = 0;i < frame->header.channels; i++)
  {
    for (j = 0;j < frame->header.blocksize; j++)
    {
      float normalizer_coeff = 1.0 / ((1 << (frame->header.bits_per_sample - 1)));
      float sample = fabs(buffer[i][j] * normalizer_coeff);
      flacstate->temp_level = flacstate->temp_level * 0.999 + sample * 0.001;
      if (sample > silence_data->threshold)
      {
        silence_data->silence_found = 0;
      }
    }
  }

  return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

static void splt_flac_error_callback(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, 
    void *client_data)
{
  splt_flac_silence_data *silence_data = (splt_flac_silence_data *) client_data;

  splt_e_set_error_data(silence_data->state, splt_t_get_filename_to_split(silence_data->state));
  silence_data->error = SPLT_ERROR_INVALID;

  splt_d_print_debug(silence_data->state, "Error while decoding flac file: %s\n", 
      FLAC__StreamDecoderErrorStatusString[status]);
}

static void splt_flac_scan_silence_and_process(splt_state *state, off_t start_offset,
    float max_threshold, unsigned long length,
    short process_silence(double time, float level, int silence_was_found, short must_flush,
      splt_scan_silence_data *ssd, int *found, int *error),
    splt_scan_silence_data *ssd, int *error)
{
  splt_c_put_progress_text(state, SPLT_PROGRESS_SCAN_SILENCE);

  splt_flac_state *flacstate = state->codec;
  splt_flac_silence_data *silence_data = splt_flac_silence_data_new(state, flacstate);
  if (!silence_data)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return;
  }

  silence_data->threshold = splt_co_convert_from_db(max_threshold);

  FLAC__StreamDecoder *decoder = FLAC__stream_decoder_new();
  if (decoder == NULL)
  {
    splt_flac_silence_data_free(silence_data);
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return;
  }

  flacstate->temp_level = 0.0;

  FLAC__StreamDecoderInitStatus status;
  char *input_filename = splt_t_get_filename_to_split(state);
  FILE *file = splt_io_fopen(input_filename, "rb");
  if (file == NULL)
  {
    splt_e_set_strerror_msg_with_data(state, input_filename);
    *error = SPLT_ERROR_CANNOT_OPEN_FILE;
    splt_flac_silence_data_free(silence_data);
    return;
  }

  if (start_offset > 0)
  {
    if (fseeko(file, start_offset, SEEK_SET) == -1)
    {
      splt_e_set_strerror_msg_with_data(state, input_filename);
      *error = SPLT_ERROR_SEEKING_FILE;
      splt_flac_silence_data_free(silence_data);
      fclose(file);
      return;
    }
  }

  status = FLAC__stream_decoder_init_FILE(decoder, file,
      splt_flac_write_callback, NULL, splt_flac_error_callback, silence_data);
  if (status != FLAC__STREAM_DECODER_INIT_STATUS_OK)
  {
    splt_d_print_debug(state, "Failed to initialize flac decoder with error %d", status);
    splt_e_set_error_data(state, splt_t_get_filename_to_split(state));
    *error = SPLT_ERROR_INVALID;
    goto end;
  }

  int split_type = splt_o_get_int_option(state, SPLT_OPT_SPLIT_MODE);
  short option_silence_mode =
    (split_type == SPLT_OPTION_SILENCE_MODE || split_type == SPLT_OPTION_TRIM_SILENCE_MODE);

  long total_time = splt_t_get_total_time(state);

  int first_time = SPLT_TRUE;
  long time0 = 0;
  int found = 0;
  while (FLAC__STREAM_DECODER_END_OF_STREAM != FLAC__stream_decoder_get_state(decoder))
  {
    if (!FLAC__stream_decoder_process_single(decoder))
    {
      break;
    }

    if (first_time)
    {
      time0 = silence_data->time;
    }

    first_time = SPLT_FALSE;

    float level = splt_co_convert_to_db(flacstate->temp_level);
    if (level < -96.0) { level = -96.0; }
    if (level > 0) { level = 0; }

    long current_time = (long) ((silence_data->time - time0) * 100);

    short must_flush = length > 0 && current_time >= length;
    int err = SPLT_OK;
    short stop = process_silence(silence_data->time, level, 
        silence_data->silence_found, must_flush, ssd, &found, &err);
    if (stop || stop == -1)
    {
      if (err < 0) { *error = err; goto end; }
      break;
    }

    if (state->split.get_silence_level)
    {
      state->split.get_silence_level((long) (silence_data->time * 100.0), level, state->split.silence_level_client_data);
    }
    state->split.p_bar->silence_db_level = level;
    state->split.p_bar->silence_found_tracks = found;

    if (option_silence_mode)
    {
      if (splt_t_split_is_canceled(state)) { break; }
      splt_c_update_progress(state, silence_data->time * 100.0, (double)total_time, 1, 0, SPLT_DEFAULT_PROGRESS_RATE2);
    }
    else
    {
      splt_c_update_progress(state, (double) current_time, (double)length, 4, 0.5, SPLT_DEFAULT_PROGRESS_RATE2);
    }
  }

  if (silence_data->error < 0)
  {
    *error = silence_data->error;
  }

end:
  FLAC__stream_decoder_delete(decoder);
  splt_flac_silence_data_free(silence_data);
}

