/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2005-2011 Alexandru Munteanu - io_fx@yahoo.fr
 *
 * http://mp3splt.sourceforge.net/
 *
 *********************************************************/

/**********************************************************
 *
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 * USA.
 *
 *********************************************************/

#include <gtk/gtk.h>

#include "ui_manager.h"

void ui_set_browser_directory(ui_state *ui, const gchar *directory)
{
  ui_infos *infos = ui->infos;

  if (infos->browser_directory)
  {
    g_free(infos->browser_directory);
    infos->browser_directory = NULL;
  }

  if (directory == NULL)
  {
    infos->browser_directory = NULL;
    return;
  }

  infos->browser_directory = g_strdup(directory);
}

const gchar *ui_get_browser_directory(ui_state *ui)
{
  return ui->infos->browser_directory;
}

void ui_set_main_win_position(ui_state *ui, gint x, gint y)
{
  if (x == 0 && y == 0)
  {
    return;
  }

  ui_main_window *main_win = ui->infos->main_win;
  main_win->root_x_pos = x;
  main_win->root_y_pos = y;
}

void ui_set_main_win_size(ui_state *ui, gint width, gint height)
{
  ui_main_window *main_win = ui->infos->main_win;
  main_win->width = width;
  main_win->height = height;
}

const ui_main_window *ui_get_main_window_infos(ui_state *ui)
{
  return ui->infos->main_win;
}

static void ui_main_window_new(ui_infos *infos)
{
  ui_main_window *main_win = g_malloc0(sizeof(ui_main_window));

  main_win->root_x_pos = 0;
  main_win->root_y_pos = 0;

  main_win->width = UI_DEFAULT_WIDTH;
  main_win->height = UI_DEFAULT_HEIGHT;

  infos->main_win = main_win;
}

static void ui_infos_new(ui_state *ui)
{
  ui_infos *infos = g_malloc0(sizeof(ui_infos));

  ui_main_window_new(infos);

  infos->browser_directory = NULL;

  ui->infos = infos;
}

ui_state *ui_state_new()
{
  ui_state *ui = g_malloc0(sizeof(ui_state));

  ui_infos_new(ui);

  return ui;
}

static void ui_main_window_free(ui_main_window **main_win)
{
  if (!main_win || !*main_win)
  {
    return;
  }

  g_free(*main_win);
  *main_win = NULL;
}

static void ui_infos_free(ui_infos **infos)
{
  if (!infos || !*infos)
  {
    return;
  }

  ui_main_window_free(&(*infos)->main_win);

  g_free(*infos);
  *infos = NULL;
}

void ui_state_free(ui_state *ui)
{
  if (ui)
  {
    ui_infos_free(&ui->infos);

    g_free(ui);
  }
}

