#!/bin/bash

. ./utils.sh || exit 1

function _create_audacity_file
{
  echo $'10.000000\t67.128000\tzero
67.128000\t127\tfirst\n
149.000000\t166.000000\tsecond
166.000000\t236.000000\t' > $AUDACITY_FILE
}

function test_audacity
{
  _create_audacity_file

  remove_output_dir

  O_FILE="Kelly_Allyn__Whiskey_Can"

  test_name="audacity mode"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 reading informations from audacity labels file '$AUDACITY_FILE' ...
 audacity labels file processed
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/zero.ogg\" created
   File \"$OUTPUT_DIR/first.ogg\" created
   File \"$OUTPUT_DIR/second.ogg\" created
   File \"$OUTPUT_DIR/.ogg\" created
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR -A $AUDACITY_FILE $OGG_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/zero.ogg"
  check_current_ogg_length "0m:57.130s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been" "Whiskey Can"\
  "2007-07-10 15:45:07" "Southern Rock" "1" "http://www.jamendo.com"

  current_file="$OUTPUT_DIR/first.ogg"
  check_current_ogg_length "0m:59.869s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been" "Whiskey Can"\
  "2007-07-10 15:45:07" "Southern Rock" "2" "http://www.jamendo.com"

  current_file="$OUTPUT_DIR/second.ogg"
  check_current_ogg_length "0m:17.000s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been" "Whiskey Can"\
  "2007-07-10 15:45:07" "Southern Rock" "3" "http://www.jamendo.com"

  current_file="$OUTPUT_DIR/.ogg"
  check_current_ogg_length "0m:18.853s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been" "Whiskey Can"\
  "2007-07-10 15:45:07" "Southern Rock" "4" "http://www.jamendo.com"

  print_ok
  echo
}

function test_audacity_and_pretend
{
  _create_audacity_file

  remove_output_dir

  O_FILE="Kelly_Allyn__Whiskey_Can"

  test_name="audacity mode & pretend"

  expected=" Pretending to split file 'songs/${O_FILE}.ogg' ...
 reading informations from audacity labels file '$AUDACITY_FILE' ...
 audacity labels file processed
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/zero.ogg\" created
   File \"$OUTPUT_DIR/first.ogg\" created
   File \"$OUTPUT_DIR/second.ogg\" created
   File \"$OUTPUT_DIR/.ogg\" created
 file split (EOF)"
  mp3splt_args="-P -d $OUTPUT_DIR -A $AUDACITY_FILE $OGG_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_is_empty

  print_ok
  echo
}

function test_audacity_and_cue_export
{
  _create_audacity_file

  remove_output_dir

  O_FILE="Kelly_Allyn__Whiskey_Can"

  test_name="audacity mode & cue export"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 reading informations from audacity labels file '$AUDACITY_FILE' ...
 audacity labels file processed
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/zero.ogg\" created
   File \"$OUTPUT_DIR/first.ogg\" created
   File \"$OUTPUT_DIR/second.ogg\" created
   File \"$OUTPUT_DIR/.ogg\" created
 file split (EOF)
 CUE file 'output/output_out.cue' created."
  mp3splt_args="-E output/out.cue -d $OUTPUT_DIR -A $AUDACITY_FILE $OGG_FILE"
  run_check_output "$mp3splt_args" "$expected"

  check_file_content "output/output_out.cue" 'TITLE "Getting Back From Where I'\''ve Been"
PERFORMER "Kelly Allyn"
FILE "songs/Kelly_Allyn__Whiskey_Can.ogg" OGG
  TRACK 01 AUDIO
    TITLE "Whiskey Can"
    PERFORMER "Kelly Allyn"
    REM ALBUM "Getting Back From Where I'\''ve Been"
    REM GENRE "Southern Rock"
    REM DATE "2007-07-10 15:45:07"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "zero"
    INDEX 01 00:10:00
  TRACK 02 AUDIO
    TITLE "Whiskey Can"
    PERFORMER "Kelly Allyn"
    REM ALBUM "Getting Back From Where I'\''ve Been"
    REM GENRE "Southern Rock"
    REM DATE "2007-07-10 15:45:07"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "first"
    INDEX 01 01:07:10
  TRACK 03 AUDIO
    TITLE "Whiskey Can"
    PERFORMER "Kelly Allyn"
    REM ALBUM "Getting Back From Where I'\''ve Been"
    REM GENRE "Southern Rock"
    REM DATE "2007-07-10 15:45:07"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "skip"
    REM NOKEEP
    INDEX 01 02:07:00
  TRACK 04 AUDIO
    TITLE "Whiskey Can"
    PERFORMER "Kelly Allyn"
    REM ALBUM "Getting Back From Where I'\''ve Been"
    REM GENRE "Southern Rock"
    REM DATE "2007-07-10 15:45:07"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "second"
    INDEX 01 02:29:00
  TRACK 05 AUDIO
    TITLE "Whiskey Can"
    PERFORMER "Kelly Allyn"
    REM ALBUM "Getting Back From Where I'\''ve Been"
    REM GENRE "Southern Rock"
    REM DATE "2007-07-10 15:45:07"
    REM COMMENT "http://www.jamendo.com"
    REM NAME ""
    INDEX 01 02:46:00'

  current_file="$OUTPUT_DIR/zero.ogg"
  check_current_ogg_length "0m:57.130s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been" "Whiskey Can"\
  "2007-07-10 15:45:07" "Southern Rock" "1" "http://www.jamendo.com"

  current_file="$OUTPUT_DIR/first.ogg"
  check_current_ogg_length "0m:59.869s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been" "Whiskey Can"\
  "2007-07-10 15:45:07" "Southern Rock" "2" "http://www.jamendo.com"

  current_file="$OUTPUT_DIR/second.ogg"
  check_current_ogg_length "0m:17.000s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been" "Whiskey Can"\
  "2007-07-10 15:45:07" "Southern Rock" "3" "http://www.jamendo.com"

  current_file="$OUTPUT_DIR/.ogg"
  check_current_ogg_length "0m:18.853s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been" "Whiskey Can"\
  "2007-07-10 15:45:07" "Southern Rock" "4" "http://www.jamendo.com"

  print_ok
  echo
}

