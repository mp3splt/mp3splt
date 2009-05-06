#!/bin/bash

. ./constants_variables.sh
. ./utils.sh

#normal mode functional tests

function test_normal_with_splitpoints
{
  test_name="normal split with splitpoints"

  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...\n\
 M3U file 'output/playlist.m3u' will be created.
 info: frame mode enabled\n\
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s\n\
 info: starting normal split\n\
   File \"output/${M_FILE}_01m_00s__02m_00s.mp3\" created\n\
   File \"output/${M_FILE}_02m_00s__03m_05s.mp3\" created\n\
 Processed 7083 frames - Sync errors: 0\n\
 file split"
  rm -f $OUTPUT_DIR/playlist.m3u
  run_check_output "-m playlist.m3u -d $OUTPUT_DIR $MP3_FILE 1.0 2.0 3.5"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s.mp3" 
  current_tags_version=1
  check_current_file_size "1362063"

  check_current_mp3_tags "artist" "aa"
  check_current_mp3_tags "album" "bb"
  check_current_mp3_tags "title" "tt"
  check_current_mp3_tags "year" "yy"
  check_current_mp3_tags "tracknumber" "nn"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s__03m_05s.mp3" 
  check_current_file_size "1525825"

  expected="${M_FILE}_01m_00s__02m_00s.mp3
${M_FILE}_02m_00s__03m_05s.mp3"
  check_file_content "$OUTPUT_DIR/playlist.m3u"

  p_green "PASSED"
  echo
}

function run_normal_mode_tests
{
  echo
  p_blue " NORMAL tests ..."
  echo
  test_normal_with_splitpoints
  p_blue " NORMAL tests DONE."
  echo
}

#main

run_normal_mode_tests
echo

