#!/bin/bash

. ./utils.sh || exit 1

function _check_silence_output_files
{
  current_file="$OUTPUT_DIR/${M_FILE}_silence_1.mp3"
  check_current_mp3_length "01.04"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1792229"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_2.mp3"
  check_current_mp3_length "01.50"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "3146272"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_3.mp3"
  check_current_mp3_length "01.10"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "3" "http://www.jamendo.com/"
  check_current_file_size "1975986"
}

function test_silence
{
  rm -f mp3splt.log
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="silence mode - check creating mp3splt.log"

  expected=" Processing file 'songs/La_Verue__Today_silence.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 info: starting silence mode split
 Silence split type: Auto mode (Th: -48.0 dB, Off: 0.80, Min: 0.00, Remove: NO, Min track: 0.00, Shots: 25)

 Total silence points found: 2. (Selected 3 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_2.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_3.mp3\" created
 silence split ok
 Average silence level: -23.09 dB"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  _check_silence_output_files

  expected="songs/La_Verue__Today_silence.mp3
-48.00\t0.00\t25
56.840000\t66.790001\t995
168.350006\t177.240005\t889"
  check_file_content "mp3splt.log" "$expected"

  print_ok
  echo

  test_name="silence mode - check reading from mp3splt.log"

  remove_output_dir

  expected=" Processing file 'songs/La_Verue__Today_silence.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 info: starting silence mode split
 Silence split type: Auto mode (Th: -48.0 dB, Off: 0.80, Min: 0.00, Remove: NO, Min track: 0.00, Shots: 25)
 Found silence log file 'mp3splt.log' ! Reading silence points from file to save time ;)
 Total silence points found: 2. (Selected 3 tracks)
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_2.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_3.mp3\" created
 silence split ok"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  _check_silence_output_files

  print_ok
  echo
}

