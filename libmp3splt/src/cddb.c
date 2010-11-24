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
#include <ctype.h>
#include <math.h>

#include "splt.h"
#include "cddb_cue_common.h"

#include "cddb.h"

static void splt_cddb_process_line(char **l, cddb_utils *cdu, splt_state *state);
static void splt_cddb_process_disc_length_line(const char *line_content, cddb_utils *cdu, splt_state *state);
static void splt_cddb_convert_points(cddb_utils *cdu, splt_state *state);
static void splt_cddb_process_offset_line(const char *line_content, cddb_utils *cdu, splt_state *state);
static cddb_utils *splt_cddb_cdu_new(splt_state *state, int *error);
static void splt_cddb_cdu_free(cddb_utils **cdu);
static void splt_cddb_process_year_line(const char *line_content,
    cddb_utils *cdu, splt_state *state);
static void splt_cddb_process_genre_line(const char *line_content,
    cddb_utils *cdu, splt_state *state);
static void splt_cddb_process_dtitle_line(const char *line_content, cddb_utils *cdu, splt_state *state);
static void splt_cddb_process_ttitle_line(const char *line_content, cddb_utils *cdu, splt_state *state);
static void splt_cddb_process_id3g_line(const char *line_content, cddb_utils *cdu, splt_state *state);

