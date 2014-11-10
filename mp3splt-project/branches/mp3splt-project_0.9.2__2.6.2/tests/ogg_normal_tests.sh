#!/bin/bash

. ./utils.sh || exit 1

#normal mode functional tests

function test_normal_with_tags
{
  remove_output_dir

  test_name="with tags"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/${O_FILE}_01m_00s__02m_01s_20h.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_02m_01s_20h__03m_00s_10h.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_03m_00s_10h__03m_04s_85h.ogg\" created
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR $OGG_FILE 1.0 2.1.2 3.0.1 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${O_FILE}_01m_00s__02m_01s_20h.ogg"
  check_current_ogg_length "1m:01.200s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "1"\
                     "http://www.jamendo.com"
  check_current_file_size "1638268"

  current_file="$OUTPUT_DIR/${O_FILE}_02m_01s_20h__03m_00s_10h.ogg" 
  check_current_ogg_length "0m:58.899s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "2"\
                     "http://www.jamendo.com"
  check_current_file_size "1513649"

  current_file="$OUTPUT_DIR/${O_FILE}_03m_00s_10h__03m_04s_85h.ogg" 
  check_current_ogg_length "0m:04.753s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "3"\
                     "http://www.jamendo.com"
  check_current_file_size "53756"

  print_ok
  echo
}

function test_normal_with_original_tags
{
  remove_output_dir

  test_name="with original tags %[@o]"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/${O_FILE}_01m_00s__02m_01s_20h.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_02m_01s_20h__03m_00s_10h.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_03m_00s_10h__03m_04s_85h.ogg\" created
 file split (EOF)"
  mp3splt_args="-g %[@o] -d $OUTPUT_DIR $OGG_FILE 1.0 2.1.2 3.0.1 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${O_FILE}_01m_00s__02m_01s_20h.ogg"
  check_ogg_tags_equal_between_files "songs/${O_FILE}.ogg" $current_file

  current_file="$OUTPUT_DIR/${O_FILE}_02m_01s_20h__03m_00s_10h.ogg" 
  check_ogg_tags_equal_between_files "songs/${O_FILE}.ogg" $current_file

  current_file="$OUTPUT_DIR/${O_FILE}_03m_00s_10h__03m_04s_85h.ogg" 
  check_ogg_tags_equal_between_files "songs/${O_FILE}.ogg" $current_file

  print_ok
  echo
}

function test_normal_no_output_tags
{
  remove_output_dir

  test_name="no output tags"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/${O_FILE}_01m_00s__02m_01s_20h.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_02m_01s_20h__03m_00s_10h.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_03m_00s_10h__03m_04s_85h.ogg\" created
 file split (EOF)"
  mp3splt_args="-n -d $OUTPUT_DIR $OGG_FILE 1.0 2.1.2 3.0.1 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${O_FILE}_01m_00s__02m_01s_20h.ogg"
  check_current_ogg_length "1m:01.200s"
  check_current_ogg_no_tags
  check_current_file_size "1637491"

  current_file="$OUTPUT_DIR/${O_FILE}_02m_01s_20h__03m_00s_10h.ogg" 
  check_current_ogg_length "0m:58.899s"
  check_current_ogg_no_tags
  check_current_file_size "1512872"

  current_file="$OUTPUT_DIR/${O_FILE}_03m_00s_10h__03m_04s_85h.ogg" 
  check_current_ogg_length "0m:04.753s"
  check_current_ogg_no_tags
  check_current_file_size "52979"

  print_ok
  echo
}

function test_normal_pretend
{
  remove_output_dir

  test_name="pretend"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Pretending to split file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/${O_FILE}_01m_00s__02m_01s_20h.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_02m_01s_20h__03m_00s_10h.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_03m_00s_10h__03m_04s_85h.ogg\" created
 file split (EOF)"
  mp3splt_args="-P -d $OUTPUT_DIR $OGG_FILE 1.0 2.1.2 3.0.1 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_is_empty

  print_ok
  echo
}

function test_normal_cue_export
{
  remove_output_dir

  test_name="cue export"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/${O_FILE}_01m_00s__02m_01s_20h.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_02m_01s_20h__03m_00s_10h.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_03m_00s_10h__03m_04s_85h.ogg\" created
 file split (EOF)
 CUE file 'output/output_out.cue' created."
  mp3splt_args="-E output/out.cue -d $OUTPUT_DIR $OGG_FILE 1.0 2.1.2 3.0.1 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_number_of_files 4

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
    REM NAME "Kelly_Allyn__Whiskey_Can_01m_00s__02m_01s_20h"
    INDEX 01 01:00:00
  TRACK 02 AUDIO
    TITLE "Whiskey Can"
    PERFORMER "Kelly Allyn"
    REM ALBUM "Getting Back From Where I'\''ve Been"
    REM GENRE "Southern Rock"
    REM DATE "2007-07-10 15:45:07"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "Kelly_Allyn__Whiskey_Can_02m_01s_20h__03m_00s_10h"
    INDEX 01 02:01:15
  TRACK 03 AUDIO
    TITLE "Whiskey Can"
    PERFORMER "Kelly Allyn"
    REM ALBUM "Getting Back From Where I'\''ve Been"
    REM GENRE "Southern Rock"
    REM DATE "2007-07-10 15:45:07"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "Kelly_Allyn__Whiskey_Can_03m_00s_10h__03m_04s_85h"
    INDEX 01 03:00:08'

  current_file="$OUTPUT_DIR/${O_FILE}_01m_00s__02m_01s_20h.ogg"
  check_current_ogg_length "1m:01.200s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "1"\
                     "http://www.jamendo.com"
  check_current_file_size "1638268"

  current_file="$OUTPUT_DIR/${O_FILE}_02m_01s_20h__03m_00s_10h.ogg" 
  check_current_ogg_length "0m:58.899s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "2"\
                     "http://www.jamendo.com"
  check_current_file_size "1513649"

  current_file="$OUTPUT_DIR/${O_FILE}_03m_00s_10h__03m_04s_85h.ogg" 
  check_current_ogg_length "0m:04.753s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "3"\
                     "http://www.jamendo.com"
  check_current_file_size "53756"

  print_ok
  echo
}

