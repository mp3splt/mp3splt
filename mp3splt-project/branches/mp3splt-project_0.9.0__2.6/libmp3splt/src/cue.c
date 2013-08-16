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
 * 02111-1307, USA.
 *********************************************************/

/*!\file

All that is needed in order to be able to read and write cue files.
 */
#include <string.h>
#include <ctype.h>

#include "splt.h"
#include "cddb_cue_common.h"

#include "cue.h"

static void splt_cue_cu_free(cue_utils **cu);

//! Process the rest of a cue line that begins with the word TRACK
static void splt_cue_process_track_line(char *line_content, cue_utils *cu, splt_state *state)
{
  //skip the word TRACK
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

  cu->time_for_track = SPLT_FALSE;
  cu->tracks++;
  cu->current_track_type = SPLT_SPLITPOINT;

  splt_tu_new_tags_if_necessary(state, cu->tracks - 1);

  if (splt_o_get_int_option(state, SPLT_OPT_CUE_CDDB_ADD_TAGS_WITH_KEEP_ORIGINAL_TAGS))
  {
    int true_value = SPLT_TRUE;
    splt_tu_set_tags_field(state, cu->tracks - 1, SPLT_TAGS_ORIGINAL, &true_value);
  }
}

static void remove_trailing_spaces_and_quote(char *ptr_e, char *in)
{
  if (ptr_e)
  {
    ptr_e--;

    while (*ptr_e == ' ' && ptr_e > in)
    {
      ptr_e--;
    }

    if (ptr_e > in)
    {
      if (*ptr_e == '"')
      {
        *ptr_e = '\0';
      }
      else
      {
        *(ptr_e + 1) = '\0';
      }
    }
  }
}

static const char *splt_cue_parse_value(char *in, int skip_last_word)
{
  char *ptr_b = in;
  char *ptr_e = NULL;

  while (*ptr_b == ' ')
  {
    ptr_b++;
  }

  if (*ptr_b == '"')
  {
    ptr_b++;
  }

  ptr_e = strchr(ptr_b + 1, '\0');

  remove_trailing_spaces_and_quote(ptr_e, in);

  if (skip_last_word)
  {
    ptr_e = strrchr(ptr_b, ' ');
    remove_trailing_spaces_and_quote(ptr_e, in);
  }

  return ptr_b;
}

static char *splt_cue_parse_and_duplicate_value(char *in, int *error)
{
  const char *ptr_b = splt_cue_parse_value(in, SPLT_FALSE);

  char *out = NULL;
  int err = splt_su_append(&out, ptr_b, strlen(ptr_b) + 1, NULL);
  if (err < 0)
  {
    *error = err;
  }

  return out;
}

//! Update data for a track in the central splt_state structure.
static int splt_cue_store_value(splt_state *state, char *in, int index, int tag_field, cue_utils *cu)
{
  if (!in)
  {
    return SPLT_OK;
  }

  splt_code error = SPLT_OK;
  char *out = splt_cue_parse_and_duplicate_value(in, &error);
  if (error < 0) { return error; }

  error = splt_tu_set_tags_field(state, index, tag_field, out);
  if (out)
  {
    free(out);
    out = NULL;
  }

  return error;
}

//! Process the rest of a cue line that begins with the word TITLE
static void splt_cue_process_title_line(char *line_content, cue_utils *cu, splt_state *state)
{
  int err = SPLT_OK;

  //skip the word TITLE
  line_content += 5;

  if (cu->tracks == -1)
  {
    char *album = splt_cue_parse_and_duplicate_value(line_content, &err);
    if (err < 0) { cu->error = err; return; }

    if (album)
    {
      splt_c_put_info_message_to_client(state, _("  Album: %s\n"), album);
      err = splt_tu_set_field_on_tags(cu->all_tags, SPLT_TAGS_ALBUM, album);
      free(album);
      if (err < 0) { cu->error = err; return; }
    }
  }
  else
  {
    if (cu->tracks > 0)
    {
      if ((err = splt_cue_store_value(state, line_content, cu->tracks-1, SPLT_TAGS_TITLE, cu)) != SPLT_OK)
      {
        cu->error = err;
        return;
      }
    }
  }
}