int new_splt_cddb_put_splitpoints(const char *file, splt_state *state, int *error)
{
  if (file == NULL)
  {
    *error = SPLT_INVALID_CDDB_FILE;
    return 0;
  }

  splt_c_put_info_message_to_client(state, 
      _(" reading informations from CDDB file %s ...\n"),file);

  splt_t_free_splitpoints_tags(state);

  *error = SPLT_CDDB_OK;

  int err = SPLT_OK;
  FILE *file_input = NULL;
  char *line = NULL;
  int tracks = 0;

  cddb_utils *cdu = splt_cddb_cdu_new(state, &err);
  if (err < 0) { *error = err; return tracks; }
  cdu->file = file;

  char prev[10] = { '\0' };
  //performer_title_split is where we split if we have
  //performer / title on cddb
  char *number = NULL, *c = NULL;
  int i = 0, j = 0;
  int append_error = SPLT_OK;
  int change_error = SPLT_OK;
  int get_error = SPLT_OK;
  int tags_error = SPLT_OK;
  //temporary variables
  long split1 = 0, split2 = 0;

  char *artist = NULL;
  char *album = NULL;
  char *perfor = NULL;

  if (!(file_input=splt_io_fopen(file, "r")))
  {
    splt_e_set_strerror_msg_with_data(state, file);
    *error = SPLT_ERROR_CANNOT_OPEN_FILE;
    return tracks;
  }

  if (fseek(file_input, 0, SEEK_SET) != 0)
  {
    splt_e_set_strerror_msg_with_data(state, file);
    *error = SPLT_ERROR_SEEKING_FILE;
    goto function_end;
  }

  while ((line = splt_io_readline(file_input, error)) != NULL)
  {
    if (*error < 0) { goto function_end; }

    splt_cddb_process_line(&line, cdu, state);
    tracks = cdu->tracks;
    if (cdu->error < 0) { *error = cdu->error; goto function_end; }
  }

  //search for "Track frame offset"
  do {
    line = splt_io_readline(file_input, error);
    if (*error < 0 || line == NULL)
    {
      splt_e_set_error_data(state,file);
      *error = SPLT_INVALID_CDDB_FILE;
      goto function_end;
    }
    number = strstr(line, "Track frame offset");

    if (line)
    {
      free(line);
      line = NULL;
    }
  } while (number == NULL);

  memset(prev, 0, 10);

  //read the track offsets
  do {
    if (line)
    {
      free(line);
      line = NULL;
    }
    line = splt_io_readline(file_input, error);
    if (*error < 0 || line == NULL)
    {
      splt_e_set_error_data(state,file);
      *error = SPLT_INVALID_CDDB_FILE;
      goto function_end;
    }
    line[strlen(line)-1] = '\0';
    if (strstr(line, "Disc length") != NULL)
    {
      break;
    }
    i = 0;
    while ((isdigit(line[i])==0) && (line[i]!='\0'))
    {
      i++;
      number = line + i;
    }
    if (number == (line + strlen(line))) 
    {
      break;
    }
    else
    {
      double temp = 0;
      temp = atof(number);

      //append the splitpoint
      //in cddb_offset*100
      //convert them lower to seconds
      append_error =
        splt_sp_append_splitpoint(state, temp * 100, NULL, SPLT_SPLITPOINT);

      if (append_error != SPLT_OK)
      {
        *error = append_error;
        goto function_end;
      }
      else
      {
        tracks++;
      }
    }

  } while (1);

  //we check parse output format
  //setting the number of tracks is important
  splt_t_set_splitnumber(state,tracks);

  //we clean the old data from the state
  splt_t_clean_split_data(state,tracks);

  //if we have Disc length right after
  //the splitpoints
  if (strstr(line,"Disc length") != NULL)
  {
    number = strstr(line, "Disc length");
    free(line);
    line = NULL;
  }
  else
  {
    //find out "Disc length"
    do {
      if (line)
      {
        free(line);
        line = NULL;
      }
      line = splt_io_readline(file_input, error);
      if (*error < 0 || line == NULL)
      {
        splt_e_set_error_data(state,file);
        *error = SPLT_INVALID_CDDB_FILE;
        goto function_end;
      }
      number = strstr(line, "Disc length");
    } while (number == NULL);
  }

  //we get the max disc length (in seconds)
  i = 0;
  while ((isdigit(line[i])==0) && (line[i]!='\0'))
  {
    i++;
    number = line + i;
  }

  //if no error, ?
  if (*error != SPLT_INVALID_CDDB_FILE)
  {
    double temp2 = 0;
    temp2 = atof(number);

    //we append the splitpoint (in seconds*100)
    append_error = splt_sp_append_splitpoint(state, temp2 * 100, NULL, SPLT_SPLITPOINT);
    if (append_error != SPLT_OK)
    {
      *error = append_error;
      goto function_end;
    }
  }
  else
  {
    //we append 0 as splitpoint
    append_error = splt_sp_append_splitpoint(state, 0, NULL, SPLT_SPLITPOINT);
    if (append_error != SPLT_OK)
    {
      *error = append_error;
      goto function_end;
    }
  }

  split2 = splt_sp_get_splitpoint_value(state, 0, &get_error);
  if (get_error != SPLT_OK)
  {
    *error = get_error;
    goto function_end;
  }

  //we convert the points previously found
  for (i=tracks-1; i>=0; i--)
  {
    split1 = splt_sp_get_splitpoint_value(state, i, &get_error);
    if (get_error != SPLT_OK)
    {
      *error = get_error;
      goto function_end;
    }

    //we remove the cddb_offset of the first splitpoint
    //and we divide by 75 (cddb specs)
    long difference = split1 - split2;
    float real_value = (float) difference / 75.f;
    change_error =
      splt_sp_set_splitpoint_value(state, i, (long) ceilf(real_value));

    if (change_error != SPLT_OK)
    {
      *error = change_error;
      goto function_end;
    }
  }

  j=0;
  //if we have found the title of the song or not
  int title = SPLT_FALSE;
  //if we have performers or not
  int performer = SPLT_FALSE;
  do {
    title = SPLT_FALSE;
    char temp[10];
    memset(temp, 0, 10);
    if (line)
    {
      free(line);
      line = NULL;
    }
    line = splt_io_readline(file_input, error);
    if (*error < 0 || line == NULL)
    {
      splt_e_set_error_data(state,file);
      *error = SPLT_INVALID_CDDB_FILE;
      goto function_end;
    }
    line[strlen(line)-1] = '\0';
    if (strlen(line)>0)
    {
      if (line[strlen(line)-1]=='\r')
      {
        line[strlen(line)-1]='\0';
      }
    }

    //some cddb files have Year and Genre before TTITLE
    if (strstr(line, "YEAR") != NULL)
    {
      tags_error = splt_tu_set_tags_field(state, 0, SPLT_TAGS_YEAR, line+6);
      if (tags_error != SPLT_OK)
      {
        *error = tags_error;
        goto function_end;
      }
    }
    else if (strstr(line, "GENRE") != NULL)
    {
      char a[4];
      strncpy(a,line+6,3);
      //this tag doesn't work correctly because GENRE is not a number
      int t = atoi(a);
      tags_error = splt_tu_set_tags_field(state, 0, SPLT_TAGS_GENRE, &t);
      if (tags_error != SPLT_OK)
      {
        *error = tags_error;
        goto function_end;
      }
    }

    //we search for the title of the disc
    if (j==0)
    {
      if (strstr(line, "DTITLE")==NULL)
      {
        free(line);
        line = NULL;
        continue;
      }
    }
    else
    {
      //we search for the title of the tracks
      if (strstr(line, "TTITLE")==NULL)
      {
        free(line);
        line = NULL;
        continue;
      }
      else
      {
        title = SPLT_TRUE;
      }
    }

    //if we don't have '=', invalid file
    if ((number=strchr(line, '='))==NULL) 
    {
      splt_e_set_error_data(state,file);
      *error = SPLT_INVALID_CDDB_FILE;
      goto function_end;
    }

    //we read the string after the = or after the /
    //the / sign is found on the DTITLE=Artist / Disc
    //we put the string in number variable
    //used for the filename
    if (j > 0)
    {
      int len = number-line;
      if (len>10) 
      {
        len = 10;
      }
      strncpy(temp, line, len);
      if ((c = strchr(number, '/'))!=NULL) 
      {
        //if found '/' in TITLE, separate performer / title
        if (title)
        {
          char *c2 = strchr(number+1,'/');
          *c2 = '\0';
          splt_su_cut_spaces_from_end(c2-1);

          //we put performer
          performer = SPLT_TRUE;
          tags_error = 
            splt_tu_set_tags_field(state, j-1, SPLT_TAGS_PERFORMER,
                number+1);

          if (tags_error != SPLT_OK)
          {
            *error = tags_error;
            goto function_end;
          }
        }
        else
        {
          tags_error = 
            splt_tu_set_tags_field(state, j-1, SPLT_TAGS_PERFORMER,
                NULL);
          if (tags_error != SPLT_OK)
          {
            *error = tags_error;
            goto function_end;
          }
        }

        perfor = (char *)splt_tu_get_tags_field(state, j-1, SPLT_TAGS_PERFORMER);
        number = c+1;
      }
    }

    //we limit number to 512?
    if (strlen(++number)>512) 
    {
      number[512]='\0';
    }

    //if what we read contains Data or Track
    if ((j > 0) && (strstr(number, "Data")!=NULL) && 
        (strstr(number, "Track")!=NULL)) 
    {
      split1 = 
        splt_sp_get_splitpoint_value(state, j, &get_error);
      if (get_error != SPLT_OK)
      {
        *error = get_error;
        goto function_end;
      }

      change_error = 
        splt_sp_set_splitpoint_value(state,j-1, split1);

      if (change_error != SPLT_OK)
      {
        *error = change_error;
        goto function_end;
      }
      tracks -= 1;
    }
    else
    {
      //otherwise, we put the title
      if ((j > 0) && (strcmp(temp, prev)==0))
      {
        tags_error = splt_tu_set_tags_field(state, j-1, SPLT_TAGS_TITLE,
            number);
        if (tags_error != SPLT_OK)
        {
          *error =tags_error;
          goto function_end;
        }
      }
      else
      {
        //we treat DTITLE
        if (j == 0)
        {
          //we put the artist
          i=0;
          while ((number[i]!='/') && (number[i]!='\0')&&(i<127)) 
          {
            i++;
          }

          char ttemp[i+1];
          int i_temp = 0;
          for (i_temp = 0; i_temp < i;i_temp++)
          {
            ttemp[i_temp] = number[i_temp];
          }
          ttemp[i] = '\0';
          tags_error = splt_tu_set_tags_field(state, 0, SPLT_TAGS_ARTIST,
              ttemp);
          if (tags_error != SPLT_OK)
          {
            *error =tags_error;
            goto function_end;
          }

          //we cut the space at the end of the artist
          artist = (char *)splt_tu_get_tags_field(state,0, SPLT_TAGS_ARTIST);
          int k = strlen(artist)-1;
          while (artist[k] == ' ')
          {
            artist[k] = '\0';
            k--;
            if (k < 0)
            {
              break;
            }
          }

          splt_c_put_info_message_to_client(state, _("\n  Artist: %s\n"), artist);

          //we put the album
          i += 1;
          number = splt_su_cut_spaces(number+i);

          tags_error = splt_tu_set_tags_field(state, 0, SPLT_TAGS_ALBUM, 
              number);
          if (tags_error != SPLT_OK)
          {
            *error =tags_error;
            goto function_end;
          }

          album = (char *)splt_tu_get_tags_field(state,0, SPLT_TAGS_ALBUM);

          splt_c_put_info_message_to_client(state, _("  Album: %s\n"), album);
        }
        else
        {
          char *t = number;
          //if we don't have = before, it means
          //we have a performer with a  perf/title
          if (*(number-1) != '=')
          {
            t = number-1;
          }
          t = splt_su_cut_spaces(t);
          tags_error = splt_tu_set_tags_field(state, j-1,
              SPLT_TAGS_TITLE, t);
          if (tags_error != SPLT_OK)
          {
            *error =tags_error;
            goto function_end;
          }
        }
        j++;
      }
    }
    strncpy(prev, temp, 10);

    free(line);
    line = NULL;
  } while (j<=tracks);

  //we search for
  //YEAR (the year) and ID3 genre
  unsigned char default_genre = 12;
  tags_error = splt_tu_set_tags_field(state, 0, SPLT_TAGS_GENRE, &default_genre);
  if (tags_error != SPLT_OK)
  {
    *error =tags_error;
    goto function_end;
  }

  while ((line = splt_io_readline(file_input, error))!=NULL)
  {
    if (*error < 0 || line == NULL)
    {
      goto function_end;
    }

    line[strlen(line)-1] = '\0';
    if (strlen(line)>0)
    {
      if (line[strlen(line)-1]=='\r') 
      {
        line[strlen(line)-1]='\0';
      }
    }
    if (strstr(line, "EXTD")==NULL) 
    {
      free(line);
      line = NULL;
      continue;
    }
    else 
    {
      if ((number=strchr(line, '='))==NULL) 
      {
        free(line);
        line = NULL;
        break;
      }
      else 
      {
        if ((c=strstr(number, "YEAR"))!=NULL)
        {
          tags_error = splt_tu_set_tags_field(state, 0, SPLT_TAGS_YEAR, c+6);
          if (tags_error != SPLT_OK)
          {
            *error = tags_error;
            goto function_end;
          }
        }
        if ((c=strstr(number, "ID3G"))!=NULL) 
        {
          strncpy(line, c+6, 3);
          int id3g = atoi(line);
          tags_error = splt_tu_set_tags_field(state, 0, SPLT_TAGS_GENRE, &id3g);
          if (tags_error != SPLT_OK)
          {
            *error = tags_error;
            goto function_end;
          }
        }
        break;
      }
    }

    if (line)
    {
      free(line);
      line = NULL;
    }
  }

  splt_tag_put_filenames_from_tags(state,tracks,error);

function_end:
  splt_cddb_cdu_free(&cdu);

  if (line)
  {
    free(line);
    line = NULL;
  }

  if (fclose(file_input) != 0)
  {
    splt_e_set_strerror_msg_with_data(state, file);
    *error = SPLT_ERROR_CANNOT_CLOSE_FILE;
  }
  file_input = NULL;

  if (*error >= 0)
  {
    splt_c_put_info_message_to_client(state, _("  Tracks: %d\n\n"), tracks);
  }

  return tracks;
}

