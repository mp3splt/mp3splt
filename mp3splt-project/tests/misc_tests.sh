#!/bin/bash

. ./utils.sh || exit 1

function _test_misc_version
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

function test_misc_with_symlink_input_dir
{
  remove_output_dir

  rm -f symlink_dir

  test_name="symlink input dir"

  M_FILE="Merci_Bonsoir__Je_veux_Only_love"

  ln -s $SONGS_DIR symlink_dir

  expected=" Processing file 'symlink_dir/Merci_Bonsoir__Je_veux_Only_love.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - 128 Kb/s - Total time: 3m.43s
 info: starting normal split
   File \"output/${M_FILE}_00m_30s__02m_00s.mp3\" created
   File \"output/${M_FILE}_02m_00s__03m_43s_81h.mp3\" created
 file split"
  mp3splt_args="-d $OUTPUT_DIR symlink_dir/$CBR_MP3 0.30 2.0 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  rm -f symlink_dir

  p_green "OK"
  echo
}

function test_misc_with_symlink_input_file
{
  remove_output_dir

  rm -f $SONGS_DIR/symlink_file

  test_name="symlink input file"

  M_FILE="Merci_Bonsoir__Je_veux_Only_love"

  cd $SONGS_DIR && ln -s $CBR_MP3 symlink_file && cd - &> /dev/null

  expected=" Processing file 'songs/symlink_file' ...
 info: resolving linked filename to 'songs/${M_FILE}.mp3'
 info: file matches the plugin 'mp3 (libmad)'
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - 128 Kb/s - Total time: 3m.43s
 info: starting normal split
   File \"output/${M_FILE}_00m_30s__02m_00s.mp3\" created
   File \"output/${M_FILE}_02m_00s__03m_43s_81h.mp3\" created
 file split"
  mp3splt_args="-d $OUTPUT_DIR $SONGS_DIR/symlink_file 0.30 2.0 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  rm -f $SONGS_DIR/symlink_file

  p_green "OK"
  echo
}

function test_misc_with_symlink_input_dir_and_symlink_input_file
{
  remove_output_dir

  rm -f $SONGS_DIR/symlink_file
  rm -f symlink_dir

  test_name="symlink input dir & symlink input file"

  M_FILE="Merci_Bonsoir__Je_veux_Only_love"

  cd $SONGS_DIR && ln -s $CBR_MP3 symlink_file && cd - &> /dev/null
  ln -s $SONGS_DIR symlink_dir

  expected=" Processing file 'symlink_dir/symlink_file' ...
 info: resolving linked filename to 'symlink_dir/${M_FILE}.mp3'
 info: file matches the plugin 'mp3 (libmad)'
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - 128 Kb/s - Total time: 3m.43s
 info: starting normal split
   File \"output/${M_FILE}_00m_30s__02m_00s.mp3\" created
   File \"output/${M_FILE}_02m_00s__03m_43s_81h.mp3\" created
 file split"
  mp3splt_args="-d $OUTPUT_DIR symlink_dir/symlink_file 0.30 2.0 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  rm -f $SONGS_DIR/symlink_file
  rm -f symlink_dir

  p_green "OK"
  echo
}

function test_misc_with_symlink_output_dir
{
  remove_output_dir

  rm -f symlink_dir

  test_name="symlink output dir"

  M_FILE="Merci_Bonsoir__Je_veux_Only_love"

  ln -s $OUTPUT_DIR symlink_dir

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - 128 Kb/s - Total time: 3m.43s
 info: starting normal split
   File \"symlink_dir/a/${M_FILE}_00m_30s__02m_00s.mp3\" created
   File \"symlink_dir/a/${M_FILE}_02m_00s__03m_43s_81h.mp3\" created
 file split"
  mp3splt_args="-d symlink_dir/a $CBR_MP3_FILE 0.30 2.0 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  rm -f symlink_dir

  p_green "OK"
  echo
}