function test_normal_pretend_and_cue_export
{
  remove_output_dir

  test_name="pretend & cue export"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Pretending to split file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/${O_FILE}_01m_00s__02m_01s_20h.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_02m_01s_20h__03m_00s_10h.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_03m_00s_10h__03m_04s_85h.ogg\" created
 file split (EOF)
 CUE file 'output/output_out.cue' created."
  mp3splt_args="-E output/out.cue -P -d $OUTPUT_DIR $OGG_FILE 1.0 2.1.2 3.0.1 EOF" 
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
    REM NAME "Kelly_Allyn__Whiskey_Can_01m_00s__02m_01s_20h"
    INDEX 01 01:00:00
  TRACK 02 AUDIO
    TITLE "Whiskey Can"
    PERFORMER "Kelly Allyn"
    REM ALBUM "Getting Back From Where I'\''ve Been"
    REM GENRE "Southern Rock"
    REM DATE "2007-07-10 15:45:07"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "Kelly_Allyn__Whiskey_Can_02m_01s_20h__03m_00s_10h"
    INDEX 01 02:01:15
  TRACK 03 AUDIO
    TITLE "Whiskey Can"
    PERFORMER "Kelly Allyn"
    REM ALBUM "Getting Back From Where I'\''ve Been"
    REM GENRE "Southern Rock"
    REM DATE "2007-07-10 15:45:07"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "Kelly_Allyn__Whiskey_Can_03m_00s_10h__03m_04s_85h"
    INDEX 01 03:00:08'

  print_ok
  echo
}

function test_normal_overlap_split
{
  remove_output_dir

  test_name="overlap"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
 info: overlapping split files with 0.30.0
   File \"$OUTPUT_DIR/${O_FILE}_01m_00s__02m_31s_20h.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_02m_01s_20h__03m_04s_85h.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_03m_00s_10h__03m_04s_85h.ogg\" created
 file split (EOF)"
  mp3splt_args="-O 0.30 -d $OUTPUT_DIR $OGG_FILE 1.0 2.1.2 3.0.1 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${O_FILE}_01m_00s__02m_31s_20h.ogg"
  check_current_ogg_length "1m:31.199s"
  check_current_file_size "2421849"

  current_file="$OUTPUT_DIR/${O_FILE}_02m_01s_20h__03m_04s_85h.ogg" 
  check_current_ogg_length "1m:03.649s"
  check_current_file_size "1561684"

  current_file="$OUTPUT_DIR/${O_FILE}_03m_00s_10h__03m_04s_85h.ogg" 
  check_current_ogg_length "0m:04.753s"
  check_current_file_size "53756"

  print_ok
  echo
}

function test_normal_overlap_and_cue_export
{
  remove_output_dir

  test_name="overlap & cue export"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
 info: overlapping split files with 0.30.0
   File \"$OUTPUT_DIR/${O_FILE}_01m_00s__02m_31s_20h.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_02m_01s_20h__03m_04s_85h.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_03m_00s_10h__03m_04s_85h.ogg\" created
 file split (EOF)
 CUE file 'output/output_out.cue' created."
  mp3splt_args="-E output/out.cue -O 0.30 -d $OUTPUT_DIR $OGG_FILE 1.0 2.1.2 3.0.1 EOF" 
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
    REM NAME "Kelly_Allyn__Whiskey_Can_01m_00s__02m_31s_20h"
    INDEX 01 01:00:00
  TRACK 02 AUDIO
    TITLE "Whiskey Can"
    PERFORMER "Kelly Allyn"
    REM ALBUM "Getting Back From Where I'\''ve Been"
    REM GENRE "Southern Rock"
    REM DATE "2007-07-10 15:45:07"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "Kelly_Allyn__Whiskey_Can_02m_01s_20h__03m_04s_85h"
    INDEX 01 02:01:15
  TRACK 03 AUDIO
    TITLE "Whiskey Can"
    PERFORMER "Kelly Allyn"
    REM ALBUM "Getting Back From Where I'\''ve Been"
    REM GENRE "Southern Rock"
    REM DATE "2007-07-10 15:45:07"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "Kelly_Allyn__Whiskey_Can_03m_00s_10h__03m_04s_85h"
    INDEX 01 03:00:08'

  current_file="$OUTPUT_DIR/${O_FILE}_01m_00s__02m_31s_20h.ogg"
  check_current_ogg_length "1m:31.199s"
  check_current_file_size "2421849"

  current_file="$OUTPUT_DIR/${O_FILE}_02m_01s_20h__03m_04s_85h.ogg" 
  check_current_ogg_length "1m:03.649s"
  check_current_file_size "1561684"

  print_ok
  echo
}

