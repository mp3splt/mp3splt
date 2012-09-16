/**********************************************************
 *
 * libmp3splt -- library based on mp3splt v2.1c,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2012 Alexandru Munteanu - io_fx@yahoo.fr
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

/**
 * @file  mp3splt.h
 * 
 * @brief libmp3splt API 
 */

#ifndef MP3SPLT_MP3SPLT_H

/**
 * @brief True value
 */
#define SPLT_TRUE 1
/**
 * @brief False value
 */
#define SPLT_FALSE 0

/************************************/
/*! @defgroup splt_error_codes Confirmation and error codes

@{
 */

//! Confirmation and error codes
typedef enum {
  SPLT_OK = 0,

  SPLT_OK_SPLIT = 1,
  SPLT_SPLITPOINT_BIGGER_THAN_LENGTH = 4,
  SPLT_SILENCE_OK = 5,
  SPLT_TIME_SPLIT_OK = 6,
  SPLT_NO_SILENCE_SPLITPOINTS_FOUND = 7,
  SPLT_OK_SPLIT_EOF = 8,
  SPLT_LENGTH_SPLIT_OK = 9,
  SPLT_TRIM_SILENCE_OK = 10,

  SPLT_FREEDB_OK = 100,
  SPLT_FREEDB_FILE_OK = 101,
  SPLT_CDDB_OK = 102,
  SPLT_CUE_OK = 103,
  SPLT_FREEDB_MAX_CD_REACHED = 104,
  SPLT_AUDACITY_OK = 105,

  SPLT_DEWRAP_OK = 200,

  SPLT_SYNC_OK = 300,
  SPLT_MIGHT_BE_VBR = 301,

  SPLT_ERR_SYNC = -300,
  SPLT_ERR_NO_SYNC_FOUND = -301,
  SPLT_ERR_TOO_MANY_SYNC_ERR = -302,

  SPLT_OUTPUT_FORMAT_OK = 400,
  SPLT_OUTPUT_FORMAT_AMBIGUOUS = 401,

  SPLT_REGEX_OK = 800,

  SPLT_ERROR_SPLITPOINTS = -1,
  SPLT_ERROR_CANNOT_OPEN_FILE = -2,
  SPLT_ERROR_INVALID = -3,
  SPLT_ERROR_EQUAL_SPLITPOINTS = -5,
  SPLT_ERROR_SPLITPOINTS_NOT_IN_ORDER = -6,
  SPLT_ERROR_NEGATIVE_SPLITPOINT = -7,
  SPLT_ERROR_INCORRECT_PATH = -8,
  SPLT_ERROR_INCOMPATIBLE_OPTIONS = -10,
  SPLT_ERROR_INPUT_OUTPUT_SAME_FILE = -12,
  SPLT_ERROR_CANNOT_ALLOCATE_MEMORY = -15,
  SPLT_ERROR_CANNOT_OPEN_DEST_FILE = -16,
  SPLT_ERROR_CANT_WRITE_TO_OUTPUT_FILE = -17,
  SPLT_ERROR_WHILE_READING_FILE = -18,
  SPLT_ERROR_SEEKING_FILE = -19,
  SPLT_ERROR_BEGIN_OUT_OF_FILE = -20,
  SPLT_ERROR_INEXISTENT_FILE = -21,
  SPLT_SPLIT_CANCELLED = -22,
  SPLT_ERROR_LIBRARY_LOCKED = -24,
  SPLT_ERROR_STATE_NULL = -25,
  SPLT_ERROR_NEGATIVE_TIME_SPLIT = -26,
  SPLT_ERROR_CANNOT_CREATE_DIRECTORY = -27,
  SPLT_ERROR_CANNOT_CLOSE_FILE = -28,
  SPLT_ERROR_NO_PLUGIN_FOUND = -29,
  SPLT_ERROR_CANNOT_INIT_LIBLTDL = -30,
  SPLT_ERROR_CRC_FAILED = -31,
  SPLT_ERROR_NO_PLUGIN_FOUND_FOR_FILE = -32,
  SPLT_ERROR_PLUGIN_ERROR = -33,
  SPLT_ERROR_TIME_SPLIT_VALUE_INVALID = -34,
  SPLT_ERROR_LENGTH_SPLIT_VALUE_INVALID = -35,
  SPLT_ERROR_CANNOT_GET_TOTAL_TIME = -36,
  SPLT_ERROR_LIBID3 = -37,

  SPLT_FREEDB_ERROR_INITIALISE_SOCKET = -101,
  SPLT_FREEDB_ERROR_CANNOT_GET_HOST = -102,
  SPLT_FREEDB_ERROR_CANNOT_OPEN_SOCKET = -103,
  SPLT_FREEDB_ERROR_CANNOT_CONNECT = -104,
  SPLT_FREEDB_ERROR_CANNOT_SEND_MESSAGE = -105,
  SPLT_FREEDB_ERROR_INVALID_SERVER_ANSWER = -106,
  SPLT_FREEDB_ERROR_SITE_201 = -107,
  SPLT_FREEDB_ERROR_SITE_200 = -108,
  SPLT_FREEDB_ERROR_BAD_COMMUNICATION = -109,
  SPLT_FREEDB_ERROR_GETTING_INFOS = -110,
  SPLT_FREEDB_NO_CD_FOUND = -111,
  SPLT_FREEDB_ERROR_CANNOT_RECV_MESSAGE = -112,
  SPLT_INVALID_CUE_FILE = -115,
  SPLT_INVALID_CDDB_FILE = -116,
  SPLT_FREEDB_NO_SUCH_CD_IN_DATABASE = -118,
  SPLT_FREEDB_ERROR_SITE = -119,
  SPLT_FREEDB_ERROR_CANNOT_DISCONNECT = -120,

  SPLT_DEWRAP_ERR_FILE_LENGTH = -200,
  SPLT_DEWRAP_ERR_VERSION_OLD = -201,
  SPLT_DEWRAP_ERR_NO_FILE_OR_BAD_INDEX = -202,
  SPLT_DEWRAP_ERR_FILE_DAMAGED_INCOMPLETE = -203,
  SPLT_DEWRAP_ERR_FILE_NOT_WRAPED_DAMAGED = -204,

  SPLT_OUTPUT_FORMAT_ERROR = -400,

  SPLT_ERROR_INEXISTENT_SPLITPOINT = -500,

  SPLT_PLUGIN_ERROR_UNSUPPORTED_FEATURE = -600,

  SPLT_INVALID_AUDACITY_FILE = -700,

  SPLT_INVALID_REGEX = -800,
  SPLT_REGEX_NO_MATCH = -801,
  SPLT_REGEX_UNAVAILABLE = -802,
} splt_code;

