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
 * The player control tab
 *
 * this file is used for the player control tab
 **********************************************************/

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <string.h>
#include <libmp3splt/mp3splt.h>
#include <unistd.h>
#include <math.h>

#ifdef __WIN32__
#include <winsock2.h>
#define usleep(x) Sleep(x/1000)
#endif

#include "util.h"
#include "tree_tab.h"
#include "player.h"
#include "player_tab.h"
#include "main_win.h"
#include "snackamp_control.h"
#include "utilities.h"
#include "split_files.h"
#include "mp3splt-gtk.h"
#include "ui_manager.h"
#include "widgets_helper.h"

#define DRAWING_AREA_WIDTH 400
#define DRAWING_AREA_HEIGHT 123 
#define DRAWING_AREA_HEIGHT_WITH_SILENCE_WAVE 232 

//! The text box showing the filename of the current input file.
GtkWidget *entry;
//! The browse button
GtkWidget *browse_button;
//! The fix ogg stream button
GtkWidget *fix_ogg_stream_button;

//! Tells us if the file was browsed or not
gint file_browsed = FALSE;
gint file_in_entry = FALSE;

//handle box for detaching window
GtkWidget *file_handle_box;

GtkWidget *player_buttons_hbox = NULL;

//if we have selected a correct file
gint incorrect_selected_file = FALSE;

gfloat total_draw_time = 0;

static const gint hundr_secs_th = 20;
static const gint tens_of_secs_th = 3 * 100;
static const gint secs_th = 40 * 100;
static const gint ten_secs_th = 3 * 6000;
static const gint minutes_th = 20 * 6000;
static const gint ten_minutes_th = 3 * 3600 * 100;

extern GtkWidget *names_from_filename;

extern splt_state *the_state;
extern gint preview_start_position;
extern gint preview_start_splitpoint;
extern GtkWidget *browse_cddb_button;
extern GtkWidget *browse_cue_button;
//main window
extern GtkWidget *window;
extern GtkWidget *percent_progress_bar;
//if we are currently splitting
extern gint we_are_splitting;
extern gchar *filename_to_split;
extern gchar *filename_path_of_split;
extern guchar *get_real_name_from_filename(guchar *filename);
extern GtkWidget *cancel_button;
extern gint debug_is_active;

extern ui_state *ui;

//our progress bar
GtkWidget *progress_bar;
//our progress bar adjustment
GtkWidget *progress_adj;

//volume adjustmnent
GtkWidget *volume_adj;
//volume bar
GtkWidget *volume_bar;

//the time label
GtkWidget *label_time;
//minutes and seconds for the player
gint player_seconds = 0, 
  player_minutes = 0, player_hundr_secs = 0;
//only for internal use when we change manually we have the real
//time which is player_seconds and the imaginary time player_seconds2
gint player_seconds2 = 0, 
  player_minutes2 = 0, player_hundr_secs2 = 0;
//wether to change the volume of the player
gboolean change_volume = TRUE;
//to see if we are on the volume bar
gboolean on_the_volume_bar = FALSE;
//variable that stocks if the song is playing or not
gboolean playing = FALSE;
//to see if we have a stream
gboolean stream = FALSE;
//the name of the song
GtkWidget *song_name_label;

//connect and disconnect to player buttons
GtkWidget *connect_button;
GtkWidget *disconnect_button;

//informations about the playing song
GtkWidget *song_infos;

//player buttons
GtkWidget *play_button;
GtkWidget *stop_button;
GtkWidget *pause_button;
GtkWidget *player_add_button;
GtkWidget *go_beg_button;
GtkWidget *go_end_button;

//silence wave
GtkWidget *silence_wave_check_button = NULL;
silence_wave *silence_points = NULL;
gint number_of_silence_points = 0;
gint malloced_num_of_silence_points = 0;
gint show_silence_wave = FALSE;
gint we_scan_for_silence = FALSE;

//stock if the timer is active or not
gboolean timer_active = FALSE;
//id of the timeout, used to stop it
gint timeout_id;

//handle for detaching
GtkWidget *player_handle;

//handle for detaching playlist
GtkWidget *playlist_handle;

extern gint file_browsed;
extern gint selected_player;
extern GArray *splitpoints;
extern gint splitnumber;

//total time of the current song
gfloat total_time = 0;
//current time
gfloat current_time = 0;

//to see if the mouse has clicked on the progress bar
gboolean mouse_on_progress_bar = FALSE;

//just used here for the timer hack
gint stay_turn = 0;

//the witdh of the drawing area
gint width_drawing_area = 0;

//our drawing area
GtkWidget *da = NULL;

//drawing zoom coefficient
gfloat zoom_coeff = 2.0;
gfloat zoom_coeff_old;
//drawing area buttons pressed
gboolean button1_pressed = FALSE;
gboolean button2_pressed = FALSE;
//drawing area pushed point left button
gint button_x;
gint button_y;
//drawing area pushed point right button
gint button_x2;
gint button_y2;
//time where we move
gfloat move_time;

extern gboolean quick_preview;
extern gint quick_preview_end_splitpoint;

gint timeout_value = 200;

//the splitpoints to move on the zoom progress bar
gint splitpoint_to_move = -1;
gboolean move_splitpoints = FALSE;
gboolean remove_splitpoints = FALSE;
gboolean select_splitpoints = FALSE;
gboolean check_splitpoint = FALSE;

gint only_press_pause = FALSE;

//our playlist tree
GtkWidget *playlist_tree = NULL;
gint playlist_tree_number = 0;

//drawing area variables
gint margin;
gint real_erase_split_length;
gint real_progress_length;
gint real_move_split_length;
gint real_checkbox_length;
gint real_text_length;
gint real_wave_length;
//
gint erase_split_ylimit;
gint progress_ylimit;
gint splitpoint_ypos;
gint checkbox_ypos;
gint text_ypos;
gint wave_ypos;

/*! \defgroup Playerbuttons Button images for the player

\@{
*/
GtkWidget *Go_BegButton_active;
GtkWidget *Go_BegButton_inactive;
GtkWidget *Go_EndButton_active;
GtkWidget *Go_EndButton_inactive;
GtkWidget *PlayButton_active;
GtkWidget *PlayButton_inactive;
GtkWidget *StopButton_active;
GtkWidget *StopButton_inactive;
GtkWidget *PauseButton_active;
GtkWidget *PauseButton_inactive;
//@}

/*! The storage for the name of the input file.

Designed to be accessed using inputfilename_set() and
inputfilename_get().
 */
GString *inputfilename;

//remove file button
GtkWidget *playlist_remove_file_button;
//remove file button
GtkWidget *playlist_remove_all_files_button;

//playlist tree enumeration
enum
  {
    COL_NAME,
    COL_FILENAME,
    PLAYLIST_COLUMNS 
  };

//function declarations
gint mytimer(gpointer data);
extern void copy_filename_to_current_description(const gchar *fname);

//! Set the name of the input file
void inputfilename_set(const gchar *filename)
{
  // Paranoia test.
  if(filename!=NULL)
    {
      // Free the old string before allocating memory for the new one
      if(inputfilename!=NULL)g_string_free(inputfilename,TRUE);
      inputfilename=g_string_new(filename);

      // Update the text in the gui field displaying the output 
      // directory - if this field is already there and thus can 
      // be updated.
      if(entry!=NULL)
	gtk_entry_set_text(GTK_ENTRY(entry), filename);
    }
}

/*! Get the name of the input file

\return 
 - The name of the input file, if set.
 - "", otherwise.
*/
gchar* inputfilename_get()
{
  if(inputfilename!=NULL)
    return(inputfilename->str);
  else
    return "";
}



//!function called from the library when scanning for the silence level
void get_silence_level(long time, float level, void *user_data)
{
  if (! silence_points)
  {
    silence_points = g_malloc(sizeof(silence_wave) * 3000);
    malloced_num_of_silence_points = 3000;
  }
  else if (number_of_silence_points >= malloced_num_of_silence_points)
  {
    silence_points = g_realloc(silence_points,
        sizeof(silence_wave) * (number_of_silence_points + 3000));
    malloced_num_of_silence_points = number_of_silence_points + 3000;
  }

  silence_points[number_of_silence_points].time = time;
  silence_points[number_of_silence_points].level = abs(level);

  number_of_silence_points++;
}

gpointer detect_silence(gpointer data)
{
  gint err = SPLT_OK;

  //erase previous points
  if (silence_points)
  {
    g_free(silence_points);
    silence_points = NULL;
    number_of_silence_points = 0;
  }

  enter_threads();

  gtk_widget_set_sensitive(cancel_button, TRUE);
  filename_to_split = inputfilename_get();

  exit_threads();

  mp3splt_set_int_option(the_state, SPLT_OPT_DEBUG_MODE, debug_is_active);
  mp3splt_set_filename_to_split(the_state, filename_to_split);

  mp3splt_set_silence_level_function(the_state, get_silence_level, NULL);
  we_are_splitting = TRUE;
  we_scan_for_silence = TRUE;

  mp3splt_set_silence_points(the_state, &err);

  we_scan_for_silence = FALSE;
  we_are_splitting = FALSE;
  mp3splt_set_silence_level_function(the_state, NULL, NULL);

  enter_threads();

  print_status_bar_confirmation(err);
  gtk_widget_set_sensitive(cancel_button, FALSE);

  exit_threads();

  return NULL;
}

/*! Initialize scanning for silence in the background.

  If showing the silence wave is disabled this function won't do anything.
 */
void scan_for_silence_wave()
{
  if (we_scan_for_silence)
  {
    cancel_button_event(NULL, NULL);
  }

  if (timer_active)
  {
    create_thread(detect_silence, NULL, TRUE, NULL);
  }
}

/*! Change the name of the song that is to be cut and played

Manages changing the filename itselves as well as recalculating the silence
wave if needed.
*/
void change_current_filename(const gchar *fname)
{
  const gchar *old_fname = inputfilename_get();
  if (!old_fname)
  {
    inputfilename_set(fname);
    if (show_silence_wave)
    {
      scan_for_silence_wave();
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(names_from_filename)))
    {
      copy_filename_to_current_description(fname);
    }
  }
  else if (strcmp(old_fname,fname) != 0)
  {
    inputfilename_set(fname);
    if (show_silence_wave)
    {
      scan_for_silence_wave();
    }
    if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(names_from_filename)))
    {
      copy_filename_to_current_description(fname);
    }
  }
}

//!resets and sets inactive the progress bar
void reset_inactive_progress_bar()
{
  gtk_widget_set_sensitive(GTK_WIDGET(progress_bar), FALSE);
  gtk_adjustment_set_value(GTK_ADJUSTMENT(progress_adj),0);
}

//!resets and sets inactive the volume bar
void reset_inactive_volume_bar()
{
  gtk_widget_set_sensitive(GTK_WIDGET(volume_bar), FALSE);
  gtk_adjustment_set_value(GTK_ADJUSTMENT(volume_adj),0);
}

//!resets the label time
void reset_label_time()
{
  if (strcmp(gtk_label_get_text(GTK_LABEL(label_time)),"") == 0)
  {
    gtk_label_set_text(GTK_LABEL(label_time), "");
  }
}

//!resets song infos, frequency, etc..
void reset_song_infos()
{
  gtk_label_set_text(GTK_LABEL(song_infos),"");
}

//!resets the song name label
void reset_song_name_label()
{
  gtk_label_set_text(GTK_LABEL(song_name_label), "");
}

//!clear song data and makes inactive progress bar
void clear_data_player()
{
  //set browse button available
  gtk_widget_set_sensitive(browse_button, TRUE);

  reset_song_name_label();
  reset_song_infos();
  reset_inactive_volume_bar();
  reset_inactive_progress_bar();
  reset_label_time();
}

//!enables the buttons of the player
void enable_player_buttons()
{
  gtk_widget_set_sensitive(stop_button, TRUE);
  gtk_button_set_image(GTK_BUTTON(stop_button), g_object_ref(StopButton_active));
  
  gtk_widget_set_sensitive(pause_button, TRUE);
  gtk_button_set_image(GTK_BUTTON(pause_button), g_object_ref(PauseButton_active));
 
 if (selected_player != PLAYER_GSTREAMER)
  {
    gtk_widget_set_sensitive(go_beg_button, TRUE);
    gtk_button_set_image(GTK_BUTTON(go_beg_button), g_object_ref(Go_BegButton_active));
    gtk_widget_set_sensitive(go_end_button, TRUE);
    gtk_button_set_image(GTK_BUTTON(go_end_button), g_object_ref(Go_EndButton_active));
  }
  gtk_widget_set_sensitive(play_button, TRUE);
  gtk_button_set_image(GTK_BUTTON(play_button), g_object_ref(PlayButton_active));
}

//!disables the buttons of the player
void disable_player_buttons()
{
  gtk_widget_set_sensitive(stop_button, FALSE);
  gtk_button_set_image(GTK_BUTTON(stop_button), g_object_ref(StopButton_inactive));
  gtk_widget_set_sensitive(pause_button, FALSE);
  gtk_button_set_image(GTK_BUTTON(pause_button), g_object_ref(PauseButton_inactive));
  gtk_widget_set_sensitive(go_beg_button, FALSE);
  gtk_button_set_image(GTK_BUTTON(go_beg_button), g_object_ref(Go_BegButton_inactive));
  gtk_widget_set_sensitive(go_end_button, FALSE);
  gtk_button_set_image(GTK_BUTTON(go_end_button), g_object_ref(Go_EndButton_inactive));
  gtk_widget_set_sensitive(play_button, FALSE);
  gtk_button_set_image(GTK_BUTTON(play_button), g_object_ref(PlayButton_active));
  gtk_widget_set_sensitive(player_add_button, FALSE);
  gtk_widget_set_sensitive(silence_wave_check_button, FALSE);
}

//! Switches between connect and disconnect button when connecting to player
void connect_change_buttons()
{
  if (selected_player != PLAYER_GSTREAMER)
  {
    show_disconnect_button();
    hide_connect_button();
  }
}

//!disconnecting changing buttons
void disconnect_change_buttons()
{
  if (selected_player != PLAYER_GSTREAMER)
  {
    hide_disconnect_button();
    show_connect_button();
  }
}