function test_normal_m3u
{
  remove_output_dir

  test_name="m3u"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 M3U file '$OUTPUT_DIR/m3u/playlist.m3u' will be created.
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/m3u/${O_FILE}_01m_00s__02m_01s_20h.ogg\" created
   File \"$OUTPUT_DIR/m3u/${O_FILE}_02m_01s_20h__03m_00s_10h.ogg\" created
   File \"$OUTPUT_DIR/m3u/${O_FILE}_03m_00s_10h__03m_04s_85h.ogg\" created
 file split (EOF)"
  mp3splt_args="-m playlist.m3u -d $OUTPUT_DIR/m3u $OGG_FILE 1.0 2.1.2 3.0.1 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  expected="Kelly_Allyn__Whiskey_Can_01m_00s__02m_01s_20h.ogg
Kelly_Allyn__Whiskey_Can_02m_01s_20h__03m_00s_10h.ogg
Kelly_Allyn__Whiskey_Can_03m_00s_10h__03m_04s_85h.ogg"
  check_file_content "$OUTPUT_DIR/m3u/playlist.m3u" "$expected"

  print_ok
  echo
}

function test_normal_pretend_and_m3u
{
  remove_output_dir

  test_name="pretend & m3u"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Pretending to split file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 M3U file 'output/output_out.m3u' will be created.
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/${O_FILE}_01m_00s__02m_01s_20h.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_02m_01s_20h__03m_00s_10h.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_03m_00s_10h__03m_04s_85h.ogg\" created
 file split (EOF)"
  mp3splt_args="-m output/out.m3u -P -d $OUTPUT_DIR $OGG_FILE 1.0 2.1.2 3.0.1 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_number_of_files 0

  print_ok
  echo
}

function test_normal_no_input_tags
{
  remove_output_dir

  test_name="no input tags"

  O_FILE="Kelly_Allyn__Whiskey_Can__no_tags"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/${O_FILE}_01m_00s__02m_01s_20h.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_02m_01s_20h__03m_00s_10h.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_03m_00s_10h__03m_04s_85h.ogg\" created
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR $NO_TAGS_OGG_FILE 1.0 2.1.2 3.0.1 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${O_FILE}_01m_00s__02m_01s_20h.ogg"
  check_current_ogg_length "1m:01.200s"
  check_current_ogg_no_tags
  check_current_file_size "1637491"

  current_file="$OUTPUT_DIR/${O_FILE}_02m_01s_20h__03m_00s_10h.ogg" 
  check_current_ogg_length "0m:58.899s"
  check_current_ogg_no_tags
  check_current_file_size "1512872"

  current_file="$OUTPUT_DIR/${O_FILE}_03m_00s_10h__03m_04s_85h.ogg" 
  check_current_ogg_length "0m:04.753s"
  check_current_ogg_no_tags
  check_current_file_size "52979"

  print_ok
  echo
}

function test_normal_custom_tags
{
  remove_output_dir

  test_name="custom tags"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/${O_FILE}_00m_05s__00m_30s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_00m_30s__01m_00s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_01m_00s__02m_01s_20h.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_02m_01s_20h__03m_00s_10h.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_03m_00s_10h__03m_04s_85h.ogg\" created
 file split (EOF)"
  tags_option="[@a=a1,@b=b1,@t=t1,@y=2000,@c=my_comment,@n=10,@g=Reggae][]%[@o,@b=album,@N=7,@g=special_genre][@a=custom_artist][@o,@n=20]"
  mp3splt_args="-d $OUTPUT_DIR -g \"$tags_option\" $OGG_FILE 0.05 0.30 1.0 2.1.2 3.0.1 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${O_FILE}_00m_05s__00m_30s.ogg"
  check_all_ogg_tags "a1" "b1" "t1" "2000" "Reggae" "10" "my_comment"

  current_file="$OUTPUT_DIR/${O_FILE}_00m_30s__01m_00s.ogg"
  check_current_ogg_no_tags

  current_file="$OUTPUT_DIR/${O_FILE}_01m_00s__02m_01s_20h.ogg"
  check_all_ogg_tags "Kelly Allyn" "album"\
                     "Whiskey Can" "2007-07-10 15:45:07" "special_genre" "7"\
                     "http://www.jamendo.com"

  current_file="$OUTPUT_DIR/${O_FILE}_02m_01s_20h__03m_00s_10h.ogg" 
  check_all_ogg_tags "custom_artist" "album"\
                     "Whiskey Can" "2007-07-10 15:45:07" "special_genre" "8"\
                     "http://www.jamendo.com"

  current_file="$OUTPUT_DIR/${O_FILE}_03m_00s_10h__03m_04s_85h.ogg" 
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "20"\
                     "http://www.jamendo.com"

  print_ok
  echo
}

