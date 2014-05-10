/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2005-2014 Alexandru Munteanu - m@ioalex.net
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

#ifndef DOUGLAS_PEUCKER_H
#define DOUGLAS_PEUCKER_H

#include "external_includes.h"
#include "ui_types.h"

typedef struct {
  gdouble distance;
  gint index;
} distance_and_index;

typedef struct {
  GdkPoint point;
  gint index;
} douglas_point;

GPtrArray *splt_douglas_peucker(GArray *gdk_points, void (*callback)(ui_state *ui),
    ui_state *ui, gdouble threshold_to_discard_points, ...);
void splt_douglas_peucker_free(GPtrArray *douglas_peucker_ptr_array);

//for unit tests
gdouble splt_find_distance(GdkPoint first, GdkPoint second);
gdouble splt_find_perpendicular_distance(GdkPoint point,
    GdkPoint segment_begin_point, GdkPoint segment_end_point);
distance_and_index *splt_find_point_with_maximum_distance(GArray *douglas_points,
    GdkPoint first_point, GdkPoint last_point);

#endif

