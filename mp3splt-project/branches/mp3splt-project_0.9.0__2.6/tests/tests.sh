#!/bin/bash

. ./utils.sh || exit 1

start_date=$(date +%s)

echo
p_green "Starting ALL tests..."
echo
echo

echo -n "0" > $FAILED_TESTS_TEMP_FILE

./misc_tests.sh || exit 1
./mp3_tests.sh || exit 1
./ogg_tests.sh || exit 1

echo >> $FAILED_TESTS_TEMP_FILE
all_failed_tests=$(cat $FAILED_TESTS_TEMP_FILE | bc -l)
if [[ $all_failed_tests -ne 0 ]];then
  p_red "Total $all_failed_tests failed tests."
  echo
else
  p_green "All tests OK."
  echo
fi

end_date=$(date +%s)

p_time_diff_green $start_date $end_date
echo -e '\n'

