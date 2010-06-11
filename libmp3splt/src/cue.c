/**********************************************************
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2010 Alexandru Munteanu - io_fx@yahoo.fr
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *********************************************************/

#include <string.h>
#include <ctype.h>

#include "splt.h"
#include "cddb_cue_common.h"

#include "cue.h"

static int splt_cue_store_value(splt_state *state, char *in,
    int index, int tag_field);
static void splt_cue_write_title_performer(splt_state *state, FILE *file_output,
    int tags_index, short with_spaces, short write_album);
static void splt_cue_process_line(char **l, cue_utils *cu, splt_state *state);
static void splt_cue_process_track_line(char *line_content, cue_utils *cu, splt_state *state);
static void splt_cue_process_title_line(char *line_content, cue_utils *cu, splt_state *state);
static void splt_cue_process_performer_line(char *line_content, cue_utils *cu, splt_state *state);
static void splt_cue_process_index_line(char *line_content, cue_utils *cu, splt_state *state);
static cue_utils *splt_cue_cu_new(splt_state *state, int *error);
static void splt_cue_cu_free(cue_utils **cu);

int splt_cue_put_splitpoints(const char *file, splt_state *state, int *error)
{
  if (file == NULL)
  { 
    *error = SPLT_INVALID_CUE_FILE;
    return 0;
  }

  splt_c_put_info_message_to_client(state, 
      _(" reading informations from CUE file %s ...\n"),file);

  splt_t_free_splitpoints_tags(state);

  *error = SPLT_CUE_OK;

  int err = SPLT_OK;
  FILE *file_input = NULL;
  char *line = NULL;
  int tracks = -1;

  cue_utils *cu = splt_cue_cu_new(state, &err);
  if (err < 0) { *error = err; return tracks; }
  cu->file = file;

  int default_genre = 12;
  err = splt_tu_set_tags_field(state, 0, SPLT_TAGS_GENRE, &default_genre);
  if (err != SPLT_OK)
  {
    *error = err;
    return tracks;
  }

  if (!(file_input=splt_io_fopen(file, "r")))
  {
    splt_e_set_strerror_msg_with_data(state, file);
    *error = SPLT_ERROR_CANNOT_OPEN_FILE;
    return tracks;
  }

  if (fseek(file_input, 0, SEEK_SET) != 0)
  {
    splt_e_set_strerror_msg_with_data(state, file);
    *error = SPLT_ERROR_SEEKING_FILE;
    goto function_end;
  }

  while ((line = splt_io_readline(file_input, error)) != NULL)
  {
    if (*error < 0) { goto function_end; }

    splt_cue_process_line(&line, cu, state);
    tracks = cu->tracks;
    if (cu->error < 0) { *error = cu->error; goto function_end; }
  }

  err = splt_sp_append_splitpoint(state, LONG_MAX,
      _("description here"), SPLT_SPLITPOINT);

  if (cu->counter == 0)
  {
    splt_e_set_error_data(state, file);
    *error = SPLT_INVALID_CUE_FILE;
    goto function_end;
  }

  if (!cu->time_for_track) 
  {
    tracks--;
  }

  splt_tag_put_filenames_from_tags(state, tracks, error);

function_end:
  splt_cue_cu_free(&cu);

  if (line)
  {
    free(line);
    line = NULL;
  }

  if (fclose(file_input) != 0)
  {
    splt_e_set_strerror_msg_with_data(state, file);
    *error = SPLT_ERROR_CANNOT_CLOSE_FILE;
  }
  file_input = NULL;

  if (*error >= 0)
  {
    splt_c_put_info_message_to_client(state, _("  Tracks: %d\n\n"), tracks);
  }

  return tracks;
}

