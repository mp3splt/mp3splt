#!/bin/bash

. ./utils.sh || exit 1

#normal mode functional tests

function test_normal_no_tags
{
  remove_output_dir

  test_name="TODO no tags"

#
#  O_FILE="test"
#
#  expected=" Processing file 'songs/${O_FILE}.ogg' ...
# info: file matches the plugin 'ogg vorbis (libvorbis)'
# info: Ogg Vorbis Stream - 44100 - 109 Kb/s - 2 channels - Total time: 63m.12s
# info: starting normal split
#   File \"$OUTPUT_DIR/${O_FILE}_01m_00s__02m_00s_20h.ogg\" created
#   File \"$OUTPUT_DIR/${O_FILE}_02m_00s_20h__03m_05s.ogg\" created
#   File \"$OUTPUT_DIR/${O_FILE}_03m_05s__63m_12s_33h.ogg\" created
# file split (EOF)"
#  mp3splt_args="$tags_option -n -d $OUTPUT_DIR $OGG_FILE 1.0 2.0.2 3.5 EOF" 
#  run_check_output "$mp3splt_args" "$expected"
#
#  current_file="$OUTPUT_DIR/${O_FILE}_01m_00s__02m_00s_20h.ogg" 
#  check_current_ogg_length "1m:00.200s"
#  check_current_ogg_no_tags
#  #check_current_file_size "835097"
#
#  current_file="$OUTPUT_DIR/${O_FILE}_02m_00s_20h__03m_05s.ogg" 
#  check_current_ogg_length "1m:04.799s"
#  check_current_ogg_no_tags
#  #check_current_file_size "1521748"
#
#  current_file="$OUTPUT_DIR/${O_FILE}_03m_05s__63m_12s_33h.ogg" 
#  check_current_ogg_length "60m:07.333s"
#  check_current_ogg_no_tags
#  #check_current_file_size "1399171"
#
  p_green "OK"
  echo
}

function test_normal_with_tags
{
  remove_output_dir

  test_name="TODO with tags"

#  O_FILE="test"
#
#  expected=" Processing file 'songs/${O_FILE}.ogg' ...
# info: file matches the plugin 'ogg vorbis (libvorbis)'
# info: Ogg Vorbis Stream - 44100 - 109 Kb/s - 2 channels - Total time: 63m.12s
# info: starting normal split
#   File \"$OUTPUT_DIR/${O_FILE}_01m_00s__02m_00s_20h.ogg\" created
#   File \"$OUTPUT_DIR/${O_FILE}_02m_00s_20h__03m_05s.ogg\" created
#   File \"$OUTPUT_DIR/${O_FILE}_03m_05s__63m_12s_33h.ogg\" created
# file split (EOF)"
#  mp3splt_args="$tags_option -d $OUTPUT_DIR $OGG_FILE 1.0 2.0.2 3.5 EOF" 
#  run_check_output "$mp3splt_args" "$expected"
#
#  current_file="$OUTPUT_DIR/${O_FILE}_01m_00s__02m_00s_20h.ogg" 
#  check_current_ogg_length "1m:00.200s"
#  #TODO: bug genre ?
#  check_all_ogg_tags "Alicia Keys" "Songs in A Minor" "mytitle"\
#  "2006" "Blues" "1" "this is a comment"
#  #check_current_file_size "835097"
#
#  current_file="$OUTPUT_DIR/${O_FILE}_02m_00s_20h__03m_05s.ogg" 
#  check_current_ogg_length "1m:04.799s"
#  #check_current_file_size "1521748"
#
#  current_file="$OUTPUT_DIR/${O_FILE}_03m_05s__63m_12s_33h.ogg" 
#  check_current_ogg_length "60m:07.333s"
#  #check_current_file_size "1399171"
#
  p_green "OK"
  echo
}

function test_normal_pretend
{
  remove_output_dir
  test_name="TODO pretend"

#TODO
  p_green "OK"
  echo
}

function test_normal_cue_export
{
  remove_output_dir
  test_name="TODO cue export"


#TODO
  p_green "OK"
  echo
}

function test_normal_pretend_and_cue_export
{
  remove_output_dir
  test_name="pretend and cue export"

#TODO
  p_green "OK"
  echo
}

function test_normal_overlap_and_cue_export
{
  remove_output_dir
  test_name="overlap and cue export"

#TODO
  p_green "OK"
  echo
}

function test_normal_pretend_and_m3u
{
  remove_output_dir
  test_name="pretend and m3u"

#TODO
  p_green "OK"
  echo
}

function test_normal_no_input_tags
{
  remove_output_dir
  test_name="no input tags"

#TODO
  p_green "OK"
  echo
}

function test_normal_m3u
{
  remove_output_dir
  test_name="m3u"

#TODO
  p_green "OK"
  echo
}

function test_normal_create_directories
{
  remove_output_dir
  test_name="create directories"

#TODO
  p_green "OK"
  echo
}

function test_normal_custom_tags
{
  remove_output_dir
  test_name="custom tags"

#TODO
  p_green "OK"
  echo
}

function test_normal_custom_tags_and_cue_export
{
  remove_output_dir
  test_name="custom tags and cue export"

#TODO
  p_green "OK"
  echo
}


function test_normal_custom_tags_and_input_no_tags
{
  remove_output_dir
  test_name="custom tags and input no tags"

#TODO
  p_green "OK"
  echo
}

function test_normal_custom_tags_multiple_percent
{
  remove_output_dir
  test_name="custom tags and multiple percent"

#TODO
  p_green "OK"
  echo
}

function test_normal_overlap_split
{
  remove_output_dir
  test_name="overlap"

#TODO
  p_green "OK"
  echo
}

function test_normal_stdin
{
  remove_output_dir
  test_name="stdin"

#TODO
  p_green "OK"
  echo
}


function test_normal_stdin_no_input_tags
{
  remove_output_dir
  test_name="stdin and no input tags"

#TODO
  p_green "OK"
  echo

}

function test_normal_stdin_and_tags
{
  remove_output_dir
  test_name="stdin and tags"

#TODO
  p_green "OK"
  echo
}

function test_normal_output_fnames_and_custom_tags
{
  remove_output_dir
#TODO
  p_green "OK"
  echo
}

function test_normal_output_fname
{
  remove_output_dir
#TODO
  p_green "OK"
  echo
}

function test_normal_output_fnames_and_dirs
{
  remove_output_dir
#TODO
  p_green "OK"
  echo
}

function test_normal_output_fnames_and_custom_tags_dirs
{
  remove_output_dir
#TODO
  p_green "OK"
  echo
}

function test_normal_stdout
{
  remove_output_dir
#TODO
  p_green "OK"
  echo
}

function test_normal_stdout_multiple_splitpoints
{
  remove_output_dir
#TODO
  p_green "OK"
  echo
}

function test_normal_custom_tags_with_replace_tags_in_tags
{
  remove_output_dir
#TODO
  p_green "OK"
  echo
}

function test_normal_split_in_equal_parts
{
  remove_output_dir
#TODO
  p_green "OK"
  echo
}

function run_normal_tests
{
  p_blue " NORMAL ogg tests ..."
  echo

  normal_test_functions=$(declare -F | grep " test_normal_" | awk '{ print $3 }')

  for test_func in $normal_test_functions;do
    eval $test_func
  done

  p_blue " NORMAL ogg tests DONE."
  echo
}

#main
export LANGUAGE="en"
start_date=$(date +%s)

run_normal_tests

end_date=$(date +%s)

p_time_diff_cyan $start_date $end_date "\t"
echo -e '\n'

exit 0

