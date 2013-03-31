/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2013 Alexandru Munteanu - m@ioalex.net
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

/*! \file

  All conversions we need (text to hundreths of a second,...)

 */
#include <math.h>
#include <string.h>

#include "splt.h"

//! Convert a string to an integer containing the hundreths of a second
long splt_co_convert_to_hundreths(const char *s)
{
  long minutes = 0, seconds = 0, hundredths = 0, i = 0;

  for(i=0; i< strlen(s); i++)
  {
    if ((s[i]<0x30 || s[i] > 0x39) && (s[i]!=':'))
    {
      return -1;
    }
  }

  if (sscanf(s, "%ld:%ld:%ld", &minutes, &seconds, &hundredths) < 2)
  {
    return -1;
  }

  if ((minutes < 0) || (seconds < 0) || (hundredths < 0))
  {
    return -1;
  }

  if ((seconds > 59) || (hundredths > 99))
  {
    return -1;
  }

  if (s[strlen(s)-2]==':')
  {
    hundredths *= 10;
  }

  long hun = hundredths;
  hun += (minutes * 60 + seconds) * 100;

  return hun;
}

//! Convert a factor to the corresponding dB value
float splt_co_convert_to_db(double input)
{
  float level;
  if (input <= 0.0)
  {
    level = -96.0;
  }
  else 
  {
    level = 20 * log10(input);
  }

  return level;
}

//! Convert a dB value to a factor
double splt_co_convert_from_db(float input)
{
  double amp;
  if (input <- 96.0)
  {
    amp = 0.0;
  }
  else 
  {
    amp = pow(10.0, input / 20.0);
  }

  return amp;
}

//! Split a long containing a time value in 1/100s to seconds, mins and 100's
void splt_co_get_mins_secs_hundr(long split_hundr, long *mins, long *secs, long *hundr)
{
  long h = split_hundr % 100;
  long split_hundr_without_h = split_hundr / 100;
  long m = split_hundr_without_h / 60;
  long s = split_hundr_without_h % 60;
  if (mins)
  {
    *mins = m;
  }
  if (secs)
  {
    *secs = s;
  }
  if (hundr)
  {
    *hundr = h;
  }
}

//! Convert float time in fractions of a second to a 1/100 seconds int
long splt_co_time_to_long(double time)
{
  return (long) (time * 100.0);
}

//! Convert float time in fractions of a second to a 1/100 seconds int rounding upwards
long splt_co_time_to_long_ceil(double time)
{
  return (long) ceil(time * 100);
}

