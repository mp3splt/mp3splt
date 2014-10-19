#!/bin/bash

. ./utils.sh || exit 1

#normal mode functional tests

function _test_normal_vbr
{
  local tags_version=$1
  local M_FILE=$2
  local extra=$3

  remove_output_dir

  local version_of_tags=$tags_version;
  if [[ $version_of_tags -eq 22 ]];then
    version_of_tags=2
  fi

  if [[ $tags_version -eq -1 ]];then
    tags_option="-n"
    test_name="vbr $extra no tags"
  else
    test_name="vbr id3v$version_of_tags"
    tags_option="-T $version_of_tags"
  fi

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  mp3splt_args="$tags_option -d $OUTPUT_DIR $SONGS_DIR/${M_FILE}.mp3 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing

  if [[ $tags_version -eq -1 ]];then
    check_current_mp3_no_tags
    check_current_file_size "1366334"
    check_current_md5sum "87d6b0d39028d774becd734968cfbb6e"
  else
    check_all_mp3_tags_with_version $version_of_tags "La Verue" "Riez Noir" "Today"\
    "2007" "Rock" "17" "1" "http://www.jamendo.com/"

    if [[ $tags_version -eq 2 ]];then
      check_current_file_size "1412518"
      check_current_md5sum "648d6987147771470b2780184269ebdd"
    elif [[ $tags_version -eq 22 ]];then
      check_current_file_size "1366550"
      check_current_md5sum "cf0f394db314fdcb78076fec4008f142"
    else
      check_current_file_size "1366462"
      check_current_md5sum "4833e153053bd37001da0b267a4c13a2"
    fi
  fi

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3" 
  check_current_mp3_length "01.04"
  check_current_file_has_xing

  if [[ $tags_version -eq -1 ]];then
    check_current_mp3_no_tags
    check_current_file_size "1521748"
    check_current_md5sum "d9a5b28bf0f5f5abe356290f5903beed"
  else
    check_all_mp3_tags_with_version $version_of_tags "La Verue" "Riez Noir"\
    "Today" "2007" "Rock" "17" "2" "http://www.jamendo.com/"

    if [[ $tags_version -eq 2 ]];then
      check_current_file_size "1567932"
      check_current_md5sum "949ab2b9ba874ea036bce7d88240b655"
    elif [[ $tags_version -eq 22 ]];then
      check_current_file_size "1521964"
      check_current_md5sum "6df5d42e1b25abb6474fe13a485e9363"
    else
      check_current_file_size "1521876"
      check_current_md5sum "a529c53717c6856921bc5d38a478b2da"
    fi
  fi

  current_file="$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing

  if [[ $tags_version -eq -1 ]];then
    check_current_mp3_no_tags
    check_current_file_size "1399171"
    check_current_md5sum "6a2c5397fd311685edc5f4d389aed918"
  else
    check_all_mp3_tags_with_version $version_of_tags "La Verue" "Riez Noir" "Today"\
      "2007" "Rock" "17" "3" "http://www.jamendo.com/"

    if [[ $tags_version -eq 2 ]];then
      check_current_file_size "1445355"
      check_current_md5sum "823569e0b0a009c66647aa757448e7e9"
    elif [[ $tags_version -eq 22 ]];then
      check_current_file_size "1399387"
      check_current_md5sum "979faf56a9f9110fd04e2eed99201658"
    else
      check_current_file_size "1399299"
      check_current_md5sum "5d70dfc063a1d54a9eaa092a1e68cc9c"
    fi
  fi

  print_ok
  echo
}

function test_normal_vbr_no_tags { _test_normal_vbr -1 "La_Verue__Today"; }
function test_normal_vbr_id3v1 { _test_normal_vbr 1 "La_Verue__Today"; }
function test_normal_vbr_id3v2 { _test_normal_vbr 2 "La_Verue__Today"; }

function test_normal_vbr_input_with_only_id3v1_and_output_no_tags
{
  MD5SUM_DISABLED=1;
  _test_normal_vbr -1 "La_Verue__Today_id3v1_only" "input with only id3v1 and output";
  MD5SUM_DISABLED=0;
}

function test_normal_vbr_input_with_only_id3v1_and_output_id3v1
{
  MD5SUM_DISABLED=1;
  _test_normal_vbr 1 "La_Verue__Today_id3v1_only" "input with only id3v1 and output";
  MD5SUM_DISABLED=0;
}

function test_normal_vbr_input_with_only_id3v1_and_output_id3v2
{
  MD5SUM_DISABLED=1;
  _test_normal_vbr 22 "La_Verue__Today_id3v1_only" "input with only id3v1 and output";
  MD5SUM_DISABLED=0;
}

function test_normal_vbr_pretend
{
  remove_output_dir

  test_name="vbr pretend"
  M_FILE="La_Verue__Today"

  expected=" Pretending to split file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  mp3splt_args="-P -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_is_empty

  print_ok
  echo
}

function test_normal_vbr_cue_export
{
  remove_output_dir

  test_name="vbr & cue export"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)
 CUE file 'output/output_out.cue' created."
  mp3splt_args="-T 2 -E output/out.cue -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_number_of_files 4

  check_file_content "output/output_out.cue" 'TITLE "Riez Noir"
PERFORMER "La Verue"
FILE "songs/La_Verue__Today.mp3" MP3
  TRACK 01 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    REM ALBUM "Riez Noir"
    REM GENRE "Rock"
    REM DATE "2007"
    REM COMMENT "http://www.jamendo.com/"
    REM NAME "La_Verue__Today_01m_00s__02m_00s_20h"
    INDEX 01 01:00:00
  TRACK 02 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    REM ALBUM "Riez Noir"
    REM GENRE "Rock"
    REM DATE "2007"
    REM COMMENT "http://www.jamendo.com/"
    REM NAME "La_Verue__Today_02m_00s_20h__03m_05s"
    INDEX 01 02:00:15
  TRACK 03 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    REM ALBUM "Riez Noir"
    REM GENRE "Rock"
    REM DATE "2007"
    REM COMMENT "http://www.jamendo.com/"
    REM NAME "La_Verue__Today_03m_05s__04m_05s_58h"
    INDEX 01 03:05:00'

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1412518"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3" 
  check_current_mp3_length "01.04"
  check_current_file_has_xing
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir"\
  "Today" "2007" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "1567932"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "3" "http://www.jamendo.com/"
  check_current_file_size "1445355"

  print_ok
  echo
}

