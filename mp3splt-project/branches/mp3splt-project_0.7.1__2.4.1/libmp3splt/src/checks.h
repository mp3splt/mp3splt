/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2011 Alexandru Munteanu - io_fx@yahoo.fr
 *
 * http://mp3splt.sourceforge.net
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307,
 * USA.
 *
 *********************************************************/

#ifndef MP3SPLT_CHECKS_H

/****************************/
/* splitpoints checks */

void splt_check_points_inf_song_length_and_convert_negatives(splt_state *state,
    int *error);
void splt_check_if_points_in_order(splt_state *state,
    int *error);

/****************************/
/* path check */

void splt_check_if_fname_path_is_correct(splt_state *state,
    const char *new_filename_path, int *error);
char *splt_check_put_dir_of_cur_song(const char *filename,
    const char *the_filename_path, int *error);

/****************************/
/* options check */

int splt_check_compatible_options(splt_state *state);
void splt_check_set_correct_options(splt_state *state);

/****************************/
/* file checks */

void splt_check_file_type(splt_state *state, int *error);
int splt_check_is_the_same_file(splt_state *state, const char *file1,
    const char *file2, int *error);

#define MP3SPLT_CHECKS_H

#endif

