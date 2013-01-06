#!/bin/bash

. ./utils.sh || exit 1


function test_time_simple
{
  remove_output_dir

  test_name="simple time"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting time mode split
   File \"$OUTPUT_DIR/${O_FILE}_00m_00s__01m_00s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_01m_00s__02m_00s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_02m_00s__03m_00s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_03m_00s__03m_04s_85h.ogg\" created
 time split ok"
  mp3splt_args="-d $OUTPUT_DIR $OGG_FILE -t 1.0"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${O_FILE}_00m_00s__01m_00s.ogg"
  check_current_ogg_length "1m:00.000s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "1"\
                     "http://www.jamendo.com"
  check_current_file_size "1631025"

  current_file="$OUTPUT_DIR/${O_FILE}_01m_00s__02m_00s.ogg"
  check_current_ogg_length "1m:00.000s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "2"\
                     "http://www.jamendo.com"
  check_current_file_size "1605571"

  current_file="$OUTPUT_DIR/${O_FILE}_02m_00s__03m_00s.ogg"
  check_current_ogg_length "1m:00.000s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "3"\
                     "http://www.jamendo.com"
  check_current_file_size "1542970"

  current_file="$OUTPUT_DIR/${O_FILE}_03m_00s__03m_04s_85h.ogg"
  check_current_ogg_length "0m:04.853s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "4"\
                     "http://www.jamendo.com"
  check_current_file_size "56387"

  print_ok
  echo
}

function test_time_no_output_tags
{
  remove_output_dir

  test_name="time & no output tags"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting time mode split
   File \"$OUTPUT_DIR/${O_FILE}_00m_00s__01m_00s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_01m_00s__02m_00s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_02m_00s__03m_00s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_03m_00s__03m_04s_85h.ogg\" created
 time split ok"
  mp3splt_args="-d $OUTPUT_DIR -n $OGG_FILE -t 1.0"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${O_FILE}_00m_00s__01m_00s.ogg"
  check_current_ogg_length "1m:00.000s"
  check_current_ogg_no_tags
  check_current_file_size "1630248"

  current_file="$OUTPUT_DIR/${O_FILE}_01m_00s__02m_00s.ogg"
  check_current_ogg_length "1m:00.000s"
  check_current_ogg_no_tags
  check_current_file_size "1604794"

  current_file="$OUTPUT_DIR/${O_FILE}_02m_00s__03m_00s.ogg"
  check_current_ogg_length "1m:00.000s"
  check_current_ogg_no_tags
  check_current_file_size "1542193"

  current_file="$OUTPUT_DIR/${O_FILE}_03m_00s__03m_04s_85h.ogg"
  check_current_ogg_length "0m:04.853s"
  check_current_ogg_no_tags
  check_current_file_size "55610"

  print_ok
  echo
}

function test_time_pretend
{
  remove_output_dir

  test_name="time & pretend"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Pretending to split file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting time mode split
   File \"$OUTPUT_DIR/${O_FILE}_00m_00s__01m_00s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_01m_00s__02m_00s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_02m_00s__03m_00s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_03m_00s__03m_04s_85h.ogg\" created
 time split ok"
  mp3splt_args="-P -d $OUTPUT_DIR $OGG_FILE -t 1.0"
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_is_empty
 
  print_ok
  echo
}