//@}

/************************************/
/*! @defgroup splt_options Options

@{
 */

/**
 * @brief Options
 *
 * Use #mp3splt_set_int_option #mp3splt_set_long_option and #mp3splt_set_float_option to set those options
 * Use #mp3splt_get_int_option #mp3splt_get_long_option and #mp3splt_get_float_option to get those options
 */
typedef enum {
  /**
   * Pretend to split the file, without actually creating output files.
   * This option works in all modes except error mode and dewrap split.
   *
   * Int option that can take the values #SPLT_TRUE or #SPLT_FALSE.
   *
   * Default is #SPLT_FALSE
   */
  SPLT_OPT_PRETEND_TO_SPLIT = 1,
  /*
   * If quiet return less messages and don't do mp3 CRC check.
   *
   * Int option that can take the values #SPLT_TRUE or #SPLT_FALSE.
   *
   * Default is #SPLT_FALSE
   */
  SPLT_OPT_QUIET_MODE,
  /**
   * If we return debug messages or not.
   *
   * Int option that can take the values #SPLT_TRUE or #SPLT_FALSE.
   *
   * Default is #SPLT_FALSE
   */
  SPLT_OPT_DEBUG_MODE,
  /**
   * The type of the split.
   *
   * Int option that can take the values from #splt_split_mode_options.
   *
   * Default is #SPLT_OPTION_NORMAL_MODE
   */
  SPLT_OPT_SPLIT_MODE,
  /**
   * What tags to put in the output split files.
   *
   * Int option that can take the values from #splt_tags_options.
   *
   * Default is #SPLT_CURRENT_TAGS
   */
  SPLT_OPT_TAGS,
  /**
   * For mp3 files, if we write the Xing header.
   *
   * Int option that can take the values #SPLT_TRUE or #SPLT_FALSE.
   *
   * Default is #SPLT_TRUE
   */
  SPLT_OPT_XING,
  /**
   * If this option is #SPLT_TRUE, we create directories from the output
   * filenames without parsing for illegal characters the output filenames.
   *
   * Otherwise, we parse for illegal characters the filenames and replace
   * them with '_'.
   *
   * Int option that can take the values #SPLT_TRUE or #SPLT_FALSE.
   *
   * Default is #SPLT_FALSE
   */
  SPLT_OPT_CREATE_DIRS_FROM_FILENAMES,
  /**
   * Defines how the output filenames are created.
   *
   * Int option that can take values from #splt_output_filenames_options.
   *
   * Default is #SPLT_OUTPUT_DEFAULT
   */
  SPLT_OPT_OUTPUT_FILENAMES,
  /**
   * For mp3 files, force to use the frame mode or not.
   * The frame mode processes the file frame by frame and
   * it is useful when splitting VBR (Variable Bit Rate) files.
   * Frame mode provides higher precision but is slower.
   * 
   * Int option that can take the values #SPLT_TRUE or #SPLT_FALSE.
   *
   * Default is #SPLT_FALSE
   */
  SPLT_OPT_FRAME_MODE,
  /**
   * If we use silence detection to auto-adjust splitpoints
   *
   * Int option that can take the values #SPLT_TRUE or #SPLT_FALSE.
   *
   * Default is #SPLT_FALSE
   */
  SPLT_OPT_AUTO_ADJUST,
  /**
   * If the input is not seekable.
   * This allows splitting mp3 streams which can be read only one time 
   * and can't be seeked.
   *
   * Int option that can take the values #SPLT_TRUE or #SPLT_FALSE.
   *
   * Default is #SPLT_FALSE
   */
  SPLT_OPT_INPUT_NOT_SEEKABLE,
  /**
   * The desired number of tracks when having a #SPLT_OPTION_SILENCE_MODE split
   *
   * Int option that can take positive integer values.
   * 0 means that we split as many files we find.
   *
   * Default is #SPLT_DEFAULT_PARAM_TRACKS
   */
  SPLT_OPT_PARAM_NUMBER_TRACKS,
  /**
   * The desired number of shots when having a
   * #SPLT_OPTION_SILENCE_MODE or #SPLT_OPTION_TRIM_SILENCE_MODE split
   * or using the #SPLT_OPT_AUTO_ADJUST option
   *
   * Int option that can take positive integer values.
   * It defines the number of shots to be found after the silence.
   * Decrease this value if you need to split files having closer silence points.
   *
   * Default is #SPLT_DEFAULT_PARAM_SHOTS
   */
  SPLT_OPT_PARAM_SHOTS,
  /**
   * Used to remove silence between the split tracks when having a #SPLT_OPTION_SILENCE_MODE split.
   *
   * Int option that can take the values #SPLT_TRUE or #SPLT_FALSE.
   *
   * Default is #SPLT_FALSE
   */
  SPLT_OPT_PARAM_REMOVE_SILENCE,
  /**
   * The time to auto-adjust before and after splitpoint
   * when having the #SPLT_OPT_AUTO_ADJUST option.
   *
   * Int option that can take positive integer values.
   *
   * Default is #SPLT_DEFAULT_PARAM_GAP
   */
  SPLT_OPT_PARAM_GAP,
  /**
   * Enables or disables writing the log file containing splitpoints
   * found with silence detection.
   *
   * Int option that can take the values #SPLT_TRUE or #SPLT_FALSE.
   *
   * Default is #SPLT_FALSE
   */
  SPLT_OPT_ENABLE_SILENCE_LOG,
  /**
   * For mp3 files, tags version to be written in output files.
   *
   * Int option that can take the values 0, 1, 2 or 12.
   * 0 is for 'same tags versions as the input file', 
   * 1 is for ID3v1, 2 for ID3v2 and 12 for both ID3v1 and ID3v2.
   *
   * Default is 0
   */
  SPLT_OPT_FORCE_TAGS_VERSION,
  /**
   * Number of files to be created when splitting by equal time length
   * with #SPLT_OPTION_LENGTH_MODE.
   *
   * Int option that can take positive values.
   *
   * Default is 1
   */
  SPLT_OPT_LENGTH_SPLIT_FILE_NUMBER,
  /**
   * Enables or disables replacing tags in tags.
   * If set to #SPLT_TRUE with an album 'myalbum_@t' and a title 'mysong'
   * will result in album 'myalbum_mysong'.
   *
   * Int option that can take the values #SPLT_TRUE or #SPLT_FALSE.
   *
   * Default is #SPLT_FALSE
   */
  SPLT_OPT_REPLACE_TAGS_IN_TAGS,
  /** 
   * Time to be added to each end splitpoint in order to overlap
   * the resulting files (in hundreths of seconds).
   *
   * Long option that can take positive values.
   *
   * Default is 0
   */
  SPLT_OPT_OVERLAP_TIME,
  /**
   * Time of the output files for the #SPLT_OPTION_TIME_MODE split (in
   * hundreths of seconds).
   *
   * Long option that can take positive values.
   *
   * Default is 6000 (one minute)
   */
  SPLT_OPT_SPLIT_TIME,
  /**
   * The threshold level (dB) to be considered as silence.
   * 
   * Threshold is used when having a #SPLT_OPTION_SILENCE_MODE, #SPLT_OPTION_TRIM_SILENCE_MODE
   * or #SPLT_OPT_AUTO_ADJUST option.
   *
   * Float option that can take values between -96 and 0.
   *
   * Default is #SPLT_DEFAULT_PARAM_THRESHOLD
   */
  SPLT_OPT_PARAM_THRESHOLD,
  /**
   * Offset of cutpoint inside the silence part when having a
   * #SPLT_OPTION_SILENCE_MODE or #SPLT_OPT_AUTO_ADJUST option
   *
   * Float option that can take values between -2 and 2.
   * 0 is the begin of silence and 1 the end.
   *
   * Default is #SPLT_DEFAULT_PARAM_OFFSET
   */
  SPLT_OPT_PARAM_OFFSET,
  /**
   * Minimum number of silence seconds to be considered a valid splitpoint.
   * All silences shorter than this value are discarded.
   *
   * Float option that can take positive float values.
   *
   * Default is #SPLT_DEFAULT_PARAM_MINIMUM_LENGTH
   */
  SPLT_OPT_PARAM_MIN_LENGTH,
  /**
   * Minimum number of seconds to be considered a valid track.
   * All tracks shorter than this value are discarded.
   *
   * Float option that can take positive float values.
   *
   * Default is #SPLT_DEFAULT_PARAM_MINIMUM_TRACK_LENGTH
   */
  SPLT_OPT_PARAM_MIN_TRACK_LENGTH,
  /**
   * Minimum number of seconds to be considered a valid track.
   * All tracks shorter than this value are joined with others.
   * The difference between #SPLT_OPT_PARAM_MIN_TRACK_LENGTH is that using this option,
   * no part from the original file is lost.
   *
   * Float option that can take positive float values.
   *
   * Default is #SPLT_DEFAULT_PARAM_MIN_TRACK_JOIN
   */
  SPLT_OPT_PARAM_MIN_TRACK_JOIN,
  /**
   * Output format of the artist extracted from filename when
   * using #SPLT_TAGS_FROM_FILENAME_REGEX.
   *
   * Int option that can take values as #splt_str_format.
   * 
   * Default is #SPLT_NO_CONVERSION
   */
  SPLT_OPT_ARTIST_TAG_FORMAT,
  /**
   * Output format of the album extracted from filename when
   * using #SPLT_TAGS_FROM_FILENAME_REGEX.
   *
   * Int option that can take values as #splt_str_format.
   * 
   * Default is #SPLT_NO_CONVERSION
   */
  SPLT_OPT_ALBUM_TAG_FORMAT,
  /**
   * Output format of the title extracted from filename when
   * using #SPLT_TAGS_FROM_FILENAME_REGEX.
   *
   * Int option that can take values as #splt_str_format.
   * 
   * Default is #SPLT_NO_CONVERSION
   */
  SPLT_OPT_TITLE_TAG_FORMAT,
  /**
   * Output format of the comment extracted from filename when
   * using #SPLT_TAGS_FROM_FILENAME_REGEX.
   *
   * Int option that can take values as #splt_str_format.
   * 
   * Default is #SPLT_NO_CONVERSION
   */
  SPLT_OPT_COMMENT_TAG_FORMAT,
  /**
   * Replace underscores with space when setting tags from filename
   * regex with #SPLT_TAGS_FROM_FILENAME_REGEX.
   *
   * Int option that can take the values #SPLT_TRUE or #SPLT_FALSE.
   *
   * Default is #SPLT_FALSE
   */
  SPLT_OPT_REPLACE_UNDERSCORES_TAG_FORMAT,
  /**
   * When importing CUE files, sets the filename to split as the FILE
   * CUE tag value if specified.
   *
   * Int option that can take the values #SPLT_TRUE or #SPLT_FALSE.
   *
   * Default is #SPLT_FALSE
   */
  SPLT_OPT_SET_FILE_FROM_CUE_IF_FILE_TAG_FOUND,
  /**
   * When using the #SPLT_OPT_PARAM_REMOVE_SILENCE, this option allows you to keep some
   * number of seconds of silence from the beginning of the silence segment.
   *
   * Default is #SPLT_DEFAULT_KEEP_SILENCE_LEFT
   */
  SPLT_OPT_KEEP_SILENCE_LEFT,
  /**
   * When using the #SPLT_OPT_PARAM_REMOVE_SILENCE, this option allows you to keep some
   * number of seconds of silence from the end of the silence segment.
   *
   * Default is #SPLT_DEFAULT_KEEP_SILENCE_RIGHT
   */
  SPLT_OPT_KEEP_SILENCE_RIGHT,
} splt_options;

