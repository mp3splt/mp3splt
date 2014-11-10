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

  M_FILE="La_Verue__Today"

  test_name="cddb mode"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 reading informations from CDDB file songs/test.cddb ...

  Artist: Don't worry
  Album: Hack me
  Tracks: 4

 cddb file processed
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/Don't worry - 1 - A famous title.mp3\" created
   File \"$OUTPUT_DIR/Don't worry - 2 - Yeah, split me !.mp3\" created
   File \"$OUTPUT_DIR/MS - 3 - 7 sins campaign.mp3\" created
   File \"$OUTPUT_DIR/Don't worry - 4 - What the hack _.mp3\" created
 Processed 9380 frames - Sync errors: 0
 file split
$auto_adjust_warning"
  mp3splt_args="-d $OUTPUT_DIR -c $CDDB_FILE $MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/Don't worry - 1 - A famous title.mp3"
  check_current_mp3_length "01.04"
  check_current_file_has_xing
  check_current_file_size "1424387"
  check_all_mp3_tags_with_version "2" "Don't worry" "Hack me" "A famous title"\
  "2009" "Other" "12" "1" ""

  current_file="$OUTPUT_DIR/Don't worry - 2 - Yeah, split me !.mp3"
  check_current_mp3_length "01.06"
  check_current_file_has_xing
  check_current_file_size "1499156"
  check_all_mp3_tags_with_version "2" "Don't worry" "Hack me" "Yeah, split me !"\
  "2009" "Other" "12" "2" ""

  current_file="$OUTPUT_DIR/MS - 3 - 7 sins campaign.mp3"
  check_current_mp3_length "01.23"
  check_current_file_has_xing
  check_current_file_size "1989671"
  check_all_mp3_tags_with_version "2" "MS" "Hack me" "7 sins campaign"\
  "2009" "Other" "12" "3" ""

  current_file="$OUTPUT_DIR/Don't worry - 4 - What the hack _.mp3"
  check_current_mp3_length "00.30"
  check_current_file_has_xing
  check_current_file_size "690906"
  check_all_mp3_tags_with_version "2" "Don't worry" "Hack me" "What the hack ?"\
  "2009" "Other" "12" "4" ""

  print_ok
  echo
}

function test_cddb_mode_and_pretend
{
  _create_cddb_file

  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="cddb mode & pretend"

  expected=" Pretending to split file 'songs/${M_FILE}.mp3' ...
 reading informations from CDDB file songs/test.cddb ...

  Artist: Don't worry
  Album: Hack me
  Tracks: 4

 cddb file processed
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/Don't worry - 1 - A famous title.mp3\" created
   File \"$OUTPUT_DIR/Don't worry - 2 - Yeah, split me !.mp3\" created
   File \"$OUTPUT_DIR/MS - 3 - 7 sins campaign.mp3\" created
   File \"$OUTPUT_DIR/Don't worry - 4 - What the hack _.mp3\" created
 Processed 9380 frames - Sync errors: 0
 file split
$auto_adjust_warning"
  mp3splt_args="-P -d $OUTPUT_DIR -c $CDDB_FILE $MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_is_empty

  print_ok
  echo
}

