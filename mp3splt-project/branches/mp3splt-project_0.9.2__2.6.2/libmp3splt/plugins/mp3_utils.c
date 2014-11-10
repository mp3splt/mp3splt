/**********************************************************
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2014 Alexandru Munteanu - m@ioalex.net
 *
 * Bit reservoir fix heavily inspired from pcutmp3.
 * It worth reminding that:
 *    pcutmp3 was created by Sebastian Gesemann
 *    pcutmp3 source code is here
 *       http://wiki.themixingbowl.org/Pcutmp3
 *    pcutmp3 is licensed as BSD
 *       quoting the author from source: http://www.hydrogenaudio.org/forums/index.php?showtopic=35654&pid=569128&mode=threaded&start=100#entry569128
 *          "Let's say BSD. :)" 
 *    See below for the pcutmp3 license
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

/**********************************************************
 * Reminder of the pcutmp3 license:
 *
 * Pcutmp3 Copyright (c) 2005, Sebastian Gesemann
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, are permitted
 * provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright notice, this list of conditions
 * and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions
 * and the following disclaimer in the documentation and/or other materials provided with the
 * distribution.
 *
 * 3. Neither the name of the Sebastian Gesemann nor the names of its contributors may be used to endorse
 * or promote products derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER
 * IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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

struct splt_header splt_mp3_makehead(unsigned long headword, 
    struct splt_mp3 mp3f, struct splt_header head, off_t ptr)
{
  head.ptr = ptr;

  int mpeg_index = splt_mp3_get_mpeg_as_int(mp3f.mpgid) != 1;
  head.bitrate = splt_mp3_tabsel_123[mpeg_index][mp3f.layer-1][splt_mp3_c_bitrate(headword)];
  head.padding = ((headword >> 9) & 0x1);

  int is_mpeg1 = mp3f.mpgid == SPLT_MP3_MPEG1_ID;

  if (mp3f.layer == MAD_LAYER_I)
    head.framesize = (head.bitrate * 12000 / mp3f.freq + head.padding) * 4;
  else if (is_mpeg1 || (mp3f.layer != MAD_LAYER_III))
    head.framesize = head.bitrate * 144000 / mp3f.freq + head.padding;
  else
    head.framesize = head.bitrate * 72000 / mp3f.freq + head.padding;

  head.has_crc = ! ((headword >> 16) & 0x1);

  if (mp3f.layer == 3)
  {
    int is_mono = ((headword >> 6 & 3) == 3); // 3 = mono
    if (is_mpeg1)
      head.sideinfo_size = is_mono ? 17 : 32;
    else
      head.sideinfo_size = is_mono ? 9 : 17;
  }
  else
  {
    head.sideinfo_size = 0;
  }

  //4 is the header size
  head.frame_data_space = head.framesize - head.sideinfo_size - 4;

  return head;
}

static void splt_mp3_store_header(splt_mp3_state *mp3state)
{
  struct splt_header *h = &mp3state->br_headers[mp3state->next_br_header_index];
  h->ptr = mp3state->h.ptr;
  h->bitrate = mp3state->h.bitrate;
  h->padding = mp3state->h.padding;
  h->framesize = mp3state->h.framesize;
  h->has_crc = mp3state->h.has_crc;
  h->sideinfo_size = mp3state->h.sideinfo_size;
  h->main_data_begin = mp3state->h.main_data_begin;
  h->frame_data_space = mp3state->h.frame_data_space;

  mp3state->next_br_header_index++;

  if (mp3state->number_of_br_headers_stored < SPLT_MP3_MAX_BYTE_RESERVOIR_HEADERS)
  {
    mp3state->number_of_br_headers_stored++;
  }

  if (mp3state->next_br_header_index >= SPLT_MP3_MAX_BYTE_RESERVOIR_HEADERS)
  {
    mp3state->next_br_header_index = 0;
  }
}

void splt_mp3_read_process_side_info_main_data_begin(splt_mp3_state *mp3state, off_t offset)
{
  //side info is only for layer 3
  if (mp3state->mp3file.layer != 3) { return; }

  //skip crc
  if (mp3state->h.has_crc)
  {
    fgetc(mp3state->file_input);
    fgetc(mp3state->file_input);
  }

  unsigned int main_data_begin = 0;
  main_data_begin = (unsigned int) fgetc(mp3state->file_input);
  //main_data_begin has 9 bits in MPEG1 and 8 in MPEG2
  if (mp3state->mp3file.mpgid == SPLT_MP3_MPEG1_ID)
  {
    main_data_begin <<= 8;
    main_data_begin |= fgetc(mp3state->file_input);
    main_data_begin >>= 7;
  }

  mp3state->h.main_data_begin = (int) main_data_begin;

  /*fprintf(stdout, "frame size = %d\t sideinfo_size = %d\t main_data_begin = %d\t frame data space = %d\n",
      mp3state->h.framesize, mp3state->h.sideinfo_size, mp3state->h.main_data_begin,
      mp3state->h.frame_data_space);
  fflush(stdout);*/

  splt_mp3_store_header(mp3state);
}