function test_normal_vbr_pretend_and_cue_export
{
  remove_output_dir

  test_name="vbr pretend & cue export"
  M_FILE="La_Verue__Today"

  expected=" Pretending to split file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)
 CUE file 'output/output_out.cue' created."
  mp3splt_args="-E output/out.cue -P -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"
 
  check_output_directory_number_of_files 1

  check_file_content "output/output_out.cue" 'TITLE "Riez Noir"
PERFORMER "La Verue"
FILE "songs/La_Verue__Today.mp3" MP3
  TRACK 01 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    REM ALBUM "Riez Noir"
    REM GENRE "Rock"
    REM DATE "2007"
    REM COMMENT "http://www.jamendo.com/"
    REM NAME "La_Verue__Today_01m_00s__02m_00s_20h"
    INDEX 01 01:00:00
  TRACK 02 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    REM ALBUM "Riez Noir"
    REM GENRE "Rock"
    REM DATE "2007"
    REM COMMENT "http://www.jamendo.com/"
    REM NAME "La_Verue__Today_02m_00s_20h__03m_05s"
    INDEX 01 02:00:15
  TRACK 03 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    REM ALBUM "Riez Noir"
    REM GENRE "Rock"
    REM DATE "2007"
    REM COMMENT "http://www.jamendo.com/"
    REM NAME "La_Verue__Today_03m_05s__04m_05s_58h"
    INDEX 01 03:05:00'

  print_ok
  echo
}

function test_normal_vbr_overlap_split
{
  remove_output_dir

  test_name="vbr overlap splitpoints"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
 info: overlapping split files with 0.30.0
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_30s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__04m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_30s__04m_05s_58h.mp3\" created
 file split (EOF)"
  mp3splt_args="-T 2 -O 0.30 -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.30 EOF"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_30s_20h.mp3"
  check_current_mp3_length "01.30"
  check_current_file_size "2054691"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__04m_00s.mp3"
  check_current_mp3_length "01.59"
  check_current_file_size "2869690"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_30s__04m_05s_58h.mp3"
  check_current_mp3_length "00.35"
  check_current_file_size "855248"

  print_ok
  echo
}

function test_normal_vbr_overlap_and_cue_export
{
  remove_output_dir

  test_name="vbr overlap & cue export"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
 info: overlapping split files with 0.30.0
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_30s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__04m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_30s__04m_05s_58h.mp3\" created
 file split (EOF)
 CUE file 'output/output_out.cue' created."
  mp3splt_args="-T 2 -E output/out.cue -O 0.30 -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.30 EOF"
  run_check_output "$mp3splt_args" "$expected"

  #TODO: what to output as CUE ?
  check_file_content 'output/output_out.cue' 'TITLE "Riez Noir"
PERFORMER "La Verue"
FILE "songs/La_Verue__Today.mp3" MP3
  TRACK 01 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    REM ALBUM "Riez Noir"
    REM GENRE "Rock"
    REM DATE "2007"
    REM COMMENT "http://www.jamendo.com/"
    REM NAME "La_Verue__Today_01m_00s__02m_30s_20h"
    INDEX 01 01:00:00
  TRACK 02 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    REM ALBUM "Riez Noir"
    REM GENRE "Rock"
    REM DATE "2007"
    REM COMMENT "http://www.jamendo.com/"
    REM NAME "La_Verue__Today_02m_00s_20h__04m_00s"
    INDEX 01 02:00:15
  TRACK 03 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    REM ALBUM "Riez Noir"
    REM GENRE "Rock"
    REM DATE "2007"
    REM COMMENT "http://www.jamendo.com/"
    REM NAME "La_Verue__Today_03m_30s__04m_05s_58h"
    INDEX 01 03:30:00'

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_30s_20h.mp3"
  check_current_mp3_length "01.30"
  check_current_file_size "2054691"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__04m_00s.mp3"
  check_current_mp3_length "01.59"
  check_current_file_size "2869690"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_30s__04m_05s_58h.mp3"
  check_current_mp3_length "00.35"
  check_current_file_size "855248"

  print_ok
  echo
}

function test_normal_vbr_m3u
{
  remove_output_dir

  test_name="vbr m3u"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 M3U file '$OUTPUT_DIR/m3u/playlist.m3u' will be created.
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/m3u/${M_FILE}_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/m3u/${M_FILE}_02m_00s_20h__03m_05s.mp3\" created
   File \"$OUTPUT_DIR/m3u/${M_FILE}_03m_05s__04m_05s_58h.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  mp3splt_args="-m playlist.m3u -d $OUTPUT_DIR/m3u $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  expected="La_Verue__Today_01m_00s__02m_00s_20h.mp3
La_Verue__Today_02m_00s_20h__03m_05s.mp3
La_Verue__Today_03m_05s__04m_05s_58h.mp3"
  check_file_content "$OUTPUT_DIR/m3u/playlist.m3u" "$expected"

  print_ok
  echo
}

function test_normal_vbr_pretend_and_m3u
{
  remove_output_dir

  test_name="vbr pretend & m3u"
  M_FILE="La_Verue__Today"

  expected=" Pretending to split file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 M3U file 'output/output_out.m3u' will be created.
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  mp3splt_args="-m output/out.m3u -P -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"
 
  check_output_directory_number_of_files 0

  print_ok
  echo
}

function test_normal_vbr_default_tags
{
  remove_output_dir

  test_name="vbr normal"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "1 2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1412646"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3" 
  check_current_mp3_length "01.04"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "1 2" "La Verue" "Riez Noir"\
  "Today" "2007" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "1568060"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "1 2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "3" "http://www.jamendo.com/"
  check_current_file_size "1445483"

  print_ok
  echo
}

function test_normal_vbr_original_tags
{
  remove_output_dir

  test_name="vbr original tags %[@o]"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  mp3splt_args="-g %[@o] -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="songs/${M_FILE}.mp3"
  #8/14: we don't support yet the total tracknumber
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "8/14" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3" 
  check_all_mp3_tags_with_version "1 2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "8" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3" 
  check_all_mp3_tags_with_version "1 2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "8" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3" 
  check_all_mp3_tags_with_version "1 2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "8" "http://www.jamendo.com/"

  print_ok
  echo
}

function test_normal_vbr_id3v1_and_id3v2
{
  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="vbr id3v1 and id3v2"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  mp3splt_args="-T 12 -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing
  check_current_file_size "1412646"
  check_all_mp3_tags_with_version "1 2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3" 
  check_current_mp3_length "01.04"
  check_current_file_has_xing
  check_current_file_size "1568060"
  check_all_mp3_tags_with_version "1 2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "2" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing
  check_current_file_size "1445483"
  check_all_mp3_tags_with_version "1 2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "3" "http://www.jamendo.com/"

  print_ok
  echo
}

function test_normal_vbr_no_input_tags
{
  remove_output_dir

  test_name="vbr normal & no input tags"
  M_FILE="La_Verue__Today__no_tags"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR $NO_TAGS_MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing
  check_current_mp3_no_tags
  check_current_file_size "1366334"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3" 
  check_current_mp3_length "01.04"
  check_current_file_has_xing
  check_current_mp3_no_tags
  check_current_file_size "1521748"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing
  check_current_mp3_no_tags
  check_current_file_size "1399171"

  print_ok
  echo
}

