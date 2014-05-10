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

#include <unistd.h>

#ifdef __WIN32__
#include <conio.h>
#endif

#include "common.h"
#include "utils.h"
#include "print_utils.h"

#include "freedb.h"

static void handle_proxy(main_data *data);
static void query_for_proxy_and_write_configuration_file(main_data *data);
static void read_proxy_settings_from_configuration_file(main_data *data);
static char *get_configuration_filename();
static char *query_for_proxy_login();
static char *get_input_line(FILE *input_file, char *key);

extern FILE *console_out;

#if defined(__BEOS__) && !defined (HAS_GETPASS)
#warning Faking getpass() !!!
char *getpass(char *p)
{
  char *ret = malloc(30);
  if (!ret)
    return NULL;
  puts(p);
  fgets(ret, 30, stdin);
  return ret;
}
#endif

#ifdef __WIN32__
char *getpass(char *s)
{
  char *pass, c;
  int i=0;
  fputs(s, stdout);
  pass = malloc(100);
  do {
    c = _getch();
    if (c!='\r') {
      if (c=='\b') {
        if (i>0) {
          printf ("\b \b");
          i--;
        }
      }
      else {
        printf ("*");
        pass[i++] = c;
      }
    }
    else break;
  } while (i<100);

  pass[i]='\0';

  printf("\n");

  return pass;
}
#endif

void do_freedb_search(main_data *data)
{
  handle_proxy(data);

  int err = SPLT_OK;
  options *opt = data->opt;
  splt_state *state = data->state;

  char search_type[30] = "";
  char get_type[30] = "";
  if (opt->freedb_search_type == SPLT_FREEDB_SEARCH_TYPE_CDDB_CGI)
  {
    snprintf(search_type, 30, "%s", "cddb_cgi");
  }
  else
  {
    snprintf(search_type, 30 ,"%s", "web_search");
  }
  if (opt->freedb_get_type == SPLT_FREEDB_GET_FILE_TYPE_CDDB_CGI)
  {
    snprintf(get_type, 30, "%s", "cddb_cgi");
  }
  else
  {
    snprintf(get_type, 30, "%s", "cddb_protocol");
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
    print_message(_("CDDB QUERY. Insert album and artist informations to find cd."));

    short first_time = SPLT_TRUE;
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

  splt_freedb_results *f_results =
    mp3splt_get_freedb_search(state, freedb_search_string,
        &err, opt->freedb_search_type,
        opt->freedb_search_server,
        opt->freedb_search_port);

  process_confirmation_error(err, data);
  if (!f_results)
  {
    print_message_exit(_("No results found"), data);
  }

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

    mp3splt_freedb_init_iterator(f_results);
    const splt_freedb_one_result *f_result = NULL;
    while ((f_result = mp3splt_freedb_next(f_results))) {
      int cd_id = mp3splt_freedb_get_id(f_result);

      const char *cd_name = mp3splt_freedb_get_name(f_result);
      fprintf(console_out,"%3d) %s\n", cd_id, cd_name);

      int i = 0;
      int number_of_revisions = mp3splt_freedb_get_number_of_revisions(f_result);
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

    } while ((selected_cd < 0) || (selected_cd >= cd_number));
  }
  else
  {
    selected_cd = opt->freedb_arg_result_option;
  }

  fprintf(console_out, _("\nGetting file from %s on port %d using %s ...\n"),
      opt->freedb_get_server,opt->freedb_get_port, get_type);
  fflush(console_out);

  err = mp3splt_write_freedb_file_result(state, selected_cd,
      MP3SPLT_CDDBFILE, opt->freedb_get_type,
      opt->freedb_get_server, opt->freedb_get_port);
  process_confirmation_error(err, data);

  mp3splt_clear_proxy(data->state);
}

static void handle_proxy(main_data *data)
{
  mp3splt_clear_proxy(data->state);
  query_for_proxy_and_write_configuration_file(data);
  read_proxy_settings_from_configuration_file(data);
}

static void read_proxy_settings_from_configuration_file(main_data *data)
{
  char *config_file = get_configuration_filename();

  FILE *input_file = fopen(config_file, "r");
  if (!input_file)
  {
    free(config_file);
    return;
  }
  free(config_file);

  fseek(input_file, 0, SEEK_SET);

  char *proxy_address = get_input_line(input_file, "PROXYADDR");
  if (!proxy_address) { goto end; }

  char *proxy_port = get_input_line(input_file, "PROXYPORT");
  if (!proxy_port) { free(proxy_address); goto end; }
  int port = atoi(proxy_port);

  fprintf(stderr, " Using proxy %s on port %d\n", proxy_address, port);

  mp3splt_use_proxy(data->state, proxy_address, port);

  if (proxy_address != NULL)
  {
    free(proxy_address);
  }
  if (proxy_port != NULL)
  {
    free(proxy_port);
  }

  char *use_proxy_auth = get_input_line(input_file, "PROXYAUTH");
  if (!use_proxy_auth) { goto end; }
  if (use_proxy_auth[0] != '1')
  {
    free(use_proxy_auth);
    goto end;
  }
  free(use_proxy_auth);

  char *authentification = get_input_line(input_file, NULL);
  if (authentification)
  {
    mp3splt_use_base64_authentification(data->state, authentification);
    free(authentification);
  }
  else
  {
    splt_code error = SPLT_OK;

    char *proxy_authentification = query_for_proxy_login();
    char *authentification_as_base64 = 
      mp3splt_encode_in_base64(data->state, proxy_authentification, &error);
    process_confirmation_error(error, data);

    memset(proxy_authentification, 0x00, strlen(proxy_authentification));
    free(proxy_authentification);

    mp3splt_use_base64_authentification(data->state, authentification_as_base64);

    memset(authentification_as_base64, 0x00, strlen(authentification_as_base64));
    free(authentification_as_base64);
  }

end:
  fclose(input_file);
}

