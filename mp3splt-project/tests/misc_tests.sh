#!/bin/bash

. ./constants_variables.sh
. ./utils.sh

function test_misc_version
{
  rm -rf $OUTPUT_DIR/*

  test_name="version"

  expected="mp3splt 2.2.6b (16/08/09) - using libmp3splt 0.5.7b
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