function test_normal_vbr_no_xing
{
  remove_output_dir

  disable_check_mp3val

  test_name="vbr no xing"
  M_FILE="La_Verue__Today"

  mp3splt_args="-x -T 2 -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" ""

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3" 
  check_current_mp3_length "01.08"
  check_current_file_has_no_xing
  check_current_file_size "1412101"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3" 
  check_current_mp3_length "01.16"
  check_current_file_has_no_xing
  check_current_file_size "1567515"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3" 
  check_current_mp3_length "00.58"
  check_current_file_has_no_xing
  check_current_file_size "1444938"

  enable_back_mp3val

  print_ok
  echo
}

function test_normal_vbr_custom_tags
{
  remove_output_dir

  test_name="vbr custom tags"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_00s__03m_05s.mp3\" created
 Processed 7084 frames - Sync errors: 0
 file split"
  tags_option="[@a=a1,@b=b1,@t=t1,@y=2000,@c=my_comment,@n=10,@g=Slow Rock][]%[@o,@b=album,@N=7,@g=Humour][@a=custom_artist][@o,@n=20]"
  mp3splt_args="-d $OUTPUT_DIR -g \"$tags_option\" $MP3_FILE 0.5 1.0 1.5 2.0 3.0 3.5"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3"
  check_all_mp3_tags_with_version "1 2" "a1" "b1" "t1" "2000"\
  "Slow Rock" "95" "10" "my_comment"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3"
  check_current_mp3_no_tags

  current_file="$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3"
  check_all_mp3_tags_with_version "1 2" "La Verue" "album" "Today"\
  "2007" "Humour" "100" "7" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3"
  check_all_mp3_tags_with_version "1 2" "custom_artist" "album" "Today"\
  "2007" "Humour" "100" "8" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_00s__03m_05s.mp3"
  check_all_mp3_tags_with_version "1 2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "20" "http://www.jamendo.com/"

  print_ok
  echo
}

function test_normal_vbr_custom_tags_without_original_tags
{
  remove_output_dir

  test_name="vbr custom tags & without original tags"
  M_FILE="La_Verue__Today_id3v1_only"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_00s__03m_05s.mp3\" created
 Processed 7084 frames - Sync errors: 0
 file split"
  tags_option="[@a=a1,@b=b1,@t=t1,@y=2000,@c=my_comment,@n=10,@g=Slow Rock][]%[@b=album,@N=7,@g=Humour][@a=custom_artist][@n=20]"
  mp3splt_args="-d $OUTPUT_DIR -g \"$tags_option\" $SONGS_DIR/${M_FILE}.mp3 0.5 1.0 1.5 2.0 3.0 3.5"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3"
  check_all_mp3_tags_with_version "1" "a1" "b1" "t1" "2000" "Slow Rock" "95" "10" "my_comment"
  check_current_mp3_no_id3v2_tags

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3"
  check_current_mp3_no_tags

  current_file="$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3"
  check_all_mp3_tags_with_version "1" "" "album" "" "" "Humour" "100" "7" ""
  check_current_mp3_no_id3v2_tags

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3"
  check_all_mp3_tags_with_version "1" "custom_artist" "album" "" "" "Humour" "100" "8" ""
  check_current_mp3_no_id3v2_tags

  current_file="$OUTPUT_DIR/${M_FILE}_03m_00s__03m_05s.mp3"
  check_all_mp3_tags_with_version "1" "" "album" "" "" "Humour" "100" "20" ""
  check_current_mp3_no_id3v2_tags

  print_ok
  echo
}

function test_normal_vbr_custom_tags_id3v1
{
  remove_output_dir

  test_name="vbr custom tags & id3v1"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_00s__03m_05s.mp3\" created
 Processed 7084 frames - Sync errors: 0
 file split"
  tags_option="[@a=a1,@b=b1,@t=t1,@y=2000,@c=my_comment,@n=10,@g=Country][]%[@o,@b=album,@N=7,@g=Humour][@a=custom_artist,@g=doesnotexists][@o,@n=20]"
  mp3splt_args="-d $OUTPUT_DIR -T 1 -g \"$tags_option\" $MP3_FILE 0.5 1.0 1.5 2.0 3.0 3.5"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3"
  check_all_mp3_tags_with_version "1" "a1" "b1" "t1" "2000"\
  "Country" "2" "10" "my_comment"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3"
  check_current_mp3_no_tags

  current_file="$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3"
  check_all_mp3_tags_with_version "1" "La Verue" "album" "Today"\
  "2007" "Humour" "100" "7" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3"
  check_all_mp3_tags_with_version "1" "custom_artist" "album" "Today"\
  "2007" "Other" "12" "8" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_00s__03m_05s.mp3"
  check_all_mp3_tags_with_version "1" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "20" "http://www.jamendo.com/"

  print_ok
  echo
}


function test_normal_vbr_custom_tags_and_cue_export
{
  remove_output_dir

  test_name="vbr custom tags & cue export"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_00s__03m_05s.mp3\" created
 Processed 7084 frames - Sync errors: 0
 file split
 CUE file 'output/output_out.cue' created."
  tags_option="[@a=a1,@b=b1,@t=t1,@y=2000,@c=my_comment,@n=10][]%[@o,@b=album,@N=7][@a=custom_artist][@o,@n=20]"
  mp3splt_args="-E output/out.cue -d $OUTPUT_DIR -g \"$tags_option\" $MP3_FILE 0.5 1.0 1.5 2.0 3.0 3.5"
  run_check_output "$mp3splt_args" "$expected"

  check_file_content "output/output_out.cue" 'TITLE "b1"
PERFORMER "a1"
FILE "songs/La_Verue__Today.mp3" MP3
  TRACK 01 AUDIO
    TITLE "t1"
    PERFORMER "a1"
    REM ALBUM "b1"
    REM DATE "2000"
    REM TRACK "10"
    REM COMMENT "my_comment"
    REM NAME "La_Verue__Today_00m_05s__01m_00s"
    INDEX 01 00:05:00
  TRACK 02 AUDIO
    REM NAME "La_Verue__Today_01m_00s__01m_05s"
    INDEX 01 01:00:00
  TRACK 03 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    REM ALBUM "album"
    REM GENRE "Rock"
    REM DATE "2007"
    REM TRACK "7"
    REM COMMENT "http://www.jamendo.com/"
    REM NAME "La_Verue__Today_01m_05s__02m_00s"
    INDEX 01 01:05:00
  TRACK 04 AUDIO
    TITLE "Today"
    PERFORMER "custom_artist"
    REM ALBUM "album"
    REM GENRE "Rock"
    REM DATE "2007"
    REM TRACK "8"
    REM COMMENT "http://www.jamendo.com/"
    REM NAME "La_Verue__Today_02m_00s__03m_00s"
    INDEX 01 02:00:00
  TRACK 05 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    REM ALBUM "Riez Noir"
    REM GENRE "Rock"
    REM DATE "2007"
    REM TRACK "20"
    REM COMMENT "http://www.jamendo.com/"
    REM NAME "La_Verue__Today_03m_00s__03m_05s"
    INDEX 01 03:00:00
  TRACK 06 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    REM ALBUM "album"
    REM GENRE "Rock"
    REM DATE "2007"
    REM TRACK "7"
    REM COMMENT "http://www.jamendo.com/"
    INDEX 01 03:05:00'

  current_file="$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3"
  check_all_mp3_tags_with_version "2" "a1" "b1" "t1"\
  "2000" "" "" "10" "my_comment"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3"
  check_current_mp3_no_tags

  current_file="$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3"
  check_all_mp3_tags_with_version "2" "La Verue" "album" "Today"\
  "2007" "Rock" "17" "7" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3"
  check_all_mp3_tags_with_version "2" "custom_artist" "album" "Today"\
  "2007" "Rock" "17" "8" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_00s__03m_05s.mp3"
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "20" "http://www.jamendo.com/"

  print_ok
  echo
}