function test_normal_custom_tags_and_cue_export
{
  remove_output_dir

  test_name="custom tags & cue export"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/${O_FILE}_00m_05s__00m_30s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_00m_30s__01m_00s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_01m_00s__02m_01s_20h.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_02m_01s_20h__03m_00s_10h.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_03m_00s_10h__03m_04s_85h.ogg\" created
 file split (EOF)
 CUE file 'output/output_out.cue' created."
  tags_option="[@a=a1,@b=b1,@t=t1,@y=2000,@c=my_comment,@n=10][]%[@o,@b=album,@N=7][@a=custom_artist][@o,@n=20]"
  mp3splt_args="-E output/out.cue -d $OUTPUT_DIR -g \"$tags_option\" $OGG_FILE 0.05 0.30 1.0 2.1.2 3.0.1 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  check_file_content "output/output_out.cue" 'TITLE "b1"
PERFORMER "a1"
FILE "songs/Kelly_Allyn__Whiskey_Can.ogg" OGG
  TRACK 01 AUDIO
    TITLE "t1"
    PERFORMER "a1"
    REM ALBUM "b1"
    REM DATE "2000"
    REM TRACK "10"
    REM COMMENT "my_comment"
    REM NAME "Kelly_Allyn__Whiskey_Can_00m_05s__00m_30s"
    INDEX 01 00:05:00
  TRACK 02 AUDIO
    REM NAME "Kelly_Allyn__Whiskey_Can_00m_30s__01m_00s"
    INDEX 01 00:30:00
  TRACK 03 AUDIO
    TITLE "Whiskey Can"
    PERFORMER "Kelly Allyn"
    REM ALBUM "album"
    REM GENRE "Southern Rock"
    REM DATE "2007-07-10 15:45:07"
    REM TRACK "7"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "Kelly_Allyn__Whiskey_Can_01m_00s__02m_01s_20h"
    INDEX 01 01:00:00
  TRACK 04 AUDIO
    TITLE "Whiskey Can"
    PERFORMER "custom_artist"
    REM ALBUM "album"
    REM GENRE "Southern Rock"
    REM DATE "2007-07-10 15:45:07"
    REM TRACK "8"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "Kelly_Allyn__Whiskey_Can_02m_01s_20h__03m_00s_10h"
    INDEX 01 02:01:15
  TRACK 05 AUDIO
    TITLE "Whiskey Can"
    PERFORMER "Kelly Allyn"
    REM ALBUM "Getting Back From Where I'\''ve Been"
    REM GENRE "Southern Rock"
    REM DATE "2007-07-10 15:45:07"
    REM TRACK "20"
    REM COMMENT "http://www.jamendo.com"
    REM NAME "Kelly_Allyn__Whiskey_Can_03m_00s_10h__03m_04s_85h"
    INDEX 01 03:00:08'

  current_file="$OUTPUT_DIR/${O_FILE}_00m_05s__00m_30s.ogg"
  check_all_ogg_tags "a1" "b1" "t1" "2000" "" "10" "my_comment"

  current_file="$OUTPUT_DIR/${O_FILE}_00m_30s__01m_00s.ogg"
  check_current_ogg_no_tags

  current_file="$OUTPUT_DIR/${O_FILE}_01m_00s__02m_01s_20h.ogg"
  check_all_ogg_tags "Kelly Allyn" "album"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "7"\
                     "http://www.jamendo.com"

  current_file="$OUTPUT_DIR/${O_FILE}_02m_01s_20h__03m_00s_10h.ogg" 
  check_all_ogg_tags "custom_artist" "album"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "8"\
                     "http://www.jamendo.com"

  current_file="$OUTPUT_DIR/${O_FILE}_03m_00s_10h__03m_04s_85h.ogg" 
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "20"\
                     "http://www.jamendo.com"

  print_ok
  echo
}

function test_normal_custom_tags_and_input_no_tags
{
  remove_output_dir

  test_name="custom tags & no input tags"

  O_FILE="Kelly_Allyn__Whiskey_Can__no_tags"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/${O_FILE}_00m_05s__00m_30s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_00m_30s__01m_00s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_01m_00s__02m_01s_20h.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_02m_01s_20h__03m_00s_10h.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_03m_00s_10h__03m_04s_85h.ogg\" created
 file split (EOF)"
  tags_option="[@a=a1,@b=b1,@t=t1,@y=2000,@c=my_comment,@n=10][]%[@o,@b=album,@N=7][@a=custom_artist][@o,@n=20]"
  mp3splt_args="-d $OUTPUT_DIR -g \"$tags_option\" $NO_TAGS_OGG_FILE 0.05 0.30 1.0 2.1.2 3.0.1 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${O_FILE}_00m_05s__00m_30s.ogg"
  check_all_ogg_tags "a1" "b1" "t1" "2000" "" "10" "my_comment"

  current_file="$OUTPUT_DIR/${O_FILE}_00m_30s__01m_00s.ogg"
  check_current_ogg_no_tags

  current_file="$OUTPUT_DIR/${O_FILE}_01m_00s__02m_01s_20h.ogg"
  check_all_ogg_tags "" "album" "" "" "" "7" ""

  current_file="$OUTPUT_DIR/${O_FILE}_02m_01s_20h__03m_00s_10h.ogg" 
  check_all_ogg_tags "custom_artist" "album" "" "" "" "8" ""

  current_file="$OUTPUT_DIR/${O_FILE}_03m_00s_10h__03m_04s_85h.ogg" 
  check_all_ogg_tags "" "" "" "" "" "20" ""

  print_ok
  echo
}

function test_normal_custom_tags_multiple_percent
{
  remove_output_dir

  test_name="custom tags multiple percent"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/${O_FILE}_00m_05s__00m_30s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_00m_30s__01m_00s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_01m_00s__02m_01s_20h.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_02m_01s_20h__03m_00s_10h.ogg\" created
 file split"
  tags_option="%[@a=a1,@b=b1,@n=10][]%[@o,@b=album,@N=7][@a=custom_artist][@o,@n=20]"
  mp3splt_args="-d $OUTPUT_DIR -g \"$tags_option\" $OGG_FILE 0.05 0.30 1.0 2.1.2 3.0.1" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${O_FILE}_00m_05s__00m_30s.ogg"
  check_all_ogg_tags "a1" "b1" "" "" "" "10" ""

  current_file="$OUTPUT_DIR/${O_FILE}_00m_30s__01m_00s.ogg"
  check_all_ogg_tags "a1" "b1" "" "" "" "10" ""

  current_file="$OUTPUT_DIR/${O_FILE}_01m_00s__02m_01s_20h.ogg"
  check_all_ogg_tags "Kelly Allyn" "album"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "7"\
                     "http://www.jamendo.com"

  current_file="$OUTPUT_DIR/${O_FILE}_02m_01s_20h__03m_00s_10h.ogg" 
  check_all_ogg_tags "custom_artist" "album"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "8"\
                     "http://www.jamendo.com"

  print_ok
  echo
}

