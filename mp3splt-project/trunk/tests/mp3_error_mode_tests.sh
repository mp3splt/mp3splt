#!/bin/bash

. ./utils.sh || exit 1

F1=La_Verue__Today.mp3
F2=La_Verue__Today__no_tags.mp3
F3=Merci_Bonsoir__Je_veux_Only_love.mp3

function _check_syncerror_split_files_sizes
{
  dir=$1
  file=$2

  current_file="$dir/${file}_1.mp3"
  check_current_file_size "5656852"

  current_file="$dir/${file}_2.mp3"
  check_current_file_size "5610540"

  current_file="$dir/${file}_3.mp3"
  check_current_file_size "3581297"
}

function _concat_files
{
  rm -f songs/syncerror.mp3
  cat songs/${F1} songs/${F2} songs/${F3} > songs/syncerror.mp3
}

function test_syncerror
{
 _concat_files

  remove_output_dir

  test_name="sync error"
  M_FILE="syncerror"

  expected=" Processing file 'songs/syncerror.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: starting error mode split
   File \"$OUTPUT_DIR/syncerror_error_1.mp3\" created
   File \"$OUTPUT_DIR/syncerror_error_2.mp3\" created
   File \"$OUTPUT_DIR/syncerror_error_3.mp3\" created
 error mode ok"
  mp3splt_args=" -d $OUTPUT_DIR -e $SYNCERR_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  _check_syncerror_split_files_sizes $OUTPUT_DIR "syncerror_error"

  print_ok
  echo
}

function test_syncerror_and_pretend
{
 _concat_files

  remove_output_dir

  test_name="sync error & pretend "
  M_FILE="syncerror"

  expected=" Pretending to split file 'songs/syncerror.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: starting error mode split
   File \"$OUTPUT_DIR/syncerror_error_1.mp3\" created
   File \"$OUTPUT_DIR/syncerror_error_2.mp3\" created
   File \"$OUTPUT_DIR/syncerror_error_3.mp3\" created
 error mode ok"
  mp3splt_args=" -P -d $OUTPUT_DIR -e $SYNCERR_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_is_empty

  print_ok
  echo
}

function test_syncerror_with_output_format
{
 _concat_files

  remove_output_dir

  test_name="sync error & output format"
  M_FILE="syncerror"

  expected=" Processing file 'songs/syncerror.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: starting error mode split
   File \"$OUTPUT_DIR/syncerror_1.mp3\" created
   File \"$OUTPUT_DIR/syncerror_2.mp3\" created
   File \"$OUTPUT_DIR/syncerror_3.mp3\" created
 error mode ok"
  mp3splt_args=" -d $OUTPUT_DIR -o @f_@n -e $SYNCERR_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  _check_syncerror_split_files_sizes $OUTPUT_DIR "syncerror"

  print_ok
  echo
}

function test_syncerror_and_create_output_dir
{
 _concat_files

  remove_output_dir

  test_name="sync error & create output dir"
  M_FILE="syncerror"

  expected=" Processing file 'songs/syncerror.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: starting error mode split
   File \"$OUTPUT_DIR/sync/syncerror_error_1.mp3\" created
   File \"$OUTPUT_DIR/sync/syncerror_error_2.mp3\" created
   File \"$OUTPUT_DIR/sync/syncerror_error_3.mp3\" created
 error mode ok"
  mp3splt_args=" -d $OUTPUT_DIR/sync -e $SYNCERR_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  _check_syncerror_split_files_sizes "$OUTPUT_DIR/sync" "syncerror_error"

  print_ok
  echo
}

function test_syncerror_and_output_format_and_create_output_dir
{
 _concat_files

  remove_output_dir

  test_name="sync error & output format & create output dir"
  M_FILE="syncerror"

  expected=" Processing file 'songs/syncerror.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: starting error mode split
   File \"$OUTPUT_DIR/sync/syncerror/syncerror_error_1.mp3\" created
   File \"$OUTPUT_DIR/sync/syncerror/syncerror_error_2.mp3\" created
   File \"$OUTPUT_DIR/sync/syncerror/syncerror_error_3.mp3\" created
 error mode ok"
  mp3splt_args=" -d $OUTPUT_DIR/sync -o @f/@f_error_@n -e $SYNCERR_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  _check_syncerror_split_files_sizes "$OUTPUT_DIR/sync/syncerror" "syncerror_error"

  print_ok
  echo
}

function test_syncerror_with_m3u
{
 _concat_files

  remove_output_dir

  test_name="sync error & m3u"
  M_FILE="syncerror"

  expected=" Processing file 'songs/syncerror.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 M3U file '$OUTPUT_DIR/playlist.m3u' will be created.
 info: starting error mode split
   File \"$OUTPUT_DIR/syncerror_error_1.mp3\" created
   File \"$OUTPUT_DIR/syncerror_error_2.mp3\" created
   File \"$OUTPUT_DIR/syncerror_error_3.mp3\" created
 error mode ok"
  mp3splt_args=" -d $OUTPUT_DIR -m playlist.m3u -e $SYNCERR_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  _check_syncerror_split_files_sizes $OUTPUT_DIR "syncerror_error"

  expected="syncerror_error_1.mp3
syncerror_error_2.mp3
syncerror_error_3.mp3"
  check_file_content "$OUTPUT_DIR/playlist.m3u" "$expected"

  print_ok
  echo
}

function run_syncerror_mode_tests
{
  p_blue " SYNC ERROR mp3 tests ..."
  echo

  syncerror_mode_test_functions=$(declare -F | grep " test_syncerror" | awk '{ print $3 }')

  for test_func in $syncerror_mode_test_functions;do
    eval $test_func
  done

  p_blue " SYNC ERROR mp3 tests DONE."
  echo
}

#main
export LC_ALL="C"
start_date=$(date +%s)

run_syncerror_mode_tests

p_failed_tests

end_date=$(date +%s)

p_time_diff_cyan $start_date $end_date "\t"
echo -e '\n'

exit 0