function test_with_symlink_output_dir_last
{
  remove_output_dir

  rm -f symlink_dir

  test_name="symlink output dir"

  M_FILE="Merci_Bonsoir__Je_veux_Only_love"

  ln -s $OUTPUT_DIR symlink_dir

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - 128 Kb/s - Total time: 3m.43s
 info: starting normal split
   File \"symlink_dir/${M_FILE}_00m_30s__02m_00s.mp3\" created
   File \"symlink_dir/${M_FILE}_02m_00s__03m_43s_81h.mp3\" created
 file split"
  mp3splt_args="-d symlink_dir $CBR_MP3_FILE 0.30 2.0 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  rm -f symlink_dir

  p_green "OK"
  echo
}

function test_misc_with_complex_input_symlink
{
  remove_output_dir

  rm -f symlink_dir1 symlink_dir2
  rm -f symlink_file1 symlink_file2

  test_name="complex input symlink"

  M_FILE="Merci_Bonsoir__Je_veux_Only_love"

  ln -s $SONGS_DIR symlink_dir1
  ln -s symlink_dir1 symlink_dir2
  ln -s symlink_dir2/$CBR_MP3 symlink_file1
  ln -s symlink_file1 symlink_file2

  expected=" Processing file 'symlink_file2' ...
 info: resolving linked filename to 'symlink_dir2/${M_FILE}.mp3'
 info: file matches the plugin 'mp3 (libmad)'
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - 128 Kb/s - Total time: 3m.43s
 info: starting normal split
   File \"output/${M_FILE}_00m_30s__02m_00s.mp3\" created
   File \"output/${M_FILE}_02m_00s__03m_43s_81h.mp3\" created
 file split"
  mp3splt_args="-d $OUTPUT_DIR symlink_file2 0.30 2.0 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  rm -f symlink_dir1 symlink_dir2
  rm -f symlink_file1 symlink_file2

  p_green "OK"
  echo
}

function test_misc_with_loop_symlink_file
{
  remove_output_dir

  rm -f symlink_file

  test_name="symlink file with loop"

  M_FILE="Merci_Bonsoir__Je_veux_Only_love"

  ln -s symlink_file symlink_file

  expected=" Processing file 'symlink_file' ...
 error: inexistent file 'symlink_file': Too many levels of symbolic links"
  mp3splt_args="-d $OUTPUT_DIR symlink_file 0.30 2.0 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  rm -f symlink_file

  p_green "OK"
  echo
}

function test_misc_with_loop_symlink_dir
{
  remove_output_dir

  rm -f symlink_dir1 symlink_dir2

  test_name="symlink dir with loop"

  M_FILE="Merci_Bonsoir__Je_veux_Only_love"

  ln -s symlink_dir1 symlink_dir2
  ln -s symlink_dir2 symlink_dir1

  expected=" Processing file 'songs/${M_FILE}.mp3' ...
 error: cannot create directory 'symlink_dir1'"
  mp3splt_args="-d symlink_dir1 $CBR_MP3_FILE 0.30 2.0 EOF" 
  run_check_output "$mp3splt_args" "$expected"

  rm -f symlink_dir1 symlink_dir2

  p_green "OK"
  echo
}

function test_misc_input_output_same_file
{
  remove_output_dir

  test_name="input and output same file"

  M_FILE="Merci_Bonsoir__Je_veux_Only_love"

  expected=" warning: output format ambiguous (@t or @n missing)
 Processing file 'songs/${M_FILE}.mp3' ...
 info: file matches the plugin 'mp3 (libmad)'
 info: MPEG 1 Layer 3 - 44100 Hz - Joint Stereo - 128 Kb/s - Total time: 3m.43s
 info: starting normal split
 input and output are the same file ('songs/Merci_Bonsoir__Je_veux_Only_love.mp3')"
  mp3splt_args="-d songs -o '$M_FILE' $CBR_MP3_FILE 1.0 2.0" 
  run_check_output "$mp3splt_args" "$expected"

  p_green "OK"
  echo
}

function test_misc_inexistent_file
{
  remove_output_dir

  test_name="inexistent file"

  M_FILE="abcd"

  expected=" Processing file '${M_FILE}.mp3' ...
 error: inexistent file '${M_FILE}.mp3': No such file or directory"
  mp3splt_args="-d songs ${M_FILE}.mp3 1.0 2.0" 
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

