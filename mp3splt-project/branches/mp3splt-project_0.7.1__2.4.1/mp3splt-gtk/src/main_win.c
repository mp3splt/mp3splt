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

/*!********************************************************
 * \file 
 * The main window
 *
 * main file that initialises the menubar, the toolbar, 
 * the tabs, about window, status error messages
 *
 *********************************************************/

//we include the "config.h" file from the config options
#ifdef HAVE_CONFIG_H
#include "config.h"
#else
#define VERSION "0.7.1"
#define PACKAGE_NAME "mp3splt-gtk"
#endif

#include <string.h>
#include "export.h"

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <libmp3splt/mp3splt.h>
#include <gdk/gdkkeysyms.h>

#ifdef __WIN32__
#include <windows.h>
#include <shlwapi.h>
#endif


#include "util.h"
#include "main_win.h"
#include "mp3splt-gtk.h"
#include "tree_tab.h"
#include "split_files.h"
#include "utilities.h"
#include "preferences_tab.h"
#include "freedb_tab.h"
#include "special_split.h"
#include "utilities.h"
#include "player_tab.h"
#include "player.h"
#include "messages.h"
#include "import.h"
#include "preferences_manager.h"
#include "player_tab.h"

#include "ui_manager.h"

//main window
GtkWidget *window = NULL;
GtkAccelGroup *window_accel_group = NULL;

//status bar
GtkWidget *status_bar;

//if we are on the preferences tab, then TRUE
gint preferences_tab = FALSE;

//player box
GtkWidget *player_box;

//the split freedb button
GtkWidget *split_freedb_button;

//new window for the progress bar
GtkWidget *percent_progress_bar;

//filename and path for the file to split
gchar *filename_to_split;
gchar *filename_path_of_split;

//if we are currently splitting
gint we_are_splitting = FALSE;
gint we_quit_main_program = FALSE;

GtkWidget *player_vbox = NULL;

//stop button to cancel the split
GtkWidget *cancel_button = NULL;

extern GtkWidget *mess_history_dialog;

extern GtkWidget *da;
extern GtkWidget *progress_bar;

extern GArray *splitpoints;
extern gint selected_id;
extern splt_state *the_state;
extern splt_freedb_results *search_results;
extern GList *player_pref_list;
extern GList *text_options_list;
extern gchar **split_files;
extern gint max_split_files;
extern gint selected_player;
extern silence_wave *silence_points;
extern gint number_of_silence_points;

extern ui_state *ui;

GtkWidget *playlist_box = NULL;

//close the window and exit button function
void quit(GtkWidget *widget, gpointer   data)
{
  save_preferences(NULL, NULL);

  if (we_are_splitting)
  {
    gint err = SPLT_OK;
    mp3splt_stop_split(the_state,&err);
    print_status_bar_confirmation(err);

    we_quit_main_program = TRUE;
    put_status_message(_(" info: stopping the split process before exiting"));
  }

  //quit the player: currently closes gstreamer
  if (player_is_running())
  {
    player_quit();
  }

  g_list_free(player_pref_list);
  g_list_free(text_options_list);
  g_array_free(splitpoints, TRUE);

  if (silence_points)
  {
    g_free(silence_points);
    silence_points = NULL;
    number_of_silence_points = 0;
  }

  gtk_main_quit();
}

void main_window_drag_data_received(GtkWidget *window,
    GdkDragContext *drag_context, gint x, gint y, GtkSelectionData *data, guint
    info, guint time, gpointer user_data)
{
  const gchar *received_data = (gchar *) gtk_selection_data_get_text(data);

  if (received_data != NULL)
  {
    gchar **drop_filenames = NULL;
    drop_filenames = g_strsplit(received_data, "\n", 0);

    gint current_index = 0;
    gchar *current_filename = drop_filenames[current_index];
    while (current_filename != NULL)
    {
      gchar *filename = NULL;
      if (strstr(current_filename, "file:") == current_filename)
      {
        filename = g_filename_from_uri(current_filename, NULL, NULL);
      }
      else
      {
        gint fname_malloc_size = strlen(current_filename) + 1;
        filename = g_malloc(sizeof(gchar) * fname_malloc_size);
        g_snprintf(filename, fname_malloc_size, "%s", current_filename);
      }

      remove_end_slash_n_r_from_filename(filename);

      if (is_filee(filename))
      {
        handle_import(filename);
      }

      if (filename)
      {
        g_free(filename);
        filename = NULL;
      }

      g_free(current_filename);
      current_index++;
      current_filename = drop_filenames[current_index];
    }

    if (drop_filenames)
    {
      g_free(drop_filenames);
      drop_filenames = NULL;
    }
  }
}

