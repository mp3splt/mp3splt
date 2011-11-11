/**********************************************************
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2011 Alexandru Munteanu - io_fx@yahoo.fr
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
 * 02111-1307,
 * USA.
 *********************************************************/
/*! \file

The Plug-in that handles mp3 files
*/

#include "splt.h"

#include "mp3.h"
#include "mp3_silence.h"
#include "mp3_utils.h"

/****************************/
/* mp3 constants */

/*! The names of all mono/stereo modes supported by mp3 

\todo translation
*/
static const char *splt_mp3_chan[] =
{
	"Mono",
	"Dual Mono",
	"Joint Stereo",
	"Stereo",
	"?"
};

/*! A table needed for fast crc32 computation

 */
static const unsigned long splt_mp3_crctab[256] = {
  0x00000000, 0x77073096, 0xee0e612c, 0x990951ba,
  0x076dc419, 0x706af48f, 0xe963a535, 0x9e6495a3,
  0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,
  0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91,
  0x1db71064, 0x6ab020f2, 0xf3b97148, 0x84be41de,
  0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,
  0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec,
  0x14015c4f, 0x63066cd9, 0xfa0f3d63, 0x8d080df5,
  0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,
  0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b,
  0x35b5a8fa, 0x42b2986c, 0xdbbbc9d6, 0xacbcf940,
  0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
  0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116,
  0x21b4f4b5, 0x56b3c423, 0xcfba9599, 0xb8bda50f,
  0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,
  0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d,
  0x76dc4190, 0x01db7106, 0x98d220bc, 0xefd5102a,
  0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,
  0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818,
  0x7f6a0dbb, 0x086d3d2d, 0x91646c97, 0xe6635c01,
  0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,
  0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457,
  0x65b0d9c6, 0x12b7e950, 0x8bbeb8ea, 0xfcb9887c,
  0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
  0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2,
  0x4adfa541, 0x3dd895d7, 0xa4d1c46d, 0xd3d6f4fb,
  0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,
  0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9,
  0x5005713c, 0x270241aa, 0xbe0b1010, 0xc90c2086,
  0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,
  0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4,
  0x59b33d17, 0x2eb40d81, 0xb7bd5c3b, 0xc0ba6cad,
  0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,
  0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683,
  0xe3630b12, 0x94643b84, 0x0d6d6a3e, 0x7a6a5aa8,
  0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
  0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe,
  0xf762575d, 0x806567cb, 0x196c3671, 0x6e6b06e7,
  0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,
  0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5,
  0xd6d6a3e8, 0xa1d1937e, 0x38d8c2c4, 0x4fdff252,
  0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,
  0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60,
  0xdf60efc3, 0xa867df55, 0x316e8eef, 0x4669be79,
  0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,
  0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f,
  0xc5ba3bbe, 0xb2bd0b28, 0x2bb45a92, 0x5cb36a04,
  0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
  0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a,
  0x9c0906a9, 0xeb0e363f, 0x72076785, 0x05005713,
  0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,
  0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21,
  0x86d3d2d4, 0xf1d4e242, 0x68ddb3f8, 0x1fda836e,
  0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,
  0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c,
  0x8f659eff, 0xf862ae69, 0x616bffd3, 0x166ccf45,
  0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,
  0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db,
  0xaed16a4a, 0xd9d65adc, 0x40df0b66, 0x37d83bf0,
  0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
  0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6,
  0xbad03605, 0xcdd70693, 0x54de5729, 0x23d967bf,
  0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,
  0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d,
};

static void splt_mp3_save_end_point(splt_state *state, splt_mp3_state *mp3state,
    int save_end_point, off_t end)
{
  mp3state->end_non_zero = end;

  if (save_end_point)
  {
    mp3state->end = end;
  }
  else
  {
    mp3state->end = 0;
  }
}

/*! Open a mp3 file for reading

\attention filename must not be null.
But it actually should never need to: If filename is NULL, then this
plugin should not have been detected
\return NULL on error
*/
static FILE *splt_mp3_open_file_read(splt_state *state, const char *filename,
    int *error)
{
  FILE *file_input = NULL;

  if (filename != NULL && ((strcmp(filename,"-") == 0) ||
        (strcmp(filename,"m-") == 0)))
  {
    file_input = stdin;
#ifdef __WIN32__
    _setmode(fileno(file_input), _O_BINARY);
#endif
  }
  else
  {
    file_input = splt_io_fopen(filename, "rb");
    if (file_input == NULL)
    {
      splt_e_set_strerror_msg_with_data(state, filename);
      *error = SPLT_ERROR_CANNOT_OPEN_FILE;
    }
  }

  return file_input;
}

/*! Open a mp3 file for writing

 */
static FILE *splt_mp3_open_file_write(splt_state *state, const char *output_fname, int *error)
{
  FILE *file_output = NULL;

  //for the stdout
  if (strcmp(output_fname, "-")==0)
  {
    file_output = stdout;
#ifdef __WIN32__
    _setmode(fileno(file_output), _O_BINARY);
#endif
  }
  else
  {
    if (!(file_output = splt_io_fopen(output_fname, "wb+")))
    {
      splt_e_set_strerror_msg_with_data(state, output_fname);
      *error = SPLT_ERROR_CANNOT_OPEN_DEST_FILE;
    }
  }

  return file_output;
}

/****************************/
/* CRC functions */

//! Calculate the CRC of an mp3 file
static unsigned long splt_mp3_c_crc(splt_state *state,
    FILE *in, off_t begin, off_t end, int *error)
{
  register unsigned long crc;
  int c;

  crc = 0xFFFFFFFF;

  if (fseeko(in, begin, SEEK_SET) == -1)
  {
    splt_e_set_strerror_msg_with_data(state, splt_t_get_filename_to_split(state));
    *error = SPLT_ERROR_SEEKING_FILE;
    return 0;
  }

  while(begin++ < end)
  {
    c = fgetc(in);
    crc = ((crc >> 8) & 0x00FFFFFF) ^ splt_mp3_crctab[(crc ^ c) & 0xFF];
  }

  return (crc ^ 0xFFFFFFFF);
}

/*! frees the splt_mp3_state structure

used by the splt_t_state_free() function
*/
static void splt_mp3_state_free(splt_state *state)
{
  splt_mp3_state *mp3state = state->codec;

  if (mp3state)
  {
    if (mp3state->mp3file.xingbuffer)
    {
      free(mp3state->mp3file.xingbuffer);
      mp3state->mp3file.xingbuffer = NULL;
    }

    //we free the state
    free(mp3state);
    state->codec = NULL;
  }
}

/****************************/
/* mp3 tags */

/*!search for ID3 v1 tag, "TAG" sequence

we don't check fseeko error

\return 
 - if ID3 v1 Tag is found the offset of mp3 data
 - else 0
*/
static int splt_mp3_getid3v1_offset(FILE *file_input)
{
  if (fseeko(file_input, (off_t) -128, SEEK_END)==-1)
  {
    return 0;
  }

  if (fgetc(file_input)=='T')
    if (fgetc(file_input)=='A')
      if (fgetc(file_input)=='G')
        return -128;

  return 0;
}

/*!search for ID3 v2 tag sequence

we don't check fseeko error

\return 
 - if ID3 v1 Tag is found the offset of mp3 data
 - else 0
*/
static off_t splt_mp3_getid3v2_end_offset(FILE *in, off_t start)
{
  unsigned long oword = 0;
  if (fseeko(in, start, SEEK_SET)==-1)
  {
    return 0;
  }

  if (fgetc(in)=='I')
    if (fgetc(in)=='D')
      if (fgetc(in)=='3')
      {
        int i;
        if (fseeko(in, (off_t) 3, SEEK_CUR)==-1)
        {
          return 0;
        }

        for (i=0; i<4; i++)
        {
          oword = (oword << 7) | fgetc(in);
        }

        return (off_t) (oword);
      }

  return 0;
}


#ifndef NO_ID3TAG

/*! get libid3tag original tags */
static id3_byte_t *splt_mp3_get_id3v2_tag_bytes(FILE *file, id3_length_t *length)
{
  id3_byte_t *bytes = NULL;
  *length = 0;

  off_t id3v2_end_offset = splt_mp3_getid3v2_end_offset(file, 0);

  if (id3v2_end_offset != 0)
  {
    size_t id3v2_size = (size_t) (id3v2_end_offset + 10);

    rewind(file);
    bytes = splt_io_fread(file, 1, id3v2_size);

    if (! bytes)
    {
      return NULL;
    }

    *length = (unsigned long) id3v2_size;
  }

  return bytes;
}

static id3_byte_t *splt_mp3_get_id3v1_tag_bytes(FILE *file, id3_length_t *length)
{
  id3_byte_t *bytes = NULL;
  *length = 0;

  off_t id3v1_offset = splt_mp3_getid3v1_offset(file);

  if (id3v1_offset != 0)
  {
    if (fseeko(file, id3v1_offset, SEEK_END) !=-1)
    {
      bytes = malloc(sizeof(unsigned char) * 128);

      if (! bytes)
      {
        return NULL;
      }

      if (fread(bytes, 1 , 128, file) != 128)
      {
        if (bytes)
        {
          free(bytes);
          bytes = NULL;
          return NULL;
        }
      }
      else
      {
        *length = (unsigned long) 128; 
      }
    }
  }

  return bytes;
}

/*! returns the tag bytes from a file

\param filename the name of the file to search for tags
\param state The central structure libmp3splt keeps all of its data in
\param error Contains the error number if an error has occoured
\param Is filled with the length of the id3 structure
\param tags_version Is filled with the version of the tag by this function.
\return The string containing the tags
*/
static id3_byte_t *splt_mp3_get_id3_tag_bytes(splt_state *state, const char *filename,
    id3_length_t *length, int *error, int *tags_version)
{
  *length = 0;
  id3_byte_t *bytes = NULL;

  FILE *file = splt_io_fopen(filename, "rb");

  if (! file)
  {
    splt_e_set_strerror_msg_with_data(state, filename);
    *error = SPLT_ERROR_CANNOT_OPEN_FILE;
    goto end;
  }
  else
  {
    id3_length_t id3v1_length = 0;
    id3_byte_t *id3v1_bytes = splt_mp3_get_id3v1_tag_bytes(file, &id3v1_length);

    id3_length_t id3v2_length = 0;
    id3_byte_t *id3v2_bytes = splt_mp3_get_id3v2_tag_bytes(file, &id3v2_length);

    if (id3v2_bytes)
    {
      *tags_version = 2;
      bytes = id3v2_bytes;
      *length = id3v2_length;

      if (id3v1_bytes)
      {
        *tags_version = 12;
        free(id3v1_bytes);
        id3v1_bytes = NULL;
      }
    }
    else if (id3v1_bytes)
    {
      *tags_version = 1;
      bytes = id3v1_bytes;
      *length = id3v1_length;
    }
  }

end:
  if (file)
  {
    if (fclose(file) != 0)
    {
      if (bytes)
      {
        free(bytes);
        bytes = NULL;
      }
      return NULL;
    }
  }

  return bytes;
}

//!puts a original field on id3 conforming to frame_type
static int splt_mp3_put_original_libid3_frame(splt_state *state,
    const struct id3_tag *id3tag, const char *frame_type, int id_type)
{
  struct id3_frame *frame = NULL;
  union id3_field *field = NULL;
  id3_ucs4_t *ucs4 = NULL;
  id3_utf8_t *tag_value = NULL;

  int err = SPLT_OK;

  frame = id3_tag_findframe(id3tag, frame_type,0);
  if (frame != NULL)
  {
    if (id_type == SPLT_MP3_ID3_COMMENT)
    {
      field = id3_frame_field(frame, 3);
      ucs4 = (id3_ucs4_t *) id3_field_getfullstring(field);
    }
    else
    {
      field = id3_frame_field(frame, 1);
      ucs4 = (id3_ucs4_t *) id3_field_getstrings(field,0);
    }
    if (ucs4 != NULL)
    {
      tag_value = id3_ucs4_utf8duplicate(ucs4);

      if (tag_value != NULL)
      {
        switch (id_type)
        {
          case SPLT_MP3_ID3_ALBUM:
            err = splt_tu_set_original_tags_field(state,SPLT_TAGS_ALBUM, tag_value);
            break;
          case SPLT_MP3_ID3_ARTIST:
            err = splt_tu_set_original_tags_field(state,SPLT_TAGS_ARTIST, tag_value);
            break;
          case SPLT_MP3_ID3_TITLE:
            if (strcmp(frame_type,ID3_FRAME_TITLE) == 0)
            {
              err = splt_tu_set_original_tags_field(state,SPLT_TAGS_TITLE, tag_value);
            }
            break;
          case SPLT_MP3_ID3_YEAR:
            err = splt_tu_set_original_tags_field(state,SPLT_TAGS_YEAR, tag_value);
            break;
          case SPLT_MP3_ID3_TRACK:
            ;
            int track = atoi((char *)tag_value);
            err = splt_tu_set_original_tags_field(state,SPLT_TAGS_TRACK, &track);
            break;
          case SPLT_MP3_ID3_COMMENT:
            err = splt_tu_set_original_tags_field(state,SPLT_TAGS_COMMENT, tag_value);
            break;
          case SPLT_MP3_ID3_GENRE:
            ;
            char *genre = (char *)tag_value;

            int id3v1 = atoi(genre);
            if ((id3v1 != 0) &&
                (id3v1 < SPLT_ID3V1_NUMBER_OF_GENRES) &&
                (state->original_tags.tags.genre == NULL))
            {
              err = splt_tu_set_original_tags_field(state, SPLT_TAGS_GENRE, splt_id3v1_genres[id3v1]);
            }
            else if (strcmp(genre, "0") == 0)
            {
              err = splt_tu_set_original_tags_field(state, SPLT_TAGS_GENRE, SPLT_UNDEFINED_GENRE);
            }
            else
            {
              err = splt_tu_set_original_tags_field(state, SPLT_TAGS_GENRE, genre);
            }
            break;
          default:
            break;
        }
        free(tag_value);
        tag_value = NULL;
      }
      else
      {
        err = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      }
    }
  }

  return err;
}

