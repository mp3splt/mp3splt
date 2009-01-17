/**********************************************************
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2008 Alexandru Munteanu - io_fx@yahoo.fr
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

#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <math.h>
#include <ctype.h>

#ifdef __WIN32__
#include <io.h>
#include <fcntl.h>
#endif

#include "mp3.h"

/****************************/
/* mp3 constants */

static const char *splt_mp3_chan[] =
{
	"Mono",
	"Dual Mono",
	"Joint Stereo",
	"Stereo",
	"?"
};

//layer, bitrate..
static const int splt_mp3_tabsel_123[2][3][16] = {
  { {128,32,64,96,128,160,192,224,256,288,320,352,384,416,448,},
    {128,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,},
    {128,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320,} },

  { {128,32,48,56,64,80,96,112,128,144,160,176,192,224,256,},
    {128,8,16,24,32,40,48,56,64,80,96,112,128,144,160,},
    {128,8,16,24,32,40,48,56,64,80,96,112,128,144,160,} }
};

//categories of mp3 songs
static const char splt_mp3_id3v1_categories[SPLT_MP3_GENRENUM][25] = {
  {"Blues"}, {"Classic Rock"}, {"Country"}, {"Dance"}, 
  {"Disco"},{"Funk"},{"Grunge"},{"Hip-Hop"},{"Jazz"},
  {"Metal"},{"New Age"},{"Oldies"}, {"Other"}, {"Pop"},
  {"R&B"}, {"Rap"}, {"Reggae"}, {"Rock"}, {"Techno"},
  {"Industrial"}, {"Alternative"}, {"Ska"}, {"Death metal"},
  {"Pranks"}, {"Soundtrack"}, {"Euro-Techno"},
  {"Ambient"}, {"Trip-hop"}, {"Vocal"}, {"Jazz+Funk"},
  {"Fusion"}, {"Trance"}, {"Classical"}, {"Instrumental"},
  {"Acid"}, {"House"}, {"Game"}, {"Sound clip"}, {"Gospel"},
  {"Noise"}, {"Alt. Rock"}, {"Bass"}, {"Soul"}, {"Punk"}, 
  {"Space"}, {"Meditative"}, {"Instrumental pop"}, 
  {"Instrumental rock"}, {"Ethnic"}, {"Gothic"},{"Darkwave"},
  {"Techno-Industrial"},{"Electronic"},{"Pop-Folk"},{"Eurodance"},
  {"Dream"},{"Southern Rock"},{"Comedy"}, {"Cult"},{"Gangsta"},
  {"Top 40"},{"Christian Rap"},{"Pop/Funk"}, {"Jungle"},
  {"Native American"},{"Cabaret"},{"New Wave"}, {"Psychedelic"},
  {"Rave"},{"Showtunes"},{"Trailer"}, {"Lo-Fi"},{"Tribal"},
  {"Acid Punk"},{"Acid Jazz"}, {"Polka"}, {"Retro"},
  {"Musical"},{"Rock & Roll"},{"Hard Rock"}, {"misc"}, {"misc"},
};

static const char unsigned splt_mp3_id3genre[SPLT_MP3_GENRENUM] = 
{ 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 
  0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F,
  0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F,
  0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B, 0x3C, 0x3D, 0x3E, 0x3F,
  0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F,
  0xFF };

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

//-filename must not be null; if filename is NULL, then this plugin should
//not have been detected
//-returns NULL if error
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
    //we open the file
    file_input = splt_u_fopen(filename, "rb");
    if (file_input == NULL)
    {
      splt_t_set_strerror_msg(state);
      splt_t_set_error_data(state,filename);
      *error = SPLT_ERROR_CANNOT_OPEN_FILE;
    }
  }

  return file_input;
}

/****************************/
/* CRC functions */

