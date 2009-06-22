/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2009 Alexandru Munteanu
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

/**********************************************************
 * Filename: special_split.c
 *
 * this file is for the special split tab, where we have special
 * splits 
 *
 *********************************************************/

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <libmp3splt/mp3splt.h>

#include "special_split.h"
#include "preferences_tab.h"
#include "main_win.h"
#include "multiple_files.h"

GtkWidget *time_label = NULL;
GtkWidget *spinner_time = NULL;

gint selected_split_mode = SELECTED_SPLIT_NORMAL;
gint split_file_mode = FILE_MODE_SINGLE;

GtkWidget *split_mode_radio_button = NULL;
GtkWidget *file_mode_radio_button = NULL;

GtkWidget *multiple_files_component = NULL;

//returns the selected split mode
gint get_selected_split_mode(GtkToggleButton *radio_b)
{
  //get the radio buttons
  GSList *radio_button_list = NULL;
  radio_button_list = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio_b));
  //we check which bubble is checked
  GtkToggleButton *test = NULL;
  gint i = 0,selected = SELECTED_SPLIT_NORMAL;
  //for each split mode, get the selected one
  for(i = 0; i < NUMBER_OF_SPLIT_MODES;i++)
    {
      test = (GtkToggleButton *) g_slist_nth_data(radio_button_list,i);
      if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(test)))
        selected = i;
    }
  
  return selected;
}

//when the split mode selection changed
void split_mode_changed(GtkToggleButton *radio_b, gpointer data)
{
  selected_split_mode = get_selected_split_mode(radio_b);

  if ((selected_split_mode == SELECTED_SPLIT_NORMAL)
      || (selected_split_mode == SELECTED_SPLIT_WRAP)
      || (selected_split_mode == SELECTED_SPLIT_ERROR))
  {
    gtk_widget_set_sensitive(GTK_WIDGET(spinner_time), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(time_label), FALSE);
  }

  if (selected_split_mode == SELECTED_SPLIT_TIME)
  {
    gtk_widget_set_sensitive(GTK_WIDGET(spinner_time), TRUE);
    gtk_widget_set_sensitive(GTK_WIDGET(time_label), TRUE);
  }

  save_preferences(NULL, NULL);
}

//sets the default split modes
void set_default_split_modes (GtkWidget *widget, 
                              gpointer data)
{
  GtkToggleButton *radio_b= GTK_TOGGLE_BUTTON(data);
  //we activate the normal split button
  GSList *radio_button_list;
  radio_button_list = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio_b));
  GtkToggleButton *test = (GtkToggleButton *)
    g_slist_nth_data(radio_button_list, SELECTED_SPLIT_NORMAL);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(test),TRUE);  
}

static void spinner_time_changed(GtkSpinButton *spinner, gpointer data)
{
  gint time = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spinner));
  gchar time_text[1024] = { '\0' };
  g_snprintf(time_text, 1024, _("\tSplit every %2d seconds."), time);
  gtk_label_set_text(GTK_LABEL(time_label), time_text);

  save_preferences(NULL, NULL);
}

static void split_file_mode_changed(GtkToggleButton *radio_b, gpointer data)
{
  GSList *radio_button_list =
    gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio_b));
  GtkToggleButton *current_radio_button = (GtkToggleButton *)
    g_slist_nth_data(radio_button_list,0);

  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(current_radio_button)))
  {
    split_file_mode = FILE_MODE_MULTIPLE;
    gtk_widget_set_sensitive(multiple_files_component, TRUE);
  }
  else
  {
    split_file_mode = FILE_MODE_SINGLE;
    gtk_widget_set_sensitive(multiple_files_component, FALSE);
  }

  save_preferences(NULL, NULL);
}

