/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2012 Alexandru Munteanu - <io_fx@yahoo.fr>
 *
 * Parts of this file have been copied from the 'vcut' 1.6
 * program provided with 'vorbis-tools' :
 *      vcut (c) 2000-2001 Michael Smith <msmith@xiph.org>
 *
 * Some parts from a more recent version of vcut :
 *           (c) 2008 Michael Gold <mgold@ncf.ca>
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
#include <math.h>

#include "ogg_utils.h"

static void splt_ogg_submit_headers_to_stream(ogg_stream_state *stream, splt_ogg_state *oggstate);
static char *splt_ogg_trackstring(int number, int *error);
static void splt_ogg_v_comment(splt_state *state, vorbis_comment *vc, char *artist,
    char *album, char *title, char *tracknum, char *date, char *genre, char *comment,
    int *error);
static void delete_all_non_null_tags(vorbis_comment *vc, 
    const char *artist, const char *album, const char *title,
    const char *tracknum, const char *date, const char *genre, 
    const char *comment, int *error);

long splt_ogg_get_blocksize(splt_ogg_state *oggstate, vorbis_info *vi, ogg_packet *op)
{
  //if this < 0, there is a problem
  int this = vorbis_packet_blocksize(vi, op);
  int ret = (this + oggstate->prevW)/4;

  oggstate->prevW = this;

  return ret;
}

ogg_int64_t splt_ogg_compute_first_granulepos(splt_state *state, splt_ogg_state *oggstate, ogg_packet *packet, int bs)
{
  ogg_int64_t first_granpos = 0;

  if (packet->granulepos >= 0)
  {
    /*fprintf(stdout,"oggstate->total_blocksize + bs = %ld\n",oggstate->total_blocksize + bs);
    fprintf(stdout,"packet granulepos = %ld\n",packet->granulepos);
    fflush(stdout);*/

    if ((packet->granulepos > oggstate->total_blocksize + bs) &&
        (oggstate->total_blocksize > 0) &&
        !packet->e_o_s &&
        (oggstate->first_granpos == 0))
    {
      first_granpos = packet->granulepos;
      oggstate->first_granpos = first_granpos;
      splt_c_put_info_message_to_client(state,
          _(" warning: unexpected position in ogg vorbis stream - split from 0.0 to EOF to fix.\n"));
    }

    oggstate->total_blocksize = packet->granulepos;
  }
  else if (oggstate->total_blocksize == -1)
  {
    oggstate->total_blocksize = 0;
  }
  else {
    oggstate->total_blocksize += bs;
    /*fprintf(stdout,"blocksize = %d, total = %ld\n", bs, oggstate->total_blocksize);
    fflush(stdout);*/
  }

  return first_granpos;
}

int splt_ogg_update_sync(splt_state *state, ogg_sync_state *sync_in, FILE *f, int *error)
{
  char *buffer = ogg_sync_buffer(sync_in, SPLT_OGG_BUFSIZE);
  if (!buffer)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return -1;
  }
  int bytes = fread(buffer,1,SPLT_OGG_BUFSIZE,f);

  if (ogg_sync_wrote(sync_in, bytes) != 0)
  {
    splt_e_set_error_data(state, splt_t_get_filename_to_split(state));
    *error = SPLT_ERROR_INVALID;
    return -1;
  }

  return bytes;
}

splt_v_packet *splt_ogg_clone_packet(ogg_packet *packet, int *error)
{
  splt_v_packet *p = NULL;

  p = malloc(sizeof(splt_v_packet));
  if (!p)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return p;
  }

  p->length = packet->bytes;
  p->packet = malloc(p->length);
  if (! p->packet)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    free(p);
    p = NULL;
    return p;
  }
  memcpy(p->packet, packet->packet, p->length);

  return p;
}

void splt_ogg_free_packet(splt_v_packet **p)
{
  if (!p || !*p)
  {
    return;
  }

  if ((*p)->packet)
  {
    free((*p)->packet);
    (*p)->packet = NULL;
  }

  free(*p);
  *p = NULL;
}

void splt_ogg_free_oggstate_headers(splt_ogg_state *oggstate)
{
  if (!oggstate->headers)
  {
    return;
  }

  int i = 0;
  for (i = 0;i < TOTAL_HEADER_PACKETS; i++)
  {
    splt_ogg_free_packet(&oggstate->headers[i]);
  }

  free(oggstate->headers);
  oggstate->headers = NULL;
}

