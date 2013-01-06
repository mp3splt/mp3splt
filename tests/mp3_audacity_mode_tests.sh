#!/bin/bash

. ./utils.sh || exit 1

function _create_audacity_file
{
  echo $'10.000000\t67.000000\tzero
67.000000\t127.000000\tfirst\n
149.000000\t206.000000\tsecond
206.000000\t236.000000\tthird' > $AUDACITY_FILE
}

function test_audacity
{
  _create_audacity_file

  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="audacity mode"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 reading informations from audacity labels file '$AUDACITY_FILE' ...
 audacity labels file processed
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/zero.mp3\" created
   File \"$OUTPUT_DIR/first.mp3\" created
   File \"$OUTPUT_DIR/second.mp3\" created
   File \"$OUTPUT_DIR/third.mp3\" created
 Processed 9036 frames - Sync errors: 0
 file split"
  mp3splt_args="-d $OUTPUT_DIR -A $AUDACITY_FILE $MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/zero.mp3"
  check_current_mp3_length "00.57"
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/first.mp3"
  check_current_mp3_length "01.00"
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "2" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/second.mp3"
  check_current_mp3_length "00.57"
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "3" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/third.mp3"
  check_current_mp3_length "00.30"
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "4" "http://www.jamendo.com/"

  print_ok
  echo
}

function test_audacity_and_pretend
{
  _create_audacity_file

  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="audacity mode & pretend"

  expected=" Pretending to split file 'songs/${M_FILE}.mp3' ...
 reading informations from audacity labels file '$AUDACITY_FILE' ...
 audacity labels file processed
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/zero.mp3\" created
   File \"$OUTPUT_DIR/first.mp3\" created
   File \"$OUTPUT_DIR/second.mp3\" created
   File \"$OUTPUT_DIR/third.mp3\" created
 Processed 9036 frames - Sync errors: 0
 file split"
  mp3splt_args="-P -d $OUTPUT_DIR -A $AUDACITY_FILE $MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_is_empty

  print_ok
  echo
}

function test_audacity_and_cue_export
{
  _create_audacity_file

  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="audacity mode & cue export"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 reading informations from audacity labels file '$AUDACITY_FILE' ...
 audacity labels file processed
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/zero.mp3\" created
   File \"$OUTPUT_DIR/first.mp3\" created
   File \"$OUTPUT_DIR/second.mp3\" created
   File \"$OUTPUT_DIR/third.mp3\" created
 Processed 9036 frames - Sync errors: 0
 file split
 CUE file 'output/output_out.cue' created."
  mp3splt_args="-E output/out.cue -d $OUTPUT_DIR -A $AUDACITY_FILE $MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

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
    REM NAME "zero"
    INDEX 01 00:10:00
  TRACK 02 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    REM ALBUM "Riez Noir"
    REM GENRE "Rock"
    REM DATE "2007"
    REM COMMENT "http://www.jamendo.com/"
    REM NAME "first"
    INDEX 01 01:07:00
  TRACK 03 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    REM ALBUM "Riez Noir"
    REM GENRE "Rock"
    REM DATE "2007"
    REM COMMENT "http://www.jamendo.com/"
    REM NAME "skip"
    REM NOKEEP
    INDEX 01 02:07:00
  TRACK 04 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    REM ALBUM "Riez Noir"
    REM GENRE "Rock"
    REM DATE "2007"
    REM COMMENT "http://www.jamendo.com/"
    REM NAME "second"
    INDEX 01 02:29:00
  TRACK 05 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    REM ALBUM "Riez Noir"
    REM GENRE "Rock"
    REM DATE "2007"
    REM COMMENT "http://www.jamendo.com/"
    REM NAME "third"
    INDEX 01 03:26:00
  TRACK 06 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    REM ALBUM "Riez Noir"
    REM GENRE "Rock"
    REM DATE "2007"
    REM COMMENT "http://www.jamendo.com/"
    REM NAME "skip"
    REM NOKEEP
    INDEX 01 03:56:00'

  current_file="$OUTPUT_DIR/zero.mp3"
  check_current_mp3_length "00.57"
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "1" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/first.mp3"
  check_current_mp3_length "01.00"
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "2" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/second.mp3"
  check_current_mp3_length "00.57"
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "3" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/third.mp3"
  check_current_mp3_length "00.30"
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "Today"\
  "2007" "Rock" "17" "4" "http://www.jamendo.com/"

  print_ok
  echo
}

