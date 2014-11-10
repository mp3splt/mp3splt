/**********************************************************
 *
 * libmp3splt -- library based on mp3splt,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2014 Alexandru Munteanu - m@ioalex.net
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301,
 * USA.
 *
 *********************************************************/

#ifndef MP3SPLT_SPLT_H

#include <stdio.h>
#include <sys/types.h>

//libtool 1.4e is buggy on mingw if we include ltdl.h
#ifndef __WIN32__
#include <ltdl.h>
#endif

#include "mp3splt.h"

struct _splt_freedb_one_result {
  /**
   * @brief Name of the album for this result
   */
  char *name;
  /**
   * @brief Unique identifier for this result
   */
  int id;
  /**
   * @brief How many revisions this result has
   *
   * If having revisions, the unique identifier for a revision is
   * #id + revision + 1, with 0 <= revision < #revision_number\n
   */
  int revision_number;
  /**
   * @brief Contains the revisions of the result
   * 
   * The #revisions table contains #revision_number elements,
   * the consecutive numbers starting at 2 (you might not need it)
   */
  int *revisions;
};

struct _splt_freedb_results {
  /**
   * @brief All the freedb results
   */
  splt_freedb_one_result *results;
  /**
   * @brief How many results we have
   */
  int number;
  int iterator_counter;
};

/**
 * @brief Maximum results for the freedb search
 */
#define SPLT_MAXCD 512

//maximum length of the disc id
#define SPLT_DISCIDLEN 8

//structure for the freedb search
struct splt_cd {
  char discid[SPLT_DISCIDLEN+1];
  char category[20];
};

typedef struct {
  struct splt_cd discs[SPLT_MAXCD];
  int foundcd;
} splt_cd_state;

//structure containing everything used for the
//freedb search
typedef struct {
  //we stock here the results of the freedb search
  struct _splt_freedb_results *search_results;
  //we stock the state of the CD
  //(for the freedb search)
  splt_cd_state *cdstate;
} splt_freedb;

typedef char _splt_one_wrap;

struct _splt_wrap {
  /**
   * @brief How many filenames we have
   */
  int wrap_files_num;
  /**
   * @brief The filenames
   */
  char **wrap_files;
  int iterator_counter;
};

struct _splt_syncerrors {
  off_t *serrors_points;
  /**
   * @brief How many syncerrors have been found
   */
  long int serrors_points_num;
};

/**
 * @brief The number of syncerrors
 *
 * @see mp3splt_get_syncerrors
 */
typedef struct _splt_syncerrors splt_syncerrors;

struct _splt_tags {
  /**
   * @brief The title
   */
  char *title;
  /**
   * @brief The artist
   */
  char *artist;
  /**
   * @brief The album
   */
  char *album;
  /**
   * @brief The performer
   * 
   * The performer is not part of the tags, but may replace the
   * #artist in some cases and it is useful for the output filenames\n
   * You can also look at #SPLT_OPT_OUTPUT_DEFAULT
   */
  char *performer;
  /**
   * @brief The year
   */
  char *year;
  /**
   * @brief A comment
   */
  char *comment;
  /**
   * @brief The track number
   */
  int track;
  /**
   * @brief The genre
   */
  char *genre;

  /*
   * @brief tags version (for mp3): 1 or 2 or 1 & 2
   */
  int tags_version;

  int set_original_tags;
  int was_auto_incremented;
};

struct _splt_original_tags {
  splt_tags tags;
  void *all_original_tags;
  int last_plugin_used;
};

struct _splt_tags_group {
  splt_tags *tags;
  int real_tagsnumber;
  int iterator_counter;
};

#define SPLT_MAXOLEN 255
#define SPLT_OUTNUM  20

//structure defining the output format
typedef struct {
  //format as @n_@t.. as a string
  char *format_string;
  //when we have @n option on output format
  char output_format_digits;
  int output_alpha_format_digits;
  //format for the cddb cue output
  char format[SPLT_OUTNUM+1][SPLT_MAXOLEN];
} splt_oformat;