/*!connect with the song fname

\param fname the file name of the song 
\param start_playing 
-  start playing
- else dont start playing right now.
*/
void connect_with_song(const gchar *fname, gint start_playing)
{
  //list with songs
  GList *song_list = NULL;

  if (fname != NULL)
    {
      song_list = g_list_append(song_list, strdup(fname));
      
      //if we must also play the song
      if (start_playing == 0)
        {
          //if the player is not running, start it ,queue to playlist and
          //play the file
          if (!player_is_running())
          {
            player_start_play_with_songs(song_list);
          }
          else
          {
            player_add_play_files(song_list);
          }
        }
      else
        {
          if (file_browsed)
            {
              //if the player is not running, start it ,queue to playlist and
              //play the file
              if (!player_is_running())
                player_start_add_files(song_list);
              else
                if(!playing)
                  player_add_files_and_select(song_list);
                else
                  player_add_files(song_list);
            }
        }
      playing = TRUE;
      
      //we start the timer
      if (!timer_active)
        {
          //30 = cursive
          timeout_id = g_timeout_add(timeout_value, mytimer, NULL);
          timer_active = TRUE;
        }
      
      //enable player buttons
      enable_player_buttons();
      //here we check if we have been connected
      if (player_is_running())
        {
          //change connect/disconnect buttons
          connect_change_buttons();
        }
    }
  
  //TODO: free elements of list
  g_list_free(song_list);
}

/*! connects to player with the song from the filename entry

  \param i 0 means then start playing, != 0 means dont start playing
  right now
 */
void connect_to_player_with_song(gint i)
{
  const gchar *fname = fname = inputfilename_get();

  //connect with the song fname
  connect_with_song(fname,i);
}

//!play button event
void connect_button_event(GtkWidget *widget, gpointer data)
{
  //we open the player if its not done
  if (!player_is_running())
  {
    player_start();
  }
 
  mytimer(NULL);
 
  //we start the timer
  if (!timer_active)
  {
    //we open socket channel  if dealing with snackamp
    if (selected_player == PLAYER_SNACKAMP)
    {
      connect_snackamp(8775);
    }

    //30 = cursive
    timeout_id = g_timeout_add(timeout_value, mytimer, NULL);
    timer_active = TRUE;
  }
 
  //connect to player with song
  //1 means dont start playing
  connect_to_player_with_song(1);
 
  //set browse button unavailable
  if (selected_player != PLAYER_GSTREAMER)
  {
    gtk_widget_set_sensitive(browse_button, FALSE);
  }
  //!enable player buttons
  enable_player_buttons();

  file_browsed = FALSE;
  
  change_volume = TRUE;
  
  //here we check if we have been connected
  if (!player_is_running())
    {
      //if not, we put a message
      GtkWidget *dialog, *label;
      dialog = gtk_dialog_new_with_buttons (_("Cannot connect to player"),
                                            (GtkWindow *)window,
                                            GTK_DIALOG_MODAL,
                                            GTK_STOCK_OK,
                                            GTK_RESPONSE_NONE,
                                            NULL);
      
      switch(selected_player)
        {
        case PLAYER_SNACKAMP:
          label = gtk_label_new
            (_("\n Cannot connect to snackAmp player.\n"
             " Please download and install snackamp from\n"
             "\thttp://snackamp.sourceforge.net\n\n"
             " Verify that snackamp is running.\n"
             " Verify that your snackamp version is >= 3.1.3\n\n"
             " Verify that you have enabled socket interface in snackamp:\n"
             " You have to go to\n"
             "\tTools->Preferences->Miscellaneous\n"
             " from the snackamp menu and check\n"
             "\tEnable Socket Interface\n"
             " Only default port is supported for now(8775)\n"
             " After that, restart snackamp and mp3splt-gtk should work.\n"));
          break;
        case PLAYER_AUDACIOUS:
          label = gtk_label_new 
            (_("\n Cannot connect to Audacious player.\n"
             " Verify that you have installed audacious.\n\n"
             " Put in your PATH variable the directory where the audacious"
             " executable is.\n"
             " If you don't know how to do that, start audacious manually"
             " and then try to connect.\n"));
          break;
        default:
          label = gtk_label_new(_("Cannot connect to player"));
          break;
        }
    
      g_signal_connect_swapped(dialog, "response", 
          G_CALLBACK(gtk_widget_destroy), dialog);
      gtk_container_add(GTK_CONTAINER(
            gtk_dialog_get_content_area(GTK_DIALOG(dialog))), label);
      gtk_widget_show_all(dialog);
    }
  else
    {
      //changes connect/disconnect buttons
      connect_change_buttons();
    }
  
  current_time = -1;
  check_update_down_progress_bar();
}

//!checks if we have a stream
void check_stream()
{
  //if we have a stream
  if (total_time == -1)
    {
      stream = TRUE;
      //reset progress bar
      reset_inactive_progress_bar();
    }
  else
    stream = FALSE;
}

//!disconnect button event
void disconnect_button_event(GtkWidget *widget, gpointer data)
{
  //if the timer is active, deactivate the function
  if (timer_active)
  {
    //we open socket channel  if dealing with snackamp
    if (selected_player == PLAYER_SNACKAMP)
    {
      disconnect_snackamp();
    }

    g_source_remove(timeout_id);
    timer_active = FALSE;
  }

  clear_data_player();
  //set browse button available
  gtk_widget_set_sensitive(browse_button, TRUE);
  //changes connect/disconnect buttons
  disconnect_change_buttons();
  //disable player buttons
  disable_player_buttons();

  //update bottom progress bar to 0 and ""
  if (!we_are_splitting)
  {
    //fraction update
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(percent_progress_bar),
        0);
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(percent_progress_bar),
        "");
  }

  const gchar *fname = inputfilename_get();
  if (is_filee(fname))
  {
    file_in_entry = TRUE;
    gtk_widget_set_sensitive(play_button, TRUE);
    gtk_button_set_image(GTK_BUTTON(play_button), g_object_ref(PlayButton_active));
  }

  player_quit();
}

//! play button event
void play_event (GtkWidget *widget, gpointer data)
{
  //only if connected to player
  if (timer_active)
    {
      if (!player_is_running())
      {
        player_start();
      }
      player_play();
      playing = TRUE;
    }
  else
    {
      //connects to player with the song
      //0 means also start playing
      connect_to_player_with_song(0);
      //set browse button unavailable
      if (selected_player != PLAYER_GSTREAMER)
      {
        gtk_widget_set_sensitive(browse_button, FALSE);
      }
    }
}

//! stop button event
void stop_event (GtkWidget *widget, gpointer data)
{
  //only if connected to player
  if (timer_active)
    {
      //unpress pause button
      if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pause_button)))
      {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pause_button), FALSE);
      }

      if (player_is_running())
        playing = FALSE;
      player_stop();
    }
}

//! pause button event
void pause_event (GtkWidget *widget, gpointer data)
{
  //only if connected to player
  if (timer_active)
    {
      if (player_is_running())
        {
          if (!only_press_pause)
            {
              player_pause();
            }
        }
    }
}

//! Event for the "previous" button 
void prev_button_event (GtkWidget *widget, gpointer data)
{
  //only if connected to player
  if (timer_active)
    if (player_is_running())
      player_prev();
}

//! event for the "next" button
void next_button_event (GtkWidget *widget, gpointer data)
{
  //only if connected to player
  if (timer_active)
    if (player_is_running())
      player_next();
}

//!changes the position inside the song
void change_song_position()
{
  //new position of the song
  gint position;
  
  position = (player_seconds2 + 
              player_minutes2*60)*1000
    +player_hundr_secs2*10;
  
  player_jump(position);  
}

//!adds a splitpoint from the player
void enable_show_silence_wave(GtkToggleButton *widget, gpointer data)
{
  if (gtk_toggle_button_get_active(widget))
  {
    show_silence_wave = TRUE;
    if (number_of_silence_points == 0)
    {

      scan_for_silence_wave();
    }
  }
  else
  {
    show_silence_wave = FALSE;
    if (we_scan_for_silence)
    {
      cancel_button_event(NULL, NULL);
    }
    //free the previous silence points if any
    if (silence_points != NULL)
    {
      g_free(silence_points);
      silence_points = NULL;
    }
    number_of_silence_points = 0;
  }

  //TODO: call draw event on da
  //da_draw_event(da, NULL, NULL);
}

void build_svg_path(GString *imagefile, gchar *svg_filename)
{
#ifdef __WIN32__
  g_string_assign(imagefile, ".");
  g_string_append(imagefile, G_DIR_SEPARATOR_S);
  g_string_append(imagefile, svg_filename);
#else
  if (strlen(IMAGEDIR) == 0)
  {
    g_string_assign(imagefile, svg_filename);
  }
  else 
  {
    g_string_assign(imagefile, IMAGEDIR);
    g_string_append(imagefile, G_DIR_SEPARATOR_S);
    g_string_append(imagefile, svg_filename);
  }
#endif
}

//!creates the player buttons hbox
GtkWidget *create_player_buttons_hbox(GtkTreeView *tree_view)
{
  player_buttons_hbox = gtk_hbox_new(FALSE, 0);

  //go at the beginning button
  GString *Imagefile = g_string_new(IMAGEDIR);

  build_svg_path(Imagefile, "backward"ICON_EXT);
  Go_BegButton_active= gtk_image_new_from_file(Imagefile->str);

  build_svg_path(Imagefile, "backward_inactive"ICON_EXT);
  Go_BegButton_inactive= gtk_image_new_from_file(Imagefile->str);
  go_beg_button = gtk_button_new();
  gtk_button_set_image(GTK_BUTTON(go_beg_button), g_object_ref(Go_BegButton_inactive));

  //put the new button in the box
  gtk_box_pack_start(GTK_BOX(player_buttons_hbox), go_beg_button, FALSE, FALSE, 0);
  gtk_button_set_relief(GTK_BUTTON(go_beg_button), GTK_RELIEF_NONE);
  g_signal_connect(G_OBJECT(go_beg_button), "clicked",
                   G_CALLBACK(prev_button_event),
                   NULL);
  gtk_widget_set_sensitive(go_beg_button, FALSE);
  gtk_widget_set_tooltip_text(go_beg_button, _("Previous"));

  //play button
  build_svg_path(Imagefile, "play"ICON_EXT);
  PlayButton_active= gtk_image_new_from_file(Imagefile->str);

  build_svg_path(Imagefile, "play_inactive"ICON_EXT);
  PlayButton_inactive= gtk_image_new_from_file(Imagefile->str);
  play_button = gtk_button_new();
  gtk_button_set_image(GTK_BUTTON(play_button), g_object_ref(PlayButton_inactive));

  //put the new button in the box
  gtk_box_pack_start(GTK_BOX(player_buttons_hbox), play_button, FALSE, FALSE, 0);
  gtk_button_set_relief(GTK_BUTTON(play_button), GTK_RELIEF_NONE);
  g_signal_connect(G_OBJECT(play_button), "clicked",
                   G_CALLBACK(play_event),
                   NULL);
  gtk_widget_set_sensitive(play_button, FALSE);
  gtk_widget_set_tooltip_text(play_button,_("Play"));

  //pause button
  build_svg_path(Imagefile, "pause"ICON_EXT);
  PauseButton_active= gtk_image_new_from_file(Imagefile->str);

  build_svg_path(Imagefile, "pause_inactive"ICON_EXT);
  PauseButton_inactive= gtk_image_new_from_file(Imagefile->str);
  pause_button = gtk_toggle_button_new();
  gtk_button_set_image(GTK_BUTTON(pause_button), g_object_ref(PauseButton_inactive));
  //put the new button in the box
  gtk_box_pack_start(GTK_BOX(player_buttons_hbox), pause_button, FALSE, FALSE, 0);
  gtk_button_set_relief(GTK_BUTTON(pause_button), GTK_RELIEF_NONE);
  g_signal_connect(G_OBJECT(pause_button), "clicked",
                   G_CALLBACK(pause_event), NULL);
  gtk_widget_set_sensitive(pause_button, FALSE);
  gtk_widget_set_tooltip_text(pause_button,_("Pause"));

  //stop button
  build_svg_path(Imagefile, "stop"ICON_EXT);
  StopButton_active= gtk_image_new_from_file(Imagefile->str);

  build_svg_path(Imagefile, "stop_inactive"ICON_EXT);
  StopButton_inactive= gtk_image_new_from_file(Imagefile->str);
  stop_button = gtk_button_new();
  gtk_button_set_image(GTK_BUTTON(stop_button), g_object_ref(StopButton_inactive));
  //put the new button in the box
  gtk_box_pack_start(GTK_BOX(player_buttons_hbox), stop_button, FALSE, FALSE, 0);
  gtk_button_set_relief(GTK_BUTTON(stop_button), GTK_RELIEF_NONE);
  g_signal_connect(G_OBJECT(stop_button), "clicked",
                   G_CALLBACK(stop_event),
                   NULL);
  gtk_widget_set_sensitive(stop_button, FALSE);
  gtk_widget_set_tooltip_text(stop_button,_("Stop"));

  //go at the end button
  build_svg_path(Imagefile, "forward"ICON_EXT);
  Go_EndButton_active= gtk_image_new_from_file(Imagefile->str);

  build_svg_path(Imagefile, "forward_inactive"ICON_EXT);
  Go_EndButton_inactive= gtk_image_new_from_file(Imagefile->str);
  go_end_button = gtk_button_new();
  gtk_button_set_image(GTK_BUTTON(go_end_button), g_object_ref(Go_EndButton_inactive));
  //put the new button in the box
  gtk_box_pack_start(GTK_BOX(player_buttons_hbox), go_end_button, FALSE, FALSE, 0);
  gtk_button_set_relief(GTK_BUTTON(go_end_button), GTK_RELIEF_NONE);
  g_signal_connect(G_OBJECT(go_end_button), "clicked",
                   G_CALLBACK(next_button_event),
                   NULL);
  gtk_widget_set_sensitive(go_end_button, FALSE);
  gtk_widget_set_tooltip_text(go_end_button,_("Next"));
  g_string_free(Imagefile,TRUE);

  //add button
  player_add_button = (GtkWidget *)create_cool_button(GTK_STOCK_ADD, _("_Add"), FALSE);
  //put the new button in the box
  gtk_box_pack_start (GTK_BOX(player_buttons_hbox), player_add_button, FALSE, FALSE, 5);
  gtk_button_set_relief(GTK_BUTTON(player_add_button), GTK_RELIEF_NONE);
  g_signal_connect(G_OBJECT(player_add_button), "clicked",
                   G_CALLBACK(add_splitpoint_from_player),
                   tree_view);
  gtk_widget_set_sensitive(player_add_button, FALSE);
  gtk_widget_set_tooltip_text(player_add_button,_("Add splitpoint from player"));
  
  //silence wave check button
  silence_wave_check_button = (GtkWidget *)
    gtk_check_button_new_with_mnemonic(_("_Show amplitude wave"));
  //put the new button in the box
  gtk_box_pack_end(GTK_BOX(player_buttons_hbox), silence_wave_check_button,
      FALSE, FALSE, 5);
  g_signal_connect(G_OBJECT(silence_wave_check_button), "toggled",
      G_CALLBACK(enable_show_silence_wave), NULL);
  gtk_widget_set_sensitive(silence_wave_check_button, FALSE);
  gtk_widget_set_tooltip_text(silence_wave_check_button,
      _("Shows the amplitude level wave"));

  /* connect player button */
  connect_button = (GtkWidget *)
    create_cool_button(GTK_STOCK_CONNECT,_("_Connect"), FALSE);
  g_signal_connect(G_OBJECT(connect_button), "clicked",
      G_CALLBACK(connect_button_event), NULL);
  gtk_widget_set_tooltip_text(connect_button,_("Connect to player"));
  
  /* disconnect player button */
  disconnect_button = (GtkWidget *)
    create_cool_button(GTK_STOCK_DISCONNECT,_("_Disconnect"), FALSE);
  g_signal_connect(G_OBJECT (disconnect_button), "clicked",
      G_CALLBACK(disconnect_button_event), NULL);
  gtk_widget_set_tooltip_text(disconnect_button,_("Disconnect from player"));

  return player_buttons_hbox;
}