//! Process the rest of a cue line that begins with the word PERFORMER
static void splt_cue_process_performer_line(char *line_content, cue_utils *cu, splt_state *state)
{
  int err = SPLT_OK;

  //skip the word PERFORMER
  line_content += 9;

  if (cu->tracks == -1)
  {
    char *artist = splt_cue_parse_and_duplicate_value(line_content, &err);
    if (err < 0) { cu->error = err; return; }

    if (artist)
    {
      splt_c_put_info_message_to_client(state, _("\n  Artist: %s\n"), artist);
      err = splt_tu_set_field_on_tags(cu->all_tags, SPLT_TAGS_ARTIST, artist);
      free(artist);
      if (err < 0) { cu->error = err; return; }
    }
  }
  else
  {
    if (cu->tracks > 0)
    {
      if ((err = splt_cue_store_value(state, line_content, cu->tracks - 1, SPLT_TAGS_ARTIST, cu)) != SPLT_OK)
      {
        cu->error = err;
        return;
      }

      if ((err = splt_cue_store_value(state, line_content, cu->tracks - 1, SPLT_TAGS_PERFORMER, cu)) != SPLT_OK)
      {
        cu->error = err;
        return;
      }
    }
  }
}

//! Process the rest of a cue line that begins with the word INDEX
static void splt_cue_process_index_line(char *line_content, cue_utils *cu, splt_state *state)
{
  int err = SPLT_OK;

  line_content += 9;

  if (cu->tracks <= 0)
  {
    return;
  }

  char *trimmed_line = splt_su_trim_spaces(line_content);

  long hundr_seconds = splt_co_convert_to_hundreths(trimmed_line);
  if (hundr_seconds == -1)
  {
    splt_e_set_error_data(state, cu->file);
    cu->error = SPLT_INVALID_CUE_FILE;
    return;
  }

  err = splt_sp_append_splitpoint(state, hundr_seconds, cu->current_name, cu->current_track_type);
  if (err < 0) { cu->error = err; return; }

  if (cu->current_name)
  {
    free(cu->current_name);
    cu->current_name = NULL;
  }

  cu->time_for_track = SPLT_TRUE;
  cu->counter++;
}

//! Process the rest of a cue line that begins with the word REM
static void splt_cue_process_rem_line(char *line_content, cue_utils *cu, splt_state *state)
{
  if (cu->tracks < 0)
  {
    return;
  }

  //skip the word REM
  line_content += 3;

  //skip all leading whitespace after the word REM
  while ((*line_content == ' ') || (*line_content == '\t'))
  {
    line_content++;
  }

  if (strstr(line_content, "NOKEEP") != NULL)
  {
    cu->current_track_type = SPLT_SKIPPOINT;
    return;
  }

  int err = SPLT_OK;

  if (strncmp(line_content, "ALBUM", 5) == 0)
  {
    line_content += 5;
    const char *album = splt_cue_parse_value(line_content, SPLT_FALSE);
    err = splt_tu_set_tags_field(state, cu->tracks-1, SPLT_TAGS_ALBUM, album);
    if (err != SPLT_OK) { cu->error = err; return; }
    return;
  }

  if (strncmp(line_content, "GENRE", 5) == 0)
  {
    line_content += 5;
    const char *genre = splt_cue_parse_value(line_content, SPLT_FALSE);
    err = splt_tu_set_tags_field(state, cu->tracks-1, SPLT_TAGS_GENRE, genre);
    if (err != SPLT_OK) { cu->error = err; return; }
    return;
  }

  if (strncmp(line_content, "DATE", 4) == 0)
  {
    line_content += 4;
    const char *year = splt_cue_parse_value(line_content, SPLT_FALSE);
    err = splt_tu_set_tags_field(state, cu->tracks-1, SPLT_TAGS_YEAR, year);
    if (err != SPLT_OK) { cu->error = err; return; }
    return;
  }

  if (strncmp(line_content, "TRACK", 5) == 0)
  {
    line_content += 5;
    const char *track = splt_cue_parse_value(line_content, SPLT_FALSE);
    int tracknumber = atoi(track);
    err = splt_tu_set_tags_field(state, cu->tracks-1, SPLT_TAGS_TRACK, &tracknumber);
    if (err != SPLT_OK) { cu->error = err; return; }
    return;
  }

  if (strncmp(line_content, "COMMENT", 7) == 0)
  {
    line_content += 7;
    const char *comment = splt_cue_parse_value(line_content, SPLT_FALSE);
    err = splt_tu_set_tags_field(state, cu->tracks-1, SPLT_TAGS_COMMENT, comment);
    if (err != SPLT_OK) { cu->error = err; return; }
    return;
  }


  if (strncmp(line_content, "NAME", 4) == 0)
  {
    line_content += 4;
    const char *name = splt_cue_parse_value(line_content, SPLT_FALSE);
    if (name != NULL)
    {
      if (cu->current_name)
      {
        free(cu->current_name);
        cu->current_name = NULL;
      }
      cu->current_name = strdup(name);
    }
  }
}

