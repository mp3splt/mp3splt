#!/bin/bash

. ./constants_variables.sh
. ./utils.sh

function _check_silence_output_files
{
  current_file="$OUTPUT_DIR/${M_FILE}_silence_1.mp3"
  check_current_mp3_length "01.04"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "None" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1791210"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_2.mp3"
  check_current_mp3_length "01.50"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "None" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "3145253"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_3.mp3"
  check_current_mp3_length "01.10"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "None" "Rock" "17" "3" "http://www.jamendo.com/"
  check_current_file_size "1974967"
}

function test_silence
{
  rm -f mp3splt.log
  rm -rf $OUTPUT_DIR/*

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
 Average silence level : -23,07 dB"
  mp3splt_args="-d $OUTPUT_DIR -s $SILENCE_MP3_FILE" 
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

  rm -rf $OUTPUT_DIR/*

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
  mp3splt_args="-d $OUTPUT_DIR -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  _check_silence_output_files

  p_green "OK"
  echo
}

function test_silence_offset
{
  rm -f mp3splt.log
  rm -rf $OUTPUT_DIR/*

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
 Average silence level : -23,07 dB"
  mp3splt_args="-d $OUTPUT_DIR -p off=0 -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_1.mp3"
  check_current_mp3_length "00.56"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "None" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1614424"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_2.mp3"
  check_current_mp3_length "01.51"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "None" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "3165391"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_3.mp3"
  check_current_mp3_length "01.17"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "None" "Rock" "17" "3" "http://www.jamendo.com/"
  check_current_file_size "2131615"

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
  rm -rf $OUTPUT_DIR/*

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
 Average silence level : -23,07 dB"
  mp3splt_args="-d $OUTPUT_DIR -p th=-18 -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_1.mp3"
  check_current_mp3_length "01.04"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "None" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1791210"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_2.mp3"
  check_current_mp3_length "01.50"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "None" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "3145253"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_3.mp3"
  check_current_mp3_length "00.27"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "None" "Rock" "17" "3" "http://www.jamendo.com/"
  check_current_file_size "794576"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_4.mp3"
  check_current_mp3_length "00.42"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "None" "Rock" "17" "4" "http://www.jamendo.com/"
  check_current_file_size "1181003"

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
  rm -rf $OUTPUT_DIR/*

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
 Average silence level : -23,07 dB"
  mp3splt_args="-d $OUTPUT_DIR -p nt=2 -s $SILENCE_MP3_FILE"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_1.mp3"
  check_current_mp3_length "01.04"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "None" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1791210"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_2.mp3"
  check_current_mp3_length "03.00"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "None" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "5119608"

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
  rm -rf $OUTPUT_DIR/*

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
 silence split ok
 Average silence level : -23,07 dB"
  mp3splt_args="-d $OUTPUT_DIR -p rm -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_1.mp3"
  check_current_mp3_length "00.56"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "None" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1614424"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_2.mp3"
  check_current_mp3_length "01.41"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "None" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "2944985"

#  current_file="$OUTPUT_DIR/${M_FILE}_silence_3.mp3"
#  check_current_mp3_length "01.17"
#  check_current_file_has_xing
#  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
#  "None" "Rock" "17" "3" "http://www.jamendo.com/"
#  check_current_file_size "2131615"

  expected="songs/La_Verue__Today_silence.mp3
-48,00\t0,00
56,840000\t66,790001\t995
168,350006\t177,240005\t889"
  check_file_content "mp3splt.log" "$expected"

  p_green "OK"
  echo
}

function test_silence_min
{
  rm -f mp3splt.log
  rm -rf $OUTPUT_DIR/*

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
 Average silence level : -23,07 dB"
  mp3splt_args="-d $OUTPUT_DIR -p min=9 -s $SILENCE_MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_1.mp3"
  check_current_mp3_length "01.04"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "None" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1791210"

  current_file="$OUTPUT_DIR/${M_FILE}_silence_2.mp3"
  check_current_mp3_length "03.00"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "None" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "5119608"

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

