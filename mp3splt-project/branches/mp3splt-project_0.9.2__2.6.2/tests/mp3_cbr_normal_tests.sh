#!/bin/bash

. ./utils.sh || exit 1

#normal mode functional tests

function test_normal_cbr
{
  tags_version=$1

  remove_output_dir

  M_FILE="Merci_Bonsoir__Je_veux_Only_love"

  test_name="cbr normal"

  tags_option="-T $tags_version"
  if [[ $tags_version -eq 0 ]];then
    tags_option=""
    tags_version=2
  elif [[ $tags_version -eq -1 ]];then
    tags_option="-n"
    test_name="cbr no tags"
  else
    test_name="cbr id3v$tags_version"
  fi

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - 128 Kb/s - Total time: 3m.43s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_00m_30s__02m_04s_50h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_04s_50h__03m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_00s__03m_43s_81h.mp3\" created
 file split"
  mp3splt_args="$tags_option -d $OUTPUT_DIR $CBR_MP3_FILE 0.30 2.04.50 3.0 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_00m_30s__02m_04s_50h.mp3" 
  check_current_mp3_length "01.34"
  check_current_file_has_no_xing

  if [[ $tags_version -eq -1 ]];then
    check_current_mp3_no_tags
    check_current_file_size "1511758"
  else
    check_all_mp3_tags_with_version $tags_version "Merci Bonsoir" "Merci album" "Je veux (only love)"\
    "2009" "Rock" "17" "1" "http://www.jamendo.com"

    if [[ $tags_version -eq 2 ]];then
      check_current_file_size "1512014"
    else
      check_current_file_size "1511886"
      check_current_md5sum "e4efe99b13b899caf6b7293b1dc8cff6"
    fi
  fi

  current_file="$OUTPUT_DIR/${M_FILE}_02m_04s_50h__03m_00s.mp3" 
  check_current_mp3_length "00.55"
  check_current_file_has_no_xing

  if [[ $tags_version -eq -1 ]];then
    check_current_mp3_no_tags
    check_current_file_size "888581"
  else
    check_all_mp3_tags_with_version $tags_version "Merci Bonsoir" "Merci album" "Je veux (only love)"\
    "2009" "Rock" "17" "2" "http://www.jamendo.com"

    if [[ $tags_version -eq 2 ]];then
      check_current_file_size "888837"
    else
      check_current_file_size "888709"
      check_current_md5sum "464db5407d6c451e20ba7d02c9cd6616"
    fi
  fi

  current_file="$OUTPUT_DIR/${M_FILE}_03m_00s__03m_43s_81h.mp3"
  check_current_mp3_length "00.43"
  check_current_file_has_no_xing

  if [[ $tags_version -eq -1 ]];then
    check_current_mp3_no_tags
    check_current_file_size "700082"
  else
    check_all_mp3_tags_with_version $tags_version "Merci Bonsoir" "Merci album" "Je veux (only love)"\
    "2009" "Rock" "17" "3" "http://www.jamendo.com"

    if [[ $tags_version -eq 2 ]];then
      check_current_file_size "700338"
    else
      check_current_file_size "700210"
      check_current_md5sum "e9de7fb2531f1a37a8748480b50e3401"
    fi
  fi

  print_ok
  echo
}

function test_normal_cbr_original_tags_v2 { test_normal_cbr 0; }
function test_normal_cbr_no_tags { test_normal_cbr -1; }
function test_normal_cbr_id3v1 { test_normal_cbr 1; }
function test_normal_cbr_id3v2 { test_normal_cbr 2; }

function test_normal_cbr_pretend
{
  remove_output_dir

  test_name="cbr normal & pretend"
  M_FILE="Merci_Bonsoir__Je_veux_Only_love"

  expected=" Pretending to split file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - 128 Kb/s - Total time: 3m.43s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_00m_30s__02m_04s_50h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_04s_50h__03m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_00s__03m_43s_81h.mp3\" created
 file split"
  mp3splt_args="-P -d $OUTPUT_DIR $CBR_MP3_FILE 0.30 2.04.50 3.0 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_is_empty

  print_ok
  echo
}

function test_normal_cbr_pretend_with_wrong_extension
{
  remove_output_dir

  test_name="cbr normal & wrong extension"
  M_FILE="Merci_Bonsoir__Je_veux_Only_love"

  cp $CBR_MP3_FILE ${CBR_MP3_FILE}.ogg

  expected=" Pretending to split file 'songs/${M_FILE}.mp3.ogg' ...
 warning: detected as .mp3 but extension does not match
 info: file matches the plugin 'mp3 (libmad)'
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - 128 Kb/s - Total time: 3m.43s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}.mp3_00m_30s__02m_04s_50h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}.mp3_02m_04s_50h__03m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}.mp3_03m_00s__03m_43s_81h.mp3\" created
 file split"
  mp3splt_args="-P -d $OUTPUT_DIR ${CBR_MP3_FILE}.ogg 0.30 2.04.50 3.0 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_is_empty

  rm -f ${CBR_MP3_FILE}.ogg

  print_ok
  echo
}