function test_normal_stdin
{
  remove_output_dir

  test_name="stdin"
 
  O_FILE="Kelly_Allyn__Whiskey_Can"

  #TODO: ogg stdin total time ?
  expected=" Processing file 'o-' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 0m.00s
 info: starting normal split
   File \"$OUTPUT_DIR/o-_01m_00s__02m_01s_20h.ogg\" created
   File \"$OUTPUT_DIR/o-_02m_01s_20h__03m_00s_10h.ogg\" created
   File \"$OUTPUT_DIR/o-_03m_00s_10h__EOF.ogg\" created
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR o- 1.0 2.1.2 3.0.1 EOF" 
  run_custom_check_output "cat songs/${O_FILE}.ogg | $MP3SPLT" "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/o-_01m_00s__02m_01s_20h.ogg"
  check_current_ogg_length "1m:01.200s"
  check_current_ogg_no_tags
  check_current_file_size "1637491"

  current_file="$OUTPUT_DIR/o-_02m_01s_20h__03m_00s_10h.ogg" 
  check_current_ogg_length "0m:58.899s"
  check_current_ogg_no_tags
  check_current_file_size "1512872"

  current_file="$OUTPUT_DIR/o-_03m_00s_10h__EOF.ogg" 
  check_current_ogg_length "0m:04.753s"
  check_current_ogg_no_tags
  check_current_file_size "52979"

  print_ok
  echo
}

function test_normal_stdin_no_input_tags
{
  remove_output_dir

  test_name="stdin & no input tags"
 
  O_FILE="Kelly_Allyn__Whiskey_Can__no_tags"

  expected=" Processing file 'o-' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 0m.00s
 info: starting normal split
   File \"$OUTPUT_DIR/o-_01m_00s__02m_01s_20h.ogg\" created
   File \"$OUTPUT_DIR/o-_02m_01s_20h__03m_00s_10h.ogg\" created
   File \"$OUTPUT_DIR/o-_03m_00s_10h__EOF.ogg\" created
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR o- 1.0 2.1.2 3.0.1 EOF" 
  run_custom_check_output "cat songs/${O_FILE}.ogg | $MP3SPLT" "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/o-_01m_00s__02m_01s_20h.ogg"
  check_current_ogg_length "1m:01.200s"
  check_current_ogg_no_tags
  check_current_file_size "1637491"

  current_file="$OUTPUT_DIR/o-_02m_01s_20h__03m_00s_10h.ogg" 
  check_current_ogg_length "0m:58.899s"
  check_current_ogg_no_tags
  check_current_file_size "1512872"

  current_file="$OUTPUT_DIR/o-_03m_00s_10h__EOF.ogg" 
  check_current_ogg_length "0m:04.753s"
  check_current_ogg_no_tags
  check_current_file_size "52979"

  print_ok
  echo
}

function test_normal_stdin_with_custom_tags
{
  remove_output_dir

  test_name="stdin & custom tags"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'o-' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 0m.00s
 info: starting normal split
   File \"$OUTPUT_DIR/o-_01m_00s__02m_01s_20h.ogg\" created
   File \"$OUTPUT_DIR/o-_02m_01s_20h__03m_00s_10h.ogg\" created
   File \"$OUTPUT_DIR/o-_03m_00s_10h__EOF.ogg\" created
 file split (EOF)"
  mp3splt_args="-g %[@a=a1,@b=b1,@y=1070,@N=1] -d $OUTPUT_DIR o- 1.0 2.1.2 3.0.1 EOF" 
  run_custom_check_output "cat songs/${O_FILE}.ogg | $MP3SPLT" "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/o-_01m_00s__02m_01s_20h.ogg"
  check_current_ogg_length "1m:01.200s"
  check_all_ogg_tags "a1" "b1" "" "1070" "" "1" ""
  check_current_file_size "1637546"

  current_file="$OUTPUT_DIR/o-_02m_01s_20h__03m_00s_10h.ogg" 
  check_current_ogg_length "0m:58.899s"
  check_all_ogg_tags "a1" "b1" "" "1070" "" "2" ""
  check_current_file_size "1512927"

  current_file="$OUTPUT_DIR/o-_03m_00s_10h__EOF.ogg" 
  check_current_ogg_length "0m:04.753s"
  check_all_ogg_tags "a1" "b1" "" "1070" "" "3" ""
  check_current_file_size "53034"

  print_ok
  echo
}

function test_normal_output_fname
{
  remove_output_dir

  test_name="output fname"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" warning: output format ambiguous (@t or @n missing)
 Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/test.ogg\" created
 file split"
  mp3splt_args="-o 'test' -d $OUTPUT_DIR $OGG_FILE 1.0 2.0" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/test.ogg"
  check_current_ogg_length "1m:00.000s"

  print_ok
  echo
}

