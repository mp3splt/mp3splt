/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2008 Alexandru Munteanu
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
 * Filename: cddb_cue.c
 *
 * this file is for the cddb and cue file tab,
 * where we choose splitpoints from a cue or cddb file
 *
 *********************************************************/

#include <gtk/gtk.h>
#include <libmp3splt/mp3splt.h>
#include <glib/gi18n.h>

#include "util.h"
#include "player_tab.h"
#include "freedb_tab.h"
#include "main_win.h"
#include "preferences_tab.h"

#include <string.h>

//handle box of this page
GtkWidget *cddb_cue_handle_box;

//the cddb entry
GtkWidget *cddb_entry;
//the browse cddb button
GtkWidget *browse_cddb_button;
//the add splitpoints cddb button
GtkWidget *add_cddb_button;

//the cue entry
GtkWidget *cue_entry;
//the browse cue button
GtkWidget *browse_cue_button;
//the add splitpoints cue button
GtkWidget *add_cue_button;

//if we have selected a correct file
gint cddb_incorrect_selected_file = FALSE;
//if we have selected a correct file
gint cue_incorrect_selected_file = FALSE;

//the state main mp3splt state
extern splt_state *the_state;
extern GtkWidget *output_entry;

//when closing the new window after detaching
void close_cddb_cue_popup_window_event( GtkWidget *window,
                                        gpointer data )
{
  GtkWidget *window_child;

  window_child = gtk_bin_get_child(GTK_BIN(window));

  gtk_widget_reparent(GTK_WIDGET(window_child),
                      GTK_WIDGET(cddb_cue_handle_box));

  gtk_widget_destroy(window);
}

//when we detach the handle
void handle_cddb_cue_detached_event (GtkHandleBox *handlebox,
                                     GtkWidget *widget,
                                     gpointer data)
{
  //new window
  GtkWidget *window;
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_widget_reparent(GTK_WIDGET(widget), GTK_WIDGET(window));

  g_signal_connect (G_OBJECT (window), "delete_event",
                    G_CALLBACK (close_cddb_cue_popup_window_event),
                    NULL);

  gtk_widget_show(GTK_WIDGET(window));
}

//event for the cue file chooser ok button
void cue_file_chooser_ok_event(gchar *fname)
{
  gtk_entry_set_text(GTK_ENTRY(cue_entry), fname);
  gtk_widget_set_sensitive(GTK_WIDGET(add_cue_button),
                           TRUE);
}

//event for the cddb file chooser ok button
void cddb_file_chooser_ok_event(gchar *fname)
{
  gtk_entry_set_text(GTK_ENTRY(cddb_entry), fname);
  gtk_widget_set_sensitive(GTK_WIDGET(add_cddb_button),
                           TRUE);
}

//adds cddb splitpoints from the cddb file
//selected with the cddb_entry
void add_cddb_splitpoints(gpointer *data)
{
  //lock gtk
  gdk_threads_enter();
  
  gtk_widget_set_sensitive(GTK_WIDGET(add_cddb_button),
                           FALSE);
  
  gchar *filename;
  gint err;
  
  filename = 
    (gchar *)gtk_entry_get_text(GTK_ENTRY(cddb_entry));
  
  //we put the output default option
  if(get_checked_output_radio_box())
    {
      mp3splt_set_int_option(the_state, SPLT_OPT_OUTPUT_FILENAMES,
                             SPLT_OUTPUT_DEFAULT);
    }
  else
    {
      mp3splt_set_int_option(the_state, SPLT_OPT_OUTPUT_FILENAMES,
                             SPLT_OUTPUT_FORMAT);
    }
  
  //unlock gtk
  gdk_threads_leave();
  
  //we get the information from the cddb file
  mp3splt_put_cddb_splitpoints_from_file(the_state, filename, &err);
  
  //lock gtk
  gdk_threads_enter();
  
  //here we have in err a possible error from the freedb
  print_status_bar_confirmation(err);
  
  if (err >= 0)
    {
      //1 means cddb split
      update_splitpoints_from_the_state(1);
    }
  
  gtk_widget_set_sensitive(GTK_WIDGET(add_cddb_button),
                           TRUE);
  
  //unlock gtk
  gdk_threads_leave();
}

