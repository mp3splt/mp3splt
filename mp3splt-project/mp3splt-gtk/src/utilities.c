/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2006 Munteanu Alexandru
 * Contact: io_alex_2002@yahoo.fr
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
#include <glib/gstdio.h>

#include "player.h"
#include "preferences_tab.h"

//preferences
extern GtkWidget *save_button;
extern GtkWidget *revert_to_save_button;
extern GtkWidget *check_save_dialog;
extern gint save_dialog;
extern GtkWidget *directory_entry;
extern GtkWidget *player_combo_box;
extern gint selected_player;
extern GList *player_pref_list;
extern GtkWidget *radio_button;

extern GtkWidget *entry;


//EXTERNAL OPTIONS
//frame mode option
extern GtkWidget *frame_mode;
//auto-adjust option
extern GtkWidget *adjust_mode;
//seekable option
extern GtkWidget *seekable_mode;
//gap parameter
extern GtkWidget *spinner_adjust_gap;
//offset parameter
extern GtkWidget *spinner_adjust_offset;
//threshold parameter
extern GtkWidget *spinner_adjust_threshold;

//output for the file output
extern GtkWidget *output_entry;
extern GtkWidget *radio_output;
extern GtkWidget *tags_radio;

//check if its a file
gint is_filee(gchar *fname)
{
  struct stat buffer;
  gint         status;
  
  status = stat((gchar *)fname, &buffer);
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
  gchar *home_dir = (gchar *)g_get_home_dir();
 
#ifdef __WIN32__
  //manage c:\ because the gtk dir returns us "c:\"
  //and the normal directories without the "\"
  if (home_dir[strlen(home_dir)-1] == '\\')
    {
      home_dir[strlen(home_dir)-1] = '\0';
    }
#endif

  //mp3splt dir name+path
  gchar *mp3splt_dir_with_path;
  gint malloc_number = strlen(home_dir) + 
    strlen(mp3splt_dir)+2;
  //allocate memory for the mp3splt dir with path
  mp3splt_dir_with_path = 
    (gchar *)malloc(malloc_number *sizeof(gchar *));
  g_snprintf(mp3splt_dir_with_path, malloc_number,
             "%s%s%s", home_dir,G_DIR_SEPARATOR_S,
             mp3splt_dir);
  
  //filename+path
  gchar *filename;
  gint fname_malloc_number = strlen(mp3splt_dir_with_path)+30;
  filename = 
    (gchar *)malloc(fname_malloc_number*sizeof(gchar *));
  
  status = g_stat(mp3splt_dir_with_path, &buffer);
  //if it is not a directory
  if (S_ISDIR(buffer.st_mode) == 0)
    {
      //if its a file
      if (S_ISREG(buffer.st_mode) != 0)
        {
          gchar *backup_file;
          malloc_number = strlen(mp3splt_dir_with_path)+5;
          backup_file = (gchar *)
            malloc(malloc_number*sizeof(gchar *));
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
  
  //free some memory
  g_free(mp3splt_dir_with_path);
  
  return filename;
}

//checks if the preferences are different from those in the config file
gboolean check_if_different_from_config_file()
{
  //the returned result
  gint different = FALSE;
  GKeyFile *key_file = g_key_file_new();
  
  //filename
  gchar *filename;
  filename = get_preferences_filename();

  g_key_file_load_from_file(key_file,
                            filename,
                            G_KEY_FILE_KEEP_COMMENTS,
                            NULL);
  g_free(filename);
  
  //check the save_dialog from general
  if (g_key_file_get_boolean(key_file,
                             "general",
                             "save_dialog",
                             NULL) !=
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(check_save_dialog)))
    different = TRUE;
  
  //check the save_path from split
  GString *path1;
  GString *path2;
  gchar *file_string = 
    g_key_file_get_string(key_file,
                          "split",
                          "save_path", NULL);
  path1 = g_string_new(file_string);
  path2 = 
    g_string_new(gtk_entry_get_text(GTK_ENTRY(directory_entry)));
  if (!g_string_equal(path1,path2))
    different = TRUE;
  
  //free elements used
  g_string_free(path1, TRUE);
  g_string_free(path2, TRUE);
  g_free(file_string);
  
  //check the selected_player from player
  if (selected_player != 
      g_key_file_get_integer(key_file,
                             "player",
                             "default_player",
                             NULL))
    different = TRUE;

  GString *lang = (GString *)get_checked_language();
  file_string = g_key_file_get_string(key_file,
                                      "general",
                                      "language",
                                      NULL);
  GString *langg =
    g_string_new(file_string);
  //free memory
  g_free(file_string);
  
  //check the language from general
  if (!g_string_equal(lang,langg))
    different = TRUE;
  
  //freeing memory
  g_string_free(lang, TRUE);
  g_string_free(langg, TRUE);
  
  gint item;
  //check frame mode
  item = g_key_file_get_boolean(key_file,
                                "split",
                                "frame_mode",
                                NULL);
  if (item !=
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(frame_mode)))
    different = TRUE;
  
  //check seekable mode
  item = g_key_file_get_boolean(key_file,
                                "split",
                                "seekable_mode",
                                NULL);
  if (item !=
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(seekable_mode)))
    different = TRUE;
  
  //check adjust event
  item = g_key_file_get_boolean(key_file,
                                "split",
                                "adjust_mode",
                                NULL);

  if (item !=
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(adjust_mode)))
    different = TRUE;
  
  //checking adjust parameters...
  //checking adjust threshold
  //this is float
  item = g_key_file_get_integer(key_file,
                                "split",
                                "adjust_threshold",
                                NULL);
  if ((item <
       (gint)(gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinner_adjust_threshold)) * 100) - 1)
      || (item >
          (gint)(gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinner_adjust_threshold)) * 100) + 1))
    different = TRUE;

  //checking adjust offset
  //this is float
  item = g_key_file_get_integer(key_file,
                                "split",
                                "adjust_offset",
                                NULL);
  if ((item <
       (gint)(gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinner_adjust_offset)) * 100) - 1)
      || item > (gint)(gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinner_adjust_offset)) * 100) + 1)
    different = TRUE;
  
  //checking adjust gap
  item = g_key_file_get_integer(key_file,
                                "split",
                                "adjust_gap",
                                NULL);
  if (item != 
      gtk_spin_button_get_value(GTK_SPIN_BUTTON(spinner_adjust_gap)))
    different = TRUE;
  
  //check if the output format is different
  gchar *output_format = 
    g_key_file_get_string(key_file,
                          "output",
                          "output_format", NULL);
  
  if (strcmp(gtk_entry_get_text(GTK_ENTRY(output_entry)),
             output_format) != 0)
    {
      different = TRUE;
    }
  g_free(output_format);

  //check the default output format or not
  gboolean checked_default_output = 
    g_key_file_get_boolean(key_file,
                           "output",
                           "default_output_format", NULL);
  if (get_checked_output_radio_box() !=
      checked_default_output)
    {
      different = TRUE;
    }
  
  //we check the tag selection
  gint tag_pref_file = 
    g_key_file_get_integer(key_file,
                           "split",
                           "tags", NULL);
  gint tag_selection = get_checked_tags_radio_box();
  if (tag_selection != tag_pref_file)
    {
      different = TRUE;
    }

  //free memory
  g_key_file_free(key_file);
  
  return different;
}

