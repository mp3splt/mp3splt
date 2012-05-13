#!/bin/bash

. ./utils.sh || exit 1

F1=La_Verue__Today.mp3
F2=La_Verue__Today__no_tags.mp3
F3=Merci_Bonsoir__Je_veux_Only_love.mp3

function _check_dewrapped_files_sizes
{
  dir=$1

  current_file="$dir/$F1"
  check_current_file_size "5656852"

  current_file="$dir/$F2"
  check_current_file_size "5610540"

  current_file="$dir/$F3"
  check_current_file_size "3581297"
}

function _wrap_files
{
  rm -f songs/wrapped.mp3
  mp3wrap songs/wrapped.mp3 songs/${F1} songs/${F2} songs/${F3} &>/dev/null
  mv songs/wrapped_MP3WRAP.mp3 songs/wrapped.mp3
}

function _wrap_files_with_absolute_paths
{
  rm -f songs/wrapped.mp3
  cur_dir=$(pwd)
  mp3wrap songs/wrapped.mp3 $cur_dir/songs/${F1} $cur_dir/songs/${F2} $cur_dir/songs/${F3} &>/dev/null
  mv songs/wrapped_MP3WRAP.mp3 songs/wrapped.mp3
}

function test_wrap_mode_list_files
{
  _wrap_files_with_absolute_paths

  remove_output_dir

  test_name="list wrapped files"
  M_FILE="wrapped"

  cur_dir=$(pwd)
  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 Detected file created with: Mp3Wrap v. 0.5
 Check for file integrity: calculating CRC please wait...  OK
 Total files: 3

$cur_dir/songs/$F1
$cur_dir/songs/$F2
$cur_dir/songs/$F3
"
  mp3splt_args=" -l $WRAPPED_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_is_empty

  print_ok
  echo
}

function test_wrap_mode
{
  _wrap_files

  remove_output_dir

  test_name="wrap mode"
  M_FILE="wrapped"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: starting wrap mode split
 Detected file created with: Mp3Wrap v. 0.5
 Check for file integrity: calculating CRC please wait...  OK
 Total files: 3
   File \"output/$F1\" created
   File \"output/$F2\" created
   File \"output/$F3\" created
 wrap split ok

All files have been split correctly. Visit http://mp3wrap.sourceforge.net!"
  mp3splt_args=" -d $OUTPUT_DIR -w $WRAPPED_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  _check_dewrapped_files_sizes $OUTPUT_DIR

  print_ok
  echo
}

function test_wrap_mode_with_pretend
{
  _wrap_files

  remove_output_dir

  test_name="wrap mode & pretend"
  M_FILE="wrapped"

  expected=" Pretending to split file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: starting wrap mode split
 Detected file created with: Mp3Wrap v. 0.5
 Check for file integrity: calculating CRC please wait...  OK
 Total files: 3
   File \"output/$F1\" created
   File \"output/$F2\" created
   File \"output/$F3\" created
 wrap split ok

All files have been split correctly. Visit http://mp3wrap.sourceforge.net!"
  mp3splt_args=" -P -d $OUTPUT_DIR -w $WRAPPED_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  check_output_directory_is_empty

  print_ok
  echo
}

function test_wrap_mode_with_absolute_paths
{
  _wrap_files_with_absolute_paths

  remove_output_dir

  test_name="wrap mode with absolute paths"
  M_FILE="wrapped"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: starting wrap mode split
 Detected file created with: Mp3Wrap v. 0.5
 Check for file integrity: calculating CRC please wait...  OK
 Total files: 3
   File \"output/$F1\" created
   File \"output/$F2\" created
   File \"output/$F3\" created
 wrap split ok

All files have been split correctly. Visit http://mp3wrap.sourceforge.net!"
  mp3splt_args=" -d $OUTPUT_DIR -w $WRAPPED_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  _check_dewrapped_files_sizes $OUTPUT_DIR

  print_ok
  echo
}

function test_wrap_mode_with_create_output_dirs
{
  _wrap_files_with_absolute_paths

  remove_output_dir

  test_name="wrap mode with create output dirs"
  M_FILE="wrapped"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: starting wrap mode split
 Detected file created with: Mp3Wrap v. 0.5
 Check for file integrity: calculating CRC please wait...  OK
 Total files: 3
   File \"output/wrapped/$F1\" created
   File \"output/wrapped/$F2\" created
   File \"output/wrapped/$F3\" created
 wrap split ok

All files have been split correctly. Visit http://mp3wrap.sourceforge.net!"
  mp3splt_args=" -d $OUTPUT_DIR/wrapped -w $WRAPPED_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  _check_dewrapped_files_sizes "$OUTPUT_DIR/wrapped"

  print_ok
  echo
}

function test_wrap_mode_and_create_m3u
{
  _wrap_files_with_absolute_paths

  remove_output_dir

  test_name="wrap mode with abs paths & create m3u"
  M_FILE="wrapped"

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 M3U file 'output/playlist.m3u' will be created.
 info: starting wrap mode split
 Detected file created with: Mp3Wrap v. 0.5
 Check for file integrity: calculating CRC please wait...  OK
 Total files: 3
   File \"output/$F1\" created
   File \"output/$F2\" created
   File \"output/$F3\" created
 wrap split ok

All files have been split correctly. Visit http://mp3wrap.sourceforge.net!"
  mp3splt_args=" -d $OUTPUT_DIR -m playlist.m3u -w $WRAPPED_FILE" 
  run_check_output "$mp3splt_args" "$expected"

  expected="La_Verue__Today.mp3
La_Verue__Today__no_tags.mp3
Merci_Bonsoir__Je_veux_Only_love.mp3"
  check_file_content "$OUTPUT_DIR/playlist.m3u" "$expected"

  _check_dewrapped_files_sizes "$OUTPUT_DIR"

  print_ok
  echo
}

function run_wrap_mode_tests
{
  p_blue " WRAP mp3 tests ..."
  echo

  wrap_mode_test_functions=$(declare -F | grep " test_wrap_mode" | awk '{ print $3 }')

  for test_func in $wrap_mode_test_functions;do
    eval $test_func
  done

  p_blue " WRAP mp3 tests DONE."
  echo
}

#main
export LC_ALL="C"
start_date=$(date +%s)

run_wrap_mode_tests

p_failed_tests

end_date=$(date +%s)

p_time_diff_cyan $start_date $end_date "\t"
echo -e '\n'

exit 0