struct _splt_point {
  /**
   * @brief Value of the splitpoint in hundreths of seconds
   */
  long value;
  /**
   * @brief Name of the new filename issued from the splitpoint
   */
  char *name;
  /**
   * @brief Type of the splitpoint
   * @see splt_type_of_splitpoint
   */
  int type;
};

struct _splt_points {
  splt_point *points;
  int real_splitnumber;
  int iterator_counter;
};

/**
 * @brief The data structure for the progress bar
 */
struct splt_progres {
  /*!maximum number of characters for the filename

    (without the extension) when displaying the
    progress warning; default is 40 */
  int progress_text_max_char;
  //! Name of the file  we are currently splitting
  char filename_shorted[512];
  //!the current percent of the progress
  float percent_progress;
  //!the splitpoint we are currently splitting
  int current_split;
  //!the maximum number of splits
  int max_splits;
  /*!the progress bar type

    can be:
    - SPLT_PROGRESS_PREPARE
    - SPLT_PROGRESS_CREATE
    - SPLT_PROGRESS_SEARCH_SYNC
    - SPLT_PROGRESS_SCAN_SILENCE */
  int progress_type;
  //!infos for the silence split
  int silence_found_tracks;
  //! The silence leven in dB
  float silence_db_level;
  //! A pointer to the callback function
  void (*progress)(struct splt_progres*, void *);
  void *progress_cb_data;
};

//! The data libmp3splt keeps all its internal data in
typedef struct {
  //! total time of the song
  long total_time;
  //!When splitting: the number of the last splitpoint we arrived at.
  int current_split;
  //! for user feedback: the number of the file we are splitting
  int current_split_file_number;
  /*! how many split points do we have?
    
    this is updated only by
    check_splitpts_inf_song_length()
    If this function has not been called
    look at points->real_splitnumber instead.
  */
  int splitnumber;
  /*! Pointer to the fallback function that is 
      called when the library starts to split
      a new file.
      
      the char* is the filename that is currently
      being split.
  */
  void (*file_split)(const char *, void *);
  void *file_split_cb_data;

  void (*write_cb)(const void *ptr, size_t size, size_t nmemb, void *cb_data);
  void *write_cb_data;

  //!All infos for the progress bar
  splt_progress *p_bar;
  //!callback for sending the silence level to the client
  void (*get_silence_level)(long time, float level, void *user_data);
  //!user data set by the client for the 'get_silence_level' function
  void *silence_level_client_data;
  //!sends a message to the main program to tell what we are doing
  void (*put_message)(const char *, splt_message_type, void *);
  void *put_message_cb_data;
  //!structure in which we have all the splitpoints
  splt_points *points;

  splt_tags tags_like_x;

  splt_tags_group *tags_group;
} splt_struct;

