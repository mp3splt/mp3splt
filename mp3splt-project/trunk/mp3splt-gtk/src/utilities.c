/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2010 Alexandru Munteanu
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

//!check if the string passed as an argument points to a file
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

/*!check if a string points to a directory

\todo why guchar?
*/
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

/*! check if a sting points to a file

\todo
 - Why guchar ?
 - And what is the difference to is_filee?
*/
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

//transform text to utf8
//free_or_not:  TRUE if we free text before and 
//otherwise FALSE
//the third result returns us if the result must be freed 
//or not
gchar *transform_to_utf8(gchar *text, gint free_or_not,
    gint *must_be_freed)
{
  gchar *temp;

  gsize bytes_read;
  gsize bytes_written;

  if(!(g_utf8_validate (text, -1,NULL)) &&
     (text != NULL))
    {
      temp = g_convert(text, -1, "UTF-8", "ISO-8859-1", &bytes_read, &bytes_written, NULL);
      if (free_or_not)
        g_free(text);
          
      *must_be_freed = TRUE;
          
      return temp;
    }
  
  *must_be_freed = FALSE;
  
  return text;
}


