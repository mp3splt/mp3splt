#!/bin/bash

. ./utils.sh || exit 1

function test_misc_version
{
  test_name="version"

  expected="mp3splt 2.2.7 (25/09/09) - using libmp3splt 0.5.8
\tMatteo Trotta <mtrotta AT users.sourceforge.net>
\tAlexandru Munteanu <io_fx AT yahoo.fr>"
  mp3splt_args=" -v" 
  run_check_output "$mp3splt_args" "$expected"

  p_green "OK"
  echo
}

function run_misc_tests
{
  p_blue " MISC tests ..."
  echo

  misc_test_functions=$(declare -F | grep " test_misc" | awk '{ print $3 }')

  for test_func in $misc_test_functions;do
    eval $test_func
  done

  p_blue " MISC tests DONE."
  echo
}

#main
export LANGUAGE="en"
start_date=$(date +%s)

run_misc_tests

end_date=$(date +%s)

p_time_diff_cyan $start_date $end_date "\t"
echo -e '\n'

exit 0

