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

#include "flac_tags.h"

static FLAC__uint32 splt_flac_t_read_unsigned_integer_of_32_bits(unsigned char *data)
{
  return splt_flac_l_unpack_uint32_little_endian(data, 4);
}

static unsigned char *splt_flac_t_read_vector(unsigned char *ptr, FLAC__uint32 length)
{
  unsigned char *new = malloc(sizeof(unsigned char) * (length + 1));
  if (new == NULL) { return NULL; }
  memcpy(new, ptr, length);
  new[length] = '\0';
  return new;
}

splt_flac_vorbis_tags *splt_flac_vorbis_tags_new(splt_code *error)
{
  splt_flac_vorbis_tags *vorbis_tags = malloc(sizeof(splt_flac_vorbis_tags));
  if (vorbis_tags == NULL) { *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY; return NULL; }

  vorbis_tags->number_of_tags = 0;
  vorbis_tags->total_bytes = 0;
  vorbis_tags->tags = NULL;

  return vorbis_tags;
}

void splt_flac_vorbis_tags_free(splt_flac_vorbis_tags **vorbis_tags)
{
  if (!vorbis_tags || !*vorbis_tags)
  {
    return;
  }

  if ((*vorbis_tags)->tags)
  {
    FLAC__uint32 i = 0;
    for (;i < (*vorbis_tags)->number_of_tags; i++)
    {
      free((*vorbis_tags)->tags[i]);
    }

    free((*vorbis_tags)->tags);
  }

  free(*vorbis_tags);
  *vorbis_tags = NULL;
}

void splt_flac_vorbis_tags_append_with_prefix(splt_flac_vorbis_tags *vorbis_tags,
    char *prefix, char *comment, splt_code *error)
{
  if (comment == NULL || strlen(comment) == 0 || comment[0] == '\0') { return; }

  char *all_comment = NULL;
  int err = splt_su_append_str(&all_comment, prefix, comment, NULL);
  if (err < 0) { *error = err; return; }

  splt_flac_vorbis_tags_append(vorbis_tags, all_comment, error);

  free(all_comment);
}

void splt_flac_vorbis_tags_append(splt_flac_vorbis_tags *vorbis_tags,
    char *comment, splt_code *error)
{
  if (vorbis_tags->tags == NULL)
  {
    vorbis_tags->tags = malloc(sizeof(char *));
  }
  else
  {
    size_t realloc_size = sizeof(char *) * (vorbis_tags->number_of_tags + 1);
    vorbis_tags->tags = realloc(vorbis_tags->tags, realloc_size);
  }

  if (vorbis_tags->tags == NULL) { *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY; return; }
  vorbis_tags->tags[vorbis_tags->number_of_tags] = NULL;

  int err = splt_su_copy(comment, &vorbis_tags->tags[vorbis_tags->number_of_tags]);
  if (err < 0) { *error = err; return; }

  vorbis_tags->number_of_tags++;

  vorbis_tags->total_bytes += 4 + strlen(comment);
}

static void splt_flac_t_parse_and_store_comment(char *comment,
    splt_flac_tags *flac_tags, int index, splt_code *error)
{
  if (comment == NULL) { return; }

  int err = SPLT_OK;
  splt_tags *tags = flac_tags->original_tags;

  if (strncmp(comment, "TITLE=", 6) == 0)
  {
    err = splt_tu_set_field_on_tags(tags, SPLT_TAGS_TITLE, comment + 6);
    if (err < 0) { *error = err; }
    return;
  }

  if (strncmp(comment, "ALBUM=", 6) == 0)
  {
    err = splt_tu_set_field_on_tags(tags, SPLT_TAGS_ALBUM, comment + 6);
    if (err < 0) { *error = err; }
    return;
  }

  if (strncmp(comment, "TRACKNUMBER=", 12) == 0)
  {
    int track = atoi(comment + 12);
    err = splt_tu_set_field_on_tags(tags, SPLT_TAGS_TRACK, &track);
    if (err < 0) { *error = err; }
    return;
  }

  if (strncmp(comment, "ARTIST=", 7) == 0)
  {
    err = splt_tu_set_field_on_tags(tags, SPLT_TAGS_ARTIST, comment + 7);
    if (err < 0) { *error = err; }
    return;
  }

  /*if (strncmp(comment, "PERFORMER=", 10) == 0)
  {
    err = splt_tu_set_field_on_tags(tags, SPLT_TAGS_PERFORMER, comment + 10);
    if (err < 0) { *error = err; }
    return;
  }*/

  if (strncmp(comment, "GENRE=", 6) == 0)
  {
    err = splt_tu_set_field_on_tags(tags, SPLT_TAGS_GENRE, comment + 6);
    if (err < 0) { *error = err; }
    return;
  }

  if (strncmp(comment, "DATE=", 5) == 0)
  {
    //TODO: different date formats
    err = splt_tu_set_field_on_tags(tags, SPLT_TAGS_YEAR, comment + 5);
    if (err < 0) { *error = err; }
    return;
  }

  if (strncmp(comment, "COMMENT=", 8) == 0)
  {
    err = splt_tu_set_field_on_tags(tags, SPLT_TAGS_COMMENT, comment + 8);
    if (err < 0) { *error = err; }
    return;
  }

  splt_flac_vorbis_tags_append(flac_tags->other_tags, comment, error);
}

