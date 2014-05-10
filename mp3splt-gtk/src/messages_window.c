/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2014 Alexandru Munteanu
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

/*!********************************************************
 * \file 
 * The messages history dialog
 *********************************************************/

#include "messages_window.h"

//! Returns the current local time in form of a string
static const char *get_current_system_time()
{
  time_t cur_time = time(NULL);
  const struct tm *tm = localtime(&cur_time);

  static char time_str[128] = { '\0' };
  strftime(time_str, sizeof(time_str), "(%H:%M:%S) ", tm);

  return time_str;
}

//! Record this message in the message history
void put_message_in_history(const gchar *message, splt_message_type mess_type, ui_state *ui)
{
  gui_state *gui = ui->gui;

  if (mess_type == SPLT_MESSAGE_INFO || mess_type == SPLT_MESSAGE_WARNING ||
      (mess_type == SPLT_MESSAGE_DEBUG && ui->infos->debug_is_active))
  {
    GtkTextTag *gray_tag = gtk_text_tag_table_lookup(gui->mess_hist_tag_table, "gray_bold");

    GtkTextIter iter;
    gtk_text_buffer_get_end_iter(ui->gui->mess_hist_buffer, &iter);
    const char *current_system_time = get_current_system_time();
    gtk_text_buffer_insert_with_tags(ui->gui->mess_hist_buffer,
        &iter, current_system_time, -1, gray_tag, NULL);

    gtk_text_buffer_insert(ui->gui->mess_hist_buffer, &iter, message, -1);
    gtk_text_buffer_insert(ui->gui->mess_hist_buffer, &iter, "\n", -1);

    gtk_text_iter_set_line_offset(&iter, 0);

    GtkTextMark *mark = gtk_text_buffer_get_mark(ui->gui->mess_hist_buffer, "end");
    gtk_text_buffer_move_mark(ui->gui->mess_hist_buffer, mark, &iter);
    gtk_text_view_scroll_mark_onscreen(GTK_TEXT_VIEW(ui->gui->mess_hist_view), mark);
  }
}

//! The event that is issued if the user clicks on a message tag
static void debug_check_event(GtkToggleButton *debug_toggle, ui_state *ui)
{
  if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(debug_toggle)))
  {
    ui->infos->debug_is_active = TRUE;
    return;
  }

  ui->infos->debug_is_active = FALSE;
}

//! The event issued when the "Clear messages" button is clicked
static void clear_messages_event(GtkWidget *widget, ui_state *ui)
{
  GtkTextIter start_iter;
  gtk_text_buffer_get_start_iter(ui->gui->mess_hist_buffer, &start_iter);
  GtkTextIter end_iter;
  gtk_text_buffer_get_end_iter(ui->gui->mess_hist_buffer, &end_iter);
  gtk_text_buffer_delete(ui->gui->mess_hist_buffer, &start_iter, &end_iter);
}

//! Add tags to the message history entry
static void add_mess_hist_tags(GtkTextTagTable *mess_hist_tag_table)
{
  GtkTextTag *tag = gtk_text_tag_new("gray_bold");

  GValue fg_val = { 0 };
  g_value_init(&fg_val, G_TYPE_STRING);
  g_value_set_static_string(&fg_val, "gray");
  g_object_set_property(G_OBJECT(tag), "foreground", &fg_val);

  gtk_text_tag_table_add(mess_hist_tag_table, tag);
}

//! The portion of the message history dialog that contains the messages
static GtkWidget *create_text_component(ui_state *ui)
{
  GtkWidget *vbox = wh_vbox_new();
  gtk_container_set_border_width(GTK_CONTAINER(vbox), 3);

  //text view
  GtkTextTagTable *mess_hist_tag_table = gtk_text_tag_table_new();
  ui->gui->mess_hist_tag_table = mess_hist_tag_table;
  add_mess_hist_tags(mess_hist_tag_table);

  GtkTextBuffer *mess_hist_buffer = gtk_text_buffer_new(mess_hist_tag_table);
  ui->gui->mess_hist_buffer = mess_hist_buffer;

  GtkTextIter iter;
  gtk_text_buffer_get_end_iter(ui->gui->mess_hist_buffer, &iter);
  gtk_text_buffer_create_mark(ui->gui->mess_hist_buffer, "end", &iter, TRUE);
  GtkWidget *mess_hist_view = gtk_text_view_new_with_buffer(ui->gui->mess_hist_buffer);
  ui->gui->mess_hist_view = mess_hist_view;

  gtk_text_view_set_editable(GTK_TEXT_VIEW(mess_hist_view), FALSE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(mess_hist_view), FALSE);
  gtk_text_view_set_left_margin(GTK_TEXT_VIEW(mess_hist_view), 5);

  GtkWidget *scrolled_window = gtk_scrolled_window_new(NULL, NULL);
  gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_NONE);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled_window),
      GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  gtk_container_add(GTK_CONTAINER(scrolled_window), mess_hist_view);

  GtkWidget *hbox = wh_hbox_new();

  //debug option
  GtkWidget *debug_check_button =
    gtk_check_button_new_with_mnemonic(_("Enable _debug messages"));
  g_signal_connect(G_OBJECT(debug_check_button), "toggled", G_CALLBACK(debug_check_event), ui);
  gtk_box_pack_start(GTK_BOX(hbox), debug_check_button, FALSE, FALSE, 0);

  //clear button
  GtkWidget *clear_button = wh_create_cool_button("edit-clear", _("C_lear"), FALSE);
  g_signal_connect(G_OBJECT(clear_button), "clicked", G_CALLBACK(clear_messages_event), ui);
  gtk_box_pack_end(GTK_BOX(hbox), clear_button, FALSE, FALSE, 0);

  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 3);

  return vbox;
}

//! Create the message history dialog
void create_mess_history_window(ui_state *ui)
{
  GtkWidget *text_component = create_text_component(ui);
  ui->gui->mess_history_window = 
    wh_create_window_with_close_button(_("Messages history"), 550, 300,
        GTK_WIN_POS_CENTER, GTK_WINDOW(ui->gui->window),
        text_component, NULL);
}