int splt_cddb_put_splitpoints(const char *file, splt_state *state, int *error)
{
  splt_t_free_splitpoints_tags(state);

  *error = SPLT_CDDB_OK;

  splt_c_put_info_message_to_client(state, 
      _(" reading informations from CDDB file %s ...\n"),file);

  //our file
  FILE *file_input = NULL;
  char *line = NULL;
  char prev[10] = { '\0' };
  //performer_title_split is where we split if we have
  //performer / title on cddb
  char *number = NULL, *c = NULL;
  //tracks is the number of tracks
  int tracks = 0, i = 0, j = 0;
  int append_error = SPLT_OK;
  int change_error = SPLT_OK;
  int get_error = SPLT_OK;
  int tags_error = SPLT_OK;
  //temporary variables
  long split1 = 0, split2 = 0;

  char *artist = NULL;
  char *album = NULL;
  char *perfor = NULL;

  if (!(file_input=splt_io_fopen(file, "r")))
  {
    splt_e_set_strerror_msg_with_data(state, file);
    *error = SPLT_ERROR_CANNOT_OPEN_FILE;
    return tracks;
  }

  if (fseek(file_input, 0, SEEK_SET) != 0)
  {
    splt_e_set_strerror_msg_with_data(state, file);
    *error = SPLT_ERROR_SEEKING_FILE;
    goto function_end;
  }

  //search for "Track frame offset"
  do {
    line = splt_io_readline(file_input, error);
    if (*error < 0 || line == NULL)
    {
      splt_e_set_error_data(state,file);
      *error = SPLT_INVALID_CDDB_FILE;
      goto function_end;
    }
    number = strstr(line, "Track frame offset");

    if (line)
    {
      free(line);
      line = NULL;
    }
  } while (number == NULL);

  memset(prev, 0, 10);

  //read the track offsets
  do {
    if (line)
    {
      free(line);
      line = NULL;
    }
    line = splt_io_readline(file_input, error);
    if (*error < 0 || line == NULL)
    {
      splt_e_set_error_data(state,file);
      *error = SPLT_INVALID_CDDB_FILE;
      goto function_end;
    }
    line[strlen(line)-1] = '\0';
    if (strstr(line, "Disc length") != NULL)
    {
      break;
    }
    i = 0;
    while ((isdigit(line[i])==0) && (line[i]!='\0'))
    {
      i++;
      number = line + i;
    }
    if (number == (line + strlen(line))) 
    {
      break;
    }
    else
    {
      double temp = 0;
      temp = atof(number);

      //append the splitpoint
      //in cddb_offset*100
      //convert them lower to seconds
      append_error =
        splt_sp_append_splitpoint(state, temp * 100, NULL, SPLT_SPLITPOINT);

      if (append_error != SPLT_OK)
      {
        *error = append_error;
        goto function_end;
      }
      else
      {
        tracks++;
      }
    }

  } while (1);

  //we check parse output format
  //setting the number of tracks is important
  splt_t_set_splitnumber(state,tracks);

  //we clean the old data from the state
  splt_t_clean_split_data(state,tracks);

  //if we have Disc length right after
  //the splitpoints
  if (strstr(line,"Disc length") != NULL)
  {
    number = strstr(line, "Disc length");
    free(line);
    line = NULL;
  }
  else
  {
    //find out "Disc length"
    do {
      if (line)
      {
        free(line);
        line = NULL;
      }
      line = splt_io_readline(file_input, error);
      if (*error < 0 || line == NULL)
      {
        splt_e_set_error_data(state,file);
        *error = SPLT_INVALID_CDDB_FILE;
        goto function_end;
      }
      number = strstr(line, "Disc length");
    } while (number == NULL);
  }

  //we get the max disc length (in seconds)
  i = 0;
  while ((isdigit(line[i])==0) && (line[i]!='\0'))
  {
    i++;
    number = line + i;
  }

  //if no error, ?
  if (*error != SPLT_INVALID_CDDB_FILE)
  {
    double temp2 = 0;
    temp2 = atof(number);

    //we append the splitpoint (in seconds*100)
    append_error = splt_sp_append_splitpoint(state, temp2 * 100, NULL, SPLT_SPLITPOINT);
    if (append_error != SPLT_OK)
    {
      *error = append_error;
      goto function_end;
    }
  }
  else
  {
    //we append 0 as splitpoint
    append_error = splt_sp_append_splitpoint(state, 0, NULL, SPLT_SPLITPOINT);
    if (append_error != SPLT_OK)
    {
      *error = append_error;
      goto function_end;
    }
  }

  split2 = splt_sp_get_splitpoint_value(state, 0, &get_error);
  if (get_error != SPLT_OK)
  {
    *error = get_error;
    goto function_end;
  }

  //we convert the points previously found
  for (i=tracks-1; i>=0; i--)
  {
    split1 = splt_sp_get_splitpoint_value(state, i, &get_error);
    if (get_error != SPLT_OK)
    {
      *error = get_error;
      goto function_end;
    }

    //we remove the cddb_offset of the first splitpoint
    //and we divide by 75 (cddb specs)
    long difference = split1 - split2;
    float real_value = (float) difference / 75.f;
    change_error =
      splt_sp_set_splitpoint_value(state, i, (long) ceilf(real_value));

    if (change_error != SPLT_OK)
    {
      *error = change_error;
      goto function_end;
    }
  }

  j=0;
  //if we have found the title of the song or not
  int title = SPLT_FALSE;
  //if we have performers or not
  int performer = SPLT_FALSE;
  do {
    title = SPLT_FALSE;
    char temp[10];
    memset(temp, 0, 10);
    if (line)
    {
      free(line);
      line = NULL;
    }
    line = splt_io_readline(file_input, error);
    if (*error < 0 || line == NULL)
    {
      splt_e_set_error_data(state,file);
      *error = SPLT_INVALID_CDDB_FILE;
      goto function_end;
    }
    line[strlen(line)-1] = '\0';
    if (strlen(line)>0)
    {
      if (line[strlen(line)-1]=='\r')
      {
        line[strlen(line)-1]='\0';
      }
    }

    //some cddb files have Year and Genre before TTITLE
    if (strstr(line, "YEAR") != NULL)
    {
      tags_error = splt_tu_set_tags_field(state, 0, SPLT_TAGS_YEAR, line+6);
      if (tags_error != SPLT_OK)
      {
        *error = tags_error;
        goto function_end;
      }
    }
    else if (strstr(line, "GENRE") != NULL)
    {
      char a[4];
      strncpy(a,line+6,3);
      //this tag doesn't work correctly because GENRE is not a number
      int t = atoi(a);
      tags_error = splt_tu_set_tags_field(state, 0, SPLT_TAGS_GENRE, &t);
      if (tags_error != SPLT_OK)
      {
        *error = tags_error;
        goto function_end;
      }
    }

    //we search for the title of the disc
    if (j==0)
    {
      if (strstr(line, "DTITLE")==NULL)
      {
        free(line);
        line = NULL;
        continue;
      }
    }
    else
    {
      //we search for the title of the tracks
      if (strstr(line, "TTITLE")==NULL)
      {
        free(line);
        line = NULL;
        continue;
      }
      else
      {
        title = SPLT_TRUE;
      }
    }

    //if we don't have '=', invalid file
    if ((number=strchr(line, '='))==NULL) 
    {
      splt_e_set_error_data(state,file);
      *error = SPLT_INVALID_CDDB_FILE;
      goto function_end;
    }

    //we read the string after the = or after the /
    //the / sign is found on the DTITLE=Artist / Disc
    //we put the string in number variable
    //used for the filename
    if (j > 0)
    {
      int len = number-line;
      if (len>10) 
      {
        len = 10;
      }
      strncpy(temp, line, len);

      /*fprintf(stdout,"line = _%s_\n",line);
      fprintf(stdout,"number = _%s_\n",number);
      fprintf(stdout,"temp = _%s_\n",temp);
      fflush(stdout);*/

      if ((c = strchr(number, '/'))!=NULL) 
      {
        //if found '/' in TITLE, separate performer / title
        if (title)
        {
          char *c2 = strchr(number+1,'/');
          *c2 = '\0';
          splt_su_cut_spaces_from_end(c2-1);

          //we put performer
          performer = SPLT_TRUE;
          tags_error = 
            splt_tu_set_tags_field(state, j-1, SPLT_TAGS_PERFORMER,
                number+1);

          if (tags_error != SPLT_OK)
          {
            *error = tags_error;
            goto function_end;
          }
        }
        else
        {
          tags_error = 
            splt_tu_set_tags_field(state, j-1, SPLT_TAGS_PERFORMER,
                NULL);
          if (tags_error != SPLT_OK)
          {
            *error = tags_error;
            goto function_end;
          }
        }

        perfor = (char *)splt_tu_get_tags_field(state, j-1, SPLT_TAGS_PERFORMER);
        number = c+1;
      }
    }

    //we limit number to 512?
    if (strlen(++number)>512) 
    {
      number[512]='\0';
    }

    //if what we read contains Data or Track
    if ((j > 0) && (strstr(number, "Data")!=NULL) && 
        (strstr(number, "Track")!=NULL)) 
    {
      split1 = 
        splt_sp_get_splitpoint_value(state, j, &get_error);
      if (get_error != SPLT_OK)
      {
        *error = get_error;
        goto function_end;
      }

      change_error = 
        splt_sp_set_splitpoint_value(state,j-1, split1);

      if (change_error != SPLT_OK)
      {
        *error = change_error;
        goto function_end;
      }
      tracks -= 1;
    }
    else
    {
      /*fprintf(stdout,"temp = _%s_\n", temp);
      fprintf(stdout,"previous = _%s_\n", prev);
      fflush(stdout);*/

      //otherwise, we put the title
      if ((j > 0) && (strcmp(temp, prev)==0))
      {
        tags_error = splt_tu_set_tags_field(state, j-1, SPLT_TAGS_TITLE,
            number);
        if (tags_error != SPLT_OK)
        {
          *error =tags_error;
          goto function_end;
        }
      }
      else
      {
        //we treat DTITLE
        if (j == 0)
        {
          //we put the artist before the slash
          i=0;
          while ((number[i]!='/') && (number[i]!='\0')&&(i<127)) 
          {
            i++;
          }

          char ttemp[i+1];
          int i_temp = 0;
          for (i_temp = 0; i_temp < i;i_temp++)
          {
            ttemp[i_temp] = number[i_temp];
          }
          ttemp[i] = '\0';
          tags_error = splt_tu_set_tags_field(state, 0, SPLT_TAGS_ARTIST,
              ttemp);
          if (tags_error != SPLT_OK)
          {
            *error =tags_error;
            goto function_end;
          }

          //we cut the space at the end of the artist
          artist = (char *)splt_tu_get_tags_field(state,0, SPLT_TAGS_ARTIST);
          int k = strlen(artist)-1;
          while (artist[k] == ' ')
          {
            artist[k] = '\0';
            k--;
            if (k < 0)
            {
              break;
            }
          }

          splt_c_put_info_message_to_client(state, _("\n  Artist: %s\n"), artist);

          //we put the album after the slash
          i += 1;
          number = splt_su_cut_spaces(number+i);

          tags_error = splt_tu_set_tags_field(state, 0, SPLT_TAGS_ALBUM, 
              number);
          if (tags_error != SPLT_OK)
          {
            *error =tags_error;
            goto function_end;
          }

          album = (char *)splt_tu_get_tags_field(state,0, SPLT_TAGS_ALBUM);

          splt_c_put_info_message_to_client(state, _("  Album: %s\n"), album);
        }
        else
        {
          char *t = number;
          //if we don't have = before, it means
          //we have a performer with a  perf/title
          if (*(number-1) != '=')
          {
            t = number-1;
          }
          t = splt_su_cut_spaces(t);
          tags_error = splt_tu_set_tags_field(state, j-1,
              SPLT_TAGS_TITLE, t);
          if (tags_error != SPLT_OK)
          {
            *error =tags_error;
            goto function_end;
          }
        }
        j++;
      }
    }
    strncpy(prev, temp, 10);

    free(line);
    line = NULL;
  } while (j<=tracks);

  //we search for
  //YEAR (the year) and ID3 genre
  unsigned char default_genre = 12;
  tags_error = splt_tu_set_tags_field(state, 0, SPLT_TAGS_GENRE, &default_genre);
  if (tags_error != SPLT_OK)
  {
    *error =tags_error;
    goto function_end;
  }

  while ((line = splt_io_readline(file_input, error))!=NULL)
  {
    if (*error < 0 || line == NULL)
    {
      goto function_end;
    }

    line[strlen(line)-1] = '\0';
    if (strlen(line)>0)
    {
      if (line[strlen(line)-1]=='\r') 
      {
        line[strlen(line)-1]='\0';
      }
    }
    if (strstr(line, "EXTD")==NULL) 
    {
      free(line);
      line = NULL;
      continue;
    }
    else 
    {
      if ((number=strchr(line, '='))==NULL) 
      {
        free(line);
        line = NULL;
        break;
      }
      else 
      {
        if ((c=strstr(number, "YEAR"))!=NULL)
        {
          tags_error = splt_tu_set_tags_field(state, 0, SPLT_TAGS_YEAR, c+6);
          if (tags_error != SPLT_OK)
          {
            *error = tags_error;
            goto function_end;
          }
        }
        if ((c=strstr(number, "ID3G"))!=NULL) 
        {
          strncpy(line, c+6, 3);
          int id3g = atoi(line);
          tags_error = splt_tu_set_tags_field(state, 0, SPLT_TAGS_GENRE, &id3g);
          if (tags_error != SPLT_OK)
          {
            *error = tags_error;
            goto function_end;
          }
        }
        break;
      }
    }

    if (line)
    {
      free(line);
      line = NULL;
    }
  }

  splt_tag_put_filenames_from_tags(state,tracks,error);

function_end:
  if (line)
  {
    free(line);
    line = NULL;
  }
  if (fclose(file_input) != 0)
  {
    splt_e_set_strerror_msg_with_data(state, file);
    *error = SPLT_ERROR_CANNOT_CLOSE_FILE;
  }
  file_input = NULL;

  if (*error >= 0)
  {
    splt_c_put_info_message_to_client(state, _("  Tracks: %d\n\n"),tracks);
  }

  return tracks;
}

