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
 * Filename: utilities.c
 *
 * this file is for creating default preferences file, getting
 * preferences filename, ... utilities
 *
 *********************************************************/

#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>
#include <libmp3splt/mp3splt.h>
#include <glib.h>
#include <glib/gi18n.h>
#include <glib/gstdio.h>

#include "player.h"
#include "preferences_tab.h"
#include "main_win.h"

//preferences
extern GtkWidget *directory_entry;
extern GtkWidget *player_combo_box;
extern gint selected_player;
extern GList *player_pref_list;
extern GtkWidget *radio_button;

extern GtkWidget *split_mode_radio_button;
extern GtkWidget *file_mode_radio_button;

extern GtkWidget *entry;


//EXTERNAL OPTIONS
//frame mode option
extern GtkWidget *frame_mode;
//auto-adjust option
extern GtkWidget *adjust_mode;
//gap parameter
extern GtkWidget *spinner_adjust_gap;
//offset parameter
extern GtkWidget *spinner_adjust_offset;
//threshold parameter
extern GtkWidget *spinner_adjust_threshold;

extern GtkWidget *create_dirs_from_output_files;

//output for the file output
extern GtkWidget *output_entry;
extern GtkWidget *output_label;
extern GtkWidget *radio_output;
extern GtkWidget *tags_radio;
extern GtkWidget *tags_version_radio;
extern GtkWidget *spinner_time;

extern splt_state *the_state;

//check if its a file
gint is_filee(const gchar *fname)
{
  if (fname == NULL)
  {
    return FALSE;
  }

  struct stat buffer;
  gint         status;

  status = g_stat(fname, &buffer);
  if (status == 0)
  {
    //if it is a file
    if (S_ISREG(buffer.st_mode) != 0)
    {
      return TRUE;
    }
    else
    {
      return FALSE;
    }
  }
  else
  {
    return FALSE;
  }
}