void update_output_options()
{
  //output options
  if (get_checked_output_radio_box() == 0)
    {
      mp3splt_set_int_option(the_state, SPLT_OPT_OUTPUT_FILENAMES,
                             SPLT_OUTPUT_DEFAULT);
      char *data = (char *)
        gtk_entry_get_text(GTK_ENTRY(output_entry));
      gint error = SPLT_OUTPUT_FORMAT_OK;
      mp3splt_set_oformat(the_state, data, &error);
      print_status_bar_confirmation(error);
    }
  else
    {
      mp3splt_set_int_option(the_state, SPLT_OPT_OUTPUT_FILENAMES,
                             SPLT_OUTPUT_FORMAT);
    }
}

void cddb_add_button_event(GtkWidget *widget,
                           gpointer *data)
{
  update_output_options();
  g_thread_create((GThreadFunc)add_cddb_splitpoints,
                  NULL, TRUE, NULL);
}

//adds cue splitpoints from the cue file
//selected with the cue_entry
void add_cue_splitpoints(gpointer *data)
{
  //lock gtk
  gdk_threads_enter();
  
  gtk_widget_set_sensitive(GTK_WIDGET(add_cue_button),
                           FALSE);

  gchar *filename;
  gint err;
  
  filename = 
    (gchar *)gtk_entry_get_text(GTK_ENTRY(cue_entry));
  
  //we put the output default option
  if(get_checked_output_radio_box())
    {
      mp3splt_set_int_option(the_state, SPLT_OPT_OUTPUT_FILENAMES,
                             SPLT_OUTPUT_DEFAULT);
    }
  else
    {
      mp3splt_set_int_option(the_state, SPLT_OPT_OUTPUT_FILENAMES,
                             SPLT_OUTPUT_FORMAT);
    }
  
  //lock gtk
  gdk_threads_leave();
  
  //we get the information from the cddb file
  mp3splt_put_cue_splitpoints_from_file(the_state,filename, &err);
  
  //lock gtk
  gdk_threads_enter();
  
  if (err >= 0)
    {
      //2 means cue split
      update_splitpoints_from_the_state(2);
    }
  
  //here we have in err a possible error from the freedb
  print_status_bar_confirmation(err);
  
  gtk_widget_set_sensitive(GTK_WIDGET(add_cue_button),
                           TRUE);
  
  //lock gtk
  gdk_threads_leave();
}

void cue_add_button_event(GtkWidget *widget,
                          gpointer *data)
{
  update_output_options();
  g_thread_create((GThreadFunc)add_cue_splitpoints,
                  NULL, TRUE, NULL);
}