static void splt_cddb_process_line(char **l, cddb_utils *cdu, splt_state *state)
{
  if (!l || !*l) { return; }

  char *line = *l;

  splt_su_line_to_unix(line);
  splt_su_str_cut_last_char(line);

  char *line_content = NULL;
  if ((line_content = strstr(line, "Track frame offset")) != NULL)
  {
    cdu->read_offsets = SPLT_TRUE;
  }
  else if ((line_content = strstr(line, "Disc length")) != NULL)
  {
    splt_cddb_process_disc_length_line(line_content, cdu, state);
  }
  else if (cdu->read_offsets)
  {
    splt_cddb_process_offset_line(line_content, cdu, state);
  }
  else if ((line_content = strstr(line, "YEAR")) != NULL)
  {
    splt_cddb_process_year_line(line_content, cdu, state);
  }
  else if ((line_content = strstr(line, "GENRE")) != NULL)
  {
    splt_cddb_process_genre_line(line_content, cdu, state);
  }
  else if ((line_content = strstr(line, "DTITLE")) != NULL)
  {
    splt_cddb_process_dtitle_line(line_content, cdu, state);
  }
  else if ((line_content = strstr(line, "TTITLE")) != NULL)
  {
    splt_cddb_process_ttitle_line(line_content, cdu, state);
  }
  else if ((line_content = strstr(line, "ID3G")) != NULL)
  {
    splt_cddb_process_id3g_line(line_content, cdu, state);
  }

  free(*l);
  *l = NULL;
}

