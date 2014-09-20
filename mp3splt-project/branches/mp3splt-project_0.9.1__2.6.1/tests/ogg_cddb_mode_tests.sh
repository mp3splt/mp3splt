#!/bin/bash

. ./utils.sh || exit 1

function _create_cddb_file
{
  echo "# xmcd
#
# Track frame offsets:
# 150
# 5000
# 10000
# 16230
#
# Disc length: 245 seconds
#
# Revision: 1
# Processed by: tracktype.org
# Submitted via: anonymous
#
DISCID=fffffff
DTITLE=Don't worry / Hack me
DYEAR=2009
DGENRE=
TTITLE0=A famous title
TTITLE1=Yeah, split me !
TTITLE2=MS / 7 sins campaign
TTITLE3=What the hack ?
EXTD=
EXTT0=
EXTT1=
EXTT2=
EXTT3=
PLAYORDER=
.
" > $CDDB_FILE
}

function test_cddb_mode
{
  _create_cddb_file

  remove_output_dir

  test_name="cddb mode"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 reading informations from CDDB file songs/test.cddb ...

  Artist: Don't worry
  Album: Hack me
  Tracks: 4

 cddb file processed
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/Don't worry - 1 - A famous title.ogg\" created
   File \"$OUTPUT_DIR/Don't worry - 2 - Yeah, split me !.ogg\" created
   File \"$OUTPUT_DIR/MS - 3 - 7 sins campaign.ogg\" created
 file split (EOF)
$auto_adjust_warning"
  mp3splt_args="-d $OUTPUT_DIR -c $CDDB_FILE $OGG_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/Don't worry - 1 - A famous title.ogg"
  check_current_ogg_length "1m:04.670s"
  check_current_file_size "1747391"
  check_all_ogg_tags "Don't worry" "Hack me" "A famous title" "2009" "Other" "1" ""

  current_file="$OUTPUT_DIR/Don't worry - 2 - Yeah, split me !.ogg"
  check_current_ogg_length "1m:06.670s"
  check_current_file_size "1784037"
  check_all_ogg_tags "Don't worry" "Hack me" "Yeah, split me !" "2009" "Other" "2" ""

  current_file="$OUTPUT_DIR/MS - 3 - 7 sins campaign.ogg"
  check_current_ogg_length "0m:53.513s"
  check_current_file_size "1296876"
  check_all_ogg_tags "MS" "Hack me" "7 sins campaign" "2009" "Other" "3" ""

  print_ok
  echo
}

function test_cddb_mode_and_pretend
{
  _create_cddb_file

  remove_output_dir

  test_name="cddb mode & pretend"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Pretending to split file 'songs/${O_FILE}.ogg' ...
 reading informations from CDDB file songs/test.cddb ...

  Artist: Don't worry
  Album: Hack me
  Tracks: 4

 cddb file processed
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/Don't worry - 1 - A famous title.ogg\" created
   File \"$OUTPUT_DIR/Don't worry - 2 - Yeah, split me !.ogg\" created
   File \"$OUTPUT_DIR/MS - 3 - 7 sins campaign.ogg\" created
 file split (EOF)
$auto_adjust_warning"
  mp3splt_args="-P -d $OUTPUT_DIR -c $CDDB_FILE $OGG_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_is_empty

  print_ok
  echo
}

