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

#ifndef DRAG_AND_DROP_H

#define DRAG_AND_DROP_H

#include "all_includes.h"

typedef enum {
  DND_SINGLE_MODE_AUDIO_FILE,
  DND_BATCH_MODE_AUDIO_FILES,
  DND_DATA_FILES,
  DND_SINGLE_MODE_AUDIO_FILE_AND_DATA_FILES,
} drop_type;

enum {
  DROP_PLAIN,
  DROP_STRING,
  DROP_URI_LIST
};

static const GtkTargetEntry drop_types[] = {
  { "text/plain", 0, DROP_PLAIN },
  { "STRING", 0, DROP_STRING },
  { "text/uri-list", 0, DROP_URI_LIST }
};

void dnd_add_drag_data_received_to_widget(GtkWidget *widget, drop_type type, ui_state *ui);

#endif