static void splt_mp3_update_existing_xing(splt_mp3_state *mp3state, unsigned long frames, 
    unsigned long bytes)
{
  mp3state->mp3file.xingbuffer[mp3state->mp3file.xing_offset+4] = (frames >> 24) & 0xFF;
  mp3state->mp3file.xingbuffer[mp3state->mp3file.xing_offset+5] = (frames >> 16) & 0xFF;
  mp3state->mp3file.xingbuffer[mp3state->mp3file.xing_offset+6] = (frames >> 8) & 0xFF;
  mp3state->mp3file.xingbuffer[mp3state->mp3file.xing_offset+7] = frames & 0xFF;

  mp3state->mp3file.xingbuffer[mp3state->mp3file.xing_offset+8] = (bytes >> 24) & 0xFF;
  mp3state->mp3file.xingbuffer[mp3state->mp3file.xing_offset+9] = (bytes >> 16) & 0xFF;
  mp3state->mp3file.xingbuffer[mp3state->mp3file.xing_offset+10] = (bytes >> 8) & 0xFF;
  mp3state->mp3file.xingbuffer[mp3state->mp3file.xing_offset+11] = bytes & 0xFF;
}

static int splt_mp3_xing_content_size(splt_mp3_state *mp3state)
{
  struct splt_mp3 *mp3file = &mp3state->mp3file;

  unsigned long xing_flags = 
    (unsigned long) ((mp3file->xingbuffer[mp3file->xing_offset] << 24) | 
        (mp3file->xingbuffer[mp3file->xing_offset + 1] << 16) |
        (mp3file->xingbuffer[mp3file->xing_offset + 2] << 8) |
        (mp3file->xingbuffer[mp3file->xing_offset + 3]));

  int xing_content_size = 0;
  if (xing_flags & SPLT_MP3_XING_FRAMES)
  {
    xing_content_size += 4;
    mp3file->xing_has_frames = SPLT_TRUE;
  }
  if (xing_flags & SPLT_MP3_XING_BYTES)
  {
    xing_content_size += 4;
    mp3file->xing_has_bytes = SPLT_TRUE;
  }
  if (xing_flags & SPLT_MP3_XING_TOC)
  {
    xing_content_size += 100;
    mp3file->xing_has_toc = SPLT_TRUE;
  }
  if (xing_flags & SPLT_MP3_XING_QUALITY)
  {
    xing_content_size += 4;
    mp3file->xing_has_quality = SPLT_TRUE;
  }

  return xing_content_size;
}

static int splt_mp3_xing_frame_has_lame(splt_mp3_state *mp3state)
{
  struct splt_mp3 mp3file = mp3state->mp3file;

  off_t end_xing_offset =
    mp3file.xing_offset + mp3file.xing_content_size + SPLT_MP3_XING_FLAGS_SIZE;

  //4 for the LAME characters
  if (mp3state->mp3file.xing <= end_xing_offset + 4)
  {
    return SPLT_FALSE;
  }

  if (mp3file.xingbuffer[end_xing_offset] == 'L' &&
      mp3file.xingbuffer[end_xing_offset + 1] == 'A' &&
      mp3file.xingbuffer[end_xing_offset + 2] == 'M' &&
      mp3file.xingbuffer[end_xing_offset + 3] == 'E')
  {
    return SPLT_TRUE;
  }

  return SPLT_FALSE;
}

static off_t splt_mp3_get_delay_offset(splt_mp3_state *mp3state)
{
  off_t end_xing_offset =
    mp3state->mp3file.xing_offset + mp3state->mp3file.xing_content_size + SPLT_MP3_XING_FLAGS_SIZE;
  return end_xing_offset + SPLT_MP3_LAME_DELAY_OFFSET;
}

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

