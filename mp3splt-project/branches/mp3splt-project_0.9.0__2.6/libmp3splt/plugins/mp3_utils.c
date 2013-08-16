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
 * USA.
 *********************************************************/

#include "mp3_utils.h"

//! Initializes a stream frame
void splt_mp3_init_stream_frame(splt_mp3_state *mp3state)
{
  mad_stream_init(&mp3state->stream);
  mad_frame_init(&mp3state->frame);
}

//! Finishes a stream frame
void splt_mp3_finish_stream_frame(splt_mp3_state *mp3state)
{
  mad_stream_finish(&mp3state->stream);
  mad_frame_finish(&mp3state->frame);
}

/*! does nothing important for libmp3splt

\todo review this..
*/
void splt_mp3_checksync(splt_mp3_state *mp3state)
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

//!calculates bitrate
int splt_mp3_c_bitrate(unsigned long head)
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

//!make mp3 header bitrate, padding, offset, framesize
struct splt_header splt_mp3_makehead(unsigned long headword, 
    struct splt_mp3 mp3f, struct splt_header head, off_t ptr)
{
  head.ptr = ptr;
  head.bitrate = splt_mp3_tabsel_123[1 - mp3f.mpgid][mp3f.layer-1][splt_mp3_c_bitrate(headword)];
  head.padding = ((headword>>9)&0x1);
  head.framesize = (head.bitrate*144000)/
    (mp3f.freq<<(1 - mp3f.mpgid)) + head.padding;

  return head;
}

//!finds first header from start_pos. Returns -1 if no header is found
off_t splt_mp3_findhead(splt_mp3_state *mp3state, off_t start)
{
  if (splt_io_get_word(mp3state->file_input, 
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

//! Finds first valid header from start. Will work with high probabilty, i hope :)
off_t splt_mp3_findvalidhead(splt_mp3_state *mp3state, off_t start)
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

//!finds xing info offset and returns it?
int splt_mp3_xing_info_off(splt_mp3_state *mp3state)
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

/*! Get a frame

\return  negative value means: error
*/
int splt_mp3_get_frame(splt_mp3_state *mp3state)
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
    mp3state->stream.error = MAD_ERROR_NONE;
  }

  //mad_frame_decode() returns -1 if error, 0 if no error
  return mad_frame_decode(&mp3state->frame, &mp3state->stream);
}

/*! used by mp3split and mp3_scan_silence

gets a frame and checks for its validity; sets the mp3state->data_ptr
the pointer to the frame  and the mp3state->data_len the length of the
frame 

\param state The central structure libmp3splt keeps all its data in
\param error Contains the error number for libmp3splt

\return  
 - 1 if ok, 
 - -1 if end of file, 
 - 0 if nothing (???) 
 - and -3 if other error; 
 .
On error the error number will be set in the '*error' parameter
*/
int splt_mp3_get_valid_frame(splt_state *state, int *error)
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
        if ((mp3state->syncdetect) && (state->syncerrors>SPLT_MAXSYNC))
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
          splt_e_set_error_data(state, mad_stream_errorstr(&mp3state->stream));
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

