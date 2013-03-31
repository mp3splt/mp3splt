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

#include "drawing_helper.h"

void dh_set_color(cairo_t *cairo, GdkColor *color)
{
  gdk_cairo_set_source_color(cairo, color);
}

void dh_set_white_color(cairo_t *cairo_surface)
{
  GdkColor color;
  color.red = 255 * 255;color.green = 255 * 255;color.blue = 255 * 255;
  dh_set_color(cairo_surface, &color);
}

void dh_set_red_color(cairo_t *cairo_surface)
{
  GdkColor color;
  color.red = 255 * 255;color.green = 0 * 255;color.blue = 0 * 255;
  dh_set_color(cairo_surface, &color);
}

void dh_draw_rectangle(cairo_t *cairo, gboolean filled, gint x, gint y, 
    gint width, gint height)
{
  cairo_rectangle(cairo, x, y, width, height);

  if (filled)
  {
    cairo_fill(cairo);
  }

  cairo_stroke(cairo);
}

void dh_draw_arc(cairo_t *cairo, gboolean filled, gint x, gint y,
    double radius, double angle1, double angle2)
{
  cairo_arc(cairo, x, y, radius, angle1, angle2);

  if (filled)
  {
    cairo_fill(cairo);
  }

  cairo_stroke(cairo);
}

void dh_draw_text(cairo_t *cairo, const gchar *text, gint x, gint y)
{
  dh_draw_text_with_size(cairo, text, x, y, 11.0);
}

void dh_draw_text_with_size(cairo_t *cairo, const gchar *text, gint x, gint y, 
    gdouble font_size)
{
  cairo_select_font_face(cairo, "Sans 11", CAIRO_FONT_SLANT_NORMAL, CAIRO_FONT_WEIGHT_NORMAL);
  cairo_set_font_size(cairo, font_size);

  cairo_move_to(cairo, x, y + 13);
  cairo_show_text(cairo, text);
}

void dh_draw_line_with_width(cairo_t *cairo, gint x1, gint y1, gint x2, gint y2,
    gboolean line_is_dashed, gboolean stroke, double line_width)
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

  cairo_set_line_width(cairo, line_width);
  cairo_set_line_cap(cairo, CAIRO_LINE_CAP_ROUND);
  cairo_move_to(cairo, x1, y1);
  cairo_line_to(cairo, x2, y2);

  if (stroke)
  {
    cairo_stroke(cairo);
  }
}

void dh_draw_line(cairo_t *cairo, gint x1, gint y1, gint x2, gint y2,
    gboolean line_is_dashed, gboolean stroke)
{
  dh_draw_line_with_width(cairo, x1, y1, x2, y2, line_is_dashed, stroke, 1.2);
}

void draw_point(cairo_t *cairo, gint x, gint y)
{
  dh_draw_line(cairo, x, y, x, y, FALSE, FALSE);
}