static void splt_cddb_process_id3g_line(const char *line_content, cddb_utils *cdu, splt_state *state)
{
  int err = SPLT_OK;

  int id3g = atoi(line_content+6);
  err = splt_tu_set_tags_field(state, 0, SPLT_TAGS_GENRE, &id3g);
  if (err < 0) { cdu->error = err; return; }
}

static void splt_cddb_process_ttitle_line(const char *line_content, cddb_utils *cdu, splt_state *state)
{
  int err = SPLT_OK;

  char *equal_ptr = NULL;
  if ((equal_ptr = strchr(line_content, '=')) == NULL) 
  {
    splt_e_set_error_data(state, cdu->file);
    cdu->error = SPLT_INVALID_CDDB_FILE;
    return;
  }

  char *slash = strchr(equal_ptr, '/');
  if (slash != NULL)
  {
    splt_su_cut_spaces_from_end(slash);
    char *performer = slash + 1;
    err = splt_tu_set_tags_field(state, 0, SPLT_TAGS_PERFORMER, performer);
    if (err < 0) { cdu->error = err; return; }
    slash = '\0';
  }

  char *title = equal_ptr + 1;

  splt_su_cut_spaces(title);
  err = splt_tu_set_tags_field(state, 0, SPLT_TAGS_TITLE, title);
  if (err < 0) { cdu->error = err; return; }
}

