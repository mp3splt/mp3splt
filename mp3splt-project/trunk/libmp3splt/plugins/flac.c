/**********************************************************
 *
 * libmp3splt flac plugin 
 *
 * Copyright (c) 2013 Alexandru Munteanu - <m@ioalex.net>
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

#include <string.h>

#include "splt.h"

#include "flac_frame_reader.h"
#include "flac_metadata_utils.h"
#include "flac.h"

static void splt_flac_get_info(splt_state *state, FILE *file_input, splt_code *error);
static splt_flac_state *splt_flac_info(FILE *in, splt_state *state, splt_code *error);
static FILE *splt_flac_open_file_read(splt_state *state, const char *filename, splt_code *error);
static splt_flac_state *splt_flac_state_new(splt_code *error);
static void splt_flac_state_free(splt_flac_state *flacstate);

void splt_pl_set_plugin_info(splt_plugin_info *info, int *error)
{
  info->version = 0.1;

  info->name = malloc(40);
  if (info->name == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return;
  }
  snprintf(info->name, 39, "flac (libflac)");

  info->extension = malloc(strlen(SPLT_FLAC_EXT)+2);
  if (info->extension == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return;
  }
  snprintf(info->extension, strlen(SPLT_FLAC_EXT)+1, SPLT_FLAC_EXT);

  info->upper_extension = splt_su_convert(info->extension, SPLT_TO_UPPERCASE, error);
}

int splt_pl_check_plugin_is_for_file(splt_state *state, int *error)
{
  char *input_filename = splt_t_get_filename_to_split(state);

  //TODO: stdin

  FLAC__StreamMetadata stream_info;
  if (!FLAC__metadata_get_streaminfo(input_filename, &stream_info))
  {
    splt_d_print_debug(state, "File %s is not a valid flac file.\n", input_filename);
    return SPLT_FALSE;
  }

  splt_d_print_debug(state, "File %s is a valid flac file.\n", input_filename);

  return SPLT_TRUE;
}

void splt_pl_init(splt_state *state, int *error)
{
  //TODO: stdin warning

  char *input_filename = splt_t_get_filename_to_split(state);
  FILE *file_input = splt_flac_open_file_read(state, input_filename, error);
  if (file_input == NULL) { return; }

  splt_flac_get_info(state, file_input, error);
}

void splt_pl_end(splt_state *state, int *error)
{
  splt_flac_state *flacstate = state->codec;
  if (!flacstate) { return; }

  //TODO: check stdin

  splt_flac_state_free(flacstate);
  state->codec = NULL;
}

double splt_pl_split(splt_state *state, const char *output_fname,
    double begin_point, double end_point, int *error, int save_end_point) 
{
  splt_flac_state *flacstate = state->codec;

  FILE *output_file = splt_io_fopen(output_fname, "wb+");
  if (output_file == NULL)
  {
    splt_e_set_strerror_msg_with_data(state, output_fname);
    *error = SPLT_ERROR_CANNOT_OPEN_DEST_FILE;
    return end_point;
  }

  splt_fr_read_and_write_frames(state, flacstate->in, output_file,
      flacstate->streaminfo.min_blocksize, 
      flacstate->streaminfo.max_blocksize,
      flacstate->streaminfo.bits_per_sample,
      flacstate->streaminfo.sample_rate,
      flacstate->streaminfo.channels,
      flacstate->streaminfo.min_framesize,
      flacstate->streaminfo.max_framesize,
      error);

  if (output_file)
  {
    //TODO check != stdout
    if (fclose(output_file) != 0)
    {
      splt_e_set_strerror_msg_with_data(state, output_fname);
      *error = SPLT_ERROR_CANNOT_CLOSE_FILE;
    }
  }

  return end_point;
}

static void splt_flac_get_info(splt_state *state, FILE *file_input, splt_code *error)
{
  splt_flac_state *flacstate = splt_flac_info(file_input, state, error);
  state->codec = flacstate;
  if (*error < 0) { return; }

  if (! splt_o_messages_locked(state))
  {
    char flac_infos[1024] = { '\0' };
    snprintf(flac_infos, 1023, 
        _(" info: Native FLAC - %u Hz - %d channels of %d bits"),
        flacstate->streaminfo.sample_rate, flacstate->streaminfo.channels,
        flacstate->streaminfo.bits_per_sample);

    char total_time[256] = { '\0' };
    int total_seconds = (int) splt_t_get_total_time(state) / 100;
    int minutes = total_seconds / 60;
    int seconds = total_seconds % 60;
    snprintf(total_time, 255, _(" - Total time: %dm.%02ds"), minutes, seconds%60);

    splt_c_put_info_message_to_client(state, "%s%s\n", flac_infos, total_time);
  }
}

static splt_flac_state *splt_flac_info(FILE *in, splt_state *state, splt_code *error)
{
  splt_flac_state *flacstate = splt_flac_state_new(error);
  if (flacstate == NULL) { return NULL; }

  flacstate->in = in;

  //TODO: stdin check ?

  splt_flac_mu_read(flacstate, state, flacstate->in, error);
  if (*error < 0)
  {
    splt_flac_state_free(flacstate);
    return NULL;
  }

  double total_time =
    ((double) flacstate->streaminfo.total_samples / 
     (double) flacstate->streaminfo.sample_rate) * 100.0;
  splt_t_set_total_time(state, (long)total_time);

  return flacstate;
}

static FILE *splt_flac_open_file_read(splt_state *state, const char *filename, splt_code *error)
{
  //TODO: stdin check

  FILE *file_input = splt_io_fopen(filename, "rb");
  if (file_input == NULL)
  {
    splt_e_set_strerror_msg_with_data(state, filename);
    *error = SPLT_ERROR_CANNOT_OPEN_FILE;
  }

  return file_input;
}

static splt_flac_state *splt_flac_state_new(splt_code *error)
{
  splt_flac_state *flacstate = malloc(sizeof(splt_flac_state));
  if (flacstate == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return NULL;
  }

  memset(flacstate, 0, sizeof(splt_flac_state));

  return flacstate;
}

static void splt_flac_state_free(splt_flac_state *flacstate)
{
  if (!flacstate) { return; }

  free(flacstate);
}

