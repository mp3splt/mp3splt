/**********************************************************
 *
 * mp3splt-gtk -- utility based on mp3splt,
 *                for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2005-2009 Alexandru Munteanu - io_fx@yahoo.fr
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

/**********************************************************
 * Filename: special_split.h
 *
 * header of special_split.c, defines constants, etc..
 *
 *********************************************************/

#define SELECTED_SPLIT_NORMAL 3
#define SELECTED_SPLIT_TIME 2
#define SELECTED_SPLIT_WRAP 1
#define SELECTED_SPLIT_ERROR 0

#define NUMBER_OF_SPLIT_MODES 4

gint get_selected_split_mode(GtkToggleButton *radio_b);
void deactivate_silence_parameters();
void activate_silence_parameters();
void split_mode_changed (GtkToggleButton *radio_b,
                         gpointer data);
void set_default_split_modes (GtkWidget *widget, 
                              gpointer data);
GtkWidget *create_special_split_page();