function test_time_cue_export
{
  remove_output_dir

  test_name="time & cue export"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting time mode split
   File \"$OUTPUT_DIR/${O_FILE}_00m_00s__01m_00s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_01m_00s__02m_00s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_02m_00s__03m_00s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_03m_00s__03m_04s_85h.ogg\" created
 time split ok
 CUE file 'output/output_out.cue' created."
  mp3splt_args="-E output/out.cue -d $OUTPUT_DIR $OGG_FILE -t 1.0"
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
    REM NAME "Kelly_Allyn__Whiskey_Can_00m_00s__01m_00s"
    INDEX 01 00:00:00
  TRACK 02 AUDIO
    TITLE "Whiskey Can"
    PERFORMER "Kelly Allyn"
    REM ALBUM "Getting Back From Where I'\''ve Been"
    REM GENRE "Southern Rock"
    REM DATE "2007-07-10 15:45:07"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "Kelly_Allyn__Whiskey_Can_01m_00s__02m_00s"
    INDEX 01 01:00:00
  TRACK 03 AUDIO
    TITLE "Whiskey Can"
    PERFORMER "Kelly Allyn"
    REM ALBUM "Getting Back From Where I'\''ve Been"
    REM GENRE "Southern Rock"
    REM DATE "2007-07-10 15:45:07"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "Kelly_Allyn__Whiskey_Can_02m_00s__03m_00s"
    INDEX 01 02:00:00
  TRACK 04 AUDIO
    TITLE "Whiskey Can"
    PERFORMER "Kelly Allyn"
    REM ALBUM "Getting Back From Where I'\''ve Been"
    REM GENRE "Southern Rock"
    REM DATE "2007-07-10 15:45:07"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "Kelly_Allyn__Whiskey_Can_03m_00s__03m_04s_85h"
    INDEX 01 03:00:00'

  current_file="$OUTPUT_DIR/${O_FILE}_00m_00s__01m_00s.ogg"
  check_current_ogg_length "1m:00.000s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "1"\
                     "http://www.jamendo.com"
  check_current_file_size "1631025"

  current_file="$OUTPUT_DIR/${O_FILE}_01m_00s__02m_00s.ogg"
  check_current_ogg_length "1m:00.000s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "2"\
                     "http://www.jamendo.com"
  check_current_file_size "1605571"

  current_file="$OUTPUT_DIR/${O_FILE}_02m_00s__03m_00s.ogg"
  check_current_ogg_length "1m:00.000s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "3"\
                     "http://www.jamendo.com"
  check_current_file_size "1542970"

  current_file="$OUTPUT_DIR/${O_FILE}_03m_00s__03m_04s_85h.ogg"
  check_current_ogg_length "0m:04.853s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "4"\
                     "http://www.jamendo.com"
  check_current_file_size "56387"

  print_ok
  echo
}

function test_time_pretend_cue_export
{
  remove_output_dir

  test_name="time & pretend & cue export"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Pretending to split file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting time mode split
   File \"$OUTPUT_DIR/${O_FILE}_00m_00s__01m_00s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_01m_00s__02m_00s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_02m_00s__03m_00s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_03m_00s__03m_04s_85h.ogg\" created
 time split ok
 CUE file 'output/output_out.cue' created."
  mp3splt_args="-P -E output/out.cue -d $OUTPUT_DIR $OGG_FILE -t 1.0"
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_number_of_files 1

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
    REM NAME "Kelly_Allyn__Whiskey_Can_00m_00s__01m_00s"
    INDEX 01 00:00:00
  TRACK 02 AUDIO
    TITLE "Whiskey Can"
    PERFORMER "Kelly Allyn"
    REM ALBUM "Getting Back From Where I'\''ve Been"
    REM GENRE "Southern Rock"
    REM DATE "2007-07-10 15:45:07"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "Kelly_Allyn__Whiskey_Can_01m_00s__02m_00s"
    INDEX 01 01:00:00
  TRACK 03 AUDIO
    TITLE "Whiskey Can"
    PERFORMER "Kelly Allyn"
    REM ALBUM "Getting Back From Where I'\''ve Been"
    REM GENRE "Southern Rock"
    REM DATE "2007-07-10 15:45:07"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "Kelly_Allyn__Whiskey_Can_02m_00s__03m_00s"
    INDEX 01 02:00:00
  TRACK 04 AUDIO
    TITLE "Whiskey Can"
    PERFORMER "Kelly Allyn"
    REM ALBUM "Getting Back From Where I'\''ve Been"
    REM GENRE "Southern Rock"
    REM DATE "2007-07-10 15:45:07"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "Kelly_Allyn__Whiskey_Can_03m_00s__03m_04s_85h"
    INDEX 01 03:00:00'

  print_ok
  echo
}