function test_normal_vbr_custom_tags_and_input_no_tags
{
  remove_output_dir

  test_name="vbr custom tags & no input tags"
  M_FILE="La_Verue__Today__no_tags"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_00s__03m_05s.mp3\" created
 Processed 7084 frames - Sync errors: 0
 file split"
  tags_option="[@a=a1,@b=b1,@t=t1,@y=2000,@c=my_comment,@n=10][]%[@o,@b=album,@N=7][@a=custom_artist][@o,@n=20]"
  mp3splt_args="-d $OUTPUT_DIR -g \"$tags_option\" $NO_TAGS_MP3_FILE 0.5 1.0 1.5 2.0 3.0 3.5"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3"
  check_all_mp3_tags_with_version "1 2" "a1" "b1" "t1"\
  "2000" "" "" "10" "my_comment"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3"
  check_current_mp3_no_tags

  current_file="$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3"
  check_all_mp3_tags_with_version "1 2" "" "album" "" "" "" "" "7" ""

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3"
  check_all_mp3_tags_with_version "1 2" "custom_artist" "album" "" ""\
  "" "" "8" ""

  current_file="$OUTPUT_DIR/${M_FILE}_03m_00s__03m_05s.mp3"
  check_all_mp3_tags_with_version "1 2" "" "" "" "" "" "" "20" ""

  print_ok
  echo
}

function test_normal_vbr_custom_tags_multiple_percent
{
  remove_output_dir

  test_name="vbr custom tags multiple percent"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_00s__03m_05s.mp3\" created
 Processed 7084 frames - Sync errors: 0
 file split"
  tags_option="%[@a=a1,@b=b1,@n=10][]%[@o,@b=album,@N=7][@a=custom_artist][@o,@n=20]"
  mp3splt_args="-d $OUTPUT_DIR -g \"$tags_option\" $MP3_FILE 0.5 1.0 1.5 2.0 3.0 3.5"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3"
  check_all_mp3_tags_with_version "2" "a1" "b1" "" "" "" "" "10" ""

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3"
  check_all_mp3_tags_with_version "2" "a1" "b1" "" "" "" "" "10" ""

  current_file="$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3"
  check_all_mp3_tags_with_version "2" "La Verue" "album" "Today"\
  "2007" "Rock" "17" "7" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3"
  check_all_mp3_tags_with_version "2" "custom_artist" "album" "Today"\
  "2007" "Rock" "17" "8" "http://www.jamendo.com/"

  print_ok
  echo
}

function test_normal_vbr_stdin
{
  no_tags_file=$1

  remove_output_dir

  if [[ -z $no_tags_file ]];then
    test_name="vbr stdin"
    M_FILE="La_Verue__Today"
    sync_errors=1
    frames=9400
    last_file_size=806141
  elif [[ $no_tags_file -eq "no_input_tags" ]];then
    test_name="vbr stdin & no input tags"
    M_FILE="La_Verue__Today__no_tags"
    frames=9399
    sync_errors=0
    last_file_size=806037
  fi

  expected=" Processing file '-' ...
 info: file matches the plugin 'mp3 (libmad)'
 warning: stdin '-' is supposed to be mp3 stream.
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE NS - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/-_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/-_02m_00s_20h__03m_30s.mp3\" created
   File \"$OUTPUT_DIR/-_03m_30s__04m_05s_58h.mp3\" created
 Processed $frames frames - Sync errors: $sync_errors
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR - 1.0 2.0.2 3.30 EOF"
  run_custom_check_output "cat songs/${M_FILE}.mp3 | $MP3SPLT" "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/-_01m_00s__02m_00s_20h.mp3"
  check_current_mp3_length "01.00"
  check_current_mp3_no_tags
  check_current_file_size "1365917"

  current_file="$OUTPUT_DIR/-_02m_00s_20h__03m_30s.mp3"
  check_current_mp3_length "01.29"
  check_current_mp3_no_tags
  check_current_file_size "2113734"

  current_file="$OUTPUT_DIR/-_03m_30s__04m_05s_58h.mp3"
  check_current_mp3_length "00.35"
  check_current_mp3_no_tags
  check_current_file_size "$last_file_size"

  print_ok
  echo
}

function test_normal_vbr_stdin_no_input_tags { test_normal_vbr_stdin "no_input_tags"; }

function _test_normal_vbr_stdin_and_tags
{
  local tags_version=$1

  remove_output_dir

  test_name="vbr stdin and tags v$tags_version"
  M_FILE="La_Verue__Today"

  expected=" Processing file '-' ...
 info: file matches the plugin 'mp3 (libmad)'
 warning: stdin '-' is supposed to be mp3 stream.
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE NS - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/-_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/-_02m_00s_20h__03m_30s.mp3\" created
   File \"$OUTPUT_DIR/-_03m_30s__04m_05s_58h.mp3\" created
 Processed 9400 frames - Sync errors: 1
 file split (EOF)"
  mp3splt_args="-g %[@a=a1,@b=b1,@y=1070,@N=1] -T $tags_version -d $OUTPUT_DIR - 1.0 2.0.2 3.30 EOF"
  run_custom_check_output "cat songs/${M_FILE}.mp3 | $MP3SPLT" "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/-_01m_00s__02m_00s_20h.mp3"
  check_all_mp3_tags_with_version $tags_version "a1" "b1" "" "1070"\
  "" "" "1" ""
  check_current_mp3_length "01.00"
  check_current_file_has_xing
  if [[ $tags_version -eq 2 ]];then
    check_current_file_size "1365997"
  else
    check_current_file_size "1366045"
  fi

  current_file="$OUTPUT_DIR/-_02m_00s_20h__03m_30s.mp3"
  check_all_mp3_tags_with_version $tags_version "a1" "b1" "" "1070"\
  "" "" "2" ""
  check_current_mp3_length "01.29"
  check_current_file_has_xing
  if [[ $tags_version -eq 2 ]];then
    check_current_file_size "2113814"
  else
    check_current_file_size "2113862"
  fi

  current_file="$OUTPUT_DIR/-_03m_30s__04m_05s_58h.mp3"
  check_all_mp3_tags_with_version $tags_version "a1" "b1" "" "1070"\
  "" "" "3" ""
  check_current_mp3_length "00.35"
  check_current_file_has_xing
  if [[ $tags_version -eq 2 ]];then
    check_current_file_size "806221"
  else
    check_current_file_size "806269"
  fi

  print_ok
  echo
}

