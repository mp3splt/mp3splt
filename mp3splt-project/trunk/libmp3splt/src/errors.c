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
#include <errno.h>

#ifdef __WIN32__
#include <winsock.h>
#else
#include <netdb.h>
#endif

#include "splt.h"

void splt_e_set_errors_default_values(splt_state *state)
{
  state->err.error_data = NULL;
  state->err.strerror_msg = NULL;
}

void splt_e_free_errors(splt_state *state)
{
  if (state->err.error_data)
  {
    free(state->err.error_data);
    state->err.error_data = NULL;
  }
  if (state->err.strerror_msg)
  {
    free(state->err.strerror_msg);
    state->err.strerror_msg = NULL;
  }
}

void splt_e_set_error_data(splt_state *state, const char *error_data)
{
  if (state->err.error_data)
  {
    free(state->err.error_data);
    state->err.error_data = NULL;
  }
  if (error_data)
  {
    state->err.error_data = malloc(sizeof(char) * (strlen(error_data) + 1));
    if (state->err.error_data)
    {
      snprintf(state->err.error_data, strlen(error_data)+1, "%s", error_data);
    }
  }
}

void splt_e_set_error_data_from_splitpoint(splt_state *state, long splitpoint)
{
  char str_value[256] = { '\0' };
  long mins = 0, secs = 0, hundr = 0;
  splt_sp_get_mins_secs_hundr_from_splitpoint(splitpoint, &mins, &secs, &hundr);
  snprintf(str_value,256,"%ldm%lds%ldh",mins,secs,hundr);
  splt_e_set_error_data(state, str_value);
}

void splt_e_set_error_data_from_splitpoints(splt_state *state, long splitpoint1,
    long splitpoint2)
{
  char str_value[256] = { '\0' };
  long mins = 0, secs = 0, hundr = 0;
  long mins2 = 0, secs2 = 0, hundr2 = 0;
  splt_sp_get_mins_secs_hundr_from_splitpoint(splitpoint1, &mins, &secs, &hundr);
  splt_sp_get_mins_secs_hundr_from_splitpoint(splitpoint2, &mins2, &secs2, &hundr2);
  snprintf(str_value,256,"%ldm%lds%ldh, %ldm%lds%ldh",
      mins, secs, hundr, mins2, secs2, hundr2);
  splt_e_set_error_data(state, str_value);
}

void splt_e_set_strerr_msg(splt_state *state, const char *message)
{
  if (state->err.strerror_msg)
  {
    free(state->err.strerror_msg);
    state->err.strerror_msg = NULL;
  }

  if (message)
  {
    state->err.strerror_msg = malloc(sizeof(char) * (strlen(message) + 1));
    if (state->err.strerror_msg)
    {
      snprintf(state->err.strerror_msg,strlen(message)+1,"%s",message);
    }
    else
    {
      splt_e_error(SPLT_IERROR_CHAR,__func__, 0, _("not enough memory"));
    }
  }
  else
  {
    state->err.strerror_msg = NULL;
  }
}

void splt_e_clean_strerror_msg(splt_state *state)
{
  splt_e_set_strerr_msg(state, NULL);
}

void splt_e_set_strerror_msg(splt_state *state)
{
  char *strerr = strerror(errno);
  splt_e_set_strerr_msg(state, strerr);
}

void splt_e_set_strherror_msg(splt_state *state)
{
#ifndef __WIN32__
  const char *hstrerr = hstrerror(h_errno);
  splt_e_set_strerr_msg(state, hstrerr);
#else
  splt_e_set_strerr_msg(state, _("Network error"));
#endif
}

void splt_e_error(int error_type, const char *function,
    int arg_int, char *arg_char)
{
  switch (error_type)
  {
    case SPLT_IERROR_INT:
      fprintf(stderr,
          "libmp3splt: error in %s with value %d\n",
          function, arg_int);
      fflush(stderr);
      break;
    case SPLT_IERROR_SET_ORIGINAL_TAGS:
      fprintf(stderr,
          "libmp3splt: cannot set original file tags, "
          "libmp3splt not compiled with libid3tag\n");
      fflush(stderr);
      break;
    case SPLT_IERROR_CHAR:
      fprintf(stderr,
          "libmp3splt: error in %s with message '%s'\n",function,arg_char);
      fflush(stderr);
      break;
    default:
      fprintf(stderr,
          "libmp3splt: unknown error in %s\n", function);
      fflush(stderr);
      break;
  }
}