gboolean configure_window_callback(GtkWindow *window, GdkEvent *event, gpointer data)
{
  ui_state *ui = (ui_state *)data;

  ui_set_main_win_position(ui, event->configure.x, event->configure.y); 
  ui_set_main_win_size(ui, event->configure.width, event->configure.height);

  return FALSE;
}

void initialize_window()
{
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  g_signal_connect(G_OBJECT(window), "configure-event", G_CALLBACK(configure_window_callback), ui);

  window_accel_group = gtk_accel_group_new();
  gtk_window_add_accel_group(GTK_WINDOW(window), window_accel_group);
 
  gtk_window_set_title(GTK_WINDOW(window), PACKAGE_NAME" "VERSION);
  gtk_container_set_border_width (GTK_CONTAINER (window), 0);

  g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(quit), NULL);
  g_signal_connect(G_OBJECT(window), "drag-data-received",
      G_CALLBACK(main_window_drag_data_received), NULL);
  gtk_drag_dest_set(window, GTK_DEST_DEFAULT_MOTION | GTK_DEST_DEFAULT_DROP,
      drop_types, 3, GDK_ACTION_COPY | GDK_ACTION_MOVE);
 
  GString *Imagefile = g_string_new(PIXMAP_PATH);
  build_svg_path(Imagefile, "mp3splt-gtk_ico"ICON_EXT);
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file(Imagefile->str, NULL);
  gtk_window_set_default_icon(pixbuf);
  g_string_free(Imagefile, TRUE);
}

void activate_url(GtkAboutDialog *about, const gchar *link, gpointer data)
{
#ifdef __WIN32__
  char default_browser[512] = { '\0' };
  DWORD dwType, dwSize = sizeof(default_browser) - 1;

  SHGetValue(HKEY_CURRENT_USER,
        TEXT("Software\\Clients\\StartMenuInternet"),
        TEXT(""),
        &dwType,
        default_browser,
        &dwSize);

  if (default_browser[0] != '\0')
  {
    SHGetValue(HKEY_LOCAL_MACHINE,
        TEXT("SOFTWARE\\Clients\\StartMenuInternet"),
        TEXT(""),
        &dwType,
        default_browser,
        &dwSize);
  }

  if (default_browser[0] != '\0')
  {
    char browser_exe[2048] = { '\0' };
    dwSize = sizeof(browser_exe) - 1;

    char browser_exe_registry[1024] = { '\0' };
    snprintf(browser_exe_registry, 1024,
        "SOFTWARE\\Clients\\StartMenuInternet\\%s\\shell\\open\\command\\",
        default_browser);

    SHGetValue(HKEY_LOCAL_MACHINE,
        TEXT(browser_exe_registry), TEXT(""),
        &dwType, browser_exe, &dwSize);

    if (browser_exe[0] != '\0')
    {
      gint browser_command_size = strlen(browser_exe) + strlen(link) + 2;
      char *browser_command = g_malloc(sizeof(char) * browser_command_size);
      if (browser_command)
      {
        snprintf(browser_command, browser_command_size, "%s %s",
            browser_exe, link);

        STARTUPINFO si;
        PROCESS_INFORMATION pinf;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pinf, sizeof(pinf));

        if (! CreateProcess(NULL, browser_command,
              NULL, NULL, FALSE, 0, NULL, NULL, &si, &pinf))
        {
          put_status_message(_("Error launching external command"));
        }

        CloseHandle(pinf.hProcess);
        CloseHandle(pinf.hThread);

        g_free(browser_command);
        browser_command = NULL;
      }
    }
  }
