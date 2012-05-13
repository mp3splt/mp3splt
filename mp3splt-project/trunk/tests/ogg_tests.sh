#!/bin/bash

. ./utils.sh || exit 1

ogg_start_date=$(date +%s)

echo
p_green "Starting OGG tests..."
echo
echo

./ogg_normal_tests.sh || exit 1
./ogg_time_tests.sh || exit 1
./ogg_cddb_mode_tests.sh || exit 1
./ogg_cue_mode_tests.sh || exit 1
./ogg_audacity_mode_tests.sh || exit 1
./ogg_silence_mode_tests.sh || exit 1

./ogg_stream_tests.sh || exit 1

ogg_end_date=$(date +%s)

p_time_diff_green $ogg_start_date $ogg_end_date
echo -e '\n'

