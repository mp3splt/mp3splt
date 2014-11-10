#!/bin/bash

. ./utils.sh || exit 1

function _create_cue_file
{
  Q='"'
  echo "PERFORMER ${Q}GNU_Linux${Q}
TITLE ${Q}Gentoo${Q}
FILE ${Q}xx$OGG_FILE${Q} OGG
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
    INDEX 01 03:01:00
" > $CUE_FILE
}

function _create_incomplete_cue_file
{
  Q=$1
  echo "PERFORMER ${Q}GNU_Linux${Q}
TITLE ${Q}Gentoo${Q}
FILE ${Q}xx$OGG_FILE${Q} OGG
  TRACK 01 AUDIO
    TITLE ${Q}Our piano${Q}
    PERFORMER ${Q}First performer${Q}
    INDEX 01 00:00:00
  TRACK 02 AUDIO
    INDEX 01 01:43:00
  TRACK 03 AUDIO
    TITLE ${Q}Our laptop${Q}
    PERFORMER ${Q}Third performer${Q}
    INDEX 01 03:01:00
" > $CUE_FILE
}

function _test_cue_mode
{
  with_quotes=$1

  _create_cue_file $with_quotes

  remove_output_dir

  O_FILE="Kelly_Allyn__Whiskey_Can"

  if [[ ! -z $with_quotes ]];then
    test_name="cue mode & quotes"
  else
    test_name="cue mode"
  fi

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 reading informations from CUE file songs/test.cue ...

  Artist: GNU_Linux
  Album: Gentoo
  Tracks: 3

 cue file processed
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/First performer - 1 - Our piano.ogg\" created
   File \"$OUTPUT_DIR/GNU_Linux - 2 - Our guitar.ogg\" created
   File \"$OUTPUT_DIR/Third performer - 3 - Our laptop.ogg\" created
 file split (EOF)
$auto_adjust_warning"
  mp3splt_args="-d $OUTPUT_DIR -c $CUE_FILE $OGG_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/First performer - 1 - Our piano.ogg"
  check_current_ogg_length "1m:43.000s"
  check_current_file_size "2784310"
  check_all_ogg_tags "First performer" "Gentoo" "Our piano" "" "" "1" ""

  current_file="$OUTPUT_DIR/GNU_Linux - 2 - Our guitar.ogg"
  check_current_ogg_length "1m:18.000s"
  check_current_file_size "2011375"
  check_all_ogg_tags "GNU_Linux" "Gentoo" "Our guitar" "" "" "2" ""

  current_file="$OUTPUT_DIR/Third performer - 3 - Our laptop.ogg"
  check_current_ogg_length "0m:03.853s"
  check_current_file_size "33257"
  check_all_ogg_tags "Third performer" "Gentoo" "Our laptop" "" "" "3" ""

  print_ok
  echo
}

