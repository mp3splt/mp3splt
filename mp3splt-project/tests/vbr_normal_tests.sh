#!/bin/bash

. ./utils.sh || exit 1

#normal mode functional tests

function test_normal_vbr
{
  local tags_version=$1

  remove_output_dir

  M_FILE="La_Verue__Today"

  if [[ $tags_version -eq -1 ]];then
    tags_option="-n"
    test_name="vbr no tags"
  else
    test_name="vbr id3v$tags_version"
    tags_option="-T $tags_version"
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
  mp3splt_args="$tags_option -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing

  if [[ $tags_version -eq -1 ]];then
    check_current_mp3_no_tags
    check_current_file_size "1366334"
  else
    check_all_mp3_tags_with_version $tags_version "La Verue" "Riez Noir" "Today"\
    "2007" "Rock" "17" "1" "http://www.jamendo.com/"

    if [[ $tags_version -eq 2 ]];then
      check_current_file_size "1366550"
    else
      check_current_file_size "1366462"
    fi
  fi

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3" 
  check_current_mp3_length "01.04"
  check_current_file_has_xing

  if [[ $tags_version -eq -1 ]];then
    check_current_mp3_no_tags
    check_current_file_size "1521748"
  else
    check_all_mp3_tags_with_version $tags_version "La Verue" "Riez Noir"\
    "Today" "2007" "Rock" "17" "2" "http://www.jamendo.com/"

    if [[ $tags_version -eq 2 ]];then
      check_current_file_size "1521964"
    else
      check_current_file_size "1521876"
    fi
  fi

  current_file="$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing

  if [[ $tags_version -eq -1 ]];then
    check_current_mp3_no_tags
    check_current_file_size "1399171"
  else
    check_all_mp3_tags_with_version $tags_version "La Verue" "Riez Noir" "Today"\
      "2007" "Rock" "17" "3" "http://www.jamendo.com/"

    if [[ $tags_version -eq 2 ]];then
      check_current_file_size "1399387"
    else
      check_current_file_size "1399299"
    fi
  fi

  p_green "OK"
  echo
}

function test_normal_vbr_no_tags { test_normal_vbr -1; }
function test_normal_vbr_id3v1 { test_normal_vbr 1; }
function test_normal_vbr_id3v2 { test_normal_vbr 2; }

function test_normal_vbr_original_tags
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
  check_all_mp3_tags_with_version 1 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"
  check_current_file_size "1366678"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3" 
  check_current_mp3_length "01.04"
  check_current_file_has_xing
  check_all_mp3_tags_with_version 1 "La Verue" "Riez Noir"\
  "Today" "2007" "Rock" "17" "2" "http://www.jamendo.com/"
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir"\
  "Today" "2007" "Rock" "17" "2" "http://www.jamendo.com/"
  check_current_file_size "1522092"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing
  check_all_mp3_tags_with_version 1 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "3" "http://www.jamendo.com/"
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "3" "http://www.jamendo.com/"
  check_current_file_size "1399515"

  p_green "OK"
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
  check_current_file_size "1366678"
  check_all_mp3_tags_with_version "1 2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3" 
  check_current_mp3_length "01.04"
  check_current_file_has_xing
  check_current_file_size "1522092"
  check_all_mp3_tags_with_version "1 2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "2" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing
  check_current_file_size "1399515"
  check_all_mp3_tags_with_version "1 2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "3" "http://www.jamendo.com/"

  p_green "OK"
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

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3" 
  check_current_mp3_length "01.04"
  check_current_file_has_xing
  check_current_mp3_no_tags

  current_file="$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3" 
  check_current_mp3_length "01.00"
  check_current_file_has_xing
  check_current_mp3_no_tags

  p_green "OK"
  echo
}

function test_normal_vbr_no_xing
{
  remove_output_dir

  test_name="vbr no xing"
  M_FILE="La_Verue__Today"

  mp3splt_args="-x -2 -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" ""

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_00s_20h.mp3" 
  check_current_mp3_length "01.08"
  check_current_file_has_no_xing
  check_current_file_size "1366133"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__03m_05s.mp3" 
  check_current_mp3_length "01.16"
  check_current_file_has_no_xing
  check_current_file_size "1521547"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_05s__04m_05s_58h.mp3" 
  check_current_mp3_length "00.58"
  check_current_file_has_no_xing
  check_current_file_size "1398970"

  p_green "OK"
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

  p_green "OK"
  echo
}