static unsigned long splt_mp3_c_crc(splt_state *state,
    FILE *in, off_t begin, off_t end, int *error)
{
  register unsigned long crc;
  int c;

  crc = 0xFFFFFFFF;

  if (fseeko(in, begin, SEEK_SET) == -1)
  {
    splt_t_set_strerror_msg(state);
    splt_t_set_error_data(state,splt_t_get_filename_to_split(state));
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

/****************************/
/* mp3 utils */

//does nothing important for libmp3splt
//review this..
static void splt_mp3_checksync (splt_mp3_state *mp3state)
{
  //char junk[32];
  //fprintf(stderr, "\nWarning: Too many sync errors! This may not be a mp3 file. Continue? (y/n) ");
  //fgets(junk, 31, stdin);
  //if (junk[0]=='y')

  //we don't use user interactivity in a library
  //always continue
  mp3state->syncdetect = 0;

  //else error("Aborted.",125);
}

//calculates bitrate
static int splt_mp3_c_bitrate (unsigned long head)
{
  if ((head & 0xffe00000) != 0xffe00000) return 0;
  if (!((head>>17)&3)) return 0;
  if (((head>>12)&0xf) == 0xf) return 0;
  if (!((head >> 12) & 0xf)) return 0;
  if (((head>>10)&0x3) == 0x3 ) return 0;
  if (((head >> 19) & 1)==1 && ((head>>17)&3)==3 && 
      ((head>>16)&1)==1) return 0;
  if ((head & 0xffff0000) == 0xfffe0000) return 0;

  return ((head>>12)&0xf);
}

//make mp3 header bitrate, padding, offset, framesize
static struct splt_header splt_mp3_makehead (unsigned long headword, 
    struct splt_mp3 mp3f, struct splt_header head, off_t ptr)
{
  head.ptr = ptr;
  head.bitrate = splt_mp3_tabsel_123[1 - mp3f.mpgid][mp3f.layer-1][splt_mp3_c_bitrate(headword)];
  head.padding = ((headword>>9)&0x1);
  head.framesize = (head.bitrate*144000)/
    (mp3f.freq<<(1 - mp3f.mpgid)) + head.padding;

  return head;
}

//finds first header from start_pos. Returns -1 if no header is found
static off_t splt_mp3_findhead (splt_mp3_state *mp3state, off_t start)
{
  if (splt_u_getword(mp3state->file_input, 
        start, SEEK_SET, &mp3state->headw) == -1)
  {
    return -1;
  }
  if (feof(mp3state->file_input)) 
  {
    return -1;
  }
  while (!(splt_mp3_c_bitrate(mp3state->headw)))
  {
    if (feof(mp3state->file_input)) 
    {
      return -1;
    }
    mp3state->headw <<= 8;
    mp3state->headw |= fgetc(mp3state->file_input);
    start++;
  }

  return start;
}

// Finds first valid header from start. Will work with high probabilty, i hope :)
static off_t splt_mp3_findvalidhead (splt_mp3_state *mp3state, off_t start)
{
  off_t begin;
  struct splt_header h;

  begin = splt_mp3_findhead(mp3state, start);

  do {
    start = begin;
    if (start == -1) 
    {
      break;
    }
    h = splt_mp3_makehead (mp3state->headw, mp3state->mp3file, h, start);
    begin = splt_mp3_findhead(mp3state, (start + 1));
  } while (begin!=(start + h.framesize));

  return start;
}

//finds xing info offset and returns it?
static int splt_mp3_xing_info_off(splt_mp3_state *mp3state)
{
  unsigned long headw = 0;
  int i;

  for (i=0; i<mp3state->mp3file.xing; i++)
  {
    if ((headw == SPLT_MP3_XING_MAGIC) || 
        (headw == SPLT_MP3_INFO_MAGIC)) // "Xing" or "Info"
    {
      return i;
    }
    headw <<= 8;
    headw |= mp3state->mp3file.xingbuffer[i];
  }

  return 0;
}

//get a frame
//-returns a negative value if error
static int splt_mp3_get_frame(splt_mp3_state *mp3state)
{
  if(mp3state->stream.buffer==NULL || 
      mp3state->stream.error==MAD_ERROR_BUFLEN)
  {
    size_t readSize, remaining;
    unsigned char *readStart;

    if (feof(mp3state->file_input))
    {
      return -2;
    }

    if(mp3state->stream.next_frame!=NULL)
    {
      remaining = mp3state->stream.bufend - mp3state->stream.next_frame;
      memmove(mp3state->inputBuffer, mp3state->stream.next_frame, remaining);
      readStart = mp3state->inputBuffer + remaining;
      readSize = SPLT_MAD_BSIZE - remaining;
    }
    else
    {
      readSize = SPLT_MAD_BSIZE;
      readStart=mp3state->inputBuffer;
      remaining=0;
    }

    readSize=fread(readStart, 1, readSize, mp3state->file_input);
    if (readSize <= 0)
    {
      return -2;
    }

    mp3state->buf_len = readSize + remaining;
    mp3state->bytes += readSize;
    //does not set any error
    mad_stream_buffer(&mp3state->stream, mp3state->inputBuffer, 
        readSize+remaining);
  }

  //mad_frame_decode() returns -1 if error, 0 if no error
  return mad_frame_decode(&mp3state->frame,&mp3state->stream);
}

//used by mp3split and mp3_scan_silence
//gets a frame and checks for its validity
//returns 1 if ok, -1 if end of file, 0 if nothing ?
//and -3 if other error; the error will be set in the '*error' parameter
//sets the mp3state->data_ptr the pointer to the frame
//and the mp3state->data_len the length of the frame
static int splt_mp3_get_valid_frame(splt_state *state, int *error)
{
  splt_mp3_state *mp3state = state->codec;
  int ok = 0;
  do
  {
    int ret = splt_mp3_get_frame(mp3state);
    if(ret != 0)
    {
      if (ret == -2)
      {
        return -1;
      }
      if (mp3state->stream.error == MAD_ERROR_LOSTSYNC)
      {
        //syncerrors
        state->syncerrors++;
        if ((mp3state->syncdetect)&&
            (state->syncerrors>SPLT_MAXSYNC))
        {
          splt_mp3_checksync(mp3state);
        }
      }
      if(MAD_RECOVERABLE(mp3state->stream.error))
      {
        continue;
      }
      else
      {
        if(mp3state->stream.error==MAD_ERROR_BUFLEN)
        {
          continue;
        }
        else
        {
          splt_t_set_error_data(state, mad_stream_errorstr(&mp3state->stream));
          *error = SPLT_ERROR_PLUGIN_ERROR;
          return -3;
        }
      }
    }
    else
    {
      //the important stuff
      mp3state->data_ptr = (unsigned char *) mp3state->stream.this_frame;
      if (mp3state->stream.next_frame!=NULL)
      {
        mp3state->data_len = (long) (mp3state->stream.next_frame - mp3state->stream.this_frame);
      }
      ok = 1;
    }

  } while (!ok);

  return ok;
}

//search for ID3 v1 tag, "TAG" sequence
//if found returns offset of mp3 data
//-else returns 0
//-we don't check fseeko error
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

//check if there is a ID3v2. 
//if found, it returns offset of mp3 data.
//-else returns 0
//-we don't check fseeko error
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

//frees the splt_mp3_state structure,
//used in the splt_t_state_free() function
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

//returns the genre of the song, mp3splt used this in cddb search
static unsigned char splt_mp3_getgenre (const char *genre_string)
{
  int i;
  for (i=0; i< SPLT_MP3_GENRENUM; i++)
  {
    if (strncmp(genre_string, splt_mp3_id3v1_categories[i],
          strlen(genre_string))==0)
    {
      return splt_mp3_id3genre[i];
    }
  }

  return 0xFF;
}

#ifndef NO_ID3TAG

void put_id3_frame_in_tag_with_content(struct id3_tag *id, const char *frame_type,
    int field_number, const char *content, int *error)
{
  struct id3_frame *id_frame = NULL;
  id3_ucs4_t *field_content = NULL;
  union id3_field *id_field = NULL;

  if (content)
  {
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
    //the comment is a full string : field number 3
    else if (field_number == 3)
    {
      if (id3_field_setfullstring(id_field, field_content) == -1)
      {
        goto error;
      }
    }
    if (field_content)
    {
      free(field_content);
      field_content = NULL;
    }
    if (id3_tag_attachframe(id, id_frame) == -1)
    {
      goto error;
    }
    id3_frame_delete(id_frame);
  }

  return;

error:
  *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
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

static char *splt_mp3_id3tag(const char *title, const char *artist,
    const char *album, const char *year, unsigned char genre, 
    const char *comment, int track, int *error, unsigned long *number_of_bytes,
    int tags_version)
{
  struct id3_tag *id = id3_tag_new();

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

  put_id3_frame_in_tag_with_content(id, ID3_FRAME_TITLE, 1, title, error);
  if (*error < 0) { goto error; }
  put_id3_frame_in_tag_with_content(id, ID3_FRAME_ARTIST, 1, artist, error);
  if (*error < 0) { goto error; }
  put_id3_frame_in_tag_with_content(id, ID3_FRAME_ALBUM, 1, album, error);
  if (*error < 0) { goto error; }
  put_id3_frame_in_tag_with_content(id, ID3_FRAME_YEAR, 1, year, error);
  if (*error < 0) { goto error; }
  put_id3_frame_in_tag_with_content(id, ID3_FRAME_COMMENT, 3, comment, error);
  if (*error < 0) { goto error; }
  if (track != -INT_MAX)
  {
    char track_str[255] = { '\0' };
    snprintf(track_str,254,"%d",track);
    put_id3_frame_in_tag_with_content(id, ID3_FRAME_TRACK, 1, track_str, error);
  }
  if (*error < 0) { goto error; }
  put_id3_frame_in_tag_with_content(id, ID3_FRAME_GENRE, 1,
      splt_mp3_id3v1_categories[genre], error);
  if (*error < 0) { goto error; }

  //get the number of bytes needed for the tags
  bytes_length = id3_tag_render(id, NULL);

  if (bytes_length > 0)
  {
    //allocate memory for the tags
    bytes = malloc(sizeof(id3_byte_t) * bytes_length);
    if (!bytes)
    {
      goto error;
    }
    memset(bytes, '\0', sizeof(id3_byte_t) * bytes_length);

    bytes_length = id3_tag_render(id, bytes);

    id3_tag_delete(id);

    *number_of_bytes = (unsigned long) bytes_length;
  }

  return (char *) bytes;

error:
  *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  id3_tag_delete(id);
  *number_of_bytes = 0;
  if (bytes)
  {
    free(bytes);
    bytes = NULL;
  }
  return NULL;
}
#endif

#ifdef NO_ID3TAG
//returns a id3v1 buffer as string
//return must be freed
//-returns NULL if error
static char *splt_mp3_simple_id3v1(const char *title, const char *artist,
    const char *album, const char *year, unsigned char genre, 
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
    if (track != -INT_MAX)
    {
      if (track != 0x00)
      {
        id[j-1] = (char) track;
      }
    }      
    id[j] = (char) genre;
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

//returns a id3v2 or id3v1 buffer as string
//return must be freed
//-returns NULL if error
static char *splt_mp3_get_id3_tags(splt_state *state,
    const char *title, const char *artist,
    const char *album, const char *year, unsigned char genre, 
    const char *comment, int track, int *error,
    unsigned long *number_of_bytes, int *version)
{
  char *id = NULL;

#ifdef NO_ID3TAG
  id = splt_mp3_simple_id3v1(title, artist, album, year, genre, comment, track,
      error, number_of_bytes);
  *version = 1;
#else
  int original_tags_version = state->original_tags.tags_version;
  int force_tags_version = splt_t_get_int_option(state, SPLT_OPT_FORCE_TAGS_VERSION);

  int output_tags_version = original_tags_version;
  if ((force_tags_version == 1) || (force_tags_version == 2))
  {
    output_tags_version = force_tags_version;
  }

  if (output_tags_version == 1)
  {
    id = splt_mp3_id3tag(title, artist, album, year, genre, comment, track,
        error, number_of_bytes, 1);
    *version = 1;
  }
  else
  {
    id = splt_mp3_id3tag(title, artist, album, year, genre, comment, track,
        error, number_of_bytes, 2);
    *version = 2;
  }
#endif

  return id;
}

#ifndef NO_ID3TAG
//puts a original field on id3 conforming to frame_type
static int splt_mp3_put_original_id3_frame(splt_state *state,
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
        int length = strlen((char *)tag_value);
        switch (id_type)
        {
          case SPLT_MP3_ID3_ALBUM:
            err = splt_t_set_original_tags_field(state,SPLT_TAGS_ALBUM,
                0,(char *)tag_value,0x0,length);
            break;
          case SPLT_MP3_ID3_ARTIST:
            err = splt_t_set_original_tags_field(state,SPLT_TAGS_ARTIST,
                0,(char *)tag_value,0x0,length);
            break;
          case SPLT_MP3_ID3_TITLE:
            if (strcmp(frame_type,ID3_FRAME_TITLE) == 0)
            {
              err = splt_t_set_original_tags_field(state,SPLT_TAGS_TITLE,
                  0,(char *)tag_value,0x0,length);
            }
            break;
          case SPLT_MP3_ID3_YEAR:
            err = splt_t_set_original_tags_field(state,SPLT_TAGS_YEAR,
                0,(char *)tag_value,0x0,length);
            break;
          case SPLT_MP3_ID3_TRACK:
            err = splt_t_set_original_tags_field(state,SPLT_TAGS_TRACK,
                atof((char*)tag_value), NULL,0x0,0);
            break;
          case SPLT_MP3_ID3_COMMENT:
            err = splt_t_set_original_tags_field(state,SPLT_TAGS_COMMENT,
                0,(char*)tag_value,0x0,length);
            break;
          case SPLT_MP3_ID3_GENRE:
            err = splt_t_set_original_tags_field(state,SPLT_TAGS_GENRE,
                0,NULL,splt_mp3_getgenre((char *)tag_value),0);

            int number = 80;
            number = atoi((char *)tag_value);
            //if we have a number returned by tag_value
            if ((number != 0) &&
                (state->original_tags.genre == 0xFF))
            {
              err = splt_t_set_original_tags_field(state,SPLT_TAGS_GENRE,
                  0,NULL,number,0);
            }
            //if we have 0 returned
            if (strcmp((char*)tag_value, "0") == 0)
            {
              err = splt_t_set_original_tags_field(state,SPLT_TAGS_GENRE,
                  0,NULL,12,0);
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

static id3_byte_t *get_id3v2_tag_bytes(FILE *file, id3_length_t *length)
{
  id3_byte_t *bytes = NULL;
  *length = 0;

  off_t id3v2_end_offset = splt_mp3_getid3v2_end_offset(file, 0);

  if (id3v2_end_offset != 0)
  {
    unsigned long id3v2_size = (unsigned long) id3v2_end_offset + 10;
    bytes = malloc(sizeof(unsigned char) * id3v2_size);

    if (! bytes)
    {
      return NULL;
    }

    rewind(file);

    //read the whole id3v2 tags
    if (fread(bytes, 1, id3v2_size, file) != id3v2_size)
    {
      if (bytes)
      {
        free(bytes);
        bytes = NULL;
      }

      return NULL;
    }

    *length = id3v2_size;
  }

  return bytes;
}

static id3_byte_t *get_id3v1_tag_bytes(FILE *file, id3_length_t *length)
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

static id3_byte_t *get_id3_tag_bytes(splt_state *state,const char *filename,
    id3_length_t *length, int *error, int *tags_version)
{
  *length = 0;
  id3_byte_t *bytes = NULL;

  FILE *file = splt_u_fopen(filename, "r");

  if (! file)
  {
    splt_t_set_strerror_msg(state);
    splt_t_set_error_data(state,filename);
    *error = SPLT_ERROR_CANNOT_OPEN_FILE;
    goto end;
  }
  else
  {
    bytes = get_id3v2_tag_bytes(file, length);
    *tags_version = 2;

    if (! bytes)
    {
      bytes = get_id3v1_tag_bytes(file, length);
      *tags_version = 1;
    }
  }

end:
  if (fclose(file) != 0)
  {
    if (bytes)
    {
      free(bytes);
      bytes = NULL;
    }
    return NULL;
  }

  return bytes;
}

//macro used only in the following function splt_mp3_get_original_tags
#define MP3_VERIFY_ERROR() \
if (err != SPLT_OK) \
{ \
*tag_error = err; \
goto end; \
};

//this function puts the original id3 tags if we have libid3tag enabled
//at compilation time
static void splt_mp3_get_original_tags(const char *filename, splt_state *state,
    int *tag_error)
{
  //we get the id3 from the original file using libid3tag
  struct id3_tag *id3tag = NULL;

  //get out the tags from the file; id3_file_open doesn't work with win32 utf16 filenames
  id3_length_t id3_tag_length = 0;
  int tags_version = 0;
  id3_byte_t *id3_tag_bytes = get_id3_tag_bytes(state, filename, &id3_tag_length,
      tag_error, &tags_version);

  if (*tag_error >= 0)
  {
    if (id3_tag_bytes)
    {
      id3tag = id3_tag_parse(id3_tag_bytes, id3_tag_length);

      if (id3tag)
      {
        int err = SPLT_OK;

        err = splt_t_set_original_tags_field(state,SPLT_TAGS_VERSION,
            tags_version, NULL, 0, 0);
        MP3_VERIFY_ERROR();
        err = splt_mp3_put_original_id3_frame(state,id3tag,ID3_FRAME_ARTIST,
            SPLT_MP3_ID3_ARTIST);
        MP3_VERIFY_ERROR();
        err = splt_mp3_put_original_id3_frame(state,id3tag,ID3_FRAME_ALBUM,
            SPLT_MP3_ID3_ALBUM);
        MP3_VERIFY_ERROR();
        err = splt_mp3_put_original_id3_frame(state,id3tag,ID3_FRAME_TITLE,
            SPLT_MP3_ID3_TITLE);
        MP3_VERIFY_ERROR();
        err = splt_mp3_put_original_id3_frame(state,id3tag,ID3_FRAME_YEAR,
            SPLT_MP3_ID3_YEAR);
        MP3_VERIFY_ERROR();
        err = splt_mp3_put_original_id3_frame(state,id3tag,ID3_FRAME_GENRE,
            SPLT_MP3_ID3_GENRE);
        MP3_VERIFY_ERROR();
        err = splt_mp3_put_original_id3_frame(state,id3tag,ID3_FRAME_COMMENT,
            SPLT_MP3_ID3_COMMENT);
        MP3_VERIFY_ERROR();
        err = splt_mp3_put_original_id3_frame(state,id3tag,ID3_FRAME_TRACK,
            SPLT_MP3_ID3_TRACK);
        MP3_VERIFY_ERROR();

        free(id3tag);
        id3tag = NULL;
      }
    }

end: 
    ;
  }

  if (id3_tag_bytes)
  {
    free(id3_tag_bytes);
    id3_tag_bytes = NULL;
  }
}
#endif

//put the song tags
//return a buffer containing the tags : must be freed
static char *splt_mp3_get_tags(const char *filename, splt_state *state, int *error,
    unsigned long *number_of_bytes, int *id3_version)
{
  char *id3_data = NULL;

  if (splt_t_get_int_option(state, SPLT_OPT_TAGS) == SPLT_TAGS_ORIGINAL_FILE)
  {
#ifndef NO_ID3TAG
    char *title = state->original_tags.title;
    char *artist = state->original_tags.artist;

    //only if we have the artist or the title
    if (((artist != NULL) && (artist[0] != '\0'))
        || ((title != NULL) && (title[0] != '\0')))
    {
      id3_data = splt_mp3_get_id3_tags(state,
          state->original_tags.title,
          state->original_tags.artist,
          state->original_tags.album,
          state->original_tags.year,
          state->original_tags.genre, 
          state->original_tags.comment,
          state->original_tags.track,
          error, number_of_bytes, id3_version);
    }
#else
    splt_u_error(SPLT_IERROR_SET_ORIGINAL_TAGS,__func__, 0, NULL);
#endif
  }
  else
  {
    if (splt_t_get_int_option(state,SPLT_OPT_TAGS) == SPLT_CURRENT_TAGS)
    {
      int current_split = splt_t_get_current_split_file_number(state) - 1;
      int old_current_split = current_split;

      //if we set all the tags like the x one
      int remaining_tags_like_x = splt_t_get_int_option(state,SPLT_OPT_ALL_REMAINING_TAGS_LIKE_X); 
      if ((current_split >= state->split.real_tagsnumber) &&
          (remaining_tags_like_x != -1))
      {
        current_split = remaining_tags_like_x;
      }

      //only if the tags exists for the current split
      if (splt_t_tags_exists(state,current_split))
      {
        char *title = NULL;
        char *artist = NULL;
        title = splt_t_get_tags_char_field(state, current_split, SPLT_TAGS_TITLE);
        artist = splt_t_get_tags_char_field(state, current_split, SPLT_TAGS_ARTIST);

        splt_t_set_auto_increment_tracknumber_tag(state, old_current_split, current_split);

        //only if we have the artist or the title
        if (((artist != NULL) && (artist[0] != '\0'))
            || ((title != NULL) && (title[0] != '\0')))
        {
          int tags_number = 0;
          splt_tags *tags = splt_t_get_tags(state, &tags_number);

          int track = 0;
          if (tags[current_split].track > 0)
          {
            track = tags[current_split].track;
          }
          else
          {
            track = current_split+1;
          }

          if (splt_t_tags_exists(state,current_split))
          {
            id3_data = splt_mp3_get_id3_tags(state,
                tags[current_split].title,
                tags[current_split].artist,
                tags[current_split].album,
                tags[current_split].year,
                tags[current_split].genre,
                tags[current_split].comment,
                track, error, number_of_bytes, id3_version);
          }
        }
      }
    }
  }

  return id3_data;
}

//returns possible error
int splt_mp3_write_id3_tags(splt_state *state, FILE *file_output, const
    char *output_fname, int tags_version)
{
  const char *filename = splt_t_get_filename_to_split(state);
  unsigned long number_of_bytes = 0;
  int id3_version = 2;
  int error = SPLT_OK;

  char *id3_tags = splt_mp3_get_tags(filename, state, &error, &number_of_bytes, &id3_version);

  if ((error >= 0) && (id3_tags) && (number_of_bytes > 0))
  {
    if (id3_version == tags_version)
    {
      //id3v1
      if (id3_version == 1)
      {
        if (fseeko(file_output, splt_mp3_getid3v1_offset(file_output), SEEK_END)!=-1)
        {
          if (fwrite(id3_tags, 1, number_of_bytes, file_output) < number_of_bytes)
          {
            splt_t_set_error_data(state, output_fname);
            error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
            goto end;
          }
        }
        else
        {
          splt_t_set_strerror_msg(state);
          splt_t_set_error_data(state, output_fname);
          error = SPLT_ERROR_SEEKING_FILE;
          goto end;
        }
      }
      //id3v2
      else if (id3_version == 2)
      {
        if (fwrite(id3_tags, 1, number_of_bytes, file_output) < number_of_bytes)
        {
          splt_t_set_error_data(state, output_fname);
          error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
          goto end;
        }
      }
    }
  }

end:
  if (id3_tags)
  {
    free(id3_tags);
    id3_tags = NULL;
  }

  return error;
}


/****************************/
/* mp3 infos */

//puts in the state informations about mp3 file
//must be called before splt_mp3_split()
//enables framemode if xing header found 
//xing header is often associated with VBR (variable bit rate)
static splt_mp3_state *splt_mp3_info(FILE *file_input, splt_state *state,
    int framemode, int *error)
{
  splt_mp3_state *mp3state = state->codec;

  int prev = -1, len;

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
  mp3state->first = 1;
  mp3state->file_input = file_input;
  mp3state->framemode = framemode;
  mp3state->headw = 0;
  mp3state->mp3file.xing = 0;
  mp3state->mp3file.xing_offset = 0;
  mp3state->mp3file.xingbuffer = NULL;
  mp3state->mp3file.len = splt_u_flength(state, file_input, filename, error);
  if (error < 0)
  {
    return NULL;
  }
  splt_t_set_total_time(state,0);
  mp3state->data_ptr = NULL;
  mp3state->data_len = 0;
  mp3state->buf_len = 0;
  mp3state->bytes = 0;

  //we initialise the mad structures
  mad_stream_init(&mp3state->stream);
  mad_frame_init(&mp3state->frame);
  mad_synth_init(&mp3state->synth);

  mad_timer_reset(&mp3state->timer);
  /*mp3state->timer.seconds = 0;
  mp3state->timer.fraction= 0;*/

  //we read mp3 infos and set pointers to read the mp3 data
  do
  {
    int ret = splt_mp3_get_frame(mp3state);

    if (ret==-2)
    {
      splt_t_set_error_data(state,filename);
      *error = SPLT_ERROR_INVALID;
      goto function_end;
    }

    if ((prev == 0) && 
        ((ret == 0) || 
         (mp3state->stream.error==MAD_ERROR_BUFLEN)))
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

          mp3state->mp3file.xing = mp3state->data_len;

          if ((mp3state->mp3file.xingbuffer = 
                malloc(mp3state->mp3file.xing))==NULL)
          {
            *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
            goto function_end;
          }

          memcpy(mp3state->mp3file.xingbuffer, mp3state->data_ptr,
              mp3state->mp3file.xing);
          mp3state->mp3file.xing_offset = 
            splt_mp3_xing_info_off(mp3state);
          //set framemode true (because VBR)
          splt_t_set_int_option(state, SPLT_OPT_FRAME_MODE, SPLT_TRUE);
          mp3state->framemode = 1;
          //print message to client because frame mode enabled
          if (!splt_t_messages_locked(state))
          {
            if (!splt_t_get_iopt(state,SPLT_INTERNAL_FRAME_MODE_ENABLED))
            {
              splt_t_put_message_to_client(state, " info: frame mode enabled\n");
              splt_t_set_iopt(state,SPLT_INTERNAL_FRAME_MODE_ENABLED,SPLT_TRUE);
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
    splt_t_set_error_data(state,filename);
    *error = SPLT_ERROR_INVALID;
    goto function_end;
  }

  //we put useful infos in the state
  mp3state->mp3file.firsth = (off_t) (mp3state->bytes - len);
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
      long temp =
        ((mp3state->mp3file.len - mp3state->mp3file.firsth)
         / mp3state->mp3file.bitrate) * 100;
      splt_t_set_total_time(state, temp);
    }
  }

function_end:
  //we free memory allocated by mad_frame_decode(..)
  mad_stream_finish(&mp3state->stream);
  mad_frame_finish(&mp3state->frame);
  mad_synth_finish(&mp3state->synth);

  return mp3state;
}

static void splt_mp3_end(splt_state *state, int *error)
{
  splt_mp3_state *mp3state = state->codec;
  if (mp3state)
  {
    if (mp3state->file_input)
    {
      if (mp3state->file_input != stdin)
      {
        if (fclose(mp3state->file_input) != 0)
        {
          splt_t_set_strerror_msg(state);
          splt_t_set_error_data(state,
              splt_t_get_filename_to_split(state));
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

//gets the mp3 info and puts it in the state
static void splt_mp3_get_info(splt_state *state, FILE *file_input, int *error)
{
  //checks if valid mp3 file
  //before last argument, if framemode or not
  //last argument if we put messages to clients or not
  state->codec = splt_mp3_info(file_input, state,
        splt_t_get_int_option(state,SPLT_OPT_FRAME_MODE), error);
  //if error
  if ((*error < 0) || 
      (state->codec == NULL))
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
    if ((! splt_t_messages_locked(state)) &&
        (splt_t_get_int_option(state, SPLT_OPT_SPLIT_MODE) != SPLT_OPTION_WRAP_MODE) &&
        (splt_t_get_int_option(state, SPLT_OPT_SPLIT_MODE) != SPLT_OPTION_ERROR_MODE))
    {
      splt_mp3_state *mp3state = state->codec;
      struct splt_mp3 *mfile = &mp3state->mp3file;
      //codec infos
      char mpeg_infos[2048] = { '\0' };
      snprintf(mpeg_infos,2048, " info: MPEG %d Layer %d - %d Hz - %s", (2-mfile->mpgid), mfile->layer, mfile->freq, splt_mp3_chan[mfile->channels]);
      //frame mode or bitrate
      char frame_mode_infos[256] = { '\0' };
      if (mp3state->framemode)
      {
        if (splt_t_get_int_option(state, SPLT_OPT_INPUT_NOT_SEEKABLE))
        {
          snprintf(frame_mode_infos,256," - FRAME MODE NS");
        }
        else
        {
          snprintf(frame_mode_infos,256," - FRAME MODE");
        }
      }
      else 
      {
        snprintf(frame_mode_infos,256," - %d Kb/s",mfile->bitrate * SPLT_MP3_BYTE / 1000);
      }
      //total time
      char total_time[256] = { '\0' };
      int total_seconds = (int) splt_t_get_total_time(state) / 100;
      int minutes = total_seconds / 60;
      int seconds = total_seconds % 60;
      snprintf(total_time,256," - Total time: %dm.%02ds", minutes, seconds%60);
      //put all the infos together
      char all_infos[3072] = { '\0' };
      snprintf(all_infos,3071,"%s%s%s\n",mpeg_infos,frame_mode_infos,total_time);
      splt_t_put_message_to_client(state, all_infos);
    }
  }
}

/****************************/
/* mp3 scan for silence */

//used by mp3_scan_silence, and compare with threshold, returns 0 if
//silence spot > threshold, 1 otherwise
//-computes one frame
static int splt_mp3_silence(splt_mp3_state *mp3state, int channels, mad_fixed_t threshold)
{
  int i, j;
  mad_fixed_t sample;
  int silence = 1;

  for (j=0; j<channels; j++)
  {
    for(i=0; i<mp3state->synth.pcm.length; i++)
    {
      //get silence spot ?
      sample = mad_f_abs(mp3state->synth.pcm.samples[j][i]);
      mp3state->temp_level = mp3state->temp_level * 0.999 + sample * 0.001;

      if (sample > threshold)
      {
        silence = 0;
      }
    }
  }

  return silence;
}

//scan for silence
//-returns the number of silence points found
//and -1 if error; the error is set in the '*error' parameter
static int splt_mp3_scan_silence(splt_state *state, off_t begin, 
    unsigned long length, float threshold, 
    float min, short output, int *error)
{
  int len = 0, found = 0, shot;
  short first, flush = 0, stop = 0;
  unsigned long silence_begin = 0, silence_end = 0, time;
  unsigned long count = 0;
  off_t pos;
  mad_fixed_t th;

  splt_mp3_state *mp3state = state->codec;

  splt_t_put_progress_text(state,SPLT_PROGRESS_SCAN_SILENCE);

  pos = begin;
  th = mad_f_tofixed(splt_u_convertfromdB(threshold));

  //we seek to the begin
  if (fseeko(mp3state->file_input, begin, SEEK_SET)==-1)
  {
    splt_t_set_strerror_msg(state);
    splt_t_set_error_data(state, splt_t_get_filename_to_split(state));
    *error = SPLT_ERROR_SEEKING_FILE;
    return -1;
  }

  first = output;
  shot = SPLT_DEFAULTSHOT;

  //initialise mad stuff
  mad_stream_init(&mp3state->stream);
  mad_frame_init(&mp3state->frame);
  mad_synth_init(&mp3state->synth);

  mad_timer_reset(&mp3state->timer);
  /*mp3state->timer.seconds = 0;
  mp3state->timer.fraction= 0;*/

  mp3state->temp_level = 0.0;

  //we do the effective scan
  do
  {
    int mad_err = SPLT_OK;
    switch (splt_mp3_get_valid_frame(state, &mad_err))
    {
      case 1:
        //1 we have a valid frame
        //we get mad infos and put them in the mp3state
        mad_timer_add(&mp3state->timer, mp3state->frame.header.duration);
        mad_synth_frame(&mp3state->synth,&mp3state->frame);
        time = (unsigned long) mad_timer_count(mp3state->timer, MAD_UNITS_CENTISECONDS);

        if (length > 0)
        {
          if (time >= length)
          {
            flush = 1;
            stop = 1;
          }
        }

        if ((!flush) && (splt_mp3_silence(mp3state, MAD_NCHANNELS(&mp3state->frame.header), th)))
        {
          if (len == 0) silence_begin = time;
          if (first == 0) 
          {
            len++;
          }
          if (shot < SPLT_DEFAULTSHOT)
            shot+=2;
          silence_end = time;
        }
        else
        {
          if (len > SPLT_DEFAULTSILLEN)
          {
            if ((flush) || (shot <= 0))
            {
              double begin_position, end_position;
              begin_position = (double) (silence_begin / 100.f);
              end_position = (double) (silence_end / 100.f);
              len = (int) (silence_end - silence_begin);

              if ((end_position - begin_position - min) >= 0.f)
              {
                if (splt_t_ssplit_new(&state->silence_list, begin_position, end_position,
                      len, error) == -1)
                {
                  stop = 1;
                  found = -1;
                  break;
                }
                found++;
              }

              len = 0;
              shot = SPLT_DEFAULTSHOT;
            }
          }
          else 
          {
            len = 0;
          }

          if ((first) && (shot <= 0))
          {
            first = 0;
          }

          if (shot > 0) 
          {
            shot--;
          }
        }

        if (mp3state->mp3file.len > 0)
        {
          pos = ftello(mp3state->file_input);

          if (count++ % 10 == 0)
          {
            float level = splt_u_convert2dB(mad_f_todouble(mp3state->temp_level));
            if (state->split.get_silence_level)
            {
              state->split.get_silence_level(time, level, state->split.silence_level_client_data);
            }
            state->split.p_bar->silence_db_level = level;
            state->split.p_bar->silence_found_tracks = found;
          }

          //if we don't have silence split,
          //put the 1/4 of progress
          if (splt_t_get_int_option(state, SPLT_OPT_SPLIT_MODE) != 
              SPLT_OPTION_SILENCE_MODE)
          {
            splt_t_update_progress(state,(float)(time),
                (float)(length), 4,1/(float)4,
                SPLT_DEFAULT_PROGRESS_RATE);
          }
          else
          {
            //if we have cancelled the split
            if (splt_t_split_is_canceled(state))
            {
              stop = 1;
            }
            splt_t_update_progress(state,(float)pos,
                (float)(mp3state->mp3file.len),
                1,0,SPLT_DEFAULT_PROGRESS_RATE);
          }
        }
        break;
      case 0:
        //0 we do nothing
        break;
      case -1:
        // -1 means eof
        stop = 1;
        break;
      case -3:
        //error from libmad
        stop = 1;
        *error = mad_err;
        found = -1;
        break;
      default:
        break;
    }
  } while (!stop && (found < SPLT_MAXSILENCE));

  //only if we have silence mode, we set progress to 100%
  if (splt_t_get_int_option(state, SPLT_OPT_SPLIT_MODE) == 
      SPLT_OPTION_SILENCE_MODE)
  {
    splt_t_update_progress(state,1.0,1.0,1,1,1);
  }

  //we finish with mad_*
  mad_frame_finish(&mp3state->frame);
  mad_stream_finish(&mp3state->stream);
  mad_synth_finish(&mp3state->synth);

  return found;
}

/****************************/
/* mp3 split */

//used for the mp3 sync errors split and mp3 split(for header)
//returns 0 if no errors, SPLT_ defined errors if ones
//It justs copies the data of the input file from a begin offset
//to an end offset, and, eventually, a Xing frame (for VBR)
//at the beginning and a ID3v1 at the end, to an outputfile.
static int splt_mp3_simple_split(splt_state *state, const char *output_fname,
    off_t begin, off_t end, int do_write_tags)
{
  splt_u_print_debug("We do mp3 simple split on output...",0,output_fname);
  splt_u_print_debug("Mp3 simple split offset begin is",begin,NULL);
  splt_u_print_debug("Mp3 simple split offset end is",end,NULL);

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
  int split_mode = splt_t_get_int_option(state, SPLT_OPT_SPLIT_MODE);

  splt_t_put_progress_text(state,SPLT_PROGRESS_CREATE);

  char *filename = splt_t_get_filename_to_split(state);

  position = ftello(mp3state->file_input); // Save current position

  if (fseeko(mp3state->file_input, begin, SEEK_SET)==-1)
  {
    return SPLT_ERROR_BEGIN_OUT_OF_FILE;
  }

  //get the file size
  off_t st_size;
  char *fname_to_split = splt_t_get_filename_to_split(state);
  if(splt_u_stat(fname_to_split, NULL, &st_size) == 0)
  {
    mp3state->end2 = st_size;
  }
  else
  {
    splt_t_set_strerror_msg(state);
    splt_t_set_error_data(state,fname_to_split);
    return SPLT_ERROR_CANNOT_OPEN_FILE;
  }

  // - means stdout
  if (strcmp(output_fname, "-")==0)
  {
    file_output = stdout;
#ifdef __WIN32__
    _setmode(fileno(file_output), _O_BINARY);
#endif
  }
  else
  {
    if (!(file_output=splt_u_fopen(output_fname, "wb+")))
    {
      splt_t_set_strerror_msg(state);
      splt_t_set_error_data(state, output_fname);
      return SPLT_ERROR_CANNOT_OPEN_DEST_FILE;
    }
  }

#ifndef NO_ID3TAG
  //write id3 tags version 2 at the start of the file, if necessary
  if (do_write_tags)
  {
    int err = SPLT_OK;
    if ((err = splt_mp3_write_id3_tags(state, file_output, output_fname, 2)) < 0)
    {
      error = err;
      goto function_end;
    }
  }
#endif

  if (mp3state->mp3file.xing!=0)
  {
    //don't write the xing header if we have the no tags
    if (splt_t_get_int_option(state,SPLT_OPT_TAGS) != SPLT_NO_TAGS)
    {
      //don't write the xing header if error mode split
      if (state->options.split_mode != SPLT_OPTION_ERROR_MODE)
      {
        if(fwrite(mp3state->mp3file.xingbuffer, 1, 
              mp3state->mp3file.xing, file_output) < mp3state->mp3file.xing)
        {
          splt_t_set_error_data(state, output_fname);
          error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
          goto function_end;
        }
      }
    }
  }

  while (!feof(mp3state->file_input))
  {
    readed = SPLT_MP3_READBSIZE;
    if (end!=-1)
    {
      if (begin>=end) 
      {
        break;
      }
      if ((end-begin) < SPLT_MP3_READBSIZE)
      {
        readed = end-begin;
      }
    }

    if ((readed = fread(buffer, 1, readed, mp3state->file_input))==-1)
    {
      break;
    }

    if (fwrite(buffer, 1, readed, file_output) < readed)
    {
      splt_t_set_error_data(state,output_fname);
      error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
      goto function_end;
    }
    begin += readed;

    //we update the progress bar
    if ((split_mode == SPLT_OPTION_WRAP_MODE) ||
        (split_mode == SPLT_OPTION_ERROR_MODE) ||
        ((split_mode == SPLT_OPTION_NORMAL_MODE)
         && (!splt_t_get_int_option(state, SPLT_OPT_AUTO_ADJUST)) 
         && (!splt_t_get_int_option(state, SPLT_OPT_FRAME_MODE))))
    {
      temp_end = end;
      //for the last split
      if (end == -1)
      {
        temp_end = mp3state->end2;
      }

      splt_t_update_progress(state,(float)(begin-start),
          (float)(temp_end-start),1,0,
          SPLT_DEFAULT_PROGRESS_RATE);
    }
    else
    {
      //if auto adjust, we have 50%
      if (splt_t_get_int_option(state, SPLT_OPT_AUTO_ADJUST))
      {
        splt_t_update_progress(state,(float)(begin-start),
            (float)(end-start),
            2,0.5,
            SPLT_DEFAULT_PROGRESS_RATE);
      }
      else
      {
        if (splt_t_get_int_option(state, SPLT_OPT_SPLIT_MODE)
            == SPLT_OPTION_TIME_MODE)
        {
          temp_end = end;
          //for the last split
          if (end == -1)
          {
            temp_end = mp3state->end2;
          }

          //if framemode
          if (splt_t_get_int_option(state, SPLT_OPT_FRAME_MODE))
          {
            splt_t_update_progress(state,(float)(begin-start),
                (float)(temp_end-start),
                2,0.5,
                SPLT_DEFAULT_PROGRESS_RATE);
          }
          else
          {
            splt_t_update_progress(state,(float)(begin-start),
                (float)(temp_end-start),
                1,0,
                SPLT_DEFAULT_PROGRESS_RATE);
          }
        }
        else
        {
          splt_t_update_progress(state,(float)(begin-start),
              (float)(end-start),
              2,0.5,
              SPLT_DEFAULT_PROGRESS_RATE);
        }
      }
    }
  }

  //write id3 tags version 1 at the end of the file, if necessary
  if (do_write_tags)
  {
    int err = SPLT_OK;
    if ((err = splt_mp3_write_id3_tags(state, file_output, output_fname, 1)) < 0)
    {
      error = err;
      goto function_end;
    }
  }

  if (fseeko(mp3state->file_input, position, SEEK_SET)==-1)
  {
    splt_t_set_strerror_msg(state);
    splt_t_set_error_data(state, filename);
    goto function_end;
  }

function_end:
  if (file_output != stdout)
  {
    if (fclose(file_output) != 0)
    {
      splt_t_set_strerror_msg(state);
      splt_t_set_error_data(state, filename);
      return SPLT_ERROR_CANNOT_CLOSE_FILE;
    }
  }
  file_output = NULL;

  return error;
}

//the main mp3 split function
//filename is our filename
//state is our state
//fbegin_sec is the beggining splitpoint
//fend_sec is the end splitpoint
//adjustoption is if we adjust with silence detection or not
//seekable is if we split in seekable mode or not
//threshold - see manual
//must be called after splt_mp3_info()
//returns possible error in '*error'
static void splt_mp3_split(const char *output_fname, splt_state *state,
    double fbegin_sec, double fend_sec, int *error, int save_end_point)
{
  splt_u_print_debug("Mp3 split...",0,NULL);
  splt_u_print_debug("Output filename is",0,output_fname);
  splt_u_print_debug("Begin position",fbegin_sec,NULL);
  splt_u_print_debug("End position",fend_sec,NULL);

  splt_mp3_state *mp3state = state->codec;

  short adjustoption = splt_t_get_int_option(state, SPLT_OPT_PARAM_GAP);
  short seekable = ! splt_t_get_int_option(state, SPLT_OPT_INPUT_NOT_SEEKABLE);
  float threshold = splt_t_get_float_option(state, SPLT_OPT_PARAM_THRESHOLD);

  short eof=0, check_bitrate=0;

  char *filename = splt_t_get_filename_to_split(state);

  FILE *file_output = NULL;
  short writing = 0, finished=0;
  unsigned long fbegin=0;
  off_t wrote = 0;
  int len = 0;
  //for the progress
  unsigned long stopped_frames = 0;
  int progress_adjust_val = 2;
  if (adjustoption) 
  {
    progress_adjust_val = 4;
  }

  splt_t_put_progress_text(state,SPLT_PROGRESS_CREATE);

  mad_stream_init(&mp3state->stream);
  mad_frame_init(&mp3state->frame);

  //if not seekable
  if (!seekable)
  {
    splt_u_print_debug("Starting not seekable...",0,NULL);

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
      if (!(file_output=splt_u_fopen(output_fname, "wb+")))
      {
        splt_t_set_strerror_msg(state);
        splt_t_set_error_data(state,output_fname);
        *error = SPLT_ERROR_CANNOT_OPEN_DEST_FILE;
        mad_frame_finish(&mp3state->frame);
        mad_stream_finish(&mp3state->stream);
        return;
      }
    }

#ifndef NO_ID3TAG
    int err = SPLT_OK;
    //write id3 tags version 2 at the start of the file
    if ((err = splt_mp3_write_id3_tags(state, file_output, output_fname, 2)) < 0)
    {
      *error = err;
      goto bloc_end;
    }
#endif

    //if we have the framemode
    if (mp3state->framemode)
    {
      splt_u_print_debug("Starting mp3 frame mode...",0,NULL);

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
        //we write xing if vbr
        if (!writing && (time >= begin_c))
        {
          writing = 1;
          fbegin = mp3state->frames;

          if (mp3state->mp3file.xing > 0)
          {
            wrote = fwrite(mp3state->mp3file.xingbuffer, 1, mp3state->mp3file.xing, file_output);
            if (wrote < mp3state->mp3file.xing)
            {
              splt_t_set_error_data(state,output_fname);
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
            if ((len = fwrite(mp3state->data_ptr, 1, mp3state->data_len, file_output))
                < mp3state->data_len)
            {
              splt_t_set_error_data(state,output_fname);
              *error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
              goto bloc_end;
            }
            wrote = (off_t) (wrote + len);
            mp3state->data_len = 0;
          }
          if ((end_c > 0) && (time > end_c))
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
        if (splt_t_get_int_option(state,SPLT_OPT_SPLIT_MODE)
            == SPLT_OPTION_TIME_MODE)
        {
          splt_t_update_progress(state,(float)(time-begin_c),
              (float)(end_c-begin_c),1,0,
              SPLT_DEFAULT_PROGRESS_RATE);
        }
        else
        {
          splt_t_update_progress(state,(float)(time),
              (float)(end_c),1,0,
              SPLT_DEFAULT_PROGRESS_RATE);
        }

        int mad_err = SPLT_OK;
        //we get next frame
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
      splt_u_print_debug("Starting mp3 non frame mode...",0,NULL);

      off_t begin = 0, end = 0;
      if (fend_sec != -1)
      {
        end = (off_t) (fend_sec * 
            mp3state->mp3file.bitrate + mp3state->mp3file.firsth);
      }

      //find begin point because no 'end' saved point
      if (mp3state->end == 0)
      {
        begin = (off_t) (fbegin_sec * mp3state->mp3file.bitrate + mp3state->mp3file.firsth);

        if ((mp3state->bytes == begin) && (mp3state->data_len > 0))
        {
          len = (long) (mp3state->inputBuffer + mp3state->buf_len - mp3state->data_ptr);
          if (len < 0)
          {
            splt_t_set_error_data(state,filename);
            *error = SPLT_ERROR_WHILE_READING_FILE;
            goto bloc_end;
          }
          if (fwrite(mp3state->data_ptr, 1, len, file_output) < len)
          {
            splt_t_set_error_data(state,output_fname);
            *error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
            goto bloc_end;
          }
          wrote = (off_t) (wrote + len);
          mp3state->data_len = 0;
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
            if ((mp3state->data_len = fread(mp3state->inputBuffer, 
                    1, to_read, mp3state->file_input))<=0)
            {
              *error = SPLT_ERROR_BEGIN_OUT_OF_FILE;
              goto bloc_end;
            }
            mp3state->bytes+=mp3state->data_len;
          }

          int mad_err = SPLT_OK;
          //we get next frame
          switch (splt_mp3_get_valid_frame(state, &mad_err))
          {
            case 1:
              len = (long) (mp3state->inputBuffer + mp3state->buf_len - mp3state->data_ptr);
              if (len < 0)
              {
                splt_t_set_error_data(state,filename);
                *error = SPLT_ERROR_WHILE_READING_FILE;
                goto bloc_end;
              }
              if (fwrite(mp3state->data_ptr, 1, len, file_output) < len)
              {
                splt_t_set_error_data(state,output_fname);
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
          splt_t_set_error_data(state,filename);
          *error = SPLT_ERROR_WHILE_READING_FILE;
          goto bloc_end;
        }
        if (fwrite(mp3state->data_ptr, 1, len, file_output) < len)
        {
          splt_t_set_error_data(state,output_fname);
          *error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
          goto bloc_end;
        }
        wrote = (off_t) (wrote + len);
        mp3state->data_len = 0;
        begin = mp3state->end;
      }

      long split_begin_point = mp3state->bytes;
      //while not end of file, read write :
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

        //we read the file input
        if (feof(mp3state->file_input) || 
            ((mp3state->data_len = 
              fread(mp3state->inputBuffer, 1, to_read, mp3state->file_input))<=0))
        {
          eof = 1;
          *error = SPLT_OK_SPLIT_EOF;
          break;
        }

        //we write to file output
        if (fwrite(mp3state->inputBuffer, 1, mp3state->data_len, file_output) < mp3state->data_len)
        {
          splt_t_set_error_data(state,output_fname);
          *error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
          goto bloc_end;
        }

        mp3state->bytes += mp3state->data_len;

        splt_t_update_progress(state, (float) (mp3state->bytes-split_begin_point),
            (float)(end-split_begin_point), 1,0,SPLT_DEFAULT_PROGRESS_RATE);
      }

      if (save_end_point)
      {
        mp3state->end = end;
      }
      else
      {
        mp3state->end = 0;
      }

      if (!eof)
      {
        int mad_err = SPLT_OK;
        switch (splt_mp3_get_valid_frame(state, &mad_err))
        {
          case 1:
            len = (long) (mp3state->data_ptr - mp3state->inputBuffer);
            if (len < 0)
            {
              splt_t_set_error_data(state,filename);
              *error = SPLT_ERROR_WHILE_READING_FILE;
              goto bloc_end;
            }
            if (fwrite(mp3state->inputBuffer, 1, len, file_output) < len)
            {
              splt_t_set_error_data(state,output_fname);
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

    splt_u_print_debug("Writing mp3 tags...",0,NULL);

    //we write id3 and other stuff
    if (file_output)
    {
      int err = SPLT_OK;
      //write id3 tags version 1 at the end of the file
      if ((err = splt_mp3_write_id3_tags(state, file_output, output_fname, 1)) < 0)
      {
        *error = err;
        goto bloc_end;
      }
      
      if (mp3state->mp3file.xing > 0)
      {
        if (fseeko(file_output, mp3state->mp3file.xing_offset+4, SEEK_SET)!=-1)
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
          splt_t_set_strerror_msg(state);
          splt_t_set_error_data(state, output_fname);
          *error = SPLT_ERROR_SEEKING_FILE;
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
          splt_t_set_strerror_msg(state);
          splt_t_set_error_data(state, output_fname);
          *error = SPLT_ERROR_CANNOT_CLOSE_FILE;
        }
      }
    }
    file_output = NULL;

    mad_frame_finish(&mp3state->frame);
    mad_stream_finish(&mp3state->stream);
    if (*error == SPLT_OK) { *error = SPLT_OK_SPLIT; }

    return;
  }
  //if seekable :
  else
  {
    splt_u_print_debug("Starting mp3 seekable...",0,NULL);

    off_t begin = 0, end = 0;
    //if framemode
    if (mp3state->framemode)
    {
      splt_u_print_debug("Starting mp3 frame mode...",0,NULL);

      unsigned long fbegin, fend, adjust;
      fbegin = fend = adjust = 0;
      fbegin = fbegin_sec * mp3state->mp3file.fps;

      if (fend_sec != -1)
      {
        //if adjustoption
        if (adjustoption)
        {
          if (fend_sec != -1)
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
            adjust=0;
          }
        }
        fend = fend_sec * mp3state->mp3file.fps;
      }
      else 
      {
        fend = 0xFFFFFFFF;
      }

      splt_u_print_debug("Finding begin...",0,NULL);

      if (mp3state->end == 0)
      {
        if (mp3state->first)
        {
          mp3state->h.ptr = mp3state->mp3file.firsthead.ptr;
          mp3state->h.framesize = mp3state->mp3file.firsthead.framesize;
          begin = mp3state->mp3file.firsthead.ptr;
          mp3state->first = 0;
        }

        splt_t_put_progress_text(state,SPLT_PROGRESS_PREPARE);

        // Finds begin by counting frames
        while (mp3state->frames < fbegin)
        {
          begin = splt_mp3_findhead(mp3state, mp3state->h.ptr + mp3state->h.framesize);
          if (begin==-1)
          {
            *error = SPLT_ERROR_BEGIN_OUT_OF_FILE;
            goto bloc_end2;
          }

          //count the number of syncerrors
          if ((begin!=mp3state->h.ptr + mp3state->h.framesize)&&(state->syncerrors>=0)) 
          {
            state->syncerrors++;
          }
          if ((mp3state->syncdetect)&&(state->syncerrors> SPLT_MAXSYNC))
          {
            splt_mp3_checksync(mp3state);
          }

          mp3state->h = splt_mp3_makehead (mp3state->headw, mp3state->mp3file, mp3state->h, begin);
          mp3state->frames++;

          //if we have adjust mode, then put only 25%
          //else put 50%
          if (adjustoption)
          {
            splt_t_update_progress(state,(float)(mp3state->frames),
                (float)fend, 8,
                0,SPLT_DEFAULT_PROGRESS_RATE);
          }
          else
          {
            splt_t_update_progress(state,(float)(mp3state->frames),
                (float)fend,progress_adjust_val,
                0,SPLT_DEFAULT_PROGRESS_RATE);
          }
        }
      }
      else
      {
        begin = mp3state->end;
      }

      splt_u_print_debug("Begin is...",begin,NULL);

      if (mp3state->mp3file.len > 0)
      {
        if (begin >= mp3state->mp3file.len) // If we can check, we just do that :)
        {
          *error = SPLT_ERROR_BEGIN_OUT_OF_FILE;
          goto bloc_end2;
        }
      }

      splt_t_put_progress_text(state,SPLT_PROGRESS_PREPARE);

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
        int split_mode = splt_t_get_int_option(state, SPLT_OPT_SPLIT_MODE);
        if (((split_mode == SPLT_OPTION_TIME_MODE) || 
              (split_mode == SPLT_OPTION_SILENCE_MODE))
            && (!splt_t_get_int_option(state,SPLT_OPT_AUTO_ADJUST)))
        {
          splt_t_update_progress(state, (float)(mp3state->frames-fbegin),
              (float)(fend-fbegin), progress_adjust_val,
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
                splt_t_update_progress(state,
                    (float)(mp3state->frames-frames_begin),
                    (float)(fend-frames_begin),
                    4,0,SPLT_DEFAULT_PROGRESS_RATE);
              }
              else
              {
                splt_t_update_progress(state,
                    (float)(mp3state->frames-frames_begin),
                    (float)(fend-frames_begin),
                    8,1/(float)8,SPLT_DEFAULT_PROGRESS_RATE);
              }
            }
          }
          else
          {
            splt_t_update_progress(state,
                (float)(mp3state->frames-stopped_frames),
                (float)(fend-stopped_frames),
                progress_adjust_val,
                0,SPLT_DEFAULT_PROGRESS_RATE);
          }
        }

        //if adjust option, scans for silence
        if ((adjust) && (mp3state->frames >= fend))
        {
          int silence_points_found =
            splt_mp3_scan_silence(state, end, 2 * adjust, threshold, 0.f, 0, error);
          //if error, go out
          if (silence_points_found == -1)
          {
            goto bloc_end2;
          }
          else if (silence_points_found > 0)
          {
            adjust = (unsigned long) (splt_u_silence_position(state->silence_list, mp3state->off) 
                * mp3state->mp3file.fps);
          }
          else
          {
            adjust = (unsigned long) (adjustoption * mp3state->mp3file.fps);
          }

          fend += adjust;

          end = splt_mp3_findhead(mp3state, end);

          splt_t_ssplit_free(&state->silence_list);
          adjust=0;
          //progress
          splt_t_put_progress_text(state,SPLT_PROGRESS_PREPARE);
          stopped_frames = mp3state->frames;
        }
      }

      //save end point for performance reasons
      if (save_end_point)
      {
        mp3state->end = end;
      }
      else
      {
        mp3state->end = 0;
      }

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
      splt_u_print_debug("Starting mp3 non frame mode...",0,NULL);

      //find begin point if the last 'end' not saved
      if (mp3state->end == 0) 
      {
        begin = (off_t) (fbegin_sec * mp3state->mp3file.bitrate + mp3state->mp3file.firsth);
        // Finds first valid header. Mantain clean files.
        begin = splt_mp3_findvalidhead(mp3state, begin);

        if (begin==-1)
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

      if (fend_sec != -1)
      {
        end = (off_t) (fend_sec * mp3state->mp3file.bitrate + mp3state->mp3file.firsth);
        end = splt_mp3_findvalidhead(mp3state, end); // We take the complete frame
        if (splt_mp3_tabsel_123[1 - mp3state->mp3file.mpgid][mp3state->mp3file.layer-1][splt_mp3_c_bitrate(mp3state->headw)] != 
            mp3state->mp3file.firsthead.bitrate)
          check_bitrate = 1;
      }

      //save end point for performance reasons
      if (save_end_point)
      {
        mp3state->end = end;
      }
      else
      {
        mp3state->end = 0;
      }
    }

    //seekable real split
    int err = splt_mp3_simple_split(state, output_fname, begin, end, SPLT_TRUE);
    if (err < 0) { *error = err; }
  }

  if (check_bitrate)
  {
    *error = SPLT_MIGHT_BE_VBR;
  }

  if (*error == SPLT_OK) { *error = SPLT_OK_SPLIT; }

bloc_end2:
  mad_frame_finish(&mp3state->frame);
  mad_stream_finish(&mp3state->stream);
}

/****************************/
/* mp3 syncerror */

//this function is searching for the id3v1 and id3v2 and returns the offset
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

//the function counts the number of sync error splits, 
//and sets the offsets
static void splt_mp3_syncerror_search(splt_state *state, int *error)
{
  off_t offset = 0;
  char *filename = splt_t_get_filename_to_split(state);
  int sync_err = SPLT_OK;

  splt_mp3_state *mp3state = state->codec;

  splt_t_put_progress_text(state,SPLT_PROGRESS_SEARCH_SYNC);

  mp3state->h.ptr = mp3state->mp3file.firsthead.ptr;
  mp3state->h.framesize = mp3state->mp3file.firsthead.framesize;

  //if the filename is correct
  int is_file = splt_check_is_file(state, filename);
  if (*error < 0) { return; }

  if (!is_file)
  {
    *error = SPLT_ERROR_INEXISTENT_FILE;
    return;
  }

  //we get the file length for the progress
  off_t st_size;
  if(splt_u_stat(filename, NULL, &st_size) == 0)
  {
    //put the start point
    sync_err = splt_t_serrors_append_point(state, 0);
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

        //put syncerror splitpoint offset
        sync_err = splt_t_serrors_append_point(state, serror_point);
        if (sync_err != SPLT_OK)
        {
          *error = sync_err;
          return;
        }
        offset = splt_mp3_findvalidhead(mp3state, serror_point);
        if (splt_u_getword(mp3state->file_input, offset, SEEK_SET, &mp3state->headw) == -1)
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
      splt_t_update_progress(state,(float)(offset),
          (float)(st_size),1,0,
          SPLT_DEFAULT_PROGRESS_RATE);
    }
  }
  else
  {
    splt_t_set_strerror_msg(state);
    splt_t_set_error_data(state,filename);
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
  sync_err = splt_t_serrors_append_point(state, LONG_MAX);
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

static const unsigned char splt_mp3_albumwraphead[22] =
{
  0xa, 0x23, 0x54, 0x49, 0x54, 0x32, 0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x00, 0x41, 0x6c, 0x62, 0x75, 0x6d, 0x57, 0x72, 0x61, 0x70,
};

//this function dewraps a file
//we return the possible error in the process_result parameter
static void splt_mp3_dewrap(int listonly, const char *dir, int *error, splt_state *state)
{
  *error = SPLT_DEWRAP_OK;

  //if albumwrap or mp3wrap
  short albumwrap=0, mp3wrap=0;
  //wrapfiles = the wrapped files number
  int wrapfiles=0, i, j, k=0;
  unsigned char c;
  char filename[2048] = { '\0' };
  off_t begin=0, end=0, len = 0, id3offset = 0;
  char junk[384] = { '\0' };
  char *file_to_dewrap = splt_t_get_filename_to_split(state);

  //we free previously wrap files
  splt_t_wrap_free(state);

  //if error
  if (*error != SPLT_DEWRAP_OK)
  {
    return;
  }
  else
  {
    splt_mp3_state *mp3state = state->codec;

    if (*error >= 0)
    {
      len = splt_u_flength(state, mp3state->file_input, file_to_dewrap, error);
      if (error < 0) { return; }

      id3offset = splt_mp3_getid3v2_end_offset(mp3state->file_input, 0);

      //we go at the beginning of the file
      if (fseeko(mp3state->file_input, id3offset, SEEK_SET)==-1)
      {
        *error = SPLT_DEWRAP_ERR_FILE_NOT_WRAPED_DAMAGED;
        return;
      }

      splt_u_print_debug("We search for wrap string...",0,NULL);

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
        splt_u_print_debug("We do the effective dewrap...",0,NULL);

        //client informations
        char client_infos[2048] = { '\0' };

        //mp3wrap checkings and we get the wrap file number
        if (mp3wrap) {
          splt_u_print_debug("We do mp3 mp3wrap check...",0,NULL);
          short indexver;

          //Mp3Wrap version
          char major_v = fgetc(mp3state->file_input);
          char minor_v = fgetc(mp3state->file_input);
          snprintf(client_infos,1024,
              " Detected file created with: Mp3Wrap v. %c.%c\n",major_v,minor_v);

          splt_t_put_message_to_client(state, client_infos);

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
            if (splt_u_getword(mp3state->file_input, 0, SEEK_CUR, &fcrc)==-1)
            {
              *error = SPLT_DEWRAP_ERR_FILE_NOT_WRAPED_DAMAGED;
              return;
            }

            //perform CRC only if we don't have quiet mode
            if (! splt_t_get_int_option(state, SPLT_OPT_QUIET_MODE))
            {
              begin = ftello(mp3state->file_input);
              if (fseeko(mp3state->file_input, 
                    splt_mp3_getid3v1_offset(mp3state->file_input), SEEK_END)==-1)
              {
                splt_t_set_strerror_msg(state);
                splt_t_set_error_data(state, file_to_dewrap);
                *error = SPLT_ERROR_SEEKING_FILE;
                return;
              }
              end = ftello(mp3state->file_input);
              splt_t_put_message_to_client(state,
                  " Check for file integrity: calculating CRC please wait... ");
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
                splt_t_put_message_to_client(state, " OK\n");
              }
              if (fseeko(mp3state->file_input, begin, SEEK_SET)==-1)
              {
                splt_t_set_strerror_msg(state);
                splt_t_set_error_data(state, file_to_dewrap);
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
          splt_u_print_debug("We do mp3 albumwrap check...",0,NULL);
          //Mp3Wrap version
          snprintf(client_infos,1024, " Detected file created with: AlbumWrap\n");
          splt_t_put_message_to_client(state, client_infos);

          if (fseeko(mp3state->file_input, (off_t) 0x52d, SEEK_SET)==-1)
          {
            *error = SPLT_DEWRAP_ERR_FILE_NOT_WRAPED_DAMAGED;
            return;
          }
          i = 0;
          while (((c=fgetc(mp3state->file_input))!=0x20) &&(i<384))
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

        splt_u_print_debug("Number of wrap splitpoints is",wrapfiles+1,NULL);

        snprintf(client_infos, 1024, " Total files: %d\n",wrapfiles);
        splt_t_put_message_to_client(state, client_infos);
        
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
                if (splt_u_getword (mp3state->file_input, 0, SEEK_CUR, &w)==-1)
                {
                  splt_t_set_error_data(state,file_to_dewrap);
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
                  splt_t_set_error_data(state,file_to_dewrap);
                  *error = SPLT_DEWRAP_ERR_FILE_DAMAGED_INCOMPLETE;
                  return;
                }
                j = 0;
                while ((c=fgetc(mp3state->file_input))!='[')
                  if (j++ > 32) 
                  {
                    splt_t_set_error_data(state,file_to_dewrap);
                    *error = SPLT_DEWRAP_ERR_FILE_DAMAGED_INCOMPLETE;
                    return;
                  }
                if (fseeko(mp3state->file_input, (off_t) 3, SEEK_CUR)==-1)
                {
                  splt_t_set_error_data(state,file_to_dewrap);
                  *error = SPLT_DEWRAP_ERR_FILE_DAMAGED_INCOMPLETE;
                  return;
                }
                j = 0;
                while ((j<384) && ((c = fgetc(mp3state->file_input))!='['))
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

              if (splt_u_getword (mp3state->file_input, 0, SEEK_CUR, &w) == -1)
              {
                splt_t_set_error_data(state,file_to_dewrap);
                *error = SPLT_DEWRAP_ERR_FILE_DAMAGED_INCOMPLETE;
                return;
              }

              end = (off_t) (w + id3offset);

              memset(junk, 0x00, 384);
              char *ptr = filename;
              //create output directories for the wrapped files
              while (((ptr = strchr(ptr, SPLT_DIRCHAR))!=NULL)&&((ptr-filename)<384))
              {
                ptr++;
                strncpy(junk, filename, ptr-filename);
                if (! splt_u_check_if_directory(junk))
                {
                  if ((splt_u_mkdir(junk))==-1)
                  {
                    *error = SPLT_ERROR_CANNOT_CREATE_DIRECTORY;
                    return;
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
                  splt_t_set_error_data(state,file_to_dewrap);
                  *error = SPLT_DEWRAP_ERR_FILE_DAMAGED_INCOMPLETE;
                  return;
                }
                j = 0;
                while ((j<384) && ((c = fgetc(mp3state->file_input))!='['))
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
                splt_t_set_error_data(state,file_to_dewrap);
                *error = SPLT_DEWRAP_ERR_FILE_DAMAGED_INCOMPLETE;
              }
              j = 0;
              while ((c=fgetc(mp3state->file_input))!='[')
                if (j++ > 32) 
                {
                  splt_t_set_error_data(state,file_to_dewrap);
                  *error = SPLT_DEWRAP_ERR_FILE_DAMAGED_INCOMPLETE;
                  return;
                }
              if (fseeko (mp3state->file_input, (off_t) 3, SEEK_CUR)==-1)
              {
                splt_t_set_error_data(state,file_to_dewrap);
                *error = SPLT_DEWRAP_ERR_FILE_DAMAGED_INCOMPLETE;
                return;
              }
              j = 0;
              while ((c=fgetc(mp3state->file_input))!='[')
                if (j++ > 32) 
                {
                  splt_t_set_error_data(state,file_to_dewrap);
                  *error = SPLT_DEWRAP_ERR_FILE_DAMAGED_INCOMPLETE;
                  return;
                }
              if (fseeko(mp3state->file_input, (off_t) 3, SEEK_CUR)==-1)
              {
                splt_t_set_error_data(state,file_to_dewrap);
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

            splt_u_print_debug("We have found the file",0,filename);
            splt_u_print_debug("We cut the dirchar",0,NULL);

            //we cut the .DIRCHAR before the filename
            char str_temp[4];
            snprintf(str_temp,4,"%c%c",'.',SPLT_DIRCHAR);
            if (strstr(filename,str_temp) != NULL)
            {
              if (filename != NULL)
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
            }

            if (feof(mp3state->file_input)) 
            {
              splt_t_set_error_data(state,file_to_dewrap);
              *error = SPLT_DEWRAP_ERR_FILE_DAMAGED_INCOMPLETE;
              return;
            }

            //if we only list the contents
            //we put the files in the wrap_files
            if (listonly)
            {
              splt_u_print_debug("We only list wrapped files",0,NULL);

              int put_file_error = SPLT_OK;
              put_file_error = splt_t_wrap_put_file(state, wrapfiles, i, filename);
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
              splt_u_print_debug("We split wrapped file",0,NULL);

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
                  snprintf(filename, 2048,"%s%c%s", dir, SPLT_DIRCHAR, ptr);
                }
                splt_u_print_debug("wrap dir",0,dir);
                splt_u_print_debug("wrap after dir",0,ptr);
              }

              //we put xingbuffer
              if (mp3state->mp3file.xingbuffer)
              {
                free(mp3state->mp3file.xingbuffer);
                mp3state->mp3file.xingbuffer = NULL;
              }
              mp3state->mp3file.xing = 0;

              int append_err = SPLT_OK;
              append_err = splt_t_append_splitpoint(state,0,
                  splt_u_get_real_name(filename), SPLT_SPLITPOINT);
              if (append_err != SPLT_OK)
              {
                *error = append_err;
                return;
              }

              //cut extension
              int cut_err = splt_u_cut_splitpoint_extension(state,i);
              if (cut_err != SPLT_OK)
              {
                *error = cut_err;
                return;
              }

              //do the real wrap split
              ret = splt_mp3_simple_split(state, filename, begin, end, SPLT_FALSE);

              //if we could split put the split file
              if (ret >= 0)
              {
                ret = splt_t_put_split_file(state, filename);
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

/****************************/
/* External plugin API */

//returns the plugin infos (name, version, extension)
//-alloced data in splt_plugin_info will be freed at the end of the program
void splt_pl_set_plugin_info(splt_plugin_info *info, int *error)
{
  float plugin_version = 0.1;

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
}

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
      mp3state->off = splt_t_get_float_option(state,SPLT_OPT_PARAM_OFFSET);

      //we initialise frames to 1
      if (splt_t_get_total_time(state) > 0)
      {
        mp3state->frames = 1;
      }
    }
  }
}

void splt_pl_init(splt_state *state, int *error)
{
  splt_mp3_init(state, error);
}

void splt_pl_end(splt_state *state, int *error)
{
  //put infos about the frames processed and the number of sync errors
  //ONLY if framemode
  if ((splt_t_get_int_option(state, SPLT_OPT_SPLIT_MODE) != SPLT_OPTION_SILENCE_MODE)
      && (splt_t_get_int_option(state, SPLT_OPT_SPLIT_MODE) != SPLT_OPTION_ERROR_MODE)
      && (splt_t_get_int_option(state, SPLT_OPT_SPLIT_MODE) != SPLT_OPTION_WRAP_MODE))
  {
    if (splt_t_get_int_option(state, SPLT_OPT_FRAME_MODE))
    {
      if (*error >= 0)
      {
        splt_mp3_state *mp3state = state->codec;
        char message[1024] = { '\0' };
        snprintf(message, 1024,
            " Processed %lu frames - Sync errors: %lu\n",
            mp3state->frames, state->syncerrors);
        splt_t_put_message_to_client(state, message);
      }
    }
  }
  splt_mp3_end(state, error);
}

//check if file is mp3
int splt_pl_check_plugin_is_for_file(splt_state *state, int *error)
{
  char *filename = splt_t_get_filename_to_split(state);

  if (filename != NULL && ((strcmp(filename,"-") == 0) ||
      (strcmp(filename,"m-") == 0)))
  {
    return SPLT_TRUE;
  }

  int is_mp3 = SPLT_FALSE;

  splt_t_lock_messages(state);
  splt_mp3_init(state, error);
  splt_t_unlock_messages(state);
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

//search for syncerrors
void splt_pl_search_syncerrors(splt_state *state, int *error)
{
  //we detect sync errors
  splt_mp3_syncerror_search(state, error);
}

//get wrap files or dewrap
void splt_pl_dewrap(splt_state *state, int listonly, const char *dir, int *error)
{
  splt_t_wrap_free(state);
  splt_mp3_dewrap(listonly, dir, error, state);
}

void splt_pl_split(splt_state *state, const char *final_fname,
    double begin_point, double end_point, int *error, int save_end_point)
{
  //effective mp3 split
  splt_mp3_split(final_fname, state, begin_point, end_point, error, save_end_point);
}

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
  error = splt_mp3_simple_split(state, output_fname, begin, end, SPLT_FALSE);

  return error;
}

int splt_pl_scan_silence(splt_state *state, int *error)
{
  float offset = splt_t_get_float_option(state,SPLT_OPT_PARAM_OFFSET);
  float threshold = splt_t_get_float_option(state, SPLT_OPT_PARAM_THRESHOLD);
  float min_length = splt_t_get_float_option(state, SPLT_OPT_PARAM_MIN_LENGTH);
  int found = 0;

  splt_mp3_state *mp3state = state->codec;
  mp3state->off = offset;

  found = splt_mp3_scan_silence(state, mp3state->mp3file.firsthead.ptr, 0,
      threshold, min_length, 1, error);

  return found;
}

void splt_pl_set_original_tags(splt_state *state, int *error)
{
#ifndef NO_ID3TAG
  char *filename = splt_t_get_filename_to_split(state);
  splt_mp3_get_original_tags(filename, state, error);
#else
  //splt_u_error(SPLT_IERROR_SET_ORIGINAL_TAGS,__func__, 0, NULL);
#endif
}