function test_time_overlap_split
{
  remove_output_dir

  test_name="time overlap splitpoints"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting time mode split
 info: overlapping split files with 0.30.0
   File \"$OUTPUT_DIR/${O_FILE}_00m_00s__02m_30s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_02m_00s__03m_04s_85h.ogg\" created
 time split ok"
  mp3splt_args="-O 0.30 -d $OUTPUT_DIR $OGG_FILE -t 2.0"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${O_FILE}_00m_00s__02m_30s.ogg"
  check_current_ogg_length "2m:30.000s"
  check_current_file_size "4016423"

  current_file="$OUTPUT_DIR/${O_FILE}_02m_00s__03m_04s_85h.ogg"
  check_current_ogg_length "1m:04.849s"
  check_current_file_size "1593645"

  print_ok
  echo
}

function test_time_overlap_split_and_cue_export
{
  remove_output_dir

  test_name="time overlap splitpoints & cue export"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting time mode split
 info: overlapping split files with 0.30.0
   File \"$OUTPUT_DIR/${O_FILE}_00m_00s__02m_30s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_02m_00s__03m_04s_85h.ogg\" created
 time split ok
 CUE file 'output/output_out.cue' created."
  mp3splt_args="-O 0.30 -E output/out.cue -d $OUTPUT_DIR $OGG_FILE -t 2.0"
  run_check_output "$mp3splt_args" "$expected"

  check_file_content 'output/output_out.cue' 'TITLE "Getting Back From Where I'\''ve Been"
PERFORMER "Kelly Allyn"
FILE "songs/Kelly_Allyn__Whiskey_Can.ogg" OGG
  TRACK 01 AUDIO
    TITLE "Whiskey Can"
    PERFORMER "Kelly Allyn"
    REM ALBUM "Getting Back From Where I'\''ve Been"
    REM GENRE "Southern Rock"
    REM DATE "2007-07-10 15:45:07"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "Kelly_Allyn__Whiskey_Can_00m_00s__02m_30s"
    INDEX 01 00:00:00
  TRACK 02 AUDIO
    TITLE "Whiskey Can"
    PERFORMER "Kelly Allyn"
    REM ALBUM "Getting Back From Where I'\''ve Been"
    REM GENRE "Southern Rock"
    REM DATE "2007-07-10 15:45:07"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "Kelly_Allyn__Whiskey_Can_02m_00s__03m_04s_85h"
    INDEX 01 02:00:00'

  current_file="$OUTPUT_DIR/${O_FILE}_00m_00s__02m_30s.ogg"
  check_current_ogg_length "2m:30.000s"
  check_current_file_size "4016423"

  current_file="$OUTPUT_DIR/${O_FILE}_02m_00s__03m_04s_85h.ogg"
  check_current_ogg_length "1m:04.849s"
  check_current_file_size "1593645"

  print_ok
  echo
}

function test_time_m3u
{
  remove_output_dir

  test_name="time m3u"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 M3U file '$OUTPUT_DIR/m3u/playlist.m3u' will be created.
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting time mode split
   File \"$OUTPUT_DIR/m3u/${O_FILE}_00m_00s__01m_50s.ogg\" created
   File \"$OUTPUT_DIR/m3u/${O_FILE}_01m_50s__03m_04s_85h.ogg\" created
 time split ok"
  mp3splt_args="-m playlist.m3u -d $OUTPUT_DIR/m3u $OGG_FILE -t 1.50"
  run_check_output "$mp3splt_args" "$expected"

  expected="Kelly_Allyn__Whiskey_Can_00m_00s__01m_50s.ogg
Kelly_Allyn__Whiskey_Can_01m_50s__03m_04s_85h.ogg"
  check_file_content "$OUTPUT_DIR/m3u/playlist.m3u" "$expected"

  print_ok
  echo
}