#endif
}

void about_window(GtkWidget *widget, gpointer *data)
{
  GtkWidget *dialog = gtk_about_dialog_new();

  //for the bitmap
  GdkPixbuf *pixbuf = gdk_pixbuf_new_from_file (PIXMAP_PATH"mp3splt-gtk.png",
      NULL);
  gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog), pixbuf);
  
  gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog), (gchar *)PACKAGE_NAME);
  gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), VERSION);
  gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog),
                                 PACKAGE_NAME" : Copyright © 2005-2011 Alexandru"
                                 " Munteanu \n mp3splt : Copyright © 2002-2005 Matteo Trotta");

  gchar *b1 = NULL;
  gchar b3[100] = { '\0' };
  b1 = (gchar *)_("using");
  gchar library_version[20] = { '\0' };
  mp3splt_get_version(library_version);
  g_snprintf(b3, 100, "-%s-\n%s libmp3splt %s",
             _("release of "MP3SPLT_GTK_DATE), b1, library_version);
  
  gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog), b3);
  
  gtk_about_dialog_set_license(GTK_ABOUT_DIALOG(dialog),
                                "\n"
                                "This program is free software; you can "
                                "redistribute it and/or \n"
                                "modify it under the terms of the GNU General Public License\n"
                                "as published by the Free Software "
                                "Foundation; either version 2\n"
                                "of the License, or (at your option) "
                                "any later version.\n\n"
                                "This program is distributed in the "
                                "hope that it will be useful,\n"
                                "but WITHOUT ANY WARRANTY; without even "
                                "the implied warranty of\n"
                                "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
                                "GNU General Public License for more details.\n\n"
                                "You should have received a copy of the GNU General Public License\n"
                                "along with this program; if not, write "
                                "to the Free Software\n"
                                "Foundation, Inc., 59 Temple Place -"
                                "Suite 330, Boston, MA  02111-1307, "
                                "USA.");

  g_signal_connect(G_OBJECT(dialog), "activate-link", G_CALLBACK(activate_url), NULL);

  gtk_about_dialog_set_website_label(GTK_ABOUT_DIALOG(dialog),
      "http://mp3splt.sourceforge.net");
  gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog),
      "http://mp3splt.sourceforge.net");

  gtk_about_dialog_set_translator_credits(GTK_ABOUT_DIALOG(dialog),
      "Mario Blättermann <mariobl@gnome.org>");

  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

/*! Removes status bar message

Used for the ok button event.
*/
void remove_status_message()
{
  guint status_id =
    gtk_statusbar_get_context_id(GTK_STATUSBAR(status_bar), "mess");
  gtk_statusbar_pop(GTK_STATUSBAR(status_bar), status_id);
}

/*! Output a info message to the status message bar

The message type is automatically set to SPLT_MESSAGE_INFO.
If you don't want that use put_status_message instead.
\param text The text that has to be displayed.
*/

void put_status_message(const gchar *text)
{
  put_status_message_with_type(text, SPLT_MESSAGE_INFO);
}

/*! Output a message to the status message bar.

\param text The text that has to be displayed.
\param splt_message_type The type of the message.

If the type is to be set to SPLT_MESSAGE_INFO put_status_message
can be used instead; The enum for the message types is defined in
libmp3splt.h
 */
void put_status_message_with_type(const gchar *text, splt_message_type mess_type)
{
  if (mess_type == SPLT_MESSAGE_INFO)
  {
    guint status_id =
      gtk_statusbar_get_context_id(GTK_STATUSBAR(status_bar), "mess");

    gtk_statusbar_pop(GTK_STATUSBAR(status_bar), status_id);
    gtk_statusbar_push(GTK_STATUSBAR(status_bar), status_id, text);
  }

  put_message_in_history(text, mess_type);
}

