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

#include "splt.h"

//used by cddb and cue functions
//the cddb and cue functions does not fill in all the tags, this
//function completes the fill
//and calls another function that sets the output filenames
void splt_tag_put_filenames_from_tags(splt_state *state,
    int tracks, int *error)
{
  int i = 0;

  char *artist0 = NULL;
  char *first_artist = (char *)splt_tu_get_tags_field(state, 0, SPLT_TAGS_ARTIST);
  artist0 = splt_su_safe_strdup(first_artist, error);
  if (*error < 0) { goto function_end; }

  char *album0 = NULL;
  char *first_album = (char *)splt_tu_get_tags_field(state, 0, SPLT_TAGS_ALBUM);
  album0 = splt_su_safe_strdup(first_album, error);
  if (*error < 0) { goto function_end; }

  char *year0 = NULL;
  char *first_year = (char *)splt_tu_get_tags_field(state, 0, SPLT_TAGS_YEAR);
  year0 = splt_su_safe_strdup(first_year, error);
  if (*error < 0) { goto function_end; }

  unsigned char genre0 = 0x0;
  unsigned char *g = (unsigned char *)splt_tu_get_tags_field(state, 0, SPLT_TAGS_GENRE);
  if (g != NULL) {
    genre0 = *g;
  }
  int tags_error = SPLT_OK;

  if (splt_t_get_int_option(state, SPLT_OPT_OUTPUT_FILENAMES) ==
      SPLT_OUTPUT_DEFAULT)
  {
    splt_t_set_oformat(state, SPLT_DEFAULT_CDDB_CUE_OUTPUT, error, SPLT_TRUE);
    if (*error < 0) { goto function_end; }
  }

  //we put the same artist, album, year and genre everywhere
  for (i = 0; i < tracks;i++)
  {
    if (i != 0)
    {
      tags_error = splt_tu_set_tags_field(state, i, SPLT_TAGS_ARTIST, artist0);
      if (tags_error != SPLT_OK)
      {
        *error = tags_error;
        goto function_end;
      }

      tags_error = splt_tu_set_tags_field(state, i, SPLT_TAGS_ALBUM, album0);
      if (tags_error != SPLT_OK)
      {
        *error = tags_error;
        goto function_end;
      }
      tags_error = splt_tu_set_tags_field(state, i, SPLT_TAGS_YEAR, year0);
      if (tags_error != SPLT_OK)
      {
        *error = tags_error;
        goto function_end;
      }
      tags_error = splt_tu_set_tags_field(state, i, SPLT_TAGS_GENRE, &genre0);
      if (tags_error != SPLT_OK)
      {
        *error = tags_error;
        goto function_end;
      }
    }
  }

  if (*error >= 0)
  {
    int err_format = SPLT_OK;

    const char *format = splt_t_get_oformat(state);
    if (format != NULL)
    {
      //we put the outputted filename
      char *old_format = strdup(format);
      if (old_format != NULL)
      {
        splt_t_set_oformat(state, old_format,&err_format, SPLT_TRUE);
        free(old_format);
        old_format = NULL;
      }
      else
      {
        *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
        goto function_end;
      }
    }

    splt_t_set_oformat_digits_tracks(state, tracks);

    if (err_format >= 0)
    {
      //we set the current split to 0
      splt_t_set_current_split(state,0);
      int current_split = 0;
      do {
        int filename_error = SPLT_OK;
        filename_error =
          splt_u_finish_tags_and_put_output_format_filename(state, current_split);
        if (filename_error != SPLT_OK)
        {
          *error = filename_error;
          break;
        }
        splt_t_current_split_next(state);
        current_split = splt_t_get_current_split(state);
      } while (current_split < tracks);
    }
    else
    {
      *error = err_format;
    }
  }

function_end:
  //free some memory
  if (artist0)
  {
    free(artist0);
    artist0 = NULL;
  }
  if (album0)
  {
    free(album0);
    album0 = NULL;
  }
  if (year0)
  {
    free(year0);
    year0 = NULL;
  }
}