//!song information about frequency, rate, stereo, etc
GtkWidget *create_song_informations_hbox()
{
  GtkWidget *song_info_hbox;

  song_info_hbox = gtk_hbox_new (FALSE, 0);

  song_infos = gtk_label_new ("");
  gtk_box_pack_start (GTK_BOX (song_info_hbox), song_infos, FALSE, FALSE, 40);

  //the label time
  label_time = gtk_label_new("");
  gtk_box_pack_start (GTK_BOX (song_info_hbox), label_time, FALSE, FALSE, 5);

  return song_info_hbox;
}

//!when we unclick the progress bar
gboolean progress_bar_unclick_event (GtkWidget *widget,
                                   GdkEventCrossing *event,
                                   gpointer user_data)
{
  change_song_position();

  player_minutes = player_minutes2;
  player_seconds = player_seconds2; 
  player_hundr_secs = player_hundr_secs2; 
  
  mouse_on_progress_bar = FALSE;
  
  return FALSE;
}

//!when we click the progress bar
gboolean progress_bar_click_event (GtkWidget *widget,
                                   GdkEventCrossing *event,
                                   gpointer user_data)
{
  mouse_on_progress_bar = TRUE;
  return FALSE;
}

//!returns the total time in hundreths of second
gfloat get_total_time()
{
  return total_time;
}

//!returns the total elapsed time
gfloat get_elapsed_time()
{
  //progress position
  gfloat adj_position;
  adj_position =
    gtk_adjustment_get_value(GTK_ADJUSTMENT(progress_adj));
  
  //the current time
  current_time =
    (adj_position * total_time)/100000;
  
  return current_time;
}

//!refreshes the drawing area
void refresh_drawing_area()
{
  GdkRectangle update_rect;
  
  gint width_drawing_area;
  gint height_drawing_area;
  wh_get_widget_size(da, &width_drawing_area, &height_drawing_area);
 
  update_rect.x = 0;
  update_rect.y = 0;
  update_rect.width = width_drawing_area;
  update_rect.height = height_drawing_area;
  
  gdk_window_invalidate_rect(gtk_widget_get_window(da), &update_rect, FALSE);
}

//!updates bottom progress bar
void check_update_down_progress_bar()
{
  //if we are not currently splitting
  if (!we_are_splitting)
  {
    //if we are between 2 splitpoints,
    //we draw yellow rectangle
    gfloat total_interval = 0;
    gfloat progress_time = 0;
    gint splitpoint_time_left = -1;
    gint splitpoint_time_right = -1;
    gint splitpoint_left_index = -1;
    get_splitpoint_time_left_right(&splitpoint_time_left,
        &splitpoint_time_right,
        &splitpoint_left_index);

    if ((splitpoint_time_left != -1) && 
        (splitpoint_time_right != -1))
    {
      //percent progress bar stuff
      total_interval = splitpoint_time_right - splitpoint_time_left;
      if (total_interval != 0)
      {
        progress_time = (current_time-splitpoint_time_left)/
          total_interval;
      }
    }
    else
    {
      if (splitpoint_time_right == -1)
      {
        total_interval = total_time - splitpoint_time_left;
        if (total_interval != 0)
        {
          progress_time = (current_time-splitpoint_time_left)/
            total_interval;
        }
      }
      else
      {
        total_interval = splitpoint_time_right;
        if (total_interval != 0)
        {
          progress_time = current_time/total_interval;
        }
      }
    }
    //we update the percent progress bar  
    if (progress_time < 0)
    {
      progress_time = 0;
    }
    if (progress_time > 1)
    {
      progress_time = 1;
    }
    if ((progress_time >= 0) && (progress_time <= 1))
    {
      //fraction update
      gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(percent_progress_bar),
          progress_time);
    }

    gchar *progress_description =
      get_splitpoint_name(splitpoint_left_index-1);
    gchar description_shorted[512] = { '\0' };
    //if we have a splitpoint on our right
    //and we are before the first splitpoint
    if (splitpoint_time_right != -1)
    {
      if (splitpoint_time_left == -1)
      {
        if (progress_description != NULL)
        {
          g_snprintf(description_shorted,60, _("before %s"),
              progress_description);
        }
      }
      else
      {
        if (progress_description != NULL)
        {
          g_snprintf(description_shorted, 60,"%s", progress_description);
        }
      }
    }
    else
    {
      if (splitpoint_time_left != -1)
      {
        if (progress_description != NULL)
        {
          g_snprintf(description_shorted, 60,"%s", progress_description);
        }
      }
      else
      {
        //TODO ugly code in 'fname' usage !
        gchar *fname;
        fname = inputfilename_get();
        fname = (gchar *)get_real_name_from_filename((guchar *)fname);
        g_snprintf(description_shorted,60,"%s",fname);
        if (fname != NULL)
        {
          if (strlen(fname) > 60)
          {
            description_shorted[strlen(description_shorted)-1] = '.';
            description_shorted[strlen(description_shorted)-2] = '.';
            description_shorted[strlen(description_shorted)-3] = '.';
          }
        }
      }
    }
    //we put "..."
    if (progress_description != NULL)
    {
      if (strlen(progress_description) > 60)
      {
        description_shorted[strlen(description_shorted)-1] = '.';
        description_shorted[strlen(description_shorted)-2] = '.';
        description_shorted[strlen(description_shorted)-3] = '.';
      }
    }

    //progress text
    //we write the name and the progress on the bar
    gtk_progress_bar_set_text(GTK_PROGRESS_BAR(percent_progress_bar),
        description_shorted);
    g_free(progress_description);
  }
}

//!event when the progress bar value changed
void progress_bar_value_changed_event (GtkRange *range,
                                       gpointer user_data)
{
  refresh_drawing_area();
  
  //progress position
  gfloat adj_position =
    (gint)gtk_adjustment_get_value(GTK_ADJUSTMENT(progress_adj));
  
  //we get out the hundredth
  player_hundr_secs2 = (gint)current_time % 100;
  
  gint tt2;
  //we keep only the seconds
  tt2 = total_time / 100;
  current_time = (adj_position * tt2) / 100000;
  
  player_seconds2 = (gint)current_time % 60;
  player_minutes2 = (gint)current_time / 60;
  
  current_time = get_elapsed_time();
  
  check_update_down_progress_bar();
}

//!scroll event for the progress bar
gboolean progress_bar_scroll_event (GtkWidget *widget,
                                    GdkEventScroll *event,
                                    gpointer user_data)
{
  //--
  return FALSE;
}

//!when we enter the progress bar
gboolean progress_bar_enter_event (GtkWidget *widget,
                                   GdkEventCrossing *event,
                                   gpointer user_data)
{
  //--
  return FALSE;
}

//!when we leave the progress bar
gboolean progress_bar_leave_event (GtkWidget *widget,
                                   GdkEventCrossing *event,
                                   gpointer user_data)
{
  //--
  return FALSE;
}

//!song progress bar
GtkWidget *create_song_bar_hbox()
{
  GtkWidget *song_bar_hbox;

  //our progress bar
  song_bar_hbox = gtk_hbox_new (FALSE, 0);
  progress_adj = (GtkWidget *)gtk_adjustment_new (0.0, 0.0, 100001.0, 0, 10000, 1000);
  progress_bar = gtk_hscale_new (GTK_ADJUSTMENT (progress_adj));
  g_object_set(progress_bar, "draw-value", FALSE, NULL);
  //when we click on the bar
  g_signal_connect (G_OBJECT (progress_bar), "button-press-event",
                    G_CALLBACK (progress_bar_click_event), NULL);
  //when we unclick on the bar
  g_signal_connect (G_OBJECT (progress_bar), "button-release-event",
                    G_CALLBACK (progress_bar_unclick_event), NULL);
  //when are on the bar
  g_signal_connect (G_OBJECT (progress_bar), "enter-notify-event",
                    G_CALLBACK (progress_bar_enter_event), NULL);
  //when move away from the bar
  g_signal_connect (G_OBJECT (progress_bar), "leave-notify-event",
                    G_CALLBACK (progress_bar_leave_event), NULL);
  //when the bar is modified
  g_signal_connect (G_OBJECT (progress_bar), "value-changed",
                    G_CALLBACK (progress_bar_value_changed_event), NULL);
  //when we scroll
  g_signal_connect (G_OBJECT (progress_bar), "scroll-event",
                    G_CALLBACK (progress_bar_scroll_event), NULL);

  gtk_widget_set_sensitive(GTK_WIDGET(progress_bar), FALSE);

  gtk_box_pack_start (GTK_BOX (song_bar_hbox), progress_bar, TRUE, TRUE, 5);

  return song_bar_hbox;
}

//!prints information about the song, frequency, kbps, stereo
void print_about_the_song()
{
  gchar total_infos[512];
  
  player_get_song_infos(total_infos);
  
  gtk_label_set_text(GTK_LABEL(song_infos), total_infos);
}

//!prints the player filename
void print_player_filename()
{
  gchar *fname = player_get_filename();
  
  if ((fname != NULL) &&
      (strcmp(fname, "disconnect")))
  {
    change_current_filename(fname);
  }
  
  gchar *title;
  title = player_get_title();
  gchar new_title[90];
  g_snprintf(new_title,75, "%s",title);
  if (title != NULL)
    {
      if (strlen(title) > 75)
        {
          new_title[strlen(new_title)-1] = '.';
          new_title[strlen(new_title)-2] = '.';
          new_title[strlen(new_title)-3] = '.';
        }
    }
  gtk_label_set_text(GTK_LABEL(song_name_label), 
                     new_title);
  
  g_free(fname);
  g_free(title);
}

/*! get time elapsed from the song and print it on the screen

Also prints filename, frequency, bitrate, mono, stereo
*/
void print_all_song_infos()
{
  //prints frequency, stereo, etc
  print_about_the_song();
  print_player_filename();
}

/*! prints the song time elapsed

\param i 
 - 0 means normal state
 - 1 means we reset the time
*/
void print_song_time_elapsed()
{
  //temp is temporary
  gint time, temp;
  gchar seconds[16], minutes[16], seconds_minutes[64];
  
  time = player_get_elapsed_time();
  
  //the hundredth of seconds
  player_hundr_secs = (time % 1000) / 10;
  
  temp = (time/1000)/60;
  //sets the global variables
  //for the minutes and the seconds
  player_minutes = temp;
  player_seconds = (time/1000) - (temp*60); 
  //calculate time and print time
  g_snprintf(minutes, 16, "%d", temp);
  g_snprintf(seconds, 16, "%d", (time/1000) - (temp*60));
  
  //variables for the total time
  gchar total_seconds[16], total_minutes[16];
  
  gint tt;
  tt = total_time * 10;
  temp = (tt/1000)/60;
  
  //calculate time and print time
  g_snprintf(total_minutes, 16, "%d", temp);
  g_snprintf(total_seconds, 16, "%d", (tt/1000) - (temp*60));
  g_snprintf(seconds_minutes, 64, "%s  :  %s  /  %s  :  %s", 
             minutes, seconds, total_minutes, total_seconds);
      
  gtk_label_set_text(GTK_LABEL(label_time), seconds_minutes);
}

//!change volume to match the players volume
void change_volume_bar()
{
  if (player_is_running())
    {
      //players volume
      gint volume;
      volume = player_get_volume();
      
      if (volume != -1)
        gtk_adjustment_set_value(GTK_ADJUSTMENT(volume_adj),
                                 volume);
    }
}

//!progress bar synchronisation with player
void change_progress_bar()
{
  if ((player_is_running())
      && (!mouse_on_progress_bar))
    {
      //new position of the progress bar
      gdouble adj_position;
      
      //total time in hundredths of seconds
      total_time = player_get_total_time() / 10;
      
      current_time = ((player_seconds + player_minutes*60)*100
                      + player_hundr_secs);
      
      adj_position = (current_time *100000) / total_time;
      
      gtk_adjustment_set_value(GTK_ADJUSTMENT(progress_adj),
                               (gdouble) adj_position);
      
      current_time = get_elapsed_time();
      //we check if the current time is between the preview
      //splitpoints, we cancel the preview
      gint stop_splitpoint
        = get_splitpoint_time(quick_preview_end_splitpoint) 
        / 10;
      gint start_splitpoint
        = get_splitpoint_time(preview_start_splitpoint) 
        / 10;
      if ((stop_splitpoint < (gint)(current_time-150))
          || (start_splitpoint > (gint)(current_time+150)))
        {
          cancel_quick_preview();
        }
    }
}

//!creates the filename player hbox
GtkWidget *create_filename_player_hbox()
{
  GtkWidget *filename_player_hbox;

  //horizontal filename's player box and filename label(song_name_label)
  filename_player_hbox = gtk_hbox_new (FALSE, 0);
  song_name_label = gtk_label_new ("");
  g_object_set(G_OBJECT(song_name_label), "selectable", FALSE, NULL);
  gtk_box_pack_start (GTK_BOX (filename_player_hbox), song_name_label, FALSE, FALSE, 15);

  return filename_player_hbox;
}

//!changes the volume of the player
void change_volume_event(GtkWidget *widget,
                         gpointer data)
{
  if (gtk_widget_get_sensitive(volume_bar))
    {
      gint volume_adj_position;
      volume_adj_position = (gint)gtk_adjustment_get_value(GTK_ADJUSTMENT(volume_adj));
      player_set_volume(volume_adj_position);
    }
}

//!when we unclick the volume bar
gboolean volume_bar_unclick_event (GtkWidget *widget,
                                   GdkEventCrossing *event,
                                   gpointer user_data)
{
  change_volume = TRUE;
  return FALSE;
}

//!when we click the volume bar
gboolean volume_bar_click_event (GtkWidget *widget,
                                   GdkEventCrossing *event,
                                   gpointer user_data)
{
  change_volume = FALSE;
  return FALSE;
}

//!when we enter the volume bar
gboolean volume_bar_enter_event (GtkWidget *widget,
                                 GdkEventCrossing *event,
                                 gpointer user_data)
{
  on_the_volume_bar = TRUE;
  //--
  return FALSE;
}

//!when we leave the volume bar
gboolean volume_bar_leave_event (GtkWidget *widget,
                                 GdkEventCrossing *event,
                                 gpointer user_data)
{
  on_the_volume_bar = FALSE;
  //--
  return FALSE;
}