/* Returns 0 for success, or -1 on failure. */
int splt_ogg_write_pages_to_file(splt_state *state, ogg_stream_state *stream,
    FILE *file, int flush, int *error, const char *output_fname)
{
  ogg_page page;

  if (flush)
  {
    while (ogg_stream_flush(stream, &page))
    {
      if (splt_io_fwrite(state, page.header, 1, page.header_len, file) < page.header_len)
      {
        goto write_error;
      }
      if (splt_io_fwrite(state, page.body, 1, page.body_len, file) < page.body_len)
      {
        goto write_error;
      }
    }
  }
  else
  {
    while (ogg_stream_pageout(stream, &page))
    {
      if (splt_io_fwrite(state, page.header,1,page.header_len, file) < page.header_len)
      {
        goto write_error;
      }
      if (splt_io_fwrite(state, page.body,1,page.body_len, file) < page.body_len)
      {
        goto write_error;
      }
    }
  }

  return 0;

write_error:
  splt_e_set_error_data(state, output_fname);
  *error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
  return -1;
}

int splt_ogg_write_header_packets(splt_state *state, splt_ogg_state *oggstate,
    ogg_stream_state *stream_out, const char *output_fname, int *error)
{
  splt_ogg_submit_headers_to_stream(stream_out, oggstate);
  return splt_ogg_write_pages_to_file(state, stream_out, oggstate->out, 1,
      error, output_fname);
}

void splt_ogg_set_tags_in_headers(splt_ogg_state *oggstate, int *error)
{
  ogg_packet header_comm;
  vorbis_commentheader_out(&oggstate->vc, &header_comm);
  splt_ogg_free_packet(&oggstate->headers[1]);
  oggstate->headers[1] = splt_ogg_clone_packet(&header_comm, error);
  ogg_packet_clear(&header_comm);

  splt_ogg_free_vorbis_comment(&oggstate->vc, oggstate->cloned_vorbis_comment);
  oggstate->cloned_vorbis_comment = 2;
}

vorbis_comment *splt_ogg_clone_vorbis_comment(vorbis_comment *comment)
{
  vorbis_comment *cloned_comment = malloc(sizeof(vorbis_comment));
  if (cloned_comment == NULL)
  {
    return NULL;
  }
  memset(cloned_comment, 0x0, sizeof(vorbis_comment));

  vorbis_comment_init(cloned_comment);

  int err = splt_su_copy(comment->vendor, &cloned_comment->vendor);
  if (err < 0)
  {
    free(cloned_comment);
    return NULL;
  }

  long number_of_comments = comment->comments; 
  cloned_comment->comments = number_of_comments;

  if (number_of_comments <= 0)
  {
    cloned_comment->comment_lengths = NULL;
    cloned_comment->user_comments = NULL;
    return cloned_comment;
  }

  cloned_comment->comment_lengths = malloc(sizeof(int) * number_of_comments);
  if (cloned_comment->comment_lengths == NULL)
  {
    splt_ogg_free_vorbis_comment(cloned_comment, SPLT_TRUE);
    free(cloned_comment);
    return NULL;
  }
  memset(cloned_comment->comment_lengths, 0x0, sizeof(int) * number_of_comments);

  cloned_comment->user_comments = malloc(sizeof(char *) * number_of_comments);
  if (cloned_comment->user_comments == NULL)
  {
    splt_ogg_free_vorbis_comment(cloned_comment, SPLT_TRUE);
    free(cloned_comment);
    return NULL;
  }
  memset(cloned_comment->user_comments, 0x0, sizeof(char *) * number_of_comments);

  int i = 0;
  for (i = 0;i < number_of_comments; i++)
  {
    int err = splt_su_copy(comment->user_comments[i], &cloned_comment->user_comments[i]);
    if (err < 0)
    {
      splt_ogg_free_vorbis_comment(cloned_comment, SPLT_TRUE);
      free(cloned_comment);
      return NULL;
    }
    cloned_comment->comment_lengths[i] = comment->comment_lengths[i];
  }

  return cloned_comment;
}

