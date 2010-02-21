/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2010 Alexandru Munteanu - io_fx@yahoo.fr
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

#include "splt.h"

int splt_siu_ssplit_new(struct splt_ssplit **silence_list, 
    float begin_position, float end_position, int len, int *error)
{
  struct splt_ssplit *temp = NULL;
  struct splt_ssplit *s_new = NULL;

  if ((s_new = malloc(sizeof(struct splt_ssplit)))==NULL)
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
  }
  else 
  {
    if (temp->len < len)
    {
      s_new->next = temp;
      *silence_list = s_new;
    }
    else
    {
      if (temp->next == NULL)
        temp->next = s_new;
      else 
      {
        while (temp != NULL) 
        {
          if (temp->next != NULL) 
          {
            if (temp->next->len < len) 
            {
              // We build an ordered list by len to keep most probable silence points
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
