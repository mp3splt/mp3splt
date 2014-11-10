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

/*! Error handling

Don't use these functions directly. The version of these functions
that is meant to be used directly are all in mp3splt.c.
*/

#include <string.h>
#include <errno.h>

#ifdef __WIN32__
#include <winsock2.h>
#else
#include <netdb.h>
#endif

#include "splt.h"

void splt_e_set_errors_default_values(splt_state *state)
{
  splt_error *err = &state->err;
  err->error_data = NULL;
  err->strerror_msg = NULL;
}

void splt_e_free_errors(splt_state *state)
{
  splt_error *err = &state->err;

  if (err->error_data)
  {
    free(err->error_data);
    err->error_data = NULL;
  }

  if (err->strerror_msg)
  {
    free(err->strerror_msg);
    err->strerror_msg = NULL;
  }
}

void splt_e_set_error_data(splt_state *state, const char *error_data)
{
  splt_error *err = &state->err;

 if (err->error_data)
  {
    free(err->error_data);
    err->error_data = NULL;
  }

  if (error_data)
  {
    splt_su_copy(error_data, &err->error_data);
  }
}

void splt_e_set_error_data_from_splitpoint(splt_state *state, long splitpoint)
{
  long mins = 0, secs = 0, hundr = 0;
  splt_sp_get_mins_secs_hundr_from_splitpoint(splitpoint, &mins, &secs, &hundr);
  char str_value[256] = { '\0' };
  snprintf(str_value,256,"%ldm%lds%ldh",mins,secs,hundr);
  splt_e_set_error_data(state, str_value);
}

void splt_e_set_error_data_from_splitpoints(splt_state *state, long splitpoint1,
    long splitpoint2)
{
  long mins = 0, secs = 0, hundr = 0;
  long mins2 = 0, secs2 = 0, hundr2 = 0;
  splt_sp_get_mins_secs_hundr_from_splitpoint(splitpoint1, &mins, &secs, &hundr);
  splt_sp_get_mins_secs_hundr_from_splitpoint(splitpoint2, &mins2, &secs2, &hundr2);
  char str_value[256] = { '\0' };
  snprintf(str_value,256,"%ldm%lds%ldh, %ldm%lds%ldh",
      mins, secs, hundr, mins2, secs2, hundr2);
  splt_e_set_error_data(state, str_value);
}

void splt_e_clean_strerror_msg(splt_state *state)
{
  splt_e_set_strerr_msg(state, NULL);
}

void splt_e_set_strerror_msg(splt_state *state)
{
  splt_e_set_strerr_msg(state, strerror(errno));
}

void splt_e_set_strherror_msg(splt_state *state)
{
#ifndef __WIN32__
  splt_e_set_strerr_msg(state, hstrerror(h_errno));
#else
  splt_e_set_strerr_msg(state, _("Network error"));
#endif
}

void splt_e_set_strerr_msg_with_data(splt_state *state, const char *message, const char *data)
{
  splt_e_set_strerr_msg(state, message);
  splt_e_set_error_data(state, data);
}

void splt_e_set_strerror_msg_with_data(splt_state *state, const char *data)
{
  splt_e_set_strerror_msg(state);
  splt_e_set_error_data(state, data);
}

