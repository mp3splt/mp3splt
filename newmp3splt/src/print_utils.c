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
#include "data_manager.h"

extern FILE *console_out;
extern FILE *console_err;
extern FILE *console_progress;

void print_message(const char *m)
{
  fprintf(console_out,"%s\n",m);
  fflush(console_out);
}

void print_warning(const char *w)
{
  fprintf(console_err,_(" Warning: %s\n"),w);
  fflush(console_err);
}

void print_error(const char *e)
{
  fprintf(console_err,_(" Error: %s\n"),e);
  fflush(console_err);
}

static void print_with_spaces_after(const char *printed_value, int print_carriage_return, 
    FILE *file, main_data *data)
{
  char *value = strdup(printed_value);
  if (!value)
  {
    print_warning(_("cannot allocate memory !"));
  }

  char temp[2048] = "";
  int this_spaces = strlen(value);
  int counter = strlen(value);
  while (counter <= data->printed_value_length)
  {
    temp[counter-this_spaces] = ' ';
    counter++;
  }
  temp[counter] = '\0';

  if (print_carriage_return)
  {
    fprintf(file, "%s%s\r", value, temp);
  }
  else
  {
    int has_slash_n = SPLT_FALSE;
    if (strlen(value) > 0)
    {
      if (value[strlen(value)-1] == '\n')
      {
        has_slash_n = SPLT_TRUE;
      }
    }

    if (has_slash_n)
    {
      value[strlen(value)-1] = '\0';
      fprintf(file, "%s%s\n", value, temp);
    }
    else
    {
      fprintf(file, "%s%s", value, temp);
    }
  }

  fflush(file);

  free(value);
}

void put_library_message(const char *message, splt_message_type mess_type, void *user_data)
{
  if (mess_type == SPLT_MESSAGE_INFO)
  {
    main_data *data = (main_data *) user_data;
    print_with_spaces_after(message, SPLT_FALSE, console_out, data);
  }
  else if (mess_type == SPLT_MESSAGE_WARNING)
  {
    main_data *data = (main_data *) user_data;
    print_with_spaces_after(message, SPLT_FALSE, console_err, data);
  }
  else if (mess_type == SPLT_MESSAGE_DEBUG)
  {
    fprintf(stderr, "%s", message);
    fflush(stderr);
  }
}

void put_split_file(const char *file, void *user_data)
{
  main_data *data = (main_data *) user_data;

  char temp[1024] = "";
  int this_spaces = strlen(file)+16;
  int counter = strlen(file)+16;
  while (counter <= data->printed_value_length)
  {
    temp[counter-this_spaces] = ' ';
    counter++;
  }
  temp[counter] = '\0';

  fprintf(console_out,_("   File \"%s\" created%s\n"),file,temp);
  fflush(console_out);
}

void put_progress_bar(splt_progress *p_bar, void *user_data)
{
  main_data *data = (main_data *)user_data;

  char progress_text[2048] = " ";
  char *filename_shorted = mp3splt_progress_get_filename_shorted(p_bar);

  switch (mp3splt_progress_get_type(p_bar))
  {
    case SPLT_PROGRESS_PREPARE:
      snprintf(progress_text,2047,
          _(" preparing \"%s\" (%d of %d)"),
          filename_shorted,
          mp3splt_progress_get_current_split(p_bar),
          mp3splt_progress_get_max_splits(p_bar));
      break;
    case SPLT_PROGRESS_CREATE:
      snprintf(progress_text,2047,
          _(" creating \"%s\" (%d of %d)"),
          filename_shorted,
          mp3splt_progress_get_current_split(p_bar),
          mp3splt_progress_get_max_splits(p_bar));
      break;
    case SPLT_PROGRESS_SEARCH_SYNC:
      snprintf(progress_text,2047,
          _(" searching for sync errors..."));
      break;
    case SPLT_PROGRESS_SCAN_SILENCE:
      ;
      int silence_found_tracks = mp3splt_progress_get_silence_found_tracks(p_bar);
      float silence_db_level = mp3splt_progress_get_silence_db_level(p_bar);
      if (silence_found_tracks > 0)
      {
        snprintf(progress_text,2047,
            _("S: %02d, Level: %.2f dB; scanning for silence..."),
            silence_found_tracks, silence_db_level);
      }
      else {
        snprintf(progress_text,2047,
            _("Level: %.2f dB; scanning for silence..."),
            silence_db_level);
      }
      break;
    default:
      snprintf(progress_text,2047, " ");
      break;
  }

  float percent_progress = mp3splt_progress_get_percent_progress(p_bar);

  char printed_value[2048] = "";
  //we update the progress
  if (percent_progress <= 0.01)
  {
    snprintf(printed_value, 2047," [ - %%] %s", progress_text);
  }
  else
  {
    snprintf(printed_value, 2047," [ %.2f %%] %s", percent_progress * 100, progress_text);
  }

  print_with_spaces_after(printed_value, SPLT_TRUE, console_progress, data);
  data->printed_value_length = strlen(printed_value) + 1;

  free(filename_shorted);
}