//filename returned must be freed after
//that functions also checks if we have a directory .mp3splt-gtk and
//if not it creates it. if we have a file .mp3splt-gtk, it makes a
//backup and then creates the directory
//result must be freed!!!!
gchar *get_preferences_filename()
{
  gchar mp3splt_dir[14] = ".mp3splt-gtk";
  
  //used to see if the directory exists
  struct stat buffer;
  gint         status;
  
  //home directory
  gchar *home_dir = g_strdup(g_get_home_dir());
 
#ifdef __WIN32__
  //manage c:\ because the gtk dir returns us "c:\"
  //and the normal directories without the "\"
  if (home_dir[strlen(home_dir)-1] == '\\')
    {
      home_dir[strlen(home_dir)-1] = '\0';
    }
#endif

  gchar *mp3splt_dir_with_path = NULL;
  gint malloc_number = strlen(home_dir) + strlen(mp3splt_dir)+2;
  mp3splt_dir_with_path = malloc(malloc_number *sizeof(gchar *));
  g_snprintf(mp3splt_dir_with_path, malloc_number,
             "%s%s%s", home_dir,G_DIR_SEPARATOR_S,
             mp3splt_dir);

  if (home_dir)
  {
    g_free(home_dir);
    home_dir = NULL;
  }
  
  gint fname_malloc_number = strlen(mp3splt_dir_with_path)+30;
  gchar *filename = malloc(fname_malloc_number*sizeof(gchar *));
  
  status = g_stat(mp3splt_dir_with_path, &buffer);
  //if it is not a directory
  if (S_ISDIR(buffer.st_mode) == 0)
    {
      //if its a file
      if (S_ISREG(buffer.st_mode) != 0)
        {
          gchar *backup_file;
          malloc_number = strlen(mp3splt_dir_with_path)+5;
          backup_file = malloc(malloc_number*sizeof(gchar *));
          snprintf(backup_file,malloc_number,
                   "%s%s", mp3splt_dir_with_path,".bak");
          //we rename the file
          g_rename(mp3splt_dir_with_path,
                   backup_file);
          g_free(backup_file);
        }
      //if it is not a directory and not a file, we suppose we can
      //create the directory
#ifdef __WIN32__      
      g_mkdir(mp3splt_dir_with_path, 0775);
#else
      g_mkdir(mp3splt_dir_with_path,
              S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
    }
  
  //the config filename+path
  g_snprintf(filename,fname_malloc_number,
             "%s%smp3splt-gtk_prefs",
             mp3splt_dir_with_path, G_DIR_SEPARATOR_S);
  
  if (mp3splt_dir_with_path)
  {
    g_free(mp3splt_dir_with_path);
    mp3splt_dir_with_path = NULL;
  }
  
  return filename;
}

//sets the language, loaded only at start
void set_language()
{
  GKeyFile *key_file = g_key_file_new();
  //filename
  gchar *filename = get_preferences_filename();

  //load config
  g_key_file_load_from_file(key_file,
                            filename,
                            G_KEY_FILE_KEEP_COMMENTS,
                            NULL);
  if (filename)
  {
    g_free(filename);
    filename = NULL;
  }
  
  gchar *lang = g_key_file_get_string(key_file, "general", "language", NULL);
 
 //NOTE: current function is only used for windows: code needs cleanup ?
#ifdef __WIN32__
  gchar lang_env[32] = { '\0' };
  g_snprintf(lang_env, 32, "LANG=%s", lang);
  putenv(lang_env);
#else
  setenv("LANGUAGE", lang,1);
#endif

  //freeing memory
  g_free(lang);
  g_key_file_free(key_file);
}

void load_preferences()
{
  GKeyFile *key_file = g_key_file_new();

  gchar *filename = get_preferences_filename();

  //load config
  g_key_file_load_from_file(key_file, filename, G_KEY_FILE_KEEP_COMMENTS, NULL);

  if (filename)
  {
    g_free(filename);
    filename = NULL;
  }

#ifdef __WIN32__
  //language
  gchar *file_string = g_key_file_get_string(key_file, "general", "language", NULL);
  GString *lang = g_string_new(file_string);

  //0 = german, 1 = french, 2 = english
  gint list_number = 2;
  if (g_string_equal(lang,g_string_new("de")))
  {
    list_number = 0;
  }
  else if (g_string_equal(lang, g_string_new("fr")))
  {
    list_number = 1;
  }

  GSList *radio_button_list =
    gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio_button));
  GtkWidget *our_button = (GtkWidget *)
    g_slist_nth_data(radio_button_list, list_number);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(our_button), TRUE);

  g_free(file_string);
  g_string_free(lang, TRUE);
  file_string = NULL;
  lang = NULL;
#endif

  //output_path
  gchar *save_path = g_key_file_get_string(key_file, "split", "save_path", NULL);
  if (save_path != NULL)
  {
    gtk_entry_set_text(GTK_ENTRY(directory_entry), save_path);
  }
  g_free(save_path);
  save_path = NULL;

  //selected player
  gint item = g_key_file_get_integer(key_file, "player", "default_player",NULL);
  //if its the first time we load preferences
  gint i;
  for(i = 0; i < 4; i++)
  {
    if (item == GPOINTER_TO_INT(g_list_nth_data(player_pref_list, i)))
    {
      selected_player = GPOINTER_TO_INT(g_list_nth_data(player_pref_list, i));
      goto jump_near;
    }
  }