function test_cddb_mode_and_cue_export
{
  _create_cddb_file

  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="cddb mode & cue export"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 reading informations from CDDB file songs/test.cddb ...

  Artist: Don't worry
  Album: Hack me
  Tracks: 4

 cddb file processed
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/Don't worry - 1 - A famous title.mp3\" created
   File \"$OUTPUT_DIR/Don't worry - 2 - Yeah, split me !.mp3\" created
   File \"$OUTPUT_DIR/MS - 3 - 7 sins campaign.mp3\" created
   File \"$OUTPUT_DIR/Don't worry - 4 - What the hack _.mp3\" created
 Processed 9380 frames - Sync errors: 0
 file split
 CUE file 'output/output_out.cue' created.
$auto_adjust_warning"
  mp3splt_args="-E output/out.cue -d $OUTPUT_DIR -c $CDDB_FILE $MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_number_of_files 5

  check_file_content "output/output_out.cue" "TITLE \"Hack me\"
PERFORMER \"Don't worry\"
FILE \"songs/La_Verue__Today.mp3\" MP3
  TRACK 01 AUDIO
    TITLE \"A famous title\"
    PERFORMER \"Don't worry\"
    REM ALBUM \"Hack me\"
    REM GENRE \"Other\"
    REM DATE \"2009\"
    REM TRACK \"1\"
    REM NAME \"Don't worry - 1 - A famous title\"
    INDEX 01 00:00:00
  TRACK 02 AUDIO
    TITLE \"Yeah, split me !\"
    PERFORMER \"Don't worry\"
    REM ALBUM \"Hack me\"
    REM GENRE \"Other\"
    REM DATE \"2009\"
    REM TRACK \"2\"
    REM NAME \"Don't worry - 2 - Yeah, split me !\"
    INDEX 01 01:04:50
  TRACK 03 AUDIO
    TITLE \"7 sins campaign\"
    PERFORMER \"MS\"
    REM ALBUM \"Hack me\"
    REM GENRE \"Other\"
    REM DATE \"2009\"
    REM TRACK \"3\"
    REM NAME \"MS - 3 - 7 sins campaign\"
    INDEX 01 02:11:26
  TRACK 04 AUDIO
    TITLE \"What the hack ?\"
    PERFORMER \"Don't worry\"
    REM ALBUM \"Hack me\"
    REM GENRE \"Other\"
    REM DATE \"2009\"
    REM TRACK \"4\"
    REM NAME \"Don't worry - 4 - What the hack _\"
    INDEX 01 03:34:30
  TRACK 05 AUDIO
    TITLE \"\"
    PERFORMER \"\"
    INDEX 01 04:05:00"

  current_file="$OUTPUT_DIR/Don't worry - 1 - A famous title.mp3"
  check_current_mp3_length "01.04"
  check_current_file_has_xing
  check_current_file_size "1424387"
  check_all_mp3_tags_with_version "2" "Don't worry" "Hack me" "A famous title"\
  "2009" "Other" "12" "1" ""

  current_file="$OUTPUT_DIR/Don't worry - 2 - Yeah, split me !.mp3"
  check_current_mp3_length "01.06"
  check_current_file_has_xing
  check_current_file_size "1499156"
  check_all_mp3_tags_with_version "2" "Don't worry" "Hack me" "Yeah, split me !"\
  "2009" "Other" "12" "2" ""

  current_file="$OUTPUT_DIR/MS - 3 - 7 sins campaign.mp3"
  check_current_mp3_length "01.23"
  check_current_file_has_xing
  check_current_file_size "1989671"
  check_all_mp3_tags_with_version "2" "MS" "Hack me" "7 sins campaign"\
  "2009" "Other" "12" "3" ""

  current_file="$OUTPUT_DIR/Don't worry - 4 - What the hack _.mp3"
  check_current_mp3_length "00.30"
  check_current_file_has_xing
  check_current_file_size "690906"
  check_all_mp3_tags_with_version "2" "Don't worry" "Hack me" "What the hack ?"\
  "2009" "Other" "12" "4" ""

  print_ok
  echo
}