void print_version(FILE *std)
{
  char *version = mp3splt_get_version();
  fprintf(std, PACKAGE_NAME" "VERSION" ("MP3SPLT_DATE") -"
      " %s libmp3splt %s\n",_("using"), version);
  fflush(std);
  free(version);
}

void print_authors(FILE *std)
{
  fprintf(std, "\t"MP3SPLT_AUTHOR1" "MP3SPLT_EMAIL1"\n\t"MP3SPLT_AUTHOR2" "MP3SPLT_EMAIL2"\n");
  fflush(std);
}

void print_no_warranty(FILE *std)
{
  fprintf(std, _("THIS SOFTWARE COMES WITH ABSOLUTELY NO WARRANTY!"
        " USE AT YOUR OWN RISK!\n"));
  fflush(std);
}

void print_version_authors(FILE *std)
{
  print_version(std);
  print_authors(std);
  print_no_warranty(std);
}

void print_error_exit(const char *m, main_data *data)
{
  print_error(m);
  free_main_struct(&data);
  exit(1);
}

void print_message_exit(const char *m, main_data *data)
{
  print_message(m);
  free_main_struct(&data);
  exit(0);
}

void process_confirmation_error(int error, main_data *data)
{
  char *error_from_library = NULL;
  error_from_library = mp3splt_get_strerror(data->state, error);
  if (error_from_library != NULL)
  {
    if (error >= 0)
    {
      print_message(error_from_library);
      free(error_from_library);
    }
    else
    {
      int size = strlen(error_from_library) + 10;
      char *error_with_slash_n = malloc(sizeof(char) * size);
      snprintf(error_with_slash_n, size, "%s\n", error_from_library);
      print_with_spaces_after(error_with_slash_n, SPLT_FALSE, console_err, data);
      free(error_with_slash_n);

      free(error_from_library);
      free_main_struct(&data);
      exit(1);
    }
    error_from_library = NULL;
  }

  if (error == SPLT_DEWRAP_OK)
  {
    print_message(_("\nAll files have been split correctly."
          " Visit http://mp3wrap.sourceforge.net!"));
  }
}