/**
 * @brief Split mode. Possible values for the #SPLT_OPT_SPLIT_MODE option.
 */
typedef enum {
  /**
   * Normal split - using splitpoints provided with #mp3splt_append_splitpoint.
   */
  SPLT_OPTION_NORMAL_MODE,
  /**
   * //TODO
   * Split the file created with mp3wrap or albumwrap.
   */
  SPLT_OPTION_WRAP_MODE,
  /**
   * Split with silence detection
   */
  SPLT_OPTION_SILENCE_MODE,
  /**
   * Split with trim silence detection
   */
  SPLT_OPTION_TRIM_SILENCE_MODE,
  /**
   * Split with error mode 
   * It is useful to split large file derivated from a concatenation of
   * smaller files
   */
  SPLT_OPTION_ERROR_MODE,
  /**
   * Will create an indefinite number of smaller files with
   * a fixed time length specified by #SPLT_OPT_SPLIT_TIME
   */
  SPLT_OPTION_TIME_MODE,
  /**
   * Split in X pieces of equal time length.
   * X is defined by the #SPLT_OPT_LENGTH_SPLIT_FILE_NUMBER option
   */
  SPLT_OPTION_LENGTH_MODE,
} splt_split_mode_options;

/**
 * @brief Values for the #SPLT_OPT_OUTPUT_FILENAMES option
 *
 * Values for the #SPLT_OPT_OUTPUT_FILENAMES option
 */
