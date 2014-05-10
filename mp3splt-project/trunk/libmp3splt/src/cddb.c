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
 * 02111-1307, USA.
 *********************************************************/

/*! \file

  The cddb search functionality
 */
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "splt.h"
#include "cddb_cue_common.h"

#include "cddb.h"

static void splt_cddb_process_line(char **l, cddb_utils *cdu, splt_state *state);
static void splt_cddb_process_disc_length_line(const char *line_content, cddb_utils *cdu, splt_state *state);
static void splt_cddb_convert_points(cddb_utils *cdu, splt_state *state);
static void splt_cddb_process_offset_line(const char *line_content, cddb_utils *cdu, splt_state *state);
static cddb_utils *splt_cddb_cdu_new(splt_state *state, int *error);
static void splt_cddb_cdu_free(cddb_utils **cdu);
static void splt_cddb_process_year_line(const char *line_content,
    cddb_utils *cdu, splt_state *state);
static void splt_cddb_process_genre_line(char *line_content, cddb_utils *cdu, splt_state *state);
static void splt_cddb_process_dtitle_line(const char *line_content, cddb_utils *cdu, splt_state *state);
static void splt_cddb_process_ttitle_line(const char *line_content, cddb_utils *cdu, splt_state *state);
static void splt_cddb_process_id3g_line(const char *line_content, cddb_utils *cdu, splt_state *state);

