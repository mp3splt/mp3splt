/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2011 Alexandru Munteanu
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

#ifndef RADIO_HELPER_H

#include <gtk/gtk.h>

GtkWidget *rh_append_radio_to_vbox(GtkWidget *radio_button, const gchar *text,
    gint value,
    void (*callback)(GtkToggleButton *, gpointer),
    GtkWidget *vbox);
gint rh_get_active_value(GtkWidget *radio_button);
GtkWidget *rh_get_radio_from_value(GtkWidget *radio_button, gint value);

#define RADIO_HELPER_H
#endif

