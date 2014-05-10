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
#include "options_manager.h"

char **rmopt(char **argv, int offset, int tot)
{
#ifdef __WIN32__
  int i = 0;
  for (i = 0; i < offset;i++)
  {
    if (argv[i])
    {
      free(argv[i]);
      argv[i] = NULL;
    }
  }
#endif

  char **first = &argv[1];
  while (offset < tot)
  {
    *first = argv[offset];
    first++;
    offset++;
  }

  return argv;
}

void free_options(options **opt)
{
  if (opt)
  {
    if (*opt)
    {
      if ((*opt)->dir_arg)
      {
        free((*opt)->dir_arg);
        (*opt)->dir_arg = NULL;
      }

      if ((*opt)->cddb_arg)
      {
        free((*opt)->cddb_arg);
        (*opt)->cddb_arg = NULL;
      }

      if ((*opt)->export_cue_arg)
      {
        free((*opt)->export_cue_arg);
        (*opt)->export_cue_arg = NULL;
      }

      if ((*opt)->audacity_labels_arg)
      {
        free((*opt)->audacity_labels_arg);
        (*opt)->audacity_labels_arg = NULL;
      }

      if ((*opt)->m3u_arg)
      {
        free((*opt)->m3u_arg);
        (*opt)->m3u_arg = NULL;
      }

      if ((*opt)->full_log_arg)
      {
        free((*opt)->full_log_arg);
        (*opt)->full_log_arg = NULL;
      }

      if ((*opt)->param_args)
      {
        free((*opt)->param_args);
        (*opt)->param_args = NULL;
      }

      if ((*opt)->tags_from_fname_regex_arg)
      {
        free((*opt)->tags_from_fname_regex_arg);
        (*opt)->tags_from_fname_regex_arg = NULL;
      }

      if ((*opt)->custom_tags)
      {
        free((*opt)->custom_tags);
        (*opt)->custom_tags = NULL;
      }

      if ((*opt)->output_format)
      {
        free((*opt)->output_format);
        (*opt)->output_format = NULL;
      }
      free(*opt);
      *opt = NULL;
    }
  }
}

options *new_options()
{
  options *opt = my_malloc(sizeof(options));

  opt->T_option = SPLT_FALSE;
  opt->T_option_value = 0;
  opt->w_option = SPLT_FALSE; opt->l_option = SPLT_FALSE;
  opt->e_option = SPLT_FALSE; opt->f_option = SPLT_FALSE;
  opt->c_option = SPLT_FALSE; opt->t_option = SPLT_FALSE;
  opt->s_option = SPLT_FALSE; opt->a_option = SPLT_FALSE;
  opt->r_option = SPLT_FALSE;
  opt->p_option = SPLT_FALSE; opt->o_option = SPLT_FALSE;
  opt->d_option = SPLT_FALSE; opt->k_option = SPLT_FALSE;
  opt->K_option = SPLT_FALSE;
  opt->g_option = SPLT_FALSE; opt->n_option = SPLT_FALSE;
  opt->q_option = SPLT_FALSE; opt->i_option = SPLT_FALSE;
  opt->N_option = SPLT_FALSE; opt->O_option = SPLT_FALSE;
  opt->G_option = SPLT_FALSE;
  opt->E_option = SPLT_FALSE;
  opt->P_option = SPLT_FALSE;
  opt->x_option = SPLT_FALSE;
  opt->X_option = SPLT_FALSE;
  opt->qq_option = SPLT_FALSE;
  opt->A_option = SPLT_FALSE;
  opt->m_option = SPLT_FALSE;
  opt->F_option = SPLT_FALSE;
  opt->S_option = SPLT_FALSE;
  opt->S_option_value = 0;
  opt->tags_from_fname_regex_arg = NULL;
  opt->cddb_arg = NULL;
  opt->dir_arg = NULL;
  opt->export_cue_arg = NULL;
  opt->audacity_labels_arg = NULL;
  opt->m3u_arg = NULL;
  opt->full_log_arg = NULL;
  opt->param_args = NULL;
  opt->output_format = NULL;

  opt->custom_tags = NULL;

  //we put the default values for freedb search
  //by default, CDDB_CGI (cddb.cgi) port 80 on freedb2.org
  opt->freedb_search_type = SPLT_FREEDB_SEARCH_TYPE_CDDB_CGI;
  snprintf(opt->freedb_search_server,255, "%s", SPLT_FREEDB2_CGI_SITE);
  opt->freedb_search_port = SPLT_FREEDB_CDDB_CGI_PORT;
  //we put the default values for the freedb cddb file get
  //by default, CDDB_CGI (cddb.cgi) port 80 on freedb2.org
  opt->freedb_get_type = SPLT_FREEDB_GET_FILE_TYPE_CDDB_CGI;
  snprintf(opt->freedb_get_server,255, "%s", SPLT_FREEDB2_CGI_SITE);
  opt->freedb_get_port = SPLT_FREEDB_CDDB_CGI_PORT;

  opt->freedb_arg_search_string[0] = '\0';
  opt->freedb_arg_result_option = -1;

  return opt;
}

regex_options *new_regex_options()
{
  regex_options *regex_opt = my_malloc(sizeof(regex_options));

  regex_opt->replace_underscores_by_space = SPLT_FALSE;

  regex_opt->artist_text_format = SPLT_NO_CONVERSION;
  regex_opt->album_text_format = SPLT_NO_CONVERSION;
  regex_opt->title_text_format = SPLT_NO_CONVERSION;
  regex_opt->comment_text_format = SPLT_NO_CONVERSION;

  regex_opt->default_comment = NULL;
  regex_opt->regex = NULL;

  return regex_opt;
}

void free_regex_options(regex_options **regex_opt)
{
  if (!regex_opt || !*regex_opt)
  {
    return;
  }

  if ((*regex_opt)->default_comment)
  {
    free((*regex_opt)->default_comment);
    (*regex_opt)->default_comment = NULL;
  }

  if ((*regex_opt)->regex)
  {
    free((*regex_opt)->regex);
    (*regex_opt)->regex = NULL;
  }

  free(*regex_opt);
  *regex_opt = NULL;
}

