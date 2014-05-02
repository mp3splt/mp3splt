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

Utilities needed for silence detection.
*/
#include "splt.h"

int splt_siu_ssplit_new(struct splt_ssplit **silence_list, 
    float begin_position, float end_position, int len, int *error)
{
  struct splt_ssplit *temp = NULL;
  struct splt_ssplit *s_new = NULL;

  if ((s_new = malloc(sizeof(struct splt_ssplit))) == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return -1;
  }

  s_new->len = len;
  s_new->begin_position = begin_position;
  s_new->end_position = end_position;
  s_new->next = NULL;

  temp = *silence_list;
  if (temp == NULL)
  {
    *silence_list = s_new; // No elements
    return 0;
  }

  if (temp->len < len)
  {
    s_new->next = temp;
    *silence_list = s_new;
  }
  else
  {
    if (temp->next == NULL)
    {
      temp->next = s_new;
    }
    else 
    {
      while (temp != NULL) 
      {
        if (temp->next != NULL) 
        {
          if (temp->next->len < len) 
          {
            //build an ordered list by len to keep most probable silence points
            s_new->next = temp->next;
            temp->next = s_new;
            break;
          }
        }
        else 
        {
          temp->next = s_new;
          break;
        }
        temp = temp->next;
      }
    }
  }

  return 0;
}

void splt_siu_ssplit_free(struct splt_ssplit **silence_list)
{
  struct splt_ssplit *temp = NULL, *saved = NULL;

  if (silence_list)
  {
    temp = *silence_list;

    while (temp != NULL)
    {
      saved = temp->next;
      free(temp);
      temp = saved;
    }

    *silence_list = NULL;
  }
}

float splt_siu_silence_position(struct splt_ssplit *temp, float off)
{
  float length_of_silence = (temp->end_position - temp->begin_position);
  return temp->begin_position + (length_of_silence * off);
}

int splt_siu_parse_ssplit_file(splt_state *state, FILE *log_file, int *error)
{
  char *line = NULL; 
  int found = 0;

  while ((line = splt_io_readline(log_file, error)) != NULL)
  {
    if (*error < 0) { break; }

    int len = 0;
    float begin_position = 0, end_position = 0;
    if (sscanf(line, "%f\t%f\t%d", &begin_position, &end_position, &len) == 3)
    {
      splt_siu_ssplit_new(&state->silence_list, begin_position, end_position, len, error);
      if (*error < 0)
      {
        break;
      }

      found++;
    }

    if (line)
    {
      free(line);
      line = NULL;
    }
  }

  if (line)
  {
    free(line);
    line = NULL;
  }

  return found;
}