//!macro used only in the following function splt_mp3_get_original_tags
#define MP3_VERIFY_ERROR() \
if (err != SPLT_OK) \
{ \
*tag_error = err; \
goto end; \
};

/*! this function reads the original id3 tags 

This function only does do anything if we have libid3tag enabled
at compilation time
*/
static void splt_mp3_get_original_tags(const char *filename,
    splt_state *state, int *tag_error)
{
  int err = SPLT_OK;

  //we get the id3 from the original file using libid3tag
  struct id3_tag *id3tag = NULL;

  //get out the tags from the file; id3_file_open doesn't work with win32 utf16 filenames
  id3_length_t id3_tag_length = 0;
  int tags_version = 0;
  id3_byte_t *id3_tag_bytes =
    splt_mp3_get_id3_tag_bytes(state, filename, &id3_tag_length, tag_error,
        &tags_version);

  if (*tag_error >= 0)
  {
    if (id3_tag_bytes)
    {
      id3tag = id3_tag_parse(id3_tag_bytes, id3_tag_length);

      if (id3tag)
      {
        err = splt_tu_set_original_tags_field(state,SPLT_TAGS_VERSION, &tags_version);
        MP3_VERIFY_ERROR();
        err = splt_mp3_put_original_libid3_frame(state,id3tag,ID3_FRAME_ARTIST,
            SPLT_MP3_ID3_ARTIST);
        MP3_VERIFY_ERROR();
        err = splt_mp3_put_original_libid3_frame(state,id3tag,ID3_FRAME_ALBUM,
            SPLT_MP3_ID3_ALBUM);
        MP3_VERIFY_ERROR();
        err = splt_mp3_put_original_libid3_frame(state,id3tag,ID3_FRAME_TITLE,
            SPLT_MP3_ID3_TITLE);
        MP3_VERIFY_ERROR();
        err = splt_mp3_put_original_libid3_frame(state,id3tag,ID3_FRAME_YEAR,
            SPLT_MP3_ID3_YEAR);
        MP3_VERIFY_ERROR();
        err = splt_mp3_put_original_libid3_frame(state,id3tag,ID3_FRAME_GENRE,
            SPLT_MP3_ID3_GENRE);
        MP3_VERIFY_ERROR();
        err = splt_mp3_put_original_libid3_frame(state,id3tag,ID3_FRAME_COMMENT,
            SPLT_MP3_ID3_COMMENT);
        MP3_VERIFY_ERROR();
        err = splt_mp3_put_original_libid3_frame(state,id3tag,ID3_FRAME_TRACK,
            SPLT_MP3_ID3_TRACK);
        MP3_VERIFY_ERROR();

        id3_tag_delete(id3tag);
      }

      tag_bytes_and_size *bytes_and_size = malloc(sizeof(tag_bytes_and_size));
      if (bytes_and_size == NULL)
      {
        err = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
        MP3_VERIFY_ERROR();
      }

      bytes_and_size->tag_bytes = id3_tag_bytes;
      bytes_and_size->tag_length = id3_tag_length;

      splt_tu_set_original_tags_data(state, bytes_and_size);
    }

end: 
    ;
  }
  else if (id3_tag_bytes)
  {
    free(id3_tag_bytes);
    id3_tag_bytes = NULL;
  }
}

/*! build ID3 tags */

static void splt_mp3_delete_existing_frames(struct id3_tag *id, const char *frame_type)
{
  struct id3_frame *frame = NULL;
  while ((frame = id3_tag_findframe(id, frame_type, 0)))
  {
    id3_tag_detachframe(id, frame);
    id3_frame_delete(frame);
  }
}

static void splt_mp3_put_libid3_frame_in_tag_with_content(struct id3_tag *id,
    const char *frame_type, int field_number, const char *content, int *error)
{
  struct id3_frame *id_frame = NULL;
  id3_ucs4_t *field_content = NULL;
  union id3_field *id_field = NULL;

  if (content)
  {
    splt_mp3_delete_existing_frames(id, frame_type);

    id_frame = id3_frame_new(frame_type);
    if (!id_frame)
    {
      goto error;
    }

    id_field = id3_frame_field(id_frame, field_number);

    id3_field_settextencoding(id3_frame_field(id_frame, 0),
        ID3_FIELD_TEXTENCODING_UTF_16);

    field_content = id3_utf8_ucs4duplicate((signed char *)content);
    if (! field_content)
    {
      goto error;
    }

    //1 is usually a string list
    if (field_number == 1)
    {
      if (id3_field_addstring(id_field, field_content) == -1)
      {
        goto error;
      }
    }
    //the comment is a full string: field number 3
    else if (field_number == 3)
    {
      if (id3_field_setfullstring(id_field, field_content) == -1)
      {
        goto error;
      }
    }

    free(field_content);
    field_content = NULL;

    if (id3_tag_attachframe(id, id_frame) == -1)
    {
      goto error;
    }

    id3_frame_delete(id_frame);
  }

  return;

error:
  *error = SPLT_ERROR_LIBID3;
  if (id_frame)
  {
    id3_frame_delete(id_frame);
  }
  if (field_content)
  {
    free(field_content);
    field_content = NULL;
  }

  return;
}

static char *splt_mp3_build_libid3tag(const char *title, const char *artist,
    const char *album, const char *year, const char *genre, 
    const char *comment, int track, int set_original_tags, 
    int *error, unsigned long *number_of_bytes, int tags_version,
    splt_state *state)
{
  struct id3_tag *id = NULL;

  tag_bytes_and_size *bytes_and_size = 
    (tag_bytes_and_size *) splt_tu_get_original_tags_data(state);

  if (set_original_tags && bytes_and_size)
  {
    id = id3_tag_parse(bytes_and_size->tag_bytes, bytes_and_size->tag_length);
  }
  else
  {
    id = id3_tag_new();
  }

  id3_byte_t *bytes = NULL;
  id3_length_t bytes_length = 0;

  if (tags_version == 1)
  {
    id3_tag_options(id, ID3_TAG_OPTION_ID3V1, ID3_TAG_OPTION_ID3V1);
  }
  else
  {
    //turn off CRC and COMPRESSION; many players don't support that ?,
    //resulting in No tags (oh !)
    id3_tag_options(id, ID3_TAG_OPTION_CRC, 0);
    id3_tag_options(id, ID3_TAG_OPTION_COMPRESSION, 0);
  }

  splt_mp3_put_libid3_frame_in_tag_with_content(id, ID3_FRAME_TITLE, 1, title, error);
  if (*error < 0) { goto error; }
  splt_mp3_put_libid3_frame_in_tag_with_content(id, ID3_FRAME_ARTIST, 1, artist, error);
  if (*error < 0) { goto error; }
  splt_mp3_put_libid3_frame_in_tag_with_content(id, ID3_FRAME_ALBUM, 1, album, error);
  if (*error < 0) { goto error; }
  splt_mp3_put_libid3_frame_in_tag_with_content(id, ID3_FRAME_YEAR, 1, year, error);
  if (*error < 0) { goto error; }
  splt_mp3_put_libid3_frame_in_tag_with_content(id, ID3_FRAME_COMMENT, 3, comment, error);
  if (*error < 0) { goto error; }
  if (track != -1)
  {
    char track_str[255] = { '\0' };
    snprintf(track_str,254,"%d",track);
    splt_mp3_put_libid3_frame_in_tag_with_content(id, ID3_FRAME_TRACK, 1,
        track_str, error);
    if (*error < 0) { goto error; }
  }

  splt_mp3_put_libid3_frame_in_tag_with_content(id, ID3_FRAME_GENRE, 1, genre, error);
  if (*error < 0) { goto error; }

  //get the number of bytes needed for the tags
  bytes_length = id3_tag_render(id, NULL);

  if (bytes_length > 0)
  {
    //allocate memory for the tags
    bytes = malloc(sizeof(id3_byte_t) * bytes_length);
    if (!bytes)
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      goto error;
    }
    memset(bytes, '\0', sizeof(id3_byte_t) * bytes_length);

    bytes_length = id3_tag_render(id, bytes);

    *number_of_bytes = (unsigned long) bytes_length;
  }

  id3_tag_delete(id);

  return (char *) bytes;

error:
  id3_tag_delete(id);
  *number_of_bytes = 0;
  if (bytes)
  {
    free(bytes);
    bytes = NULL;
  }

  return NULL;
}

#else

//! A list of genre numbers
static const char unsigned splt_mp3_id3v1_genre_mapping[SPLT_ID3V1_NUMBER_OF_GENRES] = 
{ 0x00,

  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A,
  0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13, 0x14,
  0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 
  0x1F, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 
  0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30, 0x31, 0x32,
  0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 
  0x3D, 0x3E, 0x3F, 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46,
  0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,

  0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
  0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F, 0x60, 0x61, 0x62, 0x63,
  0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D,
  0x6E, 0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77,
  0x78, 0x79, 0x7A, 0x7B, 0x7C, 0x7D,

  0xFF };

//!returns the genre of the song, mp3splt used this in cddb search
static unsigned char splt_mp3_get_id3v1_mapping(const char *genre_string)
{
  if (genre_string == NULL)
  {
    return 0xFF;
  }

  int i = 0;
  for (i = 0; i < SPLT_ID3V1_NUMBER_OF_GENRES; i++)
  {
    if (strncmp(genre_string, splt_id3v1_genres[i], strlen(genre_string)) == 0)
    {
      return splt_mp3_id3v1_genre_mapping[i];
    }
  }

  return 0xFF;
}

/*! returns a id3v1 buffer as string

\attention The string this function returns is malloc()'ed and must be
freed by the caller after use.
\return The string or NULL on error
*/
static char *splt_mp3_build_simple_id3v1(const char *title, const char *artist,
    const char *album, const char *year, const char *genre, 
    const char *comment, int track, int *error, unsigned long *number_of_bytes)
{
  char *id = NULL;
  char buffer[30] = { '\0' };
  int j = 3,i = 0;

  if ((id = malloc(sizeof(char) * 128)) != NULL)
  {
    memset(id,'\0',128);

    strncpy(id, SPLT_MP3_TAG, 4);

    memset(buffer, '\0', 30);
    if (title!=NULL) strncpy(buffer, title, 30);
    for (i=0; i<30; i++) id[j++]=buffer[i];

    memset(buffer, '\0', 30);
    if (artist!=NULL) strncpy(buffer, artist, 30);
    for (i=0; i<30; i++) id[j++]=buffer[i];

    memset(buffer, '\0', 30);
    if (album!=NULL) strncpy(buffer, album, 30);
    for (i=0; i<30; i++) id[j++]=buffer[i];

    memset(buffer, '\0', 30);
    if (year!=NULL) strncpy(buffer, year, 4);
    for (i=0; i<4; i++) id[j++]=buffer[i];

    memset(buffer, '\0', 30);
    if (comment!=NULL) strncpy(buffer, comment, 30);
    for (i=0; i<30; i++) 
    {
      id[j++]=buffer[i];
    }
    //if we have a positive track
    if (track != -1)
    {
      if (track != 0x00)
      {
        id[j-1] = (char) track;
      }
    }
    id[j] = (char) splt_mp3_get_id3v1_mapping(genre);
  }
  else
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return NULL;
  }

  *number_of_bytes = 128;

  return id;
}
#endif

