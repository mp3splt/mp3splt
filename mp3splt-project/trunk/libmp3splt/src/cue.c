/**********************************************************
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2010 Alexandru Munteanu - io_fx@yahoo.fr
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *********************************************************/

#include <string.h>

#include "splt.h"
#include "cddb_cue_common.h"

#include "cue.h"

//-if we have a string "test", this functions cuts out the two "
//and cleans the string of other unwanted characters
//-then puts the information in the tags
//-returns possible error
static int splt_cue_set_value(splt_state *state, char *in,
    int index, int tag_field)
{
  int error = SPLT_OK;

  char *ptr_b = in, *ptr_e = NULL;

	if (in)
	{
		//-find begin
		//while we don't have spaces any more
		while (*ptr_b == ' ')
		{
			ptr_b++;
		}
		//if we have a '"', then skip quote
		if (*ptr_b == '"')
		{
			ptr_b++;
		}

		//-find and and put '\0'
		//go to the end of line
		ptr_e = strchr(ptr_b+1,'\n');
		if (ptr_e)
		{
			//move back ignoring spaces
			while (*ptr_e == ' ')
			{
				ptr_e--;
			}
			//if we have a '"', then skip quote
			if (*(ptr_e-1) == '"')
			{
				ptr_e--;
			}
			*ptr_e = '\0';
		}

		char *out = NULL;
		if ((out = malloc(strlen(ptr_b)+1)) == NULL)
		{
			error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
		}
		else
		{
			strncpy(out, ptr_b, (strlen(ptr_b)+1));
			int tags_err = SPLT_OK;

			//put Artist + Album info to client
			char *client_infos = malloc(sizeof(char) * (strlen(out)+30));
			if (client_infos == NULL)
			{
				error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
			}
			else
			{
				if (tag_field == SPLT_TAGS_ARTIST)
				{
					snprintf(client_infos,strlen(out)+30, _("\n  Artist: %s\n"), out);
					splt_t_put_info_message_to_client(state, client_infos);
				}
				else if (tag_field == SPLT_TAGS_ALBUM)
				{
					snprintf(client_infos,strlen(out)+30, _("  Album: %s\n"), out);
					splt_t_put_info_message_to_client(state, client_infos);
				}
				free(client_infos);
				client_infos = NULL;

				tags_err = splt_tu_set_tags_char_field(state, index, tag_field, out);
				if (tags_err != SPLT_OK)
				{
					error = tags_err;
				}
			}
			if (out)
			{
				free(out);
				out = NULL;
			}
		}
	}

  return error;
}

