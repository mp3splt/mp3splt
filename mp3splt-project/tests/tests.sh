#!/bin/bash

echo
echo -n "Main start date: "
date "+%D %H:%M:%S"

./vbr_normal_tests.sh || exit 1
./cbr_normal_tests.sh || exit 1
./vbr_time_tests.sh || exit 1
./wrap_mode_tests.sh || exit 1

echo -n "Main end date: "
date "+%D %H:%M:%S"
echo