typedef enum {
  //!output specified by the set_oformat
  SPLT_OUTPUT_FORMAT,
  //!the default output. depends of the type of the split.
  SPLT_OUTPUT_DEFAULT,
  /*!we don't change anything
    
    Of course we use our internal logic to generate at 
    least unique names in this case
  */
  SPLT_OUTPUT_CUSTOM
} splt_output_filenames_options;

/**
 * @brief Default value for the #SPLT_OPT_PARAM_THRESHOLD option
 */
#define SPLT_DEFAULT_PARAM_THRESHOLD -48.0
/**
 * @brief Default value for the #SPLT_OPT_PARAM_OFFSET option
 */
#define SPLT_DEFAULT_PARAM_OFFSET 0.8
/**
 * @brief Default value for the #SPLT_OPT_PARAM_MIN_LENGTH option
 */
#define SPLT_DEFAULT_PARAM_MINIMUM_LENGTH 0.0
/**
 * @brief Default value for the #SPLT_OPT_PARAM_MIN_TRACK_LENGTH option
 */
#define SPLT_DEFAULT_PARAM_MINIMUM_TRACK_LENGTH 0.0
/**
 * @brief Default value for the #SPLT_OPT_PARAM_MIN_TRACK_JOIN option
 */
#define SPLT_DEFAULT_PARAM_MIN_TRACK_JOIN 0.0
/**
 * @brief Default value for the #SPLT_OPT_PARAM_GAP option
 */
