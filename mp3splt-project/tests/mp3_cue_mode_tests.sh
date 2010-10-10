#!/bin/bash

. ./utils.sh || exit 1

function _create_cue_file
{
  Q='"'
  echo "PERFORMER ${Q}GNU_Linux${Q}
TITLE ${Q}Gentoo${Q}
FILE ${Q}$MP3_FILE${Q} MP3
  TRACK 01 AUDIO
    TITLE ${Q}Our piano${Q}
    PERFORMER ${Q}First performer${Q}
    INDEX 01 00:00:00
  TRACK 02 AUDIO
    TITLE ${Q}Our guitar${Q}
    INDEX 01 01:43:00
  TRACK 03 AUDIO
    TITLE ${Q}Our laptop${Q}
    PERFORMER ${Q}Third performer${Q}
    INDEX 01 03:20:00
" > $CUE_FILE
}

function _create_incomplete_cue_file
{
  Q=$1
  echo "PERFORMER ${Q}GNU_Linux${Q}
TITLE ${Q}Gentoo${Q}
FILE ${Q}$MP3_FILE${Q} MP3
  TRACK 01 AUDIO
    TITLE ${Q}Our piano${Q}
    PERFORMER ${Q}First performer${Q}
    INDEX 01 00:00:00
  TRACK 02 AUDIO
    INDEX 01 01:43:00
  TRACK 03 AUDIO
    TITLE ${Q}Our laptop${Q}
    PERFORMER ${Q}Third performer${Q}
    INDEX 01 03:20:00
" > $CUE_FILE
}

function _test_cue_mode
{
  with_quotes=$1

  _create_cue_file $with_quotes

  remove_output_dir

  M_FILE="La_Verue__Today"

  if [[ ! -z $with_quotes ]];then
    test_name="cue mode & quotes"
  else
    test_name="cue mode"
  fi

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 reading informations from CUE file songs/test.cue ...

  Artist: GNU_Linux
  Album: Gentoo
  Tracks: 3

 cue file processed
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/First performer - 1 - Our piano.mp3\" created
   File \"$OUTPUT_DIR/GNU_Linux - 2 - Our guitar.mp3\" created
   File \"$OUTPUT_DIR/Third performer - 3 - Our laptop.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)
$auto_adjust_warning"
  mp3splt_args="-d $OUTPUT_DIR -c $CUE_FILE $MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/First performer - 1 - Our piano.mp3"
  check_current_mp3_length "01.43"
  check_current_file_has_xing
  check_current_file_size "2292252"
  check_all_mp3_tags_with_version "2" "First performer" "Gentoo" "Our piano"\
  "None" "Other" "12" "1" ""

  current_file="$OUTPUT_DIR/GNU_Linux - 2 - Our guitar.mp3"
  check_current_mp3_length "01.37"
  check_current_file_has_xing
  check_current_file_size "2287989"
  check_all_mp3_tags_with_version "2" "GNU_Linux" "Gentoo" "Our guitar"\
  "None" "Other" "12" "2" ""

  current_file="$OUTPUT_DIR/Third performer - 3 - Our laptop.mp3"
  check_current_mp3_length "00.45"
  check_current_file_has_xing
  check_current_file_size "1031950"
  check_all_mp3_tags_with_version "2" "Third performer" "Gentoo" "Our laptop"\
  "None" "Other" "12" "3" ""

  p_green "OK"
  echo
}