//!event for the cancel button
void cancel_button_event(GtkWidget *widget, gpointer data)
{
  gint err = SPLT_OK;
  mp3splt_stop_split(the_state,&err);
  print_status_bar_confirmation(err);
  
  if (widget != NULL)
  {
    gtk_widget_set_sensitive(widget, FALSE);
  }
  put_status_message(_(" info: stopping the split process.. please wait"));
}

//!event for the split button
void split_button_event(GtkWidget *widget, gpointer data)
{
  //if we are not splitting
  if (!we_are_splitting)
  {
    mp3splt_set_int_option(the_state, SPLT_OPT_OUTPUT_FILENAMES,
        SPLT_OUTPUT_DEFAULT);

    gint err = SPLT_OK;

    put_options_from_preferences();

    //output format
    if (mp3splt_get_int_option(the_state, SPLT_OPT_SPLIT_MODE,&err)
        != SPLT_OPTION_NORMAL_MODE)
    {
      if (!get_checked_output_radio_box())
      {
        mp3splt_set_int_option(the_state, SPLT_OPT_OUTPUT_FILENAMES,
            SPLT_OUTPUT_FORMAT);
      }
    }

    filename_to_split = inputfilename_get();

    filename_path_of_split = outputdirectory_get();

    if (filename_path_of_split != NULL)
    {
      we_are_splitting = TRUE;
      create_thread(split_it, NULL, TRUE, NULL);
      gtk_widget_set_sensitive(GTK_WIDGET(cancel_button), TRUE);
    }
    else
    {
      put_status_message((gchar *)_(" error: no file selected"));
    }
  }
  else
  {
    put_status_message((gchar *)_(" error: split in progress..."));
  }
}

//!creates the toolbar
GtkWidget *create_toolbar()
{
  GtkWidget *box = gtk_hbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(box), 0);
  gtk_box_pack_start(GTK_BOX(box), 
      gtk_image_new_from_stock(GTK_STOCK_APPLY, GTK_ICON_SIZE_SMALL_TOOLBAR), 
      FALSE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(box), gtk_label_new(_("Split !")), FALSE, FALSE, 0);

  GtkWidget *split_button = gtk_button_new();
  gtk_container_add(GTK_CONTAINER(split_button), box);
  gtk_widget_set_tooltip_text(split_button,_("Split !"));
  gtk_container_set_border_width(GTK_CONTAINER(split_button), 0);
  gtk_button_set_relief(GTK_BUTTON(split_button), GTK_RELIEF_HALF);

  g_signal_connect(G_OBJECT(split_button), "clicked",
      G_CALLBACK(split_button_event), NULL);

  GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
  GtkWidget *hbox = gtk_hbox_new(FALSE, 0);

  gtk_box_pack_start(GTK_BOX(hbox), split_button, TRUE, FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

  return vbox;
}

//!event for the "messages history" button
void show_messages_history_dialog(GtkWidget *widget, gpointer data)
{
  gtk_widget_show_all(GTK_WIDGET(mess_history_dialog));
}

#ifndef NO_GNOME
void ShowHelp()
{
  GError* gerror = NULL;
  gtk_show_uri(gdk_screen_get_default(), "ghelp:mp3splt-gtk",  gtk_get_current_event_time(), &gerror);
}
#endif

static gchar *my_dgettext(const gchar *key, const gchar *domain)
{
  return dgettext("mp3splt-gtk", key);
}