#define SPLT_DEFAULT_PARAM_GAP 30
/**
 * @brief Default value for the #SPLT_OPT_PARAM_NUMBER_TRACKS option
 */
#define SPLT_DEFAULT_PARAM_TRACKS 0
/**
 * @brief Default value for the #SPLT_OPT_PARAM_SHOTS option
 */
#define SPLT_DEFAULT_PARAM_SHOTS 25
/**
* @brief Default value for the #SPLT_OPT_KEEP_SILENCE_LEFT option
*/
#define SPLT_DEFAULT_KEEP_SILENCE_LEFT 0
/**
 * @brief Default value for the #SPLT_OPT_KEEP_SILENCE_RIGHT option
 */
#define SPLT_DEFAULT_KEEP_SILENCE_RIGHT 0

/**
 * @brief Values for the #SPLT_OPT_TAGS option
 *
 * Values for the #SPLT_OPT_TAGS option
 */
typedef enum {
  /**
   * Keep the tags of the original file
   */
  SPLT_TAGS_ORIGINAL_FILE,
  /**
   * Keep the tags issued from cddb, cue or
   * set by the user with #mp3splt_append_tags
   */
  SPLT_CURRENT_TAGS,
  /**
   * Does not put any tags
   */
  SPLT_NO_TAGS,
  /**
   * Sets tags from filename regex.
   * See #mp3splt_set_input_filename_regex.
   */
  SPLT_TAGS_FROM_FILENAME_REGEX,
} splt_tags_options;

#define SPLT_DEFAULT_OUTPUT "@f_@mm_@ss_@hh__@Mm_@Ss_@Hh"

/**
 * @brief Default output for the cddb and cue.
 * See #mp3splt_set_oformat
 */
#define SPLT_DEFAULT_CDDB_CUE_OUTPUT "@A - @n - @t"
/**
 * @brief Default output for the syncerror.
 * See #mp3splt_set_oformat
 */
#define SPLT_DEFAULT_SYNCERROR_OUTPUT "@f_error_@n"
/**
 * @brief Default output for the silence split.
 * See #mp3splt_set_oformat
 */
#define SPLT_DEFAULT_SILENCE_OUTPUT "@f_silence_@n"
/**
 * @brief Default output for the trim silence split.
 * See #mp3splt_set_oformat
 */
#define SPLT_DEFAULT_TRIM_SILENCE_OUTPUT "@f_trimmed"

typedef enum {
  SPLT_NO_CONVERSION,
  SPLT_TO_LOWERCASE,
  SPLT_TO_UPPERCASE,
  SPLT_TO_FIRST_UPPERCASE,
  SPLT_TO_WORD_FIRST_UPPERCASE
} splt_str_format;

//@}

/**
 * Freedb constants
 */

/*
 * tracktype.org search type
 */
#define SPLT_FREEDB_SEARCH_TYPE_CDDB_CGI 1
/*
 * freedb search type: not implemented yet
 */
#define SPLT_FREEDB_SEARCH_TYPE_CDDB 2
/*
 * get file from tracktype.org
 */
#define SPLT_FREEDB_GET_FILE_TYPE_CDDB_CGI 3
/*
 * get file from freedb
 */
#define SPLT_FREEDB_GET_FILE_TYPE_CDDB 4
/**
 * default tracktype.org port
 */
#define SPLT_FREEDB_CDDB_CGI_PORT 80
/**
 * default freedb port
 */
#define SPLT_FREEDB_CDDB_PORT 8880
/**
 * urls of tracktype.org and freedb.org
 */
#define SPLT_FREEDB_CGI_SITE "freedb.org/~cddb/cddb.cgi"
#define SPLT_FREEDB2_CGI_SITE "tracktype.org/~cddb/cddb.cgi"

/* other useful variables */

#define MP3SPLT_LIB_GETTEXT_DOMAIN "libmp3splt"

