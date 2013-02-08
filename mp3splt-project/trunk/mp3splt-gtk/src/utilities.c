/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright: (C) 2005-2013 Alexandru Munteanu
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
 *  miscellaneous utilities
 *
 * Miscellaneous utilities like the check if a string may 
 * contain a valid file- or directory name.
 ********************************************************/

#include "utilities.h"

/*!check if specified directory exists
*/
gint directory_exists(const gchar *directory)
{
  if (directory == NULL)
  {
    return FALSE;
  }

  struct stat buffer;
  gint status = g_stat(directory, &buffer);

  if (status == 0 && S_ISDIR(buffer.st_mode) != 0)
    return TRUE;

  return FALSE;
}

/*! check if specified file exists
*/
gint file_exists(const gchar *fname)
{
  if (fname == NULL)
  {
    return FALSE;
  }

  struct stat buffer;
  gint status = g_stat(fname, &buffer);

  if (status == 0 && S_ISREG(buffer.st_mode) != 0)
    return TRUE;

  return FALSE;
}

/*! Issues the message "Processing file <filename>" into the message bar

\param filename The filename that has to be printed.
 */
void print_processing_file(gchar *filename, ui_state *ui)
{
  gint fname_status_size = (strlen(filename) + 255);
  gchar *fname_status = g_malloc(sizeof(char) * fname_status_size);
  g_snprintf(fname_status, fname_status_size,
      _("Processing file '%s' ..."), filename);
  put_status_message(fname_status, ui);
  if (fname_status)
  {
    free(fname_status);
    fname_status = NULL;
  }
}

/*! Removes trailing \\r or \\n characters from a filename
 */
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

/*! transform text to utf8

\param text The text thet has to be converted
\param free_or_not: TRUE if this function has to g_free() the text if
during conversion it has to copy the text to a new (e.G. larger) buffer.
\param must_be_freed reads true, if this function has allocated a new
chunk of memory to have somewhere to put the output string in - which
means that the memory the output string is in has to be freed after usage. 
\return 
*/
gchar *transform_to_utf8(gchar *text, gint free_or_not, gint *must_be_freed)
{
  gchar *temp;

  gsize bytes_read;
  gsize bytes_written;

  if (!(g_utf8_validate (text, -1,NULL)) && (text != NULL))
  {
    temp = g_convert(text, -1, "UTF-8", "ISO-8859-1", &bytes_read, &bytes_written, NULL);
    if (free_or_not)
    {
      g_free(text);
    }

    *must_be_freed = TRUE;

    return temp;
  }

  *must_be_freed = FALSE;

  return text;
}

void build_path(GString *path, const gchar *dir, const gchar *filename)
{
#ifdef __WIN32__
  g_string_assign(path, ".");
  g_string_append(path, G_DIR_SEPARATOR_S);
  g_string_append(path, filename);
#else
  if (strlen(dir) == 0)
  {
    g_string_assign(path, filename);
  }
  else 
  {
    g_string_assign(path, dir);
    g_string_append(path, G_DIR_SEPARATOR_S);
    g_string_append(path, filename);
  }
#endif
}

gboolean double_equals(gdouble double_to_compare, gdouble compared_value)
{
  return fabs(double_to_compare - compared_value) < DOUBLE_PRECISION;
}


