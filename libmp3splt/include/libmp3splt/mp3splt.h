/**********************************************************
 *
 * libmp3splt -- library based on mp3splt v2.1c,
 *               for mp3/ogg splitting without decoding
 *
 * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
 * Copyright (c) 2005-2012 Alexandru Munteanu - io_fx@yahoo.fr
 *
 * And others ... see the AUTHORS file provided with libmp3splt source.
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
 * \file mp3splt.h
 * 
 * \brief libmp3splt API 
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

/** @defgroup splt_error_codes_ Confirmation and error codes
@{
 */

/**
 * @brief Confirmation and error codes.
 *
 * @see #mp3splt_get_strerror
 */
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

/** @defgroup splt_state_ Initialisation of the main state
//@{
 */

/**
 * @brief Main structure used in libmp3splt.
 * All members are private.
 *
 * @see #mp3splt_new_state, #mp3splt_free_state
 */
typedef struct _splt_state splt_state;

/**
 * @brief Creates a new #splt_state structure.
 *
 * \note #mp3splt_find_plugins must to be called after.
 *
 * @param[out] error Possible error; can be NULL.
 * @return A newly allocated #splt_state.
 *
 * @see #mp3splt_free_state
 */
splt_state *mp3splt_new_state(splt_code *error);

/**
 * @brief Free the memory of the \p state.
 *
 * @param[in] state Main state to be freed.
 * @return Possible error.
 *
 * @see #mp3splt_new_state
 */
splt_code mp3splt_free_state(splt_state *state);

/**
 * @brief Appends an additional directory where plugins are searched.
 *
 * @param[in] state Main state.
 * @param[in] directory Additional directory where plugins will be searched.
 * @return Possible error.
 *
 * @see #mp3splt_find_plugins
 */
splt_code mp3splt_append_plugins_scan_dir(splt_state *state, const char *directory);

/**
 * @brief Finds the plugins in the plugins directories.
 * This function must be called after the \p state initialisation.
 *
 * @param[in] state Main state.
 * @return Possible error.
 *
 * @see #mp3splt_append_plugins_scan_dir
 */
splt_code mp3splt_find_plugins(splt_state *state);

//@}

/** @addtogroup splt_error_codes_
 * @{
 */

/**
 * @brief Returns the error message of the \p error.
 *
 * Please note that you have to call this function right after receiving the \p error because the
 * returned error message can contain information that is replaced if other error occurs.
 *
 * @param[in] state Main state.
 * @param[in] error Error code to be checked.
 * @return Error message of the \p error. Result must be freed.
 */
char *mp3splt_get_strerror(splt_state *state, splt_code error);

//@}

/** @defgroup splt_options_ Options
@{
 */

/**
 * @brief Split options.
 *
 * Use #mp3splt_set_int_option, #mp3splt_set_long_option and #mp3splt_set_float_option to set those
 * options.
 *
 * Use #mp3splt_get_int_option, #mp3splt_get_long_option and #mp3splt_get_float_option to get those
 * options.
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
  /**
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
   * If set to #SPLT_TRUE with an album 'myalbum_\@t' and a title 'mysong'
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
   * number of seconds of silence from the beginning of the output file
   * (end of the silence segment).
   *
   * Float option that can take positive values.
   *
   * Default is #SPLT_DEFAULT_KEEP_SILENCE_LEFT
   */
  SPLT_OPT_KEEP_SILENCE_LEFT,
  /**
   * When using the #SPLT_OPT_PARAM_REMOVE_SILENCE, this option allows you to keep some
   * number of seconds of silence from the end of the output file
   * (beginning of the silence segment).
   *
   * Float option that can take positive values.
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
   * Normal split mode - using splitpoints provided with #mp3splt_append_splitpoint.
   */
  SPLT_OPTION_NORMAL_MODE,
  /**
   * Wrap split mode - split the files created with [mp3wrap](http://mp3wrap.sourceforge.net/) or albumwrap.
   * To just find out the wrapped files, see #mp3splt_get_wrap_files.
   */
  SPLT_OPTION_WRAP_MODE,
  /**
   * Silence detection split mode.
   * To just find out the silence detection splitpoints, see #mp3splt_set_silence_points.
   */
  SPLT_OPTION_SILENCE_MODE,
  /**
   * Trim silence detection split mode.
   * To just find out the trim silence detection splitpoints, see #mp3splt_set_trim_silence_points.
   */
  SPLT_OPTION_TRIM_SILENCE_MODE,
  /**
   * Synchronisation error split mode (mp3 only).
   * It is useful to split large file derivated from a concatenation of smaller files.
   */
  SPLT_OPTION_ERROR_MODE,
  /**
   * Create an indefinite number of smaller files with a fixed time length specified by
   * #SPLT_OPT_SPLIT_TIME.
   */
  SPLT_OPTION_TIME_MODE,
  /**
   * Split in #SPLT_OPT_LENGTH_SPLIT_FILE_NUMBER pieces of equal time length.
   */
  SPLT_OPTION_LENGTH_MODE,
} splt_split_mode_options;

/**
 * @brief Values for the #SPLT_OPT_OUTPUT_FILENAMES option
 */
