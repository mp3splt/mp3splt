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

#ifndef UI_MANAGER_H

#define UI_MANAGER_H

#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <errno.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <locale.h>
#include <ctype.h>

#include <sys/stat.h>
#include <sys/types.h>

#ifdef __WIN32__

#include <windows.h>
#include <shlwapi.h>
#include <winsock2.h>
#define usleep(x) Sleep(x/1000)

#else

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

#endif

#include <gtk/gtk.h>

#include <glib.h>
#include <glib/gi18n.h>
#include <glib/gstdio.h>
#include <glib/gprintf.h>

#include <gdk/gdkkeysyms.h>

#include <libmp3splt/mp3splt.h>

#include "preferences_manager.h"

#define UI_DEFAULT_WIDTH 550
#define UI_DEFAULT_HEIGHT 420

typedef struct
{
  gboolean checked;
  gint mins;
  gint secs;
  gint hundr_secs;
} Split_point;

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
  preferences_state *preferences;
  splt_state *mp3splt_state;
  GArray *splitpoints;
} ui_state;

#include "main_win.h"
#include "player_tab.h"
#include "tree_tab.h"
#include "widgets_helper.h"
#include "preferences_tab.h"
#include "utilities.h"
#include "mp3splt-gtk.h"
#include "player.h"
#include "freedb_tab.h"
#include "import.h"
#include "options_manager.h"
#include "export.h"
#include "split_files.h"
#include "special_split.h"
#include "messages.h"
#include "snackamp_control.h"
#include "xmms_control.h"
#include "gstreamer_control.h"
#include "multiple_files.h"
#include "libmp3splt_manager.h"
#include "combo_helper.h"
#include "radio_helper.h"
#include "drawing_helper.h"
#include "douglas_peucker.h"

ui_state *ui_state_new();
void ui_state_free(ui_state *ui);

void ui_set_browser_directory(ui_state *ui, const gchar *directory);
const gchar *ui_get_browser_directory(ui_state *ui);

void ui_set_main_win_position(ui_state *ui, gint x, gint y);
void ui_set_main_win_size(ui_state *ui, gint width, gint height);
const ui_main_window *ui_get_main_window_infos(ui_state *ui);

void ui_register_spinner_int_preference(gchar *main_key, gchar *second_key,
    gint default_value, GtkWidget *spinner,
    void (*update_spinner_value_cb)(GtkWidget *spinner, gpointer data),
    gpointer user_data_for_cb, ui_state *ui);

void ui_register_range_preference(gchar *main_key, gchar *second_key,
    gint default_value, GtkWidget *range,
    void (*update_adjustment_value)(GtkAdjustment *adjustment, gpointer user_data),
    gpointer user_data_for_cb, ui_state *ui);

void ui_load_preferences(GKeyFile *key_file, ui_state *ui);
void ui_save_preferences(GKeyFile *key_file, ui_state *ui);
void ui_write_default_preferences(GKeyFile *key_file, ui_state *ui);

void ui_fail(ui_state *ui, const gchar *message, ...);

#endif

