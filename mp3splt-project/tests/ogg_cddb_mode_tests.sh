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
  check_current_file_size "1747388"
  check_all_ogg_tags "Don't worry" "Hack me" "A famous title" "2009" "Other" "1" ""

  current_file="$OUTPUT_DIR/Don't worry - 2 - Yeah, split me !.ogg"
  check_current_ogg_length "1m:06.670s"
  check_current_file_size "1784034"
  check_all_ogg_tags "Don't worry" "Hack me" "Yeah, split me !" "2009" "Other" "2" ""

  current_file="$OUTPUT_DIR/MS - 3 - 7 sins campaign.ogg"
  check_current_ogg_length "0m:53.513s"
  check_current_file_size "1296873"
  check_all_ogg_tags "MS" "Hack me" "7 sins campaign" "2009" "Other" "3" ""

  p_green "OK"
  echo
}

#TODO
#function test_cddb_mode_and_pretend

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
export LANGUAGE="en"
start_date=$(date +%s)

run_cddb_mode_tests

end_date=$(date +%s)

p_time_diff_cyan $start_date $end_date "\t"
echo -e '\n'

exit 0