static char *get_input_line(FILE *input_file, char *key)
{
  char *line = NULL;
  size_t length = 0;

#ifndef HAVE_GETLINE
  char junk[512];
  fgets(junk, 511, input_file);
  junk[strlen(junk)] = '\0';

  length = strlen(junk) + 1;
  line = my_malloc(sizeof(char) * length);
  snprintf(line, length, junk);
#else
  if (getline(&line, &length, input_file) == -1)
  {
    if (line) { free(line); }
    return NULL;
  }
#endif

  if (!line) { return NULL; }

  int line_length = strlen(line);
  if (line_length < 3)
  {
    free(line);
    return NULL;
  }

  line[line_length-1] = '\0';
  if (line[line_length-2] == '\r')
  {
    line[line_length-2] = '\0';
  }

  if (key == NULL)
  {
    return line;
  }

  if (strstr(line, key) == NULL) {
    free(line);
    return NULL;
  }

  char *value_start = NULL;
  if ((value_start = strchr(line, '=')) == NULL) {
    print_warning(_("the configuration file is malformed !"));
    free(line);
    return NULL;
  }

  char *value = strdup(value_start + 1);
  if (!value)
  {
    print_warning(_("cannot allocate memory !"));
  }

  free(line);

  return value;
}

static void query_for_proxy_and_write_configuration_file(main_data *data)
{
  char *config_file = get_configuration_filename();

  FILE *output_file = NULL;
  if ((output_file = fopen(config_file, "r")))
  {
    free(config_file);
    fclose(output_file);
    return;
  }

  if (data->opt->q_option)
  {
    return;
  }

  if (!(output_file = fopen(config_file, "w+")))
  {
    free(config_file);
    print_warning(_("can't open the configuration file !"));
    return;
  }
  free(config_file);

  int user_input_length = 1024;
  char user_input[1024] = { '\0' };

  fprintf(stderr, _("Will you use a proxy ? (y/n): "));
  fgets(user_input, user_input_length, stdin);
  if (user_input[0] != 'y')
  {
    goto close_file;
  }

  fprintf(stderr, _("Proxy Address: "));
  fgets(user_input, user_input_length, stdin);
  fprintf(output_file, "PROXYADDR=%s", user_input);

  fprintf(stderr, _("Proxy Port: "));
  fgets(user_input, user_input_length, stdin);
  fprintf(output_file, "PROXYPORT=%s", user_input);

  fprintf(stderr, _("Need authentication ? (y/n): "));
  fgets(user_input, user_input_length, stdin);
  if (user_input[0] != 'y')
  {
    goto close_file;
  }

  fprintf(output_file, "PROXYAUTH=1\n");
  fprintf(stderr, _("Would you like to save the password (insecure) ? (y/n): "));
  fgets(user_input, user_input_length, stdin);
  if (user_input[0] != 'y')
  {
    goto close_file;
  }

  char *authentification = query_for_proxy_login();

  splt_code error = SPLT_OK;
  char *authentification_as_base64 = 
    mp3splt_encode_in_base64(data->state, authentification, &error);
  process_confirmation_error(error, data);

  memset(authentification, 0x00, strlen(authentification));
  free(authentification);

  fprintf(output_file, "%s\n", authentification_as_base64);

  memset(authentification_as_base64, 0x00, strlen(authentification_as_base64));
  free(authentification_as_base64);

close_file:
  fclose(output_file);
}

static char *query_for_proxy_login()
{
  char user_input[130];
  fprintf(console_out, _("Username: "));
  fgets(user_input, 128, stdin);
  user_input[strlen(user_input)-1] = '\0';

  char *pass = getpass(_("Password: "));

  size_t login_size = strlen(user_input) + strlen(pass) + 2;
  char *login = my_malloc(sizeof(char) * login_size);
  snprintf(login, login_size, "%s:%s", user_input, pass);

  memset(pass, 0x00, strlen(pass));
  free(pass);

  return login;
}

static char *get_configuration_filename()
{
  char *home_directory = getenv("HOME");

  size_t home_directory_length = 0;
  if (home_directory)
  {
    home_directory_length = strlen(home_directory);
  }

  size_t maximum_length = home_directory_length + strlen(PROXY_CONFIG_FILE) + 2;
  char *config_file = my_malloc(sizeof(char) * maximum_length);

  if (home_directory != NULL)
  {
    snprintf(config_file, maximum_length, "%s%c"PROXY_CONFIG_FILE, home_directory, SPLT_DIRCHAR);
  }
  else
  {
    snprintf(config_file, maximum_length, PROXY_CONFIG_FILE);
  }

  return config_file;
}