void splt_cue_export_to_file(splt_state *state, const char *out_file,
    short stop_at_total_time, int *error)
{
  int err = SPLT_OK;

  int num_of_splitpoints = splt_t_get_splitnumber(state);
  if (num_of_splitpoints <= 0)
  {
    return;
  }

  long total_time = splt_t_get_total_time(state);
  FILE *file_output = NULL;

  splt_d_print_debug(state, "Cue output file without output path = _%s_\n", out_file);

  char *dup_out_file = NULL;
  err = splt_su_copy(out_file, &dup_out_file);
  if (err < 0) { *error = err; return; }
  char *cue_out_file = splt_su_get_file_with_output_path(state, dup_out_file, &err);
  free(dup_out_file);
  dup_out_file = NULL;
  if (err < 0) { *error = err; goto end; }

  splt_d_print_debug(state, "Cue output file with output path = _%s_\n", cue_out_file);

  if (!(file_output = splt_io_fopen(cue_out_file, "w")))
  {
    splt_e_set_strerror_msg_with_data(state, cue_out_file);
    *error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
    goto end;
  }

  splt_cue_write_title_performer(state, file_output, 0, SPLT_FALSE, SPLT_TRUE);

  char *fname = splt_t_get_filename_to_split(state);

  char new_upper_ext[10] = { '\0' };
  const char *upper_ext = splt_p_get_upper_extension(state, &err);
  int i = 0;
  for (i = 1;i < strlen(upper_ext);i++)
  {
    new_upper_ext[i-1] = upper_ext[i];
  }

  fprintf(file_output, "FILE \"%s\" %s\n", fname, new_upper_ext);
  if (err < 0) { *error = err; goto end; }

  splt_t_set_current_split(state, 0);
  for (i = 0;i < num_of_splitpoints;i++)
  {
    long splitpoint = splt_sp_get_splitpoint_value(state, i, &err);
    if (err < 0) { *error = err; break; }

    if (stop_at_total_time)
    {
      //todo: splitpoint can be slightly != than total_time sometimes
      // (test with silence and cue)
      if (total_time > 0  && splitpoint >= total_time)
      {
        break;
      }
    }

    fprintf(file_output, "  TRACK %02d AUDIO\n", i+1);

    splt_cue_write_title_performer(state, file_output, -1, SPLT_TRUE, SPLT_FALSE);

    long mins = 0, secs = 0, hundr = 0;
    splt_sp_get_mins_secs_hundr_from_splitpoint(splitpoint, &mins, &secs, &hundr);
    fprintf(file_output, "    INDEX 01 %02ld:%02ld:%02ld\n", mins, secs, hundr);

    splt_t_current_split_next(state);
  }

end:
  fflush(file_output);
  if (fclose(file_output) != 0)
  {
    splt_e_set_strerror_msg_with_data(state, cue_out_file);
    *error = SPLT_ERROR_CANNOT_CLOSE_FILE;
  }
  file_output = NULL;

  splt_c_put_info_message_to_client(state, 
      _(" CUE file '%s' created.\n"), cue_out_file);

  if (cue_out_file)
  {
    free(cue_out_file);
    cue_out_file = NULL;
  }
}

static int splt_cue_store_value(splt_state *state, char *in,
    int index, int tag_field)
{
  int error = SPLT_OK;

  char *ptr_b = in;
  char *ptr_e = NULL;

  if (!in)
  {
    return SPLT_OK;
  }

  while (*ptr_b == ' ')
  {
    ptr_b++;
  }

  if (*ptr_b == '"')
  {
    ptr_b++;
  }

  ptr_e = strchr(ptr_b + 1, '\0');
  if (ptr_e)
  {
    while (*ptr_e == ' ' && ptr_e > in)
    {
      ptr_e--;
    }

    if (ptr_e > in)
    {
      if (*(ptr_e-1) == '"')
      {
        ptr_e--;
      }
      *ptr_e = '\0';
    }
  }

  char *out = NULL;
  error = splt_su_append(&out, ptr_b, strlen(ptr_b) + 1, NULL);
  if (error < 0) { return error; }

  if (tag_field == SPLT_TAGS_ARTIST)
  {
    splt_c_put_info_message_to_client(state, _("\n  Artist: %s\n"), out);
  }
  else if (tag_field == SPLT_TAGS_ALBUM)
  {
    splt_c_put_info_message_to_client(state, _("  Album: %s\n"), out);
  }

  error = splt_tu_set_tags_field(state, index, tag_field, out);
  if (out)
  {
    free(out);
    out = NULL;
  }

  return error;
}

static void splt_cue_write_title_performer(splt_state *state, FILE *file_output,
    int tags_index, short with_spaces, short write_album)
{
  splt_tags *tags = NULL;
  if (tags_index >= 0)
  {
    tags = splt_tu_get_tags_at(state, tags_index);
  }
  else
  {
    tags = splt_tu_get_current_tags(state);
  }

  if (tags)
  {
    if (write_album)
    {
      if (tags->album)
      {
        if (with_spaces) { fprintf(file_output, "    "); }
        fprintf(file_output, "TITLE \"%s\"\n", tags->album);
      }
    }
    else
    {
      if (tags->title)
      {
        if (with_spaces) { fprintf(file_output, "    "); }
        fprintf(file_output, "TITLE \"%s\"\n", tags->title);
      }
    }

    char *performer = splt_tu_get_artist_or_performer_ptr(tags);
    if (performer)
    {
      if (with_spaces) { fprintf(file_output, "    "); }
      fprintf(file_output, "PERFORMER \"%s\"\n", performer);
    }
  }
  else
  {
    if (with_spaces) { fprintf(file_output, "    "); }
    fprintf(file_output, "TITLE \"\"\n");
    if (with_spaces) { fprintf(file_output, "    "); }
    fprintf(file_output, "PERFORMER \"\"\n");
  }
}