function test_normal_vbr_stdin_and_tags_v1 { _test_normal_vbr_stdin_and_tags 1; }
function test_normal_vbr_stdin_and_tags_v2 { _test_normal_vbr_stdin_and_tags 2; }

function test_normal_vbr_output_fname
{
  remove_output_dir

  test_name="vbr output fname"
  M_FILE="La_Verue__Today"

  expected=" warning: output format ambiguous (@t or @n missing)
 Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/test.mp3\" created
 Processed 4595 frames - Sync errors: 0
 file split"
  mp3splt_args="-o 'test' -d $OUTPUT_DIR $MP3_FILE 1.0 2.0" 
  run_check_output "$mp3splt_args" "$expected"

  check_if_file_exist "$OUTPUT_DIR/test.mp3"

  current_file="$OUTPUT_DIR/test.mp3" 
  check_current_mp3_length "01.00"

  print_ok
  echo
}

function test_normal_vbr_output_fnames_and_custom_tags
{
  remove_output_dir

  test_name="vbr output fnames & custom tags"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/a1_b1_t1_1_10_${M_FILE} 00:05:00 01:00:30.mp3\" created
   File \"$OUTPUT_DIR/___2_2_${M_FILE} 01:00:30 01:05:00.mp3\" created
   File \"$OUTPUT_DIR/La Verue_album_Today_3_7_${M_FILE} 01:05:00 02:00:00.mp3\" created
 Processed 4595 frames - Sync errors: 0
 file split"
  tags_option="[@a=a1,@b=b1,@t=t1,@y=2000,@c=my_comment,@n=10][]%[@o,@b=album,@N=7]"
  output_option="\"@a_@b_@t_@n_@N_@f+@m:@s:@h @M:@S:@H\""
  mp3splt_args="-d $OUTPUT_DIR -g \"$tags_option\" -o $output_option $MP3_FILE 0.5 1.0.30 1.5 2.0"
  run_check_output "$mp3splt_args" "$expected"

  check_if_file_exist "$OUTPUT_DIR/a1_b1_t1_1_10_${M_FILE} 00:05:00 01:00:30.mp3"
  check_if_file_exist "$OUTPUT_DIR/___2_2_${M_FILE} 01:00:30 01:05:00.mp3"
  check_if_file_exist "$OUTPUT_DIR/La Verue_album_Today_3_7_${M_FILE} 01:05:00 02:00:00.mp3"

  print_ok
  echo
}

function test_normal_vbr_output_fnames_and_custom_tags_with_no_track_genre
{
  remove_output_dir

  test_name="vbr output fnames & custom tags"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/a1_b1_t1_1__${M_FILE} 00:05:00 01:00:30.mp3\" created
   File \"$OUTPUT_DIR/___2_2_${M_FILE} 01:00:30 01:05:00.mp3\" created
   File \"$OUTPUT_DIR/La Verue_album_Today_3_7_${M_FILE} 01:05:00 02:00:00.mp3\" created
 Processed 4595 frames - Sync errors: 0
 file split"
  tags_option="[@a=a1,@b=b1,@t=t1,@y=2000,@c=my_comment,@n=-2][]%[@o,@b=album,@N=7]"
  output_option="\"@a_@b_@t_@n_@N_@f+@m:@s:@h @M:@S:@H\""
  mp3splt_args="-d $OUTPUT_DIR -g \"$tags_option\" -o $output_option $MP3_FILE 0.5 1.0.30 1.5 2.0"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/a1_b1_t1_1__${M_FILE} 00:05:00 01:00:30.mp3" 
  check_current_mp3_length "00.55"
  check_all_mp3_tags_with_version 2 "a1" "b1" "t1" "2000" "" "" "" "my_comment"

  check_if_file_exist "$OUTPUT_DIR/___2_2_${M_FILE} 01:00:30 01:05:00.mp3"
  check_if_file_exist "$OUTPUT_DIR/La Verue_album_Today_3_7_${M_FILE} 01:05:00 02:00:00.mp3"

  print_ok
  echo
}

function test_normal_vbr_output_fnames_and_dirs
{
  remove_output_dir

  test_name="vbr output fnames & directories"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/La Verue/Riez Noir/La Verue-Today-Rock 1.mp3\" created
   File \"$OUTPUT_DIR/La Verue/Riez Noir/La Verue-Today-Rock 2.mp3\" created
   File \"$OUTPUT_DIR/La Verue/Riez Noir/La Verue-Today-Rock 3.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  output_option="@a/@b/@a-@t-@g @n"
  mp3splt_args="-o '$output_option' -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  check_if_directory_exist "$OUTPUT_DIR/La Verue"
  check_if_directory_exist "$OUTPUT_DIR/La Verue/Riez Noir"
  check_if_file_exist "$OUTPUT_DIR/La Verue/Riez Noir/La Verue-Today-Rock 1.mp3"
  check_if_file_exist "$OUTPUT_DIR/La Verue/Riez Noir/La Verue-Today-Rock 2.mp3"
  check_if_file_exist "$OUTPUT_DIR/La Verue/Riez Noir/La Verue-Today-Rock 3.mp3"

  print_ok
  echo
}

function test_normal_vbr_output_fnames_and_custom_tags_and_dirs
{
  remove_output_dir

  test_name="vbr output fnames & custom tags & directories"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/La Verue/album1/La Verue-Today-Rock 1.mp3\" created
   File \"$OUTPUT_DIR/La Verue/album2/La Verue-Today-Rock 2.mp3\" created
   File \"$OUTPUT_DIR/La Verue/album3/La Verue-Today-Soundtrack 3.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  output_option="@a/@b/@a-@t-@g @n"
  tags_option="%[@o,@b=album1][@b=album2][@b=album3,@g=Soundtrack]"
  mp3splt_args="-o '$output_option' -g \"$tags_option\" -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  check_if_directory_exist "$OUTPUT_DIR/La Verue"
  check_if_directory_exist "$OUTPUT_DIR/La Verue/album1"
  check_if_directory_exist "$OUTPUT_DIR/La Verue/album2"
  check_if_directory_exist "$OUTPUT_DIR/La Verue/album3"
  check_if_file_exist "$OUTPUT_DIR/La Verue/album1/La Verue-Today-Rock 1.mp3"
  check_if_file_exist "$OUTPUT_DIR/La Verue/album2/La Verue-Today-Rock 2.mp3"
  check_if_file_exist "$OUTPUT_DIR/La Verue/album3/La Verue-Today-Soundtrack 3.mp3"

  print_ok
  echo
}