typedef enum {
  /**
   * @brief Output filename format specified by #mp3splt_set_oformat.
   */
  SPLT_OUTPUT_FORMAT,
  /**
   * @brief The default output; depends of the type of the split.
   *
   * Some defaults are #SPLT_DEFAULT_OUTPUT, #SPLT_DEFAULT_CDDB_CUE_OUTPUT,
   * #SPLT_DEFAULT_SYNCERROR_OUTPUT, #SPLT_DEFAULT_SILENCE_OUTPUT and 
   * #SPLT_DEFAULT_TRIM_SILENCE_OUTPUT.
   */
  SPLT_OUTPUT_DEFAULT,
  /**
   * @brief The names of the splitpoints provided by #mp3splt_append_splitpoint
   * are used to generate the output filenames.
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
 */
typedef enum {
  /**
   * @brief Keep the tags of the original input file.
   */
  SPLT_TAGS_ORIGINAL_FILE,
  /**
   * @brief Keep the tags issued from CDDB, CUE, tracktype.org or set by the user with #mp3splt_append_tags.
   */
  SPLT_CURRENT_TAGS,
  /**
   * @brief Do not set any tags.
   */
  SPLT_NO_TAGS,
  /**
   * @brief Sets tags from filename using regular expression provided by #mp3splt_set_input_filename_regex.
   */
  SPLT_TAGS_FROM_FILENAME_REGEX,
} splt_tags_options;

/**
 * @brief Default output for #SPLT_OPTION_NORMAL_MODE when using #SPLT_OUTPUT_DEFAULT.
 * @see #mp3splt_set_oformat
 */
#define SPLT_DEFAULT_OUTPUT "@f_@mm_@ss_@hh__@Mm_@Ss_@Hh"

/**
 * @brief Output filename format for CDDB and CUE when using #SPLT_OUTPUT_DEFAULT.
 * @see #mp3splt_set_oformat, #SPLT_OPT_OUTPUT_FILENAMES
 */
#define SPLT_DEFAULT_CDDB_CUE_OUTPUT "@A - @n - @t"
/**
 * @brief Output filename format for #SPLT_OPTION_ERROR_MODE when using #SPLT_OUTPUT_DEFAULT.
 * @see #mp3splt_set_oformat, #SPLT_OPT_OUTPUT_FILENAMES
 */
#define SPLT_DEFAULT_SYNCERROR_OUTPUT "@f_error_@n"
/**
 * @brief Output filename format for #SPLT_OPTION_SILENCE_MODE when using #SPLT_OUTPUT_DEFAULT.
 * @see #mp3splt_set_oformat, #SPLT_OPT_OUTPUT_FILENAMES
 */
#define SPLT_DEFAULT_SILENCE_OUTPUT "@f_silence_@n"
/**
 * @brief Default output for the trim silence split.
 * @see #mp3splt_set_oformat, #SPLT_OPT_OUTPUT_FILENAMES
 */
#define SPLT_DEFAULT_TRIM_SILENCE_OUTPUT "@f_trimmed"

/**
 * @brief Values for #SPLT_OPT_ARTIST_TAG_FORMAT, #SPLT_OPT_ALBUM_TAG_FORMAT,
 * #SPLT_OPT_TITLE_TAG_FORMAT and #SPLT_OPT_COMMENT_TAG_FORMAT when using
 * #SPLT_TAGS_FROM_FILENAME_REGEX.
 */
typedef enum {
  /**
   * @brief Keep the input "as is" without further change.
   */
  SPLT_NO_CONVERSION,
  /**
   * @brief Convert all characters of the input to lowercase.
   */
  SPLT_TO_LOWERCASE,
  /**
   * @brief Convert all characters of the input to uppercase.
   */
  SPLT_TO_UPPERCASE,
  /**
   * @brief Convert just the first character to uppercase.
   */
  SPLT_TO_FIRST_UPPERCASE,
  /**
   * @brief Convert the first character of each word to uppercase.
   */
  SPLT_TO_WORD_FIRST_UPPERCASE
} splt_str_format;

/**
 * @brief Sets the value of an int option in the \p state.
 *
 * @param[in] state Main state.
 * @param[in] option Target option from #splt_options.
 * @param[in] value Value for the \p option.
 * @return Possible error.
 */
splt_code mp3splt_set_int_option(splt_state *state, splt_options option, int value);

/**
 * @brief Sets the value of a long option in the \p state.
 *
 * @param[in] state Main state.
 * @param[in] option Target option from #splt_options.
 * @param[in] value Value for the \p option.
 * @return Possible error.
 */
splt_code mp3splt_set_long_option(splt_state *state, splt_options option, long value);

/**
 * @brief Sets the value of a float option in the \p state.
 *
 * @param[in] state Main state.
 * @param[in] option Target option from #splt_options.
 * @param[in] value Value for the \p option.
 * @return Possible error.
 */
splt_code mp3splt_set_float_option(splt_state *state, splt_options option, float value);

/**
 * @brief Returns the value of an int option from the \p state.
 *
 * @param[in] state Main state.
 * @param[in] option Target option from #splt_options.
 * @param[out] error Possible error; can be NULL.
 * @return Option value
 */
int mp3splt_get_int_option(splt_state *state, splt_options option, splt_code *error);

/**
 * @brief Returns the value of a long option from the \p state.
 *
 * @param[in] state Main state.
 * @param[in] option Target option from #splt_options.
 * @param[out] error Possible error; can be NULL.
 * @return Option value
 */
long mp3splt_get_long_option(splt_state *state, splt_options option, splt_code *error);

/**
 * @brief Returns the value of a float option from the \p state.
 *
 * @param[in] state Main state.
 * @param[in] option Target option from #splt_options.
 * @param[out] error Possible error; can be NULL.
 * @return Option value
 */
float mp3splt_get_float_option(splt_state *state, splt_options option, splt_code *error);

/**
 * @brief Set the output format when using #SPLT_OUTPUT_FORMAT.
 *
 * @param[in] state Main state.
 * @param[in] format Format of the output files.
 * @return Possible error.
 */
splt_code mp3splt_set_oformat(splt_state *state, const char *format);

//@}

/** @defgroup splt_filepaths_ Input filename and paths
@{
 */

/**
 * @brief Sets the input filename to split.
 *
 * @param[in] state Main state.
 * @param[in] filename Input filename to be split.
 * @return Possible error.
 */
splt_code mp3splt_set_filename_to_split(splt_state *state, const char *filename);

/** 
 * @brief Sets the output directory where the split files will be created.
 *
 * @param[in] state Main state.
 * @param[in] path_of_split Output directory for the generated files.
 * @return Possible error.
 */
splt_code mp3splt_set_path_of_split(splt_state *state, const char *path_of_split);

/**
 * @brief Returns the filename to be split from the \p state.
 *
 * It is useful after importing a CUE file that provides a filename
 * with the FILE tag.
 *
 * @param[in] state Main state.
 * @return Possible error.
 *
 * @see #SPLT_OPT_SET_FILE_FROM_CUE_IF_FILE_TAG_FOUND
 */
const char *mp3splt_get_filename_to_split(splt_state *state);

/**
 * @brief Output filename for a M3U file that will be created in the output path.
 *
 * The M3U file will contain all the split files.
 * It will not be created if this function is not called.
 *
 * @param[in] state Main state.
 * @param[in] m3u_filename M3U filename.
 * @return Possible error.
 *
 * @see #mp3splt_set_path_of_split
 */
splt_code mp3splt_set_m3u_filename(splt_state *state, const char *m3u_filename);

/**
 * @brief Log filename for the #SPLT_OPTION_SILENCE_MODE split mode that will be created in the
 *        output path.
 *
 * The log filename is useful to find out the silence splitpoints with different parameters
 * without having to detect silence every time.
 * Note that if changing the #SPLT_OPT_PARAM_MIN_LENGTH or #SPLT_OPT_PARAM_THRESHOLD or the input
 * filename, the silence detection will still need be to be recomputed.
 *
 * By default, the filename is \p mp3splt.log.
 *
 * \note <i>Log file structure:</i>\n
 *   The first line contains the name of the split file.\n
 *   The second line contains the threshold and the minimum silence length.\n
 *   The next lines contain each one three columns:
 *     - the first column is the start position of the found silence (in seconds.fractions)\n
 *     - the second column is the end position of the found silence (in seconds.fractions)\n
 *     - the third column is the order of magnitude of the silence length; it is useful to
 *       find out most probable silence points
 *
 * @param[in] state Main state.
 * @param[in] filename Log filename when detecting splitpoints from silence.
 * @return Possible error.
 *
 * @see #mp3splt_set_path_of_split
 */
splt_code mp3splt_set_silence_log_filename(splt_state *state, const char *filename);

//@}

/** @defgroup splt_callback_ Registering callback functions
@{
 */

/**
 * @brief Type of the message sent to the client
 *
 * @see #mp3splt_set_message_function
 */
typedef enum {
  /**
   * @brief Info message
   */
  SPLT_MESSAGE_INFO,
  /**
   * @brief Debug message
   */
  SPLT_MESSAGE_DEBUG
} splt_message_type;

/** 
 * Register callback function used to send text messages to the client.
 *
 * @param[in] state Main state.
 * @param[in] message_cb Callback function to be called.
 * @param[in] cb_data User data sent through \p message_cb.
 * @return Possible error.
 *
 * Parameters of the callback \p message_cb function:
 *
 * \p message Text message received.\n
 * \p type Type of the text message received.\n
 * \p cb_data The user data passed to the #mp3splt_set_message_function.
 *
 * @see #splt_message_type
 */
splt_code mp3splt_set_message_function(splt_state *state, 
    void (*message_cb)(const char *message, splt_message_type type, void *cb_data), void *cb_data);

/** 
 * @brief Register callback function that is called when an output file is created.
 *
 * @param[in] state The central struct libmp3splt keeps all its data in
 * @param[in] file_cb Callback function to be called.
 * @param[in] cb_data User data sent through \p file_cb.
 * @return Possible error.
 *
 * Parameters of the callback \p file_cb function:
 *
 * \p filename Output filename that has been created.\n
 * \p cb_data The user data passed to the #mp3splt_set_split_filename_function.
 */
splt_code mp3splt_set_split_filename_function(splt_state *state,
    void (*file_cb)(const char *filename, void *cb_data),
    void *cb_data);

/**
 * @brief Type of messages sent to the client using the callback registered with
 * #mp3splt_set_progress_function.
 */
typedef enum {
  /**
   * @brief Preparing to split a song.
   */
  SPLT_PROGRESS_PREPARE,
  /**
   * @brief Creating the output file.
   */
  SPLT_PROGRESS_CREATE,
  /**
   * @brief Searching for synchronisation errors - when using #SPLT_OPTION_ERROR_MODE.
   */
  SPLT_PROGRESS_SEARCH_SYNC,
  /**
   * @brief Scanning for silence - when using #SPLT_OPTION_SILENCE_MODE,
   * #SPLT_OPT_AUTO_ADJUST, #mp3splt_set_silence_points or #mp3splt_set_trim_silence_points.
   */
  SPLT_PROGRESS_SCAN_SILENCE
} splt_progress_messages;

/**
 * @brief Structure containing all information needed to display a progress bar.
 * All members are private.
 * 
 * It is passed as parameter to the callback function registered with
 * #mp3splt_set_progress_function.
 *
 * @see #mp3splt_progress_get_type
 * @see #mp3splt_progress_get_filename_shorted
 * @see #mp3splt_progress_get_current_split
 * @see #mp3splt_progress_get_max_splits
 * @see #mp3splt_progress_get_silence_found_tracks
 * @see #mp3splt_progress_get_silence_db_level
 * @see #mp3splt_progress_get_percent_progress
 */
typedef struct splt_progres splt_progress;

/**
 * @brief Register callback function called to keep track of the current progress.
 * 
 * @param[in] state Main state.
 * @param[in] progress_cb Callback function to be called.
 * @param[in] cb_data User data sent through \p progress_cb.
 * @return Possible error.
 *
 * Parameters of the callback \p progress_cb function:
 *
 * \p p_bar Progress bar informations.\n
 * \p cb_data The user data passed to the #mp3splt_set_progress_function.
 */
splt_code mp3splt_set_progress_function(splt_state *state,
    void (*progress_cb)(splt_progress *p_bar, void *cb_data), void *cb_data);

/**
 * @return The type of #splt_progress_messages.
 */
int mp3splt_progress_get_type(const splt_progress *p_bar);

/**
 * @return The filename being processed. Result must be freed.
 */
char *mp3splt_progress_get_filename_shorted(const splt_progress *p_bar);

/**
 * @return The current split number.
 */
int mp3splt_progress_get_current_split(const splt_progress *p_bar);

/**
 * @return The total number of files to be created.
 */
int mp3splt_progress_get_max_splits(const splt_progress *p_bar);

/**
 * @return The number of silence spots found.
 */
int mp3splt_progress_get_silence_found_tracks(const splt_progress *p_bar);

/**
 * @return The audio threshold level.
 */
float mp3splt_progress_get_silence_db_level(const splt_progress *p_bar);

/**
 * @return The progress percentage between 0 and 1.
 */
float mp3splt_progress_get_percent_progress(const splt_progress *p_bar);

/**
 * @brief Register callback function that is called when looking for
 *        silence detection.
 *
 * @param[in] state Main state.
 * @param[in] get_silence_cb Callback function to be called.
 * @param[in] user_data User data sent through \p get_silence_cb.
 * @return Possible error.
 *
 * Parameters of the callback \p get_silence_cb function:
 *
 * \p time Current time in hundreths of seconds.\n
 * \p level Current silence level.\n
 * \p user_data The user data passed to the #mp3splt_set_silence_level_function.
 */
splt_code mp3splt_set_silence_level_function(splt_state *state,
  void (*get_silence_cb)(long time, float level, void *user_data),
  void *user_data);

//!@}

/** @defgroup splt_splitpoints_ Splitpoints handling
@{
 */

/**
 * @brief Type of the splitpoint.
 * @see #mp3splt_append_splitpoint, #mp3splt_point_get_type
 */
typedef enum {
  /**
   * @brief Regular splitpoint.
   */
  SPLT_SPLITPOINT,
  /**
   * @brief Splitpoint used only to end the previous segment.
   *        Segment starting with this splitpoint will not be split.
   */
  SPLT_SKIPPOINT,
} splt_type_of_splitpoint;

/**
 * @brief Structure defining one splitpoint.
 * All members are private.
 *
 * @see mp3splt_append_splitpoint
 * @see mp3splt_get_splitpoints
 * @see mp3splt_erase_all_splitpoints
 */
typedef struct _splt_point splt_point;

/**
 * @brief Creates a new splitpoint with the \p splitpoint_value.
 *
 * By default, the splitpoint has no name and it has the type #SPLT_SPLITPOINT.
 *
 * @param[in] splitpoint_value The time of the splitpoint in hundreths of seconds.
 * @param[out] error Possible error; can be NULL.
 * @return Newly allocated point.
 *
 * @see #mp3splt_point_set_name
 * @see #mp3splt_point_set_type
 * @see #mp3splt_append_splitpoint
 */
splt_point *mp3splt_point_new(long splitpoint_value, splt_code *error);

/**
 * @brief Sets the name on the \p splitpoint.
 *
 * @param[in] splitpoint Splitpoint to be changed.
 * @param[in] name Name of the splitpoint to be set. Useful when using #SPLT_OUTPUT_CUSTOM.
 * @return Possible error.
 */
splt_code mp3splt_point_set_name(splt_point *splitpoint, const char *name);

/**
 * @brief Sets the name on the \p splitpoint.
 *
 * @param[in] splitpoint Splitpoint to be changed.
 * @param[in] type Type of the splitpoint.
 * @return Possible error.
 */
splt_code mp3splt_point_set_type(splt_point *splitpoint, splt_type_of_splitpoint type);

/**
 * @brief Append a new splitpoint to the \p state.
 *
 * @param[in] state Main state.
 * @param[in] splitpoint Splitpoint to be appended; splitpoint is freed afterwards.
 * @return Possible error.
 *
 * @see #mp3splt_point_new
*/
splt_code mp3splt_append_splitpoint(splt_state *state, splt_point *splitpoint);

/**
 * @brief Structure containing several #splt_point.
 * All members are private.
 *
 * @see #mp3splt_points_init_iterator
 * @see #mp3splt_points_next
 */
typedef struct _splt_points splt_points;

/**
 * @brief Returns all the splitpoints of the \p state.
 *
 * @param[in] state Main state.
 * @param[out] error Possible error; can be NULL.
 * @return The splitpoints from the \p state.
 *
 * @see #mp3splt_points_init_iterator
 * @see #mp3splt_points_next
 */
splt_points *mp3splt_get_splitpoints(splt_state *state, splt_code *error);

/**
 * @brief Initialisation of the iterator for use with #mp3splt_points_next.
 *
 * @param[in] splitpoints Splitpoints returned with #mp3splt_get_splitpoints.
 *
 * @see #mp3splt_points_next
 */
void mp3splt_points_init_iterator(splt_points *splitpoints);

/**
 * @brief Returns the next splitpoint from the \p splitpoints.
 *
 * @param[in] splitpoints Splitpoints to be processed.
 * @return Next splitpoint of \p splitpoints or NULL if none found or no point remains.
 *
 * @see #mp3splt_point_get_value
 * @see #mp3splt_point_get_type
 * @see #mp3splt_point_get_name
 */
const splt_point *mp3splt_points_next(splt_points *splitpoints);

/**
 * @brief Returns the time value of the splitpoint \p point.
 *
 * @param[in] point Splitpoint to be queried.
 * @return The time value of the splitpoint.
 *
 * @see #mp3splt_get_splitpoints
 */
long mp3splt_point_get_value(const splt_point *point);

/**
 * @brief Returns the type of the splitpoint \p point.
 * Type can be #splt_type_of_splitpoint.
 *
 * @param[in] point Splitpoint to be queried.
 * @return The type of the requested splitpoint.
 *
 * @see #mp3splt_get_splitpoints
 */
splt_type_of_splitpoint mp3splt_point_get_type(const splt_point *point);

/**
 * @brief Returns the name of the splitpoint \p point.
 *
 * @param[in] point Splitpoint to be queried.
 * @return The name of the requested splitpoint. Result must be freed.
 *
 * @see #mp3splt_get_splitpoints
 */
char *mp3splt_point_get_name(const splt_point *point);

/**
 * @brief Erase all splitpoints from the \p state.
 *
 * @param[in] state Main state.
 * @return Possible error.
 */
splt_code mp3splt_erase_all_splitpoints(splt_state *state);

//@}

/** @defgroup splt_tags_ Tags handling
@{
 */

/**
 * @brief Undefined genre string.
 */
#define SPLT_UNDEFINED_GENRE "Other"

/**
 * @brief Number of ID3v1 genres.
 * @see #splt_id3v1_genres
 */
#define SPLT_ID3V1_NUMBER_OF_GENRES 127

/**
 * @brief ID3v1 genres.
 */
extern const char splt_id3v1_genres[SPLT_ID3V1_NUMBER_OF_GENRES][25];

/**
 * @brief Key tags useful with #mp3splt_append_tags.
 */
typedef enum {
  SPLT_TAGS_TITLE = 1,
  SPLT_TAGS_ARTIST = 2,
  SPLT_TAGS_ALBUM = 3,
  SPLT_TAGS_YEAR = 4,
  SPLT_TAGS_COMMENT = 5,
  SPLT_TAGS_TRACK = 6,
  SPLT_TAGS_GENRE = 7,
  SPLT_TAGS_PERFORMER = 8,
} splt_tag_key;

/**
 * @brief Structure containing the tags for one output file.
 * All members are private.
 *
 * The structure contains the tags that we can set to one generated file.
 * Tags may also define the output filenames.
 *
 * @see mp3splt_tags_set
 */
typedef struct _splt_tags splt_tags;

/**
 * @brief Creates a new tags structure.
 *
 * @param[in] error Possible error; can be NULL.
 * @return Newly allocated tags.
 *
 * @see #mp3splt_tags_set
 * @see #mp3splt_append_tags
 */
splt_tags *mp3splt_tags_new(splt_code *error);

/**
 * @brief Set tags values in the \p tags.
 *
 * The ... parameters are pairs of (key, value); arguments must end with 0, where key is a #splt_tag_key
 * and value is const char *.
 *
 * Example:
 * \code{.c}
 *   mp3splt_tags_set(tags, SPLT_TAGS_ARTIST, "my_artist", SPLT_TAGS_ALBUM, "my_album", 0);
 * \endcode
 *
 * @param[in] tags Tags to be changed.
 * @return Possible error.
 *
 * @see #mp3splt_append_tags
 */
splt_code mp3splt_tags_set(splt_tags *tags, ...);

/**
 * @brief Append the \p tags in the \p state.
 *
 * Tags must be appended in the same order as the splitpoints.
 *
 * First appended tags are mapped to the segment between the first two splitpoints.
 * Second appended tags are mapped to the second segment between the second and third splitpoints.
 *
 * @param[in] state Main state.
 * @param[in] tags Tags to be appended to the \p state.
 * @return Possible error.
 */
splt_code mp3splt_append_tags(splt_state *state, splt_tags *tags);

/**
 * @brief Structure containing a group of tags.
 * All members are private.
 */
typedef struct _splt_tags_group splt_tags_group;

/**
 * @brief Returns all the tags of the \p state.
 *
 * @param[in] state Main state.
 * @param[out] error Possible error; can be NULL.
 * @return The tags group of the \p state.
 *
 * @see #mp3splt_tags_group_init_iterator
 * @see #mp3splt_tags_group_next
 */
splt_tags_group *mp3splt_get_tags_group(splt_state *state, splt_code *error);

/**
 * @brief Initialisation of the iterator for use with #mp3splt_tags_group_next.
 *
 * @param[in] tags_group Group of tags returned with #mp3splt_get_tags_group.
 *
 * @see #mp3splt_tags_group_next
 */
void mp3splt_tags_group_init_iterator(splt_tags_group *tags_group);

/**
 * @brief Returns the next tags from the \p tags_group.
 *
 * @param[in] tags_group Tags group to be processed.
 * @return Next tags of \p tags_group or NULL if none found or no tags remains.
 *
 * @see #mp3splt_tags_get
 */
splt_tags *mp3splt_tags_group_next(splt_tags_group *tags_group);

/**
 * @brief Returns the value of \p key from the \p tags. Result must be freed.
 */
char *mp3splt_tags_get(splt_tags *tags, splt_tag_key key);

/**
 * @brief Fill the \p state with tags parsed from the \p tags string.
 *
 * \p tags should contain one or more square brackets pairs [].
 * The tags defined in the first pair of square brackets will be set on the first split file.
 * Those defined in the second pair of square brackets will be set on the second split file, ...
 * Inside a pair of square brackets, each tag is defined as \@variable=value and they tags are 
 * separated by commas. If a percent sign % is found before the open square bracket character, 
 * then the pair of square brackets following the % character will define the default tags in 
 * the following files. Multiple '%' can be defined.
 * An optional 'r' character can be placed at the start, to replace tags in tags.
 * The 'replace tags in tags' option is not recursive.
 *
 * Variables can be: \@a - artist, \@b - album, \@t - title, \@y - year, \@c - comment, \@g - genre, 
 * \@n - track number, \@o - set original tags, \@N - auto increment track number.
 * Variables for the start splitpoint: \@m - minutes, \@s - seconds, \@h - hundreths of seconds.
 * Variables for the end splitpoint: \@M - minutes, \@S - seconds, \@H - hundreths of seconds.
 *
 * Using the 'replace tags in tags' option, you can also use the following variables, which are 
 * replaced by the data from the original tags: \#a, \#b, \#t, \#y, \#c, \#g.
 * Note that this will only work if \@o has been found before. 
 *
 * @param[in] state Main state.
 * @param[in] tags String containing tags to be parsed and appended.
 * @param[out] error Possible error; can be NULL.
 * @return #SPLT_TRUE if the input tags are ambiguous.
 * Tags might be ambiguous if the input does not seem to be valid or if \@t or \@n is missing.
 */
int mp3splt_put_tags_from_string(splt_state *state, const char *tags, splt_code *error);

/**
 * @brief Erase all the tags from the \p state.
 *
 * @param[in] state Main state.
 * @return Possible error.
 */
splt_code mp3splt_erase_all_tags(splt_state *state);

/**
 * @brief Defines the regex that will be used for #SPLT_TAGS_FROM_FILENAME_REGEX.
 *
 * Tags will be extracted using the following variables:
 *   (?\<artist>), (?\<album>), (?\<title>), (?\<tracknum>), (?\<year>), (?\<comment>), (?\<genre>).
 *
 * Example: (?\<artist>.*?) _ (?\<album>.*?) will extract
 * 'one artist' and 'one album' from 'one artist _ one album'.
 *
 * @param[in] state Main state.
 * @param[in] regex Regular expression used to set the tags from the filename.
 * @return Possible error.
 */
splt_code mp3splt_set_input_filename_regex(splt_state *state, const char *regex);

/**
 * @brief Default comment tag when using #SPLT_TAGS_FROM_FILENAME_REGEX and no comment found.
 *
 * @param[in] state Main state.
 * @param[in] default_comment_tag Default comment if no comment has been found.
 * @return Possible error.
 *
 * @see #mp3splt_set_input_filename_regex
 */
splt_code mp3splt_set_default_comment_tag(splt_state *state, const char *default_comment_tag);

/**
 * @brief Default genre tag when using #SPLT_TAGS_FROM_FILENAME_REGEX and no genre found.
 *
 * @param[in] state Main state.
 * @param[in] default_genre_tag Default genre if no genre has been found.
 * @return Possible error.
 *
 * @see #mp3splt_set_input_filename_regex
 */
splt_code mp3splt_set_default_genre_tag(splt_state *state, const char *default_genre_tag);

/**
 * @brief Parse the filename provided with #mp3splt_set_filename_to_split using regex 
 * provided by #mp3splt_set_input_filename_regex and returns the parsed tags.
 *
 * @param[in] state Main state.
 * @param[out] error Possible error; can be NULL.
 * @return Parsed tags; must be freed with #mp3splt_free_one_tag.
 *
 * @see #mp3splt_set_filename_to_split
 * @see #mp3splt_set_input_filename_regex
 * @see #mp3splt_free_one_tag
 */
splt_tags *mp3splt_parse_filename_regex(splt_state *state, splt_code *error);

/**
 * @brief Free the memory of one #splt_tags
 *
 * @param[in] tags Pointer to a #splt_tags.
 *
 * @see #mp3splt_parse_filename_regex
 */
void mp3splt_free_one_tag(splt_tags *tags);

//@}

/** @defgroup splt_split_ Split functions
@{
 */

/**
 * @brief Executes the main split process.
 *
 * @param[in] state Main state.
 * @return Possible error.
 *
 * @see #mp3splt_stop_split
 * @see #mp3splt_new_state
 * @see #mp3splt_set_filename_to_split
 * @see #splt_options
 * @see #mp3splt_set_path_of_split
 */
splt_code mp3splt_split(splt_state *state);

/**
 * @brief Stop the main split process.
 *
 * @param[in] state Main state.
 * @return Possible error.
 *
 * @see #mp3splt_split
 */
splt_code mp3splt_stop_split(splt_state *state);

/**
 * @brief Recursive search of all the filenames matching the loaded plugins.
 *
 * @param[in] state Main state.
 * @param[in] filename Directory to be looked for recursive search.
 * @param[out] num_of_files_found Number of files found.
 * @param[out] error Possible error; can be NULL.
 * @return Newly allocated array of found files that must be freed.
 *
 * @see #mp3splt_set_filename_to_split and #mp3splt_split
 */
char **mp3splt_find_filenames(splt_state *state, const char *filename,
    int *num_of_files_found, splt_code *error);

//@}

/** @defgroup splt_import_ Import splitpoints
@{
 */

/**
 * @brief Type of the import.
 *
 * @see #mp3splt_import
 */
typedef enum {
  CUE_IMPORT,
  CDDB_IMPORT,
  AUDACITY_LABELS_IMPORT
} splt_import_type;

/**
 * @brief Import splitpoints from the \p file having the \p type into the \p state.
 *
 * @param[in] state Main state.
 * @param[in] type Type of the import.
 * @param[in] file File to import.
 * @return Possible error.
 *
 * @see #mp3splt_split
 */
splt_code mp3splt_import(splt_state *state, splt_import_type type, const char *file);

/**
 * @brief Search CDDB file using CDDB CGI protocol (tracktype.org).
 *
 * @see #mp3splt_get_freedb_search
 */
#define SPLT_FREEDB_SEARCH_TYPE_CDDB_CGI 1

/**
 * @brief Get CDDB file using CDDB CGI protocol (tracktype.org or freedb.org).
 *
 * @see #mp3splt_write_freedb_file_result
 */
#define SPLT_FREEDB_GET_FILE_TYPE_CDDB_CGI 3

/**
 * @brief Get CDDB file using freedb.org CDDB protocol.
 *
 * @see #mp3splt_write_freedb_file_result
 */
#define SPLT_FREEDB_GET_FILE_TYPE_CDDB 4

/**
 * @brief Default port.
 *
 * @see #mp3splt_get_freedb_search
 * @see #mp3splt_write_freedb_file_result
 */
#define SPLT_FREEDB_CDDB_CGI_PORT 80

/**
 * @brief URL of tracktype.org when using #SPLT_FREEDB_SEARCH_TYPE_CDDB_CGI and
 * #SPLT_FREEDB_GET_FILE_TYPE_CDDB_CGI types.
 * 
 * @see #mp3splt_get_freedb_search
 * @see #mp3splt_write_freedb_file_result
 */
#define SPLT_FREEDB2_CGI_SITE "tracktype.org/~cddb/cddb.cgi"

/**
 * @brief Structure containing the freedb search results.
 * All members are private.
 *
 * @see mp3splt_get_freedb_search
 * @see #mp3splt_freedb_init_iterator
 * @see #mp3splt_freedb_next
 */
typedef struct _splt_freedb_results splt_freedb_results;

/**
 * @brief Structure containing only one freedb result.
 *
 * @see #mp3splt_freedb_get_id
 * @see #mp3splt_freedb_get_name
 * @see #mp3splt_freedb_get_number_of_revisions
 */
typedef struct _splt_freedb_one_result splt_freedb_one_result;

/**
 * @brief Search on the internet for the \p searched_string and returns the results.
 *
 * @param[in] state Main state.
 * @param[in] searched_string Search string - might be artist or album.
 * @param[out] error Possible error; can be NULL.
 * @param[in] search_type Type of the search.
 *                        Only #SPLT_FREEDB_SEARCH_TYPE_CDDB_CGI is supported for the moment.
 * @param[in] search_server You can use #SPLT_FREEDB2_CGI_SITE as search server.
 * @param[in] port Port of the \p search_server. Can be #SPLT_FREEDB_CDDB_CGI_PORT.
 * @return The search results.
 *
 * @see #mp3splt_freedb_init_iterator
 * @see #mp3splt_freedb_next
 * @see #mp3splt_write_freedb_file_result
 */
splt_freedb_results *mp3splt_get_freedb_search(splt_state *state,
    const char *searched_string, splt_code *error,
    int search_type, const char *search_server, int port);

/**
 * @brief Initialisation of the iterator for use with #mp3splt_freedb_next
 *
 * @param[in] freedb_results Freedb results returned with #mp3splt_get_freedb_search.
 *
 * @see #mp3splt_freedb_next
 */
void mp3splt_freedb_init_iterator(splt_freedb_results *freedb_results);

/**
 * @brief Returns the next freedb result from the \p freedb_results.
 *
 * @param[in] freedb_results Freedb results to be processed.
 * @return Next freedb result of \p freedb_results or NULL if none found or no result remains.
 *
 * @see #mp3splt_freedb_get_id
 * @see #mp3splt_freedb_get_name
 * @see #mp3splt_freedb_get_number_of_revisions
 */
const splt_freedb_one_result *mp3splt_freedb_next(splt_freedb_results *freedb_results);

/**
 * @brief Returns the ID of the \p result.
 * The ID is needed when using #mp3splt_write_freedb_file_result.
 */
int mp3splt_freedb_get_id(const splt_freedb_one_result *result);

/**
 * @brief Returns the name of the \p result.
 */
char *mp3splt_freedb_get_name(const splt_freedb_one_result *result);

/**
 * @brief Returns the number of revisions of the \p result.
 */
int mp3splt_freedb_get_number_of_revisions(const splt_freedb_one_result *result);

/**
 * @brief Downloads the CDDB file of the \p disc_id and writes it to a file.
 *
 * @param[in] state Main state.
 * @param[in] disc_id ID of the chosen disc provided by #mp3splt_freedb_get_id.
 * @param[in] output_file Name of the output CDDB file that will be written.
 * @param[in] cddb_get_type Type of the download.
 *            Can be #SPLT_FREEDB_GET_FILE_TYPE_CDDB or #SPLT_FREEDB_GET_FILE_TYPE_CDDB_CGI.
 * @param[in] cddb_get_server Name of the server from the file is downloaded.
 *            Can be #SPLT_FREEDB2_CGI_SITE (or freedb.org or freedb.org/~cddb/cddb.cgi).
 * @param[in] port Port of the \p cddb_get_server.
 *                 Can be #SPLT_FREEDB_CDDB_CGI_PORT (or 8880) for example.
 * @return Possible error.
 *
 * @see #mp3splt_get_freedb_search
 */
splt_code mp3splt_write_freedb_file_result(splt_state *state,
    int disc_id, const char *output_file,
    int cddb_get_type, const char *cddb_get_server, int port);

//@}

/** @defgroup splt_export_ Export splitpoints
@{
 */

/**
 * @brief Type of the export.
 *
 * @see #mp3splt_export
 */
typedef enum {
  CUE_EXPORT
} splt_export_type;

/**
 * @brief Export splitpoints from the \p state into the \p file saved as \p type.
 *
 * @param[in] state Main state.
 * @param[in] type Export type.
 * @param[in] file File to be written with splitpoints from the \p state.
 * @param[in] stop_at_total_time If #SPLT_TRUE, don't export splitpoints after the total time
 *                               of the input file.
 * @return Possible error.
 */
splt_code mp3splt_export(splt_state *state, splt_export_type type, 
    const char *file, int stop_at_total_time);

//@}

/** @defgroup splt_wrap_ Wrap utilities
@{
 */

/**
 * @brief Structure containg the wrapped filenames found inside the input filename.
 * All members are private.
 *
 * @see #mp3splt_get_wrap_files
 * @see #mp3splt_wrap_init_iterator
 * @see #mp3splt_wrap_next
 */
typedef struct _splt_wrap splt_wrap;

/**
 * @brief Structure containing one wrapped file 
 * All members are private
 *
 * @see #mp3splt_wrap_get_wrapped_file
 */
typedef struct _splt_one_wrap splt_one_wrap;

/**
 * @brief Returns the wrapped files found from the input filename set with
 * #mp3splt_set_filename_to_split.
 *
 * @param[in] state Main state.
 * @param[out] error Possible error; can be NULL.
 * @return Wrapped files found.
 *
 * @see #mp3splt_wrap_init_iterator
 * @see #mp3splt_wrap_next
 */
splt_wrap *mp3splt_get_wrap_files(splt_state *state, splt_code *error);

/**
 * @brief Initialisation of the iterator for use with #mp3splt_wrap_next.
 *
 * @param[in] wrap Wrapped files returned with #mp3splt_get_wrap_files.
 *
 * @see #mp3splt_wrap_next
 */
void mp3splt_wrap_init_iterator(splt_wrap *wrap);

/**
 * @brief Returns the next wrapped file from the \p wrap.
 *
 * @param[in] wrap Wrapped files to be processed.
 * @return Next wrapped file of \p wrap or NULL if none found or no wrapped file remains.
 *
 * @see #mp3splt_wrap_get_wrapped_file
 */
const splt_one_wrap *mp3splt_wrap_next(splt_wrap *wrap);

/**
 * @brief Returns the wrapped file from \p one_wrap. Result must be freed.
 */
char *mp3splt_wrap_get_wrapped_file(const splt_one_wrap *one_wrap);

//@}

/** @defgroup splt_other_ Other utilities
@{
 */

/**
 * @brief Scan for silence and set silence splitpoints in the \p state.
 *
 * @param[in] state Main state.
 * @param[out] error Possible error; can be NULL.
 * @return The number of silence spots found.
 */
int mp3splt_set_silence_points(splt_state *state, splt_code *error);

/**
 * @brief Scan for silence and set silence trim splitpoints in the \p state.
 *
 * @param[in] state Main state.
 * @return Possible error.
 */
splt_code mp3splt_set_trim_silence_points(splt_state *state);

/**
 * @brief Returns the version of libmp3splt. Result must be freed.
 */
char *mp3splt_get_version();

#ifdef __WIN32__
/**
 * @brief Returns \p source converted from UTF-16 to UTF-8.
 */
char *mp3splt_win32_utf16_to_utf8(const wchar_t *source);
#endif

/**
 * @brief Returns #SPLT_TRUE if \p filename is a directory.
 */
int mp3splt_check_if_directory(const char *filename);

#ifndef SPLT_DIRCHAR
#ifdef __WIN32__
#define SPLT_DIRCHAR '\\'
#define SPLT_DIRSTR "\\"
#else
/**
 * @brief Path separator as character (/ or \\)
 */
#define SPLT_DIRCHAR '/'
/**
 * @brief Path separator as string (/ or \\)
 */
#define SPLT_DIRSTR "/"
#endif
#endif

/**
 * @brief Used in mp3splt and mp3splt-gtk.
 */
#define MP3SPLT_LIB_GETTEXT_DOMAIN "libmp3splt"

//@}

/**
 * @brief Allow several inclusions of this file.
 */
#define MP3SPLT_MP3SPLT_H
#endif