function test_normal_output_fnames_and_custom_tags
{
  remove_output_dir

  test_name="output fnames & custom tags"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/a1_b1_t1_1_10_${O_FILE} 01:00:00 02:01:20.ogg\" created
   File \"$OUTPUT_DIR/___2_2_${O_FILE} 02:01:20 03:00:10.ogg\" created
   File \"$OUTPUT_DIR/Kelly Allyn_album_Whiskey Can_3_7_${O_FILE} 03:00:10 03:04:85.ogg\" created
 file split (EOF)"
  tags_option="[@a=a1,@b=b1,@t=t1,@y=2000,@c=my_comment,@n=10][]%[@o,@b=album,@N=7]"
  output_option="\"@a_@b_@t_@n_@N_@f+@m:@s:@h @M:@S:@H\""
  mp3splt_args="-d $OUTPUT_DIR -g \"$tags_option\" -o $output_option $OGG_FILE 1.0 2.1.2 3.0.1 EOF"
  run_check_output "$mp3splt_args" "$expected"

  check_if_file_exist "$OUTPUT_DIR/a1_b1_t1_1_10_${O_FILE} 01:00:00 02:01:20.ogg"
  check_if_file_exist "$OUTPUT_DIR/___2_2_${O_FILE} 02:01:20 03:00:10.ogg"
  check_if_file_exist "$OUTPUT_DIR/Kelly Allyn_album_Whiskey Can_3_7_${O_FILE} 03:00:10 03:04:85.ogg"

  print_ok
  echo
}

function test_normal_output_fnames_and_dirs
{
  remove_output_dir

  test_name="output fnames & directories"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  local artist="Kelly Allyn"
  local album="Getting Back From Where I've Been"
  local title="Whiskey Can"
  local genre="Southern Rock"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/$artist/$album/$artist-$title-$genre 1.ogg\" created
   File \"$OUTPUT_DIR/$artist/$album/$artist-$title-$genre 2.ogg\" created
   File \"$OUTPUT_DIR/$artist/$album/$artist-$title-$genre 3.ogg\" created
 file split (EOF)"
  output_option="@a/@b/@a-@t-@g @n"
  mp3splt_args="-o '$output_option' -d $OUTPUT_DIR $OGG_FILE 1.0 2.0.2 3.0 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  check_if_directory_exist "$OUTPUT_DIR/$artist"
  check_if_directory_exist "$OUTPUT_DIR/$artist/$album"
  check_if_file_exist "$OUTPUT_DIR/$artist/$album/$artist-$title-$genre 1.ogg"
  check_if_file_exist "$OUTPUT_DIR/$artist/$album/$artist-$title-$genre 2.ogg"
  check_if_file_exist "$OUTPUT_DIR/$artist/$album/$artist-$title-$genre 3.ogg"

  print_ok
  echo
}

function test_normal_output_fnames_and_custom_tags_and_dirs
{
  remove_output_dir

  test_name="output fnames & custom tags & directories"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  local artist="Kelly Allyn"
  local title="Whiskey Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/$artist/album1/$artist-$title-Southern Rock 1.ogg\" created
   File \"$OUTPUT_DIR/$artist/album2/$artist-$title-Southern Rock 2.ogg\" created
   File \"$OUTPUT_DIR/$artist/album3/$artist-$title-Speech 3.ogg\" created
 file split (EOF)"
  output_option="@a/@b/@a-@t-@g @n"
  tags_option="%[@o,@b=album1][@b=album2][@b=album3,@g=Speech]"
  mp3splt_args="-o '$output_option' -g \"$tags_option\" -d $OUTPUT_DIR $OGG_FILE 1.0 2.0.2 3.0 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  check_if_directory_exist "$OUTPUT_DIR/$artist"
  check_if_directory_exist "$OUTPUT_DIR/$artist/album1"
  check_if_directory_exist "$OUTPUT_DIR/$artist/album2"
  check_if_directory_exist "$OUTPUT_DIR/$artist/album3"
  check_if_file_exist "$OUTPUT_DIR/$artist/album1/$artist-$title-Southern Rock 1.ogg"
  check_if_file_exist "$OUTPUT_DIR/$artist/album2/$artist-$title-Southern Rock 2.ogg"
  check_if_file_exist "$OUTPUT_DIR/$artist/album3/$artist-$title-Speech 3.ogg"

  print_ok
  echo
}

function test_normal_stdout
{
  remove_output_dir

  test_name="stdout"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"-\" created
 file split"
  mp3splt_args="-o - $OGG_FILE 1.0 2.30.2"
  run_custom_check_output "$MP3SPLT $mp3splt_args > $OUTPUT_DIR/stdout.ogg" "" "$expected"

  current_file="$OUTPUT_DIR/stdout.ogg"
  check_current_ogg_length "1m:30.199s"
  check_current_file_size "2396476"

  print_ok
  echo
}

function test_normal_stdout_multiple_splitpoints
{
  remove_output_dir

  test_name="stdout & splitpoints > 2"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Warning: multiple splitpoints with stdout !
 Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"-\" created
   File \"-\" created
 file split"
  mp3splt_args="-o - $OGG_FILE 1.0 2.30.2 2.50"
  run_custom_check_output "$MP3SPLT $mp3splt_args > $OUTPUT_DIR/stdout.ogg" "" "$expected"

  current_file="$OUTPUT_DIR/stdout.ogg"
#TODO: 2 outputs are concatenated in the same file ? should we do something ?
  check_current_ogg_length "1m:30.199s
0m:19.800s"
  check_current_file_size "2917206"

  print_ok
  echo
}