function test_cue_mode { _test_cue_mode; }
function test_cue_mode_with_quotes { _test_cue_mode \"; }

function test_cue_mode_and_pretend
{
  _create_cue_file

  remove_output_dir

  O_FILE="Kelly_Allyn__Whiskey_Can"

  test_name="cue mode & pretend"

  expected=" Pretending to split file 'songs/${O_FILE}.ogg' ...
 reading informations from CUE file songs/test.cue ...

  Artist: GNU_Linux
  Album: Gentoo
  Tracks: 3

 cue file processed
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/First performer - 1 - Our piano.ogg\" created
   File \"$OUTPUT_DIR/GNU_Linux - 2 - Our guitar.ogg\" created
   File \"$OUTPUT_DIR/Third performer - 3 - Our laptop.ogg\" created
 file split (EOF)
$auto_adjust_warning"
  mp3splt_args="-P -d $OUTPUT_DIR -c $CUE_FILE $OGG_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_is_empty

  print_ok
  echo
}

function test_cue_mode_and_cue_export
{
  _create_cue_file

  remove_output_dir

  O_FILE="Kelly_Allyn__Whiskey_Can"

  test_name="cue mode & cue export"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 reading informations from CUE file songs/test.cue ...

  Artist: GNU_Linux
  Album: Gentoo
  Tracks: 3

 cue file processed
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/First performer - 1 - Our piano.ogg\" created
   File \"$OUTPUT_DIR/GNU_Linux - 2 - Our guitar.ogg\" created
   File \"$OUTPUT_DIR/Third performer - 3 - Our laptop.ogg\" created
 file split (EOF)
 CUE file 'output/output_out.cue' created.
$auto_adjust_warning"
  mp3splt_args="-E output/out.cue -d $OUTPUT_DIR -c $CUE_FILE $OGG_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_number_of_files 4

  check_file_content "output/output_out.cue" 'TITLE "Gentoo"
PERFORMER "First performer"
FILE "songs/Kelly_Allyn__Whiskey_Can.ogg" OGG
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
    INDEX 01 03:01:00'

  current_file="$OUTPUT_DIR/First performer - 1 - Our piano.ogg"
  check_current_ogg_length "1m:43.000s"
  check_current_file_size "2784310"
  check_all_ogg_tags "First performer" "Gentoo" "Our piano" "" "" "1" ""

  current_file="$OUTPUT_DIR/GNU_Linux - 2 - Our guitar.ogg"
  check_current_ogg_length "1m:18.000s"
  check_current_file_size "2011375"
  check_all_ogg_tags "GNU_Linux" "Gentoo" "Our guitar" "" "" "2" ""

  current_file="$OUTPUT_DIR/Third performer - 3 - Our laptop.ogg"
  check_current_ogg_length "0m:03.853s"
  check_current_file_size "33257"
  check_all_ogg_tags "Third performer" "Gentoo" "Our laptop" "" "" "3" ""

  print_ok
  echo
}

function test_cue_mode_and_cue_export_and_pretend
{
  _create_cue_file

  remove_output_dir

  O_FILE="Kelly_Allyn__Whiskey_Can"

  test_name="cue mode & cue export & pretend"

  expected=" Pretending to split file 'songs/${O_FILE}.ogg' ...
 reading informations from CUE file songs/test.cue ...

  Artist: GNU_Linux
  Album: Gentoo
  Tracks: 3

 cue file processed
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/First performer - 1 - Our piano.ogg\" created
   File \"$OUTPUT_DIR/GNU_Linux - 2 - Our guitar.ogg\" created
   File \"$OUTPUT_DIR/Third performer - 3 - Our laptop.ogg\" created
 file split (EOF)
 CUE file 'output/output_out.cue' created.
$auto_adjust_warning"
  mp3splt_args="-P -E output/out.cue -d $OUTPUT_DIR -c $CUE_FILE $OGG_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_number_of_files 1

  check_file_content "output/output_out.cue" 'TITLE "Gentoo"
PERFORMER "First performer"
FILE "songs/Kelly_Allyn__Whiskey_Can.ogg" OGG
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
    INDEX 01 03:01:00'

  print_ok
  echo
}

function test_cue_mode_incomplete
{
  _create_incomplete_cue_file

  remove_output_dir

  O_FILE="Kelly_Allyn__Whiskey_Can"

  test_name="cue mode incomplete"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 reading informations from CUE file songs/test.cue ...

  Artist: GNU_Linux
  Album: Gentoo
  Tracks: 3

 cue file processed
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/First performer - 1 - Our piano.ogg\" created
   File \"$OUTPUT_DIR/GNU_Linux - 2 - .ogg\" created
   File \"$OUTPUT_DIR/Third performer - 3 - Our laptop.ogg\" created
 file split (EOF)
$auto_adjust_warning"
  mp3splt_args="-d $OUTPUT_DIR -c $CUE_FILE $OGG_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/First performer - 1 - Our piano.ogg"
  check_current_ogg_length "1m:43.000s"
  check_all_ogg_tags "First performer" "Gentoo" "Our piano" "" "" "1" ""

  current_file="$OUTPUT_DIR/GNU_Linux - 2 - .ogg"
  check_current_ogg_length "1m:18.000s"
  check_all_ogg_tags "GNU_Linux" "Gentoo" "" "" "" "2" ""

  current_file="$OUTPUT_DIR/Third performer - 3 - Our laptop.ogg"
  check_current_ogg_length "0m:03.853s"
  check_all_ogg_tags "Third performer" "Gentoo" "Our laptop" "" "" "3" ""

  print_ok
  echo
}

function test_cue_mode_quiet
{
  _create_incomplete_cue_file

  remove_output_dir

  O_FILE="Kelly_Allyn__Whiskey_Can"

  test_name="cue mode & quiet"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 reading informations from CUE file songs/test.cue ...

  Artist: GNU_Linux
  Album: Gentoo
  Tracks: 3

 cue file processed
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/First performer - 1 - Our piano.ogg\" created
   File \"$OUTPUT_DIR/GNU_Linux - 2 - .ogg\" created
   File \"$OUTPUT_DIR/Third performer - 3 - Our laptop.ogg\" created
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR -q -c $CUE_FILE $OGG_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  print_ok
  echo
}

function test_cue_mode_and_output_format
{
  _create_cue_file

  remove_output_dir

  O_FILE="Kelly_Allyn__Whiskey_Can"

  test_name="cue mode & output format"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 reading informations from CUE file songs/test.cue ...

  Artist: GNU_Linux
  Album: Gentoo
  Tracks: 3

 cue file processed
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/GNU_Linux/1/001-First performer-GNU_Linux-First performer-Gentoo-Our piano.ogg\" created
   File \"$OUTPUT_DIR/GNU_Linux/2/002-GNU_Linux-GNU_Linux--Gentoo-Our guitar.ogg\" created
   File \"$OUTPUT_DIR/GNU_Linux/3/003-Third performer-GNU_Linux-Third performer-Gentoo-Our laptop.ogg\" created
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR -o @a/@n/@n3-@A-@a-@p-@b-@t -q -c $CUE_FILE $OGG_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/GNU_Linux/1/001-First performer-GNU_Linux-First performer-Gentoo-Our piano.ogg"
  check_current_ogg_length "1m:43.000s"

  current_file="$OUTPUT_DIR/GNU_Linux/2/002-GNU_Linux-GNU_Linux--Gentoo-Our guitar.ogg"
  check_current_ogg_length "1m:18.000s"
 
  current_file="$OUTPUT_DIR/GNU_Linux/3/003-Third performer-GNU_Linux-Third performer-Gentoo-Our laptop.ogg"
  check_current_ogg_length "0m:03.853s"

  print_ok
  echo
}

function run_cue_mode_tests
{
  p_blue " CUE ogg tests ..."
  echo

  cue_mode_test_functions=$(declare -F | grep " test_cue" | awk '{ print $3 }')

  for test_func in $cue_mode_test_functions;do
    eval $test_func
  done

  p_blue " CUE ogg tests DONE."
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

