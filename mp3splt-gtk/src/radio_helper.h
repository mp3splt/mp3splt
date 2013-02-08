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

#ifndef RADIO_HELPER_H

#define RADIO_HELPER_H

#include "external_includes.h"

GtkWidget *rh_append_radio_to_vbox(GtkWidget *radio_button, const gchar *text,
    gint value, void (*callback)(GtkToggleButton *, gpointer),
    gpointer callback_data, GtkWidget *vbox);
gint rh_get_active_value(GtkWidget *radio_button);
void rh_set_radio_value(GtkWidget *radio_button, gint key_value, gboolean value);

#endif