/*! returns a id3v1 or id3v2 buffer as string

\attention The string this function returns is malloc()'ed and must be
freed by the caller after use.
\return The string or NULL on error
*/
static char *splt_mp3_build_id3_tags(splt_state *state,
    const char *title, const char *artist,
    const char *album, const char *year, const char *genre,
    const char *comment, int track, int set_original_tags,
    int *error, unsigned long *number_of_bytes, int version)
{
  char *id = NULL;

#ifdef NO_ID3TAG
  if (version == 1)
  {
    splt_d_print_debug(state,"Setting ID3v1 tags without libid3tag\n");
    id = splt_mp3_build_simple_id3v1(title, artist, album, year, genre, comment, track,
        error, number_of_bytes);
  }
#else
  if (version == 1)
  {
    splt_d_print_debug(state,"Setting ID3v1 tags with libid3tag\n");
    id = splt_mp3_build_libid3tag(title, artist, album, year, genre, comment, track,
        set_original_tags, error, number_of_bytes, 1, state);
  }
  else
  {
    splt_d_print_debug(state,"Setting ID3v2 tags with libid3tag\n");
    id = splt_mp3_build_libid3tag(title, artist, album, year, genre, comment, track,
        set_original_tags, error, number_of_bytes, 2, state);
  }
#endif

  return id;
}

/*! put the song tags

\return a buffer containing the tags
\attention The string this function returns is malloc()'ed and must be
freed by the caller after use.
*/
static char *splt_mp3_build_tags(const char *filename, splt_state *state, int *error,
    unsigned long *number_of_bytes, int id3_version)
{
  char *id3_data = NULL;

  if (splt_o_get_int_option(state,SPLT_OPT_TAGS) != SPLT_NO_TAGS)
  {
    splt_tags *tags = splt_tu_get_current_tags(state);

    if (tags)
    {
      char *artist_or_performer = splt_tu_get_artist_or_performer_ptr(tags);
      id3_data = splt_mp3_build_id3_tags(state,
          tags->title, artist_or_performer, tags->album,
          tags->year, tags->genre, tags->comment,
          tags->track, tags->set_original_tags ,error, number_of_bytes, id3_version);
    }
  }

  return id3_data;
}

//!writes id3v1 tags to 'file_output'
int splt_mp3_write_id3v1_tags(splt_state *state, FILE *file_output,
    const char *output_fname)
{
  const char *filename = splt_t_get_filename_to_split(state);
  unsigned long number_of_bytes = 0;
  int error = SPLT_OK;

  char *id3_tags = splt_mp3_build_tags(filename, state, &error, &number_of_bytes, 1);

  if ((error >= 0) && (id3_tags) && (number_of_bytes > 0))
  {
    if (file_output)
    {
      if (fseeko(file_output, splt_mp3_getid3v1_offset(file_output), SEEK_END)!=-1)
      {
        if (splt_io_fwrite(state, id3_tags, 1, number_of_bytes, file_output) < number_of_bytes)
        {
          splt_e_set_error_data(state, output_fname);
          error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
        }
      }
      else
      {
        splt_e_set_strerror_msg_with_data(state, output_fname);
        error = SPLT_ERROR_SEEKING_FILE;
      }
    }
  }

  if (id3_tags)
  {
    free(id3_tags);
    id3_tags = NULL;
  }

  return error;
}

#ifndef NO_ID3TAG
int splt_mp3_write_id3v2_tags(splt_state *state, FILE *file_output,
    const char *output_fname, off_t *end_offset)
{
  const char *filename = splt_t_get_filename_to_split(state);
  unsigned long number_of_bytes = 0;
  int error = SPLT_OK;

  char *id3_tags = splt_mp3_build_tags(filename, state, &error, &number_of_bytes, 2);

  if ((error >= 0) && (id3_tags) && (number_of_bytes > 0))
  {
    if (splt_io_fwrite(state, id3_tags, 1, number_of_bytes, file_output) < number_of_bytes)
    {
      splt_e_set_error_data(state, output_fname);
      error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
    }
    else
    {
      if (end_offset != NULL)
      {
        *end_offset = number_of_bytes;
      }
    }
  }

  if (id3_tags)
  {
    free(id3_tags);
    id3_tags = NULL;
  }

  return error;
}
#endif

//!returns the output tags version
int splt_mp3_get_output_tags_version(splt_state *state)
{
#ifdef NO_ID3TAG
  splt_d_print_debug(state,"Output tags version is ID3v1 without libid3tag\n");
  return 1;
#else
  int original_tags_version = state->original_tags.tags.tags_version;
  int force_tags_version = splt_o_get_int_option(state, SPLT_OPT_FORCE_TAGS_VERSION);

  int output_tags_version = original_tags_version;
  if (force_tags_version != 0)
  {
    output_tags_version = force_tags_version;
  }

  if ((output_tags_version == 0) &&
      (splt_o_get_int_option(state, SPLT_OPT_TAGS) == SPLT_CURRENT_TAGS))
  {
    char *filename = splt_t_get_filename_to_split(state);
    if (strcmp(filename, "-") != 0)
    {
      output_tags_version = 12;
    }
  }

  splt_d_print_debug(state,"Output tags version is ID3v _%d_\n", output_tags_version);

  return output_tags_version;
#endif
}

/****************************/
/* mp3 infos */

/*! Reads all information about a mp3 file

The information is put into the "state" structure.

 - This function must be called before splt_mp3_split()
 - enables framemode if xing header is found 
 - xing header is often associated with VBR (variable bit rate)
*/
static splt_mp3_state *splt_mp3_info(FILE *file_input, splt_state *state,
    int framemode, int *error)
{
  splt_mp3_state *mp3state = state->codec;

  int prev = -1;
  long len;

  if ((mp3state = malloc(sizeof(splt_mp3_state)))==NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return NULL;
  }
  memset(mp3state, 0x0, sizeof(splt_mp3_state));

  char *filename = splt_t_get_filename_to_split(state);

  //always quiet
  mp3state->syncdetect = 0;

  //we initialise default values
  mp3state->frames = 1;
  mp3state->end = 0;
  mp3state->end_non_zero = 0;
  mp3state->first = 1;
  mp3state->file_input = file_input;
  mp3state->framemode = framemode;
  mp3state->headw = 0;
  mp3state->mp3file.xing = 0;
  mp3state->mp3file.xing_offset = 0;
  mp3state->mp3file.xingbuffer = NULL;
  //ignore flength error (ex for non seekable stdin)
  mp3state->mp3file.len = splt_io_get_file_length(state, file_input, filename, error);
  splt_t_set_total_time(state, 0);
  mp3state->data_ptr = NULL;
  mp3state->data_len = 0;
  mp3state->buf_len = 0;
  mp3state->bytes = 0;

  //we initialise the mad structures
  splt_mp3_init_stream_frame(mp3state);
  mad_synth_init(&mp3state->synth);

  mad_timer_reset(&mp3state->timer);

  //we read mp3 infos and set pointers to read the mp3 data
  do
  {
    int ret = splt_mp3_get_frame(mp3state);

    if (ret == -2)
    {
      splt_e_set_error_data(state,filename);
      *error = SPLT_ERROR_INVALID;
      goto function_end;
    }

    if ((prev == 0) &&
        ((ret == 0) || (mp3state->stream.error == MAD_ERROR_BUFLEN)))
    {
      break;
    }

    //if we have succeeded to read a frame
    if (ret == 0)
    {
      //we set pointer to the frame
      mp3state->data_ptr = (unsigned char *) mp3state->stream.this_frame;
      //we set length of frame
      if(mp3state->stream.next_frame!=NULL)
      {
        mp3state->data_len = (long) (mp3state->stream.next_frame - mp3state->stream.this_frame);
      }

      if (mp3state->stream.anc_bitlen > 64)
      {
        int tag = 0;
        struct mad_bitptr ptr = mp3state->stream.anc_ptr;
        struct mad_bitptr start = ptr;
        //we search for xing (variable bit rate)
        unsigned long xing_word = mad_bit_read(&ptr, 32);
        if ((xing_word==SPLT_MP3_XING_MAGIC) ||
            (xing_word==SPLT_MP3_INFO_MAGIC))
        {
          tag = 1;
        }
        //Handle misplaced Xing header in mp3 files with CRC
        else 
        {
          if (xing_word == ((SPLT_MP3_XING_MAGIC << 16) & 0xffffffffL) 
              || xing_word == ((SPLT_MP3_INFO_MAGIC << 16) & 0xffffffffL))
          {
            ptr = start;
            mad_bit_skip(&ptr, 16);
            tag = 1;
          }
        }

        //if we have xing, put infos
        if (tag)
        {
          xing_word = mad_bit_read(&ptr, 32);
          if (xing_word & SPLT_MP3_XING_FRAMES)
          {
            mad_timer_t total;
            mp3state->frames = mad_bit_read(&ptr, 32);
            total = mp3state->frame.header.duration;
            mad_timer_multiply(&total, mp3state->frames);
            float total_time_milliseconds = (float) mad_timer_count(total, MAD_UNITS_MILLISECONDS);
            total_time_milliseconds /= 10.f;
            splt_t_set_total_time(state, (long) ceilf(total_time_milliseconds));
          }

          if (xing_word & SPLT_MP3_XING_BYTES)
          {
            if (mp3state->mp3file.len == 0)
              mp3state->mp3file.len = mad_bit_read(&ptr, 32);
          }

          if (splt_o_get_int_option(state, SPLT_OPT_XING))
          {
            mp3state->mp3file.xing = mp3state->data_len;

            if ((mp3state->mp3file.xingbuffer = 
                  malloc(mp3state->mp3file.xing))==NULL)
            {
              *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
              goto function_end;
            }

            memcpy(mp3state->mp3file.xingbuffer, mp3state->data_ptr,
                mp3state->mp3file.xing);
            mp3state->mp3file.xing_offset = splt_mp3_xing_info_off(mp3state);
          }

          splt_o_set_int_option(state, SPLT_OPT_FRAME_MODE, SPLT_TRUE);
          mp3state->framemode = 1;

          if (!splt_o_messages_locked(state))
          {
            if (!splt_o_get_iopt(state, SPLT_INTERNAL_FRAME_MODE_ENABLED))
            {
              int split_mode =
                splt_o_get_int_option(state, SPLT_OPT_SPLIT_MODE);

              if (split_mode != SPLT_OPTION_WRAP_MODE &&
                  split_mode != SPLT_OPTION_ERROR_MODE)
              {
                splt_c_put_info_message_to_client(state,
                    _(" info: found Xing or Info header. Switching to frame mode... \n"));
                splt_o_set_iopt(state, SPLT_INTERNAL_FRAME_MODE_ENABLED, SPLT_TRUE);
              }
            }
          }
          continue;
        }
      }
    }

    prev = ret;
  } while (1);

  len = (long) (mp3state->buf_len - (mp3state->data_ptr - mp3state->inputBuffer));

  if (len < 0)
  {
    splt_e_set_error_data(state,filename);
    *error = SPLT_ERROR_INVALID;
    goto function_end;
  }

  //we put useful infos in the state
  mp3state->mp3file.firsth = (off_t) (mp3state->bytes - len);
  splt_d_print_debug(state, "mp3 firsth bytes = %ld\n", mp3state->bytes);
  splt_d_print_debug(state, "mp3 firsth len = %ld\n", len);
  mp3state->bytes = mp3state->mp3file.firsth;
  mp3state->headw = 
    (unsigned long) ((mp3state->data_ptr[0] << 24) | 
        (mp3state->data_ptr[1] << 16) |
        (mp3state->data_ptr[2] << 8) | (mp3state->data_ptr[3]));
  mp3state->mp3file.mpgid = (int) ((mp3state->headw >> 19)&1);
  mp3state->mp3file.layer = mp3state->frame.header.layer;

  mp3state->mp3file.freq = mp3state->frame.header.samplerate;
  mp3state->mp3file.bitrate = mp3state->frame.header.bitrate/SPLT_MP3_BYTE;

  mp3state->mp3file.firsthead = 
    splt_mp3_makehead(mp3state->headw, mp3state->mp3file, mp3state->mp3file.firsthead, mp3state->mp3file.firsth);

  mp3state->mp3file.fps = (float) (mp3state->mp3file.freq*(2-mp3state->mp3file.mpgid));
  mp3state->mp3file.fps /= SPLT_MP3_PCM;

  //we put the channels stuff (mono, stereo)
  switch(mp3state->frame.header.mode)
  {
    case MAD_MODE_SINGLE_CHANNEL:
      mp3state->mp3file.channels = 0;
      break;
    case MAD_MODE_DUAL_CHANNEL:
      mp3state->mp3file.channels = 1;
      break;
    case MAD_MODE_JOINT_STEREO:
      mp3state->mp3file.channels = 2;
      break;
    case MAD_MODE_STEREO:
      mp3state->mp3file.channels = 3;
      break;
    default:
      mp3state->mp3file.channels = 4;
      break;
  }

  //we put the total time for constant bit rate
  //if it was not set for the variable bit rate
  if (splt_t_get_total_time(state) == 0)
  {
    if (mp3state->mp3file.len > 0)
    {
      long temp = (long)
        (((double)(mp3state->mp3file.len - mp3state->mp3file.firsth)
          / (double)mp3state->mp3file.bitrate) * 100.0);

      splt_t_set_total_time(state, temp);
    }
  }

function_end:
  //we free memory allocated by mad_frame_decode(..)
  //TODO: memory leak
  //splt_mp3_finish_stream_frame(mp3state);
  mad_synth_finish(&mp3state->synth);

  return mp3state;
}

