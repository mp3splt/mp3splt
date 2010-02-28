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

#include <string.h>

#include "splt.h"

static void splt_fu_free_freedb_search(splt_state *state);

void splt_fu_set_default_values(splt_state *state)
{
  state->fdb.search_results = NULL;
  state->fdb.cdstate = NULL;
}

void splt_fu_freedb_free_search(splt_state *state)
{
  splt_fu_free_freedb_search(state);
  splt_cd_state *cdstate = state->fdb.cdstate;
  if (cdstate != NULL)
  { 
    free(cdstate);
    cdstate = NULL;
  }
}

int splt_fu_freedb_init_search(splt_state *state)
{
  int error = SPLT_OK;

  if ((state->fdb.cdstate = malloc(sizeof(splt_cd_state))) == NULL)
  {
    error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  }
  else
  {
    state->fdb.cdstate->foundcd = 0;
    if ((state->fdb.search_results = malloc(sizeof(splt_freedb_results))) == NULL)
    {
      free(state->fdb.cdstate);
      state->fdb.cdstate = NULL;
      error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    }
    else
   {
      state->fdb.search_results->number = 0;
      state->fdb.search_results->results = NULL;
    }
  }

  return error;
}

//sets a freedb result
//if revision != -1, then not a revision
int splt_fu_freedb_append_result(splt_state *state, const char *album_name, int revision)
{
  int error = SPLT_OK;

  if (album_name == NULL)
  {
    return error;
  }

  if (state->fdb.search_results->number == 0)
  {
    state->fdb.search_results->results = malloc(sizeof(splt_freedb_one_result));
    if (state->fdb.search_results->results == NULL)
    {
      error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    }
    else
    {
      state->fdb.search_results->results[0].revisions = NULL;
      state->fdb.search_results->results[0].name = strdup(album_name);
      if (state->fdb.search_results->results[0].name == NULL)
      {
        error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      }
      else
      {
        state->fdb.search_results->results[state->fdb.search_results->number]
          .revision_number = 0;
        state->fdb.search_results->results[state->fdb.search_results->number]
          .id = 0;
        state->fdb.search_results->number++;
      }
    }
  }
  else
  {
    //if its not a revision
    if (revision != -1)
    {
      state->fdb.search_results->results = 
        realloc(state->fdb.search_results->results,
            (state->fdb.search_results->number + 1)
            * sizeof(splt_freedb_one_result));
      state->fdb.search_results->results[state->fdb.search_results->number].revisions = NULL;
      if (state->fdb.search_results->results == NULL)
      {
        error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
      }
      else
      {
        state->fdb.search_results->results[state->fdb.search_results->number]
          .name = strdup(album_name);
        if (state->fdb.search_results->results[state->fdb.search_results->number]
            .name == NULL)
        {
          error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
        }
        else
        {
          state->fdb.search_results->results[state->fdb.search_results->number]
            .revision_number = 0;
          state->fdb.search_results->results[state->fdb.search_results->number]
            .id = (state->fdb.search_results->results[state->fdb.search_results->number - 1]
                .id + state->fdb.search_results->results[state->fdb.search_results->number - 1]
                .revision_number + 1);
          state->fdb.search_results->number++;
        }
      }
    }
    else
      //if it's a revision
    {
      //if it's the first revision
      if (state->fdb.search_results->results[state->fdb.search_results->number-1]
          .revision_number == 0)
      {
        state->fdb.search_results->results[state->fdb.search_results->number-1].revisions =
          malloc(sizeof(int));
        if (state->fdb.search_results->results[state->fdb.search_results->number-1].revisions
            == NULL)
        {
          error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;              
        }
        else
        {
          state->fdb.search_results->results[state->fdb.search_results->number-1].revisions[0]
            = atoi(album_name);
          state->fdb.search_results->results[state->fdb.search_results->number-1].revision_number++;
        }
      }
      else
        //if it's not the first revision
      {
        state->fdb.search_results->results[state->fdb.search_results->number-1].revisions =
          realloc(state->fdb.search_results->results
              [state->fdb.search_results->number-1].revisions,
              (state->fdb.search_results->results[state->fdb.search_results->number-1]
               .revision_number + 1)
              * sizeof(int));
        if (state->fdb.search_results->results[state->fdb.search_results->number-1].revisions
            == NULL)
        {
          error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;              
        }
        else
        {
          state->fdb.search_results->results[state->fdb.search_results->number-1].
            revisions[state->fdb.search_results->results[state->fdb.search_results->number-1].revision_number]
            = atoi(album_name);
          state->fdb.search_results->results[state->fdb.search_results->number-1].revision_number++;
        }
      }
    }
  }

  return error;
}

static void splt_fu_free_freedb_search(splt_state *state)
{
  splt_freedb_results *search_results = state->fdb.search_results;

  if (state->fdb.search_results)
  {
    int i;
    for(i = 0; i < search_results->number;i++)
    {
      if (search_results->results[i].revisions)
      {
        free(search_results->results[i].revisions);
        search_results->results[i].revisions = NULL;
      }
      if (search_results->results[i].name)
      {
        free(search_results->results[i].name);
        search_results->results[i].name = NULL;
      }
    }
    if (search_results->results)
    {
      free(search_results->results);
      search_results->results = NULL;
    }

    state->fdb.search_results->number = 0;
    free(state->fdb.search_results);
    state->fdb.search_results = NULL;
  }
}

int splt_fu_freedb_get_found_cds(splt_state *state)
{
  return state->fdb.cdstate->foundcd;
}

void splt_fu_freedb_found_cds_next(splt_state *state)
{
  state->fdb.cdstate->foundcd = splt_fu_freedb_get_found_cds(state)+1;
}

void splt_fu_freedb_set_disc(splt_state *state, int index,
    const char *discid, const char *category, int category_size)
{
  splt_cd_state *cdstate = state->fdb.cdstate;

  if ((index >= 0) && (index < SPLT_MAXCD))
  {
    memset(cdstate->discs[index].category, '\0', 20);
    snprintf(cdstate->discs[index].category, category_size,"%s",category);
#ifdef __WIN32__
    //snprintf seems buggy
    cdstate->discs[index].category[category_size-1] = '\0';
#endif
    splt_d_print_debug(state,"Setting disc category _%s_\n", cdstate->discs[index].category);

    memset(cdstate->discs[index].discid, '\0', SPLT_DISCIDLEN+1);
    snprintf(cdstate->discs[index].discid,SPLT_DISCIDLEN+1,"%s",discid);
#ifdef __WIN32__
    //snprintf seems buggy
    cdstate->discs[index].discid[SPLT_DISCIDLEN] = '\0';
#endif
    splt_d_print_debug(state,"Setting disc id _%s_\n", cdstate->discs[index].discid);
  }
  else
  {
    splt_e_error(SPLT_IERROR_INT, __func__, index, NULL);
  }
}

const char *splt_fu_freedb_get_disc_category(splt_state *state, int index)
{
  splt_cd_state *cdstate = state->fdb.cdstate;

  if ((index >= 0) && (index < cdstate->foundcd))
  {
    return cdstate->discs[index].category;
  }
  else
  {
    splt_e_error(SPLT_IERROR_INT, __func__, index, NULL);
    return NULL;
  }
}

const char *splt_fu_freedb_get_disc_id(splt_state *state, int index)
{
  splt_cd_state *cdstate = state->fdb.cdstate;

  if ((index >= 0) && (index < cdstate->foundcd))
  {
    return cdstate->discs[index].discid;
  }
  else
  {
    splt_e_error(SPLT_IERROR_INT, __func__, index, NULL);
    return NULL;
  }
}