function test_audacity_and_pretend_and_cue_export
{
  _create_audacity_file

  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="audacity mode & pretend & cue export"

  expected=" Pretending to split file 'songs/${M_FILE}.mp3' ...
 reading informations from audacity labels file '$AUDACITY_FILE' ...
 audacity labels file processed
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/zero.mp3\" created
   File \"$OUTPUT_DIR/first.mp3\" created
   File \"$OUTPUT_DIR/second.mp3\" created
   File \"$OUTPUT_DIR/third.mp3\" created
 Processed 9036 frames - Sync errors: 0
 file split
 CUE file 'output/output_out.cue' created."
  mp3splt_args="-P -E output/out.cue -d $OUTPUT_DIR -A $AUDACITY_FILE $MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

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
    REM NAME "zero"
    INDEX 01 00:10:00
  TRACK 02 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    REM ALBUM "Riez Noir"
    REM GENRE "Rock"
    REM DATE "2007"
    REM COMMENT "http://www.jamendo.com/"
    REM NAME "first"
    INDEX 01 01:07:00
  TRACK 03 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    REM ALBUM "Riez Noir"
    REM GENRE "Rock"
    REM DATE "2007"
    REM COMMENT "http://www.jamendo.com/"
    REM NAME "skip"
    REM NOKEEP
    INDEX 01 02:07:00
  TRACK 04 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    REM ALBUM "Riez Noir"
    REM GENRE "Rock"
    REM DATE "2007"
    REM COMMENT "http://www.jamendo.com/"
    REM NAME "second"
    INDEX 01 02:29:00
  TRACK 05 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    REM ALBUM "Riez Noir"
    REM GENRE "Rock"
    REM DATE "2007"
    REM COMMENT "http://www.jamendo.com/"
    REM NAME "third"
    INDEX 01 03:26:00
  TRACK 06 AUDIO
    TITLE "Today"
    PERFORMER "La Verue"
    REM ALBUM "Riez Noir"
    REM GENRE "Rock"
    REM DATE "2007"
    REM COMMENT "http://www.jamendo.com/"
    REM NAME "skip"
    REM NOKEEP
    INDEX 01 03:56:00'

  check_output_directory_number_of_files 1

  print_ok
  echo
}

function test_audacity_and_output_format
{
  _create_audacity_file

  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="audacity mode & output format"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 reading informations from audacity labels file '$AUDACITY_FILE' ...
 audacity labels file processed
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/1_Today_00:10:00__01:07:00.mp3\" created
   File \"$OUTPUT_DIR/2_Today_01:07:00__02:07:00.mp3\" created
   File \"$OUTPUT_DIR/3_Today_02:29:00__03:26:00.mp3\" created
   File \"$OUTPUT_DIR/4_Today_03:26:00__03:56:00.mp3\" created
 Processed 9036 frames - Sync errors: 0
 file split"
  mp3splt_args="-d $OUTPUT_DIR -o \"@n_@t_@m:@s:@h__@M:@S:@H\" -A $AUDACITY_FILE $MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/1_Today_00:10:00__01:07:00.mp3"
  check_current_mp3_length "00.57"

  current_file="$OUTPUT_DIR/2_Today_01:07:00__02:07:00.mp3"
  check_current_mp3_length "01.00"

  current_file="$OUTPUT_DIR/3_Today_02:29:00__03:26:00.mp3"
  check_current_mp3_length "00.57"

  current_file="$OUTPUT_DIR/4_Today_03:26:00__03:56:00.mp3"
  check_current_mp3_length "00.30"

  print_ok
  echo
}

