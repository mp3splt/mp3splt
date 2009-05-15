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
  mp3splt_args="$tags_option -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

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
  current_tags_version=2
  rm -f $OUTPUT_DIR/*

  test_name="no xing"
  M_FILE="La_Verue__Today"

  mp3splt_args="-x -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" ""

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
  current_tags_version=2
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
  mp3splt_args="-m playlist.m3u -d $OUTPUT_DIR/m3u $MP3_FILE 1.0 2.0.2 3.5 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  expected="La_Verue__Today_01m_00s__02m_00s_20h.mp3
La_Verue__Today_02m_00s_20h__03m_05s.mp3
La_Verue__Today_03m_05s__04m_05s_58h.mp3"
  check_file_content "$OUTPUT_DIR/m3u/playlist.m3u" "$expected"

  p_green "OK"
  echo
}

function test_normal_create_directories
{
  current_tags_version=2
  rm -rf $OUTPUT_DIR/*

  test_name="create directories"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: frame mode enabled
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

function test_normal_custom_tags
{
  current_tags_version=2
  rm -rf $OUTPUT_DIR/*

  test_name="custom tags"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: frame mode enabled
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_00s__03m_05s.mp3\" created
 Processed 7083 frames - Sync errors: 0
 file split"
  tags_option="[@a=a1,@b=b1,@t=t1,@y=2000,@c=my_comment,@n=10][]%[@o,@b=album,@N=7][@a=custom_artist][@o,@n=20]"
  mp3splt_args="-d $OUTPUT_DIR -g $tags_option $MP3_FILE 0.5 1.0 1.5 2.0 3.0 3.5"
  run_check_output "$mp3splt_args" "$expected"

  id_str="id "

  current_file="$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3"
  check_all_current_mp3_tags "a1" "b1" "t1"\
  "2000" "Other (${id_str}12)" "10" "my_comment"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3"
  check_all_current_mp3_tags "" "" "" "None" "Other (${id_str}12)" "2" ""

  current_file="$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3"
  check_all_current_mp3_tags "La Verue" "album" "Today"\
  "2007" "Rock (${id_str}17)" "7" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3"
  check_all_current_mp3_tags "custom_artist" "album" "Today"\
  "2007" "Rock (${id_str}17)" "8" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_00s__03m_05s.mp3"
  check_all_current_mp3_tags "La Verue" "Riez Noir" "Today"\
  "2007" "Rock (${id_str}17)" "20" "http://www.jamendo.com/"

  p_green "OK"
  echo
}

function test_normal_custom_tags_multiple_percent
{
  current_tags_version=2
  rm -rf $OUTPUT_DIR/*

  test_name="custom tags multiple percent"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: frame mode enabled
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_00s__03m_05s.mp3\" created
 Processed 7083 frames - Sync errors: 0
 file split"
  tags_option="%[@a=a1,@b=b1,@n=10][]%[@o,@b=album,@N=7][@a=custom_artist][@o,@n=20]"
  mp3splt_args="-d $OUTPUT_DIR -g $tags_option $MP3_FILE 0.5 1.0 1.5 2.0 3.0 3.5"
  run_check_output "$mp3splt_args" "$expected"

  id_str="id "

  current_file="$OUTPUT_DIR/${M_FILE}_00m_05s__01m_00s.mp3"
  check_all_current_mp3_tags "a1" "b1" "" "None" "Other (${id_str}12)" "10" ""

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__01m_05s.mp3"
  check_all_current_mp3_tags "a1" "b1" "" "None" "Other (${id_str}12)" "2" ""

  current_file="$OUTPUT_DIR/${M_FILE}_01m_05s__02m_00s.mp3"
  check_all_current_mp3_tags "La Verue" "album" "Today"\
  "2007" "Rock (${id_str}17)" "7" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s__03m_00s.mp3"
  check_all_current_mp3_tags "custom_artist" "album" "Today"\
  "2007" "Rock (${id_str}17)" "8" "http://www.jamendo.com/"

  p_green "OK"
  echo
}

function test_normal_overlap_split
{
  current_tags_version=2
  rm -rf $OUTPUT_DIR/*

  test_name="overlap splitpoints"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: frame mode enabled
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
 info: overlapping split files with 0.30.0
   File \"$OUTPUT_DIR/${M_FILE}_01m_00s__02m_30s_20h.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_02m_00s_20h__04m_00s.mp3\" created
   File \"$OUTPUT_DIR/${M_FILE}_03m_30s__04m_05s_58h.mp3\" created
 file split (EOF)"
  mp3splt_args="-O 0.30 -d $OUTPUT_DIR $MP3_FILE 1.0 2.0.2 3.30 EOF"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${M_FILE}_01m_00s__02m_30s_20h.mp3"
  check_current_mp3_length "01.30"
  check_current_file_size "2007679"

  current_file="$OUTPUT_DIR/${M_FILE}_02m_00s_20h__04m_00s.mp3"
  check_current_mp3_length "01.59"
  check_current_file_size "2823096"

  current_file="$OUTPUT_DIR/${M_FILE}_03m_30s__04m_05s_58h.mp3"
  check_current_mp3_length "00.35"
  check_current_file_size "809280"

  p_green "OK"
  echo
}

function test_normal_stdin
{
  current_tags_version=2
  rm -rf $OUTPUT_DIR/*

  test_name="stdin"
  M_FILE="La_Verue__Today"

  expected=" Processing file '-' ...
 warning: stdin '-' is supposed to be mp3 stream.
 info: frame mode enabled
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE NS - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/-_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/-_02m_00s_20h__03m_30s.mp3\" created
   File \"$OUTPUT_DIR/-_03m_30s__04m_05s_58h.mp3\" created
 Processed 9400 frames - Sync errors: 1
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
  check_current_file_size "806141"

  p_green "OK"
  echo
}

function test_normal_stdin_and_tags
{
  current_tags_version=$1
  rm -rf $OUTPUT_DIR/*

  test_name="stdin and tags v$current_tags_version"
  M_FILE="La_Verue__Today"

  expected=" Processing file '-' ...
 warning: stdin '-' is supposed to be mp3 stream.
 info: frame mode enabled
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE NS - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/-_01m_00s__02m_00s_20h.mp3\" created
   File \"$OUTPUT_DIR/-_02m_00s_20h__03m_30s.mp3\" created
   File \"$OUTPUT_DIR/-_03m_30s__04m_05s_58h.mp3\" created
 Processed 9400 frames - Sync errors: 1
 file split (EOF)"
  mp3splt_args="-g %[@a=a1,@b=b1,@y=1070,@N=1] -$current_tags_version -d $OUTPUT_DIR - 1.0 2.0.2 3.30 EOF"
  run_custom_check_output "cat songs/${M_FILE}.mp3 | $MP3SPLT" "$mp3splt_args" "$expected"

  id_str=""
  if [[ $current_tags_version -eq 2 ]];then
    id_str="id "
  fi

  current_file="$OUTPUT_DIR/-_01m_00s__02m_00s_20h.mp3"
  check_all_current_mp3_tags "a1" "b1" "" "1070" "Other (${id_str}12)" "1" ""
  check_current_mp3_length "01.00"
  check_current_file_has_xing
  if [[ $current_tags_version -eq 2 ]];then
    check_current_file_size "1366020"
  else
    check_current_file_size "1366045"
  fi

  current_file="$OUTPUT_DIR/-_02m_00s_20h__03m_30s.mp3"
  check_all_current_mp3_tags "a1" "b1" "" "1070" "Other (${id_str}12)" "2" ""
  check_current_mp3_length "01.29"
  check_current_file_has_xing
  if [[ $current_tags_version -eq 2 ]];then
    check_current_file_size "2113837"
  else
    check_current_file_size "2113862"
  fi

  current_file="$OUTPUT_DIR/-_03m_30s__04m_05s_58h.mp3"
  check_all_current_mp3_tags "a1" "b1" "" "1070" "Other (${id_str}12)" "3" ""
  check_current_mp3_length "00.35"
  check_current_file_has_xing
  if [[ $current_tags_version -eq 2 ]];then
    check_current_file_size "806244"
  else
    check_current_file_size "806269"
  fi

  p_green "OK"
  echo
}

function test_normal_stdin_and_tags_v1 { test_normal_stdin_and_tags 1; }
function test_normal_stdin_and_tags_v2 { test_normal_stdin_and_tags 2; }

function test_normal_output_fnames_and_custom_tags
{
  current_tags_version=2
  rm -rf $OUTPUT_DIR/*

  test_name="output fnames & custom tags"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: frame mode enabled
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/a1_b1_t1_1_10_${M_FILE} 00:05:00 01:00:30.mp3\" created
   File \"$OUTPUT_DIR/___2_2_${M_FILE} 01:00:30 01:05:00.mp3\" created
   File \"$OUTPUT_DIR/La Verue_album_Today_3_7_${M_FILE} 01:05:00 02:00:00.mp3\" created
 Processed 4594 frames - Sync errors: 0
 file split"
  tags_option="[@a=a1,@b=b1,@t=t1,@y=2000,@c=my_comment,@n=10][]%[@o,@b=album,@N=7]"
  output_option="\"@a_@b_@t_@n_@N_@f+@m:@s:@h @M:@S:@H\""
  mp3splt_args="-d $OUTPUT_DIR -g $tags_option -o $output_option $MP3_FILE 0.5 1.0.30 1.5 2.0"
  run_check_output "$mp3splt_args" "$expected"

  id_str="id "

  check_if_file_exist "$OUTPUT_DIR/a1_b1_t1_1_10_${M_FILE} 00:05:00 01:00:30.mp3"
  check_if_file_exist "$OUTPUT_DIR/___2_2_${M_FILE} 01:00:30 01:05:00.mp3"
  check_if_file_exist "$OUTPUT_DIR/La Verue_album_Today_3_7_${M_FILE} 01:05:00 02:00:00.mp3"

  p_green "OK"
  echo
}

function test_normal_output_fnames_and_dirs
{
  current_tags_version=2
  rm -rf $OUTPUT_DIR/*

  test_name="output fnames & directories"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: frame mode enabled
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

function test_normal_output_fnames_and_custom_tags_dirs
{
  current_tags_version=2
  rm -rf $OUTPUT_DIR/*

  test_name="output fnames & custom tags & directories"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: frame mode enabled
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

function test_normal_stdout
{
  current_tags_version=2
  rm -rf $OUTPUT_DIR/*

  test_name="stdout"
  M_FILE="La_Verue__Today"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: frame mode enabled
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"-\" created
 Processed 5750 frames - Sync errors: 0
 file split"
  mp3splt_args="-o - $MP3_FILE 1.0 2.30.2"
  run_custom_check_output "$MP3SPLT $mp3splt_args > $OUTPUT_DIR/stdout.mp3" "" "$expected"

  current_file="$OUTPUT_DIR/stdout.mp3"
  check_current_mp3_length "01.30"
  check_current_file_size "2007679"

  p_green "OK"
  echo
}

function test_normal_stdout_multiple_splitpoints
{
  current_tags_version=2
  rm -rf $OUTPUT_DIR/*

  test_name="stdout & splitpoints > 2"
  M_FILE="La_Verue__Today"

  expected=" Warning: multiple splitpoints with stdout !
 Processing file 'songs/${M_FILE}.mp3' ...
 info: frame mode enabled
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"-\" created
   File \"-\" created
 Processed 6508 frames - Sync errors: 0
 file split"
  mp3splt_args="-o - $MP3_FILE 1.0 2.30.2 2.50"
  run_custom_check_output "$MP3SPLT $mp3splt_args > $OUTPUT_DIR/stdout.mp3" "" "$expected"

  current_file="$OUTPUT_DIR/stdout.mp3"
  check_current_mp3_length "01.30"
  check_current_file_size "2489578"

  p_green "OK"
  echo
}


function run_normal_mode_tests
{
  date
  echo
  p_blue " NORMAL mp3 tests ..."
  echo

  normal_tests_to_run="\
original_tags_v2 \
id3v1 \
id3v2 \
no_tags \
no_xing \
m3u \
create_directories \
custom_tags_multiple_percent \
custom_tags \
overlap_split \
stdin \
stdin_and_tags_v1 \
stdin_and_tags_v2 \
output_fnames_and_dirs \
output_fnames_and_custom_tags \
output_fnames_and_custom_tags_dirs \
stdout \
stdout_multiple_splitpoints"

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