//!scroll event for the volume bar
gboolean volume_bar_scroll_event (GtkWidget *widget,
                                  GdkEventScroll *event,
                                  gpointer user_data)
{
  change_volume_event(NULL,NULL);
  //--
  return FALSE;
}

//!creates the volume vertical bar
GtkWidget *create_volume_control_box()
{
  //our vertical box
  GtkWidget *vbox;

  vbox = gtk_vbox_new(FALSE, 0);
  volume_adj = (GtkWidget *)gtk_adjustment_new (0.0, 0.0, 101, 1, 23, 1);
  volume_bar = gtk_vscale_new (GTK_ADJUSTMENT (volume_adj));
  gtk_range_set_inverted(GTK_RANGE(volume_bar),TRUE);
  g_object_set(volume_bar, "draw-value", FALSE, NULL);
  //when we click on the volume
  g_signal_connect (G_OBJECT (volume_bar), "button-press-event",
                    G_CALLBACK (volume_bar_click_event), NULL);
  //when we unclick on the volume
  g_signal_connect (G_OBJECT (volume_bar), "button-release-event",
                    G_CALLBACK (volume_bar_unclick_event), NULL);
  //when are on the bar
  g_signal_connect (G_OBJECT (volume_bar), "enter-notify-event",
                    G_CALLBACK (volume_bar_enter_event), NULL);
  //when move away from the bar
  g_signal_connect (G_OBJECT (volume_bar), "leave-notify-event",
                    G_CALLBACK (volume_bar_leave_event), NULL);
  //when we scroll
  g_signal_connect (G_OBJECT (volume_bar), "scroll-event",
                    G_CALLBACK (volume_bar_scroll_event), NULL);
  g_signal_connect (G_OBJECT (volume_adj), "value_changed",
                    G_CALLBACK (change_volume_event), NULL);
  gtk_widget_set_sensitive(GTK_WIDGET(volume_bar), FALSE);

  gtk_box_pack_start (GTK_BOX (vbox), volume_bar, TRUE, TRUE, 0);

  return vbox;
}

//!when closing the new window after detaching
void close_player_popup_window_event( GtkWidget *window,
                                      gpointer data )
{
  GtkWidget *window_child;

  window_child = gtk_bin_get_child(GTK_BIN(window));

  gtk_widget_reparent(GTK_WIDGET(window_child), GTK_WIDGET(player_handle));

  gtk_widget_destroy(window);
}

//!when we detach the handle
void handle_player_detached_event(GtkHandleBox *handlebox,
                                  GtkWidget *widget,
                                  gpointer data)
{
  //new window
  GtkWidget *window;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  gtk_widget_reparent(GTK_WIDGET(widget), GTK_WIDGET(window));

  g_signal_connect(G_OBJECT(window), "delete_event",
                   G_CALLBACK(close_player_popup_window_event),
                   NULL);
  
  gtk_widget_show(GTK_WIDGET(window));
}

/*PangoLayout *get_drawing_text(gchar *str)
  {
  PangoContext *context = gtk_widget_create_pango_context(da);
  PangoFontDescription *desc = pango_font_description_from_string("Sans 9");
  PangoLayout *layout = pango_layout_new(context);

  pango_layout_set_text(layout, str,-1);
  pango_layout_set_font_description (layout, desc);

  g_object_unref (context);
  pango_font_description_free (desc);

  return layout;
  }*/

//!returns the value of the right drawing area
gfloat get_right_drawing_time()
{
  gfloat left = 0;
  gfloat right = total_time / zoom_coeff;
  gfloat center = (right - left) / 2;
  gfloat offset = current_time - center;
  right += offset;
  
  return right;
}

//!returns the value of the left drawing area
gfloat get_left_drawing_time()
{
  gfloat right = total_time / zoom_coeff;
  gfloat center = right/2;
  gfloat left = current_time - center;
  
  return left;
}

/*!returns the hundreth of seconds rest of a time value

\param time The time in hundreths of a second
*/
gint get_time_hundrsecs(gint time)
{
  return time % 100;
}

/*!returns the seconds rest of a time

\param time The time in hundreths of a second
*/
gint get_time_secs(gint time)
{
  return (time / 100) % 60;
}

/*!returns the minutes of a time 

\param time The time in hundreth of a second
*/
gint get_time_mins(gint time)
{
  return time / 6000;
}

/*!Converts a time value to a string we can display 

\param hundr_or_not = TRUE means we also draw hundr of secs
\param time The time in hundreths of a second
*/
gchar *get_time_for_drawing(gchar *str,
                            gint time,
                            gboolean hundr_or_not,
                            gint *number_of_chars)
{
  gint mins = get_time_mins(time);
  gint secs = get_time_secs(time);
  
  if (hundr_or_not)
    {
      gint hundr = get_time_hundrsecs(time);
      *number_of_chars = 
        g_snprintf(str,30, "%d:%02d:%02d",
                   mins,secs,hundr);
    }
  else
    {
      *number_of_chars = 
        g_snprintf(str,30, "%d:%02d",
                   mins,secs);
    }
  
  return str;
}

//!Convert time into to pixels
gint time_to_pixels(gint width, gfloat time)
{
  return (width * time * zoom_coeff)/total_time;
}

//transform pixels to time
gfloat pixels_to_time(gfloat width, gint pixels)
{
  return (total_time * (gfloat)pixels)/(width * zoom_coeff);
}

/*!returns the position of a time mark on the screen

\param width The width of the drawing 
\param The time in hundreths of a second
*/
gint get_draw_line_position(gint width, gfloat time)
{
  //position to return
  gint position;
  
  gfloat offset_time = time - current_time;
  gint offset_pixel = time_to_pixels(width, offset_time);
  position = width/2 + offset_pixel;
  
  return position;
}

static void set_color(cairo_t *cairo, GdkColor *color)
{
  gdk_cairo_set_source_color(cairo, color);
}

static void draw_rectangle(cairo_t *cairo, gboolean filled, gint x, gint y, 
    gint width, gint height)
{
  cairo_rectangle(cairo, x, y, width, height);

  if (filled)
  {
    cairo_fill(cairo);
  }
}

static void draw_arc(cairo_t *cairo, gboolean filled, gint x, gint y,
    double radius, double angle1, double angle2)
{
  cairo_arc(cairo, x, y, radius, angle1, angle2);

  if (filled)
  {
    cairo_fill(cairo);
  }
}

static void draw_text(cairo_t *cairo, const gchar *text, gint x, gint y)
{
  cairo_select_font_face(cairo, "Sans 11", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cairo, 11.0);

  cairo_move_to(cairo, x, y + 13);
  cairo_show_text(cairo, text);
}

static void draw_line(cairo_t *cairo, gint x1, gint y1, gint x2, gint y2,
    gboolean line_is_dashed)
{
  double dashes[] = { 1.0, 3.0 };
  if (line_is_dashed)
  {
    cairo_set_dash(cairo, dashes, 2, -50.0);
  }
  else
  {
    cairo_set_dash(cairo, dashes, 0, 0.0);
  }

  cairo_set_line_width(cairo, 1.3);
  cairo_set_line_cap(cairo, CAIRO_LINE_CAP_ROUND);
  cairo_move_to(cairo, x1, y1);
  cairo_line_to(cairo, x2, y2);
  cairo_stroke(cairo);
}

static void draw_point(cairo_t *cairo, gint x, gint y)
{
  draw_line(cairo, x, y, x, y, FALSE);
}

void draw_motif(GtkWidget *da, cairo_t *gc, gint ylimit, gint x, gint model)
{
  GdkColor color;
  switch(model){
  case 0:
    //hundreths
    color.red = 65000;color.green = 0;color.blue = 0;
    break;
  case 1:
    //tens of seconds
    color.red = 0;color.green = 0;color.blue = 65000;
    break;
  case 2:
    //seconds
    color.red = 0;color.green = 65000;color.blue = 0;
    break;
  case 3:
    //ten seconds
    color.red = 65000;color.green = 0;color.blue = 40000;
    break;
  case 4:
    //minutes
    color.red = 1000;color.green = 10000;color.blue = 65000;
    break;
  case 5:
    //ten minutes
    color.red = 65000;color.green = 0;color.blue = 0;
    break;
  default:
    //hours
    color.red = 0;color.green = 0;color.blue = 0;
    break;
  }

  set_color (gc, &color);

  draw_point(gc,x,ylimit+6);
  draw_point(gc,x,ylimit+7);
  draw_point(gc,x,ylimit+8);
  draw_point(gc,x-1,ylimit+8);
  draw_point(gc,x+1,ylimit+8);
  draw_point(gc,x,ylimit+9);
  draw_point(gc,x-1,ylimit+9);
  draw_point(gc,x+1,ylimit+9);
  draw_point(gc,x-2,ylimit+9);
  draw_point(gc,x+2,ylimit+9);
  draw_point(gc,x-3,ylimit+9);
  draw_point(gc,x+3,ylimit+9);
  draw_point(gc,x,ylimit+10);
  draw_point(gc,x-1,ylimit+10);
  draw_point(gc,x+1,ylimit+10);
  draw_point(gc,x-2,ylimit+10);
  draw_point(gc,x+2,ylimit+10);
  draw_point(gc,x-3,ylimit+10);
  draw_point(gc,x+3,ylimit+10);

  color.red = 0;color.green = 0;color.blue = 0;
  set_color(gc, &color);
}

//!draw the marks, minutes, seconds...
void draw_marks(gint time_interval, gint left_mark,
                gint right_mark, gint ylimit,
                GtkWidget *da, cairo_t *gc)
{
  gint left2 = (left_mark/time_interval) * time_interval;
  if (left2 < left_mark)
    left2 += time_interval;
  
  gint i;
  gint i_pixel;
  for (i=left2;i<=right_mark;i+=time_interval)
    {
      i_pixel = get_draw_line_position(width_drawing_area,i);
      
      switch(time_interval){
      case 1:
        draw_motif(da, gc, ylimit, i_pixel,0);
        break;
      case 10:
        draw_motif(da, gc, ylimit,i_pixel,1);
        break;
      case 100:
        draw_motif(da, gc, ylimit,i_pixel,2);
        break;
      case 1000:
        draw_motif(da, gc, ylimit,i_pixel,3);
        break;
      case 6000:
        draw_motif(da, gc, ylimit,i_pixel,4);
        break;
      case 60000:
        draw_motif(da, gc, ylimit,i_pixel,5);
        break;
      default:
        draw_motif(da, gc, ylimit,i_pixel,6);
        break;
      }
    }
}

//!full cancel of the quick preview
void cancel_quick_preview_all()
{
  cancel_quick_preview();
  quick_preview_end_splitpoint = -1;
  preview_start_splitpoint = -1;
}

//!cancels quick preview
void cancel_quick_preview()
{
  quick_preview = FALSE;
}

/*!motif for splitpoints

\param draw false if we draw the splitpoint we move
\param move FALSE means we don't move the splitpoint,
\param move = TRUE means we move the splitpoint
\param number_splitpoint is the current splitpoint we draw
\param splitpoint_checked = TRUE if the splitpoint is checked
*/
void draw_motif_splitpoints(GtkWidget *da, cairo_t *gc,
                            gint x,gint draw,
                            gint current_point_hundr_secs,
                            gboolean move,
                            gint number_splitpoint)
{
  int m = margin - 1;
  GdkColor color;
  Split_point point = g_array_index(splitpoints, Split_point, number_splitpoint);
  gboolean splitpoint_checked = point.checked;
  
  //top color
  color.red = 255 * 212;
  color.green = 255 * 100;
  color.blue = 255 * 200;
  //set the color for the graphic context
  set_color (gc, &color);
  
  //if it' the splitpoint we move, don't fill in the circle and
  //the square
  if (!draw)
  {
    //top buttons
    draw_rectangle (gc,
        FALSE, x-6,4,
        11,11);
  }
  else
  {
    //top buttons
    draw_rectangle (gc,
        TRUE, x-6,4,
        12,12);
    //if it's the splitpoint selected
    if (number_splitpoint == get_first_splitpoint_selected())
    {
      //top color
      color.red = 255 * 220;
      color.green = 255 * 220;
      color.blue = 255 * 255;
      //set the color for the graphic context
      set_color (gc, &color);

      draw_rectangle (gc,
          TRUE, x-4,6,
          8,8);
    }
  }

  //default color
  color.red = 255 * 212;
  color.green = 255 * 196;
  color.blue = 255 * 221;
  //set the color for the graphic context
  set_color (gc, &color);
  
  gint i;
  for(i = 0;i<5;i++)
  {
    draw_point (gc,x+i,erase_split_ylimit + m + 3);
    draw_point (gc,x-i,erase_split_ylimit + m + 3);
    draw_point (gc,x+i,erase_split_ylimit + m + 4);
    draw_point (gc,x-i,erase_split_ylimit + m + 4);
  }
  
  //if we are currently moving this splitpoint
  if (move)
  {
    //we set the green or blue color
    if (splitpoint_checked)
    {
      color.red = 15000;color.green = 40000;color.blue = 25000;
    }
    else
    {
      color.red = 25000;color.green = 25000;color.blue = 40000;
    }
    set_color (gc, &color);

    draw_line(gc, x,erase_split_ylimit + m -8, x,progress_ylimit + m, TRUE);
  }
  
  color.red = 255 * 22;
  color.green = 255 * 35;
  color.blue = 255 * 91;
  //set the color for the graphic context
  set_color (gc, &color);
  
  //draw the splitpoint motif
  for (i = -3;i <= 1;i++)
  {
    draw_point (gc,x,erase_split_ylimit + m +i);
  }
  for (i = 2;i <= 5;i++)
  {
    draw_point (gc,x,erase_split_ylimit + m + i);
  }
  for (i = 3;i <= 4;i++)
  {
    draw_point (gc,x-1,erase_split_ylimit + m + i);
    draw_point (gc,x+1,erase_split_ylimit + m + i);
  }
  for (i = 6;i <= 11;i++)
  {
    draw_point (gc,x,erase_split_ylimit + m + i);
  }
  
  //bottom splitpoint vertical bar
  for (i = 0;i < margin;i++)
  {
    draw_point (gc,x,progress_ylimit + m - i);
  }

  //bottom checkbox vertical bar
  for (i = 0;i < margin;i++)
  {
    draw_point (gc,x,splitpoint_ypos + m - i - 1);
  }

  //bottom rectangle
  set_color (gc, &color);
  color.red = 25000;color.green = 25000;color.blue = 25000;
  //bottom check rectangle
  draw_rectangle (gc,
      FALSE, x-6,splitpoint_ypos + m, 12,12);

  //draw a cross with 2 lines if the splitpoint is checked
  if (splitpoint_checked)
  {
    //
    gint left = x - 6;
    gint right = x + 6;
    //
    gint top = splitpoint_ypos + m;
    gint bottom = splitpoint_ypos + m + 12;
    draw_line(gc, left, top, right, bottom, FALSE);
    draw_line(gc, left, bottom, right, top, FALSE);
  }
  
  //we set the color
  //-if the splitpoint is checked, set green color
  if (splitpoint_checked)
  {
    color.red = 15000;color.green = 40000;color.blue = 25000;
  }
  else
  {
    color.red = 25000;color.green = 25000;color.blue = 40000;
  }
  set_color(gc, &color);
  
  draw_arc(gc, FALSE, x, progress_ylimit + m+ 1 + 7, 14 / 2, 0, 2 * G_PI);

  //only fill the circle if we don't move that splitpoint
  if (draw)
  {
    draw_arc(gc, TRUE, x, progress_ylimit + m + 1 + 8, 16 / 2, 0, 2 * G_PI);
  }
  
  if (draw)
  {
    gint number_of_chars = 0;
    gchar str[30] = { '\0' };
    get_time_for_drawing(str, current_point_hundr_secs, TRUE, &number_of_chars);
    draw_text(gc, str, x - (number_of_chars * 3), checkbox_ypos + margin - 1);
  }

  if (show_silence_wave)
  {
    //we set the black color
    color.red = 0;color.green = 0;color.blue = 0;
    set_color(gc, &color);

    gboolean dashed = FALSE;
    if (move) { dashed = TRUE; }
    draw_line(gc, x,text_ypos + margin, x,wave_ypos, dashed);
  }
}