function test_time_create_directories
{
  remove_output_dir

  test_name="time create directories"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting time mode split
   File \"$OUTPUT_DIR/a/b/c/${O_FILE}_00m_00s__02m_00s.ogg\" created
   File \"$OUTPUT_DIR/a/b/c/${O_FILE}_02m_00s__03m_04s_85h.ogg\" created
 time split ok"
  mp3splt_args=" -d $OUTPUT_DIR/a/b/c $OGG_FILE -t 2.0"
  run_check_output "$mp3splt_args" "$expected"

  check_if_directory_exist "$OUTPUT_DIR/a/b/c"
  check_if_file_exist "$OUTPUT_DIR/a/b/c/${O_FILE}_00m_00s__02m_00s.ogg"
  check_if_file_exist "$OUTPUT_DIR/a/b/c/${O_FILE}_02m_00s__03m_04s_85h.ogg"

  print_ok
  echo
}

function test_time_custom_tags
{
  remove_output_dir

  test_name="time custom tags"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting time mode split
   File \"$OUTPUT_DIR/${O_FILE}_00m_00s__00m_40s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_00m_40s__01m_20s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_01m_20s__02m_00s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_02m_00s__02m_40s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_02m_40s__03m_04s_85h.ogg\" created
 time split ok"
  tags_option="[@a=a1,@b=b1,@t=t1,@y=2000,@c=my_comment,@n=10][]%[@o,@b=album,@N=7][@a=custom_artist][@o,@n=20]"
  mp3splt_args="-d $OUTPUT_DIR -g \"$tags_option\" $OGG_FILE -t 0.40"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${O_FILE}_00m_00s__00m_40s.ogg"
  check_all_ogg_tags "a1" "b1" "t1" "2000" "" "10" "my_comment"

  current_file="$OUTPUT_DIR/${O_FILE}_00m_40s__01m_20s.ogg"
  check_current_ogg_no_tags

  current_file="$OUTPUT_DIR/${O_FILE}_01m_20s__02m_00s.ogg"
  check_all_ogg_tags "Kelly Allyn" "album" "Whiskey Can"\
  "2007-07-10 15:45:07" "Southern Rock" "7" "http://www.jamendo.com"

  current_file="$OUTPUT_DIR/${O_FILE}_02m_00s__02m_40s.ogg"
  check_all_ogg_tags "custom_artist" "album" "Whiskey Can"\
  "2007-07-10 15:45:07" "Southern Rock" "8" "http://www.jamendo.com"

  current_file="$OUTPUT_DIR/${O_FILE}_02m_40s__03m_04s_85h.ogg"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
  "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "20" "http://www.jamendo.com"

  print_ok
  echo
}

function test_time_custom_tags_multiple_percent
{
  remove_output_dir

  test_name="time custom tags multiple percent"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting time mode split
   File \"$OUTPUT_DIR/${O_FILE}_00m_00s__00m_40s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_00m_40s__01m_20s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_01m_20s__02m_00s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_02m_00s__02m_40s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_02m_40s__03m_04s_85h.ogg\" created
 time split ok"
  tags_option="%[@a=a1,@b=b1,@n=10][]%[@o,@b=album,@N=7][@a=custom_artist][@o,@n=20]"
  mp3splt_args="-d $OUTPUT_DIR -g \"$tags_option\" $OGG_FILE -t 0.40"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${O_FILE}_00m_00s__00m_40s.ogg"
  check_all_ogg_tags "a1" "b1" "" "" "" "10" ""

  current_file="$OUTPUT_DIR/${O_FILE}_00m_40s__01m_20s.ogg"
  check_all_ogg_tags "a1" "b1" "" "" "" "10" ""

  current_file="$OUTPUT_DIR/${O_FILE}_01m_20s__02m_00s.ogg"
  check_all_ogg_tags "Kelly Allyn" "album" "Whiskey Can"\
  "2007-07-10 15:45:07" "Southern Rock" "7" "http://www.jamendo.com"

  current_file="$OUTPUT_DIR/${O_FILE}_02m_00s__02m_40s.ogg"
  check_all_ogg_tags "custom_artist" "album" "Whiskey Can"\
  "2007-07-10 15:45:07" "Southern Rock" "8" "http://www.jamendo.com"

  current_file="$OUTPUT_DIR/${O_FILE}_02m_40s__03m_04s_85h.ogg"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been" "Whiskey Can"\
  "2007-07-10 15:45:07" "Southern Rock" "20" "http://www.jamendo.com"

  print_ok
  echo
}

