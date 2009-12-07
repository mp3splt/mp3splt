#!/bin/bash

. ./utils.sh || exit 1

function _check_silence_output_files
{
  current_file="$OUTPUT_DIR/${M_FILE}_silence_1.mp3"
  check_current_mp3_length "01.04"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1791231"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_2.mp3"
  check_current_mp3_length "01.50"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "3145274"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_3.mp3"
  check_current_mp3_length "01.10"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "3" "http://www.jamendo.com/"
  check_current_file_size "1974988"
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
 Silence split type: Auto mode (Th: -48,0 dB, Off: 0,80, Min: 0,00, Remove: NO)

 Total silence points found: 2. (Selected 3 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_2.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_3.mp3\" created
 silence split ok
 Average silence level: -23,08 dB"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  _check_silence_output_files

  expected="songs/La_Verue__Today_silence.mp3
-48,00\t0,00
56,840000\t66,790001\t995
168,350006\t177,240005\t889"
  check_file_content "mp3splt.log" "$expected"

  p_green "OK"
  echo

  test_name="silence mode - check reading from mp3splt.log"

  remove_output_dir

  expected=" Processing file 'songs/La_Verue__Today_silence.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 info: starting silence mode split
 Silence split type: Auto mode (Th: -48,0 dB, Off: 0,80, Min: 0,00, Remove: NO)
 Found silence log file 'mp3splt.log' ! Reading silence points from file to save time ;)
 Total silence points found: 2. (Selected 3 tracks)
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_2.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_3.mp3\" created
 silence split ok"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  _check_silence_output_files

  p_green "OK"
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
 Silence split type: Auto mode (Th: -48,0 dB, Off: 0,80, Min: 0,00, Remove: NO)

 Total silence points found: 2. (Selected 3 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_2.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_3.mp3\" created
 silence split ok
 Average silence level: -23,08 dB"
  mp3splt_args="-P -d $OUTPUT_DIR -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_is_empty
  check_if_file_does_not_exist "mp3splt.log"

  p_green "OK"
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
 Silence split type: Auto mode (Th: -48,0 dB, Off: 0,80, Min: 0,00, Remove: NO)

 Total silence points found: 2. (Selected 3 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_2.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_3.mp3\" created
 silence split ok
 Average silence level: -23,08 dB
 CUE file 'output/output_out.cue' created."
  mp3splt_args="-T 2 -E output/out.cue -d $OUTPUT_DIR -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  _check_silence_output_files

  expected="songs/La_Verue__Today_silence.mp3
-48,00\t0,00
56,840000\t66,790001\t995
168,350006\t177,240005\t889"
  check_file_content "mp3splt.log" "$expected"

  check_file_content "output/output_out.cue" 'TITLE "Riez Noir"
PERFORMER "La Verue"
FILE "songs/La_Verue__Today_silence.mp3" MP3
  TRACK 01 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    INDEX 01 00:00:00
  TRACK 02 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    INDEX 01 01:04:79
  TRACK 03 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    INDEX 01 02:55:46
  TRACK 04 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    INDEX 01 04:05:68'

  p_green "OK"
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
 Silence split type: Auto mode (Th: -48,0 dB, Off: 0,80, Min: 0,00, Remove: NO)

 Total silence points found: 2. (Selected 3 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_2.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_3.mp3\" created
 silence split ok
 Average silence level: -23,08 dB
 CUE file 'output/output_out.cue' created."
  mp3splt_args="-P -E output/out.cue -d $OUTPUT_DIR -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_number_of_files 1
  check_if_file_does_not_exist "mp3splt.log"

  p_green "OK"
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
 Silence split type: Auto mode (Th: -48,0 dB, Off: 0,00, Min: 0,00, Remove: NO)

 Total silence points found: 2. (Selected 3 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_2.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_3.mp3\" created
 silence split ok
 Average silence level: -23,08 dB"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -p off=0 -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_1.mp3"
  check_current_mp3_length "00.56"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1614445"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_2.mp3"
  check_current_mp3_length "01.51"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "3165412"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_3.mp3"
  check_current_mp3_length "01.17"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "3" "http://www.jamendo.com/"
  check_current_file_size "2131636"

  expected="songs/La_Verue__Today_silence.mp3
-48,00\t0,00
56,840000\t66,790001\t995
168,350006\t177,240005\t889"
  check_file_content "mp3splt.log" "$expected"

  p_green "OK"
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
 Silence split type: Auto mode (Th: -18,0 dB, Off: 0,80, Min: 0,00, Remove: NO)

 Total silence points found: 3. (Selected 4 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_2.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_3.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_4.mp3\" created
 silence split ok
 Average silence level: -23,08 dB"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -p th=-18 -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_1.mp3"
  check_current_mp3_length "01.04"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1791231"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_2.mp3"
  check_current_mp3_length "01.50"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "3145274"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_3.mp3"
  check_current_mp3_length "00.27"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "3" "http://www.jamendo.com/"
  check_current_file_size "794597"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_4.mp3"
  check_current_mp3_length "00.42"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "4" "http://www.jamendo.com/"
  check_current_file_size "1181024"

  expected="songs/La_Verue__Today_silence.mp3
-18,00\t0,00
56,810001\t66,790001\t998
168,350006\t177,240005\t889
202,649994\t202,990005\t34"
  check_file_content "mp3splt.log" "$expected"

  p_green "OK"
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
 Silence split type: User mode (Th: -48,0 dB, Off: 0,80, Min: 0,00, Remove: NO)

 Total silence points found: 2. (Selected 2 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_2.mp3\" created
 silence split ok
 Average silence level: -23,08 dB"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -p nt=2 -s $SILENCE_MP3_FILE"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_1.mp3"
  check_current_mp3_length "01.04"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1791231"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_2.mp3"
  check_current_mp3_length "03.00"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "5119629"

  expected="songs/La_Verue__Today_silence.mp3
-48,00\t0,00
56,840000\t66,790001\t995
168,350006\t177,240005\t889"
  check_file_content "mp3splt.log" "$expected"

  p_green "OK"
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
 Silence split type: Auto mode (Th: -48,0 dB, Off: 0,80, Min: 0,00, Remove: YES)

 Total silence points found: 2. (Selected 3 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_2.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_3.mp3\" created
 silence split ok
 Average silence level: -23,08 dB"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -p rm -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_1.mp3"
  check_current_mp3_length "00.56"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1614445"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_2.mp3"
  check_current_mp3_length "01.41"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "2945006"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_3.mp3"
  check_current_mp3_length "01.08"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "3" "http://www.jamendo.com/"
  check_current_file_size "1935336"

  expected="songs/La_Verue__Today_silence.mp3
-48,00\t0,00
56,840000\t66,790001\t995
168,350006\t177,240005\t889"
  check_file_content "mp3splt.log" "$expected"

  p_green "OK"
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
 Silence split type: Auto mode (Th: -48,0 dB, Off: 0,80, Min: 0,00, Remove: YES)

 Total silence points found: 2. (Selected 3 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/silence_1_00:00:00 00:56:84.mp3\" created
   File \"$OUTPUT_DIR/silence_2_01:06:79 02:48:35.mp3\" created
   File \"$OUTPUT_DIR/silence_3_02:57:24 04:05:69.mp3\" created
 silence split ok
 Average silence level: -23,08 dB"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -o \"silence_@n_@m:@s:@h+@M:@S:@H\" -p rm -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/silence_1_00:00:00 00:56:84.mp3"
  check_current_mp3_length "00.56"

  current_file="$OUTPUT_DIR/silence_2_01:06:79 02:48:35.mp3"
  check_current_mp3_length "01.41"

  current_file="$OUTPUT_DIR/silence_3_02:57:24 04:05:69.mp3"
  check_current_mp3_length "01.08"

  p_green "OK"
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
 Silence split type: Auto mode (Th: -48,0 dB, Off: 0,80, Min: 0,00, Remove: YES)

 Total silence points found: 2. (Selected 3 tracks)
 Writing silence log file 'mp3splt.log' ...
 info: overlapping split files with 0.10.0
   File \"$OUTPUT_DIR/silence_1_00:00:00 01:06:84.mp3\" created
   File \"$OUTPUT_DIR/silence_2_01:06:79 02:58:35.mp3\" created
   File \"$OUTPUT_DIR/silence_3_02:57:24 04:05:69.mp3\" created
 silence split ok
 Average silence level: -23,08 dB"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -O 0.10 -o \"silence_@n_@m:@s:@h+@M:@S:@H\" -p rm -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/silence_1_00:00:00 01:06:84.mp3"
  check_current_mp3_length "01.06"

  current_file="$OUTPUT_DIR/silence_2_01:06:79 02:58:35.mp3"
  check_current_mp3_length "01.51"

  current_file="$OUTPUT_DIR/silence_3_02:57:24 04:05:69.mp3"
  check_current_mp3_length "01.08"

  p_green "OK"
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
 Silence split type: Auto mode (Th: -48,0 dB, Off: 0,80, Min: 0,00, Remove: YES)

 Total silence points found: 2. (Selected 3 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_2.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_3.mp3\" created
 silence split ok
 Average silence level: -23,08 dB"
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

  p_green "OK"
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
 Silence split type: Auto mode (Th: -48,0 dB, Off: 0,80, Min: 0,00, Remove: YES)

 Total silence points found: 2. (Selected 3 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/La Verue__Riez Noir__title1_1.mp3\" created
   File \"$OUTPUT_DIR/La Verue__Riez Noir__title2_2.mp3\" created
   File \"$OUTPUT_DIR/artist3__Riez Noir__title1_3.mp3\" created
 silence split ok
 Average silence level: -23,08 dB"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -g %[@o,@N=1,@t=title1][@t=title2][@a=artist3] -o @a__@b__@t_@n -p rm -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/La Verue__Riez Noir__title1_1.mp3"
  check_current_mp3_length "00.56"

  current_file="$OUTPUT_DIR/La Verue__Riez Noir__title2_2.mp3"
  check_current_mp3_length "01.41"

  current_file="$OUTPUT_DIR/artist3__Riez Noir__title1_3.mp3"
  check_current_mp3_length "01.08"

  p_green "OK"
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
 Silence split type: Auto mode (Th: -48,0 dB, Off: 0,80, Min: 9,00, Remove: NO)

 Total silence points found: 1. (Selected 2 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${M_FILE}_silence_1.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_silence_2.mp3\" created
 silence split ok
 Average silence level: -23,08 dB"
  mp3splt_args="-T 2 -d $OUTPUT_DIR -p min=9 -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_1.mp3"
  check_current_mp3_length "01.04"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1791231"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_2.mp3"
  check_current_mp3_length "03.00"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "5119629"

  expected="songs/La_Verue__Today_silence.mp3
-48,00\t9,00
56,840000\t66,790001\t995"
  check_file_content "mp3splt.log" "$expected"

  p_green "OK"
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
export LANGUAGE="en"
start_date=$(date +%s)

run_silence_mode_tests

end_date=$(date +%s)

p_time_diff_cyan $start_date $end_date "\t"
echo -e '\n'

exit 0

