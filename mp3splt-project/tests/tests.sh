#!/bin/bash

. ./utils.sh

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

end_date=$(date +%s)

p_time_diff_green $start_date $end_date
echo -e '\n'