function test_time_stdin
{
  remove_output_dir

  test_name="time stdin"
 
  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'o-' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 0m.00s
 info: starting time mode split
   File \"$OUTPUT_DIR/o-_00m_00s__01m_00s.ogg\" created
   File \"$OUTPUT_DIR/o-_01m_00s__02m_00s.ogg\" created
   File \"$OUTPUT_DIR/o-_02m_00s__03m_00s.ogg\" created
   File \"$OUTPUT_DIR/o-_03m_00s__04m_00s.ogg\" created
 time split ok"
  mp3splt_args="-d $OUTPUT_DIR o- -t 1.0" 
  run_custom_check_output "cat songs/${O_FILE}.ogg | $MP3SPLT" "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/o-_00m_00s__01m_00s.ogg"
  check_current_ogg_length "1m:00.000s"
  check_current_ogg_no_tags
  check_current_file_size "1630248"

  current_file="$OUTPUT_DIR/o-_01m_00s__02m_00s.ogg"
  check_current_ogg_length "1m:00.000s"
  check_current_ogg_no_tags
  check_current_file_size "1604794"

  current_file="$OUTPUT_DIR/o-_02m_00s__03m_00s.ogg"
  check_current_ogg_length "1m:00.000s"
  check_current_ogg_no_tags
  check_current_file_size "1542193"

  current_file="$OUTPUT_DIR/o-_03m_00s__04m_00s.ogg"
  check_current_ogg_length "0m:04.853s"
  check_current_ogg_no_tags
  check_current_file_size "55610"

  print_ok
  echo
}

function test_time_stdin_and_tags
{
  remove_output_dir

  test_name="time stdin and tags"
 
  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'o-' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 0m.00s
 info: starting time mode split
   File \"$OUTPUT_DIR/o-_00m_00s__01m_00s.ogg\" created
   File \"$OUTPUT_DIR/o-_01m_00s__02m_00s.ogg\" created
   File \"$OUTPUT_DIR/o-_02m_00s__03m_00s.ogg\" created
   File \"$OUTPUT_DIR/o-_03m_00s__04m_00s.ogg\" created
 time split ok"
  tags_options="-g %[@a=a1,@b=b1,@y=1070,@N=1]"
  mp3splt_args="$tags_options -d $OUTPUT_DIR o- -t 1.0" 
  run_custom_check_output "cat songs/${O_FILE}.ogg | $MP3SPLT" "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/o-_00m_00s__01m_00s.ogg"
  check_current_ogg_length "1m:00.000s"
  check_all_ogg_tags "a1" "b1" "" "1070" "" "1" ""
  check_current_file_size "1630303"

  current_file="$OUTPUT_DIR/o-_01m_00s__02m_00s.ogg"
  check_current_ogg_length "1m:00.000s"
  check_all_ogg_tags "a1" "b1" "" "1070" "" "2" ""
  check_current_file_size "1604849"

  current_file="$OUTPUT_DIR/o-_02m_00s__03m_00s.ogg"
  check_current_ogg_length "1m:00.000s"
  check_all_ogg_tags "a1" "b1" "" "1070" "" "3" ""
  check_current_file_size "1542248"

  current_file="$OUTPUT_DIR/o-_03m_00s__04m_00s.ogg"
  check_current_ogg_length "0m:04.853s"
  check_all_ogg_tags "a1" "b1" "" "1070" "" "4" ""
  check_current_file_size "55665"

  print_ok
  echo
}