//sets the language, loaded only at start
void set_language()
{
  GKeyFile *key_file = g_key_file_new();
  //filename
  gchar *filename;
  filename = get_preferences_filename();

  //load config
  g_key_file_load_from_file(key_file,
                            filename,
                            G_KEY_FILE_KEEP_COMMENTS,
                            NULL);
  //free the filename
  g_free(filename);
  
  GString *lang;
  gchar *file_string =
    g_key_file_get_string(key_file,
                          "general",
                          "language",
                          NULL);
  lang = g_string_new(file_string);
  
  //0 = french, 1 = english
  //gint list_number = 1;
  if(g_string_equal(lang,g_string_new("en")))
    {
#ifdef __WIN32__
      putenv("LANG=en");
#else
      setenv("LANGUAGE", "en",1);      
#endif
    }
  else
    if(g_string_equal(lang,g_string_new("fr")))
      {
#ifdef __WIN32__
        putenv("LANG=fr");
#else
        setenv("LANGUAGE", "fr",1);
#endif
      }
  
  //freeing memory
  g_string_free(lang, TRUE);
  g_free(file_string);
  g_key_file_free(key_file);
}

//load preferences
void load_preferences()
{
  GKeyFile *key_file = g_key_file_new();
  
  //filename
  gchar *filename;
  filename = get_preferences_filename();

  //load config
  g_key_file_load_from_file(key_file,
                            filename,
                            G_KEY_FILE_KEEP_COMMENTS,
                            NULL);
  //free the filename
  g_free(filename);

  //only for the buttons, the setenv is in another function
  /* the language radio buttons */
  GString *lang;
  gchar *file_string =
    g_key_file_get_string(key_file,
                          "general",
                          "language",
                          NULL);
  lang = 
    g_string_new(file_string);
  //0 = french, 1 = english
  gint list_number = 1;
  if(g_string_equal(lang,g_string_new("en")))
    list_number = 1;
  else
    if(g_string_equal(lang,g_string_new("fr")))
      list_number = 0;
  //get the radio buttons
  GSList *radio_button_list;
  radio_button_list = gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio_button));
  GtkWidget *our_button;
  our_button = 
    (GtkWidget *)g_slist_nth_data(radio_button_list, list_number);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(our_button), TRUE);
  
  //free the string
  g_free(file_string);
  g_string_free(lang, TRUE);
  
  //check mp3split directory path
  gchar *save_path;
  save_path = g_key_file_get_string(key_file,
                                    "split",
                                    "save_path",
                                    NULL);
    
  if (save_path != NULL)
    gtk_entry_set_text(GTK_ENTRY(directory_entry), 
                       (gchar *)save_path);
  //freeing memory
  g_free(save_path);
  
  //if save dialog true
  if(g_key_file_get_boolean(key_file,
                            "general",
                            "save_dialog",
                            NULL))
    {
      save_dialog = TRUE;
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_save_dialog),
                                   TRUE);
    }
  else
    {
      save_dialog = FALSE;
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(check_save_dialog),
                                   FALSE);
    }

  gint item;
  //check selected player
  item = g_key_file_get_integer(key_file,
                                "player",
                                "default_player",
                                NULL);

  //if its the first time we load preferences
  gint i;
  for(i = 0; i < 4; i++)
    {
      if (item == (gint)
          g_list_nth_data(player_pref_list, i))
        {
          selected_player = 
            (gint)g_list_nth_data(player_pref_list, i);
          goto jump_near;
        }
    }
      
 jump_near:
  gtk_combo_box_set_active(GTK_COMBO_BOX(player_combo_box), i);
  
  //check frame mode
  item = g_key_file_get_boolean(key_file,
                                "split",
                                "frame_mode",
                                NULL);
  if (item)
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(frame_mode),TRUE);
    }
  else
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(frame_mode),FALSE);
    }
  
  //check seekable mode
  item = g_key_file_get_boolean(key_file,
                                "split",
                                "seekable_mode",
                                NULL);
  if (item)
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(seekable_mode),TRUE);
    }
  else
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(seekable_mode),FALSE);
    }
  
  //check adjust mode
  item = g_key_file_get_boolean(key_file,
                                "split",
                                "adjust_mode",
                                NULL);
  if (item)
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(adjust_mode),TRUE);
    }
  else
    {
      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(adjust_mode),FALSE);
    }
  
  //checking adjust parameters...
  //checking adjust threshold
  //this is float
  gfloat item2;
  item = g_key_file_get_integer(key_file,
                                "split",
                                "adjust_threshold",
                                NULL);  
  //we convert to float
  item2 = item/100 + (item%100)/100.;
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_adjust_threshold),
                            item2);
  //checking adjust offset
  //this is float
  item = g_key_file_get_integer(key_file,
                                "split",
                                "adjust_offset",
                                NULL);
  //we convert to float
  item2 = item/100 + (item%100)/100.;
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_adjust_offset),
                            item2);
  //checking adjust gap
  item = g_key_file_get_integer(key_file,
                                "split",
                                "adjust_gap",
                                NULL);
  gtk_spin_button_set_value(GTK_SPIN_BUTTON(spinner_adjust_gap),
                            item);
  
  //the tags options
  gint tag_pref_file = 
    g_key_file_get_integer(key_file,
                           "split",
                           "tags", NULL);
  //get the radio buttons
  GSList *tags_radio_button_list;
  tags_radio_button_list = 
    gtk_radio_button_get_group(GTK_RADIO_BUTTON(tags_radio));
  GtkWidget *the_selection;
  the_selection = 
    (GtkWidget *)g_slist_nth_data(tags_radio_button_list,
                                  tag_pref_file);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(the_selection),
                                TRUE);
  
  //the output format entry
  gchar *output_format =
    g_key_file_get_string(key_file,
                          "output",
                          "output_format", NULL);
  gtk_entry_set_text(GTK_ENTRY(output_entry), 
                     output_format);
  
  //free memory
  g_free(output_format);
  
  //the default output format or not
  gint default_output_format =
    g_key_file_get_boolean(key_file,
                           "output",
                           "default_output_format", NULL);
  //get the radio buttons
  GSList *output_radio_button_list;
  output_radio_button_list = 
    gtk_radio_button_get_group(GTK_RADIO_BUTTON(radio_output));
  GtkWidget *our_selection;
  our_selection = 
    (GtkWidget *)g_slist_nth_data(output_radio_button_list,
                                  default_output_format);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON(our_selection),
                                TRUE);
  //if we have default output format we set unsensitive 
  //the output format entry
  if (default_output_format)
    {
      gtk_widget_set_sensitive(GTK_WIDGET(output_entry), FALSE);
    }
  
  g_key_file_free(key_file);
  
  //disable save and revert to saved button
  if (GTK_WIDGET_SENSITIVE(save_button))
    gtk_widget_set_sensitive(GTK_WIDGET(save_button), FALSE);
  if (GTK_WIDGET_SENSITIVE(revert_to_save_button))
    gtk_widget_set_sensitive(GTK_WIDGET(revert_to_save_button), FALSE);
}