function test_audacity_and_custom_tags
{
  _create_audacity_file

  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="audacity mode & custom tags"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 reading informations from audacity labels file '$AUDACITY_FILE' ...
 audacity labels file processed
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/zero.mp3\" created
   File \"$OUTPUT_DIR/first.mp3\" created
   File \"$OUTPUT_DIR/second.mp3\" created
   File \"$OUTPUT_DIR/third.mp3\" created
 Processed 9036 frames - Sync errors: 0
 file split"
  mp3splt_args="-d $OUTPUT_DIR -g \"%[@o,@N=6,@t=title1][@t=tit2][@t=tit3]\" -A $AUDACITY_FILE $MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/zero.mp3"
  check_current_mp3_length "00.57"
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "title1"\
  "2007" "Rock" "17" "6" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/first.mp3"
  check_current_mp3_length "01.00"
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "tit2"\
  "2007" "Rock" "17" "7" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/second.mp3"
  check_current_mp3_length "00.57"
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "tit3"\
  "2007" "Rock" "17" "8" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/third.mp3"
  check_current_mp3_length "00.30"
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "title1"\
  "2007" "Rock" "17" "9" "http://www.jamendo.com/"

  print_ok
  echo
}

function test_audacity_and_custom_tags_and_output_format
{
  _create_audacity_file

  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="audacity mode & custom tags & output format"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 reading informations from audacity labels file '$AUDACITY_FILE' ...
 audacity labels file processed
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/title1/La Verue_Riez Noir_title1_1.mp3\" created
   File \"$OUTPUT_DIR/tit2/La Verue_Riez Noir_tit2_2.mp3\" created
   File \"$OUTPUT_DIR/tit3/La Verue_Riez Noir_tit3_3.mp3\" created
   File \"$OUTPUT_DIR/title1/La Verue_Riez Noir_title1_4.mp3\" created
 Processed 9036 frames - Sync errors: 0
 file split"
  mp3splt_args="-d $OUTPUT_DIR -o \"@t/@a_@b_@t_@n\" -g \"%[@o,@N=6,@t=title1][@t=tit2][@t=tit3]\" -A $AUDACITY_FILE $MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/title1/La Verue_Riez Noir_title1_1.mp3"
  check_current_mp3_length "00.57"
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "title1"\
  "2007" "Rock" "17" "6" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/tit2/La Verue_Riez Noir_tit2_2.mp3"
  check_current_mp3_length "01.00"
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "tit2"\
  "2007" "Rock" "17" "7" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/tit3/La Verue_Riez Noir_tit3_3.mp3"
  check_current_mp3_length "00.57"
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "tit3"\
  "2007" "Rock" "17" "8" "http://www.jamendo.com/"

  current_file="$OUTPUT_DIR/title1/La Verue_Riez Noir_title1_4.mp3"
  check_current_mp3_length "00.30"
  check_all_mp3_tags_with_version 2 "La Verue" "Riez Noir" "title1"\
  "2007" "Rock" "17" "9" "http://www.jamendo.com/"

  print_ok
  echo
}

function run_audacity_mode_tests
{
  p_blue " AUDACITY tests ..."
  echo

  audacity_mode_test_functions=$(declare -F | grep " test_audacity" | awk '{ print $3 }')

  for test_func in $audacity_mode_test_functions;do
    eval $test_func
  done

  p_blue " AUDACITY tests DONE."
  echo
}

#main
export LC_ALL="C"
start_date=$(date +%s)

run_audacity_mode_tests

p_failed_tests

end_date=$(date +%s)

p_time_diff_cyan $start_date $end_date "\t"
echo -e '\n'

exit 0