function test_normal_cbr_cue_export
{
  remove_output_dir

  M_FILE="Merci_Bonsoir__Je_veux_Only_love"

  test_name="cbr normal & cue export"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - 128 Kb/s - Total time: 3m.43s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_00m_30s__02m_04s_50h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_04s_50h__03m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_00s__03m_43s_81h.mp3\" created
 file split
 CUE file 'output/output_out.cue' created."
  mp3splt_args="-E output/out.cue -d $OUTPUT_DIR $CBR_MP3_FILE 0.30 2.04.50 3.0 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_number_of_files 4

  check_file_content "output/output_out.cue" 'TITLE "Merci album"
PERFORMER "Merci Bonsoir"
FILE "songs/Merci_Bonsoir__Je_veux_Only_love.mp3" MP3
  TRACK 01 AUDIO
    TITLE "Je veux (only love)"
    PERFORMER "Merci Bonsoir"
    REM ALBUM "Merci album"
    REM GENRE "Rock"
    REM DATE "2009"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "Merci_Bonsoir__Je_veux_Only_love_00m_30s__02m_04s_50h"
    INDEX 01 00:30:00
  TRACK 02 AUDIO
    TITLE "Je veux (only love)"
    PERFORMER "Merci Bonsoir"
    REM ALBUM "Merci album"
    REM GENRE "Rock"
    REM DATE "2009"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "Merci_Bonsoir__Je_veux_Only_love_02m_04s_50h__03m_00s"
    INDEX 01 02:04:38
  TRACK 03 AUDIO
    TITLE "Je veux (only love)"
    PERFORMER "Merci Bonsoir"
    REM ALBUM "Merci album"
    REM GENRE "Rock"
    REM DATE "2009"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "Merci_Bonsoir__Je_veux_Only_love_03m_00s__03m_43s_81h"
    INDEX 01 03:00:00'

  current_file="$OUTPUT_DIR/${M_FILE}_00m_30s__02m_04s_50h.mp3" 
  check_current_mp3_length "01.34"
  check_current_file_has_no_xing

  check_all_mp3_tags_with_version 2 "Merci Bonsoir" "Merci album" "Je veux (only love)"\
  "2009" "Rock" "17" "1" "http://www.jamendo.com"
  check_current_file_size "1512014"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_04s_50h__03m_00s.mp3" 
  check_current_mp3_length "00.55"
  check_current_file_has_no_xing

  check_all_mp3_tags_with_version 2 "Merci Bonsoir" "Merci album" "Je veux (only love)"\
  "2009" "Rock" "17" "2" "http://www.jamendo.com"
  check_current_file_size "888837"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_00s__03m_43s_81h.mp3"
  check_current_mp3_length "00.43"
  check_current_file_has_no_xing

  check_all_mp3_tags_with_version 2 "Merci Bonsoir" "Merci album" "Je veux (only love)"\
  "2009" "Rock" "17" "3" "http://www.jamendo.com"
  check_current_file_size "700338"

  print_ok
  echo
}

function test_normal_cbr_pretend_and_cue_export
{
  remove_output_dir

  test_name="cbr normal & pretend & cue export"
  M_FILE="Merci_Bonsoir__Je_veux_Only_love"

  expected=" Pretending to split file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - 128 Kb/s - Total time: 3m.43s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_00m_30s__02m_04s_50h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_04s_50h__03m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_00s__03m_43s_81h.mp3\" created
 file split
 CUE file 'output/output_out.cue' created."
  mp3splt_args="-P -E output/out.cue -d $OUTPUT_DIR $CBR_MP3_FILE 0.30 2.04.50 3.0 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_number_of_files 1

  check_file_content "output/output_out.cue" 'TITLE "Merci album"
PERFORMER "Merci Bonsoir"
FILE "songs/Merci_Bonsoir__Je_veux_Only_love.mp3" MP3
  TRACK 01 AUDIO
    TITLE "Je veux (only love)"
    PERFORMER "Merci Bonsoir"
    REM ALBUM "Merci album"
    REM GENRE "Rock"
    REM DATE "2009"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "Merci_Bonsoir__Je_veux_Only_love_00m_30s__02m_04s_50h"
    INDEX 01 00:30:00
  TRACK 02 AUDIO
    TITLE "Je veux (only love)"
    PERFORMER "Merci Bonsoir"
    REM ALBUM "Merci album"
    REM GENRE "Rock"
    REM DATE "2009"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "Merci_Bonsoir__Je_veux_Only_love_02m_04s_50h__03m_00s"
    INDEX 01 02:04:38
  TRACK 03 AUDIO
    TITLE "Je veux (only love)"
    PERFORMER "Merci Bonsoir"
    REM ALBUM "Merci album"
    REM GENRE "Rock"
    REM DATE "2009"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "Merci_Bonsoir__Je_veux_Only_love_03m_00s__03m_43s_81h"
    INDEX 01 03:00:00'

  print_ok
  echo
}