//!structure with all the options supplied to split the file
typedef struct {
  /*! this field can take the following values:
     - SPLT_OPTION_NORMAL_MODE
     - SPLT_OPTION_WRAP_MODE
     - SPLT_OPTION_SILENCE_MODE
     - SPLT_OPTION_TRIM_SILENCE_MODE
     - SPLT_OPTION_ERROR_MODE
     - SPLT_OPTION_TIME_MODE
  */
  splt_split_mode_options split_mode;

  /*! might be:
       - SPLT_TAGS_ORIGINAL_FILE - write tags from original file
       - SPLT_NO_TAGS - does not write any tags
       - SPLT_CURRENT_TAGS - tags issued from the cddb or cue for example
       - or be set manually with the functions
         splt_tags_options tags;
  */
  splt_tags_options tags;

  int xing;

  //!defines the output filenames
  splt_output_filenames_options output_filenames;

  //!quiet mode: don't perform CRC check or other interaction with the user
  int quiet_mode;

  /*!Pretend to split the file, without real split

    this option works in all modes except error mode and dewrap split. 
  */
  int pretend_to_split;

  /*! frame mode (mp3 only). 

    Process all frames, seeking split positions by counting frames
    instead of bitrate guessing.
  */
  int option_frame_mode;
  //!the time of split when split_mode = OPTION_TIME_SPLIT
  long split_time;
  long overlap_time;
  //!this option uses silence detection to auto-adjust splitpoints.
  int option_auto_adjust;
  /*! True means: input not seekable. 

    enabling this allows you to split mp3 and ogg streams
    which can be read only one time and canât be seeked.
    WARNING!
    if you don't know what this means you know why it is wise
    to set this to FALSE.
  */
  int option_input_not_seekable;

  /*! SPLT_TRUE= don't patse output filenames for illegal characters.

    Otherwise, we parse for illegal characters the filenames and replace
    them with '_'. The tags are always checked for illegal characters when
    set into filenames.
  */
  int create_dirs_from_filenames;

  //PARAMETERS---------------------------------------
  //PARAMETERS for option_auto_adjust and option_silence_mode:
  /*! the sound level to be considered silence
 
   (is a float number between -96 and 0. Default is -48 dB)*/
  float parameter_threshold;
  /*! the offset of cutpoint in silence

    Float number between -2 and 2 and allows you to adjust the offset
    of cutpoint in silence time.0 is the begin of silence, and 1 the
    end;default is 0.8. 

    Used for option_auto_adjust and option_silence_mode
  */
  float parameter_offset;

  //PARAMETERS for option_silence_mode:
  /*! the desired number of tracks for option_silence_mode

  (positive integer number of tracks to be split;by default all
  tracks are split)
  */
  int parameter_number_tracks;
  /*!
     this option defines the number of shots to be found after the silence.
     Decrease this value if you need to split files having closer silence points.

     (positive integer number; default is #SPLT_DEFAULT_PARAM_SHOTS)
  */
  int parameter_shots;
  /*! The minimum silence length for option_silence_mode [in seconds]
    
    A positive float of the minimum number of seconds to be considered
    a valid splitpoint
  */
  float parameter_minimum_length;
  /*! The minimum track length for option_silence_mode [in seconds]
 
    A positive float of the minimum number of seconds to be considered
    a valid splitpoint
  */
  float parameter_min_track_length;

  /*! The minimum track length for option_silence_mode [in seconds] to be kept as a valid segment.
  
    A positive float of the minimum number of seconds to be left as output file. Smaller
    segments will be joined among others.
  */
  float parameter_min_track_join;

  //!possible values are #splt_str_format
  int artist_tag_format;
  //!possible values are #splt_str_format
  int album_tag_format;
  //!possible values are #splt_str_format
  int title_tag_format;
  //!possible values are #splt_str_format
  int comment_tag_format;

  //!true=replace underscores with space
  int replace_underscores_tag_format;

  //!SPLT_TRUE=set the current filename to split from FILE tag - if found in the CUE file
  int set_file_from_cue_if_file_tag_found;

  //!true=remove the silence between split tracks
  int parameter_remove_silence;

  //!when using the 'remove silence' parameter, number of seconds to keep at the beginning
  float keep_silence_left;
  //!when using the 'remove silence' parameter, number of seconds to keep at the end 
  float keep_silence_right;

  //PARAMETERS for option_auto_adjust:
  /*! Auto-Adjust: the gap value around splitpoint to search for
    silence 
    
    (positive integer for the time to decode before and after
    splitpoint;default gap is 30 seconds)
  */
  int parameter_gap;

  /*! Copy all tags from tag number 'remaining_tags_like_x'
    
    set to -1 to disable
  */
  int remaining_tags_like_x;

  //!true=auto-increment the tracknumber of the tags
  int auto_increment_tracknumber_tags;

  /**
   * true=enable the silence points log ('mp3splt.log')
   */
  int enable_silence_log;

  /**
   * If we force the mp3 tags version to 1 or 2 or 1 & 2;
   * Set to 0 if force disable;
   * If disabled, original tags version is set to split files,
   *  or version 1 if compiled without libid3tag support
   */
  int force_tags_version;
  /**
   * the number of files to be created when splitting by equal time length
   */
  int length_split_file_number;
  int replace_tags_in_tags;

  int cue_set_splitpoint_names_from_rem_name;
  int cue_disable_cue_file_created_message;
  int cue_cddb_add_tags_with_keep_original_tags;
  int warn_if_no_auto_adjust_found;
  int stop_if_no_auto_adjust_found;
  int decode_and_write_flac_md5sum;
  int handle_bit_reservoir;
  int id3v2_encoding;
  int input_tags_encoding;
  long time_minimum_length;
} splt_options_variables;