void splt_e_error(int error_type, const char *function,
    int arg_int, const char *arg_char)
{
  switch (error_type)
  {
    case SPLT_IERROR_INT:
      fprintf(stderr, "libmp3splt: error in %s with value %d\n",
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
      fprintf(stderr, "libmp3splt: unknown error in %s\n", function);
      fflush(stderr);
      break;
  }
}

char *splt_e_strerror(splt_state *state, splt_code error_code)
{
  switch (error_code)
  {
    case SPLT_MIGHT_BE_VBR:
      return splt_su_get_formatted_message(state,
          _(" warning: might be VBR, use frame mode"));
    case SPLT_SYNC_OK:
      return splt_su_get_formatted_message(state, _(" error mode ok"));
    case SPLT_ERR_SYNC:
      return splt_su_get_formatted_message(state, _(" error: unknown sync error"));
    case SPLT_ERR_NO_SYNC_FOUND:
      return splt_su_get_formatted_message(state, _(" no sync errors found"));
    case SPLT_ERR_TOO_MANY_SYNC_ERR:
      return splt_su_get_formatted_message(state, 
          _(" sync error: too many sync errors"));
      //
    case SPLT_FREEDB_MAX_CD_REACHED:
      return splt_su_get_formatted_message(state,
          _(" maximum number of found CD reached"));
    case SPLT_CUE_OK:
      return splt_su_get_formatted_message(state, _(" cue file processed"));
    case SPLT_CDDB_OK:
      return splt_su_get_formatted_message(state, _(" cddb file processed"));
    case SPLT_FREEDB_FILE_OK:
      return splt_su_get_formatted_message(state, _(" freedb file downloaded"));
    case SPLT_FREEDB_OK:
      return splt_su_get_formatted_message(state, _(" freedb search processed"));
      //
    case SPLT_FREEDB_ERROR_INITIALISE_SOCKET:
      return splt_su_get_formatted_message(state,
          _(" freedb error: cannot initialise socket (%s)"),
          state->err.strerror_msg);
    case SPLT_FREEDB_ERROR_CANNOT_GET_HOST:
      return splt_su_get_formatted_message(state,
          _(" freedb error: cannot get host '%s' by name (%s)"),
          state->err.error_data, state->err.strerror_msg);
    case SPLT_FREEDB_ERROR_CANNOT_OPEN_SOCKET:
      return splt_su_get_formatted_message(state,
          _(" freedb error: cannot open socket (%s)"));
    case SPLT_FREEDB_ERROR_CANNOT_CONNECT:
      return splt_su_get_formatted_message(state,
          _(" freedb error: cannot connect to host '%s' (%s)"),
          state->err.error_data, state->err.strerror_msg);
    case SPLT_FREEDB_ERROR_CANNOT_DISCONNECT:
      return splt_su_get_formatted_message(state,
          _(" freedb error: cannot disconnect from host (%s)"),
          state->err.strerror_msg);
    case SPLT_FREEDB_ERROR_CANNOT_SEND_MESSAGE:
      return splt_su_get_formatted_message(state,
          _(" freedb error: cannot send message to host '%s' (%s)"),
          state->err.error_data, state->err.strerror_msg);
    case SPLT_FREEDB_ERROR_INVALID_SERVER_ANSWER:
      return splt_su_get_formatted_message(state,
          _(" freedb error: invalid server answer"));
    case SPLT_FREEDB_ERROR_SITE_201:
      return splt_su_get_formatted_message(state,
          _(" freedb error: site returned code 201"));
    case SPLT_FREEDB_ERROR_SITE_200:
      return splt_su_get_formatted_message(state,
          _(" freedb error: site returned code 200"));
    case SPLT_FREEDB_ERROR_BAD_COMMUNICATION:
      return splt_su_get_formatted_message(state,
          _(" freedb error: bad communication with site"));
    case SPLT_FREEDB_ERROR_GETTING_INFOS:
      return splt_su_get_formatted_message(state,
          _(" freedb error: could not get infos from site '%s'"),
          state->err.error_data);
    case SPLT_FREEDB_NO_CD_FOUND:
      return splt_su_get_formatted_message(state, _(" no CD found for this search"));
    case SPLT_FREEDB_ERROR_CANNOT_RECV_MESSAGE:
      return splt_su_get_formatted_message(state,
          _(" freedb error: cannot receive message from server '%s' (%s)"),
          state->err.error_data, state->err.strerror_msg);
    case SPLT_FREEDB_ERROR_PROXY_NOT_SUPPORTED:
      return splt_su_get_formatted_message(state,
          _(" proxy not supported with this type of protocol"));
    case SPLT_INVALID_CUE_FILE:
      return splt_su_get_formatted_message(state, _(" cue error: invalid cue file '%s'"),
          state->err.error_data);
    case SPLT_INVALID_CDDB_FILE:
      return splt_su_get_formatted_message(state, _(" cddb error: invalid cddb file '%s'"),
          state->err.error_data);
    case SPLT_FREEDB_NO_SUCH_CD_IN_DATABASE:
      return splt_su_get_formatted_message(state,
          _(" freedb error: No such CD entry in database"));
    case SPLT_FREEDB_ERROR_SITE:
      return splt_su_get_formatted_message(state,
          _(" freedb error: bad response from remote host"));
    case SPLT_ERROR_INTERNAL_SHEET:
      return splt_su_get_formatted_message(state,
          _(" internal sheet error: no internal sheet or invalid sheet"));
    case SPLT_ERROR_INTERNAL_SHEET_TYPE_NOT_SUPPORTED:
      return splt_su_get_formatted_message(state,
          _(" internal sheet error: internal sheet type not supported"));

      //
    case SPLT_DEWRAP_OK:
      return splt_su_get_formatted_message(state, _(" wrap split ok"));
      //
    case SPLT_DEWRAP_ERR_FILE_LENGTH:
      return splt_su_get_formatted_message(state, _(" wrap error: incorrect file length"));
    case SPLT_DEWRAP_ERR_VERSION_OLD:
      return splt_su_get_formatted_message(state,
          _(" wrap error: libmp3splt version too old for this wrap file"));
    case SPLT_DEWRAP_ERR_NO_FILE_OR_BAD_INDEX:
      return splt_su_get_formatted_message(state,
          _(" wrap error: no file found or bad index"));
    case SPLT_DEWRAP_ERR_FILE_DAMAGED_INCOMPLETE:
      return splt_su_get_formatted_message(state,
          _(" wrap error: file '%s' damaged or incomplete"),
          state->err.error_data);
    case SPLT_DEWRAP_ERR_FILE_NOT_WRAPED_DAMAGED:
      return splt_su_get_formatted_message(state,
          _(" wrap error: maybe not a wrapped file or wrap file damaged"));
      //
    case SPLT_OK_SPLIT_EOF:
      return splt_su_get_formatted_message(state, _(" file split (EOF)"));
    case SPLT_NO_SILENCE_SPLITPOINTS_FOUND:
      return splt_su_get_formatted_message(state, _(" no silence splitpoints found"));
    case SPLT_TIME_SPLIT_OK:
      return splt_su_get_formatted_message(state, _(" time split ok"));
    case SPLT_LENGTH_SPLIT_OK:
      return splt_su_get_formatted_message(state, _(" split in equal tracks ok"));
    case SPLT_SILENCE_OK:
      return splt_su_get_formatted_message(state, _(" silence split ok"));
    case SPLT_TRIM_SILENCE_OK:
      return splt_su_get_formatted_message(state, _(" trim using silence split ok"));
    case SPLT_SPLITPOINT_BIGGER_THAN_LENGTH:
      return splt_su_get_formatted_message(state,
          _(" file split, splitpoints bigger than length"));
    case SPLT_OK_SPLIT:
      return splt_su_get_formatted_message(state, _(" file split"));
    case SPLT_OK:
      return NULL;
    case SPLT_ERROR_SPLITPOINTS:
      return splt_su_get_formatted_message(state, _(" error: not enough splitpoints (<2)"));
    case SPLT_ERROR_CANNOT_OPEN_FILE:
      return splt_su_get_formatted_message(state,
          _(" error: cannot open file '%s': %s"),
          state->err.error_data, state->err.strerror_msg);
    case SPLT_ERROR_CANNOT_CLOSE_FILE:
      return splt_su_get_formatted_message(state,
          _(" error: cannot close file '%s': %s"),
          state->err.error_data, state->err.strerror_msg);
    case SPLT_ERROR_INVALID:
      ;
      int err = SPLT_OK;
      const char *plugin_name = splt_p_get_name(state, &err);
      return splt_su_get_formatted_message(state,
          _(" error: invalid input file '%s' for '%s' plugin"),
          state->err.error_data, plugin_name);
    case SPLT_ERROR_EQUAL_SPLITPOINTS:
      return splt_su_get_formatted_message(state,
          _(" error: splitpoints are equal (%s)"),
          state->err.error_data);
    case SPLT_ERROR_TIME_SPLIT_VALUE_INVALID:
      return splt_su_get_formatted_message(state, _(" error: invalid time split value"));
    case SPLT_ERROR_LENGTH_SPLIT_VALUE_INVALID:
      return splt_su_get_formatted_message(state,
          _(" error: invalid number of files for equal length split"));
    case SPLT_ERROR_CANNOT_GET_TOTAL_TIME:
      return splt_su_get_formatted_message(state,
          _(" error: cannot get total audio length"));
    case SPLT_ERROR_LIBID3:
      return splt_su_get_formatted_message(state,
          _(" error: unknown error while setting tags with libid3"));
    case SPLT_ERROR_FAILED_BITRESERVOIR:
      return splt_su_get_formatted_message(state,
          " error: bit reservoir failed - %s", state->err.error_data);
    case SPLT_ERROR_SPLITPOINTS_NOT_IN_ORDER:
      return splt_su_get_formatted_message(state,
          _(" error: the splitpoints are not in order (%s)"),
          state->err.error_data);
    case SPLT_ERROR_NEGATIVE_SPLITPOINT:
      return splt_su_get_formatted_message(state, _(" error: negative splitpoint (%s)"),
          state->err.error_data);
    case SPLT_ERROR_INCORRECT_PATH:
      return splt_su_get_formatted_message(state,
          _(" error: bad destination folder '%s' (%s)"),
          state->err.error_data, state->err.strerror_msg);
    case SPLT_ERROR_INCOMPATIBLE_OPTIONS:
      return splt_su_get_formatted_message(state, _(" error: incompatible options"));
    case SPLT_ERROR_INPUT_OUTPUT_SAME_FILE:
      return splt_su_get_formatted_message(state,
          _(" input and output are the same file ('%s')"),
          state->err.error_data);
    case SPLT_ERROR_CANNOT_ALLOCATE_MEMORY:
      return splt_su_get_formatted_message(state, _(" error: cannot allocate memory"));
    case SPLT_ERROR_CANNOT_OPEN_DEST_FILE:
      return splt_su_get_formatted_message(state,
          _(" error: cannot open destination file '%s': %s"),
          state->err.error_data,state->err.strerror_msg);
    case SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE:
      return splt_su_get_formatted_message(state,
          _(" error: cannot write to output file '%s'"),
          state->err.error_data);
    case SPLT_ERROR_WHILE_READING_FILE:
      return splt_su_get_formatted_message(state,
          _(" error: error while reading file '%s': %s"),
          state->err.error_data, state->err.strerror_msg);
    case SPLT_ERROR_SEEKING_FILE:
      return splt_su_get_formatted_message(state, _(" error: cannot seek file '%s'"),
          state->err.error_data);
    case SPLT_ERROR_BEGIN_OUT_OF_FILE:
      return splt_su_get_formatted_message(state, _(" error: begin point out of file"));
    case SPLT_ERROR_INEXISTENT_FILE:
      return splt_su_get_formatted_message(state, _(" error: inexistent file '%s': %s"),
          state->err.error_data,state->err.strerror_msg);
    case SPLT_SPLIT_CANCELLED:
      return splt_su_get_formatted_message(state, _(" split process cancelled"));
    case SPLT_ERROR_LIBRARY_LOCKED: 
      return splt_su_get_formatted_message(state, _(" error: library locked"));
    case SPLT_ERROR_STATE_NULL:
      return splt_su_get_formatted_message(state,
          _(" error: the state has not been initialized with 'mp3splt_new_state'"));
    case SPLT_ERROR_NEGATIVE_TIME_SPLIT:
      return splt_su_get_formatted_message(state, _(" error: negative time split"));
    case SPLT_ERROR_CANNOT_CREATE_DIRECTORY:
      return splt_su_get_formatted_message(state,
          _(" error: cannot create directory '%s'"),
          state->err.error_data);
    case SPLT_ERROR_NO_PLUGIN_FOUND:
      return splt_su_get_formatted_message(state, _(" error: no plugin found"));
    case SPLT_ERROR_CANNOT_INIT_LIBLTDL:
      return splt_su_get_formatted_message(state, _(" error: cannot initiate libltdl"));
    case SPLT_ERROR_CRC_FAILED:
      return splt_su_get_formatted_message(state, _(" error: CRC failed"));
    case SPLT_ERROR_NO_PLUGIN_FOUND_FOR_FILE:
      return splt_su_get_formatted_message(state,
          _(" error: no plugin matches the file '%s'"),
          state->err.error_data);
      //
    case SPLT_OUTPUT_FORMAT_OK:
      return NULL;
    case SPLT_OUTPUT_FORMAT_AMBIGUOUS:
      return splt_su_get_formatted_message(state,
          _(" warning: output format ambiguous (@t or @n missing)"));
      //
    case SPLT_OUTPUT_FORMAT_ERROR:
      if (state->err.error_data != NULL && strlen(state->err.error_data) == 1)
      {
        return splt_su_get_formatted_message(state,
            _(" error: illegal variable '@%s' in output format"),
            state->err.error_data);
      }
      else
      {
        return splt_su_get_formatted_message(state, _(" error: invalid output format"));
      }
      //
    case SPLT_ERROR_INEXISTENT_SPLITPOINT:
      return splt_su_get_formatted_message(state, _(" error: inexistent splitpoint"));
      //
    case SPLT_ERROR_PLUGIN_ERROR:
      return splt_su_get_formatted_message(state, _(" plugin error: '%s'"),
          state->err.error_data);
      //
    case SPLT_PLUGIN_ERROR_UNSUPPORTED_FEATURE:
      ;
      splt_plugins *pl = state->plug;
      int current_plugin = splt_p_get_current_plugin(state);
      return splt_su_get_formatted_message(state,
          _(" error: unsupported feature for the plugin '%s'"),
          pl->data[current_plugin].info.name);
    case SPLT_AUDACITY_OK:
      return splt_su_get_formatted_message(state, _(" audacity labels file processed"));
    case SPLT_INVALID_AUDACITY_FILE:
      return splt_su_get_formatted_message(state,
          _(" audacity error: invalid audacity labels file '%s'"),
          state->err.error_data);
    case SPLT_INVALID_REGEX:
      return splt_su_get_formatted_message(state,
          _(" regular expression error: %s"), state->err.error_data);
    case SPLT_REGEX_UNAVAILABLE:
      return splt_su_get_formatted_message(state, _(" regular expressions are not supported"));
    case SPLT_REGEX_NO_MATCH:
      return splt_su_get_formatted_message(state, _(" regular expression error: no match"));
    case SPLT_REGEX_OK:
      return splt_su_get_formatted_message(state, _(" regular expression ok"));
    case SPLT_ERROR_NO_AUTO_ADJUST_FOUND:
      return splt_su_get_formatted_message(state, _(" error: one of the splitpoints has not been auto-adjusted"));
    case SPLT_ERROR_INVALID_CODE:
      return splt_su_get_formatted_message(state, _(" invalid code error: '%s'"),
          state->err.error_data);
  }

  return NULL;
}

void splt_e_set_strerr_msg(splt_state *state, const char *message)
{
  splt_su_copy(message, &state->err.strerror_msg);
}

