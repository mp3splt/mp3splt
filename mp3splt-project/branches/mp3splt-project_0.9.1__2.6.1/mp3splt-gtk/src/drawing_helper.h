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

#ifndef DRAWING_HELPER_H

#define DRAWING_HELPER_H

#include "external_includes.h"

void dh_set_color(cairo_t *cairo, GdkColor *color);
void dh_set_white_color(cairo_t *cairo_surface);
void dh_set_red_color(cairo_t *cairo_surface);

void dh_draw_rectangle(cairo_t *cairo, gboolean filled, gint x, gint y, 
    gint width, gint height);

void dh_draw_arc(cairo_t *cairo, gboolean filled, gint x, gint y,
    double radius, double angle1, double angle2);

void dh_draw_text(cairo_t *cairo, const gchar *text, gint x, gint y);
void dh_draw_text_with_size(cairo_t *cairo, const gchar *text, gint x, gint y, 
    gdouble font_size);

void dh_draw_line(cairo_t *cairo, gint x1, gint y1, gint x2, gint y2,
    gboolean line_is_dashed, gboolean stroke);
void dh_draw_line_with_width(cairo_t *cairo, gint x1, gint y1, gint x2, gint y2,
    gboolean line_is_dashed, gboolean stroke, double line_width);

void draw_point(cairo_t *cairo, gint x, gint y);

#endif