function test_audacity_and_pretend_and_cue_export
{
  _create_audacity_file

  remove_output_dir

  O_FILE="Kelly_Allyn__Whiskey_Can"

  test_name="audacity mode & pretend & cue export"

  expected=" Pretending to split file 'songs/${O_FILE}.ogg' ...
 reading informations from audacity labels file '$AUDACITY_FILE' ...
 audacity labels file processed
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/zero.ogg\" created
   File \"$OUTPUT_DIR/first.ogg\" created
   File \"$OUTPUT_DIR/second.ogg\" created
   File \"$OUTPUT_DIR/.ogg\" created
 file split (EOF)
 CUE file 'output/output_out.cue' created."
  mp3splt_args="-P -E output/out.cue -d $OUTPUT_DIR -A $AUDACITY_FILE $OGG_FILE"
  run_check_output "$mp3splt_args" "$expected"

  check_file_content "output/output_out.cue" 'TITLE "Getting Back From Where I'\''ve Been"
PERFORMER "Kelly Allyn"
FILE "songs/Kelly_Allyn__Whiskey_Can.ogg" OGG
  TRACK 01 AUDIO
    TITLE "Whiskey Can"
    PERFORMER "Kelly Allyn"
    REM ALBUM "Getting Back From Where I'\''ve Been"
    REM GENRE "Southern Rock"
    REM DATE "2007-07-10 15:45:07"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "zero"
    INDEX 01 00:10:00
  TRACK 02 AUDIO
    TITLE "Whiskey Can"
    PERFORMER "Kelly Allyn"
    REM ALBUM "Getting Back From Where I'\''ve Been"
    REM GENRE "Southern Rock"
    REM DATE "2007-07-10 15:45:07"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "first"
    INDEX 01 01:07:10
  TRACK 03 AUDIO
    TITLE "Whiskey Can"
    PERFORMER "Kelly Allyn"
    REM ALBUM "Getting Back From Where I'\''ve Been"
    REM GENRE "Southern Rock"
    REM DATE "2007-07-10 15:45:07"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "skip"
    REM NOKEEP
    INDEX 01 02:07:00
  TRACK 04 AUDIO
    TITLE "Whiskey Can"
    PERFORMER "Kelly Allyn"
    REM ALBUM "Getting Back From Where I'\''ve Been"
    REM GENRE "Southern Rock"
    REM DATE "2007-07-10 15:45:07"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "second"
    INDEX 01 02:29:00
  TRACK 05 AUDIO
    TITLE "Whiskey Can"
    PERFORMER "Kelly Allyn"
    REM ALBUM "Getting Back From Where I'\''ve Been"
    REM GENRE "Southern Rock"
    REM DATE "2007-07-10 15:45:07"
    REM COMMENT "http://www.jamendo.com"
    REM NAME ""
    INDEX 01 02:46:00'

  check_output_directory_number_of_files 1

  print_ok
  echo
}