function test_cddb_mode_and_cue_export
{
  _create_cddb_file

  remove_output_dir

  test_name="cddb mode & cue export"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 reading informations from CDDB file songs/test.cddb ...

  Artist: Don't worry
  Album: Hack me
  Tracks: 4

 cddb file processed
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/Don't worry - 1 - A famous title.ogg\" created
   File \"$OUTPUT_DIR/Don't worry - 2 - Yeah, split me !.ogg\" created
   File \"$OUTPUT_DIR/MS - 3 - 7 sins campaign.ogg\" created
 file split (EOF)
 CUE file 'output/output_out.cue' created.
$auto_adjust_warning"
  mp3splt_args="-E output/out.cue -d $OUTPUT_DIR -c $CDDB_FILE $OGG_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_number_of_files 4

  check_file_content "output/output_out.cue" 'TITLE "Hack me"
PERFORMER "Don'\''t worry"
FILE "songs/Kelly_Allyn__Whiskey_Can.ogg" OGG
  TRACK 01 AUDIO
    TITLE "A famous title"
    PERFORMER "Don'\''t worry"
    REM ALBUM "Hack me"
    REM GENRE "Other"
    REM DATE "2009"
    REM TRACK "1"
    REM NAME "Don'\''t worry - 1 - A famous title"
    INDEX 01 00:00:00
  TRACK 02 AUDIO
    TITLE "Yeah, split me !"
    PERFORMER "Don'\''t worry"
    REM ALBUM "Hack me"
    REM GENRE "Other"
    REM DATE "2009"
    REM TRACK "2"
    REM NAME "Don'\''t worry - 2 - Yeah, split me !"
    INDEX 01 01:04:50
  TRACK 03 AUDIO
    TITLE "7 sins campaign"
    PERFORMER "MS"
    REM ALBUM "Hack me"
    REM GENRE "Other"
    REM DATE "2009"
    REM TRACK "3"
    REM NAME "MS - 3 - 7 sins campaign"
    INDEX 01 02:11:26'

  current_file="$OUTPUT_DIR/Don't worry - 1 - A famous title.ogg"
  check_current_ogg_length "1m:04.670s"
  check_current_file_size "1747391"
  check_all_ogg_tags "Don't worry" "Hack me" "A famous title" "2009" "Other" "1" ""

  current_file="$OUTPUT_DIR/Don't worry - 2 - Yeah, split me !.ogg"
  check_current_ogg_length "1m:06.670s"
  check_current_file_size "1784037"
  check_all_ogg_tags "Don't worry" "Hack me" "Yeah, split me !" "2009" "Other" "2" ""

  current_file="$OUTPUT_DIR/MS - 3 - 7 sins campaign.ogg"
  check_current_ogg_length "0m:53.513s"
  check_current_file_size "1296876"
  check_all_ogg_tags "MS" "Hack me" "7 sins campaign" "2009" "Other" "3" ""

  print_ok
  echo
}

function test_cddb_mode_prented_and_cue
{
  _create_cddb_file

  remove_output_dir

  test_name="cddb mode & cue export & pretend"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Pretending to split file 'songs/${O_FILE}.ogg' ...
 reading informations from CDDB file songs/test.cddb ...

  Artist: Don't worry
  Album: Hack me
  Tracks: 4

 cddb file processed
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/Don't worry - 1 - A famous title.ogg\" created
   File \"$OUTPUT_DIR/Don't worry - 2 - Yeah, split me !.ogg\" created
   File \"$OUTPUT_DIR/MS - 3 - 7 sins campaign.ogg\" created
 file split (EOF)
 CUE file 'output/output_out.cue' created.
$auto_adjust_warning"
  mp3splt_args="-P -E output/out.cue -d $OUTPUT_DIR -c $CDDB_FILE $OGG_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_number_of_files 1

  check_file_content "output/output_out.cue" 'TITLE "Hack me"
PERFORMER "Don'\''t worry"
FILE "songs/Kelly_Allyn__Whiskey_Can.ogg" OGG
  TRACK 01 AUDIO
    TITLE "A famous title"
    PERFORMER "Don'\''t worry"
    REM ALBUM "Hack me"
    REM GENRE "Other"
    REM DATE "2009"
    REM TRACK "1"
    REM NAME "Don'\''t worry - 1 - A famous title"
    INDEX 01 00:00:00
  TRACK 02 AUDIO
    TITLE "Yeah, split me !"
    PERFORMER "Don'\''t worry"
    REM ALBUM "Hack me"
    REM GENRE "Other"
    REM DATE "2009"
    REM TRACK "2"
    REM NAME "Don'\''t worry - 2 - Yeah, split me !"
    INDEX 01 01:04:50
  TRACK 03 AUDIO
    TITLE "7 sins campaign"
    PERFORMER "MS"
    REM ALBUM "Hack me"
    REM GENRE "Other"
    REM DATE "2009"
    REM TRACK "3"
    REM NAME "MS - 3 - 7 sins campaign"
    INDEX 01 02:11:26'

  print_ok
  echo
}

function test_cddb_mode_quiet
{
  _create_cddb_file

  remove_output_dir

  test_name="cddb mode & quiet"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 reading informations from CDDB file songs/test.cddb ...

  Artist: Don't worry
  Album: Hack me
  Tracks: 4

 cddb file processed
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/Don't worry - 1 - A famous title.ogg\" created
   File \"$OUTPUT_DIR/Don't worry - 2 - Yeah, split me !.ogg\" created
   File \"$OUTPUT_DIR/MS - 3 - 7 sins campaign.ogg\" created
 file split (EOF)"
  mp3splt_args="-q -d $OUTPUT_DIR -c $CDDB_FILE $OGG_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  print_ok
  echo
}