jump_near:
  gtk_combo_box_set_active(GTK_COMBO_BOX(player_combo_box), i);

  //frame mode
  item = g_key_file_get_boolean(key_file, "split", "frame_mode", NULL);
  if (item)
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(frame_mode),TRUE);
  }
  else
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(frame_mode),FALSE);
  }

  //adjust mode
  item = g_key_file_get_boolean(key_file, "split", "adjust_mode", NULL);
  if (item)
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(adjust_mode),TRUE);
  }
  else
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(adjust_mode),FALSE);
  }

  //adjust threshold
  gfloat item2;
  item = g_key_file_get_integer(key_file, "split", "adjust_threshold", NULL);  
  item2 = item/100 + (item%100)/100.;
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_adjust_threshold), item2);
  //adjust offset
  item = g_key_file_get_integer(key_file, "split", "adjust_offset", NULL);
  item2 = item/100 + (item%100)/100.;
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_adjust_offset),
      item2);
  //adjust gap
  item = g_key_file_get_integer(key_file, "split", "adjust_gap", NULL);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_adjust_gap), item);

  //tags options
  gint tag_pref_file = g_key_file_get_integer(key_file, "split", "tags", NULL);
  GSList *tags_radio_button_list = 
    gtk_radio_button_get_group(GTK_RADIO_BUTTON(tags_radio));
  GtkWidget *the_selection = 
    (GtkWidget *)g_slist_nth_data(tags_radio_button_list, tag_pref_file);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(the_selection), TRUE);

  //tags version
  tag_pref_file = g_key_file_get_integer(key_file, "split", "tags_version", NULL);

  GSList *tags_version_radio_button_list = 
    gtk_radio_button_get_group(GTK_RADIO_BUTTON(tags_version_radio));
  the_selection = 
    (GtkWidget *)g_slist_nth_data(tags_version_radio_button_list, tag_pref_file);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(the_selection), TRUE);

  //default output format or not
  gint default_output_format = g_key_file_get_boolean(key_file, "output",
      "default_output_format", NULL);
  GSList *output_radio_button_list = 
    gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio_output));
  GtkWidget *our_selection = 
    (GtkWidget *)g_slist_nth_data(output_radio_button_list, default_output_format);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(our_selection), TRUE);
  if (default_output_format)
  {
    gtk_widget_set_sensitive(GTK_WIDGET(output_entry), FALSE);
    gtk_widget_set_sensitive(GTK_WIDGET(output_label), FALSE);
  }

  //output format
  gchar *output_format = g_key_file_get_string(key_file, "output", "output_format", NULL);
  gtk_entry_set_text(GTK_ENTRY(output_entry), output_format);
  g_free(output_format);
  output_format = NULL;

  //create directories if needed
  item = g_key_file_get_boolean(key_file, "output", "create_dirs_if_needed", NULL);
  if (item)
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(create_dirs_from_output_files), TRUE);
  }
  else
  {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(create_dirs_from_output_files), FALSE);
  }

  //type of split: split mode
  gint split_mode = g_key_file_get_integer(key_file, "split",
      "split_mode", NULL);
  GSList *split_mode_radio_button_list = 
    gtk_radio_button_get_group(GTK_RADIO_BUTTON(split_mode_radio_button));
  our_selection = 
    (GtkWidget *)g_slist_nth_data(split_mode_radio_button_list, split_mode);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(our_selection), TRUE);

  //type of split: time value
  gint time_value = g_key_file_get_integer(key_file, "split",
      "split_mode_time_value", NULL);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_time), time_value);

  //type of split: file mode
  gint file_mode = g_key_file_get_integer(key_file, "split",
      "file_mode", NULL);
  GSList *file_mode_radio_button_list = 
    gtk_radio_button_get_group(GTK_RADIO_BUTTON(file_mode_radio_button));
  our_selection = 
    (GtkWidget *)g_slist_nth_data(file_mode_radio_button_list, file_mode);
  gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(our_selection), TRUE);

  g_key_file_free(key_file);
  key_file = NULL;
}