function test_audacity_and_output_format
{
  _create_audacity_file

  remove_output_dir

  O_FILE="Kelly_Allyn__Whiskey_Can"

  test_name="audacity mode & output format"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 reading informations from audacity labels file '$AUDACITY_FILE' ...
 audacity labels file processed
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/1_Whiskey Can_00:10:00__01:07:13.ogg\" created
   File \"$OUTPUT_DIR/2_Whiskey Can_01:07:13__02:07:00.ogg\" created
   File \"$OUTPUT_DIR/3_Whiskey Can_02:29:00__02:46:00.ogg\" created
   File \"$OUTPUT_DIR/4_Whiskey Can_02:46:00__03:04:85.ogg\" created
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR -o \"@n_@t_@m:@s:@h__@M:@S:@H\" -A $AUDACITY_FILE $OGG_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/1_Whiskey Can_00:10:00__01:07:13.ogg"
  check_current_ogg_length "0m:57.130s"

  current_file="$OUTPUT_DIR/2_Whiskey Can_01:07:13__02:07:00.ogg"
  check_current_ogg_length "0m:59.869s"

  current_file="$OUTPUT_DIR/3_Whiskey Can_02:29:00__02:46:00.ogg"
  check_current_ogg_length "0m:17.000s"

  current_file="$OUTPUT_DIR/4_Whiskey Can_02:46:00__03:04:85.ogg"
  check_current_ogg_length "0m:18.853s"

  print_ok
  echo
}

function test_audacity_and_custom_tags
{
  _create_audacity_file

  remove_output_dir

  O_FILE="Kelly_Allyn__Whiskey_Can"

  test_name="audacity mode & custom tags"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 reading informations from audacity labels file '$AUDACITY_FILE' ...
 audacity labels file processed
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/zero.ogg\" created
   File \"$OUTPUT_DIR/first.ogg\" created
   File \"$OUTPUT_DIR/second.ogg\" created
   File \"$OUTPUT_DIR/.ogg\" created
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR -g \"%[@o,@N=6,@t=title1][@t=tit2][@t=tit3]\" -A $AUDACITY_FILE $OGG_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/zero.ogg"
  check_current_ogg_length "0m:57.130s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been" "title1"\
  "2007-07-10 15:45:07" "Southern Rock" "6" "http://www.jamendo.com"

  current_file="$OUTPUT_DIR/first.ogg"
  check_current_ogg_length "0m:59.869s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been" "tit2"\
  "2007-07-10 15:45:07" "Southern Rock" "7" "http://www.jamendo.com"

  current_file="$OUTPUT_DIR/second.ogg"
  check_current_ogg_length "0m:17.000s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been" "tit3"\
  "2007-07-10 15:45:07" "Southern Rock" "8" "http://www.jamendo.com"

  current_file="$OUTPUT_DIR/.ogg"
  check_current_ogg_length "0m:18.853s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been" "title1"\
  "2007-07-10 15:45:07" "Southern Rock" "9" "http://www.jamendo.com"

  print_ok
  echo
}

function test_audacity_and_custom_tags_and_output_format
{
  _create_audacity_file

  remove_output_dir

  O_FILE="Kelly_Allyn__Whiskey_Can"

  test_name="audacity mode & custom tags & output format"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 reading informations from audacity labels file '$AUDACITY_FILE' ...
 audacity labels file processed
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/title1/Kelly Allyn_Getting Back From Where I've Been_title1_1.ogg\" created
   File \"$OUTPUT_DIR/tit2/Kelly Allyn_Getting Back From Where I've Been_tit2_2.ogg\" created
   File \"$OUTPUT_DIR/tit3/Kelly Allyn_Getting Back From Where I've Been_tit3_3.ogg\" created
   File \"$OUTPUT_DIR/title1/Kelly Allyn_Getting Back From Where I've Been_title1_4.ogg\" created
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR -o \"@t/@a_@b_@t_@n\" -g \"%[@o,@N=6,@t=title1][@t=tit2][@t=tit3]\" -A $AUDACITY_FILE $OGG_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/title1/Kelly Allyn_Getting Back From Where I've Been_title1_1.ogg"
  check_current_ogg_length "0m:57.130s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been" "title1"\
  "2007-07-10 15:45:07" "Southern Rock" "6" "http://www.jamendo.com"

  current_file="$OUTPUT_DIR/tit2/Kelly Allyn_Getting Back From Where I've Been_tit2_2.ogg"
  check_current_ogg_length "0m:59.869s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been" "tit2"\
  "2007-07-10 15:45:07" "Southern Rock" "7" "http://www.jamendo.com"

  current_file="$OUTPUT_DIR/tit3/Kelly Allyn_Getting Back From Where I've Been_tit3_3.ogg"
  check_current_ogg_length "0m:17.000s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been" "tit3"\
  "2007-07-10 15:45:07" "Southern Rock" "8" "http://www.jamendo.com"

  current_file="$OUTPUT_DIR/title1/Kelly Allyn_Getting Back From Where I've Been_title1_4.ogg"
  check_current_ogg_length "0m:18.853s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been" "title1"\
  "2007-07-10 15:45:07" "Southern Rock" "9" "http://www.jamendo.com"

  print_ok
  echo
}

function run_audacity_mode_tests
{
  p_blue " AUDACITY ogg tests ..."
  echo

  audacity_mode_test_functions=$(declare -F | grep " test_audacity" | awk '{ print $3 }')

  for test_func in $audacity_mode_test_functions;do
    eval $test_func
  done

  p_blue " AUDACITY ogg tests DONE."
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