//internal structures
typedef struct {
  //!Do we have send the message frame mode enabled?
  int frame_mode_enabled;
  //!if current_refresh_rate = refresh_rate, we call the progress callback
  int current_refresh_rate;
  //! if set to SPLT_TRUE then we don't send messages to clients
  int messages_locked;
  //!if we currently use the library, we lock it
  int library_locked;
  //!the new filename path (internal)
  char *new_filename_path;
} splt_internal;

typedef enum {
  SPLT_OPT_ALL_REMAINING_TAGS_LIKE_X = 10000,
  SPLT_OPT_AUTO_INCREMENT_TRACKNUMBER_TAGS,
} splt_internal_options;

//!structure containing all the data about a plugin
typedef struct {
  splt_plugin_info info;
  //!complete filename of the plugin shared object
  char *plugin_filename;
  //! plugin handle get with lt_dlopen, used later for lt_dlclose
  void *plugin_handle;
  //! plugin functions
  splt_plugin_func *func;
} splt_plugin_data;

//!internal plugins structure
typedef struct {
  //!directories where we scan for plugins
  char **plugins_scan_dirs;
  int number_of_dirs_to_scan;
  //!the number of plugins found
  int number_of_plugins_found;
  //!data structure about all the plugins
  splt_plugin_data *data;
} splt_plugins;

//!structure containing error strings for error messages
typedef struct {
  char *error_data;
  char *strerror_msg;
} splt_error;

struct splt_ssplit {
  double begin_position;
  double end_position;
  long len;
  struct splt_ssplit *next;
};

typedef struct {
  char *proxy_address;
  int proxy_port;
  char *authentification;
} splt_proxy;

struct _splt_state {
  //!setting to SPLT_TRUE cancels the split
  int cancel_split;
  //!filename to split
  char *fname_to_split;
  //!where the split file will be split
  char *path_of_split;

  //if this is non null, we write a m3u from the split files
  char *m3u_filename;
  
  //!setting tags from input filename regex
  char *input_fname_regex;

  char *default_comment_tag;
  char *default_genre_tag;

  //!tags of the original file to split
  splt_original_tags original_tags;

  //!options for the split
  splt_options_variables options;
  //!split related
  splt_struct split;
  //!output format  
  splt_oformat oformat;
  //!wrap related
  splt_wrap *wrap;
  //!syncerror related
  splt_syncerrors *serrors;
  /*! counter for the number of sync errors found

    - the state->serros->serrors_points_num must be used when processing the
      syncerrors from 'serrors' 
    - this is just a standalone counter
  */
  unsigned long syncerrors;
  //!freedb related
  splt_freedb fdb;

  //!internal options
  splt_internal iopts;

  //!see the ssplit structure
  struct splt_ssplit *silence_list;

  splt_proxy proxy;

  //!file format states, mp3,ogg..
  void *codec;

  //!error strings for error code messages
  splt_error err;

  //!plugins structure
  splt_plugins *plug;
  int current_plugin;

  //!filename of the silence log: 'mp3splt.log' in the original mp3splt
  char *silence_log_fname;
  char *silence_full_log_fname;
  FILE *full_log_file_descriptor;
};

