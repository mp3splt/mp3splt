/* libFLAC - Free Lossless Audio Codec library
 * Copyright (C) 2000,2001,2002,2003,2004,2005,2006,2007  Josh Coalson
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * - Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * - Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * - Neither the name of the Xiph.org Foundation nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Alexandru Munteanu's note : copied from libflac and modified
 * File part of the libmp3splt flac plugin.
 *
 * Copyright (c) 2014 Alexandru Munteanu - <m@ioalex.net>
 *
 * http://mp3splt.sourceforge.net
 */

/**********************************************************
 *
 * Libmp3splt license:
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

#include "flac_utils.h"
#include "from_flac_library.h"

#include <string.h>

/* CRC-8, poly = x^8 + x^2 + x^1 + x^0, init = 0 */
unsigned char const splt_flac_l_crc8_table[256] = {
	0x00, 0x07, 0x0E, 0x09, 0x1C, 0x1B, 0x12, 0x15,
	0x38, 0x3F, 0x36, 0x31, 0x24, 0x23, 0x2A, 0x2D,
	0x70, 0x77, 0x7E, 0x79, 0x6C, 0x6B, 0x62, 0x65,
	0x48, 0x4F, 0x46, 0x41, 0x54, 0x53, 0x5A, 0x5D,
	0xE0, 0xE7, 0xEE, 0xE9, 0xFC, 0xFB, 0xF2, 0xF5,
	0xD8, 0xDF, 0xD6, 0xD1, 0xC4, 0xC3, 0xCA, 0xCD,
	0x90, 0x97, 0x9E, 0x99, 0x8C, 0x8B, 0x82, 0x85,
	0xA8, 0xAF, 0xA6, 0xA1, 0xB4, 0xB3, 0xBA, 0xBD,
	0xC7, 0xC0, 0xC9, 0xCE, 0xDB, 0xDC, 0xD5, 0xD2,
	0xFF, 0xF8, 0xF1, 0xF6, 0xE3, 0xE4, 0xED, 0xEA,
	0xB7, 0xB0, 0xB9, 0xBE, 0xAB, 0xAC, 0xA5, 0xA2,
	0x8F, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9D, 0x9A,
	0x27, 0x20, 0x29, 0x2E, 0x3B, 0x3C, 0x35, 0x32,
	0x1F, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0D, 0x0A,
	0x57, 0x50, 0x59, 0x5E, 0x4B, 0x4C, 0x45, 0x42,
	0x6F, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7D, 0x7A,
	0x89, 0x8E, 0x87, 0x80, 0x95, 0x92, 0x9B, 0x9C,
	0xB1, 0xB6, 0xBF, 0xB8, 0xAD, 0xAA, 0xA3, 0xA4,
	0xF9, 0xFE, 0xF7, 0xF0, 0xE5, 0xE2, 0xEB, 0xEC,
	0xC1, 0xC6, 0xCF, 0xC8, 0xDD, 0xDA, 0xD3, 0xD4,
	0x69, 0x6E, 0x67, 0x60, 0x75, 0x72, 0x7B, 0x7C,
	0x51, 0x56, 0x5F, 0x58, 0x4D, 0x4A, 0x43, 0x44,
	0x19, 0x1E, 0x17, 0x10, 0x05, 0x02, 0x0B, 0x0C,
	0x21, 0x26, 0x2F, 0x28, 0x3D, 0x3A, 0x33, 0x34,
	0x4E, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5C, 0x5B,
	0x76, 0x71, 0x78, 0x7F, 0x6A, 0x6D, 0x64, 0x63,
	0x3E, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2C, 0x2B,
	0x06, 0x01, 0x08, 0x0F, 0x1A, 0x1D, 0x14, 0x13,
	0xAE, 0xA9, 0xA0, 0xA7, 0xB2, 0xB5, 0xBC, 0xBB,
	0x96, 0x91, 0x98, 0x9F, 0x8A, 0x8D, 0x84, 0x83,
	0xDE, 0xD9, 0xD0, 0xD7, 0xC2, 0xC5, 0xCC, 0xCB,
	0xE6, 0xE1, 0xE8, 0xEF, 0xFA, 0xFD, 0xF4, 0xF3
};