int splt_mp3_get_samples_per_frame(struct splt_mp3 *mp3file)
{
  if (mp3file->layer == MAD_LAYER_I)
  {
    return SPLT_MP3_LAYER1_SAMPLES_PER_FRAME;
  }

  if (mp3file->layer == MAD_LAYER_II)
  {
    return SPLT_MP3_LAYER3_MPEG1_AND_LAYER2_SAMPLES_PER_FRAME;
  }

  if (mp3file->mpgid == SPLT_MP3_MPEG1_ID)
  {
    return SPLT_MP3_LAYER3_MPEG1_AND_LAYER2_SAMPLES_PER_FRAME;
  }

  return SPLT_MP3_LAYER3_MPEG2_SAMPLES_PER_FRAME;
}

int splt_mp3_handle_bit_reservoir(splt_state *state)
{
  int with_bit_reservoir = splt_o_get_int_option(state, SPLT_OPT_HANDLE_BIT_RESERVOIR);
  long overlap_time = splt_o_get_long_option(state, SPLT_OPT_OVERLAP_TIME);
  int with_auto_adjust = splt_o_get_int_option(state, SPLT_OPT_AUTO_ADJUST);
  int input_not_seekable = splt_o_get_int_option(state, SPLT_OPT_INPUT_NOT_SEEKABLE);

  int supported_split_mode = SPLT_TRUE;
  int split_mode = splt_o_get_int_option(state, SPLT_OPT_SPLIT_MODE);
  if ((split_mode == SPLT_OPTION_SILENCE_MODE) || (split_mode == SPLT_OPTION_TRIM_SILENCE_MODE))
  {
    supported_split_mode = SPLT_FALSE;
  }

  int with_xing = splt_o_get_int_option(state, SPLT_OPT_XING);
  int with_frame_mode = splt_o_get_int_option(state, SPLT_OPT_FRAME_MODE);

  int handle_bit_reservoir = with_bit_reservoir &&
    overlap_time == 0 && !with_auto_adjust && !input_not_seekable &&
    supported_split_mode && with_xing && with_frame_mode;

  return handle_bit_reservoir;
}

int splt_mp3_get_mpeg_as_int(int mpgid)
{
  if (mpgid == SPLT_MP3_MPEG1_ID) { return 1; }
  if (mpgid == SPLT_MP3_MPEG2_ID) { return 2; }
  return 25;
}

void splt_mp3_parse_xing_lame(splt_mp3_state *mp3state)
{
  mp3state->mp3file.xing_offset = splt_mp3_xing_info_off(mp3state);
  mp3state->mp3file.xing_content_size = splt_mp3_xing_content_size(mp3state);

  if (!splt_mp3_xing_frame_has_lame(mp3state))
  {
    mp3state->mp3file.lame_delay = -1;
    mp3state->mp3file.lame_padding = -1;
    return;
  }

  off_t delay_offset = splt_mp3_get_delay_offset(mp3state);
  char *delay_padding_ptr = &mp3state->mp3file.xingbuffer[delay_offset];

  int first = (int) *delay_padding_ptr;
  int middle = (int) *(delay_padding_ptr + 1);
  int last = (int) *(delay_padding_ptr + 2);

  mp3state->mp3file.lame_delay = ((first & 0xFF) << 4) | (middle >> 4);
  mp3state->mp3file.lame_padding = ((middle & 0xF) << 8) | (last & 0xFF);
}

