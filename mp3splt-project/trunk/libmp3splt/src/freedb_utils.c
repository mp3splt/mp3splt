/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2011 Alexandru Munteanu - io_fx@yahoo.fr
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

/*! \file

Utilities used by the functions in freedb.c
 */
#include <string.h>

#include "splt.h"

static void splt_fu_free_freedb_search(splt_state *state);
static int splt_fu_append_first_result(splt_freedb_results *res,
    const char *album_name);
static int splt_fu_append_next_result(splt_freedb_results *res,
    const char *album_name);
static int splt_fu_append_first_revision(splt_freedb_one_result *prev,
    const char *album_name);
static int splt_fu_append_next_revision(splt_freedb_one_result *prev,
    const char *album_name);

void splt_fu_set_default_values(splt_state *state)
{
  splt_freedb *fdb = &state->fdb;
  fdb->search_results = NULL;
  fdb->cdstate = NULL;
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
  splt_freedb *fdb = &state->fdb;

  if ((fdb->cdstate = malloc(sizeof(splt_cd_state))) == NULL)
  {
    error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  }
  else
  {
    fdb->cdstate->foundcd = 0;
    if ((fdb->search_results = malloc(sizeof(splt_freedb_results))) == NULL)
    {
      free(fdb->cdstate);
      fdb->cdstate = NULL;
      error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    }
    else
    {
      fdb->search_results->number = 0;
      fdb->search_results->results = NULL;
    }
  }

  return error;
}

int splt_fu_freedb_append_result(splt_state *state, const char *album_name, int revision)
{
  splt_freedb_results *res = state->fdb.search_results;

  if (album_name == NULL)
  {
    return SPLT_OK;
  }

  if (res->number == 0)
  {
    return splt_fu_append_first_result(res, album_name);
  }

  if (revision != -1)
  {
    return splt_fu_append_next_result(res, album_name);
  }

  splt_freedb_one_result *prev = &res->results[res->number-1];

  if (prev->revision_number == 0)
  {
    return splt_fu_append_first_revision(prev, album_name);
  }

  return splt_fu_append_next_revision(prev, album_name);
}

int splt_fu_freedb_get_found_cds(splt_state *state)
{
  return state->fdb.cdstate->foundcd;
}

void splt_fu_freedb_found_cds_next(splt_state *state)
{
  state->fdb.cdstate->foundcd = splt_fu_freedb_get_found_cds(state) + 1;
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

static void splt_fu_free_freedb_search(splt_state *state)
{
  splt_freedb_results *res = state->fdb.search_results;

  if (res)
  {
    int i = 0;
    for(i = 0; i < res->number;i++)
    {
      if (res->results[i].revisions)
      {
        free(res->results[i].revisions);
        res->results[i].revisions = NULL;
      }

      if (res->results[i].name)
      {
        free(res->results[i].name);
        res->results[i].name = NULL;
      }
    }

    if (res->results)
    {
      free(res->results);
      res->results = NULL;
    }

    res->number = 0;

    free(state->fdb.search_results);
    state->fdb.search_results = NULL;
  }
}

static int splt_fu_append_first_result(splt_freedb_results *res,
    const char *album_name)
{
  int error = SPLT_OK;

  res->results = malloc(sizeof(splt_freedb_one_result));
  if (res->results == NULL)
  {
    return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  }
  memset(res->results, 0x0, sizeof(splt_freedb_one_result));

  res->results[0].revisions = NULL;
  error = splt_su_copy(album_name, &res->results[0].name);
  if (error < 0) { return error; }

  res->results[0].revision_number = 0;
  res->results[0].id = 0;
  res->number++;

  return error;
}

static int splt_fu_append_next_result(splt_freedb_results *res,
    const char *album_name)
{
  int error = SPLT_OK;

  res->results = realloc(res->results, (res->number + 1) * sizeof(splt_freedb_one_result));
  if (res->results == NULL)
  {
    return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
  }
  memset(&res->results[res->number], 0x0, sizeof(splt_freedb_one_result));

  error = splt_su_copy(album_name, &res->results[res->number].name);
  if (error < 0) { return error; }

  splt_freedb_one_result *prev = &res->results[res->number-1];

  res->results[res->number].revision_number = 0;
  res->results[res->number].id = (prev->id + prev->revision_number + 1);
  res->number++;

  return error;
}

static int splt_fu_append_first_revision(splt_freedb_one_result *prev,
    const char *album_name)
{
  prev->revisions = malloc(sizeof(int));
  if (prev->revisions == NULL)
  {
    return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;              
  }

  prev->revisions[0] = atoi(album_name);
  prev->revision_number++;

  return SPLT_OK;
}

static int splt_fu_append_next_revision(splt_freedb_one_result *prev,
    const char *album_name)
{
  prev->revisions = realloc(prev->revisions, (prev->revision_number + 1) * sizeof(int));
  if (prev->revisions == NULL)
  {
    return SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;              
  }

  prev->revisions[prev->revision_number] = atoi(album_name);
  prev->revision_number++;

  return SPLT_OK;
}