//! End writing a mp3 file
static void splt_mp3_end(splt_state *state, int *error)
{
  splt_mp3_state *mp3state = state->codec;
  if (mp3state)
  {
    splt_mp3_finish_stream_frame(mp3state);
    if (mp3state->file_input)
    {
      if (mp3state->file_input != stdin)
      {
        if (fclose(mp3state->file_input) != 0)
        {
          splt_e_set_strerror_msg_with_data(state, splt_t_get_filename_to_split(state));
          *error = SPLT_ERROR_CANNOT_CLOSE_FILE;
        }
      }
      mp3state->file_input = NULL;
    }
    //we free the mp3 state 
    splt_mp3_state_free(state);
  }
  state->codec = NULL;
}

//!gets the mp3 info and puts it in the state
static void splt_mp3_get_info(splt_state *state, FILE *file_input, int *error)
{
  //checks if valid mp3 file
  //before last argument, if framemode or not
  //last argument if we put messages to clients or not
  state->codec = splt_mp3_info(file_input, state,
        splt_o_get_int_option(state,SPLT_OPT_FRAME_MODE), error);
  //if error
  if ((*error < 0) || (state->codec == NULL))
  {
    if (state->codec != NULL)
    {
      splt_mp3_end(state, error);
    }
    return;
  }
  //print informations about the current file to the client
  else
  {
    if ((! splt_o_messages_locked(state)) &&
        (splt_o_get_int_option(state, SPLT_OPT_SPLIT_MODE) != SPLT_OPTION_WRAP_MODE) &&
        (splt_o_get_int_option(state, SPLT_OPT_SPLIT_MODE) != SPLT_OPTION_ERROR_MODE))
    {
      splt_mp3_state *mp3state = state->codec;
      struct splt_mp3 *mfile = &mp3state->mp3file;

      char mpeg_infos[1024] = { '\0' };
      snprintf(mpeg_infos, 1024, _(" info: MPEG %d Layer %d - %d Hz - %s"),
          (2-mfile->mpgid), mfile->layer, mfile->freq, splt_mp3_chan[mfile->channels]);

      char frame_mode_infos[256] = { '\0' };
      if (mp3state->framemode)
      {
        if (splt_o_get_int_option(state, SPLT_OPT_INPUT_NOT_SEEKABLE))
        {
          snprintf(frame_mode_infos, 255, _(" - FRAME MODE NS"));
        }
        else
        {
          snprintf(frame_mode_infos, 255, _(" - FRAME MODE"));
        }
      }
      else if (splt_o_get_int_option(state, SPLT_OPT_INPUT_NOT_SEEKABLE))
      {
        snprintf(frame_mode_infos, 255, _(" - NS - %d Kb/s"),
            mfile->bitrate * SPLT_MP3_BYTE / 1000);
      }
      else
      {
        snprintf(frame_mode_infos, 255, _(" - %d Kb/s"),
            mfile->bitrate * SPLT_MP3_BYTE / 1000);
      }

      char total_time[256] = { '\0' };
      int total_seconds = (int) (splt_t_get_total_time(state) / 100);
      int minutes = total_seconds / 60;
      int seconds = total_seconds % 60;
      snprintf(total_time,255, _(" - Total time: %dm.%02ds"), minutes, seconds%60);

      splt_c_put_info_message_to_client(state, 
          "%s%s%s\n", mpeg_infos, frame_mode_infos, total_time);
    }
  }
}

/****************************/
/* mp3 split */

static off_t splt_mp3_write_data_ptr(splt_state *state, const char *filename,
    const char *output_fname, FILE *file_output, int *error)
{
  splt_mp3_state *mp3state = state->codec;

  long len = (long) (mp3state->inputBuffer + mp3state->buf_len - mp3state->data_ptr);

  if (len < 0)
  {
    splt_e_set_error_data(state, filename);
    *error = SPLT_ERROR_WHILE_READING_FILE;
    return len;
  }

  if (splt_io_fwrite(state, mp3state->data_ptr, 1, len, file_output) < len)
  {
    splt_e_set_error_data(state,output_fname);
    *error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
    return len;
  }

  mp3state->data_len = 0;

  return len;
}


/*! Copies a file portion to the output

Justs copies the data of the input file from a begin offset
to an end offset, and, eventually, a Xing frame (for VBR)
at the beginning and a ID3v1 at the end, to an outputfile.

used for the mp3 sync errors, dewrap and mp3 seekable split(for header)

\return 
 - 0 if no errors, 
 - SPLT_ defined errors on error.
*/
static int splt_mp3_simple_split(splt_state *state, const char *output_fname,
    off_t begin, off_t end, int do_write_tags, short write_first_frame)
{
  splt_d_print_debug(state,"Mp3 simple split on output _%s_\n", output_fname);
  splt_d_print_debug(state,"Mp3 simple split offset begin is _%ld_\n", begin);
  splt_d_print_debug(state,"Mp3 simple split offset end is _%ld_\n", end);

  splt_mp3_state *mp3state = state->codec;

  int error = SPLT_OK_SPLIT;

  FILE *file_output = NULL;
  off_t position = 0;
  unsigned char buffer[SPLT_MP3_READBSIZE] = { '\0' };
  long readed = 0;
  //for the progress
  off_t temp_end = 0;
  //the start point of the split
  long start = begin;
  int split_mode = splt_o_get_int_option(state, SPLT_OPT_SPLIT_MODE);

  splt_c_put_progress_text(state, SPLT_PROGRESS_CREATE);

  char *filename = splt_t_get_filename_to_split(state);

  position = ftello(mp3state->file_input); // Save current position

  if (fseeko(mp3state->file_input, begin, SEEK_SET)==-1)
  {
    return SPLT_ERROR_BEGIN_OUT_OF_FILE;
  }

  //get the file size
  off_t st_size;
  char *fname_to_split = splt_t_get_filename_to_split(state);
  if(splt_io_stat(fname_to_split, NULL, &st_size) == 0)
  {
    mp3state->end2 = st_size;
  }
  else
  {
    splt_e_set_strerror_msg_with_data(state, fname_to_split);
    return SPLT_ERROR_CANNOT_OPEN_FILE;
  }

  if (! splt_o_get_int_option(state, SPLT_OPT_PRETEND_TO_SPLIT))
  {
    file_output = splt_mp3_open_file_write(state, output_fname, &error);
    if (error < 0) { return error; }
  }

  int output_tags_version = splt_mp3_get_output_tags_version(state);

#ifndef NO_ID3TAG
  //write id3 tags version 2 at the start of the file, if necessary
  if (do_write_tags && (output_tags_version == 2 || output_tags_version == 12))
  {
    int err = SPLT_OK;
    if ((err = splt_mp3_write_id3v2_tags(state, file_output,
            output_fname, NULL)) < 0)
    {
      error = err;
      goto function_end;
    }
  }
#endif

  if (mp3state->mp3file.xing != 0)
  {
    if (splt_o_get_int_option(state, SPLT_OPT_XING))
    {
      //error mode split must only contain original file data
      if (state->options.split_mode != SPLT_OPTION_ERROR_MODE)
      {
        if (splt_io_fwrite(state, mp3state->mp3file.xingbuffer, 1, 
              mp3state->mp3file.xing, file_output) < mp3state->mp3file.xing)
        {
          splt_e_set_error_data(state, output_fname);
          error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
          goto function_end;
        }
      }
    }
  }

  if (write_first_frame)
  {
    splt_mp3_write_data_ptr(state, filename, output_fname, file_output, &error);
    if (error < 0) { goto function_end; }
  }

  while (!feof(mp3state->file_input))
  {
    readed = SPLT_MP3_READBSIZE;
    if (end != -1)
    {
      if (begin >= end) 
      {
        break;
      }
      if ((end - begin) < SPLT_MP3_READBSIZE)
      {
        readed = end - begin;
      }
    }

    if ((readed = fread(buffer, 1, readed, mp3state->file_input))==-1)
    {
      break;
    }

    if (splt_io_fwrite(state, buffer, 1, readed, file_output) < readed)
    {
      splt_e_set_error_data(state,output_fname);
      error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
      goto function_end;
    }
    begin += readed;

    //we update the progress bar
    if ((split_mode == SPLT_OPTION_WRAP_MODE) ||
        (split_mode == SPLT_OPTION_ERROR_MODE) ||
        ((split_mode == SPLT_OPTION_NORMAL_MODE)
         && (!splt_o_get_int_option(state, SPLT_OPT_AUTO_ADJUST)) 
         && (!splt_o_get_int_option(state, SPLT_OPT_FRAME_MODE))))
    {
      temp_end = end;
      //for the last split
      if (end == -1)
      {
        temp_end = mp3state->end2;
      }

      splt_c_update_progress(state,(double)(begin-start),
          (double)(temp_end-start),1,0,
          SPLT_DEFAULT_PROGRESS_RATE);
    }
    else
    {
      //if auto adjust, we have 50%
      if (splt_o_get_int_option(state, SPLT_OPT_AUTO_ADJUST))
      {
        splt_c_update_progress(state,(double)(begin-start),
            (double)(end-start),
            2,0.5, SPLT_DEFAULT_PROGRESS_RATE);
      }
      else
      {
        if (splt_o_get_int_option(state, SPLT_OPT_SPLIT_MODE)
            == SPLT_OPTION_TIME_MODE)
        {
          temp_end = end;
          //for the last split
          if (end == -1)
          {
            temp_end = mp3state->end2;
          }

          //if framemode
          if (splt_o_get_int_option(state, SPLT_OPT_FRAME_MODE))
          {
            splt_c_update_progress(state,(double)(begin-start),
                (double)(temp_end-start),
                2,0.5, SPLT_DEFAULT_PROGRESS_RATE);
          }
          else
          {
            splt_c_update_progress(state,(double)(begin-start),
                (double)(temp_end-start),
                1,0, SPLT_DEFAULT_PROGRESS_RATE);
          }
        }
        else
        {
          splt_c_update_progress(state,(double)(begin-start),
              (double)(end-start),
              2,0.5, SPLT_DEFAULT_PROGRESS_RATE);
        }
      }
    }
  }

  //write id3 tags version 1 at the end of the file, if necessary
  if (do_write_tags && (output_tags_version == 1 || output_tags_version == 12))
  {
    int err = SPLT_OK;
    if ((err = splt_mp3_write_id3v1_tags(state, file_output, output_fname)) < 0)
    {
      error = err;
      goto function_end;
    }
  }

  if (fseeko(mp3state->file_input, position, SEEK_SET)==-1)
  {
    splt_e_set_strerror_msg_with_data(state, filename);
    goto function_end;
  }

function_end:
  if (file_output)
  {
    if (file_output != stdout)
    {
      if (fclose(file_output) != 0)
      {
        splt_e_set_strerror_msg_with_data(state, filename);
        return SPLT_ERROR_CANNOT_CLOSE_FILE;
      }
    }
    file_output = NULL;
  }

  return error;
}

/*!  the main mp3 split function

\param filename our filename
\param fbegin_sec the begin splitpoint
\param fend_sec the end splitpoint
\param adjustoption True if we want to fine-tune split points using
silence detection 
\param seekable True if the input file is seekable
\param threshold See manual
\param state The central structure libmp3splt keeps all its data in
\param error The error code if there has been an error
\return The number of seconds we ended our task at
\attention splt_mp3_info() must be called before calling this function 
*/

