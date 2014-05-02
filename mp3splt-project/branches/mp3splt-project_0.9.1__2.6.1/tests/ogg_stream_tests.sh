#!/bin/bash

. ./utils.sh || exit 1

function _check_silence_last_output_files
{
  current_file="$OUTPUT_DIR/${O_FILE}_silence_2.ogg"
  check_current_ogg_length "0m:45.789s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "2"\
                     "http://www.jamendo.com"
  check_current_file_size "283341"

  current_file="$OUTPUT_DIR/${O_FILE}_silence_3.ogg"
  check_current_ogg_length "0m:35.920s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "3"\
                     "http://www.jamendo.com"
  check_current_file_size "255064"

  current_file="$OUTPUT_DIR/${O_FILE}_silence_4.ogg"
  check_current_ogg_length "0m:43.222s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "4"\
                     "http://www.jamendo.com"
  check_current_file_size "299078"
}

function test_stream_with_tags
{
  remove_output_dir

  test_name="basic stream split"

  O_FILE="Kelly_Allyn__Whiskey_Can__stream"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 62 Kb/s - 2 channels - Total time: 1m.36s
 info: starting normal split
 warning: unexpected position in ogg vorbis stream - split from 0.0 to EOF to fix.
   File \"$OUTPUT_DIR/${O_FILE}_00m_30s__01m_00s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_01m_00s__01m_10s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_01m_10s__01m_36s_20h.ogg\" created
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR $STREAM_OGG_FILE 0.30 1.0 1.10 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${O_FILE}_00m_30s__01m_00s.ogg"
  check_current_ogg_length "0m:30.000s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "1"\
                     "http://www.jamendo.com"
  check_current_file_size "236794"

  current_file="$OUTPUT_DIR/${O_FILE}_01m_00s__01m_10s.ogg" 
  check_current_ogg_length "0m:10.000s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "2"\
                     "http://www.jamendo.com"
  check_current_file_size "82905"

  current_file="$OUTPUT_DIR/${O_FILE}_01m_10s__01m_36s_20h.ogg" 
  check_current_ogg_length "0m:25.690s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "3"\
                     "http://www.jamendo.com"
  check_current_file_size "208176"

  print_ok
  echo
}

function test_stream_starting_from_0
{
  remove_output_dir

  test_name="stream split starting from 0"

  O_FILE="Kelly_Allyn__Whiskey_Can__stream"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 62 Kb/s - 2 channels - Total time: 1m.36s
 info: starting normal split
 warning: unexpected position in ogg vorbis stream - split from 0.0 to EOF to fix.
   File \"$OUTPUT_DIR/${O_FILE}_00m_00s__00m_20s.ogg\" created
 file split"
  mp3splt_args="-d $OUTPUT_DIR $STREAM_OGG_FILE 0.0 0.20"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${O_FILE}_00m_00s__00m_20s.ogg"
  check_current_ogg_length "0m:20.530s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "1"\
                     "http://www.jamendo.com"
  check_current_file_size "169412"

  print_ok
  echo
}

function test_stream_from_0_to_EOF
{
  remove_output_dir

  test_name="stream split from 0 to EOF"

  O_FILE="Kelly_Allyn__Whiskey_Can__stream"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 62 Kb/s - 2 channels - Total time: 1m.36s
 info: starting normal split
 warning: unexpected position in ogg vorbis stream - split from 0.0 to EOF to fix.
   File \"$OUTPUT_DIR/${O_FILE}_00m_00s__01m_36s_20h.ogg\" created
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR $STREAM_OGG_FILE 0.0 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${O_FILE}_00m_00s__01m_36s_20h.ogg"
  check_current_ogg_length "1m:36.206s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "1"\
                     "http://www.jamendo.com"
  check_current_file_size "762411"

  print_ok
  echo
}