//creates the cddb and cue frame
GtkWidget *create_cddb_cue_frame()
{
  //main cddb cue hbox
  GtkWidget *cddb_cue_hbox;
  cddb_cue_hbox = gtk_hbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (cddb_cue_hbox),
                                  5);
  
  /* handle box for detaching */
  cddb_cue_handle_box = gtk_handle_box_new();
  gtk_container_add(GTK_CONTAINER (cddb_cue_handle_box),
                    GTK_WIDGET(cddb_cue_hbox));
  //handle event
  g_signal_connect(cddb_cue_handle_box, "child-detached",
                   G_CALLBACK(handle_cddb_cue_detached_event),
                   NULL);
  
  //vertical box
  GtkWidget *cddb_cue_vbox;
  cddb_cue_vbox = gtk_vbox_new(FALSE,0);
  gtk_box_pack_start (GTK_BOX(cddb_cue_hbox), 
                      cddb_cue_vbox , TRUE, TRUE, 4);
  
  /* begin cddb zone */
  //horizontal box for the label and the 2 buttons
  //(browse cddb entry and add splitpoints)
  GtkWidget *cddb_hbox;
  cddb_hbox = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start (GTK_BOX(cddb_cue_vbox), 
                      cddb_hbox , FALSE, FALSE, 4);
  
  //cddb label
  GtkWidget *label_cddb;
  label_cddb = gtk_label_new((gchar *)_("CDDB file : "));
  gtk_box_pack_start (GTK_BOX(cddb_hbox), label_cddb , 
                      FALSE, FALSE, 0);
  
  //browse cddb button
  browse_cddb_button =
    (GtkWidget *)create_cool_button(GTK_STOCK_FILE,
                                    (gchar *)_("_Browse .cddb file"),
                                    FALSE);
  g_signal_connect (G_OBJECT (browse_cddb_button), "clicked",
                    G_CALLBACK (browse_button_event), 
                    (gpointer *)BROWSE_CDDB_FILE);
  gtk_box_pack_start (GTK_BOX(cddb_hbox),
                      browse_cddb_button, FALSE, FALSE, 10);
  //tooltip for the button
  GtkTooltips *tooltip;
  tooltip = gtk_tooltips_new();
  gtk_tooltips_set_tip(tooltip, 
                       browse_cddb_button,
                       (gchar *)_("select CDDB file"),"");
  
  //add cddb splitpoints button
  add_cddb_button =
    (GtkWidget *)create_cool_button(GTK_STOCK_ADD,
                                    (gchar *)_("_Add cddb splitpoints"),
                                    FALSE);
  gtk_widget_set_sensitive(GTK_WIDGET(add_cddb_button), FALSE);
  g_signal_connect (G_OBJECT (add_cddb_button), "clicked",
                    G_CALLBACK (cddb_add_button_event), 
                    NULL);
  gtk_box_pack_start (GTK_BOX(cddb_hbox),
                      add_cddb_button, FALSE, FALSE, 10);
  //tooltip for the button
  gtk_tooltips_set_tip(tooltip, 
                       add_cddb_button,
                       (gchar *)_("set cddb splitpoints to the "
                                  "splitpoints table"),"");
  
  //cddb entry
  cddb_entry = gtk_entry_new();
  gtk_entry_set_editable (GTK_ENTRY (cddb_entry), FALSE);
  gtk_box_pack_start (GTK_BOX(cddb_cue_vbox), 
                      cddb_entry , FALSE, FALSE, 3);
  
  /* separator */
  GtkWidget *separator;
  separator = gtk_hseparator_new();
  gtk_box_pack_start (GTK_BOX(cddb_cue_vbox), 
                      separator , FALSE, FALSE, 15);

  /* begin cue zone */
  //horizontal box for the label and the 2 buttons
  //(browse cddb entry and add splitpoints)
  GtkWidget *cue_hbox;
  cue_hbox = gtk_hbox_new(FALSE,0);
  gtk_box_pack_start (GTK_BOX(cddb_cue_vbox), 
                      cue_hbox , FALSE, FALSE, 4);
  
  //cue label
  GtkWidget *label_cue;
  label_cue = gtk_label_new((gchar *)_("CUE file : "));
  gtk_box_pack_start (GTK_BOX(cue_hbox), label_cue , 
                      FALSE, FALSE, 0);
  
  //browse cue button
  browse_cue_button =
    (GtkWidget *)create_cool_button(GTK_STOCK_FILE,
                                    (gchar *)_("_Browse .cue file"),
                                    FALSE);
  g_signal_connect (G_OBJECT (browse_cue_button), "clicked",
                    G_CALLBACK (browse_button_event), 
                    (gpointer *)BROWSE_CUE_FILE);
  gtk_box_pack_start (GTK_BOX(cue_hbox),
                      browse_cue_button, FALSE, FALSE, 10);
  //tooltip for the button
  gtk_tooltips_set_tip(tooltip, 
                       browse_cue_button,
                       (gchar *)_("select CUE file"),"");
  
  //add cue splitpoints button
  add_cue_button =
    (GtkWidget *)create_cool_button(GTK_STOCK_ADD,
                                    (gchar *)_("_Add cue splitpoints"),
                                    FALSE);
  gtk_widget_set_sensitive(GTK_WIDGET(add_cue_button), FALSE);
  g_signal_connect (G_OBJECT (add_cue_button), "clicked",
                    G_CALLBACK (cue_add_button_event), 
                    NULL);
  gtk_box_pack_start (GTK_BOX(cue_hbox),
                      add_cue_button, FALSE, FALSE, 10);
  //tooltip for the button
  gtk_tooltips_set_tip(tooltip, 
                       add_cue_button,
                       (gchar *)_("set cue splitpoints to the "
                                  "splitpoints table"),"");
  
  //cue entry
  cue_entry = gtk_entry_new();
  gtk_entry_set_editable (GTK_ENTRY (cue_entry), FALSE);
  gtk_box_pack_start (GTK_BOX(cddb_cue_vbox), 
                      cue_entry , FALSE, FALSE, 3);
  
  return cddb_cue_handle_box;
}