//backslash character
#ifndef SPLT_DIRCHAR
#ifdef __WIN32__
#define SPLT_DIRCHAR '\\'
#define SPLT_DIRSTR "\\"
#define SPLT_NDIRCHAR '/'
#else
#define SPLT_DIRCHAR '/'
#define SPLT_DIRSTR "/"
#define SPLT_NDIRCHAR '\\'
#endif
#endif

/**
 * Initialisation and free
 */

//!structure for the splt state
typedef struct _splt_state splt_state;

/**
 * @brief Creates a new state structure
 *
 * Creates a new state structure, needed by libmp3splt
 */
splt_state *mp3splt_new_state(int *error);

//find plugins
int mp3splt_find_plugins(splt_state *state);

//this function frees the left variables in the library
//don't forget to call this function ONLY at the end of the program
//returns possible error
void mp3splt_free_state(splt_state *state, int *error);

/************************************/
/** \defgroup splt_filepaths Set filenames and paths

@{
*/

/** Sets the path for the creation of new split files

@param state The central struct libmp3splt keeps all its data in
@param path The new output path
@return The error code
 */
int mp3splt_set_path_of_split(splt_state *state, const char *path);

/** Sets path to the input file

@param state The central struct libmp3splt keeps all its data in
@param path The new input file name
@return The error code
 */
int mp3splt_set_filename_to_split(splt_state *state, const char *filename);

/** Reads out the path to the input file

Is very practical after importing a cue file: The cue file provides an
input filename but does not tell us about this.

@param state The central struct libmp3splt keeps all its data in
@param path The new input file name
@return The error code
 */
char *mp3splt_get_filename_to_split(splt_state *state);

int mp3splt_set_m3u_filename(splt_state *state, const char *filename);
int mp3splt_set_silence_log_filename(splt_state *state, const char *filename);

/**
 * Defines the regex that will be used for #SPLT_TAGS_FROM_FILENAME_REGEX
 * to get out tags.
 *
 * Example: (?<artist>.*?) _ (?<album>.*?) will extract
 * 'one artist' and 'one album' from 'one artist _ one album',
 */
int mp3splt_set_input_filename_regex(splt_state *state, const char *regex);

int mp3splt_set_default_comment_tag(splt_state *state, const char *default_comment_tag);

int mp3splt_set_default_genre_tag(splt_state *state, const char *default_genre_tag);

//@}

/************************************/
/*! @defgroup splt_callback_ The registering of callback functions

@{
 */

//!used with the 'put_message' function
typedef enum {
  /**
   * Info message
   */
  SPLT_MESSAGE_INFO,
  /**
   * Debug message
   */
  SPLT_MESSAGE_DEBUG
} splt_message_type;

/** Register the function that is to be called if we got a text message

@param state The central struct libmp3splt keeps all its data in
@param message_cb The callback function
@return The error code
 */
int mp3splt_set_message_function(splt_state *state, 
    void (*message_cb)(const char *, splt_message_type, void *), void *cb_data);

/** Register the function that is to be called on creation of a output file

@param state The central struct libmp3splt keeps all its data in
@param file_cb The callback function
@return The error code
 */
int mp3splt_set_split_filename_function(splt_state *state,
    void (*file_cb)(const char *, int, void *), void *data);

//progress messages
/**
 * @brief Progress messages sent from the library to the client
 *
 * Progress messages sent from the library to the client
 */
typedef enum {
  /**
   * Preparing to split a song
   */
  SPLT_PROGRESS_PREPARE,
  /**
   * Creating the split file
   */
  SPLT_PROGRESS_CREATE,
  /**
   * Searching for syncerrors
   */
  SPLT_PROGRESS_SEARCH_SYNC,
  /**
   * Scanning for silence
   */
  SPLT_PROGRESS_SCAN_SILENCE
} splt_progress_messages;

typedef struct splt_progres splt_progress;

/** Register the progress bar callback

@param state The central struct libmp3splt keeps all its data in
@param progress_cb The callback function
@return The error code
 */
int mp3splt_set_progress_function(splt_state *state,
    void (*progress_cb)(splt_progress *p_bar, void *), void *cb_data);

//splt_progress access functions
int mp3splt_progress_get_type(const splt_progress *p_bar);
char *mp3splt_progress_get_filename_shorted(const splt_progress *p_bar);
int mp3splt_progress_get_current_split(const splt_progress *p_bar);
int mp3splt_progress_get_max_splits(const splt_progress *p_bar);
int mp3splt_progress_get_silence_found_tracks(const splt_progress *p_bar);
float mp3splt_progress_get_silence_db_level(const splt_progress *p_bar);
float mp3splt_progress_get_percent_progress(const splt_progress *p_bar);
void mp3splt_progress_set_int_user_data(splt_progress *p_bar, int user_data);
int mp3splt_progress_get_int_user_data(const splt_progress *p_bar);

int mp3splt_set_silence_level_function(splt_state *state,
  void (*get_silence_cb)(long time, float level, void *user_data),
  void *user_data);

//!@}

/************************************/
/*! @defgroup splt_splitpoints_ Splitpoint manipulation

@{
 */