//!creates the menu bar
GtkWidget *create_menu_bar()
{
  GtkWidget *menu_box = gtk_hbox_new(FALSE,0);
  
  static GtkActionEntry const entries[] = {
    //name, stock id,   label
    { "FileMenu", NULL, N_("_File") },  
    { "HelpMenu", NULL, N_("_Help") },

    //name, stock id, label, accelerator, tooltip
    { "Open", GTK_STOCK_OPEN, N_("_Open..."), "<Ctrl>O", N_("Open"),
      G_CALLBACK(browse_button_event) },

    { "Import", GTK_STOCK_FILE, N_("_Import splitpoints..."), "<Ctrl>I", N_("Import"),
      G_CALLBACK(import_event) },
    { "Export", GTK_STOCK_SAVE_AS, N_("_Export splitpoints..."), "<Ctrl>E", N_("Export"),
      G_CALLBACK(ChooseCueExportFile) },

    { "Split", GTK_STOCK_APPLY, N_("_Split !"), "<Ctrl>S", N_("Split"),
      G_CALLBACK(split_button_event) },
    { "Messages history", GTK_STOCK_INFO, N_("Messages _history"), "<Ctrl>H", N_("Messages history"),
      G_CALLBACK(show_messages_history_dialog) },

    { "Quit", GTK_STOCK_QUIT, N_("_Quit"), "<Ctrl>Q", N_("Quit"),
      G_CALLBACK(quit) },

#ifndef NO_GNOME
    { "Contents", GTK_STOCK_HELP, N_("_Contents"), "F1", N_("Contents"),
      G_CALLBACK(ShowHelp)},
#endif

    { "About", GTK_STOCK_ABOUT, N_("_About"), "<Ctrl>A", N_("About"),
      G_CALLBACK(about_window)},
  };

  static const gchar *ui_info = 
    "<ui>"
    "  <menubar name='MenuBar'>"
    "    <menu action='FileMenu'>"
    "      <menuitem action='Open'/>"
    "      <separator/>"
    "      <menuitem action='Import'/>"
    "      <menuitem action='Export'/>"
    "      <separator/>"
    "      <menuitem action='Split'/>"
    "      <menuitem action='Messages history'/>"
    "      <separator/>"
    "      <menuitem action='Quit'/>"
    "    </menu>"
    "    <menu action='HelpMenu'>"
#ifndef NO_GNOME
    "      <menuitem action='Contents'/>"
#endif
    "      <menuitem action='About'/>"
    "    </menu>"
    "  </menubar>"
    "</ui>";

  GtkActionGroup *actions = gtk_action_group_new ("Actions");

  gtk_action_group_set_translation_domain(actions, "mp3splt-gtk");
  gtk_action_group_set_translate_func(actions,
                  (GtkTranslateFunc)my_dgettext, NULL, NULL);

  //adding the GtkActionEntry to GtkActionGroup
  gtk_action_group_add_actions (actions, entries, G_N_ELEMENTS(entries), NULL);
  GtkUIManager *ui = gtk_ui_manager_new ();
  //set action to the ui
  gtk_ui_manager_insert_action_group (ui, actions, 0);
  //set the actions to the window
  gtk_window_add_accel_group (GTK_WINDOW (window), 
                              gtk_ui_manager_get_accel_group (ui));
  //add ui from string
  gtk_ui_manager_add_ui_from_string(ui, ui_info, -1, NULL);
  
  //attach the menu
  gtk_box_pack_start (GTK_BOX (menu_box), 
                      gtk_ui_manager_get_widget(ui, "/MenuBar"),
                      FALSE, FALSE, 0);
  
  GtkWidget *toolbar = (GtkWidget *)create_toolbar();
  gtk_box_pack_start(GTK_BOX(menu_box), toolbar, TRUE, TRUE, 0);
  
  return menu_box;
}

/*!creates a cool button with image from stock

\param label_text The text that has to be displayed on the button
\param stock_id The name of the stock image to be displayed on the
	button 
\param toggle_or_not TRUE means we create a toggle button
*/
GtkWidget *create_cool_button(gchar *stock_id, gchar *label_text,
    gint toggle_or_not)
{
  GtkWidget *box;
  GtkWidget *label;
  GtkWidget *image;
  GtkWidget *button;

  box = gtk_hbox_new(FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER (box), 2);

  image = gtk_image_new_from_stock(stock_id, GTK_ICON_SIZE_MENU);
  gtk_box_pack_start(GTK_BOX(box), image, FALSE, FALSE, 3);

  if (label_text != NULL)
  {
    label = gtk_label_new (label_text);
    gtk_label_set_text_with_mnemonic(GTK_LABEL(label),label_text);
    gtk_box_pack_start (GTK_BOX (box), label, FALSE, FALSE, 3);
  }
  
  if (toggle_or_not)
  {
    button = gtk_toggle_button_new();
  }
  else
  {
    button = gtk_button_new();
  }
 
  gtk_container_add(GTK_CONTAINER(button),box);
 
  return button;
}