function test_cddb_mode_pretend_and_cue
{
  _create_cddb_file

  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="cddb mode & cue export & pretend"

  expected=" Pretending to split file 'songs/${M_FILE}.mp3' ...
 reading informations from CDDB file songs/test.cddb ...

  Artist: Don't worry
  Album: Hack me
  Tracks: 4

 cddb file processed
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/Don't worry - 1 - A famous title.mp3\" created
   File \"$OUTPUT_DIR/Don't worry - 2 - Yeah, split me !.mp3\" created
   File \"$OUTPUT_DIR/MS - 3 - 7 sins campaign.mp3\" created
   File \"$OUTPUT_DIR/Don't worry - 4 - What the hack _.mp3\" created
 Processed 9380 frames - Sync errors: 0
 file split
 CUE file 'output/output_out.cue' created.
$auto_adjust_warning"
  mp3splt_args="-P -E output/out.cue -d $OUTPUT_DIR -c $CDDB_FILE $MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_number_of_files 1

  check_file_content "output/output_out.cue" "TITLE \"Hack me\"
PERFORMER \"Don't worry\"
FILE \"songs/La_Verue__Today.mp3\" MP3
  TRACK 01 AUDIO
    TITLE \"A famous title\"
    PERFORMER \"Don't worry\"
    REM ALBUM \"Hack me\"
    REM GENRE \"Other\"
    REM DATE \"2009\"
    REM TRACK \"1\"
    REM NAME \"Don't worry - 1 - A famous title\"
    INDEX 01 00:00:00
  TRACK 02 AUDIO
    TITLE \"Yeah, split me !\"
    PERFORMER \"Don't worry\"
    REM ALBUM \"Hack me\"
    REM GENRE \"Other\"
    REM DATE \"2009\"
    REM TRACK \"2\"
    REM NAME \"Don't worry - 2 - Yeah, split me !\"
    INDEX 01 01:04:50
  TRACK 03 AUDIO
    TITLE \"7 sins campaign\"
    PERFORMER \"MS\"
    REM ALBUM \"Hack me\"
    REM GENRE \"Other\"
    REM DATE \"2009\"
    REM TRACK \"3\"
    REM NAME \"MS - 3 - 7 sins campaign\"
    INDEX 01 02:11:26
  TRACK 04 AUDIO
    TITLE \"What the hack ?\"
    PERFORMER \"Don't worry\"
    REM ALBUM \"Hack me\"
    REM GENRE \"Other\"
    REM DATE \"2009\"
    REM TRACK \"4\"
    REM NAME \"Don't worry - 4 - What the hack _\"
    INDEX 01 03:34:30
  TRACK 05 AUDIO
    TITLE \"\"
    PERFORMER \"\"
    INDEX 01 04:05:00"

  print_ok
  echo
}

function test_cddb_mode_quiet
{
  _create_cddb_file

  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="cddb mode & quiet"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 reading informations from CDDB file songs/test.cddb ...

  Artist: Don't worry
  Album: Hack me
  Tracks: 4

 cddb file processed
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/Don't worry - 1 - A famous title.mp3\" created
   File \"$OUTPUT_DIR/Don't worry - 2 - Yeah, split me !.mp3\" created
   File \"$OUTPUT_DIR/MS - 3 - 7 sins campaign.mp3\" created
   File \"$OUTPUT_DIR/Don't worry - 4 - What the hack _.mp3\" created
 Processed 9380 frames - Sync errors: 0
 file split"
  mp3splt_args="-d $OUTPUT_DIR -q -c $CDDB_FILE $MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  print_ok
  echo
}

function test_cddb_mode_and_output_format
{
  _create_cddb_file

  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="cddb mode & output format"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 reading informations from CDDB file songs/test.cddb ...

  Artist: Don't worry
  Album: Hack me
  Tracks: 4

 cddb file processed
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/001-Don't worry--Hack me-A famous title.mp3\" created
   File \"$OUTPUT_DIR/002-Don't worry--Hack me-Yeah, split me !.mp3\" created
   File \"$OUTPUT_DIR/003-Don't worry-MS-Hack me-7 sins campaign.mp3\" created
   File \"$OUTPUT_DIR/004-Don't worry--Hack me-What the hack _.mp3\" created
 Processed 9380 frames - Sync errors: 0
 file split
$auto_adjust_warning"
  mp3splt_args="-d $OUTPUT_DIR -o @n3-@a-@p-@b-@t -c $CDDB_FILE $MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/001-Don't worry--Hack me-A famous title.mp3"
  check_current_mp3_length "01.04"

  current_file="$OUTPUT_DIR/002-Don't worry--Hack me-Yeah, split me !.mp3"
  check_current_mp3_length "01.06"

  current_file="$OUTPUT_DIR/003-Don't worry-MS-Hack me-7 sins campaign.mp3"
  check_current_mp3_length "01.23"

  current_file="$OUTPUT_DIR/004-Don't worry--Hack me-What the hack _.mp3"
  check_current_mp3_length "00.30"

  print_ok
  echo
}