function test_stream_time_mode
{
  remove_output_dir

  test_name="stream split time mode"

  O_FILE="Kelly_Allyn__Whiskey_Can__stream"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 62 Kb/s - 2 channels - Total time: 1m.36s
 info: starting time mode split
 warning: unexpected position in ogg vorbis stream - split from 0.0 to EOF to fix.
   File \"$OUTPUT_DIR/${O_FILE}_00m_00s__00m_30s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_00m_30s__01m_00s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_01m_00s__01m_30s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_01m_30s__01m_36s_20h.ogg\" created
 time split ok"
  mp3splt_args="-d $OUTPUT_DIR $STREAM_OGG_FILE -t 0.30"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${O_FILE}_00m_00s__00m_30s.ogg"
  check_current_ogg_length "0m:30.515s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "1"\
                     "http://www.jamendo.com"
  check_current_file_size "249436"

  current_file="$OUTPUT_DIR/${O_FILE}_00m_30s__01m_00s.ogg"
  check_current_ogg_length "0m:30.000s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "2"\
                     "http://www.jamendo.com"
  check_current_file_size "236794"

  current_file="$OUTPUT_DIR/${O_FILE}_01m_00s__01m_30s.ogg"
  check_current_ogg_length "0m:30.000s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "3"\
                     "http://www.jamendo.com"
  check_current_file_size "241403"

  print_ok
  echo
}

function test_stream_silence_mode
{
  rm -f mp3splt.log
  remove_output_dir

  test_name="stream silence mode"

  O_FILE="Kelly_Allyn__Whiskey_Can__stream_silence"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 62 Kb/s - 2 channels - Total time: 2m.29s
 info: starting silence mode split
 Silence split type: Auto mode (Th: -16.0 dB, Off: 0.80, Min: 0.00, Remove: NO, Min track: 0.00, Shots: 25)
 warning: unexpected position in ogg vorbis stream - split from 0.0 to EOF to fix.

 Total silence points found: 3. (Selected 4 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${O_FILE}_silence_1.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_silence_2.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_silence_3.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_silence_4.ogg\" created
 silence split ok
 Average silence level: -23.65 dB"
  mp3splt_args="-s -p th=-16 -d $OUTPUT_DIR $STREAM_OGG_SILENCE_FILE"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${O_FILE}_silence_1.ogg"
  check_current_ogg_length "0m:24.339s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "1"\
                     "http://www.jamendo.com"
  check_current_file_size "130885"

  _check_silence_last_output_files

  print_ok
  echo

  test_name="stream silence mode - check reading from mp3splt.log"

  remove_output_dir

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 62 Kb/s - 2 channels - Total time: 2m.29s
 info: starting silence mode split
 Silence split type: Auto mode (Th: -16.0 dB, Off: 0.80, Min: 0.00, Remove: NO, Min track: 0.00, Shots: 25)
 Found silence log file 'mp3splt.log' ! Reading silence points from file to save time ;)
 Total silence points found: 3. (Selected 4 tracks)
 warning: unexpected position in ogg vorbis stream - split from 0.0 to EOF to fix.
   File \"$OUTPUT_DIR/${O_FILE}_silence_1.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_silence_2.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_silence_3.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_silence_4.ogg\" created
 silence split ok"
  mp3splt_args="-s -p th=-16 -d $OUTPUT_DIR $STREAM_OGG_SILENCE_FILE"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${O_FILE}_silence_1.ogg"
  check_current_ogg_length "0m:24.839s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "1"\
                     "http://www.jamendo.com"
  check_current_file_size "134985"

  _check_silence_last_output_files

  print_ok
  echo
}