function test_normal_cbr_cue_and_overlap
{
  remove_output_dir

  test_name="cbr cue & overlap"
  M_FILE="Merci_Bonsoir__Je_veux_Only_love"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - 128 Kb/s - Total time: 3m.43s
 info: starting normal split
 info: overlapping split files with 0.30.0
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_30s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_43s_81h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_30s__03m_43s_81h.mp3\" created
 file split
 CUE file 'output/output_out.cue' created."
  mp3splt_args="-O 0.30 -E output/out.cue -d $OUTPUT_DIR $CBR_MP3_FILE 1.0 2.0.2 3.30 EOF"
  run_check_output "$mp3splt_args" "$expected"

  check_file_content "output/output_out.cue" 'TITLE "Merci album"
PERFORMER "Merci Bonsoir"
FILE "songs/Merci_Bonsoir__Je_veux_Only_love.mp3" MP3
  TRACK 01 AUDIO
    TITLE "Je veux (only love)"
    PERFORMER "Merci Bonsoir"
    REM ALBUM "Merci album"
    REM GENRE "Rock"
    REM DATE "2009"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "Merci_Bonsoir__Je_veux_Only_love_01m_00s__02m_30s_20h"
    INDEX 01 01:00:00
  TRACK 02 AUDIO
    TITLE "Je veux (only love)"
    PERFORMER "Merci Bonsoir"
    REM ALBUM "Merci album"
    REM GENRE "Rock"
    REM DATE "2009"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "Merci_Bonsoir__Je_veux_Only_love_02m_00s_20h__03m_43s_81h"
    INDEX 01 02:00:15
  TRACK 03 AUDIO
    TITLE "Je veux (only love)"
    PERFORMER "Merci Bonsoir"
    REM ALBUM "Merci album"
    REM GENRE "Rock"
    REM DATE "2009"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "Merci_Bonsoir__Je_veux_Only_love_03m_30s__03m_43s_81h"
    INDEX 01 03:30:00'

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_30s_20h.mp3"
  check_current_mp3_length "01.30"
  check_current_file_size "1443051"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_43s_81h.mp3"
  check_current_mp3_length "01.43"
  check_current_file_size "1657882"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_30s__03m_43s_81h.mp3"
  check_current_mp3_length "00.13"
  check_current_file_size "220939"

  print_ok
  echo
}

function test_normal_cbr_overlap_splitpoints
{
  remove_output_dir

  test_name="cbr overlap splitpoints"
  M_FILE="Merci_Bonsoir__Je_veux_Only_love"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - 128 Kb/s - Total time: 3m.43s
 info: starting normal split
 info: overlapping split files with 0.30.0
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_30s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_43s_81h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_30s__03m_43s_81h.mp3\" created
 file split"
  mp3splt_args="-O 0.30 -d $OUTPUT_DIR $CBR_MP3_FILE 1.0 2.0.2 3.30 EOF"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_30s_20h.mp3"
  check_current_mp3_length "01.30"
  check_current_file_size "1443051"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_43s_81h.mp3"
  check_current_mp3_length "01.43"
  check_current_file_size "1657882"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_30s__03m_43s_81h.mp3"
  check_current_mp3_length "00.13"
  check_current_file_size "220939"

  print_ok
  echo
}