//writes a default configuration file
//or writes good values on an ugly existing configuration file
void write_default_preferences_file()
{
  gchar *filename = get_preferences_filename();

  GKeyFile *my_key_file = g_key_file_new();
  g_key_file_load_from_file(my_key_file, filename, G_KEY_FILE_KEEP_COMMENTS, NULL);

#ifdef __WIN32__
  gchar *file_string = NULL;

  //default language
  if (!g_key_file_has_key(my_key_file, "general", "language",NULL))
  {
    g_key_file_set_string(my_key_file, "general", "language", "en");
    g_key_file_set_comment(my_key_file, "general", "language",
        "\n language of the gui: en = english, fr = french, de = german",
        NULL);
  }
  //if we have the key, but we have ugly values
  else
  {
    file_string = g_key_file_get_string(my_key_file, "general", "language", NULL);
    GString * lang_char = g_string_new(file_string);

    if((!g_string_equal(lang_char,g_string_new("en")))
        &&(!g_string_equal(lang_char, g_string_new("fr")))
        &&(!g_string_equal(lang_char, g_string_new("de"))))
    {
      g_key_file_set_string(my_key_file, "general", "language", "en");
      g_key_file_set_comment(my_key_file, "general", "language",
          "\n language of the gui: en = english, fr = french, de = german",
          NULL);
    }

    g_free(file_string);
    g_string_free(lang_char, TRUE);
    file_string = NULL;
    lang_char = NULL;
  }
#endif

  //frame mode
  if (!g_key_file_has_key(my_key_file, "split", "frame_mode",NULL))
  {
    g_key_file_set_boolean(my_key_file, "split", "frame_mode", TRUE);
  }

  //adjust mode
  if (!g_key_file_has_key(my_key_file, "split", "adjust_mode",NULL))
  {
    g_key_file_set_boolean(my_key_file, "split", "adjust_mode", FALSE);
  }

  gint item;
  gfloat item2;
  //adjust threshold
  if (!g_key_file_has_key(my_key_file, "split", "adjust_threshold",NULL))
  {
    g_key_file_set_integer(my_key_file, "split", "adjust_threshold",
        (int)(SPLT_DEFAULT_PARAM_THRESHOLD * 100));
  }
  //if we have the key but we have ugly values
  else
  {
    item = g_key_file_get_integer(my_key_file, "split", "adjust_threshold", NULL);
    //convert to float
    item2 = item/100 + (item%100)/100.;

    if ((item2 < -96) || (item2 > 0))
    {
      g_key_file_set_integer(my_key_file, "split", "adjust_threshold",
          (int)(SPLT_DEFAULT_PARAM_THRESHOLD * 100));
    }
  }

  //adjust offset
  if (!g_key_file_has_key(my_key_file, "split", "adjust_offset",NULL))
  {
    g_key_file_set_integer(my_key_file, "split", "adjust_offset",
        (int)(SPLT_DEFAULT_PARAM_OFFSET * 100));
  }
  //if we have the key but we have ugly values
  else
  {
    item = g_key_file_get_integer(my_key_file, "split", "adjust_offset", NULL);
    //convert to float
    item2 = item/100 + (item%100)/100.;

    //if ugly values
    if ((item2 < -2) || (item2 > 2))
    {
      g_key_file_set_integer(my_key_file, "split", "adjust_offset",
          (int)(SPLT_DEFAULT_PARAM_OFFSET * 100));
    }
  }

  //adjust gap
  if (!g_key_file_has_key(my_key_file, "split", "adjust_gap",NULL))
  {
    g_key_file_set_integer(my_key_file, "split", "adjust_gap", SPLT_DEFAULT_PARAM_GAP);
  }
  //if we have the key but we have ugly values
  else
  {
    item = g_key_file_get_integer(my_key_file, "split", "adjust_gap", NULL);

    //if ugly values
    if ((item < 0) || (item > 2000))
    {
      g_key_file_set_integer(my_key_file, "split", "adjust_gap",
          SPLT_DEFAULT_PARAM_GAP);
    }
  }

  //tags options
  if (!g_key_file_has_key(my_key_file, "split", "tags",NULL))
  {
    g_key_file_set_integer(my_key_file, "split", "tags", 1);
    g_key_file_set_comment(my_key_file, "split", "tags",
        "\n 0 - No tags, 1 - Default tags, 2 - Original tags", NULL);
  }

  //tags version
  if (!g_key_file_has_key(my_key_file, "split", "tags_version",NULL))
  {
    g_key_file_set_integer(my_key_file, "split", "tags_version", 0);
    g_key_file_set_comment(my_key_file, "split", "tags_version",
        "\n 0 - same tags version as the input file, 1 - ID3v1 tags,"
        " 2 - ID3v2 tags, 3 - ID3v1 & ID3v2 tags",
        NULL);
  }

  //default player
  if (!g_key_file_has_key(my_key_file, "player", "default_player",NULL))
  {
    g_key_file_set_integer(my_key_file, "player", "default_player",
        PLAYER_GSTREAMER);
    g_key_file_set_comment (my_key_file, "player", "default_player",
        "\n 1 = PLAYER_AUDACIOUS, 2 = PLAYER_SNACKAMP, 3 = PLAYER_GSTREAMER",
        NULL);
  }
  else
  {
    //check if we support selected player
    gint the_player = g_key_file_get_integer(my_key_file, "player",
        "default_player", NULL);
    if (the_player == PLAYER_AUDACIOUS)
    {
#ifdef NO_AUDACIOUS
      g_key_file_set_integer(my_key_file, "player", "default_player",
          PLAYER_SNACKAMP);
#endif
    }
    //if the value do not make sense
    else if ((the_player > PLAYER_GSTREAMER) || (the_player < 0))
    {
      g_key_file_set_integer(my_key_file, "player", "default_player",
          PLAYER_GSTREAMER);
    }
  }

  //output format
  if (!g_key_file_has_key(my_key_file, "output", "output_format",NULL))
  {
    g_key_file_set_string(my_key_file, "output", "output_format",
        SPLT_DEFAULT_OUTPUT);
    g_key_file_set_comment (my_key_file, "output", "output_format",
        "\n the output format, contains @a,"
        "@b, @p, @t and @n, see the program for"
        " more details",
        NULL);
  }

  //default output path boolean
  if (!g_key_file_has_key(my_key_file, "output", "default_output_format",NULL))
  {
    g_key_file_set_boolean(my_key_file, "output", "default_output_format", TRUE);
    g_key_file_set_comment(my_key_file, "output", "default_output_format",
        "\n can be true or false"
        " - if we use the default output or"
        " not for cddb, cue and freedb search",
        NULL);
  }

  //frame mode
  if (!g_key_file_has_key(my_key_file, "output", "create_dirs_if_needed", NULL))
  {
    g_key_file_set_boolean(my_key_file, "output", "create_dirs_if_needed", TRUE);
  }

  //split save path (output dir)
  if (!g_key_file_has_key(my_key_file, "split", "save_path",NULL))
  {
#ifdef __WIN32__
    const gchar *home_dir = g_get_home_dir();
    gint dir_malloc_number = strlen(home_dir)+ 10;
    gchar *default_dir = malloc(dir_malloc_number*sizeof(gchar *));
    g_snprintf(default_dir, dir_malloc_number, "%s\\Desktop",home_dir);

    //see if the directory exists
    struct stat buffer;
    gint status = g_stat(default_dir, &buffer);

    if (S_ISDIR(buffer.st_mode) == 0)
    {
      g_snprintf(default_dir,dir_malloc_number, "%s",home_dir);
    }
#else
    const gchar *default_dir = g_get_home_dir();
#endif

    g_key_file_set_string(my_key_file, "split", "save_path", default_dir);
    g_key_file_set_comment(my_key_file, "split", "save_path",
        "\n this is the path where you will find your split files ",
        NULL);

#ifdef __WIN32__
    g_free(default_dir);
#endif
  }

  //type of split: split mode
  if (!g_key_file_has_key(my_key_file, "split", "split_mode",NULL))
  {
    g_key_file_set_integer(my_key_file, "split", "split_mode", 3);
    g_key_file_set_comment(my_key_file, "split", "split_mode",
        "\n 0 - error mode, 1 - wrap mode, 2 - time mode, 3 - normal mode",
        NULL);
  }

  //type of split: time value
  if (!g_key_file_has_key(my_key_file, "split", "split_mode_time_value",NULL))
  {
    g_key_file_set_integer(my_key_file, "split", "split_mode_time_value", 60);
    g_key_file_set_comment(my_key_file, "split", "split_mode_time_value",
        "\n value in seconds to split every X seconds (for the time split)",
        NULL);
  }

  //type of split: file mode
  if (!g_key_file_has_key(my_key_file, "split", "file_mode",NULL))
  {
    g_key_file_set_integer(my_key_file, "split", "file_mode", 1);
    g_key_file_set_comment(my_key_file, "split", "file_mode",
        "\n 0 - multiple files, 1 - single file",
        NULL);
  }

  gchar *key_data = g_key_file_to_data(my_key_file, NULL, NULL);

  //write content to the preferences file
  FILE *preferences_file = (FILE *)fopen(filename,"w");
  g_fprintf(preferences_file,"%s", key_data);
  fclose(preferences_file);
  preferences_file = NULL;

  if (filename)
  {
    g_free(filename);
    filename = NULL;
  }

  g_free(key_data);
  key_data = NULL;
  g_key_file_free(my_key_file);
}

