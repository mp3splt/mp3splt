/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2005-2012 Alexandru Munteanu - io_fx@yahoo.fr
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

#include "ui_manager.h"

static void ui_main_window_new(ui_infos *infos);
static void ui_infos_new(ui_state *ui);
static void ui_status_new(ui_state *ui);

static void ui_main_window_free(ui_main_window **main_win);
static void ui_infos_free(ui_infos **infos);
static void ui_status_free(gui_status **status);

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

ui_state *ui_state_new()
{
  ui_state *ui = g_malloc0(sizeof(ui_state));

  ui_infos_new(ui);
  ui->preferences = pm_state_new();

  gint error = SPLT_OK;
  ui->mp3splt_state = mp3splt_new_state(&error);
  if (error < 0)
  {
    ui_fail(ui, "mp3splt state initialization failed\n", NULL);
  }

  ui->splitpoints = g_array_new(FALSE, FALSE, sizeof(Split_point));
  ui->gui = g_malloc0(sizeof(gui_state));

  ui_status_new(ui);

  ui->return_code = EXIT_SUCCESS;

  return ui;
}

void ui_state_free(ui_state *ui)
{
  if (!ui) { return; }

  ui_infos_free(&ui->infos);
  pm_free(&ui->preferences);

  if (ui->mp3splt_state)
  {
    mp3splt_free_state(ui->mp3splt_state, NULL);
  }

  g_array_free(ui->splitpoints, TRUE);
  g_free(ui->gui);

  ui_status_free(&ui->status);

  g_free(ui);
}

void ui_register_spinner_int_preference(gchar *main_key, gchar *second_key,
    gint default_value, GtkWidget *spinner,
    void (*update_spinner_value_cb)(GtkWidget *spinner, gpointer data),
    gpointer user_data_for_cb, ui_state *ui)
{
  pm_register_spinner_int_preference(main_key, second_key,
      default_value, spinner, update_spinner_value_cb, user_data_for_cb, ui->preferences);
}

void ui_register_range_preference(gchar *main_key, gchar *second_key,
    gint default_value, GtkWidget *range,
    void (*update_adjustment_value)(GtkAdjustment *adjustment, gpointer user_data),
    gpointer user_data_for_cb, ui_state *ui)
{
  pm_register_range_preference(main_key, second_key,
      default_value, range, update_adjustment_value, user_data_for_cb, ui->preferences);
}

void ui_load_preferences(GKeyFile *key_file, ui_state *ui)
{
  pm_load(key_file, ui->preferences);
}

void ui_save_preferences(GKeyFile *key_file, ui_state *ui)
{
  pm_save(key_file, ui->preferences);
}

void ui_write_default_preferences(GKeyFile *key_file, ui_state *ui)
{
  pm_write_default(key_file, ui->preferences);
}

void ui_fail(ui_state *ui, const gchar *message, ...)
{
  if (message != NULL)
  {
    gchar formatted_message[1024] = { '\0' };

    va_list ap;
    va_start(ap, message);
    g_vsnprintf(formatted_message, 1024, message, ap);
    va_end(ap);

    fprintf(stderr, formatted_message);
    fflush(stderr);
  }

  ui->return_code = EXIT_FAILURE;

  ui_state_free(ui);

  exit(1);
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
  infos->text_options_list = NULL;

  infos->silence_points = NULL;
  infos->malloced_num_of_silence_points = 0;
  infos->number_of_silence_points = 0;

  ui->infos = infos;
}

static void ui_status_new(ui_state *ui)
{
  gui_status *status = g_malloc0(sizeof(gui_status));

  status->splitting = FALSE;
  status->quit_main_program = FALSE;

  ui->status = status;
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

  if ((*infos)->text_options_list)
  {
    g_list_free((*infos)->text_options_list);
  }

  if ((*infos)->silence_points)
  {
    g_free((*infos)->silence_points);
    (*infos)->silence_points = NULL;
    (*infos)->number_of_silence_points = 0;
  }

  g_free(*infos);
  *infos = NULL;
}

static void ui_status_free(gui_status **status)
{
  if (!status || !*status)
  {
    return;
  }

  g_free(*status);
  *status = NULL;
}