static unsigned char *splt_mp3_create_new_xing_lame_frame(splt_mp3_state *mp3state, splt_state *state,
    int *frame_size, int *xing_offset, int *end_xing_offset, splt_code *error)
{
  unsigned long frame_header = mp3state->first_frame_header_for_reservoir | 0x00010000; //disable crc
  int frame_header_created = SPLT_FALSE;

  struct splt_header first_frame_header;
  first_frame_header = 
    splt_mp3_makehead(frame_header, mp3state->mp3file, first_frame_header, 0);

  struct splt_header h;

  int i = 1;
  for (; i < 15; i++) {
    unsigned long new_frame_header = ((unsigned long) frame_header & 0xFFFF0FFF) | (i << 12);

    h = splt_mp3_makehead(new_frame_header, mp3state->mp3file, h, 0);
    if (h.framesize < 0xC0) { continue; }

    if (first_frame_header.bitrate == h.bitrate)
    {
      frame_header_created = SPLT_TRUE;
      frame_header = new_frame_header;
      break;
    }
  }

  if (!frame_header_created)
  {
    splt_d_print_debug(state,"Failed to create xing lame frame for bitrate %d \n",
        first_frame_header.bitrate);

    *error = SPLT_ERROR_FAILED_BITRESERVOIR;
    splt_e_set_error_data(state, "failed to create xing lame frame");
    return NULL;
  }

  *frame_size = h.framesize;

  unsigned char *frame = malloc(sizeof(unsigned char) * h.framesize);
  if (frame == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return NULL;
  }

  frame[0] = (unsigned char) (frame_header >> 24);
  frame[1] = (unsigned char) (frame_header >> 16);
  frame[2] = (unsigned char) (frame_header >> 8);
  frame[3] = (unsigned char) frame_header;

  int j = 4;
  for (; j < h.framesize;j++) { frame[j] = 0x0; }

  int xing_tag_offset = 4 + h.sideinfo_size;

  if (mp3state->is_guessed_vbr)
  {
    frame[xing_tag_offset++] = 'X';
    frame[xing_tag_offset++] = 'i';
    frame[xing_tag_offset++] = 'n';
    frame[xing_tag_offset++] = 'g';
  }
  else
  {
    frame[xing_tag_offset++] = 'I';
    frame[xing_tag_offset++] = 'n';
    frame[xing_tag_offset++] = 'f';
    frame[xing_tag_offset++] = 'o';
  }

  *xing_offset = xing_tag_offset;

  frame[xing_tag_offset++] = 0;
  frame[xing_tag_offset++] = 0;
  frame[xing_tag_offset++] = 0;
  frame[xing_tag_offset++] = 0x0F;
  //0x0F = SPLT_MP3_XING_FRAMES | SPLT_MP3_XING_BYTES | SPLT_MP3_XING_TOC | SPLT_MP3_XING_QUALITY

  xing_tag_offset += (100 + 4 + 4 + 4);

  *end_xing_offset = xing_tag_offset;

  frame[xing_tag_offset++] = 'L';
  frame[xing_tag_offset++] = 'A';
  frame[xing_tag_offset++] = 'M';
  frame[xing_tag_offset++] = 'E';

  frame[xing_tag_offset++] = '3';
  frame[xing_tag_offset++] = '.';
  frame[xing_tag_offset++] = '9';
  frame[xing_tag_offset++] = '4';

  return frame;
}

static void splt_mp3_update_delay_and_padding_on_lame_frame(splt_mp3_state *mp3state,
    char *delay_padding_ptr, short reservoir_frame, unsigned long *frames)
{
  int delay = mp3state->mp3file.lame_delay;
  int padding = mp3state->mp3file.lame_padding;

  delay += (mp3state->begin_sample -
      mp3state->first_frame_inclusive * mp3state->mp3file.samples_per_frame);

  long number_of_frames = 0;
  long last_frame = mp3state->last_frame_inclusive;

  if (last_frame == -1 || last_frame > mp3state->frames)
  {
    last_frame = mp3state->frames - 1;
  }

  if (last_frame != mp3state->first_frame_inclusive)
  {
    number_of_frames = last_frame - mp3state->first_frame_inclusive + 1;
  }
  *frames = number_of_frames;

  /*fprintf(stdout, "last frame = %ld\n", last_frame);
  fprintf(stdout, "first frame = %ld\n", mp3state->first_frame_inclusive);
  fflush(stdout);*/

  long number_of_samples = number_of_frames * mp3state->mp3file.samples_per_frame;
  long number_of_samples_to_play = mp3state->end_sample - mp3state->begin_sample;

  padding = number_of_samples - number_of_samples_to_play - delay;

  if (reservoir_frame)
  {
    delay += mp3state->mp3file.samples_per_frame;
    *frames = *frames + 1;
  }

  if (delay > SPLT_MP3_LAME_MAX_DELAY) { delay = SPLT_MP3_LAME_MAX_DELAY; }
  if (padding > SPLT_MP3_LAME_MAX_PADDING) { padding = SPLT_MP3_LAME_MAX_PADDING; }
  if (delay < 0) { delay = 0; }
  if (padding < 0) { padding = 0; }

  *delay_padding_ptr = (char) (delay >> 4);
  *(delay_padding_ptr + 1) = (char) (delay & 0xF) << 4 | (padding >> 8);
  *(delay_padding_ptr + 2) = (char) padding;
}