void splt_ogg_put_tags(splt_state *state, int *error)
{
  splt_d_print_debug(state,"Setting ogg tags ...\n");

  splt_ogg_state *oggstate = state->codec;

  splt_ogg_free_vorbis_comment(&oggstate->vc, oggstate->cloned_vorbis_comment);
  oggstate->cloned_vorbis_comment = 2;

  if (splt_o_get_int_option(state, SPLT_OPT_TAGS) == SPLT_NO_TAGS)
  {
    return;
  }

  splt_tags *tags = splt_tu_get_current_tags(state);
  if (!tags)
  {
    return;
  }

  char *track_string = splt_ogg_trackstring(tags->track, error);
  if (*error < 0) { return; }

  char *artist_or_performer = splt_tu_get_artist_or_performer_ptr(tags);

  vorbis_comment *original_vc = 
    (vorbis_comment *) splt_tu_get_original_tags_data(state);

  if (tags->set_original_tags && original_vc)
  {
    vorbis_comment *cloned_vc = splt_ogg_clone_vorbis_comment(original_vc);
    if (cloned_vc == NULL)
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      goto error;
    }

    oggstate->vc = *cloned_vc;
    free(cloned_vc);
    oggstate->cloned_vorbis_comment = SPLT_TRUE;
  }
  else {
    vorbis_comment_init(&oggstate->vc);
    oggstate->cloned_vorbis_comment = SPLT_FALSE;
  }

  splt_ogg_v_comment(state, &oggstate->vc,
      artist_or_performer, tags->album, tags->title, track_string,
      tags->year, tags->genre, tags->comment, error);

error:
  free(track_string);
  track_string = NULL;
}

void splt_ogg_free_vorbis_comment(vorbis_comment *vc, short cloned_vorbis_comment)
{
  if (!vc || cloned_vorbis_comment == 2)
  {
    return;
  }

  vorbis_comment *comment = vc;

  if (!cloned_vorbis_comment)
  {
    vorbis_comment_clear(comment);
    return;
  }

  long i = 0;
  for (i = 0;i < comment->comments; i++)
  {
    if (comment->user_comments[i])
    {
      free(comment->user_comments[i]);
      comment->user_comments[i] = NULL;
    }
  }

  if (comment->user_comments)
  {
    free(comment->user_comments);
    comment->user_comments = NULL;
  }

  if (comment->comment_lengths)
  {
    free(comment->comment_lengths);
    comment->comment_lengths = NULL;
  }

  if (comment->vendor)
  {
    free(comment->vendor);
    comment->vendor = NULL;
  }
}

//puts tags in vc
//what happens if 'vorbis_comment_add_tag(..)' fails ?
//- ask vorbis developers
static void splt_ogg_v_comment(splt_state *state, vorbis_comment *vc, char *artist,
    char *album, char *title, char *tracknum, char *date, char *genre, char *comment,
    int *error)
{
  if (splt_o_get_int_option(state, SPLT_OPT_TAGS) == SPLT_TAGS_ORIGINAL_FILE &&
      state->original_tags.tags.tags_version == 0)
  {
    return;
  }

  delete_all_non_null_tags(vc, artist, album, title, tracknum, date, genre, comment, error);

  if (title != NULL) { vorbis_comment_add_tag(vc, SPLT_OGG_TITLE, title); }
  if (artist != NULL) { vorbis_comment_add_tag(vc, SPLT_OGG_ARTIST, artist); }
  if (album != NULL) { vorbis_comment_add_tag(vc, SPLT_OGG_ALBUM, album); }
  if (date != NULL && strlen(date) > 0) { vorbis_comment_add_tag(vc, SPLT_OGG_DATE, date); }
  if (genre != NULL) { vorbis_comment_add_tag(vc, SPLT_OGG_GENRE, genre); }
  if (tracknum != NULL) { vorbis_comment_add_tag(vc, SPLT_OGG_TRACKNUMBER, tracknum); }
  if (comment != NULL) { vorbis_comment_add_tag(vc, SPLT_OGG_COMMENT, comment); }
}

static void add_tag_and_equal(const char *tag_name, const char *value, splt_array *to_delete, 
    int *error)
{
  if (value == NULL) { return; }

  size_t size = strlen(tag_name) + 2;
  char *tag_and_equal = malloc(size);
  if (tag_and_equal == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return;
  }
  snprintf(tag_and_equal, size, "%s=", tag_name);

  int err = splt_array_append(to_delete, tag_and_equal);
  if (err == -1)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  }
}

