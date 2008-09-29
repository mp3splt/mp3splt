/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2008 Munteanu Alexandru - io_fx@yahoo.fr
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

#ifndef MP3SPLT_TYPES_FUNC

/********************************/
/* types: state access */

splt_state *splt_t_new_state(splt_state *state, int *error);
void splt_t_free_state(splt_state *state);

/********************************/
/* types: total time access */

void splt_t_set_total_time(splt_state *state, long value);
long splt_t_get_total_time(splt_state *state);

/**********************************/
/* types: filename and path access */

void splt_t_set_new_filename_path(splt_state *state, 
    const char *new_filename_path, int *error);
char *splt_t_get_new_filename_path(splt_state *state);
int splt_t_set_filename_to_split(splt_state *state, const char *filename);
int splt_t_set_path_of_split(splt_state *state, const char *path);
int splt_t_set_m3u_filename(splt_state *state, const char *filename);
int splt_t_set_silence_log_fname(splt_state *state, const char *filename);
char *splt_t_get_filename_to_split(splt_state *state);
char *splt_t_get_path_of_split(splt_state *state);
char *splt_t_get_m3u_filename(splt_state *state);
char *splt_t_get_silence_log_fname(splt_state *state);
char *splt_t_get_m3u_file_with_path(splt_state *state, int *error);

/********************************/
/* types: current split access */

void splt_t_set_current_split(splt_state *state, int index);
int splt_t_get_current_split(splt_state *state);
void splt_t_current_split_next(splt_state *state);

/********************************/
/* types: output format access */

void splt_t_set_oformat(splt_state *state, const char *format_string,
    int *error);
int splt_t_new_oformat(splt_state *state, const char *format_string);
const char *splt_t_get_oformat(splt_state *state);
void splt_t_set_oformat_digits(splt_state *state);

/********************************/
/* types: splitnumber access */

void splt_t_set_splitnumber(splt_state *state, int number);
int splt_t_get_splitnumber(splt_state *state);

/********************************/
/* types: splitpoints access */

int splt_t_append_splitpoint(splt_state *state, long split_value,
    const char *name);
int splt_t_splitpoint_exists(splt_state *state, int index);
int splt_t_set_splitpoint_value(splt_state *state,
    int index, long split_value);
int splt_t_set_splitpoint_name(splt_state *state,
    int index, const char *name);
long splt_t_get_splitpoint_value(splt_state *state,
    int index, int *error);
char *splt_t_get_splitpoint_name(splt_state *state,
    int index, int *error);
splt_point *splt_t_get_splitpoints(splt_state *state,
    int *splitpoints_number);

/********************************/
/* types: tags access */

int splt_t_append_original_tags(splt_state *state);
void splt_t_get_original_tags(splt_state *state, int *err);
int splt_t_append_tags(splt_state *state,
    const char *title, const char *artist,
    const char *album, const char *performer,
    const char *year, const char *comment,
    int track, unsigned char genre);
int splt_t_append_only_non_null_previous_tags(splt_state *state, 
    const char *title, const char *artist,
    const char *album, const char *performer,
    const char *year, const char *comment,
    int track, unsigned char genre);
int splt_t_tags_exists(splt_state *state, int index);
int splt_t_set_original_tags_field(splt_state *state,
    int tags_field, int int_data,
    const char *char_data, unsigned char uchar_data,
    int length);
int splt_t_set_tags_char_field(splt_state *state, int index,
    int tags_field, const char *data);
int splt_t_set_tags_uchar_field(splt_state *state, int index,
    int tags_field, unsigned char data);
int splt_t_set_tags_int_field(splt_state *state, int index,
    int tags_field, int data);

splt_tags *splt_t_get_tags(splt_state *state,int *tags_number);
char *splt_t_get_tags_char_field(splt_state *state, int index,
    int tags_field);
int splt_t_get_tags_int_field(splt_state *state, int index, 
    int tags_field);
unsigned char splt_t_get_tags_uchar_field(splt_state *state, int index,
    int tags_field);
splt_tags splt_t_get_last_tags(splt_state *state);