#include "pair.h"
#include "types_func.h"
#include "cddb.h"
#include "utils.h"
#include "checks.h"
#include "plugins.h"
#include "cue.h"
#include "cddb.h"
#include "freedb.h"
#include "audacity.h"
#include "splt_array.h"
#include "string_utils.h"
#include "tags_utils.h"
#include "input_output.h"
#include "options.h"
#include "output_format.h"
#include "split_points.h"
#include "errors.h"
#include "freedb_utils.h"
#include "silence_utils.h"
#include "sync_errors.h"
#include "wrap.h"
#include "client.h"
#include "conversions.h"
#include "tags_parser.h"
#include "oformat_parser.h"
#include "debug.h"
#include "filename_regex.h"
#include "win32.h"
#include "proxy.h"

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <limits.h>
#include <locale.h>

#ifdef ENABLE_NLS
 #include <libintl.h>

 #ifndef __WIN32
  #include <langinfo.h>
 #endif
#endif

#ifdef __WIN32__

#define fseeko fseeko64
#define ftello ftello64

#else

#if !HAVE_FSEEKO
#define fseeko fseek
#define ftello ftell
#endif

#endif

#define MP3SPLT_LIB_GETTEXT_DOMAIN "libmp3splt"SPLT_SONAME

#ifdef ENABLE_NLS
#  define _(STR) dgettext(MP3SPLT_LIB_GETTEXT_DOMAIN, STR)
#else
#  define _(STR) ((const char *)STR)
#endif

/**********************************/
/* splt normal or syncerror split */

void splt_s_error_split(splt_state *state, int *error);
void splt_s_multiple_split(splt_state *state, int *error);
void splt_s_normal_split(splt_state *state, int *error);

/************************************/
/* splt time and equal length split */

void splt_s_time_split(splt_state *state, int *error);
void splt_s_equal_length_split(splt_state *state, int *error);

/************************************/
/* splt silence detection and split */

int splt_s_set_silence_splitpoints(splt_state *state, int *error);
int splt_s_set_trim_silence_splitpoints(splt_state *state, int *error);
void splt_s_silence_split(splt_state *state, int *error);
void splt_s_trim_silence_split(splt_state *state, int *error);

/****************************/
/* splt wrap split */

void splt_s_wrap_split(splt_state *state, int *error);

/* other stuff:/ */

#define SPLT_DEFAULT_PROGRESS_RATE 350
#define SPLT_DEFAULT_PROGRESS_RATE2 50

#define SPLT_DEFAULTSILLEN 10

#define SPLT_VARCHAR '@'

//max number of splitpoints 
//for silence detection and for syncerrors
#define SPLT_MAXSYNC INT_MAX
#define SPLT_MAXSILENCE INT_MAX

/* libmp3splt internals */
#define SPLT_IERROR_INT -1
#define SPLT_IERROR_SET_ORIGINAL_TAGS -2
#define SPLT_IERROR_CHAR -3

//following mp3splt.h->#splt_tag_key
#define SPLT_TAGS_VERSION 800

#define SPLT_ORIGINAL_TAGS_DEFAULT "%[@o,@N=1]"

#define SPLT_INTERNAL_PROGRESS_RATE 1
#define SPLT_INTERNAL_FRAME_MODE_ENABLED 2

//package information constants
#ifndef SPLT_PACKAGE_NAME
/**
 * @brief Package name
 */
#define SPLT_PACKAGE_NAME "libmp3splt"
#endif

#ifndef SPLT_PACKAGE_VERSION
/**
 * @brief Package version
 */
#define SPLT_PACKAGE_VERSION LIBMP3SPLT_VERSION
#endif

/**
 * @brief Package authors
 */
#define SPLT_AUTHOR "Matteo Trotta | Munteanu Alexandru"
#define SPLT_EMAIL "<mtrotta@users.sourceforge.net> | <m@ioalex.net>"
/**
 * @brief Package website
 */
#define SPLT_WEBSITE "http://mp3splt.sourceforge.net"

/**
 * @brief Freedb search type: NOT IMPLEMENTED yet
 */
#define SPLT_FREEDB_SEARCH_TYPE_CDDB 2

#ifndef SPLT_NDIRCHAR
#ifdef __WIN32__
#define SPLT_NDIRCHAR '/'
#else
#define SPLT_NDIRCHAR '\\'
#endif
#endif

#define MP3SPLT_SPLT_H

#endif