static void splt_cue_process_line(char **l, cue_utils *cu, splt_state *state)
{
  if (!l || !*l) { return; }

  char *line = *l;

  splt_su_line_to_unix(line);
  splt_su_str_cut_last_char(line);

  splt_t_clean_one_split_data(state, cu->tracks);

  char *line_content = NULL;
  if (((line_content = strstr(line, "TRACK")) != NULL)
      && (strstr(line, "AUDIO") != NULL))
  {
    splt_cue_process_track_line(line_content, cu, state);
  }
  else if ((line_content = strstr(line, "TITLE")) != NULL)
  {
    splt_cue_process_title_line(line_content, cu, state);
  }
  else if ((line_content = strstr(line, "PERFORMER")) != NULL)
  {
    splt_cue_process_performer_line(line_content, cu, state);
  }
  else if ((line_content = strstr(line, "INDEX 01")) != NULL)
  {
    splt_cue_process_index_line(line_content, cu, state);
  }

  free(*l);
  *l = NULL;
}

static void splt_cue_process_track_line(char *line_content, cue_utils *cu, splt_state *state)
{
  line_content += 5;

  if (cu->tracks == -1) 
  {
    cu->tracks = 0;
  }

  if (!cu->time_for_track) 
  {
    splt_e_set_error_data(state, cu->file);
    cu->error = SPLT_INVALID_CUE_FILE;
  }

  cu->performer = SPLT_FALSE;
  cu->title = SPLT_FALSE;
  cu->time_for_track = SPLT_FALSE;
  cu->tracks++;

  splt_tu_new_tags_if_necessary(state, cu->tracks - 1);
}

static void splt_cue_process_title_line(char *line_content, cue_utils *cu, splt_state *state)
{
  int err = SPLT_OK;
  line_content += 5;

  if (cu->tracks == -1)
  {
    if ((err = splt_cue_store_value(state, line_content, 
            0, SPLT_TAGS_ALBUM)) != SPLT_OK)
    {
      cu->error = err;
      return;
    }
  }
  else
  {
    if (cu->tracks > 0)
    {
      if ((err = splt_cue_store_value(state, line_content,
              cu->tracks-1, SPLT_TAGS_TITLE)) != SPLT_OK)
      {
        cu->error = err;
        return;
      }
    }

    cu->title = SPLT_TRUE;
  }
}

static void splt_cue_process_performer_line(char *line_content, cue_utils *cu, splt_state *state)
{
  int err = SPLT_OK;
  line_content += 9;

  if (cu->tracks == -1)
  {
    //we always have one artist in a cue file, we
    //put the performers if more than one artist
    if ((err = splt_cue_store_value(state, line_content,
            0, SPLT_TAGS_ARTIST)) != SPLT_OK)
    {
      cu->error = err;
      return;
    }
  }
  else
  {
    if (cu->tracks > 0)
    {
      if ((err = splt_cue_store_value(state, line_content,
              cu->tracks - 1, SPLT_TAGS_PERFORMER)) != SPLT_OK)
      {
        cu->error = err;
        return;
      }
    }
 
    cu->performer = SPLT_TRUE;
  }

}

static void splt_cue_process_index_line(char *line_content, cue_utils *cu, splt_state *state)
{
  int err = SPLT_OK;

  line_content += 9;

  char *dot = NULL;
  if ((dot = strchr(line_content, ':')) == NULL)
  {
    splt_e_set_error_data(state, cu->file);
    cu->error = SPLT_INVALID_CUE_FILE;
    return;
  }

  if (cu->tracks <= 0)
  {
    return;
  }

  line_content[dot - line_content] = line_content[dot - line_content+3] = '.';

  long hundr_seconds = splt_co_convert_to_hundreths(line_content);
  if (hundr_seconds == -1)
  {
    splt_e_set_error_data(state, cu->file);
    cu->error = SPLT_INVALID_CUE_FILE;
    return;
  }

  err = splt_sp_append_splitpoint(state, hundr_seconds, NULL, SPLT_SPLITPOINT);
  if (err < 0) { cu->error = err; return; }

  cu->time_for_track = SPLT_TRUE;
  cu->counter++;
}

static cue_utils *splt_cue_cu_new(splt_state *state, int *error)
{
  cue_utils *cu = malloc(sizeof(cue_utils));
  if (cu == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return NULL;
  }

  cu->tracks = -1;
  cu->time_for_track = SPLT_TRUE;
  cu->performer = SPLT_FALSE;
  cu->title = SPLT_FALSE;
  cu->counter = 0;
  cu->file = NULL;
  cu->error = SPLT_OK;

  return cu;
}

static void splt_cue_cu_free(cue_utils **cu)
{
  if (!cu || !*cu)
  {
    return;
  }

  free(*cu);
  *cu = NULL;
}