//checks if preferences file exists
//and if it does not, create it
void check_pref_file()
{
  //used to see if the file exists
  struct stat buffer;
  gint         status;
  
  gchar *pref_file = get_preferences_filename();
  
  status = stat(pref_file, &buffer);
  if (S_ISREG(buffer.st_mode) == 0)
  {
    if (S_ISDIR(buffer.st_mode) != 0)
    {
      //backup the directory
      gchar *backup_dir;
      gint malloc_number = strlen(pref_file)+5;
      backup_dir = malloc(malloc_number * sizeof(gchar *));
      snprintf(backup_dir,malloc_number,
          "%s%s",pref_file,".bak");
      //rename the directory
      g_rename(pref_file, backup_dir);
      g_free(backup_dir);
      backup_dir = NULL;
    }
  }
  
  if (pref_file)
  {
    g_free(pref_file);
    pref_file = NULL;
  }
  
  write_default_preferences_file();
}

//check if its a directory
//TODO: why guchar ?
gint check_if_dir(guchar *fname)
{
  struct stat buffer;
  gint         status;
  
  status = g_stat((gchar *)fname, &buffer);
  //if it is a directory
  if (S_ISDIR(buffer.st_mode) != 0)
    return TRUE;
  else
    return FALSE;
}

//check if its a file
//TODO: why guchar ?
gint check_if_file(guchar *fname)
{
  struct stat buffer;
  gint         status;

  status = g_stat((gchar *)fname, &buffer);
  //if it is a file
  if (S_ISREG(buffer.st_mode) != 0)
    return TRUE;
  else
    return FALSE;
}