function test_normal_custom_tags_with_replace_tags_in_tags
{
  remove_output_dir

  test_name="custom tags & replace tags in tags"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  F1="-a_10-b_a_@n-10.ogg"
  F2="Whiskey Can-Kelly Allyn-album_cc_@t-7.ogg"
  F3="Whiskey Can-Kelly Allyn-album_cc_@t-8.ogg"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/$F1\" created
   File \"$OUTPUT_DIR/$F2\" created
   File \"$OUTPUT_DIR/$F3\" created
 file split"
  tags_option="r[@a=a_@n,@b=b_@a,@c=cc_@b,@n=10]%[@o,@c=cc_@t,@b=album_@c,@N=7]"
  output_option="@t-@a-@b-@N"
  mp3splt_args="-d $OUTPUT_DIR -o $output_option -g \"$tags_option\" $OGG_FILE 0.5 1.0 1.5 2.0"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/$F1"
  check_all_ogg_tags "a_10" "b_a_@n" "" "" "" "10" "cc_b_@a"

  current_file="$OUTPUT_DIR/$F2"
  check_all_ogg_tags "Kelly Allyn" "album_cc_@t" "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "7" "cc_Whiskey Can"

  current_file="$OUTPUT_DIR/$F3"
  check_all_ogg_tags "Kelly Allyn" "album_cc_@t" "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "8" "cc_Whiskey Can"

  print_ok
  echo
}

function test_normal_custom_tags_without_replace_tags_in_tags
{
  remove_output_dir

  test_name="custom tags without replace tags in tags"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  F1="-a_@n-b_@a-10.ogg"
  F2="Whiskey Can-Kelly Allyn-album_@c-7.ogg"
  F3="Whiskey Can-Kelly Allyn-album_@c-8.ogg"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/$F1\" created
   File \"$OUTPUT_DIR/$F2\" created
   File \"$OUTPUT_DIR/$F3\" created
 file split"
  tags_option="[@a=a_@n,@b=b_@a,@c=cc_@b,@n=10]%[@o,@c=cc_@t,@b=album_@c,@N=7]"
  output_option="@t-@a-@b-@N"
  mp3splt_args="-d $OUTPUT_DIR -o $output_option -g \"$tags_option\" $OGG_FILE 0.5 1.0 1.5 2.0"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/$F1"
  check_all_ogg_tags "a_@n" "b_@a" "" "" "" "10" "cc_@b"

  current_file="$OUTPUT_DIR/$F2"
  check_all_ogg_tags "Kelly Allyn" "album_@c" "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "7" "cc_@t"

  current_file="$OUTPUT_DIR/$F3"
  check_all_ogg_tags "Kelly Allyn" "album_@c" "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "8" "cc_@t"

  print_ok
  echo
}

function test_normal_split_in_equal_parts
{
  remove_output_dir

  test_name="split in equal parts"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting 'split in equal tracks' mode
   File \"$OUTPUT_DIR/1.ogg\" created
   File \"$OUTPUT_DIR/2.ogg\" created
   File \"$OUTPUT_DIR/3.ogg\" created
   File \"$OUTPUT_DIR/4.ogg\" created
 split in equal tracks ok"
  mp3splt_args="-d $OUTPUT_DIR -o @n -S 4 $OGG_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/1.ogg"
  check_current_ogg_length "0m:46.219s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "1"\
                     "http://www.jamendo.com"

  current_file="$OUTPUT_DIR/2.ogg" 
  check_current_ogg_length "0m:46.217s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "2"\
                     "http://www.jamendo.com"

  current_file="$OUTPUT_DIR/3.ogg" 
  check_current_ogg_length "0m:46.214s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "3"\
                     "http://www.jamendo.com"

  current_file="$OUTPUT_DIR/4.ogg" 
  check_current_ogg_length "0m:46.200s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "4"\
                     "http://www.jamendo.com"

  print_ok
  echo
}

function test_normal_with_tags_from_filename_regex
{
  remove_output_dir

  test_name="with tags from filename regex"

  NEW_O_FILE="artist1__album2__title3__comment4__2__2004__Samba"
  NEW_OGG_FILE=$SONGS_DIR/${NEW_O_FILE}.ogg

  cp $OGG_FILE $NEW_OGG_FILE

  expected=" Processing file 'songs/${NEW_O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/${NEW_O_FILE}_01m_00s__02m_01s_20h.ogg\" created
   File \"$OUTPUT_DIR/${NEW_O_FILE}_02m_01s_20h__03m_00s_10h.ogg\" created
   File \"$OUTPUT_DIR/${NEW_O_FILE}_03m_00s_10h__03m_04s_85h.ogg\" created
 file split (EOF)"
  regex_option="(?<artist>.*?)__(?<album>.*?)__(?<title>.*?)__(?<comment>.*?)__(?<tracknum>.*?)__(?<year>.*?)__(?<genre>.*)"
  mp3splt_args="-d $OUTPUT_DIR -G \"regex=$regex_option\" $NEW_OGG_FILE 1.0 2.1.2 3.0.1 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${NEW_O_FILE}_01m_00s__02m_01s_20h.ogg"
  check_current_ogg_length "1m:01.200s"
  check_all_ogg_tags "artist1" "album2" "title3" "2004" "Samba" "2" "comment4"

  current_file="$OUTPUT_DIR/${NEW_O_FILE}_02m_01s_20h__03m_00s_10h.ogg" 
  check_current_ogg_length "0m:58.899s"
  check_all_ogg_tags "artist1" "album2" "title3" "2004" "Samba" "2" "comment4"

  current_file="$OUTPUT_DIR/${NEW_O_FILE}_03m_00s_10h__03m_04s_85h.ogg" 
  check_current_ogg_length "0m:04.753s"
  check_all_ogg_tags "artist1" "album2" "title3" "2004" "Samba" "2" "comment4"

  rm -f $NEW_OGG_FILE

  print_ok
  echo
}

