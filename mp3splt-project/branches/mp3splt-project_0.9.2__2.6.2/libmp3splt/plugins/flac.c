/**********************************************************
 *
 * libmp3splt flac plugin 
 *
 * Copyright (c) 2014 Alexandru Munteanu - <m@ioalex.net>
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
#include "cddb_cue_common.h"

#include "flac_silence.h"
#include "flac_metadata_utils.h"
#include "flac.h"

static void splt_flac_get_info(splt_state *state, FILE *file_input, const char *input_filename, splt_code *error);
static splt_flac_state *splt_flac_info(FILE *in, splt_state *state, const char *input_filename, splt_code *error);
static FILE *splt_flac_open_file_read(splt_state *state, const char *filename, splt_code *error);
static splt_flac_state *splt_flac_state_new(splt_code *error);
static void splt_flac_state_free(splt_flac_state *flacstate);

void splt_pl_set_plugin_info(splt_plugin_info *info, int *error)
{
  info->version = 1.0;

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
  if (strcmp(input_filename,"f-") == 0)
  {
    splt_c_put_info_message_to_client(state, " stdin is not yet supported for flac\n");
    *error = SPLT_PLUGIN_ERROR_UNSUPPORTED_FEATURE;
    return SPLT_FALSE;
  }

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

  splt_flac_get_info(state, file_input, input_filename, error);
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
  if (strcmp(output_fname, "-") == 0)
  {
    splt_c_put_info_message_to_client(state, " stdout is not yet supported for flac\n");
    *error = SPLT_PLUGIN_ERROR_UNSUPPORTED_FEATURE;
    return end_point;
  }

  splt_flac_state *flacstate = state->codec;

  splt_flac_md5_decoder *flac_md5_d = splt_flac_md5_decoder_new_and_init(state, error);
  if (*error < 0) { return end_point; }
  flacstate->fr->flac_md5_d = flac_md5_d;

  splt_tags *tags_to_write = splt_tu_get_current_tags(state);

  splt_flac_tags *flac_tags = flacstate->flac_tags;
  if (splt_o_get_int_option(state, SPLT_OPT_TAGS) == SPLT_NO_TAGS ||
      tags_to_write == NULL)
  {
    flac_tags = NULL;
  }

  splt_flac_fr_read_and_write_frames(state, flacstate->fr, 
      flacstate->metadatas, flac_tags, tags_to_write, output_fname,
      begin_point, end_point, save_end_point,
      flacstate->streaminfo.min_blocksize, 
      flacstate->streaminfo.max_blocksize,
      flacstate->streaminfo.bits_per_sample,
      flacstate->streaminfo.sample_rate,
      flacstate->streaminfo.channels,
      flacstate->streaminfo.min_framesize,
      flacstate->streaminfo.max_framesize,
      flacstate->off,
      error);

  if (*error == SPLT_OK) { *error = SPLT_OK_SPLIT; }

  return end_point;
}

int splt_pl_scan_silence(splt_state *state, int *error)
{
  float offset = splt_o_get_float_option(state,SPLT_OPT_PARAM_OFFSET);
  float threshold = splt_o_get_float_option(state, SPLT_OPT_PARAM_THRESHOLD);
  float min_length = splt_o_get_float_option(state, SPLT_OPT_PARAM_MIN_LENGTH);
  int shots = splt_o_get_int_option(state, SPLT_OPT_PARAM_SHOTS);

  splt_flac_state *flacstate = state->codec;
  flacstate->off = offset;

  int found = splt_flac_scan_silence(state, 0, 0, threshold, min_length, 
      shots, 1, error, splt_scan_silence_processor);
  if (*error < 0) { return -1; }

  return found;
}

int splt_pl_scan_trim_silence(splt_state *state, int *error)
{
  float threshold = splt_o_get_float_option(state, SPLT_OPT_PARAM_THRESHOLD);
  int shots = splt_o_get_int_option(state, SPLT_OPT_PARAM_SHOTS);

  int found = splt_flac_scan_silence(state, 0, 0, threshold, 0,
      shots, 1, error, splt_trim_silence_processor);
  if (*error < 0) { return -1; }

  return found;
}

void splt_pl_set_original_tags(splt_state *state, splt_code *error)
{
  splt_flac_state *flacstate = state->codec;
  if (flacstate->flac_tags == NULL) { return; }

  splt_d_print_debug(state, "Getting original tags for flac ...");

  splt_tu_set_to_original_tags(state, flacstate->flac_tags->original_tags, error);
}

void splt_pl_clear_original_tags(splt_original_tags *original_tags)
{
  //nothing to do - we never store original tags in the splt_state, only in the splt_flac_state
}

void splt_pl_import_internal_sheets(splt_state *state, splt_code *error)
{
  char *input_filename = splt_t_get_filename_to_split(state);

  FLAC__StreamMetadata *cuesheet = NULL;
  FLAC__bool cuesheet_read_ok = FLAC__metadata_get_cuesheet(input_filename, &cuesheet);
  if (!cuesheet_read_ok)
  {
    *error = SPLT_ERROR_INTERNAL_SHEET;
    return;
  }

  const FLAC__StreamMetadata_CueSheet *cue_sheet = &cuesheet->data.cue_sheet;
  unsigned track_number;
  long max_hundreths = 0;
  for (track_number = 0; track_number < cue_sheet->num_tracks - 1; track_number++)
  {
    const FLAC__StreamMetadata_CueSheet_Track *cue_track = cue_sheet->tracks + track_number;

    int start_index = 0;
    if (cue_track->num_indices > 1) { start_index++; }

    //TODO: only take INDEX 01 or INDEX 00 if INDEX 01 does not exists
    const FLAC__StreamMetadata_CueSheet_Index *cue_index = cue_track->indices + start_index;
    if (cue_sheet->is_cd)
    {
      long offset = (long) ((cue_track->offset + cue_index->offset) / (44100 / 75)) * 100;
      long hundreths = offset / 75;
      if (hundreths > max_hundreths) { max_hundreths = hundreths; }
      //long mins, secs, hundr;
      //splt_co_get_mins_secs_hundr(hundreths, &mins, &secs, &hundr);
      //fprintf(stdout, "%02lu:%02lu:%02lu\n", mins, secs, hundr);
      //fflush(stdout);
      splt_sp_append_splitpoint(state, hundreths, NULL, SPLT_SPLITPOINT);
    }
    else
    {
      *error = SPLT_ERROR_INTERNAL_SHEET_TYPE_NOT_SUPPORTED;
      goto end;
    }
  }

  if (track_number > 0)
  {
    splt_sp_append_splitpoint(state, LONG_MAX, _("--- last cue splitpoint ---"), SPLT_SPLITPOINT);
  }

end:
  FLAC__metadata_object_delete(cuesheet);
  if (*error < 0) { return; }

  splt_o_lock_messages(state);
  splt_pl_init(state, error);
  splt_o_unlock_messages(state);
  if (*error < 0) { return; }

  splt_flac_state *flacstate = state->codec;
  const splt_tags *our_tags = flacstate->flac_tags->original_tags;
  splt_cc_put_filenames_from_tags(state, track_number, error, our_tags, SPLT_FALSE,
      SPLT_FALSE);

  int err = SPLT_OK;
  splt_pl_end(state, &err);
  if (err < 0 && *error >= 0) { *error = err; }
}

static void splt_flac_get_info(splt_state *state, FILE *file_input, const char *input_filename, splt_code *error)
{
  splt_flac_state *flacstate = splt_flac_info(file_input, state, input_filename, error);
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

static splt_flac_state *splt_flac_info(FILE *in, splt_state *state, const char *input_filename, splt_code *error)
{
  splt_flac_state *flacstate = splt_flac_state_new(error);
  if (flacstate == NULL) { return NULL; }

  flacstate->fr = splt_flac_fr_new(in, input_filename);
  if (flacstate->fr == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    splt_flac_state_free(flacstate);
    return NULL;
  }

  flacstate->metadatas = splt_flac_m_new();
  if (flacstate->metadatas == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    splt_flac_state_free(flacstate);
    return NULL;
  }

  flacstate->flac_tags = NULL;
  flacstate->off = splt_o_get_float_option(state, SPLT_OPT_PARAM_OFFSET);

  //TODO: stdin check ?

  splt_flac_mu_read(flacstate, state, in, error);
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

  if (flacstate->fr)
  {
    splt_flac_fr_free(flacstate->fr);
    flacstate->fr = NULL;
  }

  if (flacstate->metadatas)
  {
    splt_flac_m_free(flacstate->metadatas);
    flacstate->metadatas = NULL;
  }

  if (flacstate->flac_tags)
  {
    splt_flac_t_free(&flacstate->flac_tags);
  }

  free(flacstate);
}

