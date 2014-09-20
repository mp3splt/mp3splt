#!/bin/bash

. ./utils.sh || exit 1

function _create_cue_file
{
  Q='"'
  echo "PERFORMER ${Q}GNU_Linux${Q}
TITLE ${Q}Gentoo${Q}
FILE ${Q}xx$MP3_FILE${Q} MP3
  TRACK 01 AUDIO
    TITLE ${Q}Our piano${Q}
    PERFORMER ${Q}First performer${Q}  
    INDEX 01 00:00:00
  TRACK 02 AUDIO
    TITLE ${Q}Our guitar${Q}  
    INDEX 01   01:43:00  
  TRACK 03 AUDIO
    TITLE ${Q}Our laptop${Q}
    PERFORMER ${Q}Third performer${Q}
    INDEX 01 03:20:00
" > $CUE_FILE
}

function _create_cue_with_index1_file
{
  Q='"'
  echo "PERFORMER ${Q}GNU_Linux${Q}
TITLE ${Q}Gentoo${Q}
FILE ${Q}xx$MP3_FILE${Q} MP3
  TRACK 01 AUDIO
    TITLE ${Q}Our piano${Q}
    PERFORMER ${Q}First performer${Q}  
    INDEX 1 00:00:00
  TRACK 02 AUDIO
    TITLE ${Q}Our guitar${Q}  
    INDEX 1   01:43:00  
  TRACK 03 AUDIO
    TITLE ${Q}Our laptop${Q}
    PERFORMER ${Q}Third performer${Q}
    INDEX 1 03:20:00
" > $CUE_FILE
}

function _create_cue_file_with_frames
{
  Q='"'
  echo "PERFORMER ${Q}GNU_Linux${Q}
TITLE ${Q}Gentoo${Q}
FILE ${Q}xx$MP3_FILE${Q} MP3
  TRACK 01 AUDIO
    TITLE ${Q}Our piano${Q}
    PERFORMER ${Q}First performer${Q}  
    INDEX 01 00:00:08
  TRACK 02 AUDIO
    TITLE ${Q}Our guitar${Q}  
    INDEX 01   01:43:25  
  TRACK 03 AUDIO
    TITLE ${Q}Our laptop${Q}
    PERFORMER ${Q}Third performer${Q}
    INDEX 01 03:20:71
" > $CUE_FILE
}


