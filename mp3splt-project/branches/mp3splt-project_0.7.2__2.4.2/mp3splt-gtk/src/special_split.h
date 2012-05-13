/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2005-2012 Alexandru Munteanu - io_fx@yahoo.fr
 *
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

#ifndef SPECIAL_SPLIT_H

#define FILE_MODE_SINGLE 1
#define FILE_MODE_MULTIPLE 0

#define SELECTED_SPLIT_TRIM_SILENCE 6
#define SELECTED_SPLIT_SILENCE 5
#define SELECTED_SPLIT_EQUAL_TIME_TRACKS 4
#define SELECTED_SPLIT_NORMAL 3
#define SELECTED_SPLIT_TIME 2
#define SELECTED_SPLIT_WRAP 1
#define SELECTED_SPLIT_ERROR 0

#define NUMBER_OF_SPLIT_MODES 6

void select_split_mode(int split_mode);

void deactivate_silence_parameters();
void activate_silence_parameters();
void split_mode_changed (GtkToggleButton *radio_b, gpointer data);
GtkWidget *create_special_split_page();

#define SPECIAL_SPLIT_H
#endif