function test_time_output_fnames_and_dirs
{
  remove_output_dir

  test_name="time output fnames & directories"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting time mode split
   File \"$OUTPUT_DIR/Kelly Allyn/Getting Back From Where I've Been/Kelly Allyn-Whiskey Can 1.ogg\" created
   File \"$OUTPUT_DIR/Kelly Allyn/Getting Back From Where I've Been/Kelly Allyn-Whiskey Can 2.ogg\" created
 time split ok"
  output_option="@a/@b/@a-@t @n"
  mp3splt_args="-o '$output_option' -d $OUTPUT_DIR $OGG_FILE -t 2.0"
  run_check_output "$mp3splt_args" "$expected"

  check_if_directory_exist "$OUTPUT_DIR/Kelly Allyn"
  check_if_file_exist "$OUTPUT_DIR/Kelly Allyn/Getting Back From Where I've Been/Kelly Allyn-Whiskey Can 1.ogg"
  check_if_file_exist "$OUTPUT_DIR/Kelly Allyn/Getting Back From Where I've Been/Kelly Allyn-Whiskey Can 2.ogg"

  print_ok
  echo
}

function test_time_output_fnames_and_custom_tags_dirs
{
  remove_output_dir

  test_name="time output fnames & custom tags & directories"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting time mode split
   File \"$OUTPUT_DIR/Kelly Allyn/album1/Kelly Allyn-Whiskey Can 1.ogg\" created
   File \"$OUTPUT_DIR/Kelly Allyn/album2/Kelly Allyn-Whiskey Can 2.ogg\" created
 time split ok"
  output_option="@a/@b/@a-@t @n"
  tags_option="%[@o,@b=album1][@b=album2][@b=album3]"
  mp3splt_args="-o '$output_option' -g \"$tags_option\" -d $OUTPUT_DIR $OGG_FILE -t 2.0"
  run_check_output "$mp3splt_args" "$expected"

  check_if_directory_exist "$OUTPUT_DIR/Kelly Allyn"
  check_if_directory_exist "$OUTPUT_DIR/Kelly Allyn/album1"
  check_if_directory_exist "$OUTPUT_DIR/Kelly Allyn/album2"
  check_if_file_exist "$OUTPUT_DIR/Kelly Allyn/album1/Kelly Allyn-Whiskey Can 1.ogg"
  check_if_file_exist "$OUTPUT_DIR/Kelly Allyn/album2/Kelly Allyn-Whiskey Can 2.ogg"

  print_ok
  echo
}

function test_time_stdout_multiple_splitpoints
{
  remove_output_dir

  test_name="time stdout & splitpoints > 2"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Warning: using time mode with stdout !
 Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting time mode split
   File \"-\" created
   File \"-\" created
 time split ok"
  mp3splt_args=" -o - $OGG_FILE -t 2.0"
  run_custom_check_output "$MP3SPLT $mp3splt_args > $OUTPUT_DIR/stdout.ogg" "" "$expected"

  current_file="$OUTPUT_DIR/stdout.ogg"
  check_current_ogg_length "2m:00.000s
1m:04.853s"
  check_current_file_size "4824607"

  print_ok
  echo
}

function run_time_tests
{
  p_blue " TIME ogg tests ..."
  echo

  time_test_functions=$(declare -F | grep " test_time_" | awk '{ print $3 }')

  for test_func in $time_test_functions;do
    eval $test_func
  done

  p_blue " TIME ogg tests DONE."
  echo
}

#main
export LC_ALL="C"
start_date=$(date +%s)

run_time_tests

p_failed_tests

end_date=$(date +%s)

p_time_diff_cyan $start_date $end_date "\t"
echo -e '\n'

exit 0