void splt_mp3_build_xing_lame_frame(splt_mp3_state *mp3state, off_t begin, off_t end, 
    unsigned long fbegin, splt_code *error, splt_state *state)
{
  short reservoir_frame = 0;
  short reservoir_bytes = 0;
  if (mp3state->reservoir.reservoir_frame != NULL)
  {
    reservoir_frame = 1;
    reservoir_bytes = mp3state->reservoir.reservoir_frame_size;
  }

  if (end == -1) { end = mp3state->mp3file.len; }

  unsigned long frames = (unsigned long) mp3state->frames - fbegin;
  unsigned long bytes = (unsigned long) (end - begin + reservoir_bytes + mp3state->overlapped_frames_bytes);

  if (!splt_mp3_handle_bit_reservoir(state))
  {
    bytes += mp3state->mp3file.xing;

    if (mp3state->mp3file.xing > 0)
    {
      splt_mp3_update_existing_xing(mp3state, frames, bytes);
    }

    return;
  }

  if (mp3state->mp3file.xing <= 0)
  {
    int xing_offset = 0;
    int end_xing_offset = 0;
    int frame_size = 0;
    unsigned char *frame = splt_mp3_create_new_xing_lame_frame(mp3state, state, &frame_size,
        &xing_offset, &end_xing_offset, error);
    if (*error < 0) { return; }

    char *delay_padding_ptr = (char *) &frame[end_xing_offset + SPLT_MP3_LAME_DELAY_OFFSET];
    splt_mp3_update_delay_and_padding_on_lame_frame(mp3state, delay_padding_ptr, reservoir_frame, &frames);

    frame[xing_offset + 4] = (frames >> 24) & 0xFF;
    frame[xing_offset + 5] = (frames >> 16) & 0xFF;
    frame[xing_offset + 6] = (frames >> 8) & 0xFF;
    frame[xing_offset + 7] = frames & 0xFF;

    bytes += frame_size;

    frame[xing_offset + 8] = (bytes >> 24) & 0xFF;
    frame[xing_offset + 9] = (bytes >> 16) & 0xFF;
    frame[xing_offset + 10] = (bytes >> 8) & 0xFF;
    frame[xing_offset + 11] = bytes & 0xFF;

    if (mp3state->new_xing_lame_frame)
    {
      free(mp3state->new_xing_lame_frame);
    }

    mp3state->new_xing_lame_frame_size = frame_size;
    mp3state->new_xing_lame_frame = frame;

    return;
  }

  if (splt_mp3_xing_frame_has_lame(mp3state))
  {
    bytes += mp3state->mp3file.xing;

    char *delay_padding_ptr = &mp3state->mp3file.xingbuffer[splt_mp3_get_delay_offset(mp3state)];
    splt_mp3_update_delay_and_padding_on_lame_frame(mp3state, delay_padding_ptr, reservoir_frame, &frames);
    splt_mp3_update_existing_xing(mp3state, frames, bytes);
    return;
  }

  if (mp3state->mp3file.xing > 0)
  {
    *error = SPLT_ERROR_FAILED_BITRESERVOIR;
    splt_e_set_error_data(state, "input files with Xing frame without LAME not yet supported");
  }

  //TODO2: update lame crc16
}

static int splt_mp3_current_br_header_index(splt_mp3_state *mp3state)
{
  int current_header_index = mp3state->next_br_header_index - 1;
  if (current_header_index < 0)
  {
    current_header_index = SPLT_MP3_MAX_BYTE_RESERVOIR_HEADERS - 1;
  }

  return current_header_index;
}

static int splt_mp3_previous_br_header_index(splt_mp3_state *mp3state, int current_header_index)
{
  int header_index = current_header_index - 1;
  if (header_index < 0)
  {
    header_index = SPLT_MP3_MAX_BYTE_RESERVOIR_HEADERS - 1;
  }

  return header_index;
}

static void splt_mp3_back_br_header_index(splt_mp3_state *mp3state)
{
  mp3state->next_br_header_index--;
  if (mp3state->next_br_header_index < 0)
  {
    mp3state->next_br_header_index = SPLT_MP3_MAX_BYTE_RESERVOIR_HEADERS - 1;
  }
}