//!left, right mark in hundreths of seconds
void draw_splitpoints(gint left_mark, gint right_mark, GtkWidget *da, cairo_t *gc)
{
  Split_point current_point;
  //current point in hundreth of seconds
  gint current_point_hundr_secs;
  
  gint i;
  //we get all splitpoints
  for(i = 0; i < splitnumber; i++ )
  {
    current_point =
      g_array_index(splitpoints, Split_point, i);
    current_point_hundr_secs = 
      current_point.hundr_secs +
      current_point.secs * 100 +
      current_point.mins * 6000;

    //if the splitpoint is > left and < right
    //it must be visible !
    if ((current_point_hundr_secs <= right_mark)
        &&(current_point_hundr_secs >= left_mark))
    {
      //our split pixel (Ox)
      gint split_pixel;

      //if it's the splitpoint we move, we draw it differently
      gboolean draw = TRUE;
      if (splitpoint_to_move == i)
      {
        draw = FALSE;
      }

      split_pixel = 
        get_draw_line_position(width_drawing_area,
            current_point_hundr_secs);
      draw_motif_splitpoints(da, gc, split_pixel, draw,
          current_point_hundr_secs,
          FALSE, i);
    }
  }
}

gint get_silence_wave_coeff()
{
  gint points_coeff = 1;

  //num_of_points_coeff_f : ogg ~= 1, mp3 ~= 4
  gfloat num_of_points_coeff_f =
    ceil((number_of_silence_points / total_time) * 10);
  gint num_of_points_coeff = (gint) num_of_points_coeff_f;
  gint coeff_adjust = 4;
  if (num_of_points_coeff == 1)
  {
    coeff_adjust = 1;
  }

  if (total_draw_time < secs_th)
  {
    points_coeff = 1;
  }
  else if (total_draw_time < ten_secs_th)
  {
    points_coeff = 2 * num_of_points_coeff;
  }
  else if (total_draw_time < minutes_th)
  {
    points_coeff = 4 * coeff_adjust * num_of_points_coeff;
  }
  else if (total_draw_time < ten_minutes_th)
  {
    points_coeff = 8 * coeff_adjust * num_of_points_coeff;
  }
  else
  {
    points_coeff = 32 * coeff_adjust * num_of_points_coeff;
  }

  return points_coeff;
}

//! Draws the silence wave
void draw_silence_wave(gint left_mark, gint right_mark, GtkWidget *da, cairo_t *gc)
{
  if (!silence_points || we_scan_for_silence)
  {
    return;
  }

  GdkColor color;
  color.red = 0;color.green = 0;color.blue = 0;
  set_color(gc, &color);

  gint i = 0;
  gint points_coeff = get_silence_wave_coeff();

  gint times = 0;

  gint previous_x = 0;
  gint previous_y = 0;

  for (i = 0;i < number_of_silence_points;i++)
  {
    long time = silence_points[i].time;
    float level = silence_points[i].level;

    if ((time <= right_mark) && (time >= left_mark))
    {

      if (i % points_coeff == 0)
      {
        gint x = get_draw_line_position(width_drawing_area, (gfloat) time);
        gint y = text_ypos + margin + (gint)floorf(level);

        if (times == 0)
        {
          cairo_move_to(gc, x, y);
        }
        else
        {
          draw_line(gc, previous_x, previous_y, x, y, FALSE);
        }

        previous_x = x;
        previous_y = y;

        times++;
      }

    }
  }
}

#if GTK_MAJOR_VERSION <= 2
gboolean da_draw_event(GtkWidget *da, GdkEventExpose *event, gpointer data)
{
  cairo_t *gc = gdk_cairo_create(da->window);
#else
gboolean da_draw_event(GtkWidget *da, cairo_t *gc, gpointer data)
{
#endif
  int width = 0, height = 0;
  wh_get_widget_size(da, &width, &height);
  if (show_silence_wave)
  {
    if (height != DRAWING_AREA_HEIGHT_WITH_SILENCE_WAVE)
    {
      gtk_widget_set_size_request(da, 
          DRAWING_AREA_WIDTH, DRAWING_AREA_HEIGHT_WITH_SILENCE_WAVE);
    }
  }
  else
  {
    if (height != DRAWING_AREA_HEIGHT)
    {
      gtk_widget_set_size_request(da, DRAWING_AREA_WIDTH, DRAWING_AREA_HEIGHT);
    }
  }

  //
  margin = 4;

  //
  real_erase_split_length = 12;
  real_progress_length = 26;
  real_move_split_length = 16;
  real_checkbox_length = 12;
  real_text_length = 12;
  real_wave_length = 96;

  gint erase_splitpoint_length = real_erase_split_length + (margin * 2);
  gint progress_length = real_progress_length + margin;
  gint move_split_length = real_move_split_length + margin;
  gint text_length = real_text_length + margin;
  gint checkbox_length = real_checkbox_length + margin;
  gint wave_length = real_wave_length + margin;

  //
  erase_split_ylimit = erase_splitpoint_length;
  progress_ylimit = erase_split_ylimit + progress_length;
  splitpoint_ypos = progress_ylimit + move_split_length;
  checkbox_ypos = splitpoint_ypos + checkbox_length;
  text_ypos = checkbox_ypos + text_length + margin;
  wave_ypos = text_ypos + wave_length + margin;

  gint bottom_left_middle_right_text_ypos = text_ypos;
  if (show_silence_wave)
  {
    bottom_left_middle_right_text_ypos = wave_ypos;
  }
  
  GdkColor color;
  gint nbr_chars = 0;
  
  wh_get_widget_size(da, &width_drawing_area, NULL);
  
  color.red = 255 * 235;color.green = 255 * 235;
  color.blue = 255 * 235;
  set_color (gc, &color);
  //background rectangle
  draw_rectangle (gc,
                      TRUE, 0,0,
                      width_drawing_area, wave_ypos + text_length + 2);

  color.red = 255 * 255;color.green = 255 * 255;color.blue = 255 * 255;
  set_color (gc, &color);
  
  //background white rectangles
  draw_rectangle (gc,
                      TRUE,
                      0,margin,
                      width_drawing_area,
                      real_erase_split_length);
  draw_rectangle (gc,
                      TRUE,
                      0,erase_split_ylimit,
                      width_drawing_area,
                      progress_length);
  draw_rectangle (gc,
                      TRUE,
                      0,progress_ylimit+margin,
                      width_drawing_area,
                      real_move_split_length);
  draw_rectangle (gc,
                      TRUE,
                      0,splitpoint_ypos+margin,
                      width_drawing_area,
                      real_checkbox_length);
  draw_rectangle (gc,
                      TRUE,
                      0,checkbox_ypos+margin,
                      width_drawing_area,
                      text_length);
  if (show_silence_wave)
  {
    draw_rectangle (gc, TRUE, 0, text_ypos + margin,
        width_drawing_area, wave_length);
  }
 
  //only if we are playing
  //and the timer active(connected to player)
  if(playing && timer_active)
  {
    gfloat left_time;
    gfloat right_time;
    gfloat center_time;
    left_time = get_left_drawing_time();
    right_time = get_right_drawing_time();
    center_time = current_time;

    //marks to draw seconds, minutes...
    gint left_mark = (gint)left_time;
    gint right_mark = (gint)right_time;
    if (left_mark < 0)
    {
      left_mark = 0;
    }
    if (right_mark > total_time)
    {
      right_mark = (gint)total_time;
    }

    //total draw time
    total_draw_time = right_time - left_time;

    gchar str[30] = { '\0' };
    gint beg_pixel = get_draw_line_position(width_drawing_area,0);

    gint splitpoint_time_left = -1;
    gint splitpoint_time_right = -1;
    gint splitpoint_pixels_left = -1;
    gint splitpoint_pixels_right = -1;
    gint splitpoint_pixels_length = -1;
    gint splitpoint_left_index = -1;
    get_splitpoint_time_left_right(&splitpoint_time_left,
        &splitpoint_time_right,
        &splitpoint_left_index);

    if ((splitpoint_time_left != -1) && 
        (splitpoint_time_right != -1))
    {
      //
      splitpoint_pixels_left = get_draw_line_position(width_drawing_area,
          splitpoint_time_left);
      splitpoint_pixels_right = get_draw_line_position(width_drawing_area,
          splitpoint_time_right);
      splitpoint_pixels_length = 
        splitpoint_pixels_right - splitpoint_pixels_left;

      //we put yellow rectangle between splitpoints
      //we set default black color
      color.red = 255 * 255;color.green = 255 * 255;
      color.blue = 255 * 210;
      //set the color for the graphic context
      set_color (gc, &color);
      draw_rectangle (gc,
          TRUE,splitpoint_pixels_left,
          erase_split_ylimit,
          splitpoint_pixels_length,
          progress_ylimit-
          erase_split_ylimit+1);
    }

    //we set blue color
    color.red = 255 * 150;
    color.green = 255 * 150;
    color.blue = 255 * 255;
    //set the color for the graphic context
    set_color (gc, &color);

    //if it's the first splitpoint from play preview
    if (quick_preview_end_splitpoint != -1)
    {
      gint right_pixel =
        get_draw_line_position(width_drawing_area,
            get_splitpoint_time(quick_preview_end_splitpoint)/10);
      gint left_pixel =
        get_draw_line_position(width_drawing_area,
            get_splitpoint_time(preview_start_splitpoint) /10);

      gint preview_splitpoint_length = 
        right_pixel - left_pixel + 1;

      //top buttons
      draw_rectangle (gc,
          TRUE, left_pixel,
          progress_ylimit-2,
          preview_splitpoint_length,3);

      //if we have a quick preview on going, put red bar
      if (quick_preview)
      {
        color.red = 255 * 255;color.green = 255 * 160;color.blue = 255 * 160;
        //set the color for the graphic context
        set_color (gc, &color);
        //top buttons
        draw_rectangle (gc,
            TRUE, left_pixel,
            erase_split_ylimit,
            preview_splitpoint_length,
            3);
      }
    }
    else
    {
      //if we draw until the end
      if ((preview_start_splitpoint != -1)&&
          (preview_start_splitpoint != (splitnumber-1)))
      {
        gint left_pixel =
          get_draw_line_position(width_drawing_area,
              get_splitpoint_time(preview_start_splitpoint) /10);
        //top buttons
        draw_rectangle (gc,
            TRUE, left_pixel,
            progress_ylimit-2,
            width_drawing_area-left_pixel,
            3);
        //if we have a quick preview on going, put red bar
        if (quick_preview)
        {
          color.red = 255 * 255;color.green = 255 * 160;color.blue = 255 * 160;
          //set the color for the graphic context
          set_color (gc, &color);
          //top buttons
          draw_rectangle (gc,
              TRUE, left_pixel,
              erase_split_ylimit,
              width_drawing_area-left_pixel,
              3);
        }
      }
    }

    //song start
    if ( left_time <= 0 )
    {
      color.red = 255 * 235;color.green = 255 * 235;
      color.blue = 255 * 235;
      //set the color for the graphic context
      set_color (gc, &color);
      draw_rectangle (gc,
          TRUE,
          0,0,
          beg_pixel,
          wave_ypos);
    }
    else
    {
      color.red = 30000;color.green = 0;color.blue = 30000;
      //set the color for the graphic context
      set_color (gc, &color);

      get_time_for_drawing(str, left_time, FALSE, &nbr_chars);
      draw_text(gc, str, 15, bottom_left_middle_right_text_ypos);
    }

    gint end_pixel = 
      get_draw_line_position(width_drawing_area,total_time);
    //song end
    if ( right_time >= total_time )
    {
      color.red = 255 * 235;color.green = 255 * 235;
      color.blue = 255 * 235;
      //set the color for the graphic context
      set_color (gc, &color);

      draw_rectangle (gc,
          TRUE, end_pixel,0,
          width_drawing_area,
          bottom_left_middle_right_text_ypos);
    }
    else
    {
      color.red = 30000;color.green = 0;color.blue = 30000;
      //set the color for the graphic context
      set_color (gc, &color);

      get_time_for_drawing(str, right_time, FALSE, &nbr_chars);
      draw_text(gc, str, width_drawing_area - 52, bottom_left_middle_right_text_ypos);
    }

    if (total_draw_time < hundr_secs_th)
    {
      //DRAW HUNDR OF SECONDS
      draw_marks(1, left_mark, right_mark,
          erase_split_ylimit+ progress_length/4,
          da, gc);
    }

    if (total_draw_time < tens_of_secs_th)
    {
      //DRAW TENS OF SECONDS
      draw_marks(10, left_mark, right_mark,
          erase_split_ylimit+ progress_length/4,
          da, gc);
    }

    if (total_draw_time < secs_th)
    {
      //DRAW SECONDS
      draw_marks(100, left_mark, right_mark,
          erase_split_ylimit+ progress_length/4,
          da, gc);
    }

    if (total_draw_time < ten_secs_th)
    {
      //DRAW TEN SECONDS
      draw_marks(1000,
          left_mark, right_mark,
          erase_split_ylimit+ progress_length/4,
          da, gc);
    }

    if (total_draw_time < minutes_th)
    {
      //DRAW MINUTES
      draw_marks(6000,
          left_mark, right_mark,
          erase_split_ylimit+ progress_length/4,
          da, gc);
    }

    if (total_draw_time < ten_minutes_th)
    {
      //DRAW TEN MINUTES
      draw_marks(60000,
          left_mark, right_mark,
          erase_split_ylimit+ progress_length/4,
          da, gc);
    }

    //DRAW HOURS
    draw_marks(100 * 3600,
        left_mark, right_mark,
        erase_split_ylimit+progress_length/4,
        da, gc);

    //draw mobile button1 position line
    if (button1_pressed)
    {
      gint move_time_bis = (gint)move_time;

      //if we don't move the splitpoints
      if (!move_splitpoints && !remove_splitpoints)
      {
        //if we have Audacious player selected as player,
        //we move only by seconds
        if (selected_player == PLAYER_AUDACIOUS)
          move_time_bis = (move_time_bis / 100) * 100;
      }

      gint move_pixel = 
        get_draw_line_position(width_drawing_area,
            move_time_bis);

      //if we move the splitpoints
      if (move_splitpoints)
      {
        draw_motif_splitpoints(da, gc, move_pixel,TRUE, move_time,
            TRUE, splitpoint_to_move);

        //we set default black color
        color.red = 0;color.green = 0;color.blue = 0;
        //set the color for the graphic context
        set_color (gc, &color);

        get_time_for_drawing(str, current_time, FALSE, &nbr_chars);
        draw_text(gc, str, width_drawing_area/2-11, bottom_left_middle_right_text_ypos);
      }
      else
      //we move the time
      { 
        //we set the red color
        color.red = 255 * 255;color.green = 0;color.blue = 0;
        set_color(gc, &color);

        draw_line(gc, move_pixel,erase_split_ylimit, move_pixel,progress_ylimit, TRUE);

        if (show_silence_wave)
        {
          draw_line(gc, move_pixel,text_ypos + margin, move_pixel,wave_ypos, TRUE);
        }

        //we set default black color
        color.red = 0;color.green = 0;color.blue = 0;
        //set the color for the graphic context
        set_color (gc, &color);

        get_time_for_drawing(str, move_time, FALSE, &nbr_chars);
        draw_text(gc, str, width_drawing_area/2-11, bottom_left_middle_right_text_ypos);
      }
    }
    else
    {
      //we set default black color
      color.red = 0;color.green = 0;color.blue = 0;
      //set the color for the graphic context
      set_color (gc, &color);

      get_time_for_drawing(str, center_time, FALSE, &nbr_chars);
      draw_text(gc, str, width_drawing_area/2-11, bottom_left_middle_right_text_ypos);
    }

    //we set default black color
    color.red = 0;color.green = 0;color.blue = 0;
    //set the color for the graphic context
    set_color (gc, &color);

    //we set the red color
    color.red = 255 * 255;color.green = 0;color.blue = 0;
    set_color(gc, &color);

    //the top middle line, current position
    draw_line(gc, width_drawing_area/2,erase_split_ylimit,
        width_drawing_area/2,progress_ylimit, FALSE);

    //we draw the silence wave if we have it 
    if (show_silence_wave)
    {
      draw_silence_wave(left_mark, right_mark, da, gc);

      //we set the red color
      color.red = 255 * 255;color.green = 0;color.blue = 0;
      set_color(gc, &color);

      //the draw silence wave middle line
      draw_line(gc, width_drawing_area/2,text_ypos + margin, width_drawing_area/2, wave_ypos, FALSE);
    }

    //we draw the splitpoints
    draw_splitpoints(left_mark, right_mark, da, gc);
  }
  else
  {      
    color.red = 255 * 212; color.green = 255 * 100; color.blue = 255 * 200;
    set_color (gc, &color);
    draw_text(gc, _(" left click on splitpoint selects it, right click erases it"),
        0, margin - 3);

    color.red = 0;color.green = 0;color.blue = 0;
    set_color (gc, &color);
    draw_text(gc, _(" left click + move changes song position, right click + move changes zoom"),
        0, erase_split_ylimit + margin);

    color.red = 15000;color.green = 40000;color.blue = 25000;
    set_color (gc, &color);
    draw_text(gc, 
        _(" left click on point + move changes point position, right click play preview"),
        0, progress_ylimit + margin);

    color.red = 0; color.green = 0; color.blue = 0;
    set_color (gc, &color);
    draw_text(gc, _(" left click on rectangle checks/unchecks 'keep splitpoint'"),
        0, splitpoint_ypos + 1);
  }
  
  return TRUE;
}

//returns the left splitpoint of the current play
void get_splitpoint_time_left_right(gint *time_left,
                                    gint *time_right,
                                    gint *splitpoint_left)
{
  gint i;
  Split_point current_point;
  gint current_point_hundr_secs;
  
  //we look at all splitpoints
  for(i = 0; i < splitnumber; i++ )
    {
      current_point =
        g_array_index(splitpoints, Split_point, i);
      current_point_hundr_secs = 
        current_point.hundr_secs +
        current_point.secs * 100 +
        current_point.mins * 6000;
      
      //if we found a valid splitpoint, we put them in a
      //list
      if (current_point_hundr_secs < current_time+DELTA)
        {
          *time_left = current_point_hundr_secs;
        }
      else
        {
          if (current_point_hundr_secs > current_time)
            {
              *time_right = current_point_hundr_secs;
              *splitpoint_left = i;
              break;
            }
        }
    }
  
  if (*splitpoint_left == -1)
    {
      *splitpoint_left = splitnumber;
    }
}

/*!Acquire the number of the splitpoint that has been clicked on

\param type_clicked
 - 3 means right button
 - 1 means left button
\param type
 - 1 means erase splitpoint area,
 - 2 means move splitpoint area,
 - 3 means check splitpoint area
*/
gint get_splitpoint_clicked(gint button_y, gint type_clicked,
                            gint type)
{
  //the time current position
  gint time_pos,time_right_pos,time_margin;
  gint left_time = get_left_drawing_time();
  
  gint but_y;
  
  //we see if we click on a right button
  if (type_clicked != 3)
  {
    but_y = button_y;
    time_pos = left_time + pixels_to_time(width_drawing_area,button_x);
  }
  else
  {
    but_y = button_y2;
    time_pos = left_time + pixels_to_time(width_drawing_area,button_x2);
  }

  //we get this to find time_right_pos - time_right
  //to see what time we have for X pixels
  gint pixels_to_look_for = real_erase_split_length / 2;
  if (type == 2)
  {
    pixels_to_look_for = real_move_split_length / 2;
  }

  if (type_clicked != 3)
  {
    time_right_pos = left_time+
      pixels_to_time(width_drawing_area,button_x + pixels_to_look_for);
  }
  else
  {
    time_right_pos = left_time+
      pixels_to_time(width_drawing_area,button_x2 + pixels_to_look_for);
  }

  //the time margin is the margin for the splitpoint,
  //where we can click at his left or right
  time_margin = time_right_pos - time_pos;
  
  gint margin1, margin2;
  
  if (type == 2)
  {
    margin1 = progress_ylimit + margin;
    margin2 = progress_ylimit + margin + real_move_split_length;
  }
  else if (type == 1)
  {
    margin1 = margin;
    margin2 = margin + real_erase_split_length;
  }
  else //if (type == 3)
  {
    margin1 = splitpoint_ypos + margin;
    margin2 = splitpoint_ypos + margin + real_checkbox_length;
  }

  gint splitpoint_returned = -1;
  
  //if we are in the area to move the split 
  if ((but_y > margin1) && (but_y < margin2))
  {
    //we check what splitpoints we found
    Split_point current_point;
    //current point in hundreth of seconds
    gint current_point_hundr_secs;
    gint current_point_left,current_point_right;

    gint i;
    //we look at all splitpoints
    for(i = 0; i < splitnumber; i++ )
    {
      current_point = g_array_index(splitpoints, Split_point, i);
      current_point_hundr_secs = current_point.hundr_secs +
        current_point.secs * 100 + current_point.mins * 6000;
      //left margin
      current_point_left = current_point_hundr_secs - time_margin;
      //right margin
      current_point_right = current_point_hundr_secs + time_margin;

      //if we found a valid splitpoint, we return it
      if ((time_pos >= current_point_left) && (time_pos <= current_point_right))
      {
        splitpoint_returned = i;
        break;
      }
    }
  }
  
  return splitpoint_returned;
}

//!makes a quick preview of the song
void player_quick_preview(gint splitpoint_to_preview)
{
  if (splitpoint_to_preview != -1)
  {
    preview_start_position = get_splitpoint_time(splitpoint_to_preview);
    preview_start_splitpoint = splitpoint_to_preview;

    if (!player_is_playing())
    {
      player_play();
      usleep(50000);
    }

    if (player_is_paused())
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pause_button), FALSE);
    }

    if (splitpoint_to_preview < splitnumber-1)
    {
      quick_preview_end_splitpoint = splitpoint_to_preview + 1;
    }
    else
    {
      quick_preview_end_splitpoint = -1;
    }

    player_jump(preview_start_position);
    change_progress_bar();
    put_status_message(_(" quick preview..."));

    quick_preview = FALSE;
    if (quick_preview_end_splitpoint != -1)
    {
      quick_preview = TRUE;
    }

    if (preview_start_splitpoint == (splitnumber-1))
    {
      cancel_quick_preview_all();
    }
  }
}