/**
 * we define a 'skippoint' as a splitpoint that is not taken into
 * consideration
 */
typedef enum {
  /* a regular splitpoint */
  SPLT_SPLITPOINT,
  /* a skippoint */
  SPLT_SKIPPOINT,
} splt_type_of_splitpoint;

/** Sets a new splitpoint
@param state The central struct libmp3splt keeps all its data in
@param type @see splt_type_of_splitpoint
@param split_value The position of the split point in 1/100s
@param name The name of the split point
@return The error code of any error that might have occoured
*/
int mp3splt_append_splitpoint(splt_state *state,
    long split_value, const char *name, int type);

/**
 * @brief Definition of a splitpoint
 *
 * @see mp3splt_append_splitpoint
 * @see mp3splt_get_splitpoints
 */
typedef struct _splt_point splt_point;

/** returns a pointer to a list of all splitpoints

@param state The central struct libmp3splt keeps all its data in
@param error Is filled with the error code of any error that might
             have occoured
@param splitpoints_number The number of split points in the list
@return A pointer to the split point list
 */
const splt_point *mp3splt_get_splitpoints(splt_state *state, int *splitpoints_number, int *error);

//splt_point access functions
long mp3splt_points_get_value(const splt_point *points, int index);
int mp3splt_points_get_type(const splt_point *points, int index);
char *mp3splt_points_get_name(const splt_point *points, int index);

/** erase all splitpoints
@param state The central struct libmp3splt keeps all its data in
@param error Is filled with the error code of any error that might
             have occoured
 */
void mp3splt_erase_all_splitpoints(splt_state *state, int *error);

//@}

/************************************/
/* Tags                             */

#define SPLT_UNDEFINED_GENRE "Other"

#define SPLT_ID3V1_NUMBER_OF_GENRES 127

/*! ID3v1 genres
*/
static const char splt_id3v1_genres[SPLT_ID3V1_NUMBER_OF_GENRES][25] = {
  {"Blues"},
  {"Classic Rock"}, {"Country"}, {"Dance"}, 
  {"Disco"},{"Funk"},{"Grunge"},{"Hip-Hop"},{"Jazz"},
  {"Metal"},{"New Age"},{"Oldies"}, {"Other"}, {"Pop"},
  {"R&B"}, {"Rap"}, {"Reggae"}, {"Rock"}, {"Techno"},
  {"Industrial"}, {"Alternative"}, {"Ska"}, {"Death metal"},
  {"Pranks"}, {"Soundtrack"}, {"Euro-Techno"},
  {"Ambient"}, {"Trip-hop"}, {"Vocal"}, {"Jazz+Funk"},
  {"Fusion"}, {"Trance"}, {"Classical"}, {"Instrumental"},
  {"Acid"}, {"House"}, {"Game"}, {"Sound clip"}, {"Gospel"},
  {"Noise"}, {"Alt. Rock"}, {"Bass"}, {"Soul"}, {"Punk"}, 
  {"Space"}, {"Meditative"}, {"Instrumental pop"}, 
  {"Instrumental rock"}, {"Ethnic"}, {"Gothic"},{"Darkwave"},
  {"Techno-Industrial"},{"Electronic"},{"Pop-Folk"},{"Eurodance"},
  {"Dream"},{"Southern Rock"},{"Comedy"}, {"Cult"},{"Gangsta"},
  {"Top 40"},{"Christian Rap"},{"Pop/Funk"}, {"Jungle"},
  {"Native American"},{"Cabaret"},{"New Wave"}, {"Psychedelic"},
  {"Rave"},{"Showtunes"},{"Trailer"}, {"Lo-Fi"},{"Tribal"},
  {"Acid Punk"},{"Acid Jazz"}, {"Polka"}, {"Retro"},
  {"Musical"},{"Rock & Roll"},{"Hard Rock"},

  {"Folk"}, {"Folk-Rock"}, {"National Folk"}, {"Swing"},
  {"Fast Fusion"}, {"Bebob"}, {"Latin"}, {"Revival"},
  {"Celtic"}, {"Bluegrass"}, {"Avantgarde"}, {"Gothic Rock"},
  {"Progressive Rock"}, {"Psychedelic Rock"}, {"Symphonic Rock"},
  {"Slow Rock"}, {"Big Band"}, {"Chorus"}, {"Easy Listening"},
  {"Acoustic"}, {"Humour"}, {"Speech"}, {"Chanson"}, {"Opera"},
  {"Chamber Music"}, {"Sonata"}, {"Symphony"}, {"Booty Bass"},
  {"Primus"}, {"Porn Groove"}, {"Satire"}, {"Slow Jam"},
  {"Club"}, {"Tango"}, {"Samba"}, {"Folklore"}, {"Ballad"},
  {"Power Ballad"}, {"Rhythmic Soul"}, {"Freestyle"}, {"Duet"},
  {"Punk Rock"}, {"Drum Solo"}, {"A capella"}, {"Euro-House"},
  {"Dance Hall"},

  {"misc"},
};