function test_normal_cbr_stdin
{
  remove_output_dir

  if [[ -z $no_tags_file ]];then
    test_name="cbr stdin"
    M_FILE="Merci_Bonsoir__Je_veux_Only_love"
  fi

  expected=" Processing file '-' ...
 info: file matches the plugin 'mp3 (libmad)'
 warning: stdin '-' is supposed to be mp3 stream.
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - NS - 128 Kb/s - Total time: 0m.00s
 info: starting normal split
   File \"$OUTPUT_DIR/-_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/-_02m_00s_20h__03m_30s.mp3\" created
   File \"$OUTPUT_DIR/-_03m_30s__EOF.mp3\" created
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR - 1.0 2.0.2 3.30 EOF"
  run_custom_check_output "cat songs/${M_FILE}.mp3 | $MP3SPLT" "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/-_01m_00s__02m_00s_20h.mp3"
  check_current_mp3_length "01.00"
  check_current_mp3_no_tags
  check_current_file_size "963396"

  current_file="$OUTPUT_DIR/-_02m_00s_20h__03m_30s.mp3"
  check_current_mp3_length "01.29"
  check_current_mp3_no_tags
  check_current_file_size "1436944"

  current_file="$OUTPUT_DIR/-_03m_30s__EOF.mp3"
  check_current_mp3_length "00.13"
  check_current_mp3_no_tags
  check_current_file_size "216503"

  print_ok
  echo
}

function test_normal_cbr_stdout
{
  remove_output_dir

  test_name="cbr stdout"
  M_FILE="Merci_Bonsoir__Je_veux_Only_love"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - 128 Kb/s - Total time: 3m.43s
 info: starting normal split
   File \"-\" created
 file split"
  mp3splt_args="-o - $CBR_MP3_FILE 1.0 2.30.2"
  run_custom_check_output "$MP3SPLT $mp3splt_args > $OUTPUT_DIR/stdout.mp3" "" "$expected"

  current_file="$OUTPUT_DIR/stdout.mp3"
  check_current_mp3_length "01.30"
  check_current_file_size "1443051"

  print_ok
  echo
}

function test_normal_cbr_stdout_multiple_splitpoints
{
  remove_output_dir

  test_name="cbr stdout & splitpoints > 2"
  M_FILE="Merci_Bonsoir__Je_veux_Only_love"

  expected=" Warning: multiple splitpoints with stdout !
 Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - 128 Kb/s - Total time: 3m.43s
 info: starting normal split
   File \"-\" created
   File \"-\" created
 file split"
  mp3splt_args="-o - $CBR_MP3_FILE 1.0 2.30.2 EOF"
  run_custom_check_output "$MP3SPLT $mp3splt_args > $OUTPUT_DIR/stdout.mp3" "" "$expected"

  current_file="$OUTPUT_DIR/stdout.mp3"
  check_current_mp3_length "02.43"
  check_current_file_size "2621116"

  print_ok
  echo
}

function test_normal_cbr_bit_reservoir
{
  remove_output_dir

  M_FILE="Merci_Bonsoir__Je_veux_Only_love"

  test_name="cbr normal & bit reservoir"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 warning: bit reservoir handling for gapless playback is still experimental
 info: file matches the plugin 'mp3 (libmad)'
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 3m.43s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_00m_30s__02m_04s_50h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_04s_50h__03m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_00s__03m_43s_81h.mp3\" created
 Processed 8569 frames - Sync errors: 0
 file split (EOF)"
  mp3splt_args=" -f -b -d $OUTPUT_DIR $CBR_MP3_FILE 0.30 2.04.50 3.0 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_number_of_files 3

  current_file="$OUTPUT_DIR/${M_FILE}_00m_30s__02m_04s_50h.mp3" 
  check_current_mp3_length "01.34"
  check_current_file_has_info

  check_all_mp3_tags_with_version 2 "Merci Bonsoir" "Merci album" "Je veux (only love)"\
  "2009" "Rock" "17" "1" "http://www.jamendo.com"
  check_current_file_size "1514366"
  check_current_md5sum "48304668edf0a83f4caec3648ac78208"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_04s_50h__03m_00s.mp3" 
  check_current_mp3_length "00.55"
  check_current_file_has_info
  check_current_md5sum "7b61d47fd3378a78f18496828e6d0546"

  check_all_mp3_tags_with_version 2 "Merci Bonsoir" "Merci album" "Je veux (only love)"\
  "2009" "Rock" "17" "2" "http://www.jamendo.com"
  check_current_file_size "889987"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_00s__03m_43s_81h.mp3"
  check_current_mp3_length "00.43"
  check_current_file_has_info
  check_current_md5sum "30a39e9a958ac7a3c5a4ccca16b86708"

  check_all_mp3_tags_with_version 2 "Merci Bonsoir" "Merci album" "Je veux (only love)"\
  "2009" "Rock" "17" "3" "http://www.jamendo.com"
  check_current_file_size "702193"

  print_ok
  echo
}

function run_normal_cbr_tests
{
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
export LC_ALL="C"
start_date=$(date +%s)

run_normal_cbr_tests

p_failed_tests

end_date=$(date +%s)

p_time_diff_cyan $start_date $end_date "\t"
echo -e '\n'

exit 0

