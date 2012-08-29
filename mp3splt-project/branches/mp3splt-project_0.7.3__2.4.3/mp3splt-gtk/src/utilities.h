/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2005-2012 Alexandru Munteanu - io_fx@yahoo.fr
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

#ifndef UTILITIES_H

#define UTILITIES_H

#define DOUBLE_PRECISION 0.00000001

#include "external_includes.h"
#include "ui_types.h"
#include "main_window.h"

gint directory_exists(const gchar *directory);
gint file_exists(const gchar *fname);
void print_processing_file(gchar *filename, ui_state *ui);
void remove_end_slash_n_r_from_filename(char *filename);
gchar *transform_to_utf8(gchar *text, gint free_or_not, gint *must_be_freed);
void build_path(GString *path, const gchar *dir, const gchar *filename);

gboolean double_equals(gdouble double_to_compare, gdouble compared_value);

#endif