/* CRC-16, poly = x^16 + x^15 + x^2 + x^0, init = 0 */
unsigned splt_flac_l_crc16_table[256] = {
	0x0000,  0x8005,  0x800f,  0x000a,  0x801b,  0x001e,  0x0014,  0x8011,
	0x8033,  0x0036,  0x003c,  0x8039,  0x0028,  0x802d,  0x8027,  0x0022,
	0x8063,  0x0066,  0x006c,  0x8069,  0x0078,  0x807d,  0x8077,  0x0072,
	0x0050,  0x8055,  0x805f,  0x005a,  0x804b,  0x004e,  0x0044,  0x8041,
	0x80c3,  0x00c6,  0x00cc,  0x80c9,  0x00d8,  0x80dd,  0x80d7,  0x00d2,
	0x00f0,  0x80f5,  0x80ff,  0x00fa,  0x80eb,  0x00ee,  0x00e4,  0x80e1,
	0x00a0,  0x80a5,  0x80af,  0x00aa,  0x80bb,  0x00be,  0x00b4,  0x80b1,
	0x8093,  0x0096,  0x009c,  0x8099,  0x0088,  0x808d,  0x8087,  0x0082,
	0x8183,  0x0186,  0x018c,  0x8189,  0x0198,  0x819d,  0x8197,  0x0192,
	0x01b0,  0x81b5,  0x81bf,  0x01ba,  0x81ab,  0x01ae,  0x01a4,  0x81a1,
	0x01e0,  0x81e5,  0x81ef,  0x01ea,  0x81fb,  0x01fe,  0x01f4,  0x81f1,
	0x81d3,  0x01d6,  0x01dc,  0x81d9,  0x01c8,  0x81cd,  0x81c7,  0x01c2,
	0x0140,  0x8145,  0x814f,  0x014a,  0x815b,  0x015e,  0x0154,  0x8151,
	0x8173,  0x0176,  0x017c,  0x8179,  0x0168,  0x816d,  0x8167,  0x0162,
	0x8123,  0x0126,  0x012c,  0x8129,  0x0138,  0x813d,  0x8137,  0x0132,
	0x0110,  0x8115,  0x811f,  0x011a,  0x810b,  0x010e,  0x0104,  0x8101,
	0x8303,  0x0306,  0x030c,  0x8309,  0x0318,  0x831d,  0x8317,  0x0312,
	0x0330,  0x8335,  0x833f,  0x033a,  0x832b,  0x032e,  0x0324,  0x8321,
	0x0360,  0x8365,  0x836f,  0x036a,  0x837b,  0x037e,  0x0374,  0x8371,
	0x8353,  0x0356,  0x035c,  0x8359,  0x0348,  0x834d,  0x8347,  0x0342,
	0x03c0,  0x83c5,  0x83cf,  0x03ca,  0x83db,  0x03de,  0x03d4,  0x83d1,
	0x83f3,  0x03f6,  0x03fc,  0x83f9,  0x03e8,  0x83ed,  0x83e7,  0x03e2,
	0x83a3,  0x03a6,  0x03ac,  0x83a9,  0x03b8,  0x83bd,  0x83b7,  0x03b2,
	0x0390,  0x8395,  0x839f,  0x039a,  0x838b,  0x038e,  0x0384,  0x8381,
	0x0280,  0x8285,  0x828f,  0x028a,  0x829b,  0x029e,  0x0294,  0x8291,
	0x82b3,  0x02b6,  0x02bc,  0x82b9,  0x02a8,  0x82ad,  0x82a7,  0x02a2,
	0x82e3,  0x02e6,  0x02ec,  0x82e9,  0x02f8,  0x82fd,  0x82f7,  0x02f2,
	0x02d0,  0x82d5,  0x82df,  0x02da,  0x82cb,  0x02ce,  0x02c4,  0x82c1,
	0x8243,  0x0246,  0x024c,  0x8249,  0x0258,  0x825d,  0x8257,  0x0252,
	0x0270,  0x8275,  0x827f,  0x027a,  0x826b,  0x026e,  0x0264,  0x8261,
	0x0220,  0x8225,  0x822f,  0x022a,  0x823b,  0x023e,  0x0234,  0x8231,
	0x8213,  0x0216,  0x021c,  0x8219,  0x0208,  0x820d,  0x8207,  0x0202
};