function test_output_fnames_with_digit_after_pattern
{
  remove_output_dir

  test_name="vbr output fnames & digit after pattern"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/0001_00_000 0002_00_020 1.mp3\" created
   File \"$OUTPUT_DIR/0002_00_020 0003_05_000 2.mp3\" created
   File \"$OUTPUT_DIR/0003_05_000 0004_05_058 3.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  output_option="@m4_@s2_@h3 @M4_@S2_@H3 @n"
  mp3splt_args="-o '$output_option' -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  check_if_file_exist "$OUTPUT_DIR/0001_00_000 0002_00_020 1.mp3"
  check_if_file_exist "$OUTPUT_DIR/0002_00_020 0003_05_000 2.mp3"
  check_if_file_exist "$OUTPUT_DIR/0003_05_000 0004_05_058 3.mp3"

  print_ok
  echo
}

function test_output_fnames_with_digit_after_pattern_equal_to_zero
{
  remove_output_dir

  test_name="vbr output fnames & digit after pattern"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/1m__2m_20h__1.mp3\" created
   File \"$OUTPUT_DIR/2m_20h__3m_5s__2.mp3\" created
   File \"$OUTPUT_DIR/3m_5s__4m_5s_58h__3.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  output_option="@m0m_@s0s_@h0h__@M0m_@S0s_@H0h__@n"
  mp3splt_args="-o '$output_option' -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  check_if_file_exist "$OUTPUT_DIR/1m__2m_20h__1.mp3"
  check_if_file_exist "$OUTPUT_DIR/2m_20h__3m_5s__2.mp3"
  check_if_file_exist "$OUTPUT_DIR/3m_5s__4m_5s_58h__3.mp3"

  print_ok
  echo
}

function test_normal_vbr_stdout
{
  remove_output_dir

  test_name="vbr stdout"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"-\" created
 Processed 5751 frames - Sync errors: 0
 file split"
  mp3splt_args="-T 2 -o - $MP3_FILE 1.0 2.30.2"
  run_custom_check_output "$MP3SPLT $mp3splt_args > $OUTPUT_DIR/stdout.mp3" "" "$expected"

  current_file="$OUTPUT_DIR/stdout.mp3"
  check_current_mp3_length "01.30"
  check_current_file_size "2054691"

  print_ok
  echo
}

function test_normal_vbr_stdout_multiple_splitpoints
{
  remove_output_dir

  test_name="vbr stdout & splitpoints > 2"
  M_FILE="La_Verue__Today"

  expected=" Warning: multiple splitpoints with stdout !
 Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"-\" created
   File \"-\" created
 Processed 6509 frames - Sync errors: 0
 file split"
  mp3splt_args="-T 2 -o - $MP3_FILE 1.0 2.30.2 2.50"
  run_custom_check_output "$MP3SPLT $mp3splt_args > $OUTPUT_DIR/stdout.mp3" "" "$expected"

  current_file="$OUTPUT_DIR/stdout.mp3"
#TODO: 2 outputs are concatenated in the same file ? should we do something ?
  check_current_mp3_length "01.30"
  check_current_file_size "2582245"

  print_ok
  echo
}

function test_normal_vbr_custom_tags_with_replace_tags_in_tags
{
  remove_output_dir

  test_name="vbr custom tags & replace tags in tags"
  M_FILE="La_Verue__Today"

  F1="-a_10-b_a_@n-10.mp3"
  F2="Today-La Verue-album_cc_@t-7.mp3"
  F3="Today-La Verue-album_cc_@t-8.mp3"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/$F1\" created
   File \"$OUTPUT_DIR/$F2\" created
   File \"$OUTPUT_DIR/$F3\" created
 Processed 4595 frames - Sync errors: 0
 file split"
  tags_option="r[@a=a_@n,@b=b_@a,@c=cc_@b,@n=10]%[@o,@c=cc_@t,@b=album_@c,@N=7]"
  output_option="@t-@a-@b-@N"
  mp3splt_args="-d $OUTPUT_DIR -o $output_option -g \"$tags_option\" $MP3_FILE 0.5 1.0 1.5 2.0"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/$F1"
  check_all_mp3_tags_with_version "2" "a_10" "b_a_@n" "" "" "" "" "10" "cc_b_@a"

  current_file="$OUTPUT_DIR/$F2"
  check_all_mp3_tags_with_version "2" "La Verue" "album_cc_@t" "Today" "2007"\
  "Rock" "17" "7" "cc_Today"

  current_file="$OUTPUT_DIR/$F3"
  check_all_mp3_tags_with_version "2" "La Verue" "album_cc_@t" "Today" "2007"\
  "Rock" "17" "8" "cc_Today"

  print_ok
  echo
}

function test_normal_vbr_custom_tags_without_replace_tags_in_tags
{
  remove_output_dir

  test_name="vbr custom tags without replace tags in tags"
  M_FILE="La_Verue__Today"

  F1="-a_@n-b_@a-10.mp3"
  F2="Today-La Verue-album_@c-7.mp3"
  F3="Today-La Verue-album_@c-8.mp3"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/$F1\" created
   File \"$OUTPUT_DIR/$F2\" created
   File \"$OUTPUT_DIR/$F3\" created
 Processed 4595 frames - Sync errors: 0
 file split"
  tags_option="[@a=a_@n,@b=b_@a,@c=cc_@b,@n=10]%[@o,@c=cc_@t,@b=album_@c,@N=7]"
  output_option="@t-@a-@b-@N"
  mp3splt_args="-d $OUTPUT_DIR -o $output_option -g \"$tags_option\" $MP3_FILE 0.5 1.0 1.5 2.0"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/$F1"
  check_all_mp3_tags_with_version "2" "a_@n" "b_@a" "" ""\
  "" "" "10" "cc_@b"

  current_file="$OUTPUT_DIR/$F2"
  check_all_mp3_tags_with_version "2" "La Verue" "album_@c" "Today" "2007"\
  "Rock" "17" "7" "cc_@t"

  current_file="$OUTPUT_DIR/$F3"
  check_all_mp3_tags_with_version "2" "La Verue" "album_@c" "Today" "2007"\
  "Rock" "17" "8" "cc_@t"

  print_ok
  echo
}