//error = possible error that we return
//gets the cue splitpoints from a file and puts them in the state
//returns the number of tracks found
int splt_cue_put_splitpoints(const char *file, splt_state *state, int *error)
{
  if (file == NULL)
  { 
    *error = SPLT_INVALID_CUE_FILE;
    return 0;
  }

  //clear previous splitpoints
  splt_t_free_splitpoints_tags(state);

  //default no error
  *error = SPLT_CUE_OK;
  
  char *client_infos = malloc(sizeof(char) * (strlen(file)+200));
  //put information to client
  if (client_infos == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return 0;
  }
  snprintf(client_infos, strlen(file)+200,
      _(" reading informations from CUE file %s ...\n"),file);
  splt_t_put_info_message_to_client(state, client_infos);
  free(client_infos);
  client_infos = NULL;

  int append_error = SPLT_OK;
  //our file
  FILE *file_input = NULL;
  //the line we read from the file
  char line[2048] = { '\0' };
  char *ptr = NULL, *dot = NULL;
  ptr = dot = NULL;
  //tracks = number of tracks found
  int tracks = -1, time_for_track = SPLT_TRUE;
  //counter, counts if we have at least one INDEX on the file
  int type = SPLT_CUE_NOTHING, performer = SPLT_FALSE,
    title = SPLT_FALSE, counter = 0;
  int temp_error = SPLT_OK;
  int tags_error = SPLT_OK;
  
  //default genre
  tags_error = splt_tu_set_tags_uchar_field(state, 0, SPLT_TAGS_GENRE, 12);
  if (tags_error != SPLT_OK)
  {
    *error = tags_error;
    return tracks;
  }

  //if we cannot open the file
  if (!(file_input=splt_u_fopen(file, "r")))
  {
    splt_t_set_strerror_msg(state);
    splt_t_set_error_data(state,file);
    *error = SPLT_ERROR_CANNOT_OPEN_FILE;
    return tracks;
  }
  //if we can open the file
  else
  {
    //put pointer at the beginning of the file
    if (fseek(file_input, 0, SEEK_SET) == 0)
    {
      //contains the start of the content of the line;
      //-for example for the '      PERFORMER abcd' line,
      //it will point to ' abcd'
      char *line_content = NULL;

      //we read the file line by line
      while (fgets(line, 2048, file_input)!=NULL)
      {
        //if windows file with '\r', then pretend is a unix file
        if (strlen(line) > 1)
        {
          if (line[strlen(line)-2] == '\r')
          {
            line[strlen(line)-2] = '\n';
            line[strlen(line)-1] = '\0';
          }
        }

        type = SPLT_CUE_NOTHING;

        //we read strings from file TRACK,TITLE,AUDIO,PERFORMER,INDEX
        if (((line_content = strstr(line, "TRACK"))!=NULL)
            && (strstr(line, "AUDIO")!=NULL))
        {
          line_content += 5;
          type = SPLT_CUE_TRACK;
        }
        else if ((line_content = strstr(line, "TITLE")) != NULL)
        {
          line_content += 5;
          type = SPLT_CUE_TITLE;
        }
        else if ((line_content = strstr(line, "PERFORMER")) != NULL)
        {
          type = SPLT_CUE_PERFORMER;
          line_content += 9;
        }
        else if ((ptr=strstr(line, "INDEX 01"))!=NULL)
        {
          type = SPLT_CUE_INDEX;
        }

        //we clean previous data
        splt_t_clean_one_split_data(state, tracks);

        //we analyse the previous strings found
        switch (type)
        {
          //nothing found
          case SPLT_CUE_NOTHING:
            break;
          //TRACK and AUDIO found
          case SPLT_CUE_TRACK:
            if (tracks==-1) 
            {
              tracks = 0;
            }
            if (time_for_track) 
            {
              performer = SPLT_FALSE;
              title = SPLT_FALSE;
              tracks++;
              splt_tu_new_tags_if_necessary(state, tracks-1);
            }
            else
            {
              splt_t_set_error_data(state,file);
              *error = SPLT_INVALID_CUE_FILE;
              goto function_end;
            }
            time_for_track = SPLT_FALSE;
            break;
          //TITLE found
          case SPLT_CUE_TITLE:
            if (tracks == -1)
            {
              if ((temp_error = splt_cue_set_value(state, line_content, 
                      0, SPLT_TAGS_ALBUM)) != SPLT_OK)
              {
                *error = temp_error;
                goto function_end;
              }
            }
            else
            {
              if (tracks > 0)
              {
                if ((temp_error = splt_cue_set_value(state, line_content,
                        tracks-1, SPLT_TAGS_TITLE)) != SPLT_OK)
                {
                  *error = temp_error;
                  goto function_end;
                }
              }
              title = SPLT_TRUE;
            }
            break;
          //PERFORMER found
          case SPLT_CUE_PERFORMER:        
            if (tracks == -1)
            {
              //we always have one artist in a cue file, we
              //put the performers if more than one artist
              if ((temp_error = splt_cue_set_value(state, line_content,
                      0, SPLT_TAGS_ARTIST)) != SPLT_OK)
              {
                *error = temp_error;
                goto function_end;
              }
            }
            else
            {
              if (tracks > 0)
              {
                if ((temp_error = splt_cue_set_value(state, line_content,
                        tracks-1, SPLT_TAGS_PERFORMER)) != SPLT_OK)
                {
                  *error = temp_error;
                  goto function_end;
                }
              }
              performer = SPLT_TRUE;
            }
            break;
          //INDEX 01 found
          case SPLT_CUE_INDEX:
            line[strlen(line)-1]='\0';
            ptr += 9;
            if ((dot = strchr(ptr, ':'))==NULL)
            {
              splt_t_set_error_data(state,file);
              *error = SPLT_INVALID_CUE_FILE;
              goto function_end;
            }
            else
            {
              //we replace ':' with . for the sscanf
              ptr[dot-ptr] = ptr[dot-ptr+3] = '.';

              //we convert to hundreths of seconds and put splitpoints
              if (tracks > 0)
              {
                long hundr_seconds = splt_u_convert_hundreths(ptr);
                if (hundr_seconds==-1)
                {
                  splt_t_set_error_data(state,file);
                  *error = SPLT_INVALID_CUE_FILE;
                  goto function_end;
                }

                //we append the splitpoint
                append_error =
                  splt_t_append_splitpoint(state, hundr_seconds, NULL, SPLT_SPLITPOINT);
                if (append_error != SPLT_OK)
                {
                  *error = append_error;
                  goto function_end;
                }

                time_for_track = SPLT_TRUE;
                counter++;
              }
            }
            break;
          default:
            break;
        }
      }
      //we append the last splitpoint
      append_error = splt_t_append_splitpoint(state, LONG_MAX,
          _("description here"), SPLT_SPLITPOINT);
    }
    else
    {
      splt_t_set_strerror_msg(state);
      splt_t_set_error_data(state,file);
      *error = SPLT_ERROR_SEEKING_FILE;
      goto function_end;
    }

    //if we don't find INDEX on the file, error
    if (counter == 0)
    {
      splt_t_set_error_data(state,file);
      *error = SPLT_INVALID_CUE_FILE;
      goto function_end;
    }

    //if no time for track,
    if (!time_for_track) 
    {
      tracks--;
    }

    splt_tag_put_filenames_from_tags(state,tracks,error);

function_end:
    if (fclose(file_input) != 0)
    {
      splt_t_set_strerror_msg(state);
      splt_t_set_error_data(state, file);
      *error = SPLT_ERROR_CANNOT_CLOSE_FILE;
    }
    file_input = NULL;
  }
  
  //put number of tracks
  char tracks_info[64] = { '\0' };
  snprintf(tracks_info, 64, _("  Tracks: %d\n\n"),tracks);
  splt_t_put_info_message_to_client(state, tracks_info);

  //we return the number of tracks found
  return tracks;
}