void splt_mp3_get_overlapped_frames(long last_frame, splt_mp3_state *mp3state,
    splt_state *state, splt_code *error)
{
  if (last_frame <= 0) { return; }

  long number_of_frames_to_be_overlapped = last_frame - mp3state->first_frame_inclusive + 1;

  /*fprintf(stdout, "frames to be overlapped = %ld\n", number_of_frames_to_be_overlapped);
  fflush(stdout);*/

  int current_header_index = splt_mp3_current_br_header_index(mp3state);
  mp3state->overlapped_frames_bytes = 0;

  int index = 0;
  off_t frame_offsets[SPLT_MP3_MAX_BYTE_RESERVOIR_HEADERS] = { 0 };
  int frame_sizes[SPLT_MP3_MAX_BYTE_RESERVOIR_HEADERS] = { 0 };

  int i = 0;
  for (;i < number_of_frames_to_be_overlapped; i++)
  {
    current_header_index = splt_mp3_previous_br_header_index(mp3state, current_header_index);

    mp3state->overlapped_frames_bytes += mp3state->br_headers[current_header_index].framesize;
    frame_offsets[index] = mp3state->br_headers[current_header_index].ptr;
    frame_sizes[index] = mp3state->br_headers[current_header_index].framesize;
    index++;
    mp3state->overlapped_number_of_frames++;
  }

  off_t previous_position = ftello(mp3state->file_input);

  if (mp3state->overlapped_frames) {
    free(mp3state->overlapped_frames);
  }
  mp3state->overlapped_frames = malloc(sizeof(unsigned char) * mp3state->overlapped_frames_bytes);
  if (mp3state->overlapped_frames == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return;
  }

  long current_index_in_frames = 0;
  for (i = index - 1;i >= 0; i--)
  {
    off_t frame_offset = frame_offsets[i];

    if (fseeko(mp3state->file_input, frame_offset, SEEK_SET) == -1)
    {
      splt_e_set_strerror_msg_with_data(state, splt_t_get_filename_to_split(state));
      *error = SPLT_ERROR_SEEKING_FILE;
      return;
    }

    int frame_size = frame_sizes[i];
    unsigned char *frame = splt_io_fread(mp3state->file_input, frame_size);
    if (!frame)
    {
      splt_e_clean_strerror_msg(state);
      splt_e_set_error_data(state, splt_t_get_filename_to_split(state));
      *error = SPLT_ERROR_WHILE_READING_FILE;
      return;
    }

    memcpy(mp3state->overlapped_frames + current_index_in_frames, frame, frame_size);
    current_index_in_frames += frame_size;
    free(frame);

    splt_mp3_back_br_header_index(mp3state);
  }

  if (fseeko(mp3state->file_input, previous_position, SEEK_SET) == -1)
  {
    splt_e_set_strerror_msg_with_data(state, splt_t_get_filename_to_split(state));
    *error = SPLT_ERROR_SEEKING_FILE;
    return;
  }

  /*fprintf(stdout, "overlapped frames bytes follows:\n");
  int j = 0;
  for (j = 0;j < mp3state->overlapped_frames_bytes;j++)
  {
    fprintf(stdout, "%02x ", mp3state->overlapped_frames[j]);
    fflush(stdout);
  }
  fprintf(stdout, "\n");
  fflush(stdout);*/
}

unsigned long splt_mp3_find_begin_frame(double fbegin_sec, splt_mp3_state *mp3state,
    splt_state *state, splt_code *error)
{
  unsigned long without_bit_reservoir_begin_frame = 
    (unsigned long) (fbegin_sec * mp3state->mp3file.fps);

  if (!splt_mp3_handle_bit_reservoir(state))
  {
    return without_bit_reservoir_begin_frame;
  }

  /*fprintf(stdout, "fbegin sec = %ld\n", fbegin_sec);
  fflush(stdout);*/

  long begin_sample = (long) rint((double) fbegin_sec * (double) mp3state->mp3file.freq);
  mp3state->begin_sample = begin_sample;

  long first_frame_inclusive = (long)
    ((begin_sample + mp3state->mp3file.lame_delay - SPLT_MP3_MIN_OVERLAP_SAMPLES_START)
     / mp3state->mp3file.samples_per_frame);
  if (first_frame_inclusive < 0) { first_frame_inclusive = 0; }

  /*fprintf(stdout, "begin_sample = %ld\n", begin_sample);
  fprintf(stdout, "first frame inclusive = %ld\n", first_frame_inclusive);
  fflush(stdout);*/

  mp3state->first_frame_inclusive = first_frame_inclusive;

  long last_frame = mp3state->last_frame_inclusive;
  splt_mp3_get_overlapped_frames(last_frame, mp3state, state, error);
  if (*error < 0) { return 0; }

  return (unsigned long) first_frame_inclusive;
}

