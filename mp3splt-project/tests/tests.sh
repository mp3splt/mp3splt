#!/bin/bash

. ./utils.sh || exit 1

start_date=$(date +%s)

echo
p_green "Starting ALL tests..."
echo
echo

./vbr_normal_tests.sh || exit 1
./cbr_normal_tests.sh || exit 1
./vbr_time_tests.sh || exit 1
./wrap_mode_tests.sh || exit 1
./error_mode_tests.sh || exit 1
./misc_tests.sh || exit 1
./cddb_mode_tests.sh || exit 1
./cue_mode_tests.sh || exit 1
./silence_mode_tests.sh || exit 1
./audacity_mode_tests.sh || exit 1

if [[ $RUN_INTERNET_TESTS -eq 1 ]];then
./freedb_mode_tests.sh || exit 1
fi

end_date=$(date +%s)

p_time_diff_green $start_date $end_date
echo -e '\n'