//!main vbox
GtkWidget *create_main_vbox()
{
  //big ain box contailning all with statusbar
  GtkWidget *main_vbox;
  //used for pages
  GtkWidget *frame;
  //the tree view
  GtkTreeView *tree_view;
  //the main window tabbed notebook
  GtkWidget *notebook;
  /* label for the notebook */
  GtkWidget *notebook_label;

  /* main vertical box with statusbar */
  main_vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (main_vbox), 0);

  frame = (GtkWidget *)create_choose_file_frame();
  gtk_box_pack_start(GTK_BOX(main_vbox), frame, FALSE, FALSE, 0);

  /* tabbed notebook */
  notebook = gtk_notebook_new();
  gtk_box_pack_start (GTK_BOX (main_vbox), notebook, TRUE, TRUE, 0);
  gtk_notebook_popup_enable(GTK_NOTEBOOK(notebook));
  gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), TRUE);
  gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), FALSE);
  gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
  
  //creating the tree view
  GtkWidget *splitpoints_vbox;
  splitpoints_vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (splitpoints_vbox), 0);
  tree_view = (GtkTreeView *)create_tree_view();
  frame = (GtkWidget *)create_choose_splitpoints_frame(tree_view);
  gtk_container_add(GTK_CONTAINER(splitpoints_vbox), frame);
  
  /* player page */
  player_vbox = gtk_vbox_new(FALSE,0);
  notebook_label = gtk_label_new((gchar *)_("Player"));
      
  //player control frame
  player_box = (GtkWidget *)create_player_control_frame(tree_view);
  gtk_box_pack_start(GTK_BOX(player_vbox), player_box, FALSE, FALSE, 0);

  //playlist control frame
  playlist_box = (GtkWidget *)create_player_playlist_frame();

  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), player_vbox,
      (GtkWidget *)notebook_label);
      
  /* splitpoints page */
  notebook_label = gtk_label_new((gchar *)_("Splitpoints"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), 
                           splitpoints_vbox,
                           (GtkWidget *)notebook_label);

  /* split files frame */
  GtkWidget *split_files_vbox;
  split_files_vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (split_files_vbox), 0);
  
  frame = (GtkWidget *)create_split_files();
  gtk_container_add(GTK_CONTAINER(split_files_vbox), frame);

  notebook_label = gtk_label_new((gchar *)_("Split files"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), 
                           split_files_vbox,
                           (GtkWidget *)notebook_label);
  
  /* freedb page */
  GtkWidget *freedb_vbox;
  freedb_vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (freedb_vbox), 0);
  
  frame = (GtkWidget *)create_freedb_frame();
  gtk_container_add(GTK_CONTAINER(freedb_vbox), frame);
  
  notebook_label = gtk_label_new((gchar *)_("FreeDB"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), 
                           freedb_vbox,
                           (GtkWidget *)notebook_label);
  
  /* special split page */
  GtkWidget *special_split_vbox;
  special_split_vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (special_split_vbox), 0);
  frame = (GtkWidget *)create_special_split_page();
  gtk_container_add(GTK_CONTAINER(special_split_vbox), frame);
  notebook_label = gtk_label_new(_("Type of split"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), 
                           special_split_vbox,
                           (GtkWidget *)notebook_label);
 
  /* preferences page */
  GtkWidget *preferences_vbox;
  preferences_vbox = gtk_vbox_new (FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (preferences_vbox), 0);

  frame = (GtkWidget *)create_choose_preferences();
  gtk_container_add(GTK_CONTAINER(preferences_vbox), frame);

  notebook_label = gtk_label_new((gchar *)_("Preferences"));
  gtk_notebook_append_page(GTK_NOTEBOOK(notebook), 
                           preferences_vbox,
                           (GtkWidget *)notebook_label);
  
  /* progress bar */
  percent_progress_bar = gtk_progress_bar_new();
  //we begin at 0
  gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(percent_progress_bar),
                                0.0);
  //we write 0 on the bar
  gtk_progress_bar_set_text(GTK_PROGRESS_BAR(percent_progress_bar),
                            "");
  
  //hbox for progress bar and cancel button
  GtkWidget *hbox;
  hbox = gtk_hbox_new (FALSE,0);
  //we put the progress bar in the hbox
  gtk_box_pack_start(GTK_BOX(hbox), percent_progress_bar, TRUE, TRUE, 3);
  
  //stop button
  cancel_button = create_cool_button(GTK_STOCK_CANCEL,_("S_top"), FALSE);
  //action for the cancel button
  g_signal_connect(G_OBJECT(cancel_button), "clicked",
                   G_CALLBACK(cancel_button_event), NULL);
  
  //we put the stop button in the hbox
  gtk_box_pack_start(GTK_BOX(hbox), cancel_button, FALSE, TRUE, 3);
  gtk_widget_set_sensitive(GTK_WIDGET(cancel_button), FALSE);
  
  //we put progress bar hbox in the main box
  gtk_box_pack_start(GTK_BOX(main_vbox), hbox, FALSE, FALSE, 3);  

  /* show messages history dialog */
  create_mess_history_dialog();
 
  /* statusbar */
  status_bar = gtk_statusbar_new();

  //TODO: gtk+ >= 3
  //gtk_statusbar_set_has_resize_grip(GTK_STATUSBAR(status_bar), FALSE);

  GtkWidget *mess_history_button =
    create_cool_button(GTK_STOCK_INFO, NULL, FALSE);
  gtk_button_set_relief(GTK_BUTTON(mess_history_button), GTK_RELIEF_NONE);
  gtk_widget_set_tooltip_text(mess_history_button,_("Messages history"));
  gtk_box_pack_start(GTK_BOX(status_bar), mess_history_button, FALSE, FALSE, 0);
  g_signal_connect(G_OBJECT(mess_history_button), "clicked",
      G_CALLBACK(show_messages_history_dialog), NULL);

  gtk_box_pack_start(GTK_BOX(main_vbox), status_bar, FALSE, FALSE, 0);

  return main_vbox;
}