static void splt_cddb_process_dtitle_line(const char *line_content, cddb_utils *cdu, splt_state *state)
{
  int err = SPLT_OK;

  char *equal_ptr = NULL;
  if ((equal_ptr = strchr(line_content, '=')) == NULL) 
  {
    splt_e_set_error_data(state, cdu->file);
    cdu->error = SPLT_INVALID_CDDB_FILE;
    return;
  }

  char *slash = strchr(equal_ptr, '/');
  if (slash != NULL)
  {
    splt_su_cut_spaces_from_end(slash);
    char *artist = slash + 1;
    err = splt_tu_set_tags_field(state, 0, SPLT_TAGS_ARTIST, artist);
    if (err < 0) { cdu->error = err; return; }
    splt_c_put_info_message_to_client(state, _("\n  Artist: %s\n"), artist);
    slash = '\0';
  }

  char *album = equal_ptr + 1;

  splt_su_cut_spaces(album);
  err = splt_tu_set_tags_field(state, 0, SPLT_TAGS_ALBUM, album);
  if (err < 0) { cdu->error = err; return; }
  splt_c_put_info_message_to_client(state, _("  Album: %s\n"), album);
}

/*  //todo: used for the title
  //int is_ttitle = SPLT_FALSE;
  char *slash = NULL;
  if ((slash = strchr(equal_ptr, '/')) != NULL)
  {
    if (is_ttitle)
    {
      char *performer = strchr(equal_ptr + 1, '/');
      *performer = '\0';
      splt_su_cut_spaces_from_end(performer - 1);

      err = splt_tu_set_tags_field(state, cdu->field_counter - 1,
          SPLT_TAGS_PERFORMER, equal_ptr + 1);
      if (err < 0) { cdu->error = err; return; }
    }
    else
    {
      err = splt_tu_set_tags_field(state, cdu->field_counter - 1, 
          SPLT_TAGS_PERFORMER, NULL);
      if (err < 0) { cdu->error = err; return; }
    }
  }*/

