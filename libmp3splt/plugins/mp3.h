/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2014 Alexandru Munteanu - m@ioalex.net
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

#ifndef MP3SPLT_MP3_H

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

#ifndef NO_ID3TAG
#include <id3tag.h>
#endif

#include <mad.h>

/**********************************/
/* Mp3 structures                 */

#define SPLT_MAD_BSIZE 4032

#define SPLT_MP3_MAX_BYTE_RESERVOIR_HEADERS 30

#ifndef NO_ID3TAG
typedef struct {
  id3_byte_t *tag_bytes;
  id3_length_t tag_length;

  id3_byte_t *tag_bytes_v1;
  id3_length_t tag_length_v1;

  unsigned int version;
  unsigned int bytes_tags_version;
} tag_bytes_and_size;
#endif

struct splt_header {
  off_t ptr;    // offset of header
  int bitrate;
  int padding;
  int framesize;

  int has_crc;
  int sideinfo_size;

  //info from the side info for layer 3
  int main_data_begin;
  int frame_data_space;
};

struct splt_reservoir {
  unsigned char reservoir[511];
  int reservoir_end;
  unsigned char *reservoir_frame;
  unsigned int reservoir_frame_size;
};

// Struct that will contains infos on mp3 and an header struct of first valid header
struct splt_mp3 {
  int mpgid;    // mpgid among SPLT_MP3_MPEG1_ID or SPLT_MP3_MPEG2_ID or SPLT_MP3_MPEG25_ID
  int layer;    // layer 1, 2 or 3
  //0 = single channel
  //1 = dual channel
  //2 = joint stereo
  //3 = stereo
  //4 = other
  int channels;
  int freq;
  int bitrate;
  float fps;
  int samples_per_frame;
  //used for the xing header
  int xing;
  char *xingbuffer;
  off_t xing_offset;
  int xing_content_size;
  int xing_has_frames;
  int xing_has_bytes;
  int xing_has_toc;
  int xing_has_quality;
  int lame_delay;
  int lame_padding;
  //length of the mp3 file
  off_t len;
  //where we begin reading
  off_t firsth;
  struct splt_header firsthead;
};

typedef struct {
  FILE *file_input;
  struct splt_header h;
  //if we are in framemode or not
  short framemode;
  //total frames
  unsigned long frames;
  int syncdetect;
  off_t end;
  unsigned long fend;
  off_t end_non_zero;
  off_t end2;
  off_t bytes;
  int first;
  unsigned long headw;

  unsigned first_frame_header_for_reservoir;
  int is_guessed_vbr;

  //see the mp3 structure
  struct splt_mp3 mp3file;

  //for byte reservoir
  struct splt_header br_headers[SPLT_MP3_MAX_BYTE_RESERVOIR_HEADERS];
  int next_br_header_index;
  int number_of_br_headers_stored;
  struct splt_reservoir reservoir;
  long begin_sample;
  long end_sample;
  long first_frame_inclusive;
  long last_frame_inclusive;

  long overlapped_number_of_frames;
  unsigned char *overlapped_frames;
  size_t overlapped_frames_bytes;

  int new_xing_lame_frame_size;
  unsigned char *new_xing_lame_frame;

  //used internally, libmad structures
  struct mad_stream stream;
  struct mad_frame frame;
  struct mad_synth synth;
  //internally used by the silence detection functions
  mad_fixed_t temp_level;
  //the offset
  float off;
  //used internally when reading the file
  unsigned char inputBuffer[SPLT_MAD_BSIZE];
  //mad timer
  mad_timer_t timer;
  //used internally, pointer to the beginning of a frame
  unsigned char *data_ptr;
  //used internally, length of a frame
  long data_len;
  //length of a buffer when reading a frame
  int buf_len;
} splt_mp3_state;

/****************************/
/* mp3 constants */

/*
   Frame per second:
   Each MPEG1 frame decodes to 1152 PCM
   samples, 576 with MPEG2.
   32000/1152 = 27.77778 = 16000/576
   44100/1152 = 38.28125 = 22050/576
   48000/1152 = 41.66667 = 24000/576
   */

#define SPLT_MP3_TAG "TAG"

#define SPLT_MP3_LAYER1_SAMPLES_PER_FRAME 384
#define SPLT_MP3_LAYER3_MPEG1_AND_LAYER2_SAMPLES_PER_FRAME 1152
#define SPLT_MP3_LAYER3_MPEG2_SAMPLES_PER_FRAME 576

#define SPLT_MP3_NO_END_SAMPLE -10000
#define SPLT_MP3_MIN_OVERLAP_SAMPLES_START SPLT_MP3_LAYER3_MPEG2_SAMPLES_PER_FRAME
#define SPLT_MP3_MIN_OVERLAP_SAMPLES_END SPLT_MP3_LAYER3_MPEG1_AND_LAYER2_SAMPLES_PER_FRAME

#define SPLT_MP3_BYTE 8

#define SPLT_MP3_MPEG1_ID 3
#define SPLT_MP3_MPEG2_ID 2
#define SPLT_MP3_MPEG25_ID 0

#define SPLT_MP3_XING_MAGIC 0x58696E67
#define SPLT_MP3_INFO_MAGIC 0x496E666F

#define SPLT_MP3_XING_FRAMES  0x00000001L
#define SPLT_MP3_XING_BYTES   0x00000002L
#define SPLT_MP3_XING_TOC     0x00000004L
#define SPLT_MP3_XING_QUALITY 0x00000008L

#define SPLT_MP3_XING_FLAGS_SIZE 4

#define SPLT_MP3_LAME_DELAY_OFFSET 21
#define SPLT_MP3_LAME_MAX_DELAY 4095
#define SPLT_MP3_LAME_MAX_PADDING 4095

#define SPLT_MP3_ID3_ARTIST 1
#define SPLT_MP3_ID3_ALBUM 2
#define SPLT_MP3_ID3_TITLE 3
#define SPLT_MP3_ID3_YEAR 4
#define SPLT_MP3_ID3_GENRE 5
#define SPLT_MP3_ID3_TRACK 6
#define SPLT_MP3_ID3_COMMENT 7

#define SPLT_MP3_CRCLEN 4
#define SPLT_MP3_ABWINDEXOFFSET 0x539
#define SPLT_MP3_ABWLEN 0x1f5
#define SPLT_MP3_INDEXVERSION 1
#define SPLT_MP3_READBSIZE 1024

#define SPLT_MP3EXT ".mp3"

//! The layer- and-bitrate-table
static const int splt_mp3_tabsel_123[2][3][16] = {
  { {128,32,64,96,128,160,192,224,256,288,320,352,384,416,448,},
    {128,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,},
    {128,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320,} },

  { {128,32,48,56,64,80,96,112,128,144,160,176,192,224,256,},
    {128,8,16,24,32,40,48,56,64,80,96,112,128,144,160,},
    {128,8,16,24,32,40,48,56,64,80,96,112,128,144,160,} }
};

#define MP3SPLT_MP3_H

#endif

