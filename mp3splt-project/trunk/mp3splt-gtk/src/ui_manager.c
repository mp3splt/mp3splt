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
static gui_status *ui_status_new();
static gui_state *ui_gui_new();

static void ui_main_window_free(ui_main_window **main_win);
static void ui_infos_free(ui_infos **infos);
static void ui_status_free(gui_status **status);
static void ui_gui_free(gui_state **gui);

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

  ui->status = ui_status_new();
  ui->gui = ui_gui_new();

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

  ui_status_free(&ui->status);
  ui_gui_free(&ui->gui);

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

  infos->player_seconds = 0;
  infos->player_minutes = 0;
  infos->player_hundr_secs = 0;
  infos->player_seconds2 = 0;
  infos->player_minutes2 = 0;
  infos->player_hundr_secs2 = 0;

  infos->total_time = 0;
  infos->current_time = 0;

  infos->splitnumber = 0;
  infos->width_drawing_area = 0;
  infos->zoom_coeff = 2.0;
  infos->zoom_coeff_old = 2.0;

  infos->hundr_secs_th = 20;
  infos->tens_of_secs_th = 3 * 100;
  infos->secs_th = 40 * 100;
  infos->ten_secs_th = 3 * 6000;
  infos->minutes_th = 20 * 6000;
  infos->ten_minutes_th = 3 * 3600 * 100;

  infos->one_minute_time = 1 * 6000;
  infos->three_minutes_time = 3 * 6000;
  infos->six_minutes_time = 6 * 6000;
  infos->ten_minutes_time = 10 * 6000;
  infos->twenty_minutes_time = 20 * 6000;
  infos->fourty_minutes_time = 40 * 6000;

  GArray *preview_time_windows = g_array_new(TRUE, TRUE, sizeof(gint));
  g_array_append_val(preview_time_windows, infos->one_minute_time);
  g_array_append_val(preview_time_windows, infos->three_minutes_time);
  g_array_append_val(preview_time_windows, infos->six_minutes_time);
  g_array_append_val(preview_time_windows, infos->ten_minutes_time);
  g_array_append_val(preview_time_windows, infos->twenty_minutes_time);
  g_array_append_val(preview_time_windows, infos->fourty_minutes_time);
  infos->preview_time_windows = preview_time_windows;

  infos->filtered_points_presence = NULL;
  infos->silence_wave_number_of_points_threshold = DEFAULT_SILENCE_WAVE_NUMBER_OF_POINTS_THRESHOLD;

  ui->infos = infos;
}

static gui_status *ui_status_new(ui_state *ui)
{
  gui_status *status = g_malloc0(sizeof(gui_status));

  status->splitting = FALSE;
  status->quit_main_program = FALSE;
  status->mouse_on_progress_bar = FALSE;

  status->currently_compute_douglas_peucker_filters = FALSE;
  status->show_silence_wave = FALSE;

  status->playing = FALSE;
  status->timer_active = FALSE;
  status->quick_preview_end_splitpoint = -1;
  status->preview_start_splitpoint = -1;

  status->move_time = 0;

  status->button1_pressed = FALSE;
  status->button2_pressed = FALSE;

  status->quick_preview = FALSE;

  status->button_x = 0;
  status->button_x2 = 0;
  status->button_y = 0;
  status->button_y2 = 0;

  status->move_splitpoints = FALSE;
  status->splitpoint_to_move = -1;
  status->remove_splitpoints = FALSE;
  status->select_splitpoints = FALSE;
  status->check_splitpoint = FALSE;

  status->first_splitpoint_selected = -1;

  return status;
}

static gui_state *ui_gui_new()
{
  gui_state *gui = g_malloc0(sizeof(gui_state));

  gui->margin = 4;
  gui->real_erase_split_length = 12;
  gui->real_move_split_length = 16;
  gui->real_checkbox_length = 12;
  gui->real_wave_length = 96;

  return gui;
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

  g_array_free((*infos)->preview_time_windows, TRUE);

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

static void ui_gui_free(gui_state **gui)
{
  if (!gui|| !*gui)
  {
    return;
  }

  g_free(*gui);
  *gui = NULL;
}