static void splt_cddb_process_genre_line(const char *line_content,
    cddb_utils *cdu, splt_state *state)
{
  int err = SPLT_OK;

  char a[4] = { '\0' };
  strncpy(a, line_content + 6, 3);
  //this tag doesn't work correctly because GENRE is not a number
  int t = atoi(a);

  err = splt_tu_set_tags_field(state, 0, SPLT_TAGS_GENRE, &t);
  if (err < 0) { cdu->error = err; }
}

static void splt_cddb_process_year_line(const char *line_content,
    cddb_utils *cdu, splt_state *state)
{
  int err = SPLT_OK;
  err = splt_tu_set_tags_field(state, 0, SPLT_TAGS_YEAR, line_content+6);
  if (err < 0) { cdu->error = err; }
}

static void splt_cddb_process_disc_length_line(const char *line_content,
    cddb_utils *cdu, splt_state *state)
{
  int err = SPLT_OK;

  cdu->read_offsets = SPLT_FALSE;

  splt_t_set_splitnumber(state, cdu->tracks);
  splt_t_clean_split_data(state, cdu->tracks);

  double value = splt_su_str_line_to_double(line_content);
  err = splt_sp_append_splitpoint(state, value * 100, NULL, SPLT_SPLITPOINT);

  if (err < 0) { cdu->error = err; return; }

  splt_cddb_convert_points(cdu, state);
}