static double splt_mp3_split(const char *output_fname, splt_state *state,
    double fbegin_sec, double fend_sec, int *error, int save_end_point)
{
  splt_d_print_debug(state,"Mp3 split...\n");
  splt_d_print_debug(state,"Output filename is _%s_\n", output_fname);
  splt_d_print_debug(state,"Begin position is _%lf_\n", fbegin_sec);
  splt_d_print_debug(state,"End position is _%lf_\n", fend_sec);

  splt_mp3_state *mp3state = state->codec;

  int adjustoption = splt_o_get_int_option(state, SPLT_OPT_PARAM_GAP);
  short seekable = ! splt_o_get_int_option(state, SPLT_OPT_INPUT_NOT_SEEKABLE);
  float threshold = splt_o_get_float_option(state, SPLT_OPT_PARAM_THRESHOLD);

  short fend_sec_is_not_eof =
    !splt_u_fend_sec_is_bigger_than_total_time(state, fend_sec);

  short eof=0, check_bitrate=0;

  char *filename = splt_t_get_filename_to_split(state);

  FILE *file_output = NULL;
  short writing = 0, finished=0;
  unsigned long fbegin=0;
  off_t wrote = 0;
  long len = 0;
  //for the progress
  unsigned long stopped_frames = 0;
  int progress_adjust_val = 2;

  if (adjustoption) 
  {
    progress_adjust_val = 4;
  }

  splt_c_put_progress_text(state,SPLT_PROGRESS_CREATE);

  double sec_end_time = fend_sec;

  //if not seekable
  if (!seekable)
  {
    if (! splt_o_get_int_option(state, SPLT_OPT_PRETEND_TO_SPLIT))
    {
      file_output = splt_mp3_open_file_write(state, output_fname, error);
      if (*error < 0) { return sec_end_time; };
    }

    int output_tags_version = splt_mp3_get_output_tags_version(state);

    off_t id3v2_end_offset = 0;
#ifndef NO_ID3TAG
    //write id3 tags version 2 at the start of the file
    if (output_tags_version == 2 || output_tags_version == 12)
    {
      int err = SPLT_OK;
      if ((err = splt_mp3_write_id3v2_tags(state, file_output,
              output_fname, &id3v2_end_offset)) < 0)
      {
        *error = err;
        goto bloc_end;
      }
    }
#endif

    //if we have the framemode
    if (mp3state->framemode)
    {
      splt_d_print_debug(state,"Starting not seekable mp3 frame mode...\n");

      long begin_c, end_c, time;
      //convert seconds to hundreths
      begin_c = (long) (fbegin_sec * 100);
      if (fend_sec > 0)
      {
        end_c = (long) (fend_sec * 100);
      }
      else 
      {
        end_c = 0;
      }
      time = 0;

      do
      {
        //we write xing if necessary
        if (!writing && (time >= begin_c))
        {
          writing = 1;
          fbegin = mp3state->frames;

          if (mp3state->mp3file.xing > 0)
          {
            wrote = splt_io_fwrite(state, mp3state->mp3file.xingbuffer,
                1, mp3state->mp3file.xing, file_output);
            if (wrote < mp3state->mp3file.xing)
            {
              splt_e_set_error_data(state,output_fname);
              *error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
              goto bloc_end;
            }
          }
        }

        //we do the split
        if (writing)
        {
          if (mp3state->data_len > 0)
          {
            if ((len = splt_io_fwrite(state, mp3state->data_ptr, 1, mp3state->data_len, file_output))
                < mp3state->data_len)
            {
              splt_e_set_error_data(state,output_fname);
              *error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
              goto bloc_end;
            }
            wrote = (off_t) (wrote + len);
            mp3state->data_len = 0;
          }

          if ((end_c > 0) && (time >= end_c))
          {
            finished = 1;
          }
          if (eof || finished)
          {
            finished = 1;
            if (eof) { *error = SPLT_OK_SPLIT_EOF; }
            break;
          }
        }

        //progress bar
        if (splt_o_get_int_option(state,SPLT_OPT_SPLIT_MODE)
            == SPLT_OPTION_TIME_MODE)
        {
          splt_c_update_progress(state,(double)(time-begin_c),
              (double)(end_c-begin_c),1,0,
              SPLT_DEFAULT_PROGRESS_RATE);
        }
        else
        {
          splt_c_update_progress(state,(double)(time),
              (double)(end_c),1,0,
              SPLT_DEFAULT_PROGRESS_RATE);
        }

        int mad_err = SPLT_OK;
        switch (splt_mp3_get_valid_frame(state, &mad_err))
        {
          case 1:
            mad_timer_add(&mp3state->timer, mp3state->frame.header.duration);
            mp3state->frames++;
            time = (unsigned long) mad_timer_count(mp3state->timer, MAD_UNITS_CENTISECONDS);
            break;
          case 0:
            break;
          case -1:
            eof = 1;
            *error = SPLT_OK_SPLIT_EOF;
            break;
          case -3:
            //error from libmad
            *error = mad_err;
            goto bloc_end;
            break;
          default:
            break;
        }

      } while (!finished);
    }
    //if we don't have the framemode
    else
    {
      splt_d_print_debug(state,"Starting mp3 not seekable non frame mode...\n");

      off_t begin = 0, end = 0;
      if (fend_sec_is_not_eof)
      {
        end = (off_t) (fend_sec * mp3state->mp3file.bitrate + mp3state->mp3file.firsth);
      }
      else
      {
        end = -1;
      }

      //find begin point because no 'end' saved point
      if (mp3state->end == 0)
      {
        begin = (off_t) (fbegin_sec * mp3state->mp3file.bitrate + mp3state->mp3file.firsth);

        if ((mp3state->bytes == begin) && (mp3state->data_len > 0))
        {
          wrote += splt_mp3_write_data_ptr(state, filename, output_fname, file_output, error);
          if (*error < 0) { goto bloc_end; }
        }
        else
        {
          while (mp3state->bytes < begin)
          {
            off_t to_read;
            if (feof(mp3state->file_input))
            {
              *error = SPLT_ERROR_BEGIN_OUT_OF_FILE;
              goto bloc_end;
            }
            to_read = (begin - mp3state->bytes);
            if (to_read > SPLT_MAD_BSIZE)
              to_read = SPLT_MAD_BSIZE;
            if ((mp3state->data_len = fread(mp3state->inputBuffer, 1, to_read, mp3state->file_input))<=0)
            {
              *error = SPLT_ERROR_BEGIN_OUT_OF_FILE;
              goto bloc_end;
            }
            mp3state->bytes+=mp3state->data_len;
          }

          int mad_err = SPLT_OK;
          splt_mp3_init_stream_frame(mp3state);
          switch (splt_mp3_get_valid_frame(state, &mad_err))
          {
            case 1:
              len = (long) (mp3state->inputBuffer + mp3state->buf_len - mp3state->data_ptr);
              if (len < 0)
              {
                splt_e_set_error_data(state,filename);
                *error = SPLT_ERROR_WHILE_READING_FILE;
                goto bloc_end;
              }
              if (splt_io_fwrite(state, mp3state->data_ptr, 1, len, file_output) < len)
              {
                splt_e_set_error_data(state,output_fname);
                *error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
                goto bloc_end;
              }
              wrote = (off_t) (wrote + len);
              mp3state->data_len = 0;
              break;
            case 0:
              break;
            case -1:
              eof = 1;
              *error = SPLT_ERROR_BEGIN_OUT_OF_FILE;
              break;
            case -3:
              //error from libmad
              *error = mad_err;
              goto bloc_end;
              break;
            default:
              break;
          }
        }
      }
      //set the 'begin' as the saved 'end'
      else
      {
        len = (long) (mp3state->inputBuffer + mp3state->buf_len - mp3state->data_ptr);
        if (len < 0)
        {
          splt_e_set_error_data(state,filename);
          *error = SPLT_ERROR_WHILE_READING_FILE;
          goto bloc_end;
        }
        if (splt_io_fwrite(state, mp3state->data_ptr, 1, len, file_output) < len)
        {
          splt_e_set_error_data(state,output_fname);
          *error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
          goto bloc_end;
        }
        wrote = (off_t) (wrote + len);
        mp3state->data_len = 0;
        begin = mp3state->end;
      }

      long split_begin_point = mp3state->bytes;
      //while not end of file, read write:
      while (!eof)
      {
        off_t to_read = SPLT_MAD_BSIZE;
        if (end > 0)
        {
          to_read = (end - mp3state->bytes);
          if (to_read <= 0)
          {
            break;
          }
          if (to_read > SPLT_MAD_BSIZE)
            to_read = SPLT_MAD_BSIZE;
        }

        if (feof(mp3state->file_input) || 
            ((mp3state->data_len = 
              fread(mp3state->inputBuffer, 1, to_read, mp3state->file_input))<=0))
        {
          eof = 1;
          *error = SPLT_OK_SPLIT_EOF;
          break;
        }

        if (splt_io_fwrite(state, mp3state->inputBuffer, 1,
              mp3state->data_len, file_output) < mp3state->data_len)
        {
          splt_e_set_error_data(state,output_fname);
          *error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
          goto bloc_end;
        }

        mp3state->bytes += mp3state->data_len;

        splt_c_update_progress(state, (double) (mp3state->bytes-split_begin_point),
            (double)(end-split_begin_point), 1,0,SPLT_DEFAULT_PROGRESS_RATE);
      }

      splt_mp3_save_end_point(state, mp3state, save_end_point, end);

      if (!eof)
      {
        //take the whole last frame : might result in more frames
        //but if we don't do it, we might have less frames
        int mad_err = SPLT_OK;
        splt_mp3_init_stream_frame(mp3state);
        switch (splt_mp3_get_valid_frame(state, &mad_err))
        {
          case 1:
            len = (long) (mp3state->data_ptr - mp3state->inputBuffer);
            if (len < 0)
            {
              splt_e_set_error_data(state,filename);
              *error = SPLT_ERROR_WHILE_READING_FILE;
              goto bloc_end;
            }
            if (splt_io_fwrite(state, mp3state->inputBuffer, 1, len, file_output) < len)
            {
              splt_e_set_error_data(state,output_fname);
              *error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
              goto bloc_end;
            }
            break;
          case 0:
            break;
          case -1:
            eof = 1;
            *error = SPLT_OK_SPLIT_EOF;
            break;
          case -3:
            *error = mad_err;
            goto bloc_end;
            break;
          default:
            break;
        }
      }
    }

    //we write id3 and other stuff
    if (file_output)
    {
      if (mp3state->mp3file.xing > 0)
      {
        if (fseeko(file_output, mp3state->mp3file.xing_offset+4+id3v2_end_offset, SEEK_SET)!=-1)
        {
          unsigned long headw = (unsigned long) (mp3state->frames - fbegin + 1); // Frames
          fputc((headw >> 24) & 0xFF, file_output);
          fputc((headw >> 16) & 0xFF, file_output);
          fputc((headw >> 8) & 0xFF, file_output);
          fputc((headw >> 0) & 0xFF, file_output);
          headw = (unsigned long) (wrote); // Bytes
          fputc((headw >> 24) & 0xFF, file_output);
          fputc((headw >> 16) & 0xFF, file_output);
          fputc((headw >> 8) & 0xFF, file_output);
          fputc((headw >> 0) & 0xFF, file_output);
        }
        else
        {
          splt_e_set_strerror_msg_with_data(state, output_fname);
          *error = SPLT_ERROR_SEEKING_FILE;
          goto bloc_end;
        }
      }

      //write id3 tags version 1 at the end of the file
      if (output_tags_version == 1 || output_tags_version == 12)
      {
        int err = SPLT_OK;
        if ((err = splt_mp3_write_id3v1_tags(state, file_output, output_fname)) < 0)
        {
          *error = err;
          goto bloc_end;
        }
      }
    }

bloc_end:
    if (file_output)
    {
      if (file_output != stdout)
      {
        if (fclose(file_output) != 0)
        {
          splt_e_set_strerror_msg_with_data(state, output_fname);
          *error = SPLT_ERROR_CANNOT_CLOSE_FILE;
        }
      }
    }
    file_output = NULL;

    if (*error == SPLT_OK) { *error = SPLT_OK_SPLIT; }

    return sec_end_time;
  }
  //if seekable:
  else
  {
    short write_first_frame = SPLT_FALSE;
    off_t begin = 0, end = 0;
    //if framemode
    if (mp3state->framemode)
    {
      splt_d_print_debug(state,"Starting seekable mp3 frame mode...\n");

      unsigned long fbegin, fend, adjust;
      fbegin = fend = adjust = 0;
      //prefer to split a bit before the start than loosing some frame
      //so we don't 'ceilf'
      fbegin = fbegin_sec * mp3state->mp3file.fps;

      if (fend_sec_is_not_eof)
      {
        //if adjustoption
        if (adjustoption)
        {
          if (fend_sec_is_not_eof)
          {
            float adj = (float) (adjustoption);
            float len = (fend_sec - fbegin_sec);
            if (adj > len)
            {
              adj = len;
            }
            if (fend_sec > adj)
            {
              fend_sec -= adj;
            }
            adjust = (unsigned long) (adj * 100.f);
          }
          else 
          {
            adjust = 0;
          }
        }
        //prefer to split a bit after the end than loosing some frame
        //before the end
        fend = (unsigned long) ceilf(fend_sec * mp3state->mp3file.fps);
      }
      else 
      {
        fend = 0xFFFFFFFF;
      }

      splt_d_print_debug(state,"Finding begin...\n");

      if (mp3state->end == 0)
      {
        if (mp3state->first)
        {
          mp3state->h.ptr = mp3state->mp3file.firsthead.ptr;
          mp3state->h.framesize = mp3state->mp3file.firsthead.framesize;
          begin = mp3state->mp3file.firsthead.ptr;
          mp3state->first = 0;
        }

        splt_c_put_progress_text(state,SPLT_PROGRESS_PREPARE);

        //this happens when we end and start on the same frame
        if (mp3state->frames >= fbegin && mp3state->end_non_zero != 0)
        {
          begin = mp3state->end_non_zero;
        }

        // Finds begin by counting frames
        while (mp3state->frames < fbegin)
        {
          begin = splt_mp3_findhead(mp3state, mp3state->h.ptr + mp3state->h.framesize);
          if (begin == -1) { *error = SPLT_ERROR_BEGIN_OUT_OF_FILE; goto bloc_end2; }

          //count the number of syncerrors
          if ((begin!=mp3state->h.ptr + mp3state->h.framesize)&&(state->syncerrors>=0)) 
          {
            state->syncerrors++;
          }
          if ((mp3state->syncdetect)&&(state->syncerrors> SPLT_MAXSYNC))
          {
            splt_mp3_checksync(mp3state);
          }

          mp3state->h = splt_mp3_makehead(mp3state->headw, mp3state->mp3file, mp3state->h, begin);
          mp3state->frames++;

          //if we have adjust mode, then put only 25%
          //else put 50%
          if (adjustoption)
          {
            splt_c_update_progress(state,(double)(mp3state->frames),
                (double)fend, 8,
                0,SPLT_DEFAULT_PROGRESS_RATE);
          }
          else
          {
            splt_c_update_progress(state,(double)(mp3state->frames),
                (double)fend,progress_adjust_val,
                0,SPLT_DEFAULT_PROGRESS_RATE);
          }
        }
      }
      else
      {
        begin = mp3state->end;
      }

      splt_d_print_debug(state,"Begin is _%ld_\n", begin);

      if (mp3state->mp3file.len > 0)
      {
        if (begin >= mp3state->mp3file.len) // If we can check, we just do that :)
        {
          *error = SPLT_ERROR_BEGIN_OUT_OF_FILE;
          goto bloc_end2;
        }
      }

      splt_c_put_progress_text(state,SPLT_PROGRESS_PREPARE);

      long int frames_begin = mp3state->frames;
      // Finds end by counting frames
      while (mp3state->frames <= fend)
      {
        mp3state->frames++;
        end = splt_mp3_findhead(mp3state, mp3state->h.ptr + mp3state->h.framesize);
        if (end == -1)
        {
          end = mp3state->h.ptr + mp3state->h.framesize; // Last valid offset
          eof=1;
          *error = SPLT_OK_SPLIT_EOF;
          break;
        }

        //count the number of syncerrors
        if ((end != mp3state->h.ptr + mp3state->h.framesize)&&(state->syncerrors>=0))
        {
          state->syncerrors++;
        }
        if ((mp3state->syncdetect)&&(state->syncerrors>SPLT_MAXSYNC))
        {
          splt_mp3_checksync(mp3state);
        }

        mp3state->h = splt_mp3_makehead (mp3state->headw, mp3state->mp3file, mp3state->h, end);

        //if we have a progress callback function
        //time split only calculates the end of the 
        //split
        int split_mode = splt_o_get_int_option(state, SPLT_OPT_SPLIT_MODE);
        if (((split_mode == SPLT_OPTION_TIME_MODE) || 
              (split_mode == SPLT_OPTION_SILENCE_MODE) ||
              (split_mode == SPLT_OPTION_TRIM_SILENCE_MODE))
            && (!splt_o_get_int_option(state,SPLT_OPT_AUTO_ADJUST)))
        {
          splt_c_update_progress(state, (double)(mp3state->frames-fbegin),
              (double)(fend-fbegin), progress_adjust_val,
              0, SPLT_DEFAULT_PROGRESS_RATE);
        }
        else
        {
          if (adjustoption)
          {
            if(adjust)
            {
              if (split_mode == SPLT_OPTION_TIME_MODE)
              {
                splt_c_update_progress(state,
                    (double)(mp3state->frames-frames_begin),
                    (double)(fend-frames_begin),
                    4,0,SPLT_DEFAULT_PROGRESS_RATE);
              }
              else
              {
                splt_c_update_progress(state,
                    (double)(mp3state->frames-frames_begin),
                    (double)(fend-frames_begin),
                    8,1/(float)8,SPLT_DEFAULT_PROGRESS_RATE);
              }
            }
          }
          else
          {
            splt_c_update_progress(state,
                (double)(mp3state->frames-stopped_frames),
                (double)(fend-stopped_frames),
                progress_adjust_val,
                0,SPLT_DEFAULT_PROGRESS_RATE);
          }
        }

        //if adjust option, scans for silence
        if ((adjust) && (mp3state->frames >= fend))
        {
          int silence_points_found =
            splt_mp3_scan_silence(state, end, 2 * adjust, threshold, 0.f, 0, error,
                splt_scan_silence_processor);
          //if error, go out
          if (silence_points_found == -1)
          {
            goto bloc_end2;
          }
          else if (silence_points_found > 0)
          {
            adjust = (unsigned long) (splt_siu_silence_position(state->silence_list, mp3state->off) 
                * mp3state->mp3file.fps);
          }
          else
          {
            adjust = (unsigned long) (adjustoption * mp3state->mp3file.fps);
          }
          fend += adjust;
          end = splt_mp3_findhead(mp3state, end);

          sec_end_time = mp3state->frames / mp3state->mp3file.fps;

          splt_siu_ssplit_free(&state->silence_list);
          adjust=0;
          //progress
          splt_c_put_progress_text(state,SPLT_PROGRESS_PREPARE);
          stopped_frames = mp3state->frames;
        }
      }

      splt_mp3_save_end_point(state, mp3state, save_end_point, end);

      //if xing, we get xing
      if (mp3state->mp3file.xing > 0)
      {
        unsigned long headw;
        headw = (unsigned long) (mp3state->frames - fbegin + 1); // Frames
        mp3state->mp3file.xingbuffer[mp3state->mp3file.xing_offset+4] = (headw >> 24) & 0xFF;
        mp3state->mp3file.xingbuffer[mp3state->mp3file.xing_offset+5] = (headw >> 16) & 0xFF;
        mp3state->mp3file.xingbuffer[mp3state->mp3file.xing_offset+6] = (headw >> 8) & 0xFF;
        mp3state->mp3file.xingbuffer[mp3state->mp3file.xing_offset+7] = headw  & 0xFF;
        //we put the length of the file if end is -1
        if (end == -1)
        {
          end = mp3state->mp3file.len;
        }
        headw = (unsigned long) (end - begin + mp3state->mp3file.xing); // Bytes
        mp3state->mp3file.xingbuffer[mp3state->mp3file.xing_offset+8] = (headw >> 24) & 0xFF;
        mp3state->mp3file.xingbuffer[mp3state->mp3file.xing_offset+9] = (headw >> 16) & 0xFF;
        mp3state->mp3file.xingbuffer[mp3state->mp3file.xing_offset+10] = (headw >> 8) & 0xFF;
        mp3state->mp3file.xingbuffer[mp3state->mp3file.xing_offset+11] = headw  & 0xFF;
      }
    }
    else
    //if not framemode
    {
      splt_d_print_debug(state,"Starting mp3 seekable non frame mode...\n");

      long first_frame_offset = mp3state->inputBuffer + mp3state->buf_len - mp3state->data_ptr;

      //find begin point if the last 'end' not saved
      if (mp3state->end == 0) 
      {
        begin = (off_t) (fbegin_sec * mp3state->mp3file.bitrate + mp3state->mp3file.firsth);

        if ((mp3state->bytes == begin) && (mp3state->data_len > 0))
        {
          write_first_frame = SPLT_TRUE;
          begin += first_frame_offset;
        }
        else
        {
          begin = splt_mp3_findvalidhead(mp3state, begin);
        }

        if (begin == -1)
        {
          *error = SPLT_ERROR_BEGIN_OUT_OF_FILE;
          goto bloc_end2;
        }
        if (splt_mp3_tabsel_123[1 - mp3state->mp3file.mpgid][mp3state->mp3file.layer-1][splt_mp3_c_bitrate(mp3state->headw)] != 
            mp3state->mp3file.firsthead.bitrate)
        {
          check_bitrate = 1;
        }
      }
      //set the begin point from the last 'end' saved
      else 
      {
        begin = mp3state->end;
      }

      if (fend_sec_is_not_eof)
      {
        end = (off_t) (fend_sec * mp3state->mp3file.bitrate + mp3state->mp3file.firsth);
        if (write_first_frame)
        {
          end += first_frame_offset;
        }
        //take the whole last frame : might result in more frames
        //but if we don't do it, we might have less frames
        end = splt_mp3_findvalidhead(mp3state, end);
        if (splt_mp3_tabsel_123[1 - mp3state->mp3file.mpgid][mp3state->mp3file.layer-1][splt_mp3_c_bitrate(mp3state->headw)] != 
            mp3state->mp3file.firsthead.bitrate)
          check_bitrate = 1;
      }
      else
      {
        end = -1;
      }

      splt_mp3_save_end_point(state, mp3state, save_end_point, end);
    }

    //seekable real split
    int err = splt_mp3_simple_split(state, output_fname, begin, end,
        SPLT_TRUE, write_first_frame);
    if (err < 0) { *error = err; }

    if (!save_end_point)
    {
      if (splt_o_get_long_option(state, SPLT_OPT_OVERLAP_TIME) > 0)
      {
        mp3state->frames = 1;
        mp3state->first = 1;
      }
    }
  }

  if (check_bitrate)
  {
    *error = SPLT_MIGHT_BE_VBR;
  }

  if (*error == SPLT_OK) { *error = SPLT_OK_SPLIT; }

bloc_end2:

  return sec_end_time;
}

