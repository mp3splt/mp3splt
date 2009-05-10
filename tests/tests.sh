#!/bin/bash

. ./constants_variables.sh
. ./utils.sh

#normal mode functional tests

function test_normal
{
  current_tags_version=$1

  rm -rf $OUTPUT_DIR/*

  test_name="normal"
  M_FILE="La_Verue__Today"

  tags_option="-$current_tags_version"
  if [[ $current_tags_version -eq 0 ]];then
    tags_option=""
    current_tags_version=2
  elif [[ $current_tags_version -eq -1 ]];then
    tags_option="-n"
    test_name="no tags"
  else
    test_name="id3v$current_tags_version"
  fi

  #eyeD3 prints Rock (id 17) and not Rock (17)
  id_str=""
  if [[ $current_tags_version -eq 2 ]];then
    id_str="id "
  fi

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: frame mode enabled
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  command_to_run="$tags_option -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$command_to_run" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing

  if [[ $current_tags_version -eq -1 ]];then
    check_current_mp3_no_tags
    check_current_file_size "1365812"
  else
    check_all_current_mp3_tags "La Verue" "Riez Noir" "Today"\
      "2007" "Rock (${id_str}17)" "1" "http://www.jamendo.com/"

    if [[ $current_tags_version -eq 2 ]];then
      check_current_file_size "1366028"
    else
      check_current_file_size "1365940"
    fi
  fi

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3" 
  check_current_mp3_length "01.04"
  check_current_file_has_xing

  if [[ $current_tags_version -eq -1 ]];then
    check_current_mp3_no_tags
    check_current_file_size "1521644"
  else
    check_all_current_mp3_tags "La Verue" "Riez Noir" "Today"\
      "2007" "Rock (${id_str}17)" "2" "http://www.jamendo.com/"

    if [[ $current_tags_version -eq 2 ]];then
      check_current_file_size "1521860"
    else
      check_current_file_size "1521772"
    fi
  fi

  current_file="$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing

  if [[ $current_tags_version -eq -1 ]];then
    check_current_mp3_no_tags
    check_current_file_size "1399797"
  else
    check_all_current_mp3_tags "La Verue" "Riez Noir" "Today"\
      "2007" "Rock (${id_str}17)" "3" "http://www.jamendo.com/"

    if [[ $current_tags_version -eq 2 ]];then
      check_current_file_size "1400013"
    else
      check_current_file_size "1399925"
    fi
  fi

  p_green "OK"
  echo
}

function test_normal_original_tags_v2 { test_normal 0; }
function test_normal_no_tags { test_normal -1; }
function test_normal_id3v1 { test_normal 1; }
function test_normal_id3v2 { test_normal 2; }

function test_normal_no_xing
{
  rm -f $OUTPUT_DIR/*

  test_name="no xing"
  M_FILE="La_Verue__Today"

  command_to_run="-x -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$command_to_run" ""

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3" 
  check_current_mp3_length "01.08"
  check_current_file_has_no_xing
  check_current_file_size "1365611"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3" 
  check_current_mp3_length "01.16"
  check_current_file_has_no_xing
  check_current_file_size "1521443"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3" 
  check_current_mp3_length "00.58"
  check_current_file_has_no_xing
  check_current_file_size "1399596"

  p_green "OK"
  echo
}

function test_normal_m3u
{
  rm -rf $OUTPUT_DIR/*

  test_name="m3u"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 M3U file '$OUTPUT_DIR/m3u/playlist.m3u' will be created.
 info: frame mode enabled
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/m3u/${M_FILE}_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/m3u/${M_FILE}_02m_00s_20h__03m_05s.mp3\" created
   File \"$OUTPUT_DIR/m3u/${M_FILE}_03m_05s__04m_05s_58h.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  command_to_run="-m playlist.m3u -d $OUTPUT_DIR/m3u $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$command_to_run" "$expected"

  expected="La_Verue__Today_01m_00s__02m_00s_20h.mp3
La_Verue__Today_02m_00s_20h__03m_05s.mp3
La_Verue__Today_03m_05s__04m_05s_58h.mp3"
  check_file_content "$OUTPUT_DIR/m3u/playlist.m3u" "$expected"

  p_green "OK"
  echo
}

function run_normal_mode_tests
{
  date
  echo
  p_blue " NORMAL tests ..."
  echo

  normal_tests_to_run="\
original_tags_v2 \
id3v1 \
id3v2 \
no_tags \
no_xing \
m3u"

  for t in $normal_tests_to_run;do
    eval "test_normal_"$t
  done

  p_blue " NORMAL tests DONE."
  echo
}

#main
echo
start_date=$(date "+%D %H:%M:%S")

run_normal_mode_tests

echo
end_date=$(date "+%D %H:%M:%S")
p_cyan "\tStart date : $start_date"
echo
p_cyan "\tEnd date : $end_date"
echo -e '\n'

exit 0