function test_normal_vbr_create_directories
{
  remove_output_dir

  test_name="vbr create directories"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/a/b/c/${M_FILE}_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/a/b/c/${M_FILE}_02m_00s_20h__03m_05s.mp3\" created
   File \"$OUTPUT_DIR/a/b/c/${M_FILE}_03m_05s__04m_05s_58h.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  mp3splt_args=" -d $OUTPUT_DIR/a/b/c $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  check_if_directory_exist "$OUTPUT_DIR/a/b/c"
  check_if_file_exist "$OUTPUT_DIR/a/b/c/${M_FILE}_01m_00s__02m_00s_20h.mp3"
  check_if_file_exist "$OUTPUT_DIR/a/b/c/${M_FILE}_02m_00s_20h__03m_05s.mp3"
  check_if_file_exist "$OUTPUT_DIR/a/b/c/${M_FILE}_03m_05s__04m_05s_58h.mp3"

  p_green "OK"
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
  tags_option="[@a=a1,@b=b1,@t=t1,@y=2000,@c=my_comment,@n=10][]%[@o,@b=album,@N=7][@a=custom_artist][@o,@n=20]"
  mp3splt_args="-d $OUTPUT_DIR -g $tags_option $MP3_FILE 0.5 1.0 1.5 2.0 3.0 3.5"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3"
  check_all_mp3_tags_with_version "2" "a1" "b1" "t1"\
  "2000" "Other" "12" "10" "my_comment"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3"
  check_all_mp3_tags_with_version "2" "" "" "" ""\
  "Other" "12" "2" ""

  current_file="$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3"
  check_all_mp3_tags_with_version "2" "La Verue" "album" "Today"\
  "2007" "Rock" "17" "7" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3"
  check_all_mp3_tags_with_version "2" "custom_artist" "album" "Today"\
  "2007" "Rock" "17" "8" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_00s__03m_05s.mp3"
  check_all_mp3_tags_with_version "2" "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "20" "http://www.jamendo.com/"

  p_green "OK"
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
  mp3splt_args="-d $OUTPUT_DIR -g $tags_option $NO_TAGS_MP3_FILE 0.5 1.0 1.5 2.0 3.0 3.5"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3"
  check_all_mp3_tags_with_version "1 2" "a1" "b1" "t1"\
  "2000" "Other" "12" "10" "my_comment"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3"
  check_all_mp3_tags_with_version "1 2" "" "" "" "" "Other" "12" "2" ""

  current_file="$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3"
  check_all_mp3_tags_with_version "1 2" "" "album" "" "" "Other" "12" "7" ""

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3"
  check_all_mp3_tags_with_version "1 2" "custom_artist" "album" "" ""\
  "Other" "12" "8" ""

  current_file="$OUTPUT_DIR/${M_FILE}_03m_00s__03m_05s.mp3"
  check_all_mp3_tags_with_version "1 2" "" "" "" "" "Other" "12" "20" ""

  p_green "OK"
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
  mp3splt_args="-d $OUTPUT_DIR -g $tags_option $MP3_FILE 0.5 1.0 1.5 2.0 3.0 3.5"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3"
  check_all_mp3_tags_with_version "2" "a1" "b1" "" ""\
  "Other" "12" "10" ""

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3"
  check_all_mp3_tags_with_version "2" "a1" "b1" "" ""\
  "Other" "12" "2" ""

  current_file="$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3"
  check_all_mp3_tags_with_version "2" "La Verue" "album" "Today"\
  "2007" "Rock" "17" "7" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3"
  check_all_mp3_tags_with_version "2" "custom_artist" "album" "Today"\
  "2007" "Rock" "17" "8" "http://www.jamendo.com/"

  p_green "OK"
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
  check_current_file_size "2008723"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__04m_00s.mp3"
  check_current_mp3_length "01.59"
  check_current_file_size "2823722"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_30s__04m_05s_58h.mp3"
  check_current_mp3_length "00.35"
  check_current_file_size "809280"

  p_green "OK"
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

  p_green "OK"
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
  "Other" "12" "1" ""
  check_current_mp3_length "01.00"
  check_current_file_has_xing
  if [[ $tags_version -eq 2 ]];then
    check_current_file_size "1366020"
  else
    check_current_file_size "1366045"
  fi

  current_file="$OUTPUT_DIR/-_02m_00s_20h__03m_30s.mp3"
  check_all_mp3_tags_with_version $tags_version "a1" "b1" "" "1070"\
  "Other" "12" "2" ""
  check_current_mp3_length "01.29"
  check_current_file_has_xing
  if [[ $tags_version -eq 2 ]];then
    check_current_file_size "2113837"
  else
    check_current_file_size "2113862"
  fi

  current_file="$OUTPUT_DIR/-_03m_30s__04m_05s_58h.mp3"
  check_all_mp3_tags_with_version $tags_version "a1" "b1" "" "1070"\
  "Other" "12" "3" ""
  check_current_mp3_length "00.35"
  check_current_file_has_xing
  if [[ $tags_version -eq 2 ]];then
    check_current_file_size "806244"
  else
    check_current_file_size "806269"
  fi

  p_green "OK"
  echo
}