function test_normal_with_auto_adjust
{
  remove_output_dir

  test_name="normal with auto adjust"

  O_FILE="Kelly_Allyn__Whiskey_Can_silence"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 156 Kb/s - 2 channels - Total time: 3m.04s
 Working with SILENCE AUTO-ADJUST (Threshold: -48.0 dB Gap: 30 sec Offset: 0.80 Min: 0.00 sec)
 info: starting normal split
   File \"$OUTPUT_DIR/${O_FILE}_01m_00s__02m_00s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_02m_00s__03m_00s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_03m_00s__03m_04s_85h.ogg\" created
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR $SILENCE_OGG_FILE -a 1.0 2.0 3.0 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${O_FILE}_01m_00s__02m_00s.ogg"
  check_current_ogg_length "0m:44.154s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "1"\
                     "http://www.jamendo.com"
  check_current_file_size "698968"

  current_file="$OUTPUT_DIR/${O_FILE}_02m_00s__03m_00s.ogg" 
  check_current_ogg_length "1m:00.000s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "2"\
                     "http://www.jamendo.com"
  check_current_file_size "1049558"

  current_file="$OUTPUT_DIR/${O_FILE}_03m_00s__03m_04s_85h.ogg" 
  check_current_ogg_length "0m:20.698s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "3"\
                     "http://www.jamendo.com"
  check_current_file_size "328066"

  print_ok
  echo
}

function test_normal_with_auto_adjust_min_length
{
  remove_output_dir

  test_name="normal with auto adjust & min length parameter"

  O_FILE="Kelly_Allyn__Whiskey_Can_silence"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 156 Kb/s - 2 channels - Total time: 3m.04s
 Working with SILENCE AUTO-ADJUST (Threshold: -48.0 dB Gap: 40 sec Offset: 0.00 Min: 9.00 sec)
 info: starting normal split
   File \"$OUTPUT_DIR/${O_FILE}_00m_00s__01m_20s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_01m_20s__03m_04s_85h.ogg\" created
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR $SILENCE_OGG_FILE -p gap=40,min=9,off=0 -a 0.0 1.20 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${O_FILE}_00m_00s__01m_20s.ogg"
  check_current_ogg_length "0m:50.417s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "1"\
                     "http://www.jamendo.com"
  check_current_file_size "948149"

  current_file="$OUTPUT_DIR/${O_FILE}_01m_20s__03m_04s_85h.ogg" 
  check_current_ogg_length "2m:14.435s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "2"\
                     "http://www.jamendo.com"
  check_current_file_size "2070064"

  print_ok
  echo
}

function test_normal_with_auto_adjust_min_length_no_change
{
  remove_output_dir

  test_name="normal with auto adjust & min length parameter no change"

  O_FILE="Kelly_Allyn__Whiskey_Can_silence"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 156 Kb/s - 2 channels - Total time: 3m.04s
 Working with SILENCE AUTO-ADJUST (Threshold: -48.0 dB Gap: 40 sec Offset: 0.00 Min: 10.00 sec)
 info: starting normal split
   File \"$OUTPUT_DIR/${O_FILE}_00m_00s__01m_20s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_01m_20s__03m_04s_85h.ogg\" created
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR $SILENCE_OGG_FILE -p gap=40,min=10,off=0 -a 0.0 1.20 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${O_FILE}_00m_00s__01m_20s.ogg"
  check_current_ogg_length "1m:20.000s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "1"\
                     "http://www.jamendo.com"
  check_current_file_size "1310298"

  current_file="$OUTPUT_DIR/${O_FILE}_01m_20s__03m_04s_85h.ogg" 
  check_current_ogg_length "1m:44.853s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "2"\
                     "http://www.jamendo.com"
  check_current_file_size "1708429"

  print_ok
  echo
}

function test_normal_vbr_with_negative_splitpoints
{
  remove_output_dir

  test_name="normal with negative splitpoints"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/${O_FILE}_01m_00s__01m_04s_85h.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_01m_04s_85h__02m_04s_85h.ogg\" created
 file split"
  mp3splt_args="-d $OUTPUT_DIR $OGG_FILE 1.0 EOF-2.0 EOF-1.0" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${O_FILE}_01m_00s__01m_04s_85h.ogg"
  check_current_ogg_length "0m:04.849s"

  current_file="$OUTPUT_DIR/${O_FILE}_01m_04s_85h__02m_04s_85h.ogg" 
  check_current_ogg_length "1m:00.000s"

  print_ok
  echo
}


function run_normal_tests
{
  p_blue " NORMAL ogg tests ..."
  echo

  normal_test_functions=$(declare -F | grep " test_normal_" | awk '{ print $3 }')

  for test_func in $normal_test_functions;do
    eval $test_func
  done

  p_blue " NORMAL ogg tests DONE."
  echo
}

#main
export LC_ALL="C"
start_date=$(date +%s)

run_normal_tests

p_failed_tests

end_date=$(date +%s)

p_time_diff_cyan $start_date $end_date "\t"
echo -e '\n'

exit 0