//writes a default configuration file
//or writes good value on an ugly existing configuration file
void write_default_preferences_file()
{
  gchar *filename;
  gchar *file_string;
  
  filename =
    get_preferences_filename();
  
  //our key file
  GKeyFile *my_key_file = g_key_file_new();
  g_key_file_load_from_file(my_key_file,
                            filename,
                            G_KEY_FILE_KEEP_COMMENTS,
                            NULL);

  //write the default language
  if (!g_key_file_has_key(my_key_file,
                          "general",
                          "language",NULL))
    {
      g_key_file_set_string(my_key_file,
                            "general",
                            "language",
                            "en");
      
      g_key_file_set_comment (my_key_file,
                              "general",
                              "language",
                              "\n language of the gui : en = english, fr"
                              " = french",
                              NULL);
    }
  else
    //if we have the key, but we have ugly values
    {
      GString *lang_char;
      file_string =
        g_key_file_get_string(my_key_file,
                              "general",
                              "language",
                              NULL);
      lang_char = g_string_new(file_string);

      if((!g_string_equal(lang_char,g_string_new("en")))
         &&(!g_string_equal(lang_char, g_string_new("fr"))))
        {
          g_key_file_set_string(my_key_file,
                                "general",
                                "language",
                                "en");
          
          g_key_file_set_comment (my_key_file,
                                  "general",
                                  "language",
                                  "\n language of the gui : en = english, fr"
                                  " = french",
                                  NULL);
        }
      
      //freeing memory
      g_free(file_string);
      g_string_free(lang_char, TRUE);
    }
  
  //write the save dialog
  if (!g_key_file_has_key(my_key_file,
                          "general",
                          "save_dialog",NULL))
    {
      g_key_file_set_boolean(my_key_file,
                             "general",
                             "save_dialog",
                             FALSE);
      
      g_key_file_set_comment (my_key_file,
                              "general",
                              "save_dialog",
                              "\n enable this to see a popup dialog each"
                              "time you change from the preferences tab"
                              "without saving the changes",
                              NULL);
    }
  else
    //if we have the key, but we have ugly values
    {
      GString *save_dialog_char;
      file_string = 
        g_key_file_get_string(my_key_file,
                              "general",
                              "save_dialog",
                              NULL);
      save_dialog_char = g_string_new(file_string);

      //if different from true and false
      if((!g_string_equal(save_dialog_char,g_string_new("false") ))
         &&(!g_string_equal(save_dialog_char, g_string_new("true")))
         &&(!g_string_equal(save_dialog_char, g_string_new("FALSE")))
         &&(!g_string_equal(save_dialog_char, g_string_new("TRUE"))))
        {
          //set default value
          g_key_file_set_boolean(my_key_file,
                                 "general",
                                 "save_dialog",
                                 FALSE);
          
          g_key_file_set_comment (my_key_file,
                                  "general",
                                  "save_dialog",
                                  "\n enable this to see a popup dialog each"
                                  "time you change from the preferences tab"
                                  "without saving the changes",
                                  NULL);
        }
      
      //freeing memory
      g_free(file_string);
      g_string_free(save_dialog_char, TRUE);
    }

  //write split frame mode
  if (!g_key_file_has_key(my_key_file,
                          "split",
                          "frame_mode",NULL))
    g_key_file_set_boolean(my_key_file,
                           "split",
                           "frame_mode",
                           TRUE);
  
  //write split seekable mode
  if (!g_key_file_has_key(my_key_file,
                          "split",
                          "seekable_mode",NULL))
    g_key_file_set_boolean(my_key_file,
                           "split",
                           "seekable_mode",
                           FALSE);
  
  //write split adjust mode
  if (!g_key_file_has_key(my_key_file,
                          "split",
                          "adjust_mode",NULL))
    g_key_file_set_boolean(my_key_file,
                           "split",
                           "adjust_mode",
                           FALSE);
  
  gint item;
  gfloat item2;
  //write adjust threshold
  if (!g_key_file_has_key(my_key_file,
                          "split",
                          "adjust_threshold",NULL))
    {
      g_key_file_set_integer(my_key_file,
                             "split",
                             "adjust_threshold",
                             (int)(SPLT_DEFAULT_PARAM_THRESHOLD * 100));
    }
  else
    //if we have the key but we have ugly values
    {
      item = 
        g_key_file_get_integer(my_key_file,
                               "split",
                               "adjust_threshold",
                               NULL);
      //we convert to float
      item2 = item/100 + (item%100)/100.;
      
      //if ugly values
      if ((item2 < -96) || (item2 > 0))
        g_key_file_set_integer(my_key_file,
                               "split",
                               "adjust_threshold",
                               (int)(SPLT_DEFAULT_PARAM_THRESHOLD * 100));
    }

  //write adjust offset
  if (!g_key_file_has_key(my_key_file,
                          "split",
                          "adjust_offset",NULL))
    {
      g_key_file_set_integer(my_key_file,
                             "split",
                             "adjust_offset",
                             (int)(SPLT_DEFAULT_PARAM_OFFSET * 100));
    }
  else
    //if we have the key but we have ugly values
    {
      item = 
        g_key_file_get_integer(my_key_file,
                               "split",
                               "adjust_offset",
                               NULL);
      //we convert to float
      item2 = item/100 + (item%100)/100.;
      
      //if ugly values
      if ((item2 < -2) || (item2 > 2))
        g_key_file_set_integer(my_key_file,
                               "split",
                               "adjust_offset",
                               (int)(SPLT_DEFAULT_PARAM_OFFSET * 100));
    }
  
  //write adjust gap
  if (!g_key_file_has_key(my_key_file,
                          "split",
                          "adjust_gap",NULL))
    {
      g_key_file_set_integer(my_key_file,
                             "split",
                             "adjust_gap",
                             SPLT_DEFAULT_PARAM_GAP);
    }
  else
    //if we have the key but we have ugly values
    {
      item = 
        g_key_file_get_integer(my_key_file,
                               "split",
                               "adjust_gap",
                               NULL);
      
      //if ugly values
      if ((item < 0) || (item > 2000))
        {
          g_key_file_set_integer(my_key_file,
                                 "split",
                                 "adjust_gap",
                                 SPLT_DEFAULT_PARAM_GAP);
        }
    }
  
  //tags options
  // 0 = No tags, 1 = Original file tags
  //if we don't have the key default_output_format
  if (!g_key_file_has_key(my_key_file,
                          "split",
                          "tags",NULL))
    {
      //if we don't have the key we write default values
      g_key_file_set_integer(my_key_file,
                             "split", "tags", 1);
      
      g_key_file_set_comment (my_key_file,
                              "split",
                              "tags",
                              "\n 0 - No tags, 1 - Default tags, 2 - Original tags",
                              NULL);
    }
  
  //if there is no player selection
  if (!g_key_file_has_key(my_key_file,
                          "player",
                          "default_player",NULL))
    {
      //write snackamp as default
      g_key_file_set_integer(my_key_file,
                             "player",
                             "default_player",
                             PLAYER_SNACKAMP);
      g_key_file_set_comment (my_key_file,
                              "player",
                              "default_player",
                              "\n 1"
                              " = PLAYER_BMP, 2 = PLAYER_SNACKAMP (on windows"
                              " only 0 or 2)",
                              NULL);
    }
  else
    //if we have the key, 
    //check if we support selected player
    {
      gint the_player;
      the_player = 
        g_key_file_get_integer(my_key_file,
                               "player",
                               "default_player",
                               NULL);
        
      //if we have bmp in the config file
      if (the_player == PLAYER_BMP)
        {
          //if we dont support bmp, write another player
#ifdef NO_BMP
          g_key_file_set_integer(my_key_file,
                                 "player",
                                 "default_player",
                                 PLAYER_SNACKAMP);
#endif
        }
      else
        //if the value does not make sense
        if ((the_player > PLAYER_SNACKAMP)
            || (the_player < 0))
          {
            g_key_file_set_integer(my_key_file,
                                   "player",
                                   "default_player",
                                   PLAYER_SNACKAMP);
          }
    }
  
  //cddb, freedb and cue output
  if (!g_key_file_has_key(my_key_file,
                          "output",
                          "output_format",NULL))
    {
      //if we don't have the key we write default values
      g_key_file_set_string(my_key_file,
                            "output",
                            "output_format",
                            SPLT_DEFAULT_OUTPUT);
      
      g_key_file_set_comment (my_key_file,
                              "output",
                              "output_format",
                              "\n the output format, contains @a,"
                              "@b, @p, @t and @n, see the program for"
                              " more details",
                              NULL);
    }

  //if we don't have the key default_output_format
  if (!g_key_file_has_key(my_key_file,
                          "output",
                          "default_output_format",NULL))
    {
      //if we don't have the key we write default values
      g_key_file_set_boolean(my_key_file,
                             "output",
                             "default_output_format",
                             TRUE);
      
      g_key_file_set_comment (my_key_file,
                              "output",
                              "default_output_format",
                              "\n can be true or false"
                              " - if we use the default output or"
                              " not for cddb, cue and freedb search",
                              NULL);
    }

  //if we don't have the key
  //write split save path or 
  //if we have the directory key and ""
  if (!g_key_file_has_key(my_key_file,
                          "split",
                          "save_path",NULL))
    {
#ifdef __WIN32__
      gint dir_malloc_number;
      //home directory
      gchar *home_dir = (gchar *)g_get_home_dir();
      //gchar *default_dir = (gchar *)g_get_home_dir();
      dir_malloc_number = strlen(home_dir)+ 10;
      gchar *default_dir = 
        (gchar *) malloc(dir_malloc_number*sizeof(gchar *));
      g_snprintf(default_dir,dir_malloc_number,
                 "%s\\Desktop",home_dir);
      
      //used to see if the directory exists
      struct stat buffer;
      gint         status;
      
      status = g_stat(default_dir, &buffer);
      //if it is not a directory put home directory
      if (S_ISDIR(buffer.st_mode) == 0)
        {
          g_snprintf(default_dir,dir_malloc_number,
                     "%s",home_dir);
        }
      
#else
      gchar *default_dir = 
        (gchar *)g_get_home_dir();
#endif
      
      g_key_file_set_string(my_key_file,
                            "split",
                            "save_path",
                            default_dir);
      
      g_key_file_set_comment (my_key_file,
                              "split",
                              "save_path",
                              "\n this is the path where you will find "
                              "your splitted files ",
                              NULL);

#ifdef __WIN32__
      g_free(default_dir);
#endif
    }
   
  //our data
  gchar *key_data;
  key_data = g_key_file_to_data(my_key_file, NULL, 
                                NULL);
  
  //we write content to the preferences file
  FILE *preferences_file;
  preferences_file = (FILE *)fopen(filename,"w");
  g_fprintf(preferences_file,"%s", key_data);
  fclose(preferences_file);
  
  //freeing memory
  g_free(filename);
  g_free(key_data);
  g_key_file_free(my_key_file);
}

//checks if preferences file exists
//and if it does not, create it
void check_pref_file()
{
  //used to see if the file exists
  struct stat buffer;
  gint         status;
  
  //our preferences filename
  gchar *filename;
  filename =
    get_preferences_filename();
  
  status = stat(filename, &buffer);
  //if its not a file
  if (S_ISREG(buffer.st_mode) == 0)
    {
      //if it is a directory
      if (S_ISDIR(buffer.st_mode) != 0)
        {
          //backup the directory
          gchar *backup_dir;
          gint malloc_number = strlen(filename)+5;
          backup_dir = (gchar *)
            malloc(malloc_number * sizeof(gchar *));
          snprintf(backup_dir,malloc_number,
                   "%s%s",filename,".bak");
          //we rename the directory
          g_rename(filename, backup_dir);
          
          //free memory
          g_free(backup_dir);
        }
    }
  //free memory
  g_free(filename);
  
  write_default_preferences_file();
}

//check if its a directory
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
