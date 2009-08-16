#!/bin/bash

. ./constants_variables.sh
. ./utils.sh

#normal mode functional tests

function test_normal_cbr
{
#  rm -rf $OUTPUT_DIR/*
#
#  M_FILE="La_Verue__Today"
#
#  test_name="vbr id3v1 and id3v2"
#
#  expected=" Processing file 'songs/${M_FILE}.mp3' ...
# info: file matches the plugin 'mp3 (libmad)'
# info: found Xing or Info header. Switching to frame mode... 
# info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
# info: starting normal split
#   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3\" created
#   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3\" created
#   File \"$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3\" created
# Processed 9402 frames - Sync errors: 0
# file split (EOF)"
#  mp3splt_args="-T 12 -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
#  run_check_output "$mp3splt_args" "$expected"
#
#  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3" 
#  check_current_mp3_length "01.00"
#  check_current_file_has_xing
#  check_current_file_size "1366678"
#  check_all_mp3_tags_with_version "1 2" "La Verue" "Riez Noir" "Today"\
#  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
#
#  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3" 
#  check_current_mp3_length "01.04"
#  check_current_file_has_xing
#  check_current_file_size "1522092"
#  check_all_mp3_tags_with_version "1 2" "La Verue" "Riez Noir" "Today"\
#  "2007" "Rock" "17" "2" "http://www.jamendo.com/"
#
#  current_file="$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3" 
#  check_current_mp3_length "01.00"
#  check_current_file_has_xing
#  check_current_file_size "1399515"
#  check_all_mp3_tags_with_version "1 2" "La Verue" "Riez Noir" "Today"\
#  "2007" "Rock" "17" "3" "http://www.jamendo.com/"
#
  p_green "OK"
  echo
}

function run_normal_cbr_mode_tests
{
  date
  echo
  p_blue " NORMAL CBR mp3 tests ..."
  echo

  normal_test_functions=$(declare -F | grep " test_normal_cbr_" | awk '{ print $3 }')

  for test_func in $normal_test_functions;do
    eval $test_func
  done

  p_blue " NORMAL CBR tests DONE."
  echo
}

#main
export LANGUAGE="en"
echo
start_date=$(date "+%D %H:%M:%S")

run_normal_cbr_mode_tests

echo
end_date=$(date "+%D %H:%M:%S")
p_cyan "\tStart date : $start_date"
echo
p_cyan "\tEnd date : $end_date"
echo -e '\n'

exit 0