uint32_t splt_flac_l_read_utf8_uint32(void *flac_frame_reader, splt_code *error, 
    unsigned char *number_of_bytes)
{
  uint32_t val = 0;
  unsigned i;

  uint32_t byte = splt_flac_u_read_next_byte_(flac_frame_reader, error);
  if (*error < 0) { *number_of_bytes = 0; return 0xffffffff; }
  *number_of_bytes = 1;

  if(!(byte & 0x80)) { /* 0xxxxxxx */
    val = byte;
    i = 0;
  }
  else if(byte & 0xC0 && !(byte & 0x20)) { /* 110xxxxx */
    val = byte & 0x1F;
    i = 1;
  }
  else if(byte & 0xE0 && !(byte & 0x10)) { /* 1110xxxx */
    val = byte & 0x0F;
    i = 2;
  }
  else if(byte & 0xF0 && !(byte & 0x08)) { /* 11110xxx */
    val = byte & 0x07;
    i = 3;
  }
  else if(byte & 0xF8 && !(byte & 0x04)) { /* 111110xx */
    val = byte & 0x03;
    i = 4;
  }
  else if(byte & 0xFC && !(byte & 0x02)) { /* 1111110x */
    val = byte & 0x01;
    i = 5;
  }
  else {
    *number_of_bytes = 0;
    return 0xffffffff;
  }

  for ( ; i; i--) {
    byte = splt_flac_u_read_next_byte_(flac_frame_reader, error);
    if (*error < 0) { *number_of_bytes = 0; return 0xffffffff; }
    *number_of_bytes = *number_of_bytes + 1;

    if(!(byte & 0x80) || (byte & 0x40)) { /* 10xxxxxx */
      *number_of_bytes = 0;
      return 0xffffffff;
    }

    val <<= 6;
    val |= (byte & 0x3F);
  }

  return val;
}

uint64_t splt_flac_l_read_utf8_uint64(void *flac_frame_reader, splt_code *error,
    unsigned char *number_of_bytes)
{
  uint64_t val = 0;
  unsigned i;

  uint32_t byte = splt_flac_u_read_next_byte_(flac_frame_reader, error);
  if (*error < 0) { *number_of_bytes = 0; return 0xffffffffffffffffLLU; }
  *number_of_bytes = 1;

  if(!(byte & 0x80)) { /* 0xxxxxxx */
    val = byte;
    i = 0;
  }
  else if(byte & 0xC0 && !(byte & 0x20)) { /* 110xxxxx */
    val = byte & 0x1F;
    i = 1;
  }
  else if(byte & 0xE0 && !(byte & 0x10)) { /* 1110xxxx */
    val = byte & 0x0F;
    i = 2;
  }
  else if(byte & 0xF0 && !(byte & 0x08)) { /* 11110xxx */
    val = byte & 0x07;
    i = 3;
  }
  else if(byte & 0xF8 && !(byte & 0x04)) { /* 111110xx */
    val = byte & 0x03;
    i = 4;
  }
  else if(byte & 0xFC && !(byte & 0x02)) { /* 1111110x */
    val = byte & 0x01;
    i = 5;
  }
  else if(byte & 0xFE && !(byte & 0x01)) { /* 11111110 */
    val = 0;
    i = 6;
  }
  else {
    *number_of_bytes = 0; 
    return 0xffffffffffffffffLLU;
  }

  for ( ; i; i--) {
    byte = splt_flac_u_read_next_byte_(flac_frame_reader, error);
    if (*error < 0) { *number_of_bytes = 0; return 0xffffffffffffffffLLU; }
    *number_of_bytes = *number_of_bytes + 1;

    if (!(byte & 0x80) || (byte & 0x40)) { /* 10xxxxxx */
      *number_of_bytes = 0;
      return 0xffffffffffffffffLLU;
    }

    val <<= 6;
    val |= (byte & 0x3F);
  }

  return val;
}

