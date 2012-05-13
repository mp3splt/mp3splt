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

/*!********************************************************
 * \file 
 *
 * this file contains the code for the radio button helpers.
 ********************************************************/

#include "radio_helper.h"

GtkWidget *rh_append_radio_to_vbox(GtkWidget *radio_button, const gchar *text,
    gint value,
    void (*callback)(GtkToggleButton *, gpointer),
    GtkWidget *vbox)
{
  GtkWidget *new_radio_button =
    gtk_radio_button_new_with_label_from_widget(GTK_RADIO_BUTTON(radio_button), text);
  gtk_box_pack_start(GTK_BOX(vbox), new_radio_button, FALSE, FALSE, 0);

  if (callback)
  {
    g_signal_connect(GTK_TOGGLE_BUTTON(new_radio_button), "toggled", G_CALLBACK(callback),
        NULL);
  }

  g_object_set_data(G_OBJECT(new_radio_button), "value", GINT_TO_POINTER(value));

  return new_radio_button;
}

gint rh_get_active_value(GtkWidget *radio_button)
{
  gint active_value = -1;
  GSList *radio_button_list = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio_button));

  gint i = 0;
  for(i = 0; i < g_slist_length(radio_button_list);i++)
  {
    GtkRadioButton *current_radio = (GtkRadioButton *) g_slist_nth_data(radio_button_list, i);
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(current_radio)))
    {
      active_value = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(current_radio), "value"));
      break;
    }
  }
  
  return active_value;
}

GtkWidget *rh_get_radio_from_value(GtkWidget *radio_button, gint value)
{
  GtkWidget *radio = NULL;
  GSList *radio_button_list = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio_button));

  gint i = 0;
  for(i = 0; i < g_slist_length(radio_button_list);i++)
  {
    GtkRadioButton *current_radio = (GtkRadioButton *) g_slist_nth_data(radio_button_list, i);
    gint current_value = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(current_radio), "value"));
    if (current_value == value)
    {
      radio = GTK_WIDGET(current_radio);
      break;
    }
  }

  return radio;
}