function test_cddb_mode_and_output_format_artist_or_performer
{
  _create_cddb_file

  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="cddb mode & output format artist or performer"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 reading informations from CDDB file songs/test.cddb ...

  Artist: Don't worry
  Album: Hack me
  Tracks: 4

 cddb file processed
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$OUTPUT_DIR/001-Don't worry-A famous title.mp3\" created
   File \"$OUTPUT_DIR/002-Don't worry-Yeah, split me !.mp3\" created
   File \"$OUTPUT_DIR/003-MS-7 sins campaign.mp3\" created
   File \"$OUTPUT_DIR/004-Don't worry-What the hack _.mp3\" created
 Processed 9380 frames - Sync errors: 0
 file split
$auto_adjust_warning"
  mp3splt_args="-d $OUTPUT_DIR -o @n3-@A-@t -c $CDDB_FILE $MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  print_ok
  echo
}

function test_cddb_mode_and_output_format_error
{
  _create_cddb_file

  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="cddb mode & output format error"

  expected=" error: illegal variable '@x' in output format"
  mp3splt_args="-d $OUTPUT_DIR -o @n3-@a-@p-@b-@t-@x -c $CDDB_FILE $MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  print_ok
  echo
}

function test_cddb_mode_and_output_format_and_create_dirs
{
  _create_cddb_file

  remove_output_dir

  M_FILE="La_Verue__Today"

  test_name="cddb mode & output format & create dirs"

  out_dir="$OUTPUT_DIR/Don't worry"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 reading informations from CDDB file songs/test.cddb ...

  Artist: Don't worry
  Album: Hack me
  Tracks: 4

 cddb file processed
 info: file matches the plugin 'mp3 (libmad)'
 info: found Xing or Info header. Switching to frame mode... 
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - FRAME MODE - Total time: 4m.05s
 info: starting normal split
   File \"$out_dir/1/001-Don't worry--Hack me-A famous title.mp3\" created
   File \"$out_dir/2/002-Don't worry--Hack me-Yeah, split me !.mp3\" created
   File \"$out_dir/3/003-Don't worry-MS-Hack me-7 sins campaign.mp3\" created
   File \"$out_dir/4/004-Don't worry--Hack me-What the hack _.mp3\" created
 Processed 9380 frames - Sync errors: 0
 file split
$auto_adjust_warning"
  mp3splt_args="-d $OUTPUT_DIR -o @a/@n/@n3-@a-@p-@b-@t -c $CDDB_FILE $MP3_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$out_dir/1/001-Don't worry--Hack me-A famous title.mp3"
  check_current_mp3_length "01.04"

  current_file="$out_dir/2/002-Don't worry--Hack me-Yeah, split me !.mp3"
  check_current_mp3_length "01.06"

  current_file="$out_dir/3/003-Don't worry-MS-Hack me-7 sins campaign.mp3"
  check_current_mp3_length "01.23"

  current_file="$out_dir/4/004-Don't worry--Hack me-What the hack _.mp3"
  check_current_mp3_length "00.30"

  print_ok
  echo
}

function run_cddb_mode_tests
{
  p_blue " CDDB tests ..."
  echo

  cddb_mode_test_functions=$(declare -F | grep " test_cddb" | awk '{ print $3 }')

  for test_func in $cddb_mode_test_functions;do
    eval $test_func
  done

  p_blue " CDDB tests DONE."
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