static void move_and_resize_main_window()
{
  const ui_main_window *main_win = ui_get_main_window_infos(ui);

  gint x = main_win->root_x_pos;
  gint y = main_win->root_y_pos;

  if (x != 0 && y != 0)
  {
    gtk_window_move(GTK_WINDOW(window), x, y);
  }
  else
  {
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  }

  gtk_window_resize(GTK_WINDOW(window), main_win->width, main_win->height);
}

void create_all()
{
#ifdef __WIN32__
  set_language();
#endif

  initialize_window();
 
  GtkWidget *window_vbox = gtk_vbox_new(FALSE, 0);
  gtk_container_add(GTK_CONTAINER(window), window_vbox);

  GtkWidget *menu_bar;
  menu_bar = (GtkWidget *)create_menu_bar();
  gtk_box_pack_start(GTK_BOX(window_vbox), menu_bar, FALSE, FALSE, 0);  
 
  GtkWidget *main_vbox = (GtkWidget *)create_main_vbox();
  gtk_box_pack_start(GTK_BOX(window_vbox), main_vbox, TRUE, TRUE, 0);
  
  load_preferences();

  if (selected_player == PLAYER_GSTREAMER)
  {
    hide_connect_button();
  }

  hide_disconnect_button();
  gtk_widget_hide(playlist_box);

  move_and_resize_main_window();

  gtk_widget_show_all(window);
}

/*!Output an error message from libmp3splt to the status bar

  \param The error number from the library.
 */
void print_status_bar_confirmation(gint confirmation)
{
  char *error_from_library = mp3splt_get_strerror(the_state, confirmation);
  if (error_from_library != NULL)
  {
    put_status_message(error_from_library);
    free(error_from_library);
    error_from_library = NULL;
  }
}