char *splt_e_strerror(splt_state *state, splt_code error_code)
{
  int max_error_size = 4096;
  char *error_msg = malloc(sizeof(char) * max_error_size);
  if (error_msg)
  {
    memset(error_msg,'\0',4096);

    switch (error_code)
    {
      //
      case SPLT_MIGHT_BE_VBR:
        snprintf(error_msg,max_error_size,
            _(" warning: might be VBR, use frame mode"));
        break;
      case SPLT_SYNC_OK:
        snprintf(error_msg,max_error_size, _(" error mode ok"));
        break;
      case SPLT_ERR_SYNC:
        snprintf(error_msg,max_error_size, _(" error: unknown sync error"));
        break;
      case SPLT_ERR_NO_SYNC_FOUND:
        snprintf(error_msg,max_error_size, _(" no sync errors found"));
        break;
      case SPLT_ERR_TOO_MANY_SYNC_ERR:
        snprintf(error_msg,max_error_size, _(" sync error: too many sync errors"));
        break;
        //
      case SPLT_FREEDB_MAX_CD_REACHED:
        snprintf(error_msg,max_error_size, _(" maximum number of found CD reached"));
        break;
      case SPLT_CUE_OK:
        snprintf(error_msg,max_error_size, _(" cue file processed"));
        break;
      case SPLT_CDDB_OK:
        snprintf(error_msg,max_error_size, _(" cddb file processed"));
        break;
      case SPLT_FREEDB_FILE_OK:
        snprintf(error_msg,max_error_size, _(" freedb file downloaded"));
        break;
      case SPLT_FREEDB_OK:
        snprintf(error_msg,max_error_size, _(" freedb search processed"));
        break;
        //
      case SPLT_FREEDB_ERROR_INITIALISE_SOCKET:
        snprintf(error_msg,max_error_size, 
            _(" freedb error: cannot initialise socket (%s)"),
            state->err.strerror_msg);
        break;
      case SPLT_FREEDB_ERROR_CANNOT_GET_HOST:
        snprintf(error_msg,max_error_size, 
            _(" freedb error: cannot get host '%s' by name (%s)"),
            state->err.error_data, state->err.strerror_msg);
        break;
      case SPLT_FREEDB_ERROR_CANNOT_OPEN_SOCKET:
        snprintf(error_msg,max_error_size, _(" freedb error: cannot open socket"));
        break;
      case SPLT_FREEDB_ERROR_CANNOT_CONNECT:
        snprintf(error_msg,max_error_size, 
            _(" freedb error: cannot connect to host '%s' (%s)"),
            state->err.error_data, state->err.strerror_msg);
        break;
      case SPLT_FREEDB_ERROR_CANNOT_SEND_MESSAGE:
        snprintf(error_msg,max_error_size, 
            _(" freedb error: cannot send message to host '%s' (%s)"),
            state->err.error_data, state->err.strerror_msg);
        break;
      case SPLT_FREEDB_ERROR_INVALID_SERVER_ANSWER:
        snprintf(error_msg,max_error_size, _(" freedb error: invalid server answer"));
        break;
      case SPLT_FREEDB_ERROR_SITE_201:
        snprintf(error_msg,max_error_size, _(" freedb error: site returned code 201"));
        break;
      case SPLT_FREEDB_ERROR_SITE_200:
        snprintf(error_msg,max_error_size, _(" freedb error: site returned code 200"));
        break;
      case SPLT_FREEDB_ERROR_BAD_COMMUNICATION:
        snprintf(error_msg,max_error_size, _(" freedb error: bad communication with site"));
        break;
      case SPLT_FREEDB_ERROR_GETTING_INFOS:
        snprintf(error_msg,max_error_size, _(" freedb error: could not get infos from site '%s'"),
            state->err.error_data);
        break;
      case SPLT_FREEDB_NO_CD_FOUND:
        snprintf(error_msg,max_error_size, _(" no CD found for this search"));
        break;
      case SPLT_FREEDB_ERROR_CANNOT_RECV_MESSAGE:
        snprintf(error_msg,max_error_size,
            _(" freedb error: cannot receive message from server '%s' (%s)"),
            state->err.error_data, state->err.strerror_msg);
        break;
      case SPLT_INVALID_CUE_FILE:
        snprintf(error_msg,max_error_size, _(" cue error: invalid cue file '%s'"),
            state->err.error_data);
        break;
      case SPLT_INVALID_CDDB_FILE:
        snprintf(error_msg,max_error_size, _(" cddb error: invalid cddb file '%s'"),
            state->err.error_data);
        break;
      case SPLT_FREEDB_NO_SUCH_CD_IN_DATABASE:
        snprintf(error_msg,max_error_size, _(" freedb error: No such CD entry in database"));
        break;
      case SPLT_FREEDB_ERROR_SITE:
        snprintf(error_msg,max_error_size, _(" freedb error: site returned an unknown error"));
        break;
        //
      case SPLT_DEWRAP_OK:
        snprintf(error_msg,max_error_size, _(" wrap split ok"));
        break;
        //
      case SPLT_DEWRAP_ERR_FILE_LENGTH:
        snprintf(error_msg,max_error_size, _(" wrap error: incorrect file length"));
        break;
      case SPLT_DEWRAP_ERR_VERSION_OLD:
        snprintf(error_msg,max_error_size,
            _(" wrap error: libmp3splt version too old for this wrap file"));
        break;
      case SPLT_DEWRAP_ERR_NO_FILE_OR_BAD_INDEX:
        snprintf(error_msg,max_error_size,
            _(" wrap error: no file found or bad index"));
        break;
      case SPLT_DEWRAP_ERR_FILE_DAMAGED_INCOMPLETE:
        snprintf(error_msg,max_error_size,
            _(" wrap error: file '%s' damaged or incomplete"),
            state->err.error_data);
        break;
      case SPLT_DEWRAP_ERR_FILE_NOT_WRAPED_DAMAGED:
        snprintf(error_msg,max_error_size,
            _(" wrap error: maybe not a wrapped file or wrap file damaged"));
        break;
        //
      case SPLT_OK_SPLIT_EOF:
        snprintf(error_msg,max_error_size,_(" file split (EOF)"));
        break;
      case SPLT_NO_SILENCE_SPLITPOINTS_FOUND:
        snprintf(error_msg,max_error_size, _(" no silence splitpoints found"));
        break;
      case SPLT_TIME_SPLIT_OK:
        snprintf(error_msg,max_error_size, _(" time split ok"));
        break;
      case SPLT_LENGTH_SPLIT_OK:
        snprintf(error_msg,max_error_size, _(" split in equal tracks ok"));
        break;
      case SPLT_SILENCE_OK:
        snprintf(error_msg,max_error_size, _(" silence split ok"));
        break;
      case SPLT_SPLITPOINT_BIGGER_THAN_LENGTH:
        snprintf(error_msg,max_error_size,
            _(" file split, splitpoints bigger than length"));
        break;
      case SPLT_OK_SPLIT:
        snprintf(error_msg,max_error_size, _(" file split"));
        break;
      case SPLT_OK:
        break;
      case SPLT_ERROR_SPLITPOINTS:
        snprintf(error_msg,max_error_size, _(" error: not enough splitpoints (<2)"));
        break;
      case SPLT_ERROR_CANNOT_OPEN_FILE:
        snprintf(error_msg,max_error_size,
            _(" error: cannot open file '%s': %s"),
            state->err.error_data, state->err.strerror_msg);
        break;
      case SPLT_ERROR_CANNOT_CLOSE_FILE:
        snprintf(error_msg,max_error_size,
            _(" error: cannot close file '%s': %s"),
            state->err.error_data, state->err.strerror_msg);
        break;
      case SPLT_ERROR_INVALID:
        ;
        int err = SPLT_OK;
        const char *plugin_name = splt_p_get_name(state,&err);
        snprintf(error_msg,max_error_size,
            _(" error: invalid input file '%s' for '%s' plugin"),
            state->err.error_data, plugin_name);
        break;
      case SPLT_ERROR_EQUAL_SPLITPOINTS:
        snprintf(error_msg,max_error_size,
            _(" error: splitpoints are equal (%s)"),
            state->err.error_data);
        break;
      case SPLT_ERROR_TIME_SPLIT_VALUE_INVALID:
        snprintf(error_msg,max_error_size, _(" error: invalid time split value"));
        break;
      case SPLT_ERROR_LENGTH_SPLIT_VALUE_INVALID:
        snprintf(error_msg,max_error_size, _(" error: invalid number of files for equal length split"));
        break;
      case SPLT_ERROR_CANNOT_GET_TOTAL_TIME:
        snprintf(error_msg,max_error_size, _(" error: cannot get total audio length"));
        break;
      case SPLT_ERROR_SPLITPOINTS_NOT_IN_ORDER:
        snprintf(error_msg,max_error_size,
            _(" error: the splitpoints are not in order (%s)"),
            state->err.error_data);
        break;
      case SPLT_ERROR_NEGATIVE_SPLITPOINT:
        snprintf(error_msg,max_error_size, _(" error: negative splitpoint (%s)"),
            state->err.error_data);
        break;
      case SPLT_ERROR_INCORRECT_PATH:
        snprintf(error_msg,max_error_size,
            _(" error: bad destination folder '%s' (%s)"),
            state->err.error_data, state->err.strerror_msg);
        break;
      case SPLT_ERROR_INCOMPATIBLE_OPTIONS:
        snprintf(error_msg,max_error_size, _(" error: incompatible options"));
        break;
      case SPLT_ERROR_INPUT_OUTPUT_SAME_FILE:
        snprintf(error_msg,max_error_size,
            _(" input and output are the same file ('%s')"),
            state->err.error_data);
        break;
      case SPLT_ERROR_CANNOT_ALLOCATE_MEMORY:
        snprintf(error_msg,max_error_size, _(" error: cannot allocate memory"));
        break;
      case SPLT_ERROR_CANNOT_OPEN_DEST_FILE:
        snprintf(error_msg,max_error_size,
            _(" error: cannot open destination file '%s': %s"),
            state->err.error_data,state->err.strerror_msg);
        break;
      case SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE:
        snprintf(error_msg,max_error_size,
            _(" error: cannot write to output file '%s'"),
            state->err.error_data);
        break;
      case SPLT_ERROR_WHILE_READING_FILE:
        snprintf(error_msg,max_error_size, _(" error: error while reading file '%s': %s"),
            state->err.error_data, state->err.strerror_msg);
        break;
      case SPLT_ERROR_SEEKING_FILE:
        snprintf(error_msg,max_error_size, _(" error: cannot seek file '%s'"),
            state->err.error_data);
        break;
      case SPLT_ERROR_BEGIN_OUT_OF_FILE:
        snprintf(error_msg,max_error_size, _(" error: begin point out of file"));
        break;
      case SPLT_ERROR_INEXISTENT_FILE:
        snprintf(error_msg,max_error_size, _(" error: inexistent file '%s': %s"),
            state->err.error_data,state->err.strerror_msg);
        break;
      case SPLT_SPLIT_CANCELLED:
        snprintf(error_msg,max_error_size, _(" split process cancelled"));
        break;
      case SPLT_ERROR_LIBRARY_LOCKED: 
        snprintf(error_msg,max_error_size, _(" error: library locked"));
        break;
      case SPLT_ERROR_STATE_NULL:
        snprintf(error_msg,max_error_size,
            _(" error: the state has not been initialized with 'mp3splt_new_state'"));
        break;
      case SPLT_ERROR_NEGATIVE_TIME_SPLIT:
        snprintf(error_msg,max_error_size, _(" error: negative time split"));
        break;
      case SPLT_ERROR_CANNOT_CREATE_DIRECTORY:
        snprintf(error_msg,max_error_size, _(" error: cannot create directory '%s'"),
            state->err.error_data);
        break;
      case SPLT_ERROR_NO_PLUGIN_FOUND:
        snprintf(error_msg,max_error_size, _(" error: no plugin found"));
        break;
      case SPLT_ERROR_CANNOT_INIT_LIBLTDL:
        snprintf(error_msg,max_error_size, _(" error: cannot initiate libltdl"));
        break;
      case SPLT_ERROR_CRC_FAILED:
        snprintf(error_msg,max_error_size, _(" error: CRC failed"));
        break;
      case SPLT_ERROR_NO_PLUGIN_FOUND_FOR_FILE:
        snprintf(error_msg,max_error_size,
            _(" error: no plugin matches the file '%s'"),
            state->err.error_data);
        break;
        //
      case SPLT_OUTPUT_FORMAT_OK:
        break;
      case SPLT_OUTPUT_FORMAT_AMBIGUOUS:
        snprintf(error_msg,max_error_size,
            _(" warning: output format ambiguous (@t or @n missing)"));
        break;
        //
      case SPLT_OUTPUT_FORMAT_ERROR:
        if (state->err.error_data != NULL && strlen(state->err.error_data) == 1)
        {
          snprintf(error_msg,max_error_size,
              _(" error: illegal variable '@%s' in output format"),
              state->err.error_data);
        }
        else
        {
          snprintf(error_msg,max_error_size, _(" error: invalid output format"));
        }
        break;
        //
      case SPLT_ERROR_INEXISTENT_SPLITPOINT:
        snprintf(error_msg,max_error_size, _(" error: inexistent splitpoint"));
        break;
        //
      case SPLT_ERROR_PLUGIN_ERROR:
        snprintf(error_msg,max_error_size, _(" plugin error: '%s'"),
            state->err.error_data);
        break;
        //
      case SPLT_PLUGIN_ERROR_UNSUPPORTED_FEATURE:
        ;
        splt_plugins *pl = state->plug;
        int current_plugin = splt_p_get_current_plugin(state);
        snprintf(error_msg,max_error_size, _(" error: unsupported feature for the plugin '%s'"),
            pl->data[current_plugin].info.name);
        break;
      case SPLT_AUDACITY_OK:
        snprintf(error_msg,max_error_size, _(" audacity labels file processed"));
        break;
      case SPLT_INVALID_AUDACITY_FILE:
        snprintf(error_msg,max_error_size, _(" audacity error: invalid audacity labels file '%s'"),
            state->err.error_data);
        break;
    }

    if (error_msg[0] == '\0')
    {
      free(error_msg);
      error_msg = NULL;
    }
  }
  else
  {
    //if not enough memory
    splt_e_error(SPLT_IERROR_CHAR,__func__, 0, _("not enough memory"));
  }

  return error_msg;
}

