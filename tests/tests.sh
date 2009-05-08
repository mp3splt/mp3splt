#!/bin/bash

. ./constants_variables.sh
. ./utils.sh

#normal mode functional tests

function test_normal
{
  current_tags_version=$1

  test_name="normal"
  M_FILE="La_Verue__Today"

  tags_option="-$current_tags_version"
  if [[ $current_tags_version -eq 0 ]];then
    tags_option=""
    current_tags_version=2
  elif [[ $current_tags_version -eq -1 ]];then
    tags_option="-n"
    test_name="$test_name & no tags"
  else
    test_name="$test_name & id3v$current_tags_version"
  fi

  #eyeD3 prints Rock (id 17) and not Rock (17)
  id_str=""
  if [[ $current_tags_version -eq 2 ]];then
    id_str="id "
  fi

  expected=" Processing file 'songs/${M_FILE}.mp3' ...\n\
 info: frame mode enabled\n\
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s\n\
 info: starting normal split\n\
   File \"output/${M_FILE}_01m_00s__02m_00s.mp3\" created\n\
   File \"output/${M_FILE}_02m_00s__03m_05s.mp3\" created\n\
   File \"output/${M_FILE}_03m_05s__04m_05s_58h.mp3\" created\n\
 Processed 9402 frames - Sync errors: 0\n\
 file split (EOF)"
  run_check_output "$tags_option -d $OUTPUT_DIR $MP3_FILE 1.0 2.0 3.5 EOF" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s.mp3" 
  check_current_mp3_length "1.00"
  if [[ $current_tags_version -eq -1 ]];then
    check_current_mp3_no_tags
  else
    check_all_current_mp3_tags "La Verue" "Riez Noir" "Today"\
      "2007" "Rock (${id_str}17)" "1" "http://www.jamendo.com/"
  fi

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s__03m_05s.mp3" 
  check_current_mp3_length "1.05"
  if [[ $current_tags_version -eq -1 ]];then
    check_current_mp3_no_tags
  else
    check_all_current_mp3_tags "La Verue" "Riez Noir" "Today"\
      "2007" "Rock (${id_str}17)" "2" "http://www.jamendo.com/"
  fi

  current_file="$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3" 
  check_current_mp3_length "1.00"
  if [[ $current_tags_version -eq -1 ]];then
    check_current_mp3_no_tags
  else
    check_all_current_mp3_tags "La Verue" "Riez Noir" "Today"\
      "2007" "Rock (${id_str}17)" "3" "http://www.jamendo.com/"
  fi

  p_green "PASSED"
  echo
}

function test_normal_original_tags { test_normal 0; }
function test_normal_no_tags { test_normal -1; }
function test_normal_id3v1 { test_normal 1; }
function test_normal_id3v2 { test_normal 2; }

function run_normal_mode_tests
{
  date
  echo
  p_blue " NORMAL tests ..."
  echo

  test_normal_original_tags
  test_normal_id3v1
  test_normal_id3v2
  test_normal_no_tags

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

