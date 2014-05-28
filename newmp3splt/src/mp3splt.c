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
#include "utils.h"
#include "print_utils.h"
#include "options_manager.h"
#include "options_parsers.h"
#include "options_checker.h"
#include "data_manager.h"
#include "freedb.h"
#include "windows_utils.h"

#ifndef __WIN32__
#include <langinfo.h>
#endif

#ifdef __WIN32__
#include <direct.h>
#endif

//in case of STDIN/STDOUT usage, we change the console file handle
FILE *console_out = NULL;
FILE *console_err = NULL;
FILE *console_progress = NULL;

//make a global variable: we use it in sigint_handler
splt_state *state;

void sigint_handler(int sig)
{
  mp3splt_stop_split(state);
  exit(1);
}

static int is_stdin(const char *current_filename)
{
  return strcmp(current_filename, "-") == 0 ||
    strcmp(current_filename, "o-") == 0 ||
    strcmp(current_filename, "f-") == 0;
}

int main(int argc, char **orig_argv)
{
  setlocale(LC_ALL, "");

#ifdef ENABLE_NLS
  textdomain(MP3SPLT_GETTEXT_DOMAIN);
#endif

  console_out = stdout;
  console_err = stderr;
  console_progress = stderr;

  //possible error
  int err = SPLT_OK;

  main_data *data = create_main_struct(argc, orig_argv);

#ifdef __WIN32__
  char mp3splt_uninstall_file[2048] = { '\0' };
  DWORD dwType, dwSize = sizeof(mp3splt_uninstall_file) - 1;
  SHGetValue(HKEY_LOCAL_MACHINE,
      TEXT("SOFTWARE\\mp3splt"),
      TEXT("UninstallString"),
      &dwType,
      mp3splt_uninstall_file,
      &dwSize);

  char *end = strrchr(mp3splt_uninstall_file, SPLT_DIRCHAR);
  if (end) { *end = '\0'; }

  char *executable = strdup(data->argv[0]);
  char *executable_dir = NULL;

  end = strrchr(executable, SPLT_DIRCHAR);
  if (end)
  {
    *end = '\0';
    executable_dir = executable;
  }
  else
  {
    if (mp3splt_uninstall_file[0] != '\0')
    {
      executable_dir = mp3splt_uninstall_file;
    }
  }

# ifdef ENABLE_NLS
  bindtextdomain(MP3SPLT_GETTEXT_DOMAIN, "translations");
  bindtextdomain(LIBMP3SPLT_WITH_SONAME, "translations");
# else
  #error No NLS detected
# endif

#else

# ifdef ENABLE_NLS
  bindtextdomain(MP3SPLT_GETTEXT_DOMAIN, LOCALEDIR);
# endif

#endif

#ifdef ENABLE_NLS
 #ifdef __WIN32__
  bind_textdomain_codeset(MP3SPLT_GETTEXT_DOMAIN, "UTF-8");
 #else
  bind_textdomain_codeset(MP3SPLT_GETTEXT_DOMAIN, nl_langinfo(CODESET));
 #endif
#endif

  state = mp3splt_new_state(&err);
  data->state = state;
  process_confirmation_error(err, data);
 
  splt_state *state = data->state;
  silence_level *sl = data->sl;
  options *opt = data->opt;

  //close nicely on Ctrl+C (for example)
  signal(SIGINT, sigint_handler);

  //callback for the library messages
  mp3splt_set_message_function(state, put_library_message, data);
  mp3splt_set_silence_level_function(state, get_silence_level, data->sl);
  //callback for the split files
  mp3splt_set_split_filename_function(state, put_split_file, data);

  //default we write mins_secs_hundr for normal split
  mp3splt_set_int_option(state, SPLT_OPT_OUTPUT_FILENAMES, SPLT_OUTPUT_DEFAULT);
  mp3splt_set_int_option(state, SPLT_OPT_TAGS, SPLT_TAGS_ORIGINAL_FILE);

  //parse command line options
  int option;
  while ((option = getopt(data->argc, data->argv,
          "Mm:O:DvifKkwleqnasrc:d:o:t:p:g:hQN12T:XxPE:A:S:G:F:C:I:b")) != -1)
  {
    switch (option)
    {
      case 'x':
        mp3splt_set_int_option(state, SPLT_OPT_XING, SPLT_FALSE);
        break;
      case 'h':
        show_small_help_exit(data);
        break;
      //deprecated: use -T
      case '1':
        mp3splt_set_int_option(state, SPLT_OPT_FORCE_TAGS_VERSION, 1);
        opt->T_option_value = 1;
        opt->T_option = SPLT_TRUE;
        break;
      //deprecated: use -T
      case '2':
        mp3splt_set_int_option(state, SPLT_OPT_FORCE_TAGS_VERSION, 2);
        opt->T_option_value = 2;
        opt->T_option = SPLT_TRUE;
        break;
      case 'T':
        opt->T_option_value = atoi(optarg);
        mp3splt_set_int_option(state, SPLT_OPT_FORCE_TAGS_VERSION, opt->T_option_value);
        opt->T_option = SPLT_TRUE;
        break;
      case 'D':
        mp3splt_set_int_option(state, SPLT_OPT_DEBUG_MODE, SPLT_TRUE);
        break;
      case 'v':
        print_version(console_out); 
        print_authors(console_out);
        free_main_struct(&data);
        exit(0);
        break;
      case 'f':
        mp3splt_set_int_option(state, SPLT_OPT_FRAME_MODE, SPLT_TRUE);
        opt->f_option = SPLT_TRUE;
        break;
      case 'k':
        mp3splt_set_int_option(state, SPLT_OPT_INPUT_NOT_SEEKABLE, SPLT_TRUE);
        opt->k_option = SPLT_TRUE;
        break;
      case 'K':
        opt->K_option = SPLT_TRUE;
        break;
      case 'w':
        mp3splt_set_int_option(state, SPLT_OPT_SPLIT_MODE, SPLT_OPTION_WRAP_MODE);
        opt->w_option = SPLT_TRUE;
        break;
      case 'l':
        opt->l_option = SPLT_TRUE;
        console_out = console_err;
        break;
      case 'e':
        mp3splt_set_int_option(state, SPLT_OPT_SPLIT_MODE, SPLT_OPTION_ERROR_MODE);
        opt->e_option = SPLT_TRUE;
        break;
      case 'q':
        opt->q_option = SPLT_TRUE;
        mp3splt_set_int_option(state, SPLT_OPT_QUIET_MODE, SPLT_TRUE);
        break;
      case 'n':
        opt->n_option = SPLT_TRUE;
        break;
      case 'a':
        mp3splt_set_int_option(state, SPLT_OPT_AUTO_ADJUST, SPLT_TRUE);
        opt->a_option = SPLT_TRUE;
        break;
      case 's':
        mp3splt_set_int_option(state, SPLT_OPT_SPLIT_MODE, SPLT_OPTION_SILENCE_MODE);
        opt->s_option = SPLT_TRUE;
        break;
      case 'r':
        mp3splt_set_int_option(state, SPLT_OPT_SPLIT_MODE, SPLT_OPTION_TRIM_SILENCE_MODE);
        opt->r_option = SPLT_TRUE;
        break;
      case 'i':
        mp3splt_set_int_option(state, SPLT_OPT_SPLIT_MODE, SPLT_OPTION_SILENCE_MODE);
        opt->i_option = SPLT_TRUE;
        break;
      case 'c':
        mp3splt_set_int_option(state, SPLT_OPT_TAGS, SPLT_CURRENT_TAGS);
        opt->c_option = SPLT_TRUE;
        opt->cddb_arg = strdup(optarg);
        break;
      case 'C':
        ;
        int intarg = atoi(optarg);
        if (intarg == 8)
        {
          mp3splt_set_int_option(state, SPLT_OPT_ID3V2_ENCODING, SPLT_ID3V2_UTF8);
        }
        else if (intarg == 16)
        {
          mp3splt_set_int_option(state, SPLT_OPT_ID3V2_ENCODING, SPLT_ID3V2_UTF16);
        }
        else if (intarg == 1)
        {
          mp3splt_set_int_option(state, SPLT_OPT_ID3V2_ENCODING, SPLT_ID3V2_LATIN1);
        }
        break;
      case 'I':
        ;
        int intarg2 = atoi(optarg);
        if (intarg2 == 8)
        {
          mp3splt_set_int_option(state, SPLT_OPT_INPUT_TAGS_ENCODING, SPLT_ID3V2_UTF8);
        }
        else if (intarg2 == 16)
        {
          mp3splt_set_int_option(state, SPLT_OPT_INPUT_TAGS_ENCODING, SPLT_ID3V2_UTF16);
        }
        else if (intarg2 == 1)
        {
          mp3splt_set_int_option(state, SPLT_OPT_INPUT_TAGS_ENCODING, SPLT_ID3V2_LATIN1);
        }
        break;
      case 'P':
        opt->P_option = SPLT_TRUE;
        mp3splt_set_int_option(state, SPLT_OPT_PRETEND_TO_SPLIT, SPLT_TRUE);
        break;
      case 'E':
        opt->export_cue_arg = strdup(optarg);
        opt->E_option = SPLT_TRUE;
        break;
      case 'A':
        opt->A_option = SPLT_TRUE;
        opt->audacity_labels_arg = strdup(optarg);
        break;
      case 'm':
        opt->m_option = SPLT_TRUE;
        opt->m3u_arg = strdup(optarg);
        mp3splt_set_m3u_filename(state, opt->m3u_arg);
        break;
      case 'M':
        mp3splt_set_int_option(state,
            SPLT_OPT_DECODE_AND_WRITE_FLAC_MD5SUM_FOR_CREATED_FILES, SPLT_TRUE);
        break;
      case 'F':
        opt->F_option = SPLT_TRUE;
        opt->full_log_arg = strdup(optarg);
        mp3splt_set_silence_full_log_filename(state, opt->full_log_arg);
        break;
      case 'S':
        opt->S_option = SPLT_TRUE;
        opt->S_option_value = atoi(optarg);
        mp3splt_set_int_option(state, SPLT_OPT_LENGTH_SPLIT_FILE_NUMBER, opt->S_option_value);
        mp3splt_set_int_option(state, SPLT_OPT_SPLIT_MODE, SPLT_OPTION_LENGTH_MODE);
        break;
      case 'd':
        opt->dir_arg = strdup(optarg);
        opt->d_option = SPLT_TRUE;
        break;
      case 'N':
        opt->N_option = SPLT_TRUE;
        break;
      case 'o':
        mp3splt_set_int_option(state, SPLT_OPT_OUTPUT_FILENAMES, SPLT_OUTPUT_FORMAT);
        if (optarg)
        {
          opt->output_format = strdup(optarg);
          if (!opt->output_format)
          {
            print_error_exit(_("cannot allocate memory !"),data);
          }
        }

        //if the split result must be written to stdout
        if (strcmp(optarg,"-") == 0)
        {
          console_out = stderr;
        }
        opt->o_option = SPLT_TRUE;
        break;
      case 'O':
        opt->O_option = SPLT_TRUE;
        long overlap_time = c_hundreths(optarg);

        if (overlap_time != -LONG_MAX)
        {
          mp3splt_set_long_option(state, SPLT_OPT_OVERLAP_TIME, overlap_time);
        }
        else
        {
          print_error_exit(_("bad overlap time expression.\n"
                "\tMust be min.sec[.0-99] or EOF-min.sec[.0-99], read man page for details."), data);
        }
        break;
      case 'X':
        opt->X_option = SPLT_TRUE;
        break;
      case 't':
        mp3splt_set_int_option(state, SPLT_OPT_SPLIT_MODE, SPLT_OPTION_TIME_MODE);

        // Token will point to "SEVERAL".
        char *first = strtok(optarg, ">");
        char *second = strtok(NULL, ">");

        long converted_time = c_hundreths(first);
        if (converted_time != -LONG_MAX)
        {
          mp3splt_set_long_option(state, SPLT_OPT_SPLIT_TIME, converted_time);
        }
        else
        {
          print_error_exit(_("bad time expression for the time split.\n"
                "\tMust be min.sec[.0-99] or EOF-min.sec[.0-99], read man page for details."), data);
        }

        if (second != NULL)
        {
          long time_minimum_length = c_hundreths(second);
          if (time_minimum_length == -LONG_MAX)
          {
            print_error_exit(_("bad minimum time expression.\n"
                  "\tMust be min.sec[.0-99] or EOF-min.sec[.0-99], read man page for details."), data);
          }

          mp3splt_set_long_option(state, SPLT_OPT_TIME_MINIMUM_THEORETICAL_LENGTH,
              time_minimum_length);
        }

        opt->t_option = SPLT_TRUE;
        break;
      case 'p':
        opt->p_option = SPLT_TRUE;
        opt->param_args = strdup(optarg);
        break;
      case 'G':
        if (opt->tags_from_fname_regex_arg)
        {
          free(opt->tags_from_fname_regex_arg);
          opt->tags_from_fname_regex_arg = NULL;
        }

        mp3splt_set_int_option(state, SPLT_OPT_TAGS, SPLT_TAGS_FROM_FILENAME_REGEX);

        if (optarg)
        {
          opt->tags_from_fname_regex_arg = strdup(optarg);
        }
        else
        {
          opt->tags_from_fname_regex_arg = NULL;
        }
        opt->G_option = SPLT_TRUE;
        break;
      case 'g':
        if (opt->custom_tags)
        {
          free(opt->custom_tags);
          opt->custom_tags = NULL;
        }

        mp3splt_set_int_option(state, SPLT_OPT_TAGS, SPLT_CURRENT_TAGS);

        if (optarg)
        {
          if (optarg[0] == 'r' && strlen(optarg) > 1)
          {
            opt->custom_tags = strdup(optarg+1);
            mp3splt_set_int_option(state, SPLT_OPT_REPLACE_TAGS_IN_TAGS, SPLT_TRUE);
          }
          else
          {
            opt->custom_tags = strdup(optarg);
          }
        }
        else
        {
          opt->custom_tags = NULL;
        }
        opt->g_option = SPLT_TRUE;
        break;
      case 'Q':
        opt->q_option = SPLT_TRUE;
        mp3splt_set_int_option(state, SPLT_OPT_QUIET_MODE, SPLT_TRUE);
        opt->qq_option = SPLT_TRUE;
        console_progress = stdout;
        fclose(stdout);
        break;
      case 'b':
        mp3splt_set_int_option(state, SPLT_OPT_HANDLE_BIT_RESERVOIR, SPLT_TRUE);
        break;
      default:
        print_error_exit(_("read man page for documentation or type 'mp3splt -h'."), data);
        break;
    }
  }

  //callback for the progress bar
  if (!opt->q_option && !opt->X_option)
  {
    mp3splt_set_progress_function(state, put_progress_bar, data);
  }

  //if quiet, does not write authors and other
  if (!opt->q_option && !opt->X_option)
  {
    print_version_authors(console_err);
  }

  //if -n option, set no tags whatever happends
  if (opt->n_option)
  {
    mp3splt_set_int_option(state, SPLT_OPT_TAGS, SPLT_NO_TAGS);
  }

  err = SPLT_OK;

  //add special directory search for plugins on Windows
#ifdef __WIN32__
  wchar_t *current_dir = NULL;
  if (executable_dir != NULL)
  {
    if (executable_dir[0] != '\0')
    {
      mp3splt_append_plugins_scan_dir(state, executable_dir);
      current_dir = win32_get_current_directory();
      _chdir(executable_dir);
    }
    free(executable);
    executable = NULL;
  }
#endif

  //after getting the options (especially the debug option), find plugins
  err = mp3splt_find_plugins(state);
  process_confirmation_error(err, data);

#ifdef __WIN32__
  if (current_dir)
  {
    win32_set_current_working_directory(current_dir);
    free(current_dir);
    current_dir = NULL;
  }
#endif

  //if we have parameter options
  if (opt->p_option)
  {
    float th = -200, off = -200, min = -200, min_track_length = -200;
    int gap = -200, nt = -200, rm = -200, shots = -200;
    float min_track_join = -200;
    float keep_silence_left = -200, keep_silence_right = -200;
    int warn_if_no_auto_adjust = -200, err_if_no_auto_adjust = -200;
    int parsed_p_options = 
      parse_silence_options(opt->param_args, &th, &gap, &nt, &off, &rm, &min, &min_track_length,
          &shots, &min_track_join, &keep_silence_left, &keep_silence_right,
          &warn_if_no_auto_adjust, &err_if_no_auto_adjust);
    if (parsed_p_options < 1)
    {
      print_error_exit(_("bad argument for -p option. No valid value was recognized !"), data);
    }

    if (keep_silence_left >= 0)
    {
      mp3splt_set_float_option(state, SPLT_OPT_KEEP_SILENCE_LEFT, keep_silence_left);
    }
    if (keep_silence_right >= 0)
    {
      mp3splt_set_float_option(state, SPLT_OPT_KEEP_SILENCE_RIGHT, keep_silence_right);
    }

    if (min_track_join > 0)
    {
      mp3splt_set_float_option(state, SPLT_OPT_PARAM_MIN_TRACK_JOIN, min_track_join);
    }
    if (shots != -200)
    {
      mp3splt_set_int_option(state, SPLT_OPT_PARAM_SHOTS, shots);
    }
    if (th > -100)
    {
      mp3splt_set_float_option(state, SPLT_OPT_PARAM_THRESHOLD, th);
    }
    if (off > -3)
    {
      mp3splt_set_float_option(state, SPLT_OPT_PARAM_OFFSET, off);
    }
    if (min >= 0)
    {
      mp3splt_set_float_option(state, SPLT_OPT_PARAM_MIN_LENGTH, min);
    }
    if (min_track_length > 0)
    {
      mp3splt_set_float_option(state, SPLT_OPT_PARAM_MIN_TRACK_LENGTH, min_track_length);
    }
    if (gap != -200)
    {
      mp3splt_set_int_option(state, SPLT_OPT_PARAM_GAP, gap);
    }
    if (nt != -200)
    {
      mp3splt_set_int_option(state, SPLT_OPT_PARAM_NUMBER_TRACKS, nt);
    }
    if (rm != -200)
    {
      mp3splt_set_int_option(state, SPLT_OPT_PARAM_REMOVE_SILENCE, rm);
    }
    if (warn_if_no_auto_adjust != -200)
    {
      mp3splt_set_int_option(state, SPLT_OPT_WARN_IF_NO_AUTO_ADJUST_FOUND, SPLT_TRUE);
    }
    if (err_if_no_auto_adjust != -200)
    {
      mp3splt_set_int_option(state, SPLT_OPT_STOP_IF_NO_AUTO_ADJUST_FOUND, SPLT_TRUE);
    }
  }

  int output_format_error = SPLT_OK;
  if (opt->o_option)
  {
    mp3splt_set_int_option(state, SPLT_OPT_CREATE_DIRS_FROM_FILENAMES, SPLT_TRUE);
    output_format_error = mp3splt_set_oformat(state, opt->output_format);
    process_confirmation_error(output_format_error, data);
  }

  if (optind > 1)
  {
    data->argv = rmopt(data->argv, optind, data->argc);
    data->argc -= optind-1;
  }

  check_args(argc, data);

  mp3splt_set_int_option(state, SPLT_OPT_ENABLE_SILENCE_LOG, ! opt->N_option);

  if (! opt->N_option)
  {
    mp3splt_set_silence_log_filename(state, "mp3splt.log");
    process_confirmation_error(err, data);
  }

  int i = 0;
  data->filenames = NULL;
  data->number_of_filenames = 0;
  data->splitpoints = NULL;
  data->number_of_splitpoints = 0;

  int we_had_directory_as_argument = SPLT_FALSE;

  char *argument = NULL;
  //we get out the filenames and the splitpoints from the left arguments
  for (i=1; i < data->argc; i++)
  {
    argument = data->argv[i];
    long hundreths = c_hundreths(argument);
    if (hundreths != -LONG_MAX)
    {
      append_splitpoint(data, hundreths);
    }
    else
    {
      if (mp3splt_check_if_directory(argument))
      {
        we_had_directory_as_argument = SPLT_TRUE;

        int num_of_files_found = 0;
        char **found_files =
          mp3splt_find_filenames(state, argument, &num_of_files_found, &err);
        int k = 0;
        for (k = 0;k < num_of_files_found; k++)
        {
          char *current_fname = found_files[k];
          
          append_filename(data, current_fname);

          if (found_files[k])
          {
            free(found_files[k]);
            found_files[k] = NULL;
          }
        }
        if (found_files)
        {
          free(found_files);
          found_files = NULL;
        }
        num_of_files_found = 0;
        process_confirmation_error(err, data);
      }
      else
      {
        append_filename(data, argument);
      }
    }
  }

  //if we have a normal split, we need to parse the splitpoints
  int normal_split = SPLT_FALSE;
  if (!opt->l_option && !opt->i_option && !opt->c_option &&
      !opt->e_option && !opt->t_option && !opt->w_option &&
      !opt->s_option && !opt->A_option && !opt->S_option &&
      !opt->r_option)
  {
    if (data->number_of_splitpoints < 2)
    {
      process_confirmation_error(SPLT_ERROR_SPLITPOINTS, data);
    }
    normal_split = SPLT_TRUE;
  }

  int j = 0;

  if (data->number_of_filenames <= 0)
  {
    print_error_exit(_("no input filename(s)."), data);
  }

  if (data->number_of_filenames > 1)
  {
    fprintf(console_out,"\n");
    fflush(console_out);
  }

  if (opt->output_format && (strcmp(opt->output_format, "-") == 0))
  {
    if (data->number_of_splitpoints > 2)
    {
      print_warning(_("multiple splitpoints with stdout !"));
    }
    else if (opt->t_option)
    {
      print_warning(_("using time mode with stdout !"));
    }
  }

  if (!opt->q_option && we_had_directory_as_argument)
  {
    show_files_and_ask_for_confirmation(data);
  }

  if (opt->G_option)
  {
    regex_options *regex_options = 
      parse_tags_from_fname_regex_options(opt->tags_from_fname_regex_arg, &err);
    process_confirmation_error(err, data);

    if (regex_options)
    {
      mp3splt_set_int_option(state, SPLT_OPT_REPLACE_UNDERSCORES_TAG_FORMAT,
          regex_options->replace_underscores_by_space);
      mp3splt_set_int_option(state, SPLT_OPT_ARTIST_TAG_FORMAT, regex_options->artist_text_format);
      mp3splt_set_int_option(state, SPLT_OPT_ALBUM_TAG_FORMAT, regex_options->album_text_format);
      mp3splt_set_int_option(state, SPLT_OPT_TITLE_TAG_FORMAT, regex_options->title_text_format);
      mp3splt_set_int_option(state, SPLT_OPT_COMMENT_TAG_FORMAT, regex_options->comment_text_format);
      mp3splt_set_input_filename_regex(state, regex_options->regex);
      mp3splt_set_default_comment_tag(state, regex_options->default_comment);

      free_regex_options(&regex_options);
    }
  }

  //split all the filenames
  for (j = 0;j < data->number_of_filenames; j++)
  {
    char *current_filename = data->filenames[j];

    sl->level_sum = 0;
    sl->number_of_levels = 0;
    err = SPLT_OK;

    if (opt->P_option)
    {
      fprintf(console_out,_(" Pretending to split file '%s' ...\n"),current_filename);
    }
    else
    {
      fprintf(console_out,_(" Processing file '%s' ...\n"),current_filename);
    }
    fflush(console_out);

    if (is_stdin(current_filename) && we_have_incompatible_stdin_option(opt))
    {
      print_error_exit(_("cannot use -k option (or STDIN) with"
            " one of the following options: -S -s -r -w -l -e -i -a -p -K"), data);
    }

    //we put the filename
    err = mp3splt_set_filename_to_split(state, current_filename);
    process_confirmation_error(err, data);

    if (opt->K_option)
    {
      mp3splt_read_original_tags(state);
      mp3splt_set_int_option(state, SPLT_OPT_CUE_CDDB_ADD_TAGS_WITH_KEEP_ORIGINAL_TAGS,
          SPLT_TRUE);
    }

    //if we list wrap files
    if (opt->l_option)
    {
      //if no error when putting the filename to split
      splt_wrap *wrap_files = mp3splt_get_wrap_files(state, &err);
      process_confirmation_error(err, data);

      //if no error when getting the wrap files
      mp3splt_wrap_init_iterator(wrap_files);
      fprintf(stdout,"\n");
      const splt_one_wrap *one_wrap = NULL;
      while ((one_wrap = mp3splt_wrap_next(wrap_files)))
      {
        char *wrap_file = mp3splt_wrap_get_wrapped_file(one_wrap);
        if (wrap_file)
        {
          fprintf(stdout,"%s\n", wrap_file);
          free(wrap_file);
        }
      }
      fprintf(stdout,"\n");
      fflush(stdout);
    }
    else
    {
      //count how many silence splitpoints we have
      //if we count how many silence splitpoints
      if (opt->i_option)
      {
        err = SPLT_OK;
        mp3splt_set_silence_points(state, &err);
        process_confirmation_error(err, data);
      }
      else
      {
        if (opt->c_option)
        {
          if ((strstr(opt->cddb_arg, ".cue")!=NULL)||
              (strstr(opt->cddb_arg, ".CUE")!=NULL))
          {
            err = mp3splt_import(state, CUE_IMPORT, opt->cddb_arg);
            process_confirmation_error(err, data);

            err = SPLT_OK;
            splt_point *splitpoint = mp3splt_point_new(LONG_MAX, &err);
            process_confirmation_error(err, data);
            err = mp3splt_append_splitpoint(state, splitpoint);
            process_confirmation_error(err, data);

            err = mp3splt_remove_tags_of_skippoints(state);
            process_confirmation_error(err, data);
          }
          else if (strncmp(opt->cddb_arg, "query", 5) == 0)
          {
            if (j == 0)
            {
              int ambigous = parse_query_arg(opt,opt->cddb_arg);
              if (ambigous)
              {
                print_warning(_("freedb query format ambigous !"));
              }

              do_freedb_search(data);
            }

            err = mp3splt_import(state, CDDB_IMPORT, MP3SPLT_CDDBFILE);
            process_confirmation_error(err, data);
          }
          else if (strncmp(opt->cddb_arg, "internal_sheet", 14) == 0)
          {
            err = mp3splt_import(state, PLUGIN_INTERNAL_IMPORT, current_filename);
            process_confirmation_error(err, data);
          }
          else
          {
            err = mp3splt_import(state, CDDB_IMPORT, opt->cddb_arg);
            process_confirmation_error(err, data);
          }
        }
        else if (opt->audacity_labels_arg)
        {
          err = mp3splt_import(state, AUDACITY_LABELS_IMPORT, opt->audacity_labels_arg);
          process_confirmation_error(err, data);
        }
        else if (normal_split)
        {
          //we set the splitpoints to the library
          for (i = 0;i < data->number_of_splitpoints; i++)
          {
            splt_point *splitpoint = mp3splt_point_new(data->splitpoints[i], &err);
            process_confirmation_error(err, data);

            err = mp3splt_append_splitpoint(state, splitpoint);
            process_confirmation_error(err, data);
          }
        }

        //we set the path of split for the -d option
        if (opt->d_option)
        { err = mp3splt_set_path_of_split(state, opt->dir_arg);
          process_confirmation_error(err, data);
        }

        if (opt->g_option && (opt->custom_tags != NULL))
        {
          int ambiguous = mp3splt_put_tags_from_string(state, opt->custom_tags, &err);
          process_confirmation_error(err, data);
          if (ambiguous)
          {
            print_warning(_("tags format ambiguous !"));
          }
        }

        //for cddb, filenames are already set from the library, so 
        //set output filenames to CUSTOM
        int saved_output_filenames = mp3splt_get_int_option(state, SPLT_OPT_OUTPUT_FILENAMES, &err);
        if ((opt->c_option || opt->A_option) && !opt->o_option)
        {
          mp3splt_set_int_option(state, SPLT_OPT_OUTPUT_FILENAMES, SPLT_OUTPUT_CUSTOM);
        }

        //we do the effective split
        err = mp3splt_split(state);
        process_confirmation_error(err, data);

        //for cddb, set output filenames to its old value before the split
        if (opt->c_option && !opt->o_option)
        {
          mp3splt_set_int_option(state, SPLT_OPT_OUTPUT_FILENAMES, saved_output_filenames);
        }

        //print the average silence level
        if (opt->s_option)
        {
          if (sl->print_silence_level)
          {
            if (sl->number_of_levels != 0)
            {
              float average_silence_levels = sl->level_sum / (double) sl->number_of_levels;
              char message[256] = { '\0' };
              snprintf(message,256,
                  _(" Average silence level: %.2f dB"), average_silence_levels);
              print_message(message);
            }
          }
        }
      }
    }

    if (opt->E_option)
    {
      err = mp3splt_export(state, CUE_EXPORT, opt->export_cue_arg, SPLT_TRUE);
      process_confirmation_error(err, data);
    }

    if (opt->c_option && err >= 0 && !opt->q_option &&
        !(strncmp(opt->cddb_arg, "internal_sheet", 14) == 0))
    {
      print_message(_("\n +------------------------------------------------------------------------------+\n"
            " | NOTE: When you use cddb/cue, split files might be not very precise due to:   |\n"
            " | 1) Who extracts CD tracks might use \"Remove silence\" option. This means that |\n"
            " |    the large mp3 file is shorter than CD Total time. Never use this option.  |\n"
            " | 2) Who burns CD might add extra pause seconds between tracks.  Never do it.  |\n"
            " | 3) Encoders might add some padding frames so  that  file is longer than CD.  |\n"
            " | 4) There are several entries of the same cd on CDDB, find the best for yours.|\n"
            " |    Usually you can find the correct splitpoints, so good luck!               |\n"
            " +------------------------------------------------------------------------------+\n"
            " |  TRY TO ADJUST SPLITS POINT WITH -a OPTION. Read man page for more details!  |\n"
            " +------------------------------------------------------------------------------+\n"));
    }

    if (data->number_of_filenames > 1)
    {
      fprintf(console_out,"\n");
      fflush(console_out);
    }

    err = mp3splt_erase_all_tags(state);
    process_confirmation_error(err, data);

    err = mp3splt_erase_all_splitpoints(state);
    process_confirmation_error(err, data);
  }

  free_main_struct(&data);

  return 0;
}

