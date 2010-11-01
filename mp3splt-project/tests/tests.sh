#!/bin/bash

. ./utils.sh || exit 1

start_date=$(date +%s)

echo
p_green "Starting ALL tests..."
echo
echo

./misc_tests.sh || exit 1
./mp3_tests.sh || exit 1
./ogg_tests.sh || exit 1

end_date=$(date +%s)

p_time_diff_green $start_date $end_date
echo -e '\n'