static GtkWidget *create_split_mode()
{
  GtkWidget *local_vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(local_vbox), 5);

  //normal split
  split_mode_radio_button = gtk_radio_button_new_with_label(NULL, _("Normal"));
  gtk_box_pack_start (GTK_BOX (local_vbox), split_mode_radio_button,
      FALSE, FALSE, 2);
  g_signal_connect(GTK_TOGGLE_BUTTON(split_mode_radio_button),
      "toggled", G_CALLBACK (split_mode_changed), NULL);
  
  //time split
  split_mode_radio_button = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(split_mode_radio_button), _("Time"));
  gtk_box_pack_start(GTK_BOX(local_vbox), split_mode_radio_button,
      FALSE, FALSE, 2);
  g_signal_connect(GTK_TOGGLE_BUTTON(split_mode_radio_button), "toggled",
      G_CALLBACK(split_mode_changed), NULL);
  
  //time split
  GtkWidget *horiz_fake = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start(GTK_BOX(local_vbox), horiz_fake, FALSE, FALSE, 0);
  
  gint default_time = 60;

  gchar time_text[1024] = { '\0' };
  g_snprintf(time_text, 1024, _("\tSplit every %2d seconds."), default_time);
  time_label = gtk_label_new(time_text);
  gtk_box_pack_start(GTK_BOX(horiz_fake), time_label, FALSE, FALSE, 0);
  
  GtkAdjustment *adj =
    (GtkAdjustment *)gtk_adjustment_new(0.0, 1, 2000, 1.0, 10.0, 0.0);
  spinner_time = gtk_spin_button_new(adj, 1, 0);
  gtk_box_pack_start(GTK_BOX(horiz_fake), spinner_time, FALSE, FALSE, 6);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_time), default_time);
  gtk_widget_set_sensitive(GTK_WIDGET(spinner_time), FALSE);
  gtk_widget_set_sensitive(time_label, FALSE);
  g_signal_connect(G_OBJECT(spinner_time), "value-changed",
      G_CALLBACK(spinner_time_changed), NULL);

  //wrap split
  split_mode_radio_button = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(split_mode_radio_button),
     _("Wrap - split files created with mp3wrap or albumwrap (mp3 only)"));
  gtk_box_pack_start(GTK_BOX(local_vbox), split_mode_radio_button,
      FALSE, FALSE, 2);
  g_signal_connect(GTK_TOGGLE_BUTTON(split_mode_radio_button), "toggled",
                    G_CALLBACK(split_mode_changed), NULL);
  
  //error mode split
  split_mode_radio_button = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(split_mode_radio_button), _("Error mode (mp3 only)"));
  gtk_box_pack_start(GTK_BOX(local_vbox), split_mode_radio_button,
      FALSE, FALSE, 2);
  g_signal_connect(GTK_TOGGLE_BUTTON(split_mode_radio_button), "toggled",
      G_CALLBACK(split_mode_changed), NULL);
  
  //put default values
  set_default_split_modes(NULL, split_mode_radio_button);

  GtkWidget *scrolled_window = create_scrolled_window();
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), 
                                        GTK_WIDGET(local_vbox));
 
  return scrolled_window;
}

static GtkWidget *create_single_multiple_split_modes()
{
  GtkWidget *local_vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(local_vbox), 5);

  //single file
  file_mode_radio_button = 
    gtk_radio_button_new_with_label(NULL, _("Single file"));
  gtk_box_pack_start(GTK_BOX(local_vbox), file_mode_radio_button,
      FALSE, FALSE, 2);
  g_signal_connect(GTK_TOGGLE_BUTTON(file_mode_radio_button),
      "toggled", G_CALLBACK(split_file_mode_changed), NULL);
 
  //multiple files
  file_mode_radio_button = gtk_radio_button_new_with_label_from_widget
    (GTK_RADIO_BUTTON(file_mode_radio_button), _("Multiple files"));
  gtk_box_pack_start(GTK_BOX(local_vbox), file_mode_radio_button,
      FALSE, FALSE, 2);
  g_signal_connect(GTK_TOGGLE_BUTTON(file_mode_radio_button),
      "toggled", G_CALLBACK(split_file_mode_changed), NULL);

  GtkWidget *multiple_files_hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(local_vbox), multiple_files_hbox, TRUE, TRUE, 2);

  multiple_files_component = create_multiple_files_component();
  gtk_widget_set_sensitive(multiple_files_component, FALSE);
  gtk_box_pack_start(GTK_BOX(multiple_files_hbox), multiple_files_component,
      TRUE, TRUE, 20);

  GtkWidget *scrolled_window = create_scrolled_window();
  gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrolled_window), 
                                        GTK_WIDGET(local_vbox));

  return scrolled_window;
}

//creates the special split page
GtkWidget *create_special_split_page()
{
  GtkWidget *vbox = gtk_vbox_new(FALSE, 0);;

  /* tabbed notebook */
  GtkWidget *notebook = gtk_notebook_new();
  gtk_box_pack_start(GTK_BOX(vbox), notebook, TRUE, TRUE, 0);
  gtk_notebook_popup_enable(GTK_NOTEBOOK(notebook));
  gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), TRUE);
  gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), FALSE);
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);

  GtkWidget *notebook_label = gtk_label_new(_("Split mode"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), create_split_mode(),
                           (GtkWidget *)notebook_label);

  notebook_label = gtk_label_new(_("File mode"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
      create_single_multiple_split_modes(), (GtkWidget *)notebook_label);

  return vbox;
}