function test_normal_vbr_custom_empty_tags
{
  remove_output_dir

  test_name="vbr custom tags empty tags"
  M_FILE="La_Verue__Today"

  F1="1.mp3"
  F2="2.mp3"
  F3="3.mp3"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/$F1\" created
   File \"$OUTPUT_DIR/$F2\" created
   File \"$OUTPUT_DIR/$F3\" created
 Processed 4595 frames - Sync errors: 0
 file split"
  output_option="@n"
  mp3splt_args="-d $OUTPUT_DIR -o $output_option -g \"%[]\" $MP3_FILE 0.5 1.0 1.5 2.0"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/$F1"
  check_current_mp3_no_tags

  current_file="$OUTPUT_DIR/$F2"
  check_current_mp3_no_tags

  current_file="$OUTPUT_DIR/$F3"
  check_current_mp3_no_tags

  print_ok
  echo
}

function test_normal_vbr_split_in_equal_parts
{
  remove_output_dir

  test_name="vbr split in equal parts"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting 'split in equal tracks' mode
   File \"$OUTPUT_DIR/1.mp3\" created
   File \"$OUTPUT_DIR/2.mp3\" created
   File \"$OUTPUT_DIR/3.mp3\" created
   File \"$OUTPUT_DIR/4.mp3\" created
 Processed 9402 frames - Sync errors: 0
 split in equal tracks ok"
  mp3splt_args="-d $OUTPUT_DIR -o @n -S 4 $MP3_FILE"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/1.mp3" 
  check_current_mp3_length "01.01"
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir"\
  "Today" "2007" "Rock" "17" "1" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/2.mp3" 
  check_current_mp3_length "01.01"
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir"\
  "Today" "2007" "Rock" "17" "2" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/3.mp3" 
  check_current_mp3_length "01.01"
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir"\
  "Today" "2007" "Rock" "17" "3" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/4.mp3" 
  check_current_mp3_length "01.01"
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir"\
  "Today" "2007" "Rock" "17" "4" "http://www.jamendo.com/"

  print_ok
  echo
}

function test_normal_vbr_tags_from_filename_regex
{
  remove_output_dir

  test_name="vbr tags from filename regex"

  NEW_M_FILE="artist1__album2__title3__comment4__2__2004__Samba"
  NEW_MP3_FILE=$SONGS_DIR/${NEW_M_FILE}.mp3

  cp $MP3_FILE $NEW_MP3_FILE

  F1="title3-artist1-album2-2-1.mp3"
  F2="title3-artist1-album2-2-2.mp3"
  F3="title3-artist1-album2-2-3.mp3"

  expected=" Processing file 'songs/${NEW_M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/$F1\" created
   File \"$OUTPUT_DIR/$F2\" created
   File \"$OUTPUT_DIR/$F3\" created
 Processed 4595 frames - Sync errors: 0
 file split"
  regex_option="(?<artist>.*?)__(?<album>.*?)__(?<title>.*?)__(?<comment>.*?)__(?<tracknum>.*?)__(?<year>.*?)__(?<genre>.*)"
  output_option="@t-@a-@b-@N-@n"
  mp3splt_args="-d $OUTPUT_DIR -o $output_option -G \"regex=$regex_option\" ${NEW_MP3_FILE} 0.5 1.0 1.5 2.0"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/$F1"

#ID3v2, eyeD3 apparently reads other YEAR tag from the original ID3v2 tags
  check_all_mp3_tags_with_version "2" "artist1" "album2" "title3" "2007"\
  "Samba" "114" "2" "comment4"
  check_all_mp3_tags_with_version "1" "artist1" "album2" "title3" "2004"\
  "Samba" "114" "2" "comment4"

  current_file="$OUTPUT_DIR/$F2"
  check_all_mp3_tags_with_version "2" "artist1" "album2" "title3" "2007"\
  "Samba" "114" "2" "comment4"

  current_file="$OUTPUT_DIR/$F3"
  check_all_mp3_tags_with_version "2" "artist1" "album2" "title3" "2007"\
  "Samba" "114" "2" "comment4"

  rm -f $NEW_MP3_FILE

  print_ok
  echo
}

function test_normal_vbr_with_auto_adjust
{
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="normal with auto adjust"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 Working with SILENCE AUTO-ADJUST (Threshold: -48.0 dB Gap: 30 sec Offset: 0.80 Min: 0.00 sec)
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_00m_00s__01m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_00s__04m_05s_69h.mp3\" created
 Processed 9406 frames - Sync errors: 0
 file split (EOF)"
  mp3splt_args="-a -d $OUTPUT_DIR $SILENCE_MP3_FILE 0.0 1.0 2.0 3.0 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_00m_00s__01m_00s.mp3" 
  check_current_mp3_length "01.04"
  check_current_file_has_xing
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1792357"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s.mp3" 
  check_current_mp3_length "00.55"
  check_current_file_has_xing
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "1586487"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3" 
  check_current_mp3_length "00.55"
  check_current_file_has_xing
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir"\
  "Today" "2007" "Rock" "17" "3" "http://www.jamendo.com/"
  check_current_file_size "1561672"
  
  current_file="$OUTPUT_DIR/${M_FILE}_03m_00s__04m_05s_69h.mp3" 
  check_current_mp3_length "01.10"
  check_current_file_has_xing
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "4" "http://www.jamendo.com/"
  check_current_file_size "1976114"

  print_ok
  echo
}

function test_normal_vbr_with_auto_adjust_min_length
{
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="normal with auto adjust & min length parameter"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 Working with SILENCE AUTO-ADJUST (Threshold: -48.0 dB Gap: 80 sec Offset: 0.00 Min: 9.00 sec)
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_00m_00s__02m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s__04m_05s_69h.mp3\" created
 Processed 9406 frames - Sync errors: 0
 file split (EOF)"
  mp3splt_args="-a -p off=0,gap=80,min=9 -d $OUTPUT_DIR $SILENCE_MP3_FILE 0.0 2.0 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_00m_00s__02m_00s.mp3" 
  check_current_mp3_length "00.56"
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1615571"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s__04m_05s_69h.mp3" 
  check_current_mp3_length "03.08"
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "5297541"

  print_ok
  echo
}

function test_normal_vbr_with_auto_adjust_min_length_no_change
{
  remove_output_dir

  M_FILE="La_Verue__Today_silence"

  test_name="normal with auto adjust & min length parameter no change"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Stereo - FRAME MODE - Total time: 4m.05s
 Working with SILENCE AUTO-ADJUST (Threshold: -48.0 dB Gap: 80 sec Offset: 0.00 Min: 10.00 sec)
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_00m_00s__02m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s__04m_05s_69h.mp3\" created
 Processed 9406 frames - Sync errors: 0
 file split (EOF)"
  mp3splt_args="-a -p off=0,gap=80,min=10 -d $OUTPUT_DIR $SILENCE_MP3_FILE 0.0 2.0 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_00m_00s__02m_00s.mp3" 
  check_current_mp3_length "02.00"
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "3377085"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s__04m_05s_69h.mp3" 
  check_current_mp3_length "02.05"
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "3536027"

  print_ok
  echo
}


