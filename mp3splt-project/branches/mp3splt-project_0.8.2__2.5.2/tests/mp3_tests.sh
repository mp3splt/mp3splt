#!/bin/bash

. ./utils.sh || exit 1

mp3_start_date=$(date +%s)

echo
p_green "Starting MP3 tests..."
echo
echo

./mp3_vbr_normal_tests.sh || exit 1
./mp3_vbr_time_tests.sh || exit 1
./mp3_cddb_mode_tests.sh || exit 1
./mp3_cue_mode_tests.sh || exit 1
./mp3_audacity_mode_tests.sh || exit 1
./mp3_silence_mode_tests.sh || exit 1

./mp3_cbr_normal_tests.sh || exit 1
./mp3_wrap_mode_tests.sh || exit 1
./mp3_error_mode_tests.sh || exit 1

if [[ $RUN_INTERNET_TESTS -eq 1 ]];then
  ./mp3_freedb_mode_tests.sh || exit 1
fi

mp3_end_date=$(date +%s)

p_time_diff_green $mp3_start_date $mp3_end_date
echo -e '\n'