/**
 * @brief The tags of a splitpoint
 *
 * The structure contains the tags that we can set to a filename
 * generated from a splitpoint. Tags may also define the output filenames.
 *
 * @see mp3splt_append_tags
 * @see mp3splt_get_tags
 */
typedef struct _splt_tags splt_tags;

//puts a tag
int mp3splt_append_tags(splt_state *state, 
    const char *title, const char *artist,
    const char *album, const char *performer,
    const char *year, const char *comment,
    int track, const char *genre);

//returns a pointer to all the current tags
const splt_tags *mp3splt_get_tags(splt_state *state,
    int *tags_number, int *error);

//splt_tags access functions
char *mp3splt_tags_get_artist(splt_tags *tags);
char *mp3splt_tags_get_album(splt_tags *tags);
char *mp3splt_tags_get_title(splt_tags *tags);
char *mp3splt_tags_get_genre(splt_tags *tags);
char *mp3splt_tags_get_comment(splt_tags *tags);
char *mp3splt_tags_get_year(splt_tags *tags);
int mp3splt_tags_get_track(splt_tags *tags);

//puts tags from a string
int mp3splt_put_tags_from_string(splt_state *state, const char *tags, int *error);

void mp3splt_erase_all_tags(splt_state *state, int *error);

/************************************/
/* Options                          */

int mp3splt_set_int_option(splt_state *state, int option_name, int value);
int mp3splt_set_long_option(splt_state *state, int option_name, long value);
int mp3splt_set_float_option(splt_state *state, int option_name, float value);

int mp3splt_get_int_option(splt_state *state, int option_name, int *error);
long mp3splt_get_long_option(splt_state *state, int option_name, int *error);
float mp3splt_get_float_option(splt_state *state, int option_name, int *error);

/************************************/
/* Split functions                  */

//split a ogg or mp3 file
//returns possible error
int mp3splt_split(splt_state *state);

//cancel split function
//returns possible error
void mp3splt_stop_split(splt_state *state, int *error);

/************************************/
/*    Cddb and Cue functions        */

//get the cue splitpoints from a file and puts them in the state
void mp3splt_put_cue_splitpoints_from_file(splt_state *state,
    const char *cue_file, int *error);

//read cddb splitpoints from file and puts them in the state
void mp3splt_put_cddb_splitpoints_from_file(splt_state *state,
    const char *cddb_file, int *error);

void mp3splt_put_audacity_labels_splitpoints_from_file(splt_state *state,
    const char *file, int *error);

/************************************/
/*    Freedb functions              */

/**
 * @brief All the freedb search results
 *
 * @see mp3splt_get_freedb_search
 */
typedef struct _splt_freedb_results splt_freedb_results;

//returns the freedb results and possible error
/**
 * @brief test
 */
const splt_freedb_results *mp3splt_get_freedb_search(splt_state *state,
    const char *searched_string, int *error,
    int search_type, const char *search_server, int port);

//splt_freedb_results access functions
int mp3splt_freedb_get_total_number(const splt_freedb_results *results);
int mp3splt_freedb_get_id(const splt_freedb_results *results, int index);
char *mp3splt_freedb_get_name(const splt_freedb_results *results, int index);
int mp3splt_freedb_get_number_of_revisions(const splt_freedb_results *results, int index);

void mp3splt_write_freedb_file_result(splt_state *state,
    int disc_id, const char *cddb_file, int *error,
    int cddb_get_type, const char *cddb_get_server, int port);

void mp3splt_export_to_cue(splt_state *state, const char *out_file,
    short stop_at_total_time, int *error);

void mp3splt_set_oformat(splt_state *state,
    const char *format_string, int *error);

/************************************/
/* Other utilities                  */

//counts the number of tracks found with silence detection
int mp3splt_count_silence_points(splt_state *state, int *error);

int mp3splt_set_silence_points(splt_state *state, int *error);
void mp3splt_set_trim_silence_points(splt_state *state, int *error);

//returns the version of libmp3splt (20 chars max)
void mp3splt_get_version(char *version);

//result must be freed
char *mp3splt_get_strerror(splt_state *state, int error_code);

/**
 * @brief The wrapped filenames found inside a file
 *
 * @see mp3splt_get_wrap_files
 */
typedef struct _splt_wrap splt_wrap;

//returns the wrapped files found
const splt_wrap *mp3splt_get_wrap_files(splt_state *state, int *error);

//splt_wrap access functions
int mp3splt_wrap_get_total_number(const splt_wrap *wrap_files);
char *mp3splt_wrap_get_wrapped_file(const splt_wrap *wrap_files, int index);

int mp3splt_append_plugins_scan_dir(splt_state *state, char *dir);

#ifdef __WIN32__
char *mp3splt_win32_utf16_to_utf8(const wchar_t *source);
#endif

char **mp3splt_find_filenames(splt_state *state, const char *filename,
    int *num_of_files_found, int *error);

int mp3splt_u_check_if_directory(const char *fname);

void mp3splt_free_one_tag(splt_tags *tags);
splt_tags *mp3splt_parse_filename_regex(splt_state *state, int *error);

#define MP3SPLT_MP3SPLT_H

#endif