//!drawing area press event
gboolean da_press_event (GtkWidget    *da,
                         GdkEventButton *event,
                         gpointer     data)
{
  //only if we are playing
  //and the timer active(connected to player)
  if (playing && timer_active)
  {
    if (event->button == 1)
    {
      button_x = event->x;
      button_y = event->y;
      button1_pressed = TRUE;

      if ((button_y > progress_ylimit + margin) &&
          (button_y < progress_ylimit + margin + real_move_split_length))
      {
        splitpoint_to_move = get_splitpoint_clicked(button_y,1, 2);
        //if we have found splitpoints
        if (splitpoint_to_move != -1)
        {
          move_splitpoints = TRUE;
        }
      }
      else
      {
        //if we are in the area to remove a splitpoint
        if ((button_y > margin) && (button_y < margin + real_erase_split_length))
        {
          gint splitpoint_selected;
          //TRUE means remove splitpoint area
          splitpoint_selected = get_splitpoint_clicked(button_y, 1, 1);

          //if we have found a splitpoint to select
          if (splitpoint_selected != -1)
          {
            select_splitpoints = TRUE;
            select_splitpoint(splitpoint_selected);
          }
          //refresh the drawing area
          refresh_drawing_area();
        }
        else
        {
          //if we are in the area to check a splitpoint
          if ((button_y > splitpoint_ypos + margin) &&
              (button_y < splitpoint_ypos + margin + real_checkbox_length))
          {
            gint splitpoint_selected = get_splitpoint_clicked(button_y, 1, 3);
            if (splitpoint_selected != -1)
            {
              check_splitpoint = TRUE;
              update_splitpoint_check(splitpoint_selected);
            }
            refresh_drawing_area();
          }
        }
      }

      if (!move_splitpoints)
      {
        move_time = current_time;
      }
      else
      {
        move_time = get_splitpoint_time(splitpoint_to_move) / 10;
      }
    }
    else
    {
      //right click
      if (event->button == 3)
      {
        button_x2 = event->x;
        button_y2 = event->y;
        button2_pressed = TRUE;
        zoom_coeff_old = zoom_coeff;

      if ((button_y2 > progress_ylimit + margin) &&
          (button_y2 < progress_ylimit + margin + real_move_split_length))
        {
          gint splitpoint_to_preview = -1;

          splitpoint_to_preview = get_splitpoint_clicked(button_y2,3, 2);

          //player quick preview here!!
          player_quick_preview(splitpoint_to_preview);
        }
        else
        {
          //if we are in the area to remove a splitpoint
          if ((button_y2 > margin) && (button_y2 < margin + real_erase_split_length))
          {
            gint splitpoint_to_erase = -1;

            //TRUE means remove splitpoint area
            splitpoint_to_erase = get_splitpoint_clicked(button_y2,3, 1);

            //if we have found a splitpoint to erase
            if (splitpoint_to_erase != -1)
            {
              remove_splitpoints = TRUE;
              remove_splitpoint(splitpoint_to_erase,TRUE);
            }
            //refresh the drawing area
            refresh_drawing_area();
          }
        }
      }
    }
  }

  return TRUE;
}

//!drawing area release event
gboolean da_unpress_event (GtkWidget    *da,
                           GdkEventButton *event,
                           gpointer     data)
{
  //only if we are playing
  //and the timer active(connected to player)
  if (playing && timer_active)
  {
    if (event->button == 1)
    {
      button1_pressed = FALSE;
      //if we move the current _position_
      if (!move_splitpoints && !remove_splitpoints &&
          !select_splitpoints && !check_splitpoint)
      {
        remove_status_message();
        player_jump((gint)(move_time * 10));
        change_progress_bar();

        //if we have more than 2 splitpoints
        //if we are outside the split preview, we 
        //cancel split preview
        if (quick_preview_end_splitpoint == -1)
        {
          if (move_time < get_splitpoint_time(preview_start_splitpoint) /10)
          {
            cancel_quick_preview_all();
          }
        }
        else
        {
          if ((move_time < get_splitpoint_time(preview_start_splitpoint) /10) ||
              (move_time > get_splitpoint_time(quick_preview_end_splitpoint) /10))
          {
            cancel_quick_preview_all();
          }
          else
          //if we are inside, we turn on quick preview
          {
            //if we don't have a preview with the last
            //splitpoint
            if (quick_preview_end_splitpoint != -1)
            {
              //we unpause the player
              if (player_is_paused())
              {
                player_pause();
              }
              quick_preview = TRUE;
            }
          }
        }
      }
      else
      {
        //if we moved the splitpoint
        if (move_splitpoints)
        {
          //we update the current splitpoint
          update_splitpoint_from_time(splitpoint_to_move, move_time);
          splitpoint_to_move = -1;                
        }
      }
      move_splitpoints = FALSE;
      select_splitpoints = FALSE;
      check_splitpoint = FALSE;
    }
    else
    {
      if (event->button == 3)
      {
        button2_pressed = FALSE;
        remove_splitpoints = FALSE;
      }
    }
  }
  
  //refresh the drawing area
  refresh_drawing_area();
  
  return TRUE;
}