//! Process the rest of a cue line that begins with the word FILE
static void splt_cue_process_file_line(char *line_content, cue_utils *cu, splt_state *state)
{
  if (!splt_o_get_int_option(state, SPLT_OPT_SET_FILE_FROM_CUE_IF_FILE_TAG_FOUND))
  {
    return;
  }

  //skip the word FILE
  line_content += 4;

  int err = SPLT_OK;

  const char *file_from_cue = splt_cue_parse_value(line_content, SPLT_TRUE);

  if (splt_io_check_if_file(NULL, file_from_cue))
  {
    err = splt_t_set_filename_to_split(state, file_from_cue);
    if (err < 0) { cu->error = err; }
    return;
  }

  char *file_from_cue_with_path = NULL;
  splt_su_copy(cu->file, &file_from_cue_with_path);
  splt_su_keep_path_and_remove_filename(file_from_cue_with_path);
  splt_su_append_str(&file_from_cue_with_path, SPLT_DIRSTR, file_from_cue, NULL);

  if (splt_io_check_if_file(NULL, file_from_cue_with_path))
  {
    err = splt_t_set_filename_to_split(state, file_from_cue_with_path);
  }

  if (file_from_cue_with_path)
  {
    free(file_from_cue_with_path);
    file_from_cue_with_path = NULL;
  }

  if (err < 0) { cu->error = err; return; }
}

/*! Analyze a line from a cue file

 */
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
  else if ((line_content = strstr(line, "REM")) != NULL)
  {
    splt_cue_process_rem_line(line_content, cu, state);
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
  else if ((line_content = strstr(line, "FILE")) != NULL)
  {
    splt_cue_process_file_line(line_content, cu, state);
  }

  free(*l);
  *l = NULL;
}


/* Malloc memory for and initialize a cue_utils structure

\param error Contains the libmp3splt error number if any error
       occoured in this step.
\return The address of the structure
 */
static cue_utils *splt_cue_cu_new(int *error)
{
  cue_utils *cu = malloc(sizeof(cue_utils));
  if (cu == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return NULL;
  }

  cu->tracks = -1;
  cu->time_for_track = SPLT_TRUE;
  cu->counter = 0;
  cu->file = NULL;
  cu->error = SPLT_OK;
  cu->current_track_type = SPLT_SPLITPOINT;
  cu->current_name = NULL;

  int err = SPLT_OK;
  cu->all_tags = splt_tu_new_tags(&err);
  if (err < 0)
  {
    *error = err;
    splt_cue_cu_free(&cu);
    return NULL;
  }

  return cu;
}