static void splt_cddb_convert_points(cddb_utils *cdu, splt_state *state)
{
  int err = SPLT_OK;

  long first_point = splt_sp_get_splitpoint_value(state, 0, &err);
  if (err < 0) { cdu->error = err; return; }

  int i = 0;
  for (i = cdu->tracks - 1; i >= 0; i--)
  {
    long point = splt_sp_get_splitpoint_value(state, i, &err);
    if (err < 0) { cdu->error = err; return; }

    //cddb specs
    long difference = point - first_point;
    float value = (float) difference / 75.f;
    err = splt_sp_set_splitpoint_value(state, i, (long) ceilf(value));
    if (err < 0) { cdu->error = err; return; }
  }
}

static void splt_cddb_process_offset_line(const char *line_content, 
    cddb_utils *cdu, splt_state *state)
{
  int err = SPLT_OK;

  double value = splt_su_str_line_to_double(line_content);
  err = splt_sp_append_splitpoint(state, value * 100, NULL, SPLT_SPLITPOINT);

  if (err < 0) { cdu->error = err; return; }

  cdu->tracks++;
}

static cddb_utils *splt_cddb_cdu_new(splt_state *state, int *error)
{
  cddb_utils *cdu = malloc(sizeof(cddb_utils));
  if (cdu == NULL)
  {
    *error = SPLT_ERROR_CANNOT_ALLOCATE_MEMORY;
    return NULL;
  }

  cdu->read_offsets = SPLT_FALSE;
  cdu->error = SPLT_OK;
  cdu->tracks = 0;
  cdu->file = NULL;
  cdu->field_counter = 0;

  return cdu;
}

static void splt_cddb_cdu_free(cddb_utils **cdu)
{
  if (!cdu || !*cdu)
  {
    return;
  }

  free(*cdu);
  *cdu = NULL;
}