//!on drawing area event
gboolean da_notify_event (GtkWidget     *da,
                          GdkEventMotion *event,
                          gpointer      data)
{
  //only if we are playing
  //and the timer active(connected to player)
  if ((playing && timer_active) &&
      (button1_pressed || button2_pressed))
  {
    gint x, y;
    GdkModifierType state;
    gdk_window_get_pointer (event->window, &x, &y, &state);

    //drawing area width
    gint width = 0;
    wh_get_widget_size(da, &width, NULL);
    gfloat width_drawing_area = (gfloat) width;

    if (state)
    {
      //we push left button
      if (button1_pressed)
      {
        //if we move the splitpoints
        if (move_splitpoints)
        {
          gdouble splitpoint_time = 
            get_splitpoint_time(splitpoint_to_move) / 10;

          move_time = splitpoint_time + 
            pixels_to_time(width_drawing_area,(x - button_x));
        }
        else
        {
          //if we remove a splitpoint
          if (remove_splitpoints || select_splitpoints || check_splitpoint)
          {
            move_time = current_time;
          }
          else
          {
            move_time = current_time +
              pixels_to_time(width_drawing_area,(x - button_x));
          }
        }
        //if too left or too right
        if (move_time < 0)
        {
          move_time = 0;
        }
        if (move_time > total_time)
        {
          move_time = total_time;
        }
        //refresh the drawing area
        refresh_drawing_area();
      }
      else
      {
        if (button2_pressed)
        {
          gint diff = -((event->x - button_x2) * 1);

          if (diff < (-width_drawing_area + 1))
          {
            diff = -width_drawing_area + 1;
          }
          if (diff > (width_drawing_area - 1))
          {
            diff = width_drawing_area - 1;
          }

          zoom_coeff = diff / (width_drawing_area);

          if (zoom_coeff < 0)
          {
            zoom_coeff = 1/(zoom_coeff+1);
          }
          else
          {
            zoom_coeff = 1 - zoom_coeff;
          }

          zoom_coeff = zoom_coeff_old * zoom_coeff;

          if (zoom_coeff < 0.2)
          {
            zoom_coeff = 0.2;
          }
          if (zoom_coeff > 10 * total_time / 6000)
          {
            zoom_coeff = 10 * total_time / 6000;
          }

          //refresh the drawing area
          refresh_drawing_area();
        }
      }
    }
  }
  
  return TRUE;
}

//!creates the progress drawing area under the player buttons
GtkWidget *create_drawing_area()
{
  GtkWidget *frame = gtk_frame_new(NULL);
 
  GdkColor color;
  color.red = 65000;
  color.green = 0;
  color.blue = 0;
  gtk_widget_modify_bg(frame, GTK_STATE_NORMAL, &color);

  gtk_frame_set_shadow_type(GTK_FRAME(frame), GTK_SHADOW_NONE);

  da = gtk_drawing_area_new();

  gtk_widget_set_size_request(da, DRAWING_AREA_WIDTH, DRAWING_AREA_HEIGHT);

#if GTK_MAJOR_VERSION <= 2
  g_signal_connect(da, "expose_event", G_CALLBACK(da_draw_event), NULL);
#else
  g_signal_connect(da, "draw", G_CALLBACK(da_draw_event), NULL);
#endif

  g_signal_connect(da, "button_press_event", G_CALLBACK(da_press_event), NULL);
  g_signal_connect(da, "button_release_event", G_CALLBACK(da_unpress_event), NULL);
  g_signal_connect(da, "motion_notify_event", G_CALLBACK(da_notify_event), NULL);

  gtk_widget_set_events(da, gtk_widget_get_events(da)
      | GDK_LEAVE_NOTIFY_MASK | GDK_BUTTON_PRESS_MASK
      | GDK_BUTTON_RELEASE_MASK | GDK_POINTER_MOTION_MASK
      | GDK_POINTER_MOTION_HINT_MASK);

  gtk_container_add(GTK_CONTAINER(frame),da);
  
  return frame;
}

//!creates the control player frame, stop button, play button, etc.
GtkWidget *create_player_control_frame(GtkTreeView *tree_view)
{
  //main horizontal box of the frame (contains volume control + others)
  GtkWidget *main_hbox;
  //the vbox has hboxes in it
  GtkWidget *vbox;
  GtkWidget *hbox;
  //our vertical box for volume control
  GtkWidget *volume_control_vbox;
  //really big hbox, containing all from the frame
  GtkWidget *really_big_hbox;

  //really big hbox
  really_big_hbox = gtk_hbox_new(FALSE, 0);
  
  //main hbox
  main_hbox = gtk_hbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (really_big_hbox), main_hbox, TRUE, TRUE, 4);
  
  vbox = gtk_vbox_new (FALSE, 0);
  gtk_box_pack_start (GTK_BOX (main_hbox), vbox, TRUE, TRUE, 0);

  /* handle box for detaching */
  player_handle = gtk_handle_box_new();
  gtk_container_add(GTK_CONTAINER (player_handle), GTK_WIDGET(really_big_hbox));
  //handle event
  g_signal_connect(player_handle, "child-detached",
                   G_CALLBACK(handle_player_detached_event),
                   NULL);

  //the filename player hbox
  hbox = (GtkWidget *)create_filename_player_hbox();
  gtk_container_set_border_width (GTK_CONTAINER (hbox), 0);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 5);

  //the song informations
  hbox = (GtkWidget *)create_song_informations_hbox();
  gtk_container_set_border_width (GTK_CONTAINER (hbox), 0);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 3);

  //the vertical range progress scale
  //song progress bar
  hbox = (GtkWidget *)create_song_bar_hbox();
  gtk_container_set_border_width (GTK_CONTAINER (hbox), 0);
  gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, FALSE, 0);

  //horizontal drawing area
  hbox = (GtkWidget *)create_drawing_area();
  gtk_container_set_border_width (GTK_CONTAINER (hbox), 5);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);
  
  //our horizontal player button hbox
  hbox = (GtkWidget *)create_player_buttons_hbox(tree_view);
  gtk_container_set_border_width (GTK_CONTAINER (hbox), 0);
  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 5);
  
  //player volume control vbox
  volume_control_vbox = (GtkWidget *)create_volume_control_box();
  gtk_box_pack_start(GTK_BOX(main_hbox), volume_control_vbox, FALSE, FALSE, 0);
  
  return player_handle;
}

//!add a row to the table
void add_playlist_file(const gchar *name)
{
  if (is_filee(name))
  {
    //check if the name already exists in the playlist
    gboolean name_already_exists_in_playlist = FALSE;

    GtkTreeIter iter;
    GtkTreeModel *model;
    GtkTreeView *tree_view = (GtkTreeView *)playlist_tree;

    model = gtk_tree_view_get_model(GTK_TREE_VIEW(tree_view));

    gchar *filename = NULL;
    gint i = 0;
    GtkTreePath *path = NULL;
    //for all the files from the playlist,
    while (i < playlist_tree_number)
    {
      path = gtk_tree_path_new_from_indices(i ,-1);
      gtk_tree_model_get_iter(model, &iter, path);
      gtk_tree_model_get(model, &iter, COL_FILENAME, &filename, -1);
      if (strcmp(filename,name) == 0)
      {
        name_already_exists_in_playlist = TRUE;
        break;
      }
      g_free(filename);
      i++;
    }

    if (! name_already_exists_in_playlist)
    {
      gtk_widget_set_sensitive(playlist_remove_all_files_button,TRUE);
      gtk_list_store_append (GTK_LIST_STORE(model), &iter);

      //sets text in the minute, second and milisecond column
      gtk_list_store_set (GTK_LIST_STORE(model), 
          &iter,
          COL_NAME,get_real_name_from_filename((guchar *)name),
          COL_FILENAME,name,
          -1);
      playlist_tree_number++;
    }
  }
}

//!when closing the new window after detaching
void close_playlist_popup_window_event(GtkWidget *window,
                                       gpointer data)
{
  GtkWidget *window_child;

  window_child = gtk_bin_get_child(GTK_BIN(window));

  gtk_widget_reparent(GTK_WIDGET(window_child), GTK_WIDGET(playlist_handle));

  gtk_widget_destroy(window);
}


//!when we detach the handle
void handle_playlist_detached_event(GtkHandleBox *handlebox,
                                    GtkWidget *widget,
                                    gpointer data)
{
  //new window
  GtkWidget *window;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  gtk_widget_reparent(GTK_WIDGET(widget), GTK_WIDGET(window));

  g_signal_connect(G_OBJECT(window), "delete_event",
                   G_CALLBACK(close_playlist_popup_window_event),
                   NULL);
  
  gtk_widget_show(GTK_WIDGET(window));
}

//!creates the model for the playlist
GtkTreeModel *create_playlist_model()
{
  GtkListStore *model;

  model = gtk_list_store_new(PLAYLIST_COLUMNS,
                             G_TYPE_STRING,
                             G_TYPE_STRING);

  return GTK_TREE_MODEL(model);
}

//!creates the playlist tree
GtkTreeView *create_playlist_tree()
{
  GtkTreeView *tree_view;
  GtkTreeModel *model;

  //create the model
  model = (GtkTreeModel *)create_playlist_model();
  //create the tree view
  tree_view = (GtkTreeView *) gtk_tree_view_new_with_model(model);

  return tree_view;
}

//!creates playlist columns
void create_playlist_columns (GtkTreeView *tree_view)
{
  //cells renderer
  GtkCellRendererText *renderer;
  //columns
  GtkTreeViewColumn *name_column;
  //GtkTreeViewColumn *filename_column;

  /* minutes */
  //renderer creation
  renderer = GTK_CELL_RENDERER_TEXT(gtk_cell_renderer_text_new ());
  g_object_set_data(G_OBJECT(renderer), "col", GINT_TO_POINTER(COL_NAME));
  name_column = gtk_tree_view_column_new_with_attributes 
    (_("History"), GTK_CELL_RENDERER(renderer),
     "text", COL_NAME, NULL);

  //we dont insert the column to the tree view
  /*  renderer = GTK_CELL_RENDERER_TEXT(gtk_cell_renderer_text_new ());
      filename_column = gtk_tree_view_column_new_with_attributes 
      (_("Complete filename"), GTK_CELL_RENDERER(renderer),
      "text", COL_FILENAME,
      NULL);*/
  /*  gtk_tree_view_insert_column (GTK_TREE_VIEW (tree_view),
      GTK_TREE_VIEW_COLUMN (filename_column),COL_FILENAME);*/
  
  //appends columns to the list of columns of tree_view
  gtk_tree_view_insert_column (GTK_TREE_VIEW (tree_view),
                               GTK_TREE_VIEW_COLUMN (name_column),COL_NAME);

  //middle alignment of the column name
  gtk_tree_view_column_set_alignment(GTK_TREE_VIEW_COLUMN(name_column),
                                     0.5);
  gtk_tree_view_column_set_sizing (GTK_TREE_VIEW_COLUMN(name_column),
                                   GTK_TREE_VIEW_COLUMN_AUTOSIZE);
}

//!split selection has changed
void playlist_selection_changed(GtkTreeSelection *selec,
                                gpointer data)
{
  GtkTreeModel *model;
  GtkTreeSelection *selection;
  GList *selected_list = NULL;
  
  //get the model
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(playlist_tree));
  //get the selection
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(playlist_tree));
  //get selected rows
  selected_list = gtk_tree_selection_get_selected_rows(selection, &model);

  if (g_list_length(selected_list) > 0)
  {
    gtk_widget_set_sensitive(playlist_remove_file_button, TRUE);
  }
  else
  {
    gtk_widget_set_sensitive(playlist_remove_file_button, FALSE);
  }
}

//!event for the remove file button
void playlist_remove_file_button_event(GtkWidget *widget, gpointer data)
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  GtkTreePath *path;
  GList *selected_list = NULL;
  GList *current_element = NULL;
  GtkTreeSelection *selection;
  
  //get the model
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(playlist_tree));
  //get the selection
  selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(playlist_tree));
  //get selected rows
  selected_list = gtk_tree_selection_get_selected_rows(selection, &model);
  
  //the name of the file that we have clicked on
  gchar *filename = NULL;
  
  //while the list is not empty and we have numbers in the table
  //(splitnumber >0)
  while (g_list_length(selected_list) > 0)
    {
      //get the last element
      current_element = g_list_last(selected_list);
      path = current_element->data;
      //get the iter correspondig to the path
      gtk_tree_model_get_iter(model, &iter, path);
      gtk_tree_model_get(model, &iter, 
                         COL_FILENAME, &filename, -1);
      //remove the path from the selected list
      gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
      selected_list = g_list_remove(selected_list, path);
      //remove 1 to the row number of the table
      playlist_tree_number--;
      
      //free memory
      gtk_tree_path_free(path);
      g_free(filename);
    }
  
  if (playlist_tree_number == 0)
  {
    gtk_widget_set_sensitive(playlist_remove_all_files_button, FALSE);
  }
  
  gtk_widget_set_sensitive(playlist_remove_file_button,FALSE);
  
  //we free the selected elements
  g_list_foreach(selected_list, (GFunc)gtk_tree_path_free, NULL);
  g_list_free(selected_list);  
}

//!event for the remove file button
void playlist_remove_all_files_button_event(GtkWidget *widget, gpointer data)
{
  GtkTreeIter iter;
  GtkTreeModel *model;
  
  model = gtk_tree_view_get_model(GTK_TREE_VIEW(playlist_tree));
  
  //filename to erase
  gchar *filename = NULL;
  //for all the splitnumbers
  while (playlist_tree_number > 0)
  {
    gtk_tree_model_get_iter_first(model, &iter);
    gtk_tree_model_get(model, &iter, 
        COL_FILENAME, &filename, -1);
    gtk_list_store_remove(GTK_LIST_STORE(model), &iter);
    playlist_tree_number--;
    g_free(filename);
  }
  
  gtk_widget_set_sensitive(playlist_remove_all_files_button,FALSE);
  gtk_widget_set_sensitive(playlist_remove_file_button,FALSE);
}

//!creates the horizontal queue buttons horizontal box
GtkWidget *create_delete_buttons_hbox()
{
  //our horizontal box
  GtkWidget *hbox;
  hbox = gtk_hbox_new(FALSE,0);

  //button for removing a file
  playlist_remove_file_button = (GtkWidget *)
    create_cool_button(GTK_STOCK_DELETE, _("_Erase selected entries"),FALSE);
  gtk_box_pack_start (GTK_BOX (hbox),
                      playlist_remove_file_button, TRUE, FALSE, 5);
  gtk_widget_set_sensitive(playlist_remove_file_button,FALSE);
  g_signal_connect(G_OBJECT(playlist_remove_file_button), "clicked",
                   G_CALLBACK(playlist_remove_file_button_event), NULL);
  
  //button for removing a file
  playlist_remove_all_files_button = (GtkWidget *)
    create_cool_button(GTK_STOCK_DELETE, _("E_rase all history"),FALSE);
  gtk_box_pack_start (GTK_BOX (hbox),
                      playlist_remove_all_files_button, TRUE, FALSE, 5);
  gtk_widget_set_sensitive(playlist_remove_all_files_button,FALSE);
  g_signal_connect(G_OBJECT(playlist_remove_all_files_button), "clicked",
                   G_CALLBACK(playlist_remove_all_files_button_event), NULL);
  
  return hbox;
}


