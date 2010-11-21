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

/*!********************************************************
 * \file 
 * The function that allows to export the current list of
 * splitpoints as a Cue sheet.
 *********************************************************/

#include <gtk/gtk.h>
#include <glib/gi18n.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include "export.h"
#include "main_win.h"
#include "player_tab.h"
#include "tree_tab.h"

/*! Export the current split points into a cue file

\param filename The name of the file to write to.
\todo 
 - finish writing the file
 - Is the gdk_threads_enter() really needed here?
 - If we have previously imported the file... ...do we 
   want to handle all the tags we do not use --- but that
   have been there. And if yes: How do we handle them best?
*/
void export_file(const gchar* filename)
{
  FILE *outfile;
  GtkTreeModel *model;
  GtkTreeIter iter;
  
  // The track number
  gint count = 1;

  if((outfile=fopen(filename,"w")))
    {
      put_status_message((gchar *)strerror(errno));
      return;
    };
  gdk_threads_enter();

  if(fprintf(outfile,"FILE ",inputfilename_get())<0)
    {
      put_status_message((gchar *)strerror(errno));
      return;
    };

  model = gtk_tree_view_get_model(tree_view);
  
  gchar *description;
  //we count the rows
  gint count = 0;
  //if the table is not empty
  //get iter number
  if(gtk_tree_model_get_iter_first(model, &iter))
    {
      {
	gtk_tree_model_get(GTK_TREE_MODEL(model), &iter,
			   COL_DESCRIPTION,&description,
			   -1);
      } until(!gtk_tree_model_iter_next(model, &iter));

  gdk_threads_leave();
  fclose(outfile);
}