function _create_incomplete_cue_file
{
  Q=$1
  echo "PERFORMER ${Q}GNU_Linux${Q}
TITLE ${Q}Gentoo${Q}
FILE ${Q}xx$MP3_FILE${Q} MP3
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

function _create_mp3splt_gtk_exported_cue
{
  Q='"'
  echo "REM CREATOR ${Q}MP3SPLT_GTK${Q}
FILE ${Q}${Q}
        TRACK 01 AUDIO
                TITLE ${Q}first${Q}
                INDEX 01 0:00:00
                REM NAME ${Q}first_name${Q}
        TRACK 02 AUDIO
                TITLE ${Q}skip${Q}
                REM NOKEEP
                INDEX 01 1:00:00
        TRACK 03 AUDIO
                TITLE ${Q}second${Q}
                INDEX 01 1:43:00
        TRACK 04 AUDIO
                TITLE ${Q}third${Q}
                INDEX 01 3:01:00
" > $CUE_FILE
}

function _create_exported_cue_with_frames
{
  Q='"'
  echo "FILE ${Q}songs/La_Verue__Today.mp3${Q} MP3
   TRACK 01 AUDIO
     TITLE ${Q}first${Q}
     REM TRACK ${Q}1${Q}
     REM NAME ${Q}- 1 - first${Q}
     INDEX 01 00:00:00
   TRACK 02 AUDIO
     TITLE ${Q}third${Q}
     REM TRACK ${Q}4${Q}
     REM NAME ${Q}- 2 - second${Q}
     INDEX 01 01:43:48
   TRACK 03 AUDIO
     TITLE ${Q}${Q}
     PERFORMER ${Q}${Q}
     REM NAME ${Q}- 3 - third${Q}
     INDEX 01 03:01:73
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
  check_current_file_size "2292229"
  check_all_mp3_tags_with_version "2" "First performer" "Gentoo" "Our piano"\
  "None" "" "" "1" ""

  current_file="$OUTPUT_DIR/GNU_Linux - 2 - Our guitar.mp3"
  check_current_mp3_length "01.37"
  check_current_file_has_xing
  check_current_file_size "2287966"
  check_all_mp3_tags_with_version "2" "GNU_Linux" "Gentoo" "Our guitar"\
  "None" "" "" "2" ""

  current_file="$OUTPUT_DIR/Third performer - 3 - Our laptop.mp3"
  check_current_mp3_length "00.45"
  check_current_file_has_xing
  check_current_file_size "1031927"
  check_all_mp3_tags_with_version "2" "Third performer" "Gentoo" "Our laptop"\
  "None" "" "" "3" ""

  print_ok
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

  print_ok
  echo
}

function test_cue_with_index1
{
  _create_cue_with_index1_file

  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="cue with index 1 instead of index 01"

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

  current_file="$OUTPUT_DIR/GNU_Linux - 2 - Our guitar.mp3"
  check_current_mp3_length "01.37"

  current_file="$OUTPUT_DIR/Third performer - 3 - Our laptop.mp3"
  check_current_mp3_length "00.45"

  print_ok
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
    REM ALBUM "Gentoo"
    REM TRACK "1"
    REM NAME "First performer - 1 - Our piano"
    INDEX 01 00:00:00
  TRACK 02 AUDIO
    TITLE "Our guitar"
    PERFORMER "GNU_Linux"
    REM ALBUM "Gentoo"
    REM TRACK "2"
    REM NAME "GNU_Linux - 2 - Our guitar"
    INDEX 01 01:43:00
  TRACK 03 AUDIO
    TITLE "Our laptop"
    PERFORMER "Third performer"
    REM ALBUM "Gentoo"
    REM TRACK "3"
    REM NAME "Third performer - 3 - Our laptop"
    INDEX 01 03:20:00'

  current_file="$OUTPUT_DIR/First performer - 1 - Our piano.mp3"
  check_current_mp3_length "01.43"
  check_current_file_has_xing
  check_current_file_size "2292229"
  check_all_mp3_tags_with_version "2" "First performer" "Gentoo" "Our piano"\
  "None" "" "" "1" ""

  current_file="$OUTPUT_DIR/GNU_Linux - 2 - Our guitar.mp3"
  check_current_mp3_length "01.37"
  check_current_file_has_xing
  check_current_file_size "2287966"
  check_all_mp3_tags_with_version "2" "GNU_Linux" "Gentoo" "Our guitar"\
  "None" "" "" "2" ""

  current_file="$OUTPUT_DIR/Third performer - 3 - Our laptop.mp3"
  check_current_mp3_length "00.45"
  check_current_file_has_xing
  check_current_file_size "1031927"
  check_all_mp3_tags_with_version "2" "Third performer" "Gentoo" "Our laptop"\
  "None" "" "" "3" ""

  print_ok
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
    REM ALBUM "Gentoo"
    REM TRACK "1"
    REM NAME "First performer - 1 - Our piano"
    INDEX 01 00:00:00
  TRACK 02 AUDIO
    TITLE "Our guitar"
    PERFORMER "GNU_Linux"
    REM ALBUM "Gentoo"
    REM TRACK "2"
    REM NAME "GNU_Linux - 2 - Our guitar"
    INDEX 01 01:43:00
  TRACK 03 AUDIO
    TITLE "Our laptop"
    PERFORMER "Third performer"
    REM ALBUM "Gentoo"
    REM TRACK "3"
    REM NAME "Third performer - 3 - Our laptop"
    INDEX 01 03:20:00'

  print_ok
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
  "None" "" "" "1" ""

  current_file="$OUTPUT_DIR/GNU_Linux - 2 - .mp3"
  check_current_mp3_length "01.37"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "GNU_Linux" "Gentoo" ""\
  "None" "" "" "2" ""

  current_file="$OUTPUT_DIR/Third performer - 3 - Our laptop.mp3"
  check_current_mp3_length "00.45"
  check_current_file_has_xing
  check_all_mp3_tags_with_version "2" "Third performer" "Gentoo" "Our laptop"\
  "None" "" "" "3" ""

  print_ok
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

  print_ok
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
   File \"$OUTPUT_DIR/GNU_Linux/1/001-First performer-GNU_Linux-First performer-Gentoo-Our piano.mp3\" created
   File \"$OUTPUT_DIR/GNU_Linux/2/002-GNU_Linux-GNU_Linux--Gentoo-Our guitar.mp3\" created
   File \"$OUTPUT_DIR/GNU_Linux/3/003-Third performer-GNU_Linux-Third performer-Gentoo-Our laptop.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR -o @a/@n/@n3-@A-@a-@p-@b-@t -q -c $CUE_FILE $MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/GNU_Linux/1/001-First performer-GNU_Linux-First performer-Gentoo-Our piano.mp3"
  check_current_mp3_length "01.43"

  current_file="$OUTPUT_DIR/GNU_Linux/2/002-GNU_Linux-GNU_Linux--Gentoo-Our guitar.mp3"
  check_current_mp3_length "01.37"
 
  current_file="$OUTPUT_DIR/GNU_Linux/3/003-Third performer-GNU_Linux-Third performer-Gentoo-Our laptop.mp3"
  check_current_mp3_length "00.45"

  print_ok
  echo
}

function test_cue_exported_mp3splt_gtk_cue_mode_and_output_format
{
  _create_mp3splt_gtk_exported_cue

  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="exported mp3splt-gtk cue & output format"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 reading informations from CUE file songs/test.cue ...
  Tracks: 4

 cue file processed
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/1_first.mp3\" created
   File \"$OUTPUT_DIR/2_second.mp3\" created
   File \"$OUTPUT_DIR/3_third.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR -o @n_@t -q -c $CUE_FILE $MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/1_first.mp3"
  check_current_mp3_length "01.00"

  current_file="$OUTPUT_DIR/2_second.mp3"
  check_current_mp3_length "01.18"
 
  current_file="$OUTPUT_DIR/3_third.mp3"
  check_current_mp3_length "01.04"

  print_ok
  echo
}

function test_cue_with_frames
{
  _create_cue_file_with_frames

  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="cue mode with frames"

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
   File \"$OUTPUT_DIR/1__00_00_11__01_43_33.mp3\" created
   File \"$OUTPUT_DIR/2__01_43_33__03_20_95.mp3\" created
   File \"$OUTPUT_DIR/3__03_20_95__04_05_58.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)
$auto_adjust_warning"
  mp3splt_args="-d $OUTPUT_DIR -o @n__@m_@s_@h__@M_@S_@H -c $CUE_FILE $MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  print_ok
  echo
}

function test_cue_parse_and_export_same_as_input
{
  _create_exported_cue_with_frames

  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="cue mode & export cue same as input"

  expected=" Pretending to split file 'songs/${M_FILE}.mp3' ...
 reading informations from CUE file songs/test.cue ...
  Tracks: 3

 cue file processed
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/- 1 - first.mp3\" created
   File \"$OUTPUT_DIR/- 2 - second.mp3\" created
   File \"$OUTPUT_DIR/- 3 - third.mp3\" created
 Processed 9402 frames - Sync errors: 0
 file split (EOF)
 CUE file 'output/output_out.cue' created.
$auto_adjust_warning"
  mp3splt_args="-P -E output/out.cue -d $OUTPUT_DIR -c $CUE_FILE $MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  Q='"'

  check_file_content "output/output_out.cue" "FILE ${Q}songs/La_Verue__Today.mp3${Q} MP3
  TRACK 01 AUDIO
    TITLE ${Q}first${Q}
    REM TRACK ${Q}1${Q}
    REM NAME ${Q}- 1 - first${Q}
    INDEX 01 00:00:00
  TRACK 02 AUDIO
    TITLE ${Q}third${Q}
    REM TRACK ${Q}4${Q}
    REM NAME ${Q}- 2 - second${Q}
    INDEX 01 01:43:48
  TRACK 03 AUDIO
    REM NAME ${Q}- 3 - third${Q}
    INDEX 01 03:01:73"

  print_ok
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

p_failed_tests

end_date=$(date +%s)

p_time_diff_cyan $start_date $end_date "\t"
echo -e '\n'

exit 0