//!creates the playlist of the player
GtkWidget *create_player_playlist_frame()
{
  //the main vbox inside the handle
  GtkWidget *vbox = gtk_vbox_new(FALSE, 0);

  /* handle box for detaching */
  playlist_handle = gtk_handle_box_new();
  gtk_container_add(GTK_CONTAINER(playlist_handle), GTK_WIDGET(vbox));
  //handle event
  g_signal_connect(playlist_handle, "child-detached",
                   G_CALLBACK(handle_playlist_detached_event),
                   NULL);

  // scrolled window and the tree 
  //create the tree and add it to the scrolled window
  playlist_tree = (GtkWidget *) create_playlist_tree();
  //scrolled window for the tree
  GtkWidget *scrolled_window;
  scrolled_window = gtk_scrolled_window_new (NULL, NULL);
  gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW(scrolled_window), GTK_SHADOW_NONE);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_window),
                                  GTK_POLICY_AUTOMATIC,
                                  GTK_POLICY_AUTOMATIC);
  gtk_box_pack_start(GTK_BOX(vbox), scrolled_window, TRUE, TRUE, 0);
  //create columns
  create_playlist_columns(GTK_TREE_VIEW(playlist_tree));
  //add the tree to the scrolled window
  gtk_container_add(GTK_CONTAINER(scrolled_window), GTK_WIDGET(playlist_tree));
  g_signal_connect(G_OBJECT(playlist_tree), "row-activated",
                   G_CALLBACK(split_tree_row_activated), NULL);

  //selection for the tree
  GtkWidget *playlist_tree_selection;
  playlist_tree_selection = (GtkWidget *)
    gtk_tree_view_get_selection(GTK_TREE_VIEW(playlist_tree));
  g_signal_connect(G_OBJECT(playlist_tree_selection), "changed",
                   G_CALLBACK(playlist_selection_changed), NULL);
  gtk_tree_selection_set_mode(GTK_TREE_SELECTION(playlist_tree_selection),
                              GTK_SELECTION_MULTIPLE);

  //horizontal box with delete buttons
  GtkWidget *delete_buttons_hbox;
  delete_buttons_hbox = (GtkWidget *)create_delete_buttons_hbox();
  gtk_box_pack_start(GTK_BOX(vbox), delete_buttons_hbox, FALSE, FALSE, 5);

  return playlist_handle;
}

/*! timer used to print infos about the song

Examples are the elapsed time and if it uses variable bitrate
*/
gint mytimer(gpointer data)
{
  //if connected and player running
  if (player_is_running())
    {
      if (playing)
        {
          //if we have at least one song on the playlist
          if (player_get_playlist_number() > -1)
            {
              //if the player is playing, print the time
              if (player_is_playing())
                {
                  print_all_song_infos();
                  print_song_time_elapsed();
                  if(!gtk_widget_is_sensitive(progress_bar))
                    gtk_widget_set_sensitive(GTK_WIDGET(progress_bar), TRUE);
                }
              check_stream();
              //if we have a stream, we must not change the progress bar
              if(!stream)
                {
                  change_progress_bar();
                }
              
              //part of quick preview
              if (preview_start_splitpoint != -1)
                {
                  //if we have a splitpoint after the current
                  //previewed one, update quick_preview_end
                  if (preview_start_splitpoint+1 <
                      splitnumber)
                    {
                      quick_preview_end_splitpoint =
                        preview_start_splitpoint+1;
                    }
                  else
                    {
                      if (preview_start_splitpoint+1 == 
                          splitnumber)
                        {
                          quick_preview_end_splitpoint = -1;
                        }
                    }
                }
              
              //if we have a preview, stop if needed
              if (quick_preview)
                {
                  gint stop_splitpoint
                    = get_splitpoint_time(quick_preview_end_splitpoint) 
                    / 10;
                  
                  if ((stop_splitpoint < (gint)current_time)
                      && (quick_preview_end_splitpoint != -1))
                    {
                      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pause_button),
                                                   TRUE);
                      cancel_quick_preview();
                      put_status_message(_(" quick preview finished, song paused"));
                    }
                }
              
              //enable volume bar if needed
              if(!gtk_widget_is_sensitive(volume_bar))
                gtk_widget_set_sensitive(GTK_WIDGET(volume_bar), TRUE);
                
            }
          else
            {
              playing = FALSE;
              reset_label_time();
            }
          
          if (player_is_paused())
            {
              if (!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pause_button)))
                {
                  only_press_pause = TRUE;
                  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pause_button),
                                               TRUE);
                  only_press_pause = FALSE;
                }
            }
          else
            {
              if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(pause_button)))
                {
                  only_press_pause = TRUE;
                  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(pause_button),
                                               FALSE);
                  only_press_pause = FALSE;
                }
            }
        }
      else
        //if not playing but still connected
        {
          //reset player minutes and seconds if needed
          if ((player_minutes != 0) || (player_seconds != 0))
          {
            player_minutes = 0;
            player_seconds = 0;
          }
          print_player_filename();
          reset_song_infos();
          reset_label_time();
          //reset progress bar
          reset_inactive_progress_bar();
          gtk_widget_set_sensitive(player_add_button,
                                   FALSE);
        }
      
      //if connected,almost always change volume bar
      if ((change_volume)&&
          (!on_the_volume_bar))
        change_volume_bar();
      
      //always change the state, if playing or not
      playing = player_is_playing();
      
      //we set the add button to sensitive
      if (playing)
        {
          if (!gtk_widget_get_sensitive(player_add_button))
            {
              gtk_widget_set_sensitive(player_add_button, TRUE);
            }
            if (!gtk_widget_get_sensitive(silence_wave_check_button))
            {
              gtk_widget_set_sensitive(silence_wave_check_button, TRUE);
            }
        }
      
      return TRUE;
    }
  else
    //if connected and player not running, disconnect..
    {
      //clear player data
      clear_data_player();
      //setting playing to false
      playing = FALSE;
      
      //disconnect from player
      disconnect_button_event(disconnect_button, NULL);
      return FALSE;
    }
}


/*!event for the file chooser cancel button

Moved here from the file tab
*/
void file_chooser_cancel_event()
{
  gtk_widget_set_sensitive(browse_button, TRUE);
}

//event for the file chooser ok button
void file_chooser_ok_event(gchar *fname)
{
  change_current_filename(fname);
  gtk_widget_set_sensitive(browse_button, TRUE);
  gtk_widget_set_sensitive(play_button, TRUE);
  gtk_button_set_image(GTK_BUTTON(play_button), g_object_ref(PlayButton_active));

  file_browsed = TRUE;

  if (timer_active)
  {
    GList *song_list = NULL;
    song_list = g_list_append(song_list, fname);
    player_start_add_files(song_list);
  }
}

/*! \brief Events for browse button

Also used for the cddb and cue browses.
*/
void browse_button_event(GtkWidget *widget, gpointer data)
{
  gtk_widget_set_sensitive(widget, FALSE);

  GtkWidget *file_chooser = gtk_file_chooser_dialog_new(_("Choose File"),
      NULL,
      GTK_FILE_CHOOSER_ACTION_OPEN,
      GTK_STOCK_CANCEL,
      GTK_RESPONSE_CANCEL,
      GTK_STOCK_OPEN,
      GTK_RESPONSE_ACCEPT,
      NULL);

  wh_set_browser_directory_handler(ui, file_chooser);

  GtkWidget *our_filter = (GtkWidget *)gtk_file_filter_new();
  gtk_file_filter_set_name (GTK_FILE_FILTER(our_filter), _("mp3 and ogg files (*.mp3 *.ogg)"));
  gtk_file_filter_add_pattern(GTK_FILE_FILTER(our_filter), "*.mp3");
  gtk_file_filter_add_pattern(GTK_FILE_FILTER(our_filter), "*.ogg");
  gtk_file_filter_add_pattern(GTK_FILE_FILTER(our_filter), "*.MP3");
  gtk_file_filter_add_pattern(GTK_FILE_FILTER(our_filter), "*.OGG");
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_chooser), GTK_FILE_FILTER(our_filter));

  our_filter = (GtkWidget *)gtk_file_filter_new();
  gtk_file_filter_set_name (GTK_FILE_FILTER(our_filter), _("mp3 files (*.mp3)"));
  gtk_file_filter_add_pattern(GTK_FILE_FILTER(our_filter), "*.mp3");
  gtk_file_filter_add_pattern(GTK_FILE_FILTER(our_filter), "*.MP3");
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_chooser), GTK_FILE_FILTER(our_filter));

  our_filter = (GtkWidget *)gtk_file_filter_new();
  gtk_file_filter_set_name (GTK_FILE_FILTER(our_filter), _("ogg files (*.ogg)"));
  gtk_file_filter_add_pattern(GTK_FILE_FILTER(our_filter), "*.ogg");
  gtk_file_filter_add_pattern(GTK_FILE_FILTER(our_filter), "*.OGG");
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_chooser), GTK_FILE_FILTER(our_filter));

  if (gtk_dialog_run(GTK_DIALOG(file_chooser)) == GTK_RESPONSE_ACCEPT)
  {
    gchar *filename =
      gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));

    file_chooser_ok_event(filename);

    if (filename)
    {
      g_free(filename);
      filename = NULL;
    }
  }
  else
  {
    file_chooser_cancel_event();
  }

  gtk_widget_destroy(file_chooser);
  remove_status_message();
}

//when closing the new window after detaching
void close_file_popup_window_event( GtkWidget *window,
                                    gpointer data )
{
  GtkWidget *window_child;

  window_child = gtk_bin_get_child(GTK_BIN(window));

  gtk_widget_reparent(GTK_WIDGET(window_child), GTK_WIDGET(file_handle_box));

  gtk_widget_destroy(window);
}

//!when we detach the handle
void handle_file_detached_event (GtkHandleBox *handlebox,
                                 GtkWidget *widget,
                                 gpointer data)
{
  //new window
  GtkWidget *window;

  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

  gtk_widget_reparent(GTK_WIDGET(widget), GTK_WIDGET(window));

  g_signal_connect (G_OBJECT (window), "delete_event",
                    G_CALLBACK (close_file_popup_window_event),
                    NULL);

  gtk_widget_show(GTK_WIDGET(window));
}

/*!fix ogg stream action

we split from 0 to a big number
*/
gpointer fix_ogg_stream(gpointer data)
{
  we_are_splitting = TRUE;

  enter_threads();

  gtk_widget_set_sensitive(GTK_WIDGET(fix_ogg_stream_button), FALSE);
  put_options_from_preferences();

  exit_threads();

  gint err = 0;

  mp3splt_erase_all_splitpoints(the_state,&err);
  
  mp3splt_append_splitpoint(the_state, 0, NULL, SPLT_SPLITPOINT);
  mp3splt_append_splitpoint(the_state, LONG_MAX-1, NULL, SPLT_SKIPPOINT);
 
  mp3splt_set_int_option(the_state, SPLT_OPT_OUTPUT_FILENAMES,
                         SPLT_OUTPUT_DEFAULT);
  mp3splt_set_int_option(the_state, SPLT_OPT_SPLIT_MODE,
                         SPLT_OPTION_NORMAL_MODE);
 
  enter_threads();

  remove_all_split_rows();  
  filename_to_split = (gchar *) inputfilename_get();

  exit_threads();
  
  gint confirmation = SPLT_OK;
  mp3splt_set_path_of_split(the_state,filename_path_of_split);
  mp3splt_set_filename_to_split(the_state,filename_to_split);
  confirmation = mp3splt_split(the_state);
  
  enter_threads();

  print_status_bar_confirmation(confirmation);
  gtk_widget_set_sensitive(GTK_WIDGET(fix_ogg_stream_button), TRUE);

  exit_threads();

  we_are_splitting = FALSE;

  return NULL;
}

//!we make a thread with fix_ogg_stream
void fix_ogg_stream_button_event(GtkWidget *widget, gpointer   data)
{
  create_thread(fix_ogg_stream, NULL, TRUE, NULL);
}

GtkWidget *create_choose_file_frame()
{
  /* file entry and browse button hbox */
  GtkWidget *choose_file_hbox = gtk_hbox_new (FALSE, 0);
  gtk_container_set_border_width(GTK_CONTAINER(choose_file_hbox), 6);
  
  /* handle box for detaching */
  file_handle_box = gtk_handle_box_new();
  gtk_container_add(GTK_CONTAINER(file_handle_box), GTK_WIDGET(choose_file_hbox));
  g_signal_connect(file_handle_box, "child-detached",
      G_CALLBACK(handle_file_detached_event), NULL);

  /* filename entry */
  entry = gtk_entry_new();
  gtk_editable_set_editable(GTK_EDITABLE(entry), FALSE);
  gtk_box_pack_start(GTK_BOX(choose_file_hbox), entry , TRUE, TRUE, 4);

  // Display the input file name if  we already have gotten one
  // from the command line
  if(inputfilename_get()!=NULL)
    gtk_entry_set_text(GTK_ENTRY(entry), inputfilename_get());


  /* browse button */
  browse_button = (GtkWidget *)
    create_cool_button(GTK_STOCK_OPEN,_("_Browse"), FALSE);
  g_signal_connect(G_OBJECT (browse_button), "clicked",
      G_CALLBACK(browse_button_event), (gpointer *)BROWSE_SONG);
  gtk_box_pack_start(GTK_BOX(choose_file_hbox), browse_button, FALSE, FALSE, 4);
  gtk_widget_set_tooltip_text(browse_button,_("Select file"));
 
  /* bottom buttons hbox */
  //GtkWidget *bottom_buttons_hbox = gtk_hbox_new(FALSE,0);
  
  /* fix ogg stream button */
  fix_ogg_stream_button = (GtkWidget *)
    create_cool_button(GTK_STOCK_HARDDISK,_("_Fix ogg stream"), FALSE);
  g_signal_connect(G_OBJECT(fix_ogg_stream_button), "clicked",
      G_CALLBACK(fix_ogg_stream_button_event), NULL);
 /* gtk_box_pack_start (GTK_BOX(bottom_buttons_hbox), 
                      fix_ogg_stream_button, FALSE, FALSE, 7);
  gtk_box_pack_start (GTK_BOX(main_choose_file_vbox), 
                      bottom_buttons_hbox, FALSE, FALSE, 0);*/
  
  return file_handle_box;
}

//! Hide the connect button
void hide_connect_button()
{
  gtk_widget_hide(connect_button);
}

//! Show the connect button
void show_connect_button()
{
  if (! container_has_child(GTK_CONTAINER(player_buttons_hbox), connect_button))
  {
    gtk_box_pack_start(GTK_BOX(player_buttons_hbox), connect_button, FALSE, FALSE, 7);
  }
  gtk_widget_show_all(connect_button);
}

//! Hide the disconnect button
void hide_disconnect_button()
{
  gtk_widget_hide(disconnect_button);
}

//! Show the disconnec button
void show_disconnect_button()
{
  if (! container_has_child(GTK_CONTAINER(player_buttons_hbox), disconnect_button))
  {
    gtk_box_pack_start(GTK_BOX(player_buttons_hbox), disconnect_button, FALSE, FALSE, 7);
  }
  gtk_widget_show_all(disconnect_button);
}