function test_normal_vbr_stdin_and_tags_v1 { _test_normal_vbr_stdin_and_tags 1; }
function test_normal_vbr_stdin_and_tags_v2 { _test_normal_vbr_stdin_and_tags 2; }

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
  mp3splt_args="-d $OUTPUT_DIR -g $tags_option -o $output_option $MP3_FILE 0.5 1.0.30 1.5 2.0"
  run_check_output "$mp3splt_args" "$expected"

  check_if_file_exist "$OUTPUT_DIR/a1_b1_t1_1_10_${M_FILE} 00:05:00 01:00:30.mp3"
  check_if_file_exist "$OUTPUT_DIR/___2_2_${M_FILE} 01:00:30 01:05:00.mp3"
  check_if_file_exist "$OUTPUT_DIR/La Verue_album_Today_3_7_${M_FILE} 01:05:00 02:00:00.mp3"

  p_green "OK"
  echo
}

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

  p_green "OK"
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
   File \"$OUTPUT_DIR/La Verue/Riez Noir/La Verue-Today 1.mp3\" created
   File \"$OUTPUT_DIR/La Verue/Riez Noir/La Verue-Today 2.mp3\" created
   File \"$OUTPUT_DIR/La Verue/Riez Noir/La Verue-Today 3.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  output_option="@a/@b/@a-@t @n"
  mp3splt_args="-o '$output_option' -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  check_if_directory_exist "$OUTPUT_DIR/La Verue"
  check_if_directory_exist "$OUTPUT_DIR/La Verue/Riez Noir"
  check_if_file_exist "$OUTPUT_DIR/La Verue/Riez Noir/La Verue-Today 1.mp3"
  check_if_file_exist "$OUTPUT_DIR/La Verue/Riez Noir/La Verue-Today 2.mp3"
  check_if_file_exist "$OUTPUT_DIR/La Verue/Riez Noir/La Verue-Today 3.mp3"

  p_green "OK"
  echo
}

function test_normal_vbr_output_fnames_and_custom_tags_dirs
{
  remove_output_dir

  test_name="vbr output fnames & custom tags & directories"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/La Verue/album1/La Verue-Today 1.mp3\" created
   File \"$OUTPUT_DIR/La Verue/album2/La Verue-Today 2.mp3\" created
   File \"$OUTPUT_DIR/La Verue/album3/La Verue-Today 3.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  output_option="@a/@b/@a-@t @n"
  tags_option="%[@o,@b=album1][@b=album2][@b=album3]"
  mp3splt_args="-o '$output_option' -g $tags_option -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  check_if_directory_exist "$OUTPUT_DIR/La Verue"
  check_if_directory_exist "$OUTPUT_DIR/La Verue/album1"
  check_if_directory_exist "$OUTPUT_DIR/La Verue/album2"
  check_if_directory_exist "$OUTPUT_DIR/La Verue/album3"
  check_if_file_exist "$OUTPUT_DIR/La Verue/album1/La Verue-Today 1.mp3"
  check_if_file_exist "$OUTPUT_DIR/La Verue/album2/La Verue-Today 2.mp3"
  check_if_file_exist "$OUTPUT_DIR/La Verue/album3/La Verue-Today 3.mp3"

  p_green "OK"
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
  check_current_file_size "2008723"

  p_green "OK"
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
  check_current_mp3_length "01.30"
  check_current_file_size "2490309"

  p_green "OK"
  echo
}

function run_normal_vbr_tests
{
  p_blue " NORMAL VBR mp3 tests ..."
  echo

  normal_test_functions=$(declare -F | grep " test_normal_vbr_" | awk '{ print $3 }')

  for test_func in $normal_test_functions;do
    eval $test_func
  done

  p_blue " NORMAL VBR tests DONE."
  echo
}

#main
export LANGUAGE="en"
start_date=$(date +%s)

run_normal_vbr_tests

end_date=$(date +%s)

p_time_diff_cyan $start_date $end_date "\t"
echo -e '\n'

exit 0