/*! Free the memory allocated by a cue_utils structure

This function frees the memory that has been allocated by
splt_cue_cu_new.
 */
static void splt_cue_cu_free(cue_utils **cu)
{
  if (!cu || !*cu)
  {
    return;
  }

  if ((*cu)->current_name)
  {
    free((*cu)->current_name);
    (*cu)->current_name = NULL;
  }

  splt_tu_free_one_tags(&(*cu)->all_tags);

  free(*cu);
  *cu = NULL;
}

int splt_cue_put_splitpoints(const char *file, splt_state *state, int *error)
{
  if (file == NULL)
  { 
    splt_e_set_error_data(state, file);
    *error = SPLT_INVALID_CUE_FILE;
    return 0;
  }

  splt_c_put_info_message_to_client(state, 
      _(" reading informations from CUE file %s ...\n"), file);

  splt_t_free_splitpoints_tags(state);

  *error = SPLT_CUE_OK;

  int err = SPLT_OK;
  FILE *file_input = NULL;
  char *line = NULL;
  int tracks = -1;

  cue_utils *cu = splt_cue_cu_new(&err);
  
  if (err < 0) { *error = err; return tracks; }
  cu->file = file;

  if (!(file_input = splt_io_fopen(file, "r")))
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

  if (!splt_o_get_int_option(state, SPLT_OPT_CUE_SET_SPLITPOINT_NAMES_FROM_REM_NAME))
  {
    splt_cc_put_filenames_from_tags(state, tracks, error, cu->all_tags, SPLT_TRUE);
  }

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

/*! Write title and performer information to a file

\param state The central struct that keeps all variables for this library
\param file_output The file descriptor to output the track info to
\param tags_index The number of the split point we have to output
       information for
\param with_spaces !=0 means we use indentation to make the cue file
       look nicer
 */
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

  if (!tags)
  {
    if (with_spaces) { fprintf(file_output, "    "); }
    fprintf(file_output, "TITLE \"\"\n");
    if (with_spaces) { fprintf(file_output, "    "); }
    fprintf(file_output, "PERFORMER \"\"\n");
    return;
  }

  if (write_album)
  {
    const char *album = splt_tu_get_tags_value(tags, SPLT_TAGS_ALBUM);
    if (album)
    {
      if (with_spaces) { fprintf(file_output, "    "); }
      fprintf(file_output, "TITLE \"%s\"\n", album);
    }
  }
  else
  {
    const char *title = splt_tu_get_tags_value(tags, SPLT_TAGS_TITLE);
    if (title)
    {
      if (with_spaces) { fprintf(file_output, "    "); }
      fprintf(file_output, "TITLE \"%s\"\n", title);
    }
  }

  char *performer = splt_tu_get_artist_or_performer_ptr(tags);
  if (performer)
  {
    if (with_spaces) { fprintf(file_output, "    "); }
    fprintf(file_output, "PERFORMER \"%s\"\n", performer);
  }
}

static void splt_cue_write_other_tags(splt_state *state, FILE *file_output)
{
  splt_tags *tags = splt_tu_get_current_tags(state);
  if (!tags) { return; }

  const char *album = splt_tu_get_tags_value(tags, SPLT_TAGS_ALBUM);
  if (album != NULL && (strcmp(album, "") != 0))
  {
    fprintf(file_output, "    REM ALBUM \"%s\"\n", album);
  }

  const char *genre = splt_tu_get_tags_value(tags, SPLT_TAGS_GENRE);
  if (genre != NULL && (strcmp(genre, "") != 0))
  {
    fprintf(file_output, "    REM GENRE \"%s\"\n", genre);
  }

  const char *year = splt_tu_get_tags_value(tags, SPLT_TAGS_YEAR);
  if (year != NULL && (strcmp(year, "") != 0))
  {
    fprintf(file_output, "    REM DATE \"%s\"\n", year);
  }

  if (!tags->was_auto_incremented)
  {
    const int *track = splt_tu_get_tags_value(tags, SPLT_TAGS_TRACK);
    if (track != NULL && *track > 0)
    {
      fprintf(file_output, "    REM TRACK \"%d\"\n", *track);
    }
  }

  const char *comment = splt_tu_get_tags_value(tags, SPLT_TAGS_COMMENT);
  if (comment != NULL && (strcmp(comment, "") != 0))
  {
    fprintf(file_output, "    REM COMMENT \"%s\"\n", comment);
  }
}