function test_stream_auto_adjust_silence
{
  rm -f mp3splt.log
  remove_output_dir

  test_name="stream auto adjust silence mode"

  O_FILE="Kelly_Allyn__Whiskey_Can__stream_silence"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 62 Kb/s - 2 channels - Total time: 2m.29s
 Working with SILENCE AUTO-ADJUST (Threshold: -48.0 dB Gap: 30 sec Offset: 0.80 Min: 0.00 sec)
 info: starting normal split
 warning: unexpected position in ogg vorbis stream - split from 0.0 to EOF to fix.
   File \"$OUTPUT_DIR/${O_FILE}_00m_00s__00m_07s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_00m_07s__01m_00s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_01m_00s__02m_00s.ogg\" created
 file split (EOF)"
  mp3splt_args="-a -d $OUTPUT_DIR $STREAM_OGG_SILENCE_FILE 0.0 0.07 1.0 2.0"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${O_FILE}_00m_00s__00m_07s.ogg"
  check_current_ogg_length "0m:24.328s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "1"\
                     "http://www.jamendo.com"
  check_current_file_size "134941"

  current_file="$OUTPUT_DIR/${O_FILE}_00m_07s__01m_00s.ogg"
  check_current_ogg_length "1m:20.064s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "2"\
                     "http://www.jamendo.com"
  check_current_file_size "517980"

  current_file="$OUTPUT_DIR/${O_FILE}_01m_00s__02m_00s.ogg"
  check_current_ogg_length "0m:45.372s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "3"\
                     "http://www.jamendo.com"
  check_current_file_size "314742"

  print_ok
  echo
}

function test_stream_multiple_logical_streams
{
  remove_output_dir

  test_name="multiple logical streams split"

  O_FILE="Kelly_Allyn__Whiskey_Can_multiple_logical_streams"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 62 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/${O_FILE}_01m_00s__02m_00s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_02m_00s__03m_00s.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_03m_00s__03m_04s_37h.ogg\" created
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR $MULTIPLE_LOGICAL_STREAMS_OGG_FILE 1.0 2.0 3.0 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${O_FILE}_01m_00s__02m_00s.ogg"
  check_current_ogg_length \
"0m:01.621s
0m:30.813s
0m:27.564s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "1"\
                     "http://www.jamendo.com"
  check_current_file_size "501453"

  current_file="$OUTPUT_DIR/${O_FILE}_02m_00s__03m_00s.ogg" 
  check_current_ogg_length \
"0m:03.250s
0m:30.816s
0m:25.933s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "2"\
                     "http://www.jamendo.com"
  check_current_file_size "482670"

  current_file="$OUTPUT_DIR/${O_FILE}_03m_00s__03m_04s_37h.ogg" 
  check_current_ogg_length "0m:04.375s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "3"\
                     "http://www.jamendo.com"
  check_current_file_size "11035"

  print_ok
  echo
}

function test_stream_multiple_logical_streams_from_0_to_EOF
{
  remove_output_dir

  test_name="multiple logical streams split from 0 to EOF"

  O_FILE="Kelly_Allyn__Whiskey_Can_multiple_logical_streams"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 62 Kb/s - 2 channels - Total time: 3m.04s
 info: starting normal split
   File \"$OUTPUT_DIR/all_Whiskey Can.ogg\" created
 file split (EOF)"
  mp3splt_args="-d $OUTPUT_DIR -o all_@t $MULTIPLE_LOGICAL_STREAMS_OGG_FILE 0.0 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  original_file_ogg_lengths=\
"0m:30.809s
0m:30.811s
0m:30.813s
0m:30.815s
0m:30.816s
0m:30.309s"

  current_file="$MULTIPLE_LOGICAL_STREAMS_OGG_FILE"
  check_current_ogg_length "$original_file_ogg_lengths"

  current_file="$OUTPUT_DIR/all_Whiskey Can.ogg"
  check_current_ogg_length "$original_file_ogg_lengths"

  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Southern Rock" "1"\
                     "http://www.jamendo.com"
  check_current_file_size "1480411"

  print_ok
  echo
}

function run_stream_tests
{
  p_blue " STREAM ogg tests ..."
  echo

  stream_test_functions=$(declare -F | grep " test_stream_" | awk '{ print $3 }')

  for test_func in $stream_test_functions;do
    eval $test_func
  done

  p_blue " STREAM ogg tests DONE."
  echo
}

#main
export LC_ALL="C"
start_date=$(date +%s)

run_stream_tests

p_failed_tests

end_date=$(date +%s)

p_time_diff_cyan $start_date $end_date "\t"
echo -e '\n'

exit 0