unsigned long splt_mp3_find_end_frame(double fend_sec, splt_mp3_state *mp3state,
    splt_state *state)
{
  if (!splt_mp3_handle_bit_reservoir(state))
  {
    //prefer to split a bit after the end than loosing some frame
    //before the end
    return (unsigned long) ceilf(fend_sec * mp3state->mp3file.fps);
  }

  long end_sample = (long) rint((double) fend_sec * (double) mp3state->mp3file.freq);
  if (end_sample < 0) { end_sample = 0; }
  mp3state->end_sample = end_sample;

  long last_frame_inclusive = (long)
    ((end_sample + mp3state->mp3file.lame_delay + SPLT_MP3_MIN_OVERLAP_SAMPLES_END)
     / mp3state->mp3file.samples_per_frame);

  mp3state->last_frame_inclusive = last_frame_inclusive;

  /*fprintf(stdout, "samples_per_frame = %d\n", mp3state->mp3file.samples_per_frame);
  fprintf(stdout, "lame_delay = %d\n", mp3state->mp3file.lame_delay);
  fprintf(stdout, "end_sample = %ld\n", end_sample);
  fprintf(stdout, "computed end frame = %ld\n", last_frame_inclusive + 1);
  fflush(stdout);*/

  return (unsigned long) (last_frame_inclusive + 1);
}

static void splt_mp3_extract_reservoir_main_data_bytes(splt_mp3_state *mp3state, splt_state *state, splt_code *error)
{
  //reservoir is only for layer 3
  if (mp3state->mp3file.layer != 3) { return; }

  int number_of_headers_stored = mp3state->number_of_br_headers_stored;

  int current_header_index = splt_mp3_current_br_header_index(mp3state);
  struct splt_header *h = &mp3state->br_headers[current_header_index];
  int back_pointer = h->main_data_begin;

  /*fprintf(stdout, "back pointer = %d\n", back_pointer);
  fflush(stdout);*/

  if (back_pointer > 511)
  {
    splt_e_set_error_data(state, splt_t_get_filename_to_split(state));
    *error = SPLT_ERROR_INVALID;
    return;
  }

  off_t previous_position = ftello(mp3state->file_input);

  unsigned char **data_from_frames = malloc(sizeof(unsigned char *) * SPLT_MP3_MAX_BYTE_RESERVOIR_HEADERS);
  int *bytes_to_copy = malloc(sizeof(int *) * SPLT_MP3_MAX_BYTE_RESERVOIR_HEADERS);
  if (data_from_frames == NULL || bytes_to_copy == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return;
  }

  int is_first_file = splt_t_get_current_split_file_number(state) == 1;

  int number_of_frames = 0;
  while (back_pointer > 0)
  {
    current_header_index = splt_mp3_previous_br_header_index(mp3state, current_header_index);
    number_of_headers_stored--;

    //this might happend for the first file, but we don't care for the first file
    //(for example when splitting starting at ~7 hundreths of seconds - at frame number ~2)
    if ((number_of_headers_stored < 0) && !is_first_file)
    {
      splt_e_set_error_data(state, "Bit reservoir number of headers stored is negative !");
      *error = SPLT_ERROR_INVALID_CODE;
      goto end;
    }

    h = &mp3state->br_headers[current_header_index];

    if (h->frame_data_space == 0) { continue; }

    unsigned int number_of_bytes_to_copy = h->frame_data_space;
    if (back_pointer < h->frame_data_space)
    {
      number_of_bytes_to_copy = back_pointer;
    }

    off_t frame_main_data_offset = h->ptr + h->sideinfo_size + 4;
    if (number_of_bytes_to_copy < h->frame_data_space)
    {
      frame_main_data_offset += h->frame_data_space - number_of_bytes_to_copy;
    }

    /*fprintf(stdout, "Copying %d bytes into reservoir\n", number_of_bytes_to_copy);
    fflush(stdout);*/

    if (fseeko(mp3state->file_input, frame_main_data_offset, SEEK_SET) == -1)
    {
      splt_e_set_strerror_msg_with_data(state, splt_t_get_filename_to_split(state));
      *error = SPLT_ERROR_SEEKING_FILE;
      goto end;
    }

    unsigned char *data_from_frame = splt_io_fread(mp3state->file_input, number_of_bytes_to_copy);
    if (data_from_frame)
    {
      /*unsigned int it = 0;
      for (;it < number_of_bytes_to_copy;it++)
      {
        fprintf(stdout, "%02x ", data_from_frame[it]);
      }
      fprintf(stdout, "\n");
      fflush(stdout);*/

      data_from_frames[number_of_frames] = data_from_frame;
      bytes_to_copy[number_of_frames] = number_of_bytes_to_copy;
      number_of_frames++;
    }
    else
    {
      splt_e_clean_strerror_msg(state);
      splt_e_set_error_data(state, splt_t_get_filename_to_split(state));
      *error = SPLT_ERROR_WHILE_READING_FILE;
      goto end;
    }

    back_pointer -= number_of_bytes_to_copy;
  }

  struct splt_reservoir *res = &mp3state->reservoir;
  res->reservoir_end = 0;

  number_of_frames--;
  for (;number_of_frames >= 0; number_of_frames--)
  {
    unsigned char *data_from_frame = data_from_frames[number_of_frames];
    int number_of_bytes_to_copy = bytes_to_copy[number_of_frames];

    memcpy(res->reservoir + res->reservoir_end, data_from_frame, number_of_bytes_to_copy);
    res->reservoir_end += number_of_bytes_to_copy;
    free(data_from_frame);
  }

  /*unsigned int index = 0;
  fprintf(stdout, "reservoir= _");
  for (;index < res->reservoir_end;index++)
  {
    fprintf(stdout, "%02x ", res->reservoir[index]);
  }
  fprintf(stdout, "_\n");
  fflush(stdout);*/

  if (res->reservoir_end > 0)
  {
    if (fseeko(mp3state->file_input, previous_position, SEEK_SET) == -1)
    {
      splt_e_set_strerror_msg_with_data(state, splt_t_get_filename_to_split(state));
      *error = SPLT_ERROR_SEEKING_FILE;
    }
  }

end:
  free(bytes_to_copy);
  free(data_from_frames);
}

