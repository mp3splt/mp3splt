/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2012 Alexandru Munteanu
 * Contact: io_fx@yahoo.fr
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

#ifndef _UI_MANAGER_H

#define UI_DEFAULT_WIDTH 550
#define UI_DEFAULT_HEIGHT 420

typedef struct {
  gint root_x_pos;
  gint root_y_pos;
  gint width;
  gint height;
} ui_main_window;

typedef struct {
  gchar *browser_directory;
  ui_main_window *main_win;
} ui_infos;

typedef struct {
  ui_infos *infos;
} ui_state;

ui_state *ui_state_new();
void ui_state_free(ui_state *ui);

void ui_set_browser_directory(ui_state *ui, const gchar *directory);
const gchar *ui_get_browser_directory(ui_state *ui);

void ui_set_main_win_position(ui_state *ui, gint x, gint y);
void ui_set_main_win_size(ui_state *ui, gint width, gint height);
const ui_main_window *ui_get_main_window_infos(ui_state *ui);

#define _UI_MANAGER_H
#endif