FLAC__uint32 splt_flac_l_unpack_uint32(FLAC__byte *b, unsigned bytes)
{
  FLAC__uint32 ret = 0;
  unsigned i;

  for(i = 0; i < bytes; i++)
    ret = (ret << 8) | (FLAC__uint32)(*b++);

  return ret;
}

static FLAC__uint64 splt_flac_l_unpack_uint64_(FLAC__byte *b, unsigned bytes)
{
  FLAC__uint64 ret = 0;
  unsigned i;

  for(i = 0; i < bytes; i++)
    ret = (ret << 8) | (FLAC__uint64)(*b++);

  return ret;
}

void splt_flac_l_pack_uint32(FLAC__uint32 val, FLAC__byte *b, unsigned bytes)
{
  unsigned i;

  b += bytes;

  for(i = 0; i < bytes; i++) {
    *(--b) = (FLAC__byte)(val & 0xff);
    val >>= 8;
  }
}

FLAC__uint32 splt_flac_l_unpack_uint32_little_endian(FLAC__byte *b, unsigned bytes)
{
  FLAC__uint32 ret = 0;
  unsigned i;

  b += bytes;

  for(i = 0; i < bytes; i++)
    ret = (ret << 8) | (FLAC__uint32)(*--b);

  return ret;
}

void splt_flac_l_pack_uint32_little_endian(FLAC__uint32 val, FLAC__byte *b, unsigned bytes)
{
  unsigned i;

  for(i = 0; i < bytes; i++) {
    *(b++) = (FLAC__byte)(val & 0xff);
    val >>= 8;
  }
}

void splt_flac_l_convert_to_streaminfo(FLAC__StreamMetadata_StreamInfo *block, unsigned char *bytes)
{
  FLAC__byte *b = bytes;

  block->min_blocksize = splt_flac_l_unpack_uint32(b, 2); b += 2;
  block->max_blocksize = splt_flac_l_unpack_uint32(b, 2); b += 2;
  block->min_framesize = splt_flac_l_unpack_uint32(b, 3); b += 3;
  block->max_framesize = splt_flac_l_unpack_uint32(b, 3); b += 3;
  block->sample_rate = (splt_flac_l_unpack_uint32(b, 2) << 4) | ((unsigned)(b[2] & 0xf0) >> 4);
  block->channels = (unsigned)((b[2] & 0x0e) >> 1) + 1;
  block->bits_per_sample = ((((unsigned)(b[2] & 0x01)) << 4) | (((unsigned)(b[3] & 0xf0)) >> 4)) + 1;
  block->total_samples = (((FLAC__uint64)(b[3] & 0x0f)) << 32) | splt_flac_l_unpack_uint64_(b+4, 4);
  memcpy(block->md5sum, b+8, 16);
}

unsigned char *splt_flac_l_convert_from_streaminfo(FLAC__StreamMetadata_StreamInfo *block)
{
  unsigned char *bytes = malloc(SPLT_FLAC_STREAMINFO_LENGTH);
  if (bytes == NULL) { return NULL; }

  const unsigned channels1 = block->channels - 1;
  const unsigned bps1 = block->bits_per_sample - 1;

  splt_flac_l_pack_uint32(block->min_blocksize, bytes, 2);
  splt_flac_l_pack_uint32(block->max_blocksize, bytes+2, 2);
  splt_flac_l_pack_uint32(block->min_framesize, bytes+4, 3);
  splt_flac_l_pack_uint32(block->max_framesize, bytes+7, 3);
  bytes[10] = (block->sample_rate >> 12) & 0xff;
  bytes[11] = (block->sample_rate >> 4) & 0xff;
  bytes[12] = ((block->sample_rate & 0x0f) << 4) | (channels1 << 1) | (bps1 >> 4);
  bytes[13] = (FLAC__byte)(((bps1 & 0x0f) << 4) | ((block->total_samples >> 32) & 0x0f));
  splt_flac_l_pack_uint32((FLAC__uint32)block->total_samples, bytes+14, 4);
  memcpy(bytes+18, block->md5sum, 16);

  return bytes;
}