function test_cue_mode { _test_cue_mode; }
function test_cue_mode_with_quotes { _test_cue_mode \"; }

function test_cue_mode_and_pretend
{
  _create_cue_file

  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="cue mode & pretend"

  expected=" Pretending to split file 'songs/${M_FILE}.mp3' ...
 reading informations from CUE file songs/test.cue ...

  Artist: GNU_Linux
  Album: Gentoo
  Tracks: 3

 cue file processed
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/First performer - 1 - Our piano.mp3\" created
   File \"$OUTPUT_DIR/GNU_Linux - 2 - Our guitar.mp3\" created
   File \"$OUTPUT_DIR/Third performer - 3 - Our laptop.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)
$auto_adjust_warning"
  mp3splt_args="-P -d $OUTPUT_DIR -c $CUE_FILE $MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_is_empty

  p_green "OK"
  echo
}

function test_cue_mode_and_cue_export
{
  _create_cue_file

  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="cue mode & cue export"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 reading informations from CUE file songs/test.cue ...

  Artist: GNU_Linux
  Album: Gentoo
  Tracks: 3

 cue file processed
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/First performer - 1 - Our piano.mp3\" created
   File \"$OUTPUT_DIR/GNU_Linux - 2 - Our guitar.mp3\" created
   File \"$OUTPUT_DIR/Third performer - 3 - Our laptop.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)
 CUE file 'output/output_out.cue' created.
$auto_adjust_warning"
  mp3splt_args="-E output/out.cue -d $OUTPUT_DIR -c $CUE_FILE $MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_number_of_files 4

  check_file_content "output/output_out.cue" 'TITLE "Gentoo"
PERFORMER "First performer"
FILE "songs/La_Verue__Today.mp3" MP3
  TRACK 01 AUDIO
    TITLE "Our piano"
    PERFORMER "First performer"
    INDEX 01 00:00:00
  TRACK 02 AUDIO
    TITLE "Our guitar"
    PERFORMER "GNU_Linux"
    INDEX 01 01:43:00
  TRACK 03 AUDIO
    TITLE "Our laptop"
    PERFORMER "Third performer"
    INDEX 01 03:20:00'

  current_file="$OUTPUT_DIR/First performer - 1 - Our piano.mp3"
  check_current_mp3_length "01.43"
  check_current_file_has_xing
  check_current_file_size "2292252"
  check_all_mp3_tags_with_version "2" "First performer" "Gentoo" "Our piano"\
  "None" "Other" "12" "1" ""

  current_file="$OUTPUT_DIR/GNU_Linux - 2 - Our guitar.mp3"
  check_current_mp3_length "01.37"
  check_current_file_has_xing
  check_current_file_size "2287989"
  check_all_mp3_tags_with_version "2" "GNU_Linux" "Gentoo" "Our guitar"\
  "None" "Other" "12" "2" ""

  current_file="$OUTPUT_DIR/Third performer - 3 - Our laptop.mp3"
  check_current_mp3_length "00.45"
  check_current_file_has_xing
  check_current_file_size "1031950"
  check_all_mp3_tags_with_version "2" "Third performer" "Gentoo" "Our laptop"\
  "None" "Other" "12" "3" ""

  p_green "OK"
  echo
}

function test_cue_mode_and_cue_export_and_pretend
{
  _create_cue_file

  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="cue mode & cue export & pretend"

  expected=" Pretending to split file 'songs/${M_FILE}.mp3' ...
 reading informations from CUE file songs/test.cue ...

  Artist: GNU_Linux
  Album: Gentoo
  Tracks: 3

 cue file processed
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/First performer - 1 - Our piano.mp3\" created
   File \"$OUTPUT_DIR/GNU_Linux - 2 - Our guitar.mp3\" created
   File \"$OUTPUT_DIR/Third performer - 3 - Our laptop.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)
 CUE file 'output/output_out.cue' created.
$auto_adjust_warning"
  mp3splt_args="-P -E output/out.cue -d $OUTPUT_DIR -c $CUE_FILE $MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_number_of_files 1

  check_file_content "output/output_out.cue" 'TITLE "Gentoo"
PERFORMER "First performer"
FILE "songs/La_Verue__Today.mp3" MP3
  TRACK 01 AUDIO
    TITLE "Our piano"
    PERFORMER "First performer"
    INDEX 01 00:00:00
  TRACK 02 AUDIO
    TITLE "Our guitar"
    PERFORMER "GNU_Linux"
    INDEX 01 01:43:00
  TRACK 03 AUDIO
    TITLE "Our laptop"
    PERFORMER "Third performer"
    INDEX 01 03:20:00'

  p_green "OK"
  echo
}

function test_cue_mode_incomplete
{
  _create_incomplete_cue_file

  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="cue mode incomplete"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 reading informations from CUE file songs/test.cue ...

  Artist: GNU_Linux
  Album: Gentoo
  Tracks: 3

 cue file processed
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/First performer - 1 - Our piano.mp3\" created
   File \"$OUTPUT_DIR/GNU_Linux - 2 - .mp3\" created
   File \"$OUTPUT_DIR/Third performer - 3 - Our laptop.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)
$auto_adjust_warning"
  mp3splt_args="-d $OUTPUT_DIR -c $CUE_FILE $MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/First performer - 1 - Our piano.mp3"
  check_current_mp3_length "01.43"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "First performer" "Gentoo" "Our piano"\
  "None" "Other" "12" "1" ""

  current_file="$OUTPUT_DIR/GNU_Linux - 2 - .mp3"
  check_current_mp3_length "01.37"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "GNU_Linux" "Gentoo" ""\
  "None" "Other" "12" "2" ""

  current_file="$OUTPUT_DIR/Third performer - 3 - Our laptop.mp3"
  check_current_mp3_length "00.45"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "Third performer" "Gentoo" "Our laptop"\
  "None" "Other" "12" "3" ""

  p_green "OK"
  echo
}

function test_cue_mode_quiet
{
  _create_cue_file

  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="cue mode & quiet"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 reading informations from CUE file songs/test.cue ...

  Artist: GNU_Linux
  Album: Gentoo
  Tracks: 3

 cue file processed
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/First performer - 1 - Our piano.mp3\" created
   File \"$OUTPUT_DIR/GNU_Linux - 2 - Our guitar.mp3\" created
   File \"$OUTPUT_DIR/Third performer - 3 - Our laptop.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR -q -c $CUE_FILE $MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  p_green "OK"
  echo
}

function test_cue_mode_and_output_format
{
  _create_cue_file

  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="cue mode & output format"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 reading informations from CUE file songs/test.cue ...

  Artist: GNU_Linux
  Album: Gentoo
  Tracks: 3

 cue file processed
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/GNU_Linux/1/001-GNU_Linux-First performer-Gentoo-Our piano.mp3\" created
   File \"$OUTPUT_DIR/GNU_Linux/2/002-GNU_Linux--Gentoo-Our guitar.mp3\" created
   File \"$OUTPUT_DIR/GNU_Linux/3/003-GNU_Linux-Third performer-Gentoo-Our laptop.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR -o @a/@n/@n3-@a-@p-@b-@t -q -c $CUE_FILE $MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/GNU_Linux/1/001-GNU_Linux-First performer-Gentoo-Our piano.mp3"
  check_current_mp3_length "01.43"

  current_file="$OUTPUT_DIR/GNU_Linux/2/002-GNU_Linux--Gentoo-Our guitar.mp3"
  check_current_mp3_length "01.37"
 
  current_file="$OUTPUT_DIR/GNU_Linux/3/003-GNU_Linux-Third performer-Gentoo-Our laptop.mp3"
  check_current_mp3_length "00.45"

  p_green "OK"
  echo
}

function run_cue_mode_tests
{
  p_blue " CUE tests ..."
  echo

  cue_mode_test_functions=$(declare -F | grep " test_cue" | awk '{ print $3 }')

  for test_func in $cue_mode_test_functions;do
    eval $test_func
  done

  p_blue " CUE tests DONE."
  echo
}

#main
export LC_ALL="C"
start_date=$(date +%s)

run_cue_mode_tests

end_date=$(date +%s)

p_time_diff_cyan $start_date $end_date "\t"
echo -e '\n'

exit 0

