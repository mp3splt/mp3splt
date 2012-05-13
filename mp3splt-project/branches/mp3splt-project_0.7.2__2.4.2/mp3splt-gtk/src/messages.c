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
 * The messages history dialog
 *********************************************************/

#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include <libmp3splt/mp3splt.h>

#include "main_win.h"

GtkWidget *mess_history_dialog = NULL;
GtkTextBuffer *mess_hist_buffer = NULL;
GtkTextTagTable *mess_hist_tag_table = NULL;
GtkWidget *mess_hist_view = NULL;
gint debug_is_active = FALSE;

void create_mess_history_dialog();

extern splt_state *the_state;

//! Returns the current local time in form of a string
const char *get_current_time()
{
  time_t cur_time = { 0 };
  static char time_str[128] = { '\0' };
  cur_time = time(NULL);
  const struct tm *tm = localtime(&cur_time);
  strftime(time_str, sizeof(time_str), "(%H:%M:%S) ", tm);

  return time_str;
}

//! Record this message in the message history
void put_message_in_history(const gchar *message, splt_message_type mess_type)
{
  if (mess_type == SPLT_MESSAGE_INFO ||
      (mess_type == SPLT_MESSAGE_DEBUG && debug_is_active))
  {
    GtkTextTag *gray_tag =
      gtk_text_tag_table_lookup(mess_hist_tag_table, "gray_bold");

    GtkTextIter iter;
    const char *current_time = get_current_time();
    gtk_text_buffer_get_end_iter(GTK_TEXT_BUFFER(mess_hist_buffer), &iter);
    gtk_text_buffer_insert_with_tags(GTK_TEXT_BUFFER(mess_hist_buffer),
        &iter, current_time, -1, gray_tag, NULL);

    gtk_text_buffer_insert(GTK_TEXT_BUFFER(mess_hist_buffer), &iter,
        message, -1);
    gtk_text_buffer_insert(GTK_TEXT_BUFFER(mess_hist_buffer), &iter,
        "\n", -1);

    gtk_text_iter_set_line_offset(&iter, 0);
    GtkTextMark *mark = gtk_text_buffer_get_mark(mess_hist_buffer, "end");
    gtk_text_buffer_move_mark(mess_hist_buffer, mark, &iter);
    gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(mess_hist_view), mark);
  }
}

//! Hide the message history dialog
void mess_history_hide(GtkDialog *dialog, gint response_id, gpointer data)
{
  gtk_widget_hide(mess_history_dialog);
}

/*! Hide the message history dialog

Takes less arguments than mess_history_hide. 
\todo Do we really still need both functions doing the same? They
do not seem to use the arguments they differ in at all
 */
void mess_history_hide2(GtkWidget *widget, gpointer data)
{
  mess_history_hide(NULL, 0, NULL);
  //TODO: ugly HACK!
  create_mess_history_dialog();
}

//! Add tags to the message history entry
void add_mess_hist_tags()
{
  GtkTextTag *tag = gtk_text_tag_new("gray_bold");

  GValue fg_val = { 0 };
  g_value_init(&fg_val, G_TYPE_STRING);
  g_value_set_static_string(&fg_val, "gray");
  g_object_set_property(G_OBJECT(tag), "foreground", &fg_val);

  gtk_text_tag_table_add(mess_hist_tag_table, tag);
}

//! The event that is issued if the user clicks on a message tag
void debug_check_event(GtkToggleButton *debug_toggle, gpointer user_data)
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(debug_toggle)))
  {
    debug_is_active = TRUE;
  }
  else
  {
    debug_is_active = FALSE;
  }
}

//! The event issued when the "Clear messages" button is clicked
void clear_messages_event(GtkWidget *widget, gpointer data)
{
  GtkTextIter start_iter;
  GtkTextIter end_iter;
  gtk_text_buffer_get_start_iter(mess_hist_buffer, &start_iter);
  gtk_text_buffer_get_end_iter(mess_hist_buffer, &end_iter);
  gtk_text_buffer_delete(mess_hist_buffer, &start_iter, &end_iter);
}

//! The portion of the message history dialog that contains the messages
GtkWidget *create_text_component()
{
  GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(vbox), 3);

  //text view
  mess_hist_tag_table = gtk_text_tag_table_new();
  add_mess_hist_tags();

  mess_hist_buffer = gtk_text_buffer_new(mess_hist_tag_table);
  GtkTextIter iter;
  gtk_text_buffer_get_end_iter(mess_hist_buffer, &iter);
  gtk_text_buffer_create_mark(mess_hist_buffer, "end", &iter, TRUE);
  mess_hist_view = gtk_text_view_new_with_buffer(mess_hist_buffer);

  gtk_text_view_set_editable(GTK_TEXT_VIEW(mess_hist_view), FALSE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(mess_hist_view), FALSE);
  gtk_text_view_set_left_margin(GTK_TEXT_VIEW(mess_hist_view), 5);

  GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_NONE);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
      GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  gtk_container_add(GTK_CONTAINER(scrolled_window), mess_hist_view);

  //top
  GtkWidget *hbox = gtk_hbox_new(FALSE, 0);

  //debug option
  GtkWidget *debug_check_button =
    gtk_check_button_new_with_mnemonic(_("Enable _debug messages"));
  g_signal_connect(G_OBJECT(debug_check_button), "toggled",
      G_CALLBACK(debug_check_event), NULL);
  gtk_box_pack_start(GTK_BOX(hbox), debug_check_button, FALSE, FALSE, 0);

  //clear button
  GtkWidget *clear_button =
    create_cool_button(GTK_STOCK_CLEAR, _("C_lear"), FALSE);
  g_signal_connect(G_OBJECT(clear_button), "clicked",
      G_CALLBACK(clear_messages_event), NULL);
  gtk_box_pack_end(GTK_BOX(hbox), clear_button, FALSE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 3);
  
  return vbox;
}

//! Create the message history dialog
void create_mess_history_dialog()
{
  mess_history_dialog = gtk_dialog_new_with_buttons(_("Messages history"), NULL,
      GTK_DIALOG_DESTROY_WITH_PARENT, GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE, NULL);

  gtk_window_set_default_size(GTK_WINDOW(mess_history_dialog), 550, 300);

  g_signal_connect_swapped(mess_history_dialog, "response",
    G_CALLBACK(mess_history_hide), NULL);
  g_signal_connect(G_OBJECT(mess_history_dialog), "delete_event",
      G_CALLBACK(mess_history_hide2), NULL);

  gtk_window_set_position(GTK_WINDOW(mess_history_dialog), GTK_WIN_POS_CENTER);

  GtkWidget *text_component = create_text_component();
  GtkWidget *area = gtk_dialog_get_content_area(GTK_DIALOG(mess_history_dialog));
  gtk_box_pack_start(GTK_BOX(area), text_component, TRUE, TRUE, 0);
}