/********************************/
/* types: options access */

void splt_t_set_int_option(splt_state *state, int option_name,
    int value);
void splt_t_set_float_option(splt_state *state, int option_name,
    float value);
int splt_t_get_int_option(splt_state *state, int option_name);
float splt_t_get_float_option(splt_state *state, int option_name);
//internal :
void splt_t_set_i_begin_point(splt_state *state, double value);
void splt_t_set_i_end_point(splt_state *state, double value);
double splt_t_get_i_begin_point(splt_state *state);
double splt_t_get_i_end_point(splt_state *state);
void splt_t_set_iopt(splt_state *state, int type, int value);
void splt_t_lock_messages(splt_state *state);
int splt_t_get_iopt(splt_state *state, int type);
void splt_t_set_default_iopts(splt_state *state);
void splt_t_unlock_messages(splt_state *state);
int splt_t_messages_locked(splt_state *state);
int splt_t_library_locked(splt_state *state);
void splt_t_lock_library(splt_state *state);
void splt_t_unlock_library(splt_state *state);

/********************************/
/* types: freedb access */

int splt_t_freedb_append_result(splt_state *state, const char *album_name, int revision);
int splt_t_freedb_init_search(splt_state *state);
int splt_t_freedb_get_found_cds(splt_state *state);
void splt_t_freedb_found_cds_next(splt_state *state);
void splt_t_freedb_set_disc(splt_state *state, int index,
    const char *discid, const char *category, int category_size);
const char *splt_t_freedb_get_disc_category(splt_state *state, int index);
const char *splt_t_freedb_get_disc_id(splt_state *state, int index);
void splt_t_freedb_free_search(splt_state *state);

/********************************/
/* types: silence access */

int splt_t_ssplit_new(struct splt_ssplit **silence_list, 
    float begin_position, float end_position, int len, int *error);
void splt_t_ssplit_free (struct splt_ssplit **silence_list);

/********************************/
/* types: syncerrors access */

int splt_t_serrors_append_point(splt_state *state, off_t point);
void splt_t_serrors_set_point(splt_state *state, int index,
    off_t point);
off_t splt_t_serrors_get_point(splt_state *state, int index);
void splt_t_serrors_free(splt_state *state);

/********************************/
/* types: wrap access */

int splt_t_wrap_put_file(splt_state *state, int wrapfiles,
    int index, const char *filename);
void splt_t_wrap_free(splt_state *state);

/******************************/
/* types: client communication */


int splt_t_put_split_file(splt_state *state, const char *filename);
void splt_t_put_progress_text(splt_state *state,int type);
void splt_t_put_message_to_client(splt_state *state, char *message);
void splt_t_update_progress(splt_state *state, float current_point,
    float total_points, int progress_stage,
    float progress_start, int refresh_rate);

/********************************/
/* types: miscelanneous */

void splt_t_get_mins_secs_hundr_from_splitpoint(long splitpoint,
    long *mins, long *secs, long *hudr);
void splt_t_set_error_data(splt_state *state, const char *error_data);
void splt_t_set_error_data_from_splitpoints(splt_state *state, long splitpoint1,
    long splitpoint2);
void splt_t_set_error_data_from_splitpoint(splt_state *state, long splitpoint);
void splt_t_set_strerr_msg(splt_state *state, const char *message);
void splt_t_set_strerror_msg(splt_state *state);
void splt_t_set_strherror_msg(splt_state *state);
void splt_t_clean_strerror_msg(splt_state *state);

void splt_t_free_splitpoints_tags(splt_state *state);
void splt_t_free_splitpoints(splt_state *state);
void splt_t_free_tags(splt_state *state);
void splt_t_clean_original_tags(splt_state *state);
void splt_t_clean_one_split_data(splt_state *state, int num);
void splt_t_clean_split_data(splt_state *state,int tracks);
int splt_t_split_is_canceled(splt_state *state);
void splt_t_set_stop_split(splt_state *state, int bool_value);

#define MP3SPLT_TYPES_FUNC

#endif