unsigned char *splt_flac_l_convert_to_utf8(FLAC__uint64 val, unsigned char *utf8_used_bytes)
{
  unsigned char utf8[7] = { '\0' };
  int used_bytes = 0;

  if(val < 0x80) {
    utf8[0] = val;
    used_bytes = 1; goto end;
  }

  if(val < 0x800) {
    utf8[0] = 0xC0 | (FLAC__uint32)(val>>6);
    utf8[1] = 0x80 | (FLAC__uint32)(val&0x3F);
    used_bytes = 2; goto end;
  }

  if(val < 0x10000) {
    utf8[0] = 0xE0 | (FLAC__uint32)(val>>12);
    utf8[1] = 0x80 | (FLAC__uint32)((val>>6)&0x3F);
    utf8[2] = 0x80 | (FLAC__uint32)(val&0x3F);
    used_bytes = 3; goto end;
  }

  if(val < 0x200000) {
    utf8[0] = 0xF0 | (FLAC__uint32)(val>>18);
    utf8[1] = 0x80 | (FLAC__uint32)((val>>12)&0x3F);
    utf8[2] = 0x80 | (FLAC__uint32)((val>>6)&0x3F);
    utf8[3] = 0x80 | (FLAC__uint32)(val&0x3F);
    used_bytes = 4; goto end;
  }

  if(val < 0x4000000) {
    utf8[0] = 0xF8 | (FLAC__uint32)(val>>24);
    utf8[1] = 0x80 | (FLAC__uint32)((val>>18)&0x3F);
    utf8[2] = 0x80 | (FLAC__uint32)((val>>12)&0x3F);
    utf8[3] = 0x80 | (FLAC__uint32)((val>>6)&0x3F);
    utf8[4] = 0x80 | (FLAC__uint32)(val&0x3F);
    used_bytes = 5; goto end;
  }

  if(val < 0x80000000) {
    utf8[0] = 0xFC | (FLAC__uint32)(val>>30);
    utf8[1] = 0x80 | (FLAC__uint32)((val>>24)&0x3F);
    utf8[2] = 0x80 | (FLAC__uint32)((val>>18)&0x3F);
    utf8[3] = 0x80 | (FLAC__uint32)((val>>12)&0x3F);
    utf8[4] = 0x80 | (FLAC__uint32)((val>>6)&0x3F);
    utf8[5] = 0x80 | (FLAC__uint32)(val&0x3F);
    used_bytes = 6; goto end;
  }

  utf8[0] = 0xFE;
  utf8[1] = 0x80 | (FLAC__uint32)((val>>30)&0x3F);
  utf8[2] = 0x80 | (FLAC__uint32)((val>>24)&0x3F);
  utf8[3] = 0x80 | (FLAC__uint32)((val>>18)&0x3F);
  utf8[4] = 0x80 | (FLAC__uint32)((val>>12)&0x3F);
  utf8[5] = 0x80 | (FLAC__uint32)((val>>6)&0x3F);
  utf8[6] = 0x80 | (FLAC__uint32)(val&0x3F);
  used_bytes = 7;

end:
  ;
  unsigned char *malloced_utf8 = malloc(sizeof(unsigned char) * used_bytes);
  if (malloced_utf8 == NULL) { *utf8_used_bytes = 0; return NULL; }

  int i = 0;
  for (;i < used_bytes;i++)
  {
    malloced_utf8[i] = utf8[i];
  }

  *utf8_used_bytes = used_bytes;

  return malloced_utf8;
}