static void splt_mp3_build_reservoir_frame(splt_mp3_state *mp3state, splt_state *state, splt_code *error)
{
  struct splt_reservoir *res = &mp3state->reservoir;

  if (res->reservoir_end == 0)
  {
    if (res->reservoir_frame) { free(res->reservoir_frame); }
    res->reservoir_frame = NULL;
    res->reservoir_frame_size = 0;
    return;
  }

  unsigned long first_frame_header = mp3state->headw;
  first_frame_header |= 0x00010000; //set protection bit to 1 in order to turn off CRC

  int bytes_in_main_data = res->reservoir_end + 4; //keep 4 extra bytes

  int bitrate_mask = 1;
  for (;bitrate_mask <= 14; bitrate_mask++)
  {
    unsigned long header = (first_frame_header & 0xFFFF0FFF) + (bitrate_mask << 12);
    struct splt_header h;
    h = splt_mp3_makehead(header, mp3state->mp3file, h, 0);

    if (h.frame_data_space < bytes_in_main_data)
    {
      continue;
    }

    unsigned char *frame = malloc(sizeof(unsigned char) * h.framesize);
    if (frame == NULL)
    {
      *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      return;
    }

    frame[0] = (unsigned char) (header >> 24);
    frame[1] = (unsigned char) (header >> 16);
    frame[2] = (unsigned char) (header >> 8);
    frame[3] = (unsigned char) header;

    int i = 4;
    for (; i < 4 + h.sideinfo_size;i++) { frame[i] = 0x0; }
    for (; i < h.framesize;i++) { frame[i] = 0x78; }

    memcpy(frame + h.framesize - res->reservoir_end, res->reservoir, res->reservoir_end);

    if (res->reservoir_frame) { free(res->reservoir_frame); }
    res->reservoir_frame = frame;
    res->reservoir_frame_size = h.framesize;

    return;
  }
}

void splt_mp3_extract_reservoir_and_build_reservoir_frame(splt_mp3_state *mp3state,
    splt_state *state, splt_code *error)
{
  if (!splt_mp3_handle_bit_reservoir(state))
  {
    return;
  }

  splt_mp3_extract_reservoir_main_data_bytes(mp3state, state, error);
  if (*error < 0) { return; }

  splt_mp3_build_reservoir_frame(mp3state, state, error);

  struct splt_reservoir *reservoir = &mp3state->reservoir;
  if (reservoir->reservoir_frame == NULL)
  {
    /*fprintf(stdout, "reservoir frame follows : NO reservoir frame\n");
    fflush(stdout);*/
    return;
  }

  /*fprintf(stdout, "reservoir frame follows : _ ");
  int i = 0;
  for (i = 0;i < reservoir->reservoir_frame_size; i++)
  {
    fprintf(stdout, "%02x ", reservoir->reservoir_frame[i]);
  }
  fprintf(stdout, "_\n");
  fflush(stdout);*/
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