/****************************/
/* mp3 syncerror */

/*! this function searches for the id3v1 and id3v2 and returns the offset
*/
static off_t splt_mp3_adjustsync(splt_mp3_state *mp3state, off_t begin, off_t end)
{
  off_t position;
  position = begin;
  if (fseeko(mp3state->file_input, position, SEEK_SET)==-1)
  {
    return (off_t) (-1);
  }

  // First we search for ID3v1
  while (position++ < end)
  {
    if (fgetc(mp3state->file_input)=='T') {
      if (fgetc(mp3state->file_input)=='A') {
        if (fgetc(mp3state->file_input)=='G')
          return (position + 127);
        else position++;
      }
      if (fseeko(mp3state->file_input, -1, SEEK_CUR) == -1)
      {
        return (off_t) (-1);
      }
    }
  }

  position = begin;

  if (fseeko(mp3state->file_input, position, SEEK_SET)==-1)
  {
    return (off_t) (-1);
  }

  // Now we search for ID3v2
  while (position++ < end) 
  {
    if (fgetc(mp3state->file_input)=='I')
    {
      if (fgetc(mp3state->file_input)=='D')
      {
        if (fgetc(mp3state->file_input)=='3')
        {
          return (position - 1);
        }
        else 
        {
          position++;
        }
      }
      if(fseeko(mp3state->file_input, -1, SEEK_CUR)==-1)
      {
        return (off_t) (-1);
      }
    }
  }

  return end;
}

//!This function counts the number of sync error splits and sets the offsets
static void splt_mp3_syncerror_search(splt_state *state, int *error)
{
  off_t offset = 0;
  char *filename = splt_t_get_filename_to_split(state);
  int sync_err = SPLT_OK;

  splt_mp3_state *mp3state = state->codec;

  splt_c_put_progress_text(state,SPLT_PROGRESS_SEARCH_SYNC);

  mp3state->h.ptr = mp3state->mp3file.firsthead.ptr;
  mp3state->h.framesize = mp3state->mp3file.firsthead.framesize;

  //we get the file length for the progress
  off_t st_size;
  if(splt_io_stat(filename, NULL, &st_size) == 0)
  {
    //put the start point
    sync_err = splt_se_serrors_append_point(state, 0);
    if (sync_err != SPLT_OK)
    {
      *error = sync_err;
      return;
    }

    //search for sync errors and put in splitpoints
    while (state->serrors->serrors_points_num < SPLT_MAXSYNC)
    {
      offset = splt_mp3_findhead(mp3state, mp3state->h.ptr + mp3state->h.framesize);
      if (offset==-1)
      {
        break;
      }

      if (offset != mp3state->h.ptr + mp3state->h.framesize)
      {
        off_t serror_point =
          splt_mp3_adjustsync(mp3state, mp3state->h.ptr, offset);

        sync_err = splt_se_serrors_append_point(state, serror_point);
        if (sync_err != SPLT_OK)
        {
          *error = sync_err;
          return;
        }
        offset = splt_mp3_findvalidhead(mp3state, serror_point);
        if (splt_io_get_word(mp3state->file_input, offset, SEEK_SET, &mp3state->headw) == -1)
        {
          *error = SPLT_ERR_SYNC;
          return;
        }
      }

      mp3state->h = splt_mp3_makehead (mp3state->headw, mp3state->mp3file,
          mp3state->h, offset);

      if (splt_t_split_is_canceled(state))
      {
        *error = SPLT_SPLIT_CANCELLED;
        return;
      }

      //progress
      splt_c_update_progress(state,(double)(offset),
          (double)(st_size),1,0,
          SPLT_DEFAULT_PROGRESS_RATE);
    }
  }
  else
  {
    splt_e_set_strerror_msg_with_data(state, filename);
    *error = SPLT_ERROR_CANNOT_OPEN_FILE;
    return;
  }

  if (state->serrors->serrors_points_num == 0)
  {
    *error = SPLT_ERR_NO_SYNC_FOUND;
    return;
  }

  if (state->serrors->serrors_points_num == SPLT_MAXSYNC)
  {
    *error = SPLT_ERR_TOO_MANY_SYNC_ERR;
    return;
  }

  //put the end point
  sync_err = splt_se_serrors_append_point(state, LONG_MAX);
  if (sync_err != SPLT_OK)
  {
    *error = sync_err;
    return;
  }

  *error = SPLT_SYNC_OK;

  return;
}

