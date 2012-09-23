/*
 * Mp3Splt -- Utility for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2012 Alexandru Munteanu - <io_fx@yahoo.fr>
 *
 * http://mp3splt.sourceforge.net
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111, USA.
 */

#include "common.h"
#include "print_utils.h"

#include "freedb.h"

extern FILE *console_out;

void do_freedb_search(main_data *data)
{
  int err = SPLT_OK;
  options *opt = data->opt;
  splt_state *state = data->state;

  //we find out what search and get type we have
  char search_type[30] = "";
  char get_type[30] = "";
  if (opt->freedb_search_type == SPLT_FREEDB_SEARCH_TYPE_CDDB_CGI)
  {
    snprintf(search_type,30,"%s","cddb_cgi");
  }
  else
  {
    snprintf(search_type,30,"%s","web_search");
  }
  if (opt->freedb_get_type == SPLT_FREEDB_GET_FILE_TYPE_CDDB_CGI)
  {
    snprintf(get_type,30,"%s","cddb_cgi");
  }
  else
  {
    snprintf(get_type,30,"%s","cddb_protocol");
  }

  //print out infos about the servers
  fprintf(console_out,_(" Freedb search type: %s , Site: %s , Port: %d\n"),
      search_type,opt->freedb_search_server,opt->freedb_search_port);
  fflush(console_out);
  fprintf(console_out,_(" Freedb get type: %s , Site: %s , Port: %d\n"),
      get_type,opt->freedb_get_server,opt->freedb_get_port);
  fflush(console_out);

  char *freedb_search_string = NULL;
  char freedb_input[2048] = { '\0' };
  //if we haven't chosen to search from the arguments, interactive search
  if (opt->freedb_arg_search_string[0] == '\0')
  {
    //freedb search
    print_message(_("CDDB QUERY. Insert album and"
          " artist informations to find cd."));

    short first_time = SPLT_TRUE;
    //here we search freedb
    do {
      if (!first_time)
      {
        print_message(_("\nPlease search something ..."));
      }

      memset(freedb_input, '\0', sizeof(freedb_input));

      fprintf(console_out, "\n\t____________________________________________________________]");
      fprintf(console_out, _("\r Search: ["));

      fgets(freedb_input, 2046, stdin);

      first_time = SPLT_FALSE;

      freedb_input[strlen(freedb_input)-1] = '\0';

    } while (strlen(freedb_input)==0);

    freedb_search_string = freedb_input;
  }
  else
  {
    freedb_search_string = opt->freedb_arg_search_string;
  }

  fprintf(console_out, _("\n  Search string: %s\n"),freedb_search_string);
  fprintf(console_out, _("\nSearching from %s on port %d using %s ...\n"),
      opt->freedb_search_server,opt->freedb_search_port, search_type);
  fflush(console_out);

  const splt_freedb_results *f_results =
    mp3splt_get_freedb_search(state, freedb_search_string,
        &err, opt->freedb_search_type,
        opt->freedb_search_server,
        opt->freedb_search_port);

  process_confirmation_error(err, data);
  if (!f_results)
  {
    print_message_exit(_("No results found"), data);
  }

  int number_of_results = mp3splt_freedb_get_total_number(f_results);

  //if we don't have an auto-select the result X from the arguments:
  // (query{artist}(resultX)
  //, then interactive user ask
  int selected_cd = 0;
  if (opt->freedb_arg_result_option < 0)
  {
    //print the searched informations
    print_message(_("List of found cd:"));

    int cd_number = 0;
    short end = SPLT_FALSE;
    while (cd_number < number_of_results) {
      int cd_id = mp3splt_freedb_get_id(f_results, cd_number);

      char *cd_name = mp3splt_freedb_get_name(f_results, cd_number);
      fprintf(console_out,"%3d) %s\n", cd_id, cd_name);
      free(cd_name);

      int i = 0;
      int number_of_revisions = mp3splt_freedb_get_number_of_revisions(f_results, cd_number);
      for(i = 0; i < number_of_revisions; i++)
      {
        fprintf(console_out, "  |\\=>");
        fprintf(console_out, "%3d) ", cd_id+i+1);
        fprintf(console_out, _("Revision: %d\n"), i+2);

        //break at 22
        if (((cd_id+i+2) % 22) == 0)
        {
          //duplicate, see below
          char junk[18];
          fprintf(console_out, _("-- 'q' to select cd, Enter for more:"));
          fflush(console_out);

          fgets(junk, 16, stdin);
          if (junk[0]=='q')
          {
            end = SPLT_TRUE;
            goto end;
          }
        }
      }

      //we read result from the char, q tu select cd or
      //enter to show more results
      if (((cd_id + 1) % 22) == 0)
      {
        //duplicate, see ^^
        char junk[18];
        fprintf(console_out, _("-- 'q' to select cd, Enter for more: "));
        fflush(console_out);

        fgets(junk, 16, stdin);
        if (junk[0]=='q')
        {
          end = SPLT_TRUE;
          goto end;
        }
      }

end:
      if (end)
      {
        end = SPLT_FALSE;
        break;
      }

      cd_number++;
    };

    //select the CD
    //input of the selected cd
    char sel_cd_input[1024];
    int tot = 0;
    do {
      selected_cd = 0;
      fprintf(console_out, _("Select cd #: "));
      fflush(console_out);
      fgets(sel_cd_input, 254, stdin);
      sel_cd_input[strlen(sel_cd_input)-1]='\0';
      tot = 0;

      if (sel_cd_input[tot] == '\0') 
      {
        selected_cd = -1;
      }

      while(sel_cd_input[tot] != '\0')
      {
        if (isdigit(sel_cd_input[tot++])==0)
        {
          fprintf(console_out, _("Please "));
          fflush(console_out);

          selected_cd = -1;
          break;
        }
      }

      if (selected_cd != -1) 
      {
        selected_cd = atoi(sel_cd_input);
      }

    } while ((selected_cd >= number_of_results)
        || (selected_cd < 0));
  }
  else
  {
    selected_cd = opt->freedb_arg_result_option;
    if (selected_cd >= number_of_results)
    {
      selected_cd = 0;
    }
  }

  fprintf(console_out, _("\nGetting file from %s on port %d using %s ...\n"),
      opt->freedb_get_server,opt->freedb_get_port, get_type);
  fflush(console_out);

  err = mp3splt_write_freedb_file_result(state, selected_cd,
      MP3SPLT_CDDBFILE, opt->freedb_get_type,
      opt->freedb_get_server, opt->freedb_get_port);
  process_confirmation_error(err, data);
}