static void splt_flac_t_parse_comments(unsigned char *comments,
    FLAC__uint32 total_block_length, splt_flac_tags *flac_tags, splt_code *error)
{
  int counter = 0;
  unsigned char *ptr = comments;
  FLAC__uint32 vendor_length = splt_flac_t_read_unsigned_integer_of_32_bits(ptr);
  ptr += 4;
  counter += 4;
  if (counter > total_block_length) { goto error; }

  unsigned char *vendor_string = splt_flac_t_read_vector(ptr, vendor_length);
  if (vendor_string == NULL) { *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY; return; }
  int err = splt_su_copy((char *) vendor_string, &flac_tags->vendor_string);
  flac_tags->vendor_length = vendor_length;
  if (err < 0) {
    *error = err;
    free(vendor_string);
    return;
  }

  free(vendor_string);
  ptr += vendor_length;
  counter += vendor_length;
  if (counter > total_block_length) { goto error; }

  FLAC__uint32 number_of_comments = splt_flac_t_read_unsigned_integer_of_32_bits(ptr);
  ptr += 4;
  counter += 4;
  if (counter > total_block_length) { goto error; }

  flac_tags->other_tags = splt_flac_vorbis_tags_new(error);
  if (*error < 0) { return; }

  FLAC__uint32 i = 0;
  for (;i < number_of_comments;i++)
  {
    FLAC__uint32 comment_length = splt_flac_t_read_unsigned_integer_of_32_bits(ptr);
    ptr += 4;
    counter += 4;
    if (counter > total_block_length) { goto error; }

    unsigned char *comment = splt_flac_t_read_vector(ptr, comment_length);
    if (comment == NULL) { *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY; return; }

    splt_flac_t_parse_and_store_comment((char *)comment, flac_tags, i, error);
    if (*error < 0) { free(comment); return; }

    free(comment);
    ptr += comment_length;
    counter += comment_length;
    if (counter > total_block_length) { goto error; }
  }

  return;

error:
  *error = SPLT_ERROR_INVALID;
}

splt_flac_tags *splt_flac_t_new(unsigned char *comments, FLAC__uint32 total_block_length,
    splt_code *error) {
  splt_flac_tags *flac_tags = malloc(sizeof(splt_flac_tags));
  if (flac_tags == NULL) {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return NULL;
  }

  flac_tags->vendor_string = NULL;
  flac_tags->vendor_length = 0;
  flac_tags->original_tags = splt_tu_new_tags(error);
  if (*error < 0) { goto error; }

  splt_flac_t_parse_comments(comments, total_block_length, flac_tags, error);
  if (*error < 0) { goto error; }

  return flac_tags;

error:
  splt_flac_t_free(&flac_tags);
  return NULL;
}

void splt_flac_t_free(splt_flac_tags **flac_tags) {
  if (!flac_tags || !(*flac_tags))
  {
    return;
  }

  if ((*flac_tags)->vendor_string)
  {
    free((*flac_tags)->vendor_string);
    (*flac_tags)->vendor_string = NULL;
  }

  splt_flac_vorbis_tags *other_tags = (*flac_tags)->other_tags;
  splt_flac_vorbis_tags_free(&other_tags);

  splt_tu_free_one_tags(&((*flac_tags)->original_tags));

  free(*flac_tags);
  *flac_tags = NULL;
}