function test_cddb_mode_and_output_format
{
  _create_cddb_file

  remove_output_dir

  test_name="cddb mode & output format"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 reading informations from CDDB file songs/test.cddb ...

  Artist: Don't worry
  Album: Hack me
  Tracks: 4

 cddb file processed
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/001-Don't worry--Hack me-A famous title.ogg\" created
   File \"$OUTPUT_DIR/002-Don't worry--Hack me-Yeah, split me !.ogg\" created
   File \"$OUTPUT_DIR/003-Don't worry-MS-Hack me-7 sins campaign.ogg\" created
 file split (EOF)
$auto_adjust_warning"
  mp3splt_args="-d $OUTPUT_DIR -o @n3-@a-@p-@b-@t -c $CDDB_FILE $OGG_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/001-Don't worry--Hack me-A famous title.ogg"
  check_current_ogg_length "1m:04.670s"

  current_file="$OUTPUT_DIR/002-Don't worry--Hack me-Yeah, split me !.ogg"
  check_current_ogg_length "1m:06.670s"

  current_file="$OUTPUT_DIR/003-Don't worry-MS-Hack me-7 sins campaign.ogg"
  check_current_ogg_length "0m:53.513s"

  print_ok
  echo
}

function test_cddb_mode_and_output_format_artist_or_performer
{
  _create_cddb_file

  remove_output_dir

  test_name="cddb mode & output format artist or performer"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 reading informations from CDDB file songs/test.cddb ...

  Artist: Don't worry
  Album: Hack me
  Tracks: 4

 cddb file processed
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/001-Don't worry-A famous title.ogg\" created
   File \"$OUTPUT_DIR/002-Don't worry-Yeah, split me !.ogg\" created
   File \"$OUTPUT_DIR/003-MS-7 sins campaign.ogg\" created
 file split (EOF)
$auto_adjust_warning"
  mp3splt_args="-d $OUTPUT_DIR -o @n3-@A-@t -c $CDDB_FILE $OGG_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  print_ok
  echo
}

function test_cddb_mode_and_output_format_and_create_dirs
{
  _create_cddb_file

  remove_output_dir

  test_name="cddb mode & output format & create dirs"

  O_FILE="Kelly_Allyn__Whiskey_Can"

  out_dir="$OUTPUT_DIR/Don't worry"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 reading informations from CDDB file songs/test.cddb ...

  Artist: Don't worry
  Album: Hack me
  Tracks: 4

 cddb file processed
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 218 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$out_dir/1/001-Don't worry--Hack me-A famous title.ogg\" created
   File \"$out_dir/2/002-Don't worry--Hack me-Yeah, split me !.ogg\" created
   File \"$out_dir/3/003-Don't worry-MS-Hack me-7 sins campaign.ogg\" created
 file split (EOF)
$auto_adjust_warning"
  mp3splt_args="-d $OUTPUT_DIR -o @a/@n/@n3-@a-@p-@b-@t -c $CDDB_FILE $OGG_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$out_dir/1/001-Don't worry--Hack me-A famous title.ogg"
  check_current_ogg_length "1m:04.670s"

  current_file="$out_dir/2/002-Don't worry--Hack me-Yeah, split me !.ogg"
  check_current_ogg_length "1m:06.670s"

  current_file="$out_dir/3/003-Don't worry-MS-Hack me-7 sins campaign.ogg"
  check_current_ogg_length "0m:53.513s"

  print_ok
  echo
}

function run_cddb_mode_tests
{
  p_blue " CDDB ogg tests ..."
  echo

  cddb_mode_test_functions=$(declare -F | grep " test_cddb" | awk '{ print $3 }')

  for test_func in $cddb_mode_test_functions;do
    eval $test_func
  done

  p_blue " CDDB ogg tests DONE."
  echo
}

#main
export LC_ALL="C"
start_date=$(date +%s)

run_cddb_mode_tests

p_failed_tests

end_date=$(date +%s)

p_time_diff_cyan $start_date $end_date "\t"
echo -e '\n'

exit 0