function test_silence_and_pretend
{
  rm -f mp3splt.log
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="silence mode & pretend"

  expected=" Pretending to split file 'songs/La_Verue__Today_silence.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 info: starting silence mode split
 Silence split type: Auto mode (Th: -48.0 dB, Off: 0.80, Min: 0.00, Remove: NO, Min track: 0.00, Shots: 25)

 Total silence points found: 2. (Selected 3 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_2.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_3.mp3\" created
 silence split ok
 Average silence level: -23.09 dB"
  mp3splt_args="-P -d $OUTPUT_DIR -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_is_empty
  check_if_file_does_not_exist "mp3splt.log"

  print_ok
  echo
}

function test_silence_and_cue_export
{
  rm -f mp3splt.log
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="silence mode & cue export"

  expected=" Processing file 'songs/La_Verue__Today_silence.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 info: starting silence mode split
 Silence split type: Auto mode (Th: -48.0 dB, Off: 0.80, Min: 0.00, Remove: NO, Min track: 0.00, Shots: 25)

 Total silence points found: 2. (Selected 3 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_2.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_3.mp3\" created
 silence split ok
 Average silence level: -23.09 dB
 CUE file 'output/output_out.cue' created."
  mp3splt_args="-T 2 -E output/out.cue -d $OUTPUT_DIR -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  _check_silence_output_files

  expected="songs/La_Verue__Today_silence.mp3
-48.00\t0.00\t25
56.840000\t66.790001\t995
168.350006\t177.240005\t889"
  check_file_content "mp3splt.log" "$expected"

  check_file_content "output/output_out.cue" 'TITLE "Riez Noir"
PERFORMER "La Verue"
FILE "songs/La_Verue__Today_silence.mp3" MP3
  TRACK 01 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    REM ALBUM "Riez Noir"
    REM GENRE "Rock"
    REM COMMENT "http://www.jamendo.com/"
    REM NAME "La_Verue__Today_silence_silence_1"
    INDEX 01 00:00:00
  TRACK 02 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    REM ALBUM "Riez Noir"
    REM GENRE "Rock"
    REM COMMENT "http://www.jamendo.com/"
    REM NAME "La_Verue__Today_silence_silence_2"
    INDEX 01 01:04:60
  TRACK 03 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    REM ALBUM "Riez Noir"
    REM GENRE "Rock"
    REM COMMENT "http://www.jamendo.com/"
    REM NAME "La_Verue__Today_silence_silence_3"
    INDEX 01 02:55:35'

  print_ok
  echo
}

function test_silence_and_pretend_and_cue_export
{
  rm -f mp3splt.log
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="silence mode & pretend & cue export"

  expected=" Pretending to split file 'songs/La_Verue__Today_silence.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 info: starting silence mode split
 Silence split type: Auto mode (Th: -48.0 dB, Off: 0.80, Min: 0.00, Remove: NO, Min track: 0.00, Shots: 25)

 Total silence points found: 2. (Selected 3 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_2.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_3.mp3\" created
 silence split ok
 Average silence level: -23.09 dB
 CUE file 'output/output_out.cue' created."
  mp3splt_args="-P -E output/out.cue -d $OUTPUT_DIR -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_number_of_files 1
  check_if_file_does_not_exist "mp3splt.log"

  print_ok
  echo
}

function test_silence_offset
{
  rm -f mp3splt.log
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="silence mode & offset"

  expected=" Processing file 'songs/La_Verue__Today_silence.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 info: starting silence mode split
 Silence split type: Auto mode (Th: -48.0 dB, Off: 0.00, Min: 0.00, Remove: NO, Min track: 0.00, Shots: 25)

 Total silence points found: 2. (Selected 3 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_2.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_3.mp3\" created
 silence split ok
 Average silence level: -23.09 dB"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -p off=0 -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_1.mp3"
  check_current_mp3_length "00.56"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1615443"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_2.mp3"
  check_current_mp3_length "01.51"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "3166410"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_3.mp3"
  check_current_mp3_length "01.17"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "3" "http://www.jamendo.com/"
  check_current_file_size "2132634"

  expected="songs/La_Verue__Today_silence.mp3
-48.00\t0.00\t25
56.840000\t66.790001\t995
168.350006\t177.240005\t889"
  check_file_content "mp3splt.log" "$expected"

  print_ok
  echo
}

function test_silence_threshold
{
  rm -f mp3splt.log
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="silence mode & threshold"

  expected=" Processing file 'songs/La_Verue__Today_silence.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 info: starting silence mode split
 Silence split type: Auto mode (Th: -18.0 dB, Off: 0.80, Min: 0.00, Remove: NO, Min track: 0.00, Shots: 25)

 Total silence points found: 3. (Selected 4 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_2.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_3.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_4.mp3\" created
 silence split ok
 Average silence level: -23.09 dB"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -p th=-18 -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_1.mp3"
  check_current_mp3_length "01.04"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1792229"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_2.mp3"
  check_current_mp3_length "01.50"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "3146272"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_3.mp3"
  check_current_mp3_length "00.27"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "3" "http://www.jamendo.com/"
  check_current_file_size "795595"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_4.mp3"
  check_current_mp3_length "00.42"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "4" "http://www.jamendo.com/"
  check_current_file_size "1182022"

  expected="songs/La_Verue__Today_silence.mp3
-18.00\t0.00\t25
56.810001\t66.790001\t998
168.350006\t177.240005\t889
202.649994\t202.990005\t34"
  check_file_content "mp3splt.log" "$expected"

  print_ok
  echo
}

function test_silence_nt
{
  rm -f mp3splt.log
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="silence mode & nt"

  expected=" Processing file 'songs/La_Verue__Today_silence.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 info: starting silence mode split
 Silence split type: User mode (Th: -48.0 dB, Off: 0.80, Min: 0.00, Remove: NO, Min track: 0.00, Shots: 25)

 Total silence points found: 2. (Selected 2 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_2.mp3\" created
 silence split ok
 Average silence level: -23.09 dB"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -p nt=2 -s $SILENCE_MP3_FILE"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_1.mp3"
  check_current_mp3_length "01.04"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1792229"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_2.mp3"
  check_current_mp3_length "03.00"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "5120627"

  expected="songs/La_Verue__Today_silence.mp3
-48.00\t0.00\t25
56.840000\t66.790001\t995
168.350006\t177.240005\t889"
  check_file_content "mp3splt.log" "$expected"

  print_ok
  echo
}

function test_silence_rm
{
  rm -f mp3splt.log
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="silence mode & rm"

  expected=" Processing file 'songs/La_Verue__Today_silence.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 info: starting silence mode split
 Silence split type: Auto mode (Th: -48.0 dB, Off: 0.80, Min: 0.00, Remove: YES(0.00-0.00), Min track: 0.00, Shots: 25)

 Total silence points found: 2. (Selected 3 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_2.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_3.mp3\" created
 silence split ok
 Average silence level: -23.09 dB"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -p rm -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_1.mp3"
  check_current_mp3_length "00.56"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1615443"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_2.mp3"
  check_current_mp3_length "01.41"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "2946004"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_3.mp3"
  check_current_mp3_length "01.08"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "3" "http://www.jamendo.com/"
  check_current_file_size "1936334"

  expected="songs/La_Verue__Today_silence.mp3
-48.00\t0.00\t25
56.840000\t66.790001\t995
168.350006\t177.240005\t889"
  check_file_content "mp3splt.log" "$expected"

  print_ok
  echo
}

function test_silence_rm_with_keep_left_and_right_parameter_values_and_silence_length_bigger_than_values
{
  rm -f mp3splt.log
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="silence mode & rm & keep left - right parameter values & silence length bigger than values"

  expected=" Processing file 'songs/La_Verue__Today_silence.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 info: starting silence mode split
 Silence split type: Auto mode (Th: -48.0 dB, Off: 0.80, Min: 0.00, Remove: YES(2.00-6.00), Min track: 0.00, Shots: 25)

 Total silence points found: 2. (Selected 3 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_2.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_3.mp3\" created
 silence split ok
 Average silence level: -23.09 dB"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -p rm=2_6 -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_1.mp3"
  check_current_mp3_length "01.02"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1747671"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_2.mp3"
  check_current_mp3_length "01.49"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "3122272"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_3.mp3"
  check_current_mp3_length "01.10"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "3" "http://www.jamendo.com/"
  check_current_file_size "1982038"

  expected="songs/La_Verue__Today_silence.mp3
-48.00\t0.00\t25
56.840000\t66.790001\t995
168.350006\t177.240005\t889"
  check_file_content "mp3splt.log" "$expected"

  print_ok
  echo
}

function test_silence_rm_with_keep_left_and_right_parameter_values_and_silence_length_lesser_than_values
{
  rm -f mp3splt.log
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="silence mode & rm & keep left - right parameter values & silence length lesser than values"

  expected=" Processing file 'songs/La_Verue__Today_silence.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 info: starting silence mode split
 Silence split type: Auto mode (Th: -48.0 dB, Off: 0.80, Min: 0.00, Remove: YES(10.00-20.00), Min track: 0.00, Shots: 25)

 Total silence points found: 2. (Selected 3 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_2.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_3.mp3\" created
 silence split ok
 Average silence level: -23.09 dB"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -p rm=10_20 -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_1.mp3"
  check_current_mp3_length "01.03"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1762279"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_2.mp3"
  check_current_mp3_length "01.50"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "3149196"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_3.mp3"
  check_current_mp3_length "01.11"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "3" "http://www.jamendo.com/"
  check_current_file_size "2003012"

  expected="songs/La_Verue__Today_silence.mp3
-48.00\t0.00\t25
56.840000\t66.790001\t995
168.350006\t177.240005\t889"
  check_file_content "mp3splt.log" "$expected"

  print_ok
  echo
}

function test_silence_rm_and_output_format
{
  rm -f mp3splt.log
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="silence mode & rm & output format"

  expected=" Processing file 'songs/La_Verue__Today_silence.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 info: starting silence mode split
 Silence split type: Auto mode (Th: -48.0 dB, Off: 0.80, Min: 0.00, Remove: YES(0.00-0.00), Min track: 0.00, Shots: 25)

 Total silence points found: 2. (Selected 3 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/silence_1_00:00:00 00:56:84.mp3\" created
   File \"$OUTPUT_DIR/silence_2_01:06:79 02:48:35.mp3\" created
   File \"$OUTPUT_DIR/silence_3_02:57:24 04:05:69.mp3\" created
 silence split ok
 Average silence level: -23.09 dB"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -o \"silence_@n_@m:@s:@h+@M:@S:@H\" -p rm -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/silence_1_00:00:00 00:56:84.mp3"
  check_current_mp3_length "00.56"

  current_file="$OUTPUT_DIR/silence_2_01:06:79 02:48:35.mp3"
  check_current_mp3_length "01.41"

  current_file="$OUTPUT_DIR/silence_3_02:57:24 04:05:69.mp3"
  check_current_mp3_length "01.08"

  print_ok
  echo
}

function test_silence_rm_and_overlap
{
  rm -f mp3splt.log
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="silence mode & rm & overlap"

  expected=" Processing file 'songs/La_Verue__Today_silence.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 info: starting silence mode split
 Silence split type: Auto mode (Th: -48.0 dB, Off: 0.80, Min: 0.00, Remove: YES(0.00-0.00), Min track: 0.00, Shots: 25)

 Total silence points found: 2. (Selected 3 tracks)
 Writing silence log file 'mp3splt.log' ...
 info: overlapping split files with 0.10.0
   File \"$OUTPUT_DIR/silence_1_00:00:00 01:06:84.mp3\" created
   File \"$OUTPUT_DIR/silence_2_01:06:79 02:58:35.mp3\" created
   File \"$OUTPUT_DIR/silence_3_02:57:24 04:05:69.mp3\" created
 silence split ok
 Average silence level: -23.09 dB"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -O 0.10 -o \"silence_@n_@m:@s:@h+@M:@S:@H\" -p rm -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/silence_1_00:00:00 01:06:84.mp3"
  check_current_mp3_length "01.06"

  current_file="$OUTPUT_DIR/silence_2_01:06:79 02:58:35.mp3"
  check_current_mp3_length "01.51"

  current_file="$OUTPUT_DIR/silence_3_02:57:24 04:05:69.mp3"
  check_current_mp3_length "01.08"

  print_ok
  echo
}

function test_silence_rm_and_custom_tags
{
  rm -f mp3splt.log
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="silence mode & rm & custom tags"

  expected=" Processing file 'songs/La_Verue__Today_silence.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 info: starting silence mode split
 Silence split type: Auto mode (Th: -48.0 dB, Off: 0.80, Min: 0.00, Remove: YES(0.00-0.00), Min track: 0.00, Shots: 25)

 Total silence points found: 2. (Selected 3 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_2.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_3.mp3\" created
 silence split ok
 Average silence level: -23.09 dB"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -g %[@o,@N=1,@t=title1][@t=title2][@a=artist3] -p rm -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_1.mp3"
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "title1"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_2.mp3"
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "title2"\
  "2007" "Rock" "17" "2" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_3.mp3"
  check_all_mp3_tags_with_version "2" "artist3" "Riez Noir" "title1"\
  "2007" "Rock" "17" "3" "http://www.jamendo.com/"

  print_ok
  echo
}

function test_silence_rm_and_custom_tags_and_output_format
{
  rm -f mp3splt.log
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="silence mode & rm & custom tags & output format"

  expected=" Processing file 'songs/La_Verue__Today_silence.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 info: starting silence mode split
 Silence split type: Auto mode (Th: -48.0 dB, Off: 0.80, Min: 0.00, Remove: YES(0.00-0.00), Min track: 0.00, Shots: 25)

 Total silence points found: 2. (Selected 3 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/La Verue__Riez Noir__title1_1.mp3\" created
   File \"$OUTPUT_DIR/La Verue__Riez Noir__title2_2.mp3\" created
   File \"$OUTPUT_DIR/artist3__Riez Noir__title1_3.mp3\" created
 silence split ok
 Average silence level: -23.09 dB"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -g %[@o,@N=1,@t=title1][@t=title2][@a=artist3] -o @a__@b__@t_@n -p rm -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/La Verue__Riez Noir__title1_1.mp3"
  check_current_mp3_length "00.56"

  current_file="$OUTPUT_DIR/La Verue__Riez Noir__title2_2.mp3"
  check_current_mp3_length "01.41"

  current_file="$OUTPUT_DIR/artist3__Riez Noir__title1_3.mp3"
  check_current_mp3_length "01.08"

  print_ok
  echo
}

function test_silence_min
{
  rm -f mp3splt.log
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="silence mode & min"

  expected=" Processing file 'songs/La_Verue__Today_silence.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 info: starting silence mode split
 Silence split type: Auto mode (Th: -48.0 dB, Off: 0.80, Min: 9.00, Remove: NO, Min track: 0.00, Shots: 25)

 Total silence points found: 1. (Selected 2 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_2.mp3\" created
 silence split ok
 Average silence level: -23.09 dB"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -p min=9 -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_1.mp3"
  check_current_mp3_length "01.04"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1792229"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_2.mp3"
  check_current_mp3_length "03.00"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "5120627"

  expected="songs/La_Verue__Today_silence.mp3
-48.00\t9.00\t25
56.840000\t66.790001\t995"
  check_file_content "mp3splt.log" "$expected"

  print_ok
  echo
}

function test_silence_with_trackmin
{
  rm -f mp3splt.log
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="silence mode - checking trackmin"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 info: starting silence mode split
 Silence split type: Auto mode (Th: -48.0 dB, Off: 0.80, Min: 0.00, Remove: NO, Min track: 71.00, Shots: 25)

 Total silence points found: 2. (Selected 3 tracks)
 info: track too short (1.4.80 < 1.11.0); skipped.
 info: track too short (1.10.23 < 1.11.0); skipped.
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
 silence split ok
 Average silence level: -23.09 dB"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -p trackmin=71 -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_1.mp3"
  check_current_mp3_length "01.50"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "3147420"

  expected="songs/La_Verue__Today_silence.mp3
-48.00\t0.00\t25
56.840000\t66.790001\t995
168.350006\t177.240005\t889"
  check_file_content "mp3splt.log" "$expected"

  print_ok
  echo
}

function test_silence_with_rm_and_trackmin
{
  rm -f mp3splt.log
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="silence mode & rm"

  expected=" Processing file 'songs/La_Verue__Today_silence.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 info: starting silence mode split
 Silence split type: Auto mode (Th: -48.0 dB, Off: 0.80, Min: 0.00, Remove: YES(0.00-0.00), Min track: 57.00, Shots: 25)

 Total silence points found: 2. (Selected 3 tracks)
 info: track too short (0.56.84 < 0.57.0); skipped.
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_2.mp3\" created
 silence split ok
 Average silence level: -23.09 dB"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -p rm,trackmin=57 -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_1.mp3"
  check_current_mp3_length "01.41"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "2946004"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_2.mp3"
  check_current_mp3_length "01.08"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "1936334"

  expected="songs/La_Verue__Today_silence.mp3
-48.00\t0.00\t25
56.840000\t66.790001\t995
168.350006\t177.240005\t889"
  check_file_content "mp3splt.log" "$expected"

  print_ok
  echo
}

function test_silence_with_trackjoin_no_join_done
{
  rm -f mp3splt.log
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="silence mode - trackjoin & no join"

  expected=" Processing file 'songs/La_Verue__Today_silence.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 info: starting silence mode split
 Silence split type: Auto mode (Th: -48.0 dB, Off: 0.80, Min: 0.00, Remove: NO, Min track: 0.00, Shots: 25, Min track join: 64.00)

 Total silence points found: 2. (Selected 3 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_2.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_3.mp3\" created
 silence split ok
 Average silence level: -23.09 dB"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -p trackjoin=64 -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  _check_silence_output_files

  expected="songs/La_Verue__Today_silence.mp3
-48.00\t0.00\t25
56.840000\t66.790001\t995
168.350006\t177.240005\t889"
  check_file_content "mp3splt.log" "$expected"

  print_ok
  echo
}

function test_silence_with_trackjoin_one_file_joined
{
  rm -f mp3splt.log
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="silence mode - trackjoin & one file joined"

  expected=" Processing file 'songs/La_Verue__Today_silence.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 info: starting silence mode split
 Silence split type: Auto mode (Th: -48.0 dB, Off: 0.80, Min: 0.00, Remove: NO, Min track: 0.00, Shots: 25, Min track join: 65.00)

 Total silence points found: 2. (Selected 3 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_2.mp3\" created
 silence split ok
 Average silence level: -23.09 dB"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -p trackjoin=65 -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_1.mp3"
  check_current_mp3_length "02.55"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "4936870"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_2.mp3"
  check_current_mp3_length "01.10"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "1975986"

  expected="songs/La_Verue__Today_silence.mp3
-48.00\t0.00\t25
56.840000\t66.790001\t995
168.350006\t177.240005\t889"
  check_file_content "mp3splt.log" "$expected"

  print_ok
  echo
}

function test_silence_with_trackjoin_all_files_joined
{
  rm -f mp3splt.log
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="silence mode - trackjoin & all files joined"

  expected=" Processing file 'songs/La_Verue__Today_silence.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 info: starting silence mode split
 Silence split type: Auto mode (Th: -48.0 dB, Off: 0.80, Min: 0.00, Remove: NO, Min track: 0.00, Shots: 25, Min track join: 71.00)

 Total silence points found: 2. (Selected 3 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
 silence split ok
 Average silence level: -23.09 dB"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -p trackjoin=71 -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_1.mp3"
  check_current_mp3_length "04.05"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "6911225"

  expected="songs/La_Verue__Today_silence.mp3
-48.00\t0.00\t25
56.840000\t66.790001\t995
168.350006\t177.240005\t889"
  check_file_content "mp3splt.log" "$expected"

  print_ok
  echo
}

function test_silence_full_log
{
  rm -f mp3splt.log
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="silence mode - check full log file silence_logs.txt"

  silence_logs_file="silence_logs.txt"

  expected=" Processing file 'songs/La_Verue__Today_silence.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 info: starting silence mode split
 Silence split type: Auto mode (Th: -48.0 dB, Off: 0.80, Min: 0.00, Remove: NO, Min track: 0.00, Shots: 25)

 Total silence points found: 2. (Selected 3 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_2.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_3.mp3\" created
 silence split ok
 Average silence level: -23.09 dB"
  mp3splt_args="-T 2 -F $silence_logs_file -d $OUTPUT_DIR -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  check_files_content "expected_silence_logs.txt" "$silence_logs_file"

  print_ok
  echo
}

function test_silence_trim
{
  rm -f mp3splt.log
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="trim silence mode"

  expected=" Processing file 'songs/La_Verue__Today_silence.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 info: starting trim using silence mode split
 Trim silence split - Th: -48.0 dB, Min: 0.00 sec
   File \"$OUTPUT_DIR/${M_FILE}_trimmed.mp3\" created
 trim using silence split ok"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -r $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_trimmed.mp3"
  check_current_mp3_length "04.01"
  check_current_file_size "6809389"

  print_ok
  echo
}

function test_silence_trim_with_min_length
{
  rm -f mp3splt.log
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="trim silence mode & min length parameter"

  expected=" Processing file 'songs/La_Verue__Today_silence.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 info: starting trim using silence mode split
 Trim silence split - Th: -48.0 dB, Min: 1.50 sec
 info: trim begin split at 0m_00s_00h
 info: trim end split at 4m_03s_68h
   File \"$OUTPUT_DIR/${M_FILE}_trimmed.mp3\" created
 trim using silence split ok"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -p min=1.5 -r $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_trimmed.mp3"
  check_current_mp3_length "04.03"
  check_current_file_size "6865107"

  print_ok
  echo
}

function test_silence_trim_with_min_length_small
{
  rm -f mp3splt.log
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="trim silence mode & min length parameter small"

  expected=" Processing file 'songs/La_Verue__Today_silence.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 info: starting trim using silence mode split
 Trim silence split - Th: -48.0 dB, Min: 0.50 sec
 info: trim begin split at 0m_00s_43h
 info: trim end split at 4m_02s_68h
   File \"$OUTPUT_DIR/${M_FILE}_trimmed.mp3\" created
 trim using silence split ok"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -p min=0.5 -r $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_trimmed.mp3"
  check_current_mp3_length "04.02"
  check_current_file_size "6833179"

  print_ok
  echo
}

function test_silence_trim_with_min_length_huge
{
  rm -f mp3splt.log
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="trim silence mode & min length parameter huge"

  expected=" Processing file 'songs/La_Verue__Today_silence.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 info: starting trim using silence mode split
 Trim silence split - Th: -48.0 dB, Min: 20.00 sec
 info: trim begin split at 0m_00s_00h
 info: trim end split at 4m_05s_70h
   File \"$OUTPUT_DIR/${M_FILE}_trimmed.mp3\" created
 trim using silence split ok"
  mp3splt_args="-d $OUTPUT_DIR -p min=20 -r $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_trimmed.mp3"
  check_current_mp3_length "04.05"
  check_current_file_size "6911353"

  print_ok
  echo
}

function run_silence_mode_tests
{
  p_blue " SILENCE tests ..."
  echo

  silence_mode_test_functions=$(declare -F | grep " test_silence" | awk '{ print $3 }')

  for test_func in $silence_mode_test_functions;do
    eval $test_func
  done

  p_blue " SILENCE tests DONE."
  echo
}

#main
export LC_ALL="C"
start_date=$(date +%s)

run_silence_mode_tests

p_failed_tests

end_date=$(date +%s)

p_time_diff_cyan $start_date $end_date "\t"
echo -e '\n'

exit 0

