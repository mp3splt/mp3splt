/*
 * Mp3Splt -- Utility for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2014 Alexandru Munteanu - <m@ioalex.net>
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
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "common.h"
#include "options_parsers.h"
#include "print_utils.h"
#include "utils.h"

int parse_silence_options(char *arg, float *th, int *gap,
    int *nt, float *off, int *rm, float *min, float *min_track_length, int *shots,
    float *min_track_join, float *keep_silence_left, float *keep_silence_right,
    int *warn_if_no_auto_adjust, int *err_if_no_auto_adjust)
{
  char *ptr = NULL;
  int found = 0;

  if ((gap!=NULL) && ((ptr=strstr(arg, "gap"))!=NULL))
  {
    if ((ptr=strchr(ptr, '='))!=NULL)
    {
      if (sscanf(ptr+1, "%d", gap)==1)
      {
        found++;
      }
      else 
      {
        print_warning(_("bad gap argument. It will be ignored !"));
      }
    }
  }

  if ((shots != NULL) && ((ptr = strstr(arg, "shots"))!=NULL))
  {
    if ((ptr=strchr(ptr, '='))!=NULL)
    {
      if (sscanf(ptr+1, "%d", shots)==1)
      {
        found++;
      }
      else 
      {
        print_warning(_("bad shots argument. It will be ignored !"));
      }
    }
  }

  if ((min_track_join != NULL) && ((ptr = strstr(arg, "trackjoin"))!=NULL))
  {
    if ((ptr=strchr(ptr, '='))!=NULL)
    {
      if (sscanf(ptr+1, "%f", min_track_join)==1)
      {
        found++;
      }
      else 
      {
        print_warning(_("bad trackjoin argument. It will be ignored !"));
      }
    }
  }

  if ((th!=NULL) && ((ptr=strstr(arg, "th"))!=NULL))
  {
    if ((ptr=strchr(ptr, '='))!=NULL)
    {
      if (sscanf(ptr+1, "%f", th)==1)
      {
        found++;
      }
      else 
      {
        print_warning(_("bad threshold argument. It will be ignored !"));
      }
    }
  }

  if ((nt!=NULL) && ((ptr=strstr(arg, "nt"))!=NULL))
  {
    if ((ptr=strchr(ptr, '='))!=NULL)
    {
      if (sscanf(ptr+1, "%d", nt)==1)
      {
        found++;
      }
      else 
      {
        print_warning(_("bad tracknumber argument. It will be ignored !"));
      }
    }
  }

  if (rm != NULL)
  {
    if ((ptr = strstr(arg, "rm=")) != NULL)
    {
      if (sscanf(ptr+3, "%f_%f", keep_silence_left, keep_silence_right) != 2)
      {
        *keep_silence_left = -200;
        *keep_silence_right = -200;
        print_warning(_("Bad values for the rm argument. rm parameter will be ignored!"));
      }
      found++;
      *rm = 1;
    }
    else if ((ptr = strstr(arg, "rm")) != NULL)
    {
      found++;
      *rm = 1;
    }
  }

  if ((off!=NULL) && ((ptr=strstr(arg, "off"))!=NULL))
  {
    if ((ptr=strchr(ptr, '='))!=NULL)
    {
      if (sscanf(ptr+1, "%f", off)==1)
      {
        found++;
      }
      else 
      {
        print_warning(_("bad offset argument. It will be ignored!"));
      }
    }
  }

  if ((min!=NULL) && ((ptr=strstr(arg, "trackmin"))!=NULL))
  {
    if ((ptr=strchr(ptr, '='))!=NULL)
    {
      if (sscanf(ptr+1, "%f", min_track_length)==1)
      {
        found++;
      }
      else 
      {
        print_warning(_("bad minimum track length argument. It will be ignored !"));
      }
    }
  }

  if ((min!=NULL) && ((ptr=strstr(arg, "min"))!=NULL))
  {
    if (ptr > arg && *(ptr-1) == 'k') {
      return found;
    }

    if ((ptr=strchr(ptr, '='))!=NULL)
    {
      if (sscanf(ptr+1, "%f", min)==1)
      {
        found++;
      }
      else 
      {
        print_warning(_("bad minimum silence length argument. It will be ignored !"));
      }
    }
  }

  if (warn_if_no_auto_adjust != NULL)
  {
    if ((ptr = strstr(arg, "warn_if_no_aa")) != NULL)
    {
      found++;
      *warn_if_no_auto_adjust = 1;
    }
  }

  if (err_if_no_auto_adjust != NULL)
  {
    if ((ptr = strstr(arg, "error_if_no_aa")) != NULL)
    {
      found++;
      *err_if_no_auto_adjust = 1;
    }
  }

  return found;
}

regex_options *parse_tags_from_fname_regex_options(const char *parameters, int *error)
{
  regex_options *regex_options = new_regex_options();

  char *ptr = NULL;

  if ((ptr=strstr(parameters, "regex=")) != NULL)
  {
    char *regex = ptr+6;
    regex_options->regex = my_malloc(sizeof(char) * (strlen(regex)+1));
    strncpy(regex_options->regex, regex, strlen(regex));
    regex_options->regex[strlen(regex)] = '\0';
  }
  else
  {
    print_warning(_("no regular expression found as argument."));
  }

  return regex_options;
}

//for the moment 2 ways of getting the file and one way to search it
//freedb get type can be: cddb_cgi or cddb_protocol
//freedb search type can be: cddb_cgi or web_search
//query[get=cddb_cgi://freedb2.org/~cddb/cddb.cgi:80,search=cddb_cgi://freedb2.org/~cddb/cddb.cgi:80]
//query[get=cddb_protocol://freedb.org:8880,search=cddb_cgi://freedb2.org/~cddb/cddb.cgi:80]
//query[get=cddb_cgi://freedb.org/~cddb/cddb.cgi:80,search=cddb_cgi://freedb2.org/~cddb/cddb.cgi:80]
//query[get...]{search_string}
//query[get...]{search_string}(chosen_result_int)
//we parse the query arguments
int parse_query_arg(options *opt, const char *query)
{
  const char *cur_pos = NULL;
  const char *end_pos = NULL;
  const char *test_pos = NULL;
  cur_pos = query + 5;

  short ambigous = SPLT_FALSE;

  //if we have [get=...]
  if (cur_pos[0] == '[')
  {
    cur_pos = strchr(query,'[');

    //if we don't have ], ambigous
    if (!(test_pos = strchr(cur_pos,']')))
    {
      ambigous = SPLT_TRUE;
    }

    int search_found = SPLT_FALSE;
    int get_found = SPLT_FALSE;
    int paranthesis_has_content = SPLT_FALSE;
    //we find "get" or "search"
    while((test_pos = strstr(cur_pos,"get="))||
        (test_pos = strstr(cur_pos,"search=")))
    {
      paranthesis_has_content = SPLT_TRUE;

      //we find out which one is first
      end_pos = strstr(cur_pos,"get=");
      test_pos = strstr(cur_pos,"search=");
      if (end_pos == NULL)
      {
        cur_pos = test_pos;
      }
      else
      {
        if (test_pos == NULL)
        {
          cur_pos = end_pos;
        }
        else
        {
          if (end_pos < test_pos)
          {
            cur_pos = end_pos;
          }
          else
          {
            cur_pos = test_pos;
          }
        }
      }

      //we determine the type (get or search)
      if (strstr(cur_pos,"get=") == cur_pos)
      {
        get_found = SPLT_TRUE;
        search_found = SPLT_FALSE;
        cur_pos += 4;
      }
      else
      {
        search_found = SPLT_TRUE;
        get_found = SPLT_FALSE;
        cur_pos += 7;
      }

      //we get out the type of the search
      char freedb_type[256] = "\0";
      if ((end_pos=strstr(cur_pos,"://"))||
          (end_pos=strchr(cur_pos,',')) ||
          (end_pos=strchr(cur_pos,']')))
      {
        if (end_pos-cur_pos < 255)
        {
          snprintf(freedb_type,end_pos-cur_pos+1,"%s",cur_pos);
          freedb_type[end_pos-cur_pos] = '\0';
          if ((strchr(cur_pos,',')==end_pos) ||
              (strchr(cur_pos,']')==end_pos))
          {
            cur_pos = end_pos;
          }
          else
          {
            cur_pos = end_pos+3;
          }
          end_pos = cur_pos;
        }
      }
      else
      {
        end_pos = cur_pos;
      }

      //we get out the server
      char freedb_server[256] = "\0";
      while ((*end_pos != ':') && (*end_pos != ',') &&
          (*end_pos != ']') && (*end_pos != '\0'))
      {
        end_pos++;
      }
      if ((end_pos != cur_pos) && (end_pos-cur_pos < 255)
          && (*end_pos != '\0'))
      {
        snprintf(freedb_server,end_pos-cur_pos+1,"%s",cur_pos);
        freedb_server[end_pos-cur_pos] = '\0';
        if (*(end_pos+1) == ']' || *(end_pos+1) == ',')
        {
          cur_pos = end_pos+1;
        }
        else
        {
          cur_pos = end_pos;
        }
        end_pos = cur_pos;
      }
      else
      {
        cur_pos = end_pos;
      }

      //we get out the port
      char freedb_port[10] = "\0";
      int is_only_digits = SPLT_TRUE;
      int freedb_int_port = -1;
      //if we have the port
      if (*end_pos == ':')
      {
        cur_pos++;
        end_pos++;
        while((*end_pos != ']') &&
            (*end_pos != '\0') &&
            (*end_pos != ','))
        {
          //we have to have only digits for the port
          if (!isdigit(*end_pos))
          {
            ambigous = SPLT_TRUE;
            is_only_digits = SPLT_FALSE;
          }
          end_pos++;
        }
        if ((end_pos != cur_pos) && (end_pos-cur_pos < 10))
        {
          snprintf(freedb_port,end_pos-cur_pos+1,"%s",cur_pos);
          freedb_port[end_pos-cur_pos] = '\0';
          cur_pos = end_pos;
        }

        //we get the port as integer
        if (*freedb_port != '\0')
        {
          if (is_only_digits)
          {
            freedb_int_port = atoi(freedb_port);
          }
          else
          {
            freedb_int_port = SPLT_FREEDB_CDDB_CGI_PORT;
            print_warning(_("found non digits characters in port !"
                  " (switched to default)"));
          }
        }
      }

      //we put the search type as integer
      int freedb_int_type = -1;
      if (*freedb_type != '\0')
      {
        //get type
        if (get_found)
        {
          if (strcmp(freedb_type,"cddb_protocol") == 0)
          {
            freedb_int_type = SPLT_FREEDB_GET_FILE_TYPE_CDDB;
          }
          else
          {
            if (strcmp(freedb_type,"cddb_cgi") == 0)
            {
              freedb_int_type = SPLT_FREEDB_GET_FILE_TYPE_CDDB_CGI;
            }
            else
            {
              print_warning(_("unknown search type !"
                    " (switched to default)"));
            }
          }
        }
        else
        {
          //search type
          if (strcmp(freedb_type,"cddb_cgi") == 0)
          {
            freedb_int_type = SPLT_FREEDB_SEARCH_TYPE_CDDB_CGI;
          }
          else
          {
            if (strcmp(freedb_type,"web_search") == 0)
            {
              print_warning(_("freedb web search not implemented yet !"
                    " (switched to default)"));
              freedb_int_type = SPLT_FREEDB_SEARCH_TYPE_CDDB_CGI;
            }
            else
            {
              print_warning(_("unknown get type ! (switched to default)"));
            }
          }
        }
      }
      else
      {
        ambigous = SPLT_TRUE;
      }

      //if we have found search
      if (search_found)
      {
        if (freedb_int_type != -1)
        {
          opt->freedb_search_type = freedb_int_type;
        }
        if (*freedb_server != '\0')
        {
          snprintf(opt->freedb_search_server,255, "%s",freedb_server);
        }
        else
        {
          snprintf(opt->freedb_search_server,255, "%s",SPLT_FREEDB2_CGI_SITE);
        }
        if (freedb_int_port != -1)
        {
          opt->freedb_search_port = freedb_int_port;
        }
      }
      else
      {
        //if we have found get
        if (get_found)
        {
          if (freedb_int_type != -1)
          {
            opt->freedb_get_type = freedb_int_type;
          }
          if (*freedb_server != '\0')
          {
            snprintf(opt->freedb_get_server,255, "%s", freedb_server);
          }
          else
          {
            snprintf(opt->freedb_get_server,255, "%s", SPLT_FREEDB2_CGI_SITE);
          }
          if (freedb_int_port != -1)
          {
            opt->freedb_get_port = freedb_int_port;
          }
        }
      }

      //if at the and something else than , or ], ambigous
      if ((*cur_pos != ',') &&
          (*cur_pos != ']') &&
          (*cur_pos != '\0'))
      {
        ambigous = SPLT_TRUE;
      }
    }
    //if we don't have anything inside the paranthesis,
    //ambigous
    if (!paranthesis_has_content)
    {
      ambigous = SPLT_TRUE;
    }
  }

  //possible search string
  int we_have_search_string = SPLT_FALSE;
  if (cur_pos != NULL)
  {
    if ((cur_pos = strchr(cur_pos, '{')))
    {
      //if we don't have }, ambigous
      if (!(end_pos = strchr(cur_pos,'}')))
      {
        ambigous = SPLT_TRUE;
      }
      else
      {
        if (end_pos-cur_pos < 2048)
        {
          snprintf(opt->freedb_arg_search_string, end_pos-cur_pos, "%s", cur_pos+1);
          opt->freedb_arg_search_string[end_pos-cur_pos-1] = '\0';
          we_have_search_string = SPLT_TRUE;
        }
      }
    }

    if (ambigous)
    {
      return ambigous;
    }
  }

  //possible get result integer from the search results
  if (cur_pos != NULL)
  {
    if ((cur_pos = strchr(cur_pos, '(')))
    {
      if (!we_have_search_string)
      {
        return ambigous;
      }
  
      //if we don't have ), ambigous
      if (!(end_pos = strchr(cur_pos,')')))
      {
        ambigous = SPLT_TRUE;
      }
      else
      {
        char chosen_int[256] = { '\0' };
        if (end_pos-cur_pos < 256)
        {
          snprintf(chosen_int, end_pos-cur_pos, "%s", cur_pos+1);
          opt->freedb_arg_result_option = atoi(chosen_int);
        }
      }
    }
  }

  return ambigous;
}