int splt_cddb_put_splitpoints(const char *file, splt_state *state, int *error)
{
  if (file == NULL)
  {
    splt_e_set_error_data(state, file);
    *error = SPLT_INVALID_CDDB_FILE;
    return 0;
  }

  splt_c_put_info_message_to_client(state, 
      _(" reading informations from CDDB file %s ...\n"),file);

  splt_t_free_splitpoints_tags(state);

  *error = SPLT_CDDB_OK;

  int err = SPLT_OK;
  FILE *file_input = NULL;
  char *line = NULL;
  int tracks = 0;

  cddb_utils *cdu = splt_cddb_cdu_new(state, &err);
  if (err < 0) { *error = err; return tracks; }
  cdu->file = file;

  if (!(file_input = splt_io_fopen(file, "r")))
  {
    splt_cddb_cdu_free(&cdu);
    splt_e_set_strerror_msg_with_data(state, file);
    *error = SPLT_ERROR_CANNOT_OPEN_FILE;
    return tracks;
  }

  splt_tags *all_tags = splt_tu_new_tags(error);

  if (fseek(file_input, 0, SEEK_SET) != 0)
  {
    splt_e_set_strerror_msg_with_data(state, file);
    *error = SPLT_ERROR_SEEKING_FILE;
    goto function_end;
  }

  err = splt_tu_set_tags_field(state, 0, SPLT_TAGS_GENRE, SPLT_UNDEFINED_GENRE);
  if (err < 0) { *error = err; goto function_end; }
 
  while ((line = splt_io_readline(file_input, error)) != NULL)
  {
    if (*error < 0) { goto function_end; }

    splt_cddb_process_line(&line, cdu, state);
    tracks = cdu->tracks;
    if (cdu->error < 0) { *error = cdu->error; goto function_end; }
  }

  if (*error < 0) { goto function_end; }
  splt_tags *tags_at_0 = splt_tu_get_tags_at(state, 0);
  splt_tu_copy_tags(tags_at_0, all_tags, error);
  if (*error < 0) { goto function_end; }

  splt_cc_put_filenames_from_tags(state, tracks, error, all_tags, SPLT_FALSE, SPLT_FALSE);

function_end:
  splt_tu_free_one_tags(&all_tags);
  splt_cddb_cdu_free(&cdu);

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

static void splt_cddb_process_line(char **l, cddb_utils *cdu, splt_state *state)
{
  if (!l || !*l) { return; }

  char *line = *l;

  splt_su_line_to_unix(line);
  splt_su_str_cut_last_char(line);

  char *line_content = NULL;
  if ((line_content = strstr(line, "Track frame offset")) != NULL)
  {
    cdu->read_offsets = SPLT_TRUE;
  }
  else if ((line_content = strstr(line, "Disc length")) != NULL)
  {
    splt_cddb_process_disc_length_line(line_content, cdu, state);
  }
  else if (cdu->read_offsets)
  {
    splt_cddb_process_offset_line(line, cdu, state);
  }
  else if ((line_content = strstr(line, "YEAR")) != NULL)
  {
    splt_cddb_process_year_line(line_content, cdu, state);
  }
  else if ((line_content = strstr(line, "GENRE")) != NULL)
  {
    splt_cddb_process_genre_line(line_content, cdu, state);
  }
  else if ((line_content = strstr(line, "DTITLE")) != NULL)
  {
    splt_cddb_process_dtitle_line(line_content, cdu, state);
  }
  else if ((line_content = strstr(line, "TTITLE")) != NULL)
  {
    splt_cddb_process_ttitle_line(line_content, cdu, state);
  }
  else if ((line_content = strstr(line, "ID3G")) != NULL)
  {
    splt_cddb_process_id3g_line(line_content, cdu, state);
  }

  free(*l);
  *l = NULL;
}

static void splt_cddb_process_id3g_line(const char *line_content, cddb_utils *cdu, splt_state *state)
{
  int err = SPLT_OK;

  int id3g = atoi(line_content + 6);
  if (id3g < SPLT_ID3V1_NUMBER_OF_GENRES)
  {
    err = splt_tu_set_tags_field(state, 0, SPLT_TAGS_GENRE, splt_id3v1_genres[id3g]);
    if (err < 0) { cdu->error = err; return; }
  }
}

static void splt_cddb_process_ttitle_line(const char *line_content, cddb_utils *cdu, splt_state *state)
{
  int err = SPLT_OK;

  int index = atoi(line_content+6);

  char *equal_ptr = NULL;
  if ((equal_ptr = strchr(line_content, '=')) == NULL) 
  {
    splt_e_set_error_data(state, cdu->file);
    cdu->error = SPLT_INVALID_CDDB_FILE;
    return;
  }

  if (equal_ptr == line_content)
  {
    splt_e_set_error_data(state, cdu->file);
    cdu->error = SPLT_INVALID_CDDB_FILE;
    return;
  }

  char *slash = strchr(equal_ptr, '/');
  if (slash != NULL)
  {
    char *title = splt_su_trim_spaces(slash + 1);
    err = splt_tu_set_tags_field(state, index, SPLT_TAGS_TITLE, title);
    if (err < 0) { cdu->error = err; return; }
    *slash = '\0';

    char *performer = splt_su_trim_spaces(equal_ptr + 1);
    err = splt_tu_set_tags_field(state, index, SPLT_TAGS_PERFORMER, performer);
    if (err < 0) { cdu->error = err; return; }
  }
  else
  {
    char *title = splt_su_trim_spaces(equal_ptr + 1);
    err = splt_tu_set_tags_field(state, index, SPLT_TAGS_TITLE, title);
    if (err < 0) { cdu->error = err; return; }
  }

  if (splt_o_get_int_option(state, SPLT_OPT_CUE_CDDB_ADD_TAGS_WITH_KEEP_ORIGINAL_TAGS))
  {
    int true_value = SPLT_TRUE;
    splt_tu_set_tags_field(state, index, SPLT_TAGS_ORIGINAL, &true_value);
  }
}

static void splt_cddb_process_dtitle_line(const char *line_content, cddb_utils *cdu, splt_state *state)
{
  int err = SPLT_OK;

  char *equal_ptr = NULL;
  if ((equal_ptr = strchr(line_content, '=')) == NULL) 
  {
    splt_e_set_error_data(state, cdu->file);
    cdu->error = SPLT_INVALID_CDDB_FILE;
    return;
  }

  int we_have_album = SPLT_FALSE;

  char *slash = strchr(equal_ptr, '/');
  if (slash != NULL)
  {
    char *album = splt_su_trim_spaces(slash + 1);
    err = splt_tu_set_tags_field(state, 0, SPLT_TAGS_ALBUM, album);
    if (err < 0) { cdu->error = err; return; }
    *slash = '\0';
    we_have_album = SPLT_TRUE;
  }

  char *artist = splt_su_trim_spaces(equal_ptr + 1);
  err = splt_tu_set_tags_field(state, 0, SPLT_TAGS_ARTIST, artist);
  if (err < 0) { cdu->error = err; return; }

  splt_c_put_info_message_to_client(state, _("\n  Artist: %s\n"), artist);
  if (we_have_album)
  {
    splt_c_put_info_message_to_client(state, _("  Album: %s\n"), 
        splt_tu_get_tags_field(state, 0, SPLT_TAGS_ALBUM));
  }
}

static void splt_cddb_process_genre_line(char *line_content, cddb_utils *cdu, splt_state *state)
{
  char *genre = line_content + 6;
  splt_su_cut_spaces_from_end(genre);

  if (*genre == '\0')
  {
    return;
  }

  int err = splt_tu_set_tags_field(state, 0, SPLT_TAGS_GENRE, genre);
  if (err < 0) { cdu->error = err; }
}

static void splt_cddb_process_year_line(const char *line_content,
    cddb_utils *cdu, splt_state *state)
{
  int err = splt_tu_set_tags_field(state, 0, SPLT_TAGS_YEAR, line_content+5);
  if (err < 0) { cdu->error = err; }
}

static void splt_cddb_process_disc_length_line(const char *line_content,
    cddb_utils *cdu, splt_state *state)
{
  int err = SPLT_OK;

  cdu->read_offsets = SPLT_FALSE;

  splt_t_set_splitnumber(state, cdu->tracks);
  splt_t_clean_split_data(state, cdu->tracks);

  double value = splt_su_str_line_to_double(line_content);
  err = splt_sp_append_splitpoint(state, value * 100, NULL, SPLT_SPLITPOINT);

  if (err < 0) { cdu->error = err; return; }

  splt_cddb_convert_points(cdu, state);
}

static void splt_cddb_convert_points(cddb_utils *cdu, splt_state *state)
{
  int err = SPLT_OK;

  long first_point = splt_sp_get_splitpoint_value(state, 0, &err);
  if (err < 0) { cdu->error = err; return; }

  int i = 0;
  for (i = cdu->tracks - 1; i >= 0; i--)
  {
    long point = splt_sp_get_splitpoint_value(state, i, &err);
    if (err < 0) { cdu->error = err; return; }

    //cddb specs
    long difference = point - first_point;
    float value = (float) difference / 75.f;
    err = splt_sp_set_splitpoint_value(state, i, (long) ceilf(value));
    if (err < 0) { cdu->error = err; return; }
  }
}

static void splt_cddb_process_offset_line(const char *line, 
    cddb_utils *cdu, splt_state *state)
{
  if (splt_su_str_line_has_digit(line))
  {
    int err = SPLT_OK;

    double value = splt_su_str_line_to_double(line);
    err = splt_sp_append_splitpoint(state, value * 100, NULL, SPLT_SPLITPOINT);

    if (err < 0) { cdu->error = err; return; }

    cdu->tracks++;
  }
}

static cddb_utils *splt_cddb_cdu_new(splt_state *state, int *error)
{
  cddb_utils *cdu = malloc(sizeof(cddb_utils));
  if (cdu == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return NULL;
  }

  cdu->read_offsets = SPLT_FALSE;
  cdu->error = SPLT_OK;
  cdu->tracks = 0;
  cdu->file = NULL;
  cdu->field_counter = 0;

  return cdu;
}

static void splt_cddb_cdu_free(cddb_utils **cdu)
{
  if (!cdu || !*cdu)
  {
    return;
  }

  free(*cdu);
  *cdu = NULL;
}

