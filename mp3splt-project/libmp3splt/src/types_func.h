/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2006 Munteanu Alexandru - io_alex_2002@yahoo.fr
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
                                  char *new_filename_path,
                                  int *error);
char *splt_t_get_new_filename_path(splt_state *state);
int splt_t_set_filename_to_split(splt_state *state, char *filename);
int splt_t_set_path_of_split(splt_state *state, char *path);
char *splt_t_get_filename_to_split(splt_state *state);
char *splt_t_get_path_of_split(splt_state *state);

/********************************/
/* types: file format access */

void splt_t_set_file_format(splt_state *state, int file_format);
int splt_t_get_file_format(splt_state *state);

/********************************/
/* types: current split access */

void splt_t_set_current_split(splt_state *state, int index);
int splt_t_get_current_split(splt_state *state);
void splt_t_current_split_next(splt_state *state);

/********************************/
/* types: output format access */

void splt_t_set_oformat(splt_state *state, char *format_string,
                        int *error);
int splt_t_new_oformat(splt_state *state, char *format_string);
char *splt_t_get_oformat(splt_state *state);
void splt_t_set_oformat_digits(splt_state *state);

/********************************/
/* types: splitnumber access */

void splt_t_set_splitnumber(splt_state *state, int number);
int splt_t_get_splitnumber(splt_state *state);

/********************************/
/* types: splitpoints access */

int splt_t_append_splitpoint(splt_state *state, long split_value,
                             char *name);
int splt_t_splitpoint_exists(splt_state *state, int index);
int splt_t_set_splitpoint_value(splt_state *state,
                                int index, long split_value);
int splt_t_set_splitpoint_name(splt_state *state,
                               int index, char *name);
long splt_t_get_splitpoint_value(splt_state *state,
                                 int index, int *error);
char *splt_t_get_splitpoint_name(splt_state *state,
                                 int index, int *error);
splt_point *splt_t_get_splitpoints(splt_state *state,
                                   int *splitpoints_number);

/********************************/
/* types: tags access */

void splt_t_append_original_tags(splt_state *state);
void splt_t_get_original_tags(splt_state *state, int *err);
int splt_t_append_tags(splt_state *state,
                       char *title, char *artist,
                       char *album, char *performer,
                       char *year, char *comment,
                       int track, unsigned char genre);
int splt_t_tags_exists(splt_state *state, int index);
int splt_t_set_original_tags_field(splt_state *state,
                                   int tags_field, int int_data,
                                   char *char_data, unsigned char uchar_data,
                                   int length);
int splt_t_set_tags_char_field(splt_state *state, int index,
                               int tags_field, char *data);
int splt_t_set_tags_int_field(splt_state *state, int index,
                              int tags_field, int data);
int splt_t_set_tags_uchar_field(splt_state *state, int index,
                                int tags_field, unsigned char data);

splt_tags *splt_t_get_tags(splt_state *state,int *tags_number);
char *splt_t_get_tags_char_field(splt_state *state, int index,
                                 int tags_field);
int splt_t_get_tags_int_field(splt_state *state, int index, 
                              int tags_field);
unsigned char splt_t_get_tags_uchar_field(splt_state *state, int index,
                                          int tags_field);
void splt_t_clean_original_tags(splt_state *state);

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

int splt_t_freedb_append_result(splt_state *state,char *album_name,
                                int revision);
int splt_t_freedb_init_search(splt_state *state);
int splt_t_freedb_get_found_cds(splt_state *state);
void splt_t_freedb_found_cds_next(splt_state *state);
void splt_t_freedb_set_disc(splt_state *state, int index,
                            char *category, char *discid);
char *splt_t_freedb_get_disc_category(splt_state *state, 
                                      int index);
char *splt_t_freedb_get_disc_id(splt_state *state,
                                int index);
void splt_t_freedb_free_search(splt_state *state);

/********************************/
/* types: silence access */

int splt_t_ssplit_new(struct splt_ssplit **silence_list, 
                      float begin_position, float end_position, int len);
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
                         int index, char *filename);
void splt_t_wrap_free(splt_state *state);

/******************************/
/* types: client communication */

void splt_t_put_splitted_file(splt_state *state, char *filename);
void splt_t_put_progress_text(splt_state *state,int type);
void splt_t_put_message_to_client(splt_state *state, int message);
void splt_t_update_progress(splt_state *state, float current_point,
                            float total_points, int progress_stage,
                            float progress_start, int refresh_rate);

/********************************/
/* types: miscelanneous */

void splt_t_free_splitpoints_tags(splt_state *state);
void splt_t_free_splitpoints(splt_state *state);
void splt_t_free_tags(splt_state *state);
void splt_t_clean_one_split_data(splt_state *state, int num);
void splt_t_clean_split_data(splt_state *state,int tracks);
int splt_t_split_is_canceled(splt_state *state);
void splt_t_set_stop_split(splt_state *state, int bool_value);
