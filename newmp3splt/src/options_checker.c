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

#include <string.h>

#include "common.h"
#include "data_manager.h"
#include "print_utils.h"

#include "options_checker.h"

extern FILE *console_out;

void check_args(int argc, main_data *data)
{
  options *opt = data->opt;

  if (argc < 2)
  {
    console_out = stderr;
    show_small_help_exit(data);
    return;
  }

  if (opt->k_option)
  {
    if (we_have_incompatible_stdin_option(opt))
    {
      print_error_exit(_("cannot use -k option (or STDIN) with"
            " one of the following options: -S -s -r -w -l -e -i -a -p -F"), data);
    }
  }

  if (opt->w_option)
  {
    if (opt->t_option || opt->c_option ||
        opt->s_option || opt->l_option ||
        opt->e_option || opt->i_option ||
        opt->f_option || opt->a_option ||
        opt->p_option || opt->o_option ||
        opt->g_option || opt->n_option ||
        opt->x_option || opt->A_option ||
        opt->E_option || opt->S_option ||
        opt->G_option || opt->r_option ||
        opt->F_option)
    {
      print_error_exit(_("the -w option can only be used with -m, -d, -q and -Q"), data);
    }
  }

  if (opt->l_option)
  {
    if (opt->t_option || opt->c_option ||
        opt->s_option || opt->e_option ||
        opt->i_option || opt->m_option ||
        opt->f_option || opt->a_option ||
        opt->p_option || opt->o_option ||
        opt->g_option || opt->d_option ||
        opt->n_option || opt->qq_option ||
        opt->x_option || opt->A_option ||
        opt->S_option || opt->G_option ||
        opt->r_option || opt->F_option)
    {
      print_error_exit(_("the -l option can only be used with -q"), data);
    }
  }

  if (opt->e_option)
  {
    if (opt->t_option || opt->c_option || 
        opt->s_option || opt->i_option || 
        opt->a_option || opt->p_option ||
        opt->g_option || opt->n_option ||
        opt->A_option || opt->E_option ||
        opt->S_option || opt->G_option ||
        opt->r_option || opt->F_option)
    {
      print_error_exit(_("the -e option can only be used with -m, -f, -o, -d, -q, -Q"), data);
    }
  }

  if (opt->f_option)
  {
  }

  if (opt->c_option)
  {
    if (opt->t_option || opt->s_option ||
        opt->i_option || opt->g_option ||
        opt->A_option || opt->S_option ||
        opt->G_option || opt->r_option ||
        opt->F_option)
    {
      print_error_exit(_("the -c option cannot be used with -t, -g, -G, -s, -r, -A, -i, -S or -F"), data);
    }
  }

  if (opt->A_option)
  {
    if (opt->t_option || opt->s_option ||
        opt->i_option || opt->S_option || 
        opt->r_option || opt->F_option)
    {
      print_error_exit(_("the -A option cannot be used with -t, -s, -r, -i, -S or -F"), data);
    }
  }

  if (opt->t_option)
  {
    if (opt->s_option || opt->i_option ||
        opt->S_option || opt->r_option ||
        opt->F_option)
    {
      print_error_exit(_("the -t option cannot be used with -s, -r, -i, -S or -F"), data);
    }
  }

  if (opt->s_option)
  {
    if (opt->a_option || opt->i_option ||
        opt->S_option || opt->r_option)
    {
      print_error_exit(_("-s option cannot be used with -a, -r, -i or -S"), data);
    }
  }

  if (opt->a_option)
  {
    if (opt->i_option)
    {
      print_error_exit(_("-a option cannot be used with -i"), data);
    }
  }

  if (opt->S_option)
  {
  }

  if (opt->p_option)
  {
    if (!opt->a_option && !opt->s_option && !opt->i_option && !opt->r_option)
    {
      print_error_exit(_("the -p option cannot be used without -a, -s, -r  or -i"), data);
    }
  }

  if (opt->o_option)
  {
    if (opt->i_option)
    {
      print_error_exit(_("the -o option cannot be used with -i"), data);
    }
    if (opt->output_format)
    {
      if ((strcmp(opt->output_format,"-") == 0) && (opt->m_option || opt->d_option))
      {
        print_error_exit(_("cannot use '-o -' (STDOUT) with -m or -d"), data);
      }
    }
  }

  if (opt->g_option)
  {
    if (opt->i_option || opt->n_option || opt->G_option)
    {
      print_error_exit(_("the -g option cannot be used with -n, -i or -G"), data);
    }
  }

  if (opt->d_option)
  {
    if (opt->i_option)
    {
      print_error_exit(_("the -d option cannot be used with -i"), data);
    }
  }

  if (opt->n_option)
  {
    if (opt->i_option || opt->T_option)
    {
      print_error_exit(_("the -n option cannot be used with -i or -T"), data);
    }
  }

  if (opt->m_option)
  {
    if (opt->i_option)
    {
      print_error_exit(_("the -m option cannot be used with -i"), data);
    }
  }

  if (opt->i_option)
  {
  }

  if (opt->q_option)
  {
  }

  if (opt->qq_option)
  {
    if (opt->o_option)
    {
      if (strcmp(opt->output_format,"-") == 0)
      {
        print_error_exit(_("the -Q option cannot be used with"
              " STDOUT output ('-o -')"), data);
      }
    }
    if (opt->c_option)
    {
      if (strncmp(opt->cddb_arg, "query", 5) == 0)
      {
        print_error_exit(_("the -Q option cannot be used with"
              " interactive freedb query ('-c query')"), data);
      }
    }
  }

  if (opt->N_option)
  {
    if (!opt->s_option)
    {
      print_error_exit(_("the -N option must be used with silence detection (-s option)"), data);
    }
  }

  if (opt->O_option)
  {
    if (opt->w_option || opt->e_option ||
        opt->l_option || opt->i_option)
    {
      print_error_exit(_("the -O option cannot be used with -w, -e, -l or -i"), data);
    }
  }

  if (opt->x_option)
  {
  }

  if (opt->T_option)
  {
    int force_tags_version = opt->T_option_value;
    if ((force_tags_version != 1) && (force_tags_version != 2) &&
        (force_tags_version != 12))
    {
      print_error_exit("the -T option can only have values 1, 2 or 12", data);
    }
  }

  if (opt->G_option)
  {
  }

  if (opt->F_option)
  {
    if (!opt->s_option && !opt->r_option)
    {
      print_error_exit(_("the -F option cannot be used without -s or -r"), data);
    }
  }

  if (opt->K_option)
  {
    if (!opt->c_option)
    {
      print_error_exit(_("the -K option cannot be used without -c"), data);
    }
  }
}

int we_have_incompatible_stdin_option(options *opt)
{
  return opt->s_option || opt->w_option ||
    opt->l_option || opt->e_option ||
    opt->i_option || opt->a_option ||
    opt->p_option || opt->S_option || opt->r_option || opt->F_option || opt->K_option;
}