/****************************/
/* mp3 dewrap */

//! The header that tells this album was generated with mp3wrap
static const unsigned char splt_mp3_albumwraphead[22] =
{
  0xa, 0x23, 0x54, 0x49, 0x54, 0x32, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x41, 0x6c, 0x62, 0x75, 0x6d, 0x57, 0x72, 0x61, 0x70,
};

/*! this function dewraps a file

\param state The central structure libmp3splt keeps all its data in
\param error The error code if there has been an error
*/
static void splt_mp3_dewrap(int listonly, const char *dir, int *error, splt_state *state)
{
  if (listonly)
  {
    *error = SPLT_OK;
  }
  else
  {
    *error = SPLT_DEWRAP_OK;
  }

  //if albumwrap or mp3wrap
  short albumwrap=0, mp3wrap=0;
  //wrapfiles = the wrapped files number
  int wrapfiles=0, i, j, k=0;
  unsigned char c;
  char filename[2048] = { '\0' };
  off_t begin=0, end=0, len = 0, id3offset = 0;
  char junk[2048] = { '\0' };
  char *file_to_dewrap = splt_t_get_filename_to_split(state);

  //if error
  if (*error < 0)
  {
    return;
  }
  else
  {
    splt_mp3_state *mp3state = state->codec;

    if (*error >= 0)
    {
      len = splt_io_get_file_length(state, mp3state->file_input, file_to_dewrap, error);
      if (error < 0) { return; }

      id3offset = splt_mp3_getid3v2_end_offset(mp3state->file_input, 0);

      //we go at the beginning of the file
      if (fseeko(mp3state->file_input, id3offset, SEEK_SET)==-1)
      {
        *error = SPLT_DEWRAP_ERR_FILE_NOT_WRAPED_DAMAGED;
        return;
      }

      splt_d_print_debug(state,"Searching for wrap string...\n");

      //we search the WRAP string in the file to see if it was wrapped
      //with mp3wrap
      for (i=0; i<16384; i++)
      {
        if (feof(mp3state->file_input))
        {
          *error = SPLT_DEWRAP_ERR_FILE_NOT_WRAPED_DAMAGED;
          return;
        }
        if ((id3offset = ftello(mp3state->file_input))==-1)
        {
          *error = SPLT_DEWRAP_ERR_FILE_NOT_WRAPED_DAMAGED;
          return;
        }
        if (fgetc(mp3state->file_input)=='W')
          if (fgetc(mp3state->file_input)=='R')
            if (fgetc(mp3state->file_input)=='A')
              if (fgetc(mp3state->file_input)=='P')
              {
                mp3wrap = 1;
                break;
              }
      }

      //we check if the file was wrapped with albumwrap
      //only if not mp3wrap
      if (!mp3wrap && (id3offset!=0))
      {    
        if (fseeko(mp3state->file_input, (off_t) 8, SEEK_SET)==-1)
        {
          *error = SPLT_DEWRAP_ERR_FILE_NOT_WRAPED_DAMAGED;
          return;
        }      
        albumwrap = 1;
        for (i=0; i<22; i++)
        {
          if (splt_mp3_albumwraphead[i]!=fgetc(mp3state->file_input))
          {
            albumwrap = 0;
            break;
          }
        }
      }

      //we do the mp3wrap or albumwrap
      if (albumwrap || mp3wrap)
      {
        splt_d_print_debug(state,"Effective dewrap...\n");

        //mp3wrap checkings and we get the wrap file number
        if (mp3wrap) {
          splt_d_print_debug(state,"Mp3 mp3wrap check...\n");
          short indexver;

          //Mp3Wrap version
          char major_v = fgetc(mp3state->file_input);
          char minor_v = fgetc(mp3state->file_input);

          splt_c_put_info_message_to_client(state, 
              _(" Detected file created with: Mp3Wrap v. %c.%c\n"),
              major_v,minor_v);

          indexver = fgetc(mp3state->file_input);
          if (indexver > SPLT_MP3_INDEXVERSION)
          {
            *error = SPLT_DEWRAP_ERR_VERSION_OLD;
            return;
          }
          wrapfiles = (int) fgetc(mp3state->file_input);
          if (feof(mp3state->file_input)) 
          {
            *error = SPLT_DEWRAP_ERR_FILE_NOT_WRAPED_DAMAGED;
            return;
          }
          //crc
          if (indexver > 0x0)
          {
            unsigned long crc = 0, fcrc = 0;
            if (splt_io_get_word(mp3state->file_input, 0, SEEK_CUR, &fcrc)==-1)
            {
              *error = SPLT_DEWRAP_ERR_FILE_NOT_WRAPED_DAMAGED;
              return;
            }

            //perform CRC only if we don't have quiet mode
            if (! splt_o_get_int_option(state, SPLT_OPT_QUIET_MODE))
            {
              begin = ftello(mp3state->file_input);
              if (fseeko(mp3state->file_input, 
                    splt_mp3_getid3v1_offset(mp3state->file_input), SEEK_END)==-1)
              {
                splt_e_set_strerror_msg_with_data(state, file_to_dewrap);
                *error = SPLT_ERROR_SEEKING_FILE;
                return;
              }
              end = ftello(mp3state->file_input);
              splt_c_put_info_message_to_client(state,
                  _(" Check for file integrity: calculating CRC please wait... "));
              crc = splt_mp3_c_crc(state, mp3state->file_input, begin, end, error);
              if (*error < 0)
              {
                return;
              }
              if (crc != fcrc)
              {
                //No interactivity in the library (for the moment)
                //fprintf (stderr, "BAD\nWARNING: Bad CRC. File might be damaged. Continue anyway? (y/n) ");
                //fgets(junk, 32, stdin);
                //if (junk[0]!='y')
                //error("Aborted.",125);
                //- no interactivity in the library for the moment
                *error = SPLT_ERROR_CRC_FAILED;
                return;
              }
              else 
              {
                splt_c_put_info_message_to_client(state, _(" OK\n"));
              }
              if (fseeko(mp3state->file_input, begin, SEEK_SET)==-1)
              {
                splt_e_set_strerror_msg_with_data(state, file_to_dewrap);
                *error = SPLT_ERROR_SEEKING_FILE;
                return;
              }
            }
          }
        }

        //the albumwrap checkings and we get the wrap files number in
        //wrapfiles variable
        if (albumwrap)
        {
          splt_d_print_debug(state,"Mp3 albumwrap check...\n");

          splt_c_put_info_message_to_client(state, 
              _(" Detected file created with: AlbumWrap\n"));

          if (fseeko(mp3state->file_input, (off_t) 0x52d, SEEK_SET)==-1)
          {
            *error = SPLT_DEWRAP_ERR_FILE_NOT_WRAPED_DAMAGED;
            return;
          }
          i = 0;
          while (((c=fgetc(mp3state->file_input))!=0x20) &&(i<2048))
            junk[i++] = c;
          junk[i] = '\0';
          wrapfiles = atoi (junk);
        }
        if (wrapfiles<=0)
        {
          *error = SPLT_DEWRAP_ERR_NO_FILE_OR_BAD_INDEX;
          return;
        }

        //we put the number of "splitpoints"
        state->split.splitnumber = wrapfiles+1;

        splt_c_put_info_message_to_client(state, _(" Total files: %d\n"),wrapfiles);
        
        //we do the dewrap
        for (i=0; i<wrapfiles; i++)
        {
          if (!splt_t_split_is_canceled(state))
          {
            //we put the current file to split
            splt_t_set_current_split(state, i+1);

            //if the first time, we get the begin,
            //otherwise the begin will be the end of the previous
            if (i==0)
            {
              //we get the begin wrap
              if (mp3wrap)
              {
                unsigned long w;
                if (splt_io_get_word (mp3state->file_input, 0, SEEK_CUR, &w)==-1)
                {
                  splt_e_set_error_data(state,file_to_dewrap);
                  *error = SPLT_DEWRAP_ERR_FILE_DAMAGED_INCOMPLETE;
                  return;
                }
                begin = (off_t) (w + id3offset);
              }

              //we get the begin wrap
              if (albumwrap)
              {
                if (fseeko (mp3state->file_input,
                      (off_t) SPLT_MP3_ABWINDEXOFFSET, SEEK_SET)==-1)
                {
                  splt_e_set_error_data(state,file_to_dewrap);
                  *error = SPLT_DEWRAP_ERR_FILE_DAMAGED_INCOMPLETE;
                  return;
                }
                j = 0;
                while ((c=fgetc(mp3state->file_input))!='[')
                  if (j++ > 32) 
                  {
                    splt_e_set_error_data(state,file_to_dewrap);
                    *error = SPLT_DEWRAP_ERR_FILE_DAMAGED_INCOMPLETE;
                    return;
                  }
                if (fseeko(mp3state->file_input, (off_t) 3, SEEK_CUR)==-1)
                {
                  splt_e_set_error_data(state,file_to_dewrap);
                  *error = SPLT_DEWRAP_ERR_FILE_DAMAGED_INCOMPLETE;
                  return;
                }
                j = 0;
                while ((j<2048) && ((c = fgetc(mp3state->file_input))!='['))
                  if (c!='.') junk[j++] = c;
                  else k = j;
                junk[j] = '\0';
                begin = (off_t) atol (junk);
                k = j - k;
                if (k<4)
                {
                  for (j=0; j<(4-k); j++)
                  {
                    begin = begin * 10;
                  }
                }
              }
            }
            else 
            {
              begin = end;
            }

            //we get the end and checkings..
            if (mp3wrap)
            {
              unsigned long w;
              j = 0;
              do
              {
                c = fgetc(mp3state->file_input);
                //for files wrapped using windows
                if (c==SPLT_NDIRCHAR)
                {
                  c=SPLT_DIRCHAR;
                }
                filename[j++] = c;
              }  while ((c!=0x00)&&(j<2048));

              if (splt_io_get_word (mp3state->file_input, 0, SEEK_CUR, &w) == -1)
              {
                splt_e_set_error_data(state,file_to_dewrap);
                *error = SPLT_DEWRAP_ERR_FILE_DAMAGED_INCOMPLETE;
                return;
              }

              end = (off_t) (w + id3offset);

              //create output directories for the wrapped files
              //if we don't list only and if we don't have an output
              //directory
              if (!listonly && (!dir || dir[0] == '\0'))
              {
                memset(junk, 0x00, 2048);
                char *ptr = filename;
                while (((ptr = strchr(ptr, SPLT_DIRCHAR))!=NULL)&&((ptr-filename)<2048))
                {
                  ptr++;
                  strncpy(junk, filename, ptr-filename);
                  if (! splt_io_check_if_directory(junk))
                  {
                    if ((splt_io_mkdir(state, junk)) == -1)
                    {
                      *error = SPLT_ERROR_CANNOT_CREATE_DIRECTORY;
                      splt_e_set_strerror_msg_with_data(state, junk);
                      return;
                    }
                  }
                }
              }
            }

            //we get the end wrap point for albumwrap and
            //checkings.. 
            if (albumwrap)
            {
              if (i<wrapfiles-1)
              {
                if (fseeko (mp3state->file_input, 
                      (off_t) (SPLT_MP3_ABWINDEXOFFSET + (i * SPLT_MP3_ABWLEN)), SEEK_SET)==-1)
                {
                  splt_e_set_error_data(state,file_to_dewrap);
                  *error = SPLT_DEWRAP_ERR_FILE_DAMAGED_INCOMPLETE;
                  return;
                }
                j = 0;
                while ((j<2048) && ((c = fgetc(mp3state->file_input))!='['))
                  if (c!='.') junk[j++] = c;
                  else k = j;
                junk[j] = '\0';
                end = (off_t) atol (junk);
                k = j - k;
                if (k<4)
                  for (j=0; j<(4-k); j++)
                    end = end * 10;
                end += begin;
              }
              else end = len;

              if (fseeko (mp3state->file_input, 
                    (off_t) (SPLT_MP3_ABWINDEXOFFSET + (i*SPLT_MP3_ABWLEN)), SEEK_SET)==-1)
              {
                splt_e_set_error_data(state,file_to_dewrap);
                *error = SPLT_DEWRAP_ERR_FILE_DAMAGED_INCOMPLETE;
              }
              j = 0;
              while ((c=fgetc(mp3state->file_input))!='[')
                if (j++ > 32) 
                {
                  splt_e_set_error_data(state,file_to_dewrap);
                  *error = SPLT_DEWRAP_ERR_FILE_DAMAGED_INCOMPLETE;
                  return;
                }
              if (fseeko (mp3state->file_input, (off_t) 3, SEEK_CUR)==-1)
              {
                splt_e_set_error_data(state,file_to_dewrap);
                *error = SPLT_DEWRAP_ERR_FILE_DAMAGED_INCOMPLETE;
                return;
              }
              j = 0;
              while ((c=fgetc(mp3state->file_input))!='[')
                if (j++ > 32) 
                {
                  splt_e_set_error_data(state,file_to_dewrap);
                  *error = SPLT_DEWRAP_ERR_FILE_DAMAGED_INCOMPLETE;
                  return;
                }
              if (fseeko(mp3state->file_input, (off_t) 3, SEEK_CUR)==-1)
              {
                splt_e_set_error_data(state,file_to_dewrap);
                *error = SPLT_DEWRAP_ERR_FILE_DAMAGED_INCOMPLETE;
                return;
              }
              j = 0;
              while (j<=400)
                filename[j++] = fgetc(mp3state->file_input);
              for (j=400; j>0; j--) {
                if (filename[j]==0x20)
                  filename[j]='\0';
                else break;
              }
              filename[j+1] = '\0';
            }

            splt_d_print_debug(state,"Found the file _%s_\n", filename);
            splt_d_print_debug(state,"Cut the dirchar");

            //we cut the .DIRCHAR before the filename
            char str_temp[4];
            snprintf(str_temp,4,"%c%c",'.',SPLT_DIRCHAR);
            if (strstr(filename,str_temp) != NULL)
            {
              char *filename2 = strdup(filename);
              if (!filename2)
              {
                *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
                return;
              }
              else
              {
                snprintf(filename,2048, "%s", filename2+2);
                free(filename2);
                filename2 = NULL;
              }
            }

            if (feof(mp3state->file_input)) 
            {
              splt_e_set_error_data(state,file_to_dewrap);
              *error = SPLT_DEWRAP_ERR_FILE_DAMAGED_INCOMPLETE;
              return;
            }

            //if we only list the contents
            //we put the files in the wrap_files
            if (listonly)
            {
              splt_d_print_debug(state,"Only list wrapped files\n");

              int put_file_error = SPLT_OK;
              put_file_error = splt_w_wrap_put_file(state, wrapfiles, i, filename);
              if (put_file_error != SPLT_OK)
              {
                *error = put_file_error;
                return;
              }
            }
            //if we split the file
            //we split from begin to end calculated previously
            else
            {
              splt_d_print_debug(state,"Split wrapped file\n");

              int ret = 0;
              //if we have an output directory
              //-try to create it
              if (dir && (dir[0] != '\0'))
              {
                char temp[2048] = { '\0' };
                strncpy(temp, filename, 2048);
                char *ptr = temp;
                //if we have an output dir, cut directory path from filename
                if ((ptr = strrchr(temp,SPLT_DIRCHAR)) == NULL)
                {
                  ptr = temp;
                }
                else
                {
                  if (ptr-temp > 0)
                  {
                    ptr++;
                  }
                }
                //if dir == .DIRCHAR
                if (strcmp(dir,str_temp) == 0)
                {
                  snprintf(filename, 2048,"%s%s", dir, ptr);
                }
                else
                {
                  if (dir[strlen(dir)-1] == SPLT_DIRCHAR)
                  {
                    snprintf(filename, 2048,"%s%s", dir, ptr);
                  }
                  else
                  {
                    snprintf(filename, 2048,"%s%c%s", dir, SPLT_DIRCHAR, ptr);
                  }
                }
                splt_d_print_debug(state,"wrap dir _%s_\n", dir);
                splt_d_print_debug(state,"wrap after dir _%s_\n", ptr);
              }

              //free xingbuffer
              if (mp3state->mp3file.xingbuffer)
              {
                free(mp3state->mp3file.xingbuffer);
                mp3state->mp3file.xingbuffer = NULL;
              }
              mp3state->mp3file.xing = 0;

              int append_err = SPLT_OK;
              append_err = splt_sp_append_splitpoint(state,0,
                  splt_su_get_fname_without_path(filename), SPLT_SPLITPOINT);
              if (append_err != SPLT_OK)
              {
                *error = append_err;
                return;
              }

              //cut extension
              int cut_err = splt_sp_cut_splitpoint_extension(state,i);
              if (cut_err != SPLT_OK)
              {
                *error = cut_err;
                return;
              }

              //do the real wrap split
              ret = splt_mp3_simple_split(state, filename, begin, end,
                  SPLT_FALSE, SPLT_FALSE);

              //if we could split put the split file
              if (ret >= 0)
              {
                ret = splt_c_put_split_file(state, filename);
                if (ret < 0) { *error = ret; }
              }
              else
              {
                *error = ret;
              }
            }
          }
        }
      }
      else
      {
        *error = SPLT_DEWRAP_ERR_FILE_NOT_WRAPED_DAMAGED;
        return;
      }
    }
  }
}

