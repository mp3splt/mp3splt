#!/bin/bash

. ./utils.sh || exit 1

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
                     "Whiskey Can" "2007-07-10 15:45:07" "Swing" "1"\
                     "http://www.jamendo.com"
  check_current_file_size "236181"

  current_file="$OUTPUT_DIR/${O_FILE}_01m_00s__01m_10s.ogg" 
  check_current_ogg_length "0m:10.000s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Swing" "2"\
                     "http://www.jamendo.com"
  check_current_file_size "82292"

  current_file="$OUTPUT_DIR/${O_FILE}_01m_10s__01m_36s_20h.ogg" 
  check_current_ogg_length "0m:25.690s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Swing" "3"\
                     "http://www.jamendo.com"
  check_current_file_size "207563"

  p_green "OK"
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
                     "Whiskey Can" "2007-07-10 15:45:07" "Swing" "1"\
                     "http://www.jamendo.com"
  check_current_file_size "168799"

  p_green "OK"
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
                     "Whiskey Can" "2007-07-10 15:45:07" "Swing" "1"\
                     "http://www.jamendo.com"
  check_current_file_size "761798"

  p_green "OK"
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
                     "Whiskey Can" "2007-07-10 15:45:07" "Swing" "1"\
                     "http://www.jamendo.com"
  check_current_file_size "248823"

  current_file="$OUTPUT_DIR/${O_FILE}_00m_30s__01m_00s.ogg"
  check_current_ogg_length "0m:30.000s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Swing" "2"\
                     "http://www.jamendo.com"
  check_current_file_size "236181"

  current_file="$OUTPUT_DIR/${O_FILE}_01m_00s__01m_30s.ogg"
  check_current_ogg_length "0m:30.000s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Swing" "3"\
                     "http://www.jamendo.com"
  check_current_file_size "240790"

  p_green "OK"
  echo
}

function test_stream_silence_mode
{
  rm -f mp3splt.log
  remove_output_dir

  test_name="stream silence mode"

  O_FILE="Kelly_Allyn__Whiskey_Can__stream"

  expected=" Processing file 'songs/${O_FILE}.ogg' ...
 info: file matches the plugin 'ogg vorbis (libvorbis)'
 info: Ogg Vorbis Stream - 44100 - 62 Kb/s - 2 channels - Total time: 1m.36s
 info: starting silence mode split
 Silence split type: Auto mode (Th: -16.0 dB, Off: 0.80, Min: 0.00, Remove: NO)
 warning: unexpected position in ogg vorbis stream - split from 0.0 to EOF to fix.
 Total silence points found: 2. (Selected 3 tracks)
 Writing silence log file 'mp3splt.log' ...
   File \"$OUTPUT_DIR/${O_FILE}_silence_1.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_silence_2.ogg\" created
   File \"$OUTPUT_DIR/${O_FILE}_silence_3.ogg\" created
 silence split ok
 Average silence level: -16.15 dB"
  mp3splt_args="-s -p th=-16 -d $OUTPUT_DIR $STREAM_OGG_FILE"
  run_check_output "$mp3splt_args" "$expected"

  current_file="$OUTPUT_DIR/${O_FILE}_silence_1.ogg"
  check_current_ogg_length "0m:19.079s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Swing" "1"\
                     "http://www.jamendo.com"
  check_current_file_size "157440"

  current_file="$OUTPUT_DIR/${O_FILE}_silence_2.ogg"
  check_current_ogg_length "0m:05.280s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Swing" "2"\
                     "http://www.jamendo.com"
  check_current_file_size "46350"

  current_file="$OUTPUT_DIR/${O_FILE}_silence_3.ogg"
  check_current_ogg_length "1m:11.330s"
  check_all_ogg_tags "Kelly Allyn" "Getting Back From Where I've Been"\
                     "Whiskey Can" "2007-07-10 15:45:07" "Swing" "3"\
                     "http://www.jamendo.com"
  check_current_file_size "562758"

  p_green "OK"
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

end_date=$(date +%s)

p_time_diff_cyan $start_date $end_date "\t"
echo -e '\n'

exit 0