static splt_array *build_tag_and_equal_to_delete(const char *artist, 
    const char *album, const char *title, const char *tracknum,
    const char *date, const char *genre, const char *comment, int *error)
{
  splt_array *to_delete = splt_array_new();

  add_tag_and_equal(SPLT_OGG_TITLE, title, to_delete, error);
  if (*error < 0) { goto error; }
  add_tag_and_equal(SPLT_OGG_ARTIST, artist, to_delete, error);
  if (*error < 0) { goto error; }
  add_tag_and_equal(SPLT_OGG_ALBUM, album, to_delete, error);
  if (*error < 0) { goto error; }
  if (date != NULL && strlen(date) > 0) { add_tag_and_equal(SPLT_OGG_DATE, date, to_delete, error); }
  if (*error < 0) { goto error; }
  add_tag_and_equal(SPLT_OGG_GENRE, genre, to_delete, error);
  if (*error < 0) { goto error; }
  add_tag_and_equal(SPLT_OGG_TRACKNUMBER, tracknum, to_delete, error);
  if (*error < 0) { goto error; }
  add_tag_and_equal(SPLT_OGG_COMMENT, comment, to_delete, error);
  if (*error < 0) { goto error; }

  return to_delete;

error:
  splt_array_free(&to_delete);
  return NULL;
}

static void delete_all_non_null_tags(vorbis_comment *vc, 
    const char *artist, const char *album, const char *title,
    const char *tracknum, const char *date, const char *genre, 
    const char *comment, int *error)
{
  char *vendor_backup = NULL;
  splt_array *tag_and_equal_to_delete = NULL;
  splt_array *comments = NULL;
  long i = 0, j = 0;

  tag_and_equal_to_delete = 
    build_tag_and_equal_to_delete(artist, album, title, tracknum, date, genre, comment, error);
  if (*error < 0) { return; }

  comments = splt_array_new();
  if (comments == NULL) { goto end; }

  for (i = 0;i < vc->comments; i++)
  {
    short keep_comment = SPLT_TRUE;

    long number_of_tags_to_delete = splt_array_get_number_of_elements(tag_and_equal_to_delete);
    for (j = 0;j < number_of_tags_to_delete; j++)
    {
      char *tag_and_equal = (char *) splt_array_get(tag_and_equal_to_delete, j);

      if (strncasecmp(vc->user_comments[i], 
            tag_and_equal, strlen(tag_and_equal)) == 0)
      {
        keep_comment = SPLT_FALSE;
        break;
      }
    }

    if (keep_comment)
    {
      char *user_comment = NULL;
      int err = splt_su_append(&user_comment, 
          vc->user_comments[i], vc->comment_lengths[i], NULL);
      if (err <  0) { *error = err; goto end; }
      err = splt_array_append(comments, user_comment);
      if (err == -1) { *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY; goto end; }
    }
  }

  int err = splt_su_copy(vc->vendor, &vendor_backup);
  if (err < 0) { *error = err; goto end; }

  vorbis_comment_clear(vc);

  long tags_to_keep = splt_array_get_number_of_elements(comments);
  for (i = 0;i < tags_to_keep; i++)
  {
    char *user_comment = splt_array_get(comments, i);
    vorbis_comment_add(vc, user_comment);
    free(user_comment);
    user_comment = NULL;
  }

  if (vendor_backup)
  {
    splt_su_set(&vc->vendor, vendor_backup, strlen(vendor_backup), NULL);
  }

end:
  if (vendor_backup)
  {
    free(vendor_backup);
    vendor_backup = NULL;
  }

  splt_array_free(&comments);

  long number_of_tags_to_delete = splt_array_get_number_of_elements(tag_and_equal_to_delete);
  for (j = 0;j < number_of_tags_to_delete; j++)
  {
    char *tag_and_equal = (char *) splt_array_get(tag_and_equal_to_delete, j);
    if (tag_and_equal) { free(tag_and_equal); }
  }
  splt_array_free(&tag_and_equal_to_delete);
}

static void splt_ogg_submit_headers_to_stream(ogg_stream_state *stream, 
    splt_ogg_state *oggstate)
{
  int i;
  for(i = 0;i < TOTAL_HEADER_PACKETS;i++)
  {
    ogg_packet p;
    p.bytes = oggstate->headers[i]->length;
    p.packet = oggstate->headers[i]->packet;
    p.b_o_s = ((i==0)?1:0);
    p.e_o_s = 0;
    p.granulepos=0;

    ogg_stream_packetin(stream, &p);
  }
}

static char *splt_ogg_trackstring(int number, int *error)
{
  char *track = NULL;

  if (number > 0)
  {
    int len = 0, i;
    len = ((int) (log10((double) (number)))) + 1;

    if ((track = malloc(len + 1))==NULL)
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      return NULL;
    }
    memset(track, 0, len + 1);
    for (i=len-1; i >= 0; i--)
    {
      track[i] = ((number%10) | 0x30);
      number /= 10;
    }
  }

  return track;
}