function test_normal_vbr_with_negative_splitpoints
{
  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="normal with negative splitpoints"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_05s_58h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_05s_58h__03m_05s_58h.mp3\" created
 Processed 7106 frames - Sync errors: 0
 file split"
  mp3splt_args=" -d $OUTPUT_DIR $MP3_FILE 1.0 EOF-2.0 EOF-1.0" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_05s_58h.mp3" 
  check_current_mp3_length "01.05"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_05s_58h__03m_05s_58h.mp3" 
  check_current_mp3_length "01.00"

  print_ok
  echo
}

function test_normal_vbr_custom_tags_with_replace_tags_in_tags_and_original_tags
{
  remove_output_dir

  test_name="vbr custom tags & replace tags in tags & original tags"
  M_FILE="La_Verue__Today"

  F1="-a_10-b_a_@n-10.mp3"
  F2="Today_7_8-La Verue-album_cc_@t-7.mp3"
  F3="Today_8_8-La Verue-album_cc_@t-8.mp3"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/$F1\" created
   File \"$OUTPUT_DIR/$F2\" created
   File \"$OUTPUT_DIR/$F3\" created
 Processed 4595 frames - Sync errors: 0
 file split"
  tags_option="r[@a=a_@n,@b=b_@a,@c=cc_@b,@n=10]%[@o,@c=cc_@t,@b=album_@c,@t=#t_@n_#n,@N=7]"
  output_option="@t-@a-@b-@N"
  mp3splt_args="-d $OUTPUT_DIR -o $output_option -g \"$tags_option\" $MP3_FILE 0.5 1.0 1.5 2.0"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/$F1"
  check_all_mp3_tags_with_version "2" "a_10" "b_a_@n" "" "" "" "" "10" "cc_b_@a"

  current_file="$OUTPUT_DIR/$F2"
  check_all_mp3_tags_with_version "2" "La Verue" "album_cc_@t" "Today_7_8" "2007"\
  "Rock" "17" "7" "cc_#t_@n_#n"

  current_file="$OUTPUT_DIR/$F3"
  check_all_mp3_tags_with_version "2" "La Verue" "album_cc_@t" "Today_8_8" "2007"\
  "Rock" "17" "8" "cc_#t_@n_#n"

  print_ok
  echo
}

function test_normal_vbr_custom_tags_with_replace_tags_in_tags_and_time_variables
{
  remove_output_dir

  test_name="vbr custom tags & replace tags in tags & time variables"
  M_FILE="La_Verue__Today"

  F1="-a_10-b_a_@n-10.mp3"
  F2="7__01_00__01_05-La Verue-album_cc_@t-7.mp3"
  F3="8__01_05__02_00-La Verue-album_cc_@t-8.mp3"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/$F1\" created
   File \"$OUTPUT_DIR/$F2\" created
   File \"$OUTPUT_DIR/$F3\" created
 Processed 4595 frames - Sync errors: 0
 file split"
  tags_option="r[@a=a_@n,@b=b_@a,@c=cc_@b,@n=10]%[@o,@c=cc_@t,@b=album_@c,@t=@n__@m_@s__@M_@S,@N=7]"
  output_option="@t-@a-@b-@N"
  mp3splt_args="-d $OUTPUT_DIR -o $output_option -g \"$tags_option\" $MP3_FILE 0.5 1.0 1.5 2.0"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/$F1"
  check_all_mp3_tags_with_version "2" "a_10" "b_a_@n" "" "" "" "" "10" "cc_b_@a"

  current_file="$OUTPUT_DIR/$F2"
  check_all_mp3_tags_with_version "2" "La Verue" "album_cc_@t" "7__01_00__01_05" "2007"\
  "Rock" "17" "7" "cc_@n__@m_@s__@M_@S"

  current_file="$OUTPUT_DIR/$F3"
  check_all_mp3_tags_with_version "2" "La Verue" "album_cc_@t" "8__01_05__02_00" "2007"\
  "Rock" "17" "8" "cc_@n__@m_@s__@M_@S"

  print_ok
  echo
}

function test_normal_vbr_with_sync_errors
{
  local tags_version=$1

  remove_output_dir

  disable_check_mp3val

  M_FILE="syncerror"

  test_name="vbr with sync errors"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_07m_00s__04m_05s_58h.mp3\" created
 Processed 27372 frames - Sync errors: 2
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR $SYNCERR_FILE 1.0 2.0.2 3.5 7.0 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_size "1412518"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3" 
  check_current_mp3_length "01.04"
  check_current_file_size "1567932"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3" 
  check_current_mp3_length "03.55"
  check_current_file_size "5375652"

  current_file="$OUTPUT_DIR/${M_FILE}_07m_00s__04m_05s_58h.mp3" 
  check_current_mp3_length "04.55"
  check_current_file_size "5308269"

  enable_back_mp3val

  print_ok
  echo
}

function test_normal_vbr_same_tag_bytes_in_output_file
{
  remove_output_dir

  test_name="vbr & same tag bytes in output file"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_00m_00s__04m_05s_58h.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  mp3splt_args="-g %[@O] -d $OUTPUT_DIR $MP3_FILE 0.0 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$MP3_FILE"
  check_current_file_size "5656852"

  current_file="$OUTPUT_DIR/${M_FILE}_00m_00s__04m_05s_58h.mp3" 
  check_current_mp3_length "04.05"
  check_current_file_size "5656852"

  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "8/14" "http://www.jamendo.com/"

  check_all_mp3_tags_with_version 1 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "0" "http://www.jamendo.com/"

  print_ok
  echo
}

function test_vbr_with_bit_reservoir_handling
{
  remove_output_dir

  test_name="vbr & bit reservoir handling"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 warning: bit reservoir handling for gapless playback is still experimental
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  mp3splt_args="-T 2 -f -b -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_number_of_files 3

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1412648"
  check_current_md5sum "171fd72f8667a9b3b57e801b944ec4fa"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3" 
  check_current_mp3_length "01.04"
  check_current_file_has_xing
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir"\
  "Today" "2007" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "1569184"
  check_current_md5sum "e9de1b75940c38d4ee86a4741bdabad6"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "3" "http://www.jamendo.com/"
  check_current_file_size "1446137"
  check_current_md5sum "828225f2a769fad81b564a8ab1d544f3"

  print_ok
  echo
}

function run_normal_vbr_tests
{
  p_blue " NORMAL VBR mp3 tests ..."
  echo

  normal_test_functions=$(declare -F | grep " test_normal_" | awk '{ print $3 }')

  for test_func in $normal_test_functions;do
    eval $test_func
  done

  p_blue " NORMAL VBR tests DONE."
  echo
}

#main
export LC_ALL="C"
start_date=$(date +%s)

run_normal_vbr_tests

p_failed_tests

end_date=$(date +%s)

p_time_diff_cyan $start_date $end_date "\t"
echo -e '\n'

exit 0