/*! Export all split points to a cue file

  \param out_file The name of the file to output the split points to
  \param state The splt_state structure containing the split points
  \param error Contains the error code if anything goes wrong
  \param stop_at_total_time If this parameter is !=0 we don't output
  splitpoints that lie beyond the end of the audio data. Note that the
  last splitpoint can be slightly beyond the calculated end of audio
  data.
 */
void splt_cue_export_to_file(splt_state *state, const char *out_file,
    short stop_at_total_time, int *error)
{
  int err = SPLT_OK;

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

  const char *extension = strrchr(fname, '.');
  if (extension != NULL)
  {
    char upper_extension[5] = { '\0' };
    int i = 1;
    for (i = 1;i < strlen(extension);i++)
    {
      upper_extension[i-1] = (char) toupper((int)extension[i]);
    }

    fprintf(file_output, "FILE \"%s\" %s\n", fname, upper_extension);
    if (err < 0) { *error = err; goto end; }
  }
  else
  {
    fprintf(file_output, "FILE \"%s\"\n", fname);
    if (err < 0) { *error = err; goto end; }
  }

  splt_t_set_current_split(state, 0);
  int num_of_splitpoints = splt_sp_get_real_splitpoints_number(state);
  int i;
  for (i = 0;i < num_of_splitpoints;i++)
  {
    long splitpoint = splt_sp_get_splitpoint_value(state, i, &err);
    if (err < 0) { *error = err; break; }

    int splitpoint_type = splt_sp_get_splitpoint_type(state, i, &err);
    if (err < 0) { *error = err; break; }

    const char *name = splt_sp_get_splitpoint_name(state, i, &err);
    if (err < 0) { *error = err; break; }

    //todo: splitpoint can be slightly != than total_time sometimes
    // (test with silence and cue)
    if (stop_at_total_time &&
        (total_time > 0  && splitpoint >= total_time))
    {
      break;
    }

    fprintf(file_output, "  TRACK %02d AUDIO\n", i+1);

    splt_cue_write_title_performer(state, file_output, -1, SPLT_TRUE, SPLT_FALSE);

    splt_cue_write_other_tags(state, file_output);

    if (name != NULL)
    {
      fprintf(file_output, "    REM NAME \"%s\"\n", name);
    }

    if (splitpoint_type == SPLT_SKIPPOINT)
    {
      fprintf(file_output, "    REM NOKEEP\n");
    }

    long mins = 0, secs = 0, hundr = 0;
    if (splitpoint == LONG_MAX)
    {
      splitpoint = total_time;
    }
    splt_sp_get_mins_secs_hundr_from_splitpoint(splitpoint, &mins, &secs, &hundr);
    fprintf(file_output, "    INDEX 01 %02ld:%02ld:%02ld\n", mins, secs, hundr);

    splt_t_set_current_split_file_number_next(state);
  }

  fflush(file_output);
  if (fclose(file_output) != 0)
  {
    splt_e_set_strerror_msg_with_data(state, cue_out_file);
    *error = SPLT_ERROR_CANNOT_CLOSE_FILE;
  }
  file_output = NULL;

end:
  if (!splt_o_get_int_option(state, SPLT_OPT_CUE_DISABLE_CUE_FILE_CREATED_MESSAGE))
  {
    splt_c_put_info_message_to_client(state, _(" CUE file '%s' created.\n"), cue_out_file);
  }

  if (cue_out_file)
  {
    free(cue_out_file);
    cue_out_file = NULL;
  }
}

