/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2013 Alexandru Munteanu
 * Contact: m@ioalex.net
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 *
 *********************************************************/

#ifndef LIBMP3SPLT_MANAGER_H

#define LIBMP3SPLT_MANAGER_H

#include "all_includes.h"

void lmanager_init_and_find_plugins(ui_state *ui);
void lmanager_put_split_filename(const char *filename, void *data);
void lmanager_stop_split(ui_state *ui);

typedef struct {
  ui_state *ui;
  int progress_type;
  char *filename_shorted;
  int current_split;
  int max_splits;
  int silence_found_tracks;
  float silence_db_level;
  float percent_progress;
} ui_with_p_bar;

typedef struct {
  ui_state *ui;
  char *message;
  splt_message_type mess_type;
} ui_with_message;

#endif