GtkWidget *set_title_and_get_vbox(GtkWidget *widget, gchar *title)
{
  GtkWidget *label = gtk_label_new(NULL);
  gtk_label_set_markup(GTK_LABEL(label), title);

  GtkWidget *label_hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(label_hbox), label, FALSE, FALSE, 0);

  GtkWidget *vbox = gtk_vbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(vbox), label_hbox, FALSE, FALSE, 5);

  GtkWidget *hbox = gtk_hbox_new(FALSE, 0);
  gtk_box_pack_start(GTK_BOX(hbox), widget, TRUE, TRUE, 16);

  gtk_box_pack_start(GTK_BOX(vbox), hbox, FALSE, FALSE, 0);

  return vbox;
}

void print_processing_file(gchar *filename)
{
  gint fname_status_size = (strlen(filename) + 255);
  gchar *fname_status = g_malloc(sizeof(char) * fname_status_size);
  g_snprintf(fname_status, fname_status_size,
      _("Processing file '%s' ..."), filename);
  put_status_message(fname_status);
  if (fname_status)
  {
    free(fname_status);
    fname_status = NULL;
  }
}

gboolean container_has_child(GtkContainer *container, GtkWidget *my_child)
{
  GList *children = gtk_container_get_children(GTK_CONTAINER(container));
  int i = 0;
  GtkWidget *child = NULL;
  while ((child = g_list_nth_data(children, i)) != NULL)
  {
    if (child == my_child)
    {
      return TRUE;
    }
    i++;
  }

  return FALSE;
}

void remove_end_slash_n_r_from_filename(char *filename)
{
  if (filename == NULL)
  {
    return;
  }

  gint index = strlen(filename) - 1;
  while (index >= 0)
  {
    if (filename[index] == '\n' ||
        filename[index] == '\r')
    {
      filename[index] = '\0';
    }
    else if (filename[index] != '\0')
    {
      break;
    }

    index--;
  }
}

