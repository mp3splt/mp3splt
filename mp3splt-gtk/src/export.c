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
#include "ui_manager.h"
#include "widgets_helper.h"

extern ui_state *ui;

/*! Export the current split points into a cue file

\param filename The name of the file to write to.
\todo 
 - If we have previously imported the file... ...do we 
   want to handle all the tags we do not use --- but that
   have been there? And if yes: How do we handle them best?
 - Is there any file format that better suits us than a cue file?
 - if our input file does not have an extension... ...how to handle
   this? (we output the extension in the "FILE" line.)
 - Is there really no simple C/GTK+ function for quoting quotes?
*/
void export_file(const gchar* filename)
{
  FILE *outfile;
  GtkTreeModel *model;
  GtkTreeIter iter;
  
  if((outfile=fopen(filename,"w"))==0)
    {
      put_status_message((gchar *)strerror(errno));
      return;
    };

  if(fprintf(outfile,"REM CREATOR \"MP3SPLT_GTK\"\n")<0)
    {
      put_status_message((gchar *)strerror(errno));
      return;
    }

  if(fprintf(outfile,"REM SPLT_TITLE_IS_FILENAME\n")<0)
    {
      put_status_message((gchar *)strerror(errno));
      return;
    }

  // Determine which type our input file is of.
  gchar *extension=inputfilename_get();
  gchar *tmp;
  while((tmp=strchr(extension,'.')))
    {
      extension=++tmp;
    }

  if(fprintf(outfile,"FILE \"%s\" %s\n",inputfilename_get(),extension)<0)
    {
      put_status_message((gchar *)strerror(errno));
      return;
    };

  model = gtk_tree_view_get_model(tree_view);
  
  //if the table is not empty get iter number
  if(gtk_tree_model_get_iter_first(model, &iter))
    {
      // The track number
      gint count = 1;

      do 
	{
	  // All information we need for this track
	  gchar *description;
	  gint mins,secs,hundr;
	  gboolean keep;
	  
	  gtk_tree_model_get(GTK_TREE_MODEL(model), &iter,
			     COL_DESCRIPTION,&description,
                             COL_MINUTES, &mins,
                             COL_SECONDS, &secs,
                             COL_HUNDR_SECS, &hundr,
			     COL_CHECK, &keep,
			     -1);

	  // Sometimes libmp3splt introduces an additional split point
	  // way below the end of the file --- that breaks cue import
	  // later => skip all points with extremely high time values.
	  if(mins<357850)
	    {
	      // Output the track header
	      if(fprintf(outfile,"\tTRACK %02i AUDIO\n",count++)<0)
		{
		  put_status_message((gchar *)strerror(errno));
		  return;
		};
	      
	      
	      // Output the track description escaping any quotes
	      if(fprintf(outfile,"\t\tTITLE \"")<0)
		{
		  put_status_message((gchar *)strerror(errno));
		  return;
		}
	      
	      gchar *outputchar;
	      for(outputchar=description;*outputchar!='\0';outputchar++)
		{
		  if(*outputchar=='"')
		    {
		      if(fprintf(outfile,"\\\"")<0)
			{
			  put_status_message((gchar *)strerror(errno));
			  return;
			}
		    }
		  else
		    {
		      if(fprintf(outfile,"%c",*outputchar)<0)
			{
			  put_status_message((gchar *)strerror(errno));
			  return;
			}
		    }
		}    
	      if(fprintf(outfile,"\" \n")<0)
		{
		  put_status_message((gchar *)strerror(errno));
		  return;
		};
	      
	      if(!keep)
		{
		  if(fprintf(outfile,"\t\tREM NOKEEP\n")<0)
		    {
		      put_status_message((gchar *)strerror(errno));
		      return;
		    }
		}
	      
	      if(fprintf(outfile,"\t\tINDEX 01 %d:%02d:%02d\n",mins,secs,hundr)<0)
		{
		  put_status_message((gchar *)strerror(errno));
		  return;
		}
	    }
	} while(gtk_tree_model_iter_next(model, &iter));
    }
  
  fclose(outfile);
}

//! Choose the file to save the session to
void ChooseCueExportFile(GtkWidget *widget, gpointer data)
{
  // file chooser
  GtkWidget *file_chooser;

  //creates the dialog
  file_chooser = gtk_file_chooser_dialog_new(_("Select cue file name"),
      NULL,
      GTK_FILE_CHOOSER_ACTION_SAVE,
      GTK_STOCK_CANCEL,
      GTK_RESPONSE_CANCEL,
      GTK_STOCK_SAVE,
      GTK_RESPONSE_ACCEPT,
      NULL);

  wh_set_browser_directory_handler(ui, file_chooser);

  // tells the dialog to list only cue files
  GtkWidget *our_filter = (GtkWidget *)gtk_file_filter_new();
  gtk_file_filter_set_name (GTK_FILE_FILTER(our_filter), _("cue files (*.cue)"));
  gtk_file_filter_add_pattern(GTK_FILE_FILTER(our_filter), "*.cue");
  gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(file_chooser), GTK_FILE_FILTER(our_filter));
  gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(file_chooser),TRUE);


  if (gtk_dialog_run(GTK_DIALOG(file_chooser)) == GTK_RESPONSE_ACCEPT)
  {
    gchar *filename =
      gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(file_chooser));

    //Write the output file
    export_file(filename);
    g_free(filename);
  }
  
  //destroy the dialog
  gtk_widget_destroy(file_chooser);
}