//! Initialize this plugin
void splt_mp3_init(splt_state *state, int *error)
{
  FILE *file_input = NULL;
  char *filename = splt_t_get_filename_to_split(state);

  state->syncerrors = 0;

  //if we can open the file
  if ((file_input = splt_mp3_open_file_read(state, filename, error)) != NULL)
  {
    splt_mp3_get_info(state, file_input, error);

    if (*error >= 0)
    {
      splt_mp3_state *mp3state = state->codec;
      mp3state->off = splt_o_get_float_option(state,SPLT_OPT_PARAM_OFFSET);

      //we initialise frames to 1
      if (splt_t_get_total_time(state) > 0)
      {
        mp3state->frames = 1;
      }
    }
  }
}

/*! 
\defgroup PluginAPI_MP3 The MP3 plugin's API

@{
*/

/*! Plugin API: returns the plugin infos (name, version, extension)

alloced data in splt_plugin_info will be freed by splt_t_state_free()
at the end of the program 
*/

void splt_pl_set_plugin_info(splt_plugin_info *info, int *error)
{
  float plugin_version = 1.0;

  //set plugin version
  info->version = plugin_version;

  //set plugin name
  info->name = malloc(sizeof(char) * 40);
  if (info->name != NULL)
  {
    snprintf(info->name, 39, "mp3 (libmad)");
  }
  else
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return;
  }

  //set plugin extension
  info->extension = malloc(sizeof(char) * (strlen(SPLT_MP3EXT)+2));
  if (info->extension != NULL)
  {
    snprintf(info->extension, strlen(SPLT_MP3EXT)+1, SPLT_MP3EXT);
  }
  else
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return;
  }

  info->upper_extension = splt_su_convert(info->extension, SPLT_TO_UPPERCASE, error);
}

//! Plugin API: Initialize this plugin
void splt_pl_init(splt_state *state, int *error)
{
  if (splt_io_input_is_stdin(state))
  {
    char *filename = splt_t_get_filename_to_split(state);
    if (filename[1] == '\0')
    {
      splt_c_put_info_message_to_client(state, 
          _(" warning: stdin '-' is supposed to be mp3 stream.\n"));
    }
  }

  splt_mp3_init(state, error);
}

//! Plugin API: Uninitialize this plugin
void splt_pl_end(splt_state *state, int *error)
{
  //put infos about the frames processed and the number of sync errors
  //ONLY if framemode
  if ((splt_o_get_int_option(state, SPLT_OPT_SPLIT_MODE) != SPLT_OPTION_SILENCE_MODE) 
      && (splt_o_get_int_option(state, SPLT_OPT_SPLIT_MODE) != SPLT_OPTION_TRIM_SILENCE_MODE)
      && (splt_o_get_int_option(state, SPLT_OPT_SPLIT_MODE) != SPLT_OPTION_ERROR_MODE)
      && (splt_o_get_int_option(state, SPLT_OPT_SPLIT_MODE) != SPLT_OPTION_WRAP_MODE))
  {
    if (splt_o_get_int_option(state, SPLT_OPT_FRAME_MODE))
    {
      if (*error >= 0)
      {
        splt_mp3_state *mp3state = state->codec;
        //-if we don't save the end point, the ->frames are set to 1 at the
        //end of the split
        if (mp3state->frames != 1)
        {
          splt_c_put_info_message_to_client(state, 
              _(" Processed %lu frames - Sync errors: %lu\n"),
              mp3state->frames, state->syncerrors);
        }
      }
    }
  }
  splt_mp3_end(state, error);
}

//! Plugin API: check if file can be handled by this plugin
int splt_pl_check_plugin_is_for_file(splt_state *state, int *error)
{
  char *filename = splt_t_get_filename_to_split(state);

  if (filename != NULL && ((strcmp(filename,"-") == 0) ||
      (strcmp(filename,"m-") == 0)))
  {
    return SPLT_TRUE;
  }

  int is_mp3 = SPLT_FALSE;

  splt_o_lock_messages(state);
  splt_mp3_init(state, error);
  splt_o_unlock_messages(state);
  if (*error >= 0)
  {
    splt_mp3_state *mp3state = state->codec;
    if (mp3state)
    {
      is_mp3 = SPLT_TRUE;
    }
  }
  splt_mp3_end(state, error);
 
  return is_mp3;
}

//! Plugin API: search for syncerrors
void splt_pl_search_syncerrors(splt_state *state, int *error)
{
  //we detect sync errors
  splt_mp3_syncerror_search(state, error);
}

//! Plugin API: get wrap files or dewrap
void splt_pl_dewrap(splt_state *state, int listonly, const char *dir, int *error)
{
  splt_w_wrap_free(state);
  splt_mp3_dewrap(listonly, dir, error, state);
}

double splt_pl_split(splt_state *state, const char *final_fname,
    double begin_point, double end_point, int *error, int save_end_point)
{
  return splt_mp3_split(final_fname, state, begin_point, end_point, error, save_end_point);
}

//! Plugin API: Output a portion of the file
int splt_pl_simple_split(splt_state *state, char *output_fname, off_t begin, off_t end)
{
  int error = SPLT_OK;

  splt_mp3_state *mp3state = state->codec;

  //we initialise frames to 1
  if (splt_t_get_total_time(state) > 0)
  {
    mp3state->frames = 1;
  }

  //effective mp3 split
  error = splt_mp3_simple_split(state, output_fname, begin, end,
      SPLT_FALSE, SPLT_FALSE);

  return error;
}

//! Plugin API: Scan for silence
int splt_pl_scan_silence(splt_state *state, int *error)
{
  float offset = splt_o_get_float_option(state,SPLT_OPT_PARAM_OFFSET);
  float threshold = splt_o_get_float_option(state, SPLT_OPT_PARAM_THRESHOLD);
  float min_length = splt_o_get_float_option(state, SPLT_OPT_PARAM_MIN_LENGTH);

  splt_mp3_state *mp3state = state->codec;
  mp3state->off = offset;

  int found = splt_mp3_scan_silence(state, mp3state->mp3file.firsthead.ptr, 0,
      threshold, min_length, 1, error, splt_scan_silence_processor);
  if (*error < 0) { return -1; }

  return found;
}

//! Plugin API: Scan trim using silence
int splt_pl_scan_trim_silence(splt_state *state, int *error)
{
  float threshold = splt_o_get_float_option(state, SPLT_OPT_PARAM_THRESHOLD);

  splt_mp3_state *mp3state = state->codec;

  int found = splt_mp3_scan_silence(state, mp3state->mp3file.firsthead.ptr, 0,
      threshold, 0, 1, error, splt_trim_silence_processor);
  if (*error < 0) { return -1; }

  return found;
}

//! Plugin API: Read the original Tags from the file
void splt_pl_set_original_tags(splt_state *state, int *error)
{
  splt_d_print_debug(state, "Getting original tags ...");
#ifndef NO_ID3TAG
  splt_d_print_debug(state, "Taking original ID3 tags from file using libid3tag ...\n");
  splt_mp3_get_original_tags(splt_t_get_filename_to_split(state), state, error);
#else
  splt_d_print_debug(state, "Warning ! NO_ID3TAG");
  //splt_e_error(SPLT_IERROR_SET_ORIGINAL_TAGS,__func__, 0, NULL);
#endif
}

void splt_pl_clear_original_tags(splt_original_tags *original_tags)
{
#ifndef NO_ID3TAG
  tag_bytes_and_size *bytes_and_size = (tag_bytes_and_size *) original_tags->all_original_tags;

  if (!bytes_and_size) {
    return;
  }

  if (bytes_and_size->tag_bytes)
  {
    free(bytes_and_size->tag_bytes);
    bytes_and_size->tag_bytes = NULL;
  }

  bytes_and_size->tag_length = 0;

  free(original_tags->all_original_tags);
  original_tags->all_original_tags = NULL;
#endif
}

//@}