void show_small_help_exit(main_data *data)
{
  free_main_struct(&data);

  print_message(_("\n"
        "USAGE:\n"
        "      mp3splt [OPTIONS] FILE1 [FILE2] ... [BEGIN_TIME] [TIME] ... [END_TIME]\n"
        "      TIME FORMAT: min.sec[.0-99], even if minutes are over 59\n"
        "                   or EOF-min.sec[.0-99] (or EOF for End Of File). "));
  print_message(_("\nOPTIONS (split mode options)\n"
        " -t + TIME: to split files every fixed time len. (TIME format same as above). \n"
        " -c + file.cddb, file.cue or \"query\" or \"query{album}\" or \"internal_sheet\".\n"
        "      Get splitpoints and filenames from a .cddb or .cue file or from Internet\n"
        "      (\"query\"). Use -a to auto-adjust splitpoints."));
  print_message(_(" -s   Silence detection: automatically find splitpoint. (Use -p for arguments)\n"
        " -w   Splits wrapped files created with Mp3Wrap or AlbumWrap.\n"
        " -l   Lists the tracks from file without extraction. (Only for wrapped mp3)\n"
        " -e   Error mode: split mp3 with sync error detection. (For concatenated mp3)"));
  print_message(_(" -r   Trim using silence detection (Use -p for arguments)"));

  print_message(_(" -A + AUDACITY_FILE: split with splitpoints from the audacity labels file"));
  print_message(_(" -S + SPLIT_NUMBER: split in SPLIT_NUMBER equal time files"));
/*  print_message(_(" -i   Count how many silence splitpoints we have with silence detection\n"
        "      (Use -p for arguments) - DEPRECATED: use -s with -P)\n"*/
  print_message(" -v   Prints current version and exits\n"
      " -h   Shows this help");
  print_message(_("\n(other options)\n"
        " -T + TAGS_VERSION: for mp3 files, force output tags as version 1, 2 or 1 & 2.\n"
        "      TAGS_VERSION can be 1, 2 or 12\n"
        "      (default is to set the same version as the file to split)"));
  print_message(_(" -m + M3U_FILE: Appends to the specified m3u file the split filenames.\n"
        " -f   Frame mode (mp3 only): process all frames. For higher precision and VBR.\n"
        " -b   [Experimental] Bit reservoir handling for gapless playback (mp3 only).\n"
        " -a   Auto-Adjust splitpoints with silence detection. (Use -p for arguments)"));
  print_message(_(" -p + PARAMETERS (th, nt, off, min, rm, gap, trackmin, shots, trackjoin): "
                  "user arguments for -s, -a, -t.\n"
        " -o + FORMAT: output filename pattern. Can contain those variables:\n"
        "      @a: artist tag, @p: performer tag (might not exists), @b: album tag\n"
        "      @t: title tag, @n: track number identifier, @N: track tag number\n"
        "      (a digit may follow the 'n' or 'N' for the number of digits to output),\n"
        "      @f: original filename, @g: genre"));
  print_message(_(" -g + TAGS: custom tags for the split files.\n"
        "      TAGS can contain those variables: \n"
        "         @a, @b, @t, @y, @c, @n, @g, @o (set original tags),\n"
        "         @N (auto increment track number).\n"
        "      TAGS format is like [@a=artist1,@t=title1]%[@o,@N=2,@a=artist2]\n"
        "       (% means that we set the tags for all remaining files)"));
  print_message(_(" -G + regex=REGEX: set tags from input filename. REGEX defines how to extract\n"
        "      the tags from the filename. It can contain those variables:\n"
        //"       (?<tracks>),\n"
        "         (?<artist>), (?<album>), (?<title>), (?<tracknum>), (?<year>), (?<comment>), (?<genre>)"));
  print_message(_(" -d + DIRNAME: to put all output files in the directory DIRNAME.\n"
        " -k   Consider input not seekable (slower). Default when input is STDIN (-).\n"
        " -O + TIME: Overlap split files with TIME (slower)."));
  print_message(_(" -n   No Tag: does not write ID3v1 or vorbis comment. If you need clean files.\n"
        " -x   No Xing header: does not write the Xing header. Use with -n if you wish\n"
        "      to concatenate the split files\n"
        " -N   Don't create the 'mp3splt.log' log file when using '-s'."));
  print_message(_(" -P   Pretend to split: simulation of the process, without creating any\n"
                  "      files or directories"));
  print_message(_(" -E + CUE_FILE: export splitpoints to CUE file (use with -P if needed)"));
  print_message(_(" -q   Quiet mode: try not to prompt (if possible) and print less messages.\n"
        " -Q   Very quiet mode: don't print anything to stdout and no progress bar\n"
        "       (also enables -q).\n"
        " -D   Debug mode: used to debug the program.\n\n"
        "      Please read man page for complete documentation.\n"));

  if (console_out == stderr)
  {
    exit(1);
  }
  else
  {
    exit(0);
  }
}