static void splt_cue_write_title_performer(splt_state *state, FILE *file_output,
    int tags_index, short with_spaces, short write_album)
{
  splt_tags *tags = NULL;
  if (tags_index >= 0)
  {
    tags = splt_tu_get_tags_at(state, tags_index);
  }
  else
  {
    tags = splt_tu_get_current_tags(state);
  }

  if (tags)
  {
    if (write_album)
    {
      if (tags->album)
      {
        if (with_spaces) { fprintf(file_output, "    "); }
        fprintf(file_output, "TITLE \"%s\"\n", tags->album);
      }
    }
    else
    {
      if (tags->title)
      {
        if (with_spaces) { fprintf(file_output, "    "); }
        fprintf(file_output, "TITLE \"%s\"\n", tags->title);
      }
    }

    char *performer = splt_u_get_artist_or_performer_ptr(tags);
    if (performer)
    {
      if (with_spaces) { fprintf(file_output, "    "); }
      fprintf(file_output, "PERFORMER \"%s\"\n", performer);
    }
  }
  else
  {
    if (with_spaces) { fprintf(file_output, "    "); }
    fprintf(file_output, "TITLE \"\"\n");
    if (with_spaces) { fprintf(file_output, "    "); }
    fprintf(file_output, "PERFORMER \"\"\n");
  }
}

void splt_cue_export_to_file(splt_state *state, const char *out_file,
    short stop_at_total_time, int *error)
{
  *error = SPLT_OK;

  int num_of_splitpoints = splt_t_get_splitnumber(state);
  if (num_of_splitpoints <= 0)
  {
    return;
  }

  long total_time = splt_t_get_total_time(state);
  FILE *file_output = NULL;

  splt_u_print_debug(state, "cue output file without output path = ",0, out_file);

  char *dup_out_file = strdup(out_file);
  if (!dup_out_file) { *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY; goto end; };
  char *cue_out_file = splt_u_get_file_with_output_path(state, dup_out_file, error);
  free(dup_out_file);
  dup_out_file = NULL;
  if (*error < 0) { goto end; }

  splt_u_print_debug(state, "cue output file with output path = ",0, cue_out_file);

  //we write the result to the file
  if (!(file_output = splt_u_fopen(cue_out_file, "w")))
  {
    splt_t_set_strerror_msg(state);
    splt_t_set_error_data(state, cue_out_file);
    *error = SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE;
  }
  else
  {
    splt_cue_write_title_performer(state, file_output, 0,
        SPLT_FALSE, SPLT_TRUE);

    char *fname = splt_t_get_filename_to_split(state);

    const char *upper_ext = splt_p_get_upper_extension(state, error);
    char new_upper_ext[10] = { '\0' };
    int i = 0;
    for (i = 1;i < strlen(upper_ext);i++)
    {
      new_upper_ext[i-1] = upper_ext[i];
    }

    fprintf(file_output, "FILE \"%s\" %s\n", fname, new_upper_ext);
    if (*error < 0) { goto end; }
 
    splt_t_set_current_split(state,0);
    for (i = 0;i < num_of_splitpoints;i++)
    {
      int get_err = SPLT_OK;
      long splitpoint = splt_t_get_splitpoint_value(state, i, &get_err);
      if (get_err < 0) { *error = get_err; break; }

      if (stop_at_total_time)
      {
        //todo: splitpoint can be slightly != than total_time sometimes
        // (test with silence and cue)
        if (total_time > 0  && splitpoint >= total_time)
        {
          break;
        }
      }

      fprintf(file_output, "  TRACK %02d AUDIO\n", i+1);

      splt_cue_write_title_performer(state, file_output, -1, SPLT_TRUE, SPLT_FALSE);

      long mins = 0, secs = 0, hundr = 0;
      splt_t_get_mins_secs_hundr_from_splitpoint(splitpoint, &mins, &secs, &hundr);

      fprintf(file_output, "    INDEX 01 %02ld:%02ld:%02ld\n", mins, secs, hundr);

      splt_t_current_split_next(state);
    }

end:
    if (fclose(file_output) != 0)
    {
      splt_t_set_strerror_msg(state);
      splt_t_set_error_data(state, cue_out_file);
      *error = SPLT_ERROR_CANNOT_CLOSE_FILE;
    }
    file_output = NULL;
  }

  char infos[2048] = { '\0' };
  snprintf(infos,2048,_(" CUE file '%s' created.\n"), cue_out_file);
  splt_t_put_info_message_to_client(state, infos);

  if (cue_out_file)
  {
    free(cue_out_file);
    cue_out_file = NULL;
  }
}

