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

#ifndef OPTIONS_MANAGER_H

typedef struct {
  //force id3v1 tags, force id3v2 tags or both
  short T_option;
  short T_option_value;
  //wrap split, list wrap options, error split
  short w_option; short l_option; short e_option;
  //frame mode, cddb/cue option, time split
  short f_option; short c_option; short t_option;
  //silence split, adjust option, parameters
  short s_option; short a_option; short p_option;
  //trim silence split
  short r_option;
  //output filename, output directory, seekable
  short o_option; short d_option; short k_option;
  //keep original tags when using cddb or cue import
  short K_option;
  //custom tags, no tags, quiet option
  short g_option; short n_option; short q_option;
  short E_option;
  short P_option;
  short x_option;
  short N_option;
  short O_option;
  short X_option;
  short A_option;
  short G_option;
  //-Q option
  short qq_option;
  //info -i option, m3u file option
  short i_option;
  short m_option;
  short F_option;
  short S_option;
  int S_option_value;
  char *tags_from_fname_regex_arg;
  //cddb argument, output dir argument, parameters arguments with -p
  char *cddb_arg; char *dir_arg; char *param_args;
  char *export_cue_arg;
  char *audacity_labels_arg;
  //custom tags with -g
  char *custom_tags;
  char *m3u_arg;
  char *full_log_arg;
  //output format (-o)
  char *output_format;
  //the parsed freedb_search_type
  //the parsed freedb_search_server
  //the parsed freedb_search_port
  int freedb_search_type;
  char freedb_search_server[256];
  int freedb_search_port;
  //the parsed freedb_get_type
  //the parsed freedb_get_server
  //the parsed freedb_get_port
  int freedb_get_type;
  char freedb_get_server[256];
  int freedb_get_port;
  //the search string passed in parameter -c query{my artist}
  char freedb_arg_search_string[2048];
  //the chosen result passed in parameter: -c query{my artist}[
  int freedb_arg_result_option;
} options;

typedef struct {
  int replace_underscores_by_space;

  int artist_text_format;
  int album_text_format;
  int title_text_format;
  int comment_text_format;

  char *default_comment;
  char *regex;
} regex_options;

char **rmopt(char **argv, int offset, int tot);
options *new_options();
void free_options(options **opt);

regex_options *new_regex_options();
void free_regex_options(regex_options **regex_opt);

#define OPTIONS_MANAGER_H
#endif

