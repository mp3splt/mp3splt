#!/bin/bash

. ./constants_variables.sh

exit_status=0

function remove_output_dir
{
  rm -rf ./$OUTPUT_DIR || exit 1
  mkdir -p $OUTPUT_DIR
}

function p_white
{
  echo -n -e "${@}"
}
function p_cyan
{
  echo -n -e "\033[36m${@}\033[0m"
}
function p_blue
{
  echo -n -e "\033[1;34m${@}\033[0m"
}
function p_green
{
  echo -n -e "\033[1;32m${@}\033[0m"
}
function print_ok
{
  if [[ $exit_status -ne 0 ]];then
    p_red "FAILED"
    exit_status=0
    return
  fi

  p_green "OK"
}
function p_red
{
  echo -n -e "\033[1;31m${@}\033[0m"
}
function p_yellow
{
  echo -n -e "\033[33m${@}\033[0m"
}

function p_failed_tests
{
  if [[ $failed_tests -ne 0 ]];then
    p_red "$failed_tests failed tests."
    echo
  else
    p_green " Tests OK."
    echo
  fi

  echo -n "+$failed_tests" >> $FAILED_TESTS_TEMP_FILE
}

function p_time_diff_cyan
{
  start_date=$1
  end_date=$2
  prefix=$3

  date_diff=$(($end_date - $start_date))

  p_cyan "${prefix}Tests took $date_diff seconds."
}

function p_time_diff_green
{
  start_date=$1
  end_date=$2

  date_diff=$(($end_date - $start_date))

  p_green "Tests took $date_diff seconds."
}

function check_file_content
{
  file=$1
  expected=$2
  echo -e "$expected" > $EXPECTED_FILE
  check_files_content $EXPECTED_FILE $file
}

function check_output_directory_is_empty
{
  check_output_directory_number_of_files 0
}

function check_output_directory_number_of_files
{
  expected_num_of_files=$1
  num_of_files=$(ls $OUTPUT_DIR | wc -l)
  echo -e "" > $ACTUAL_FILE
  echo -e "expected $expected_num_of_files files in output directory" > $EXPECTED_FILE
  if [[ $num_of_files -ne $expected_num_of_files ]];then
    echo -e "found $num_of_files files in the output directory" > $ACTUAL_FILE
  else
    echo -e "expected $num_of_files files in output directory" > $ACTUAL_FILE
  fi
  _check_expected_actual_files_equals
}

function check_current_mp3_no_tags
{
  check_current_mp3_no_id3v2_tags

  _run_command "id3 -R -l \"$current_file\"" "id3 command"
  id3v1=$command_output

  for tag in Artist Album Title Year Track Comment;do
    _check_mp3_tags $current_file 1 "$tag" "" "$id3v1"
  done
  _check_mp3_tags $current_file 1 "Genre" "Unknown (255)" "$id3v1"
}

function check_current_mp3_no_id3v2_tags
{
  _run_command "eyeD3 -2 --no-color $current_file " "eyeD3 command" 1

  id3v2=$command_output
  no_id3_line=$(echo "$id3v2" | grep "No ID3 v2.x tag found!" | sed 's/\s\+$//g')
  _check_equal_variables "No ID3 v2.x tag found!" "$no_id3_line"
}

function check_current_ogg_no_tags
{
  _run_command "vorbiscomment -l $current_file" "vorbiscomment command" 1
  _check_equal_variables "" "$command_output"
}

function check_all_mp3_tags_with_version
{
  year=$5
  for tags_v in $1;do
    current_tags_version=$tags_v

    if [[ $tags_v -ne 1 && $tags_v -ne 2 ]];then
      echo "Bad test; tags_version = $tags_v";
      _fail 1
    fi

    new_year=$year
    if [[ $tags_v -eq 2 ]];then
      id_str="id "
      if [[ -z $year ]];then
        new_year="None"
      fi
    else
      id_str=""
    fi

    local genre=""
    if [[ ! -z $6 ]];then
      genre="$6 (${id_str}$7)"
    elif [[ $tags_v -eq 1 ]];then
      genre="Unknown (255)"
    fi

    _check_all_current_mp3_tags "$2" "$3" "$4" "$new_year" "$genre" "$8" "$9"
  done
}

function check_mp3_tags_v2_equal_between_files
{
  original_file=$1
  created_file=$2

  _run_command "eyeD3 -2 --no-color \"$original_file\" |"\
"grep -E -v \"MB ]|Time|ID3 v\" " "eyeD3 command on $original_file"
  original_file_tags=$command_output
 
  _run_command "eyeD3 -2 --no-color \"$created_file\" |"\
"grep -E -v \"MB ]|Time|ID3 v\" " "eyeD3 command on $created_file"
  created_file_tags=$command_output

  _check_equal_variables "$original_file_tags" "$created_file_tags"
}

function check_all_ogg_tags
{
  _run_command "vorbiscomment -l \"$current_file\"" "vorbiscomment command"
  tags=$command_output

  _check_ogg_tags "$current_file" "ARTIST" "$1" "$tags"
  _check_ogg_tags "$current_file" "ALBUM" "$2" "$tags"
  _check_ogg_tags "$current_file" "TITLE" "$3" "$tags"
  _check_ogg_tags "$current_file" "DATE" "$4" "$tags"
  _check_ogg_tags "$current_file" "GENRE" "$5" "$tags"
  _check_ogg_tags "$current_file" "TRACKNUMBER" "$6" "$tags"
  _check_ogg_tags "$current_file" "COMMENT" "$7" "$tags"
}

function check_ogg_tags_equal_between_files
{
  original_file=$1
  created_file=$2

  _run_command "vorbiscomment \"$original_file\" | sort" "vorbiscomment command on $original_file"
  original_file_tags=$command_output
 
  _run_command "vorbiscomment \"$created_file\" | sort" "vorbiscomment command on $created_file"
  created_file_tags=$command_output

  _check_equal_variables "$original_file_tags" "$created_file_tags"
}

function check_current_mp3_length
{
  expected_length=$1

  _run_command "eyeD3 --no-color \"$current_file\"" "eyeD3 command"
  mp3_info=$command_output
  actual_length=$(echo "$mp3_info" | grep "Time: " | awk -F"\t" '{ print $1 }' | sed 's/Time: //g' | sed 's/:/./g')

  expected_value="Length for mp3 $current_file: '$expected_length'"
  actual_value="Length for mp3 $current_file: '$actual_length'"
  _check_equal_variables "$expected_value" "$actual_value"
}

function check_current_ogg_length
{
  expected_length=$1

  _run_command "ogginfo \"$current_file\"" "ogginfo command" 1 1

  info=$command_output
  actual_length=$(echo "$info" | grep "Playback length" | awk -F: "{ print \$2\":\"\$3 }" | sed 's/\s\+//g')

  _check_equal_variables "$expected_length" "$actual_length"
}

function run_check_output
{
  mp3splt_args=$1
  expected=$2

  _run_check_output "$MP3SPLT" "$mp3splt_args" "$expected"
}

function run_custom_check_output
{
  program=$1
  mp3splt_args=$2
  expected=$3

  _run_check_output "$program" "$mp3splt_args" "$expected"
}

function check_current_file_size
{
  expected_file_size=$1

  _run_command "du -b \"$current_file\"" "du command"
  file_size=$command_output
  actual_file_size=$(echo $command_output | awk '{ print $1 }')

  _check_equal_variables "$expected_file_size" "$actual_file_size"
}

function check_current_md5sum
{
  if [[ $MD5SUM_DISABLED == 1 ]];then return; fi

  expected_md5sum=$1

  _run_command "md5sum \"$current_file\"" "md5sum command"
  actual_md5sum=$(echo $command_output | awk '{ print $1 }')

  _check_equal_variables "$expected_md5sum" "$actual_md5sum"
}

function check_current_file_has_xing
{
  _run_command "grep 'Xing' \"$current_file\"" "grep xing command" 0 1

  if [[ $? -ne 0 ]];then
    _check_equal_variables "Expected Xing" "No Xing found for file $current_file"
  fi
}

function check_current_file_has_info
{
  _run_command "grep 'Info' \"$current_file\"" "grep info command" 0 1

  if [[ $? -ne 0 ]];then
    _check_equal_variables "Expected Info" "No Info found for file $current_file"
  fi
}


function check_current_file_has_no_xing
{
  _run_command "grep 'Xing' '$current_file'" "grep xing command" 0 1

  if [[ $? -eq 0 ]];then
    _check_equal_variables "Expected No Xing" "Xing found for file $current_file"
  fi

  _run_command "grep 'Info' '$current_file'" "grep info command" 0 1

  if [[ $? -eq 0 ]];then
    _check_equal_variables "Expected No Info" "Info found for file $current_file"
  fi
}

function check_if_directory_exist
{
  dir=$1
  if [[ ! -d $dir ]];then
    _check_equal_variables "Expected directory $dir to exist" "No such directory"
  fi
}

function check_if_file_exist
{
  file=$1
  if [[ ! -f $file ]];then
    _check_equal_variables "Expected file $file to exist" "No such file"
  fi
}

function check_if_file_does_not_exist
{
  file=$1
  if [[ -f $file ]];then
    _check_equal_variables "Expected file $file to not exist" "File found"
  fi
}


###################
#internal functions

function _print_test_title
{
  p_white "\tTesting '"
  p_yellow $test_name
  p_white "' ... "
}

function _check_expected_actual_files_equals
{
  _diff_files $EXPECTED_FILE $ACTUAL_FILE
}

function _diff_files
{
  expected_file=$1
  actual_file=$2

  diff_result_lines=$($DIFF_CMD $expected_file $actual_file 2>&1 | wc -l)

  if [[ $diff_result_lines -ne 0 ]];then
    if [[ $FAIL_FAST -eq 0 ]]; then
      failed_tests=$(($failed_tests+1))
      _fail 1
      return
    fi

    p_red "FAILED"

    echo
    $VIEW_DIFF_CMD $expected_file $actual_file 2>$TEMP_FILE
    echo

    if [[ $USE_GRAPHIC_DIFF -eq 1 ]];then
      $GRAPHIC_DIFF_CMD $expected_file $actual_file
    fi

    _fail 1
  fi
}

function check_files_content
{
  expected_f=$1
  actual_f=$2

  res=$(_run_command "$DIFF_CMD $expected_f $actual_f" "")
  if [[ $? -ne 0 ]];then
    _diff_files $expected_f $actual_f
  fi
}

function disable_check_mp3val
{
  OLD_CHECK_MP3VAL_ERRORS=$CHECK_MP3VAL_ERRORS
  CHECK_MP3VAL_ERRORS=0
}

function enable_back_mp3val
{
  CHECK_MP3VAL_ERRORS=$OLD_CHECK_MP3VAL_ERRORS
}

function _check_equal_variables
{
  expected_v=$1
  actual_v=$2

  echo -e "$expected_v" > $EXPECTED_FILE
  echo -e "$actual_v" > $ACTUAL_FILE

  check_files_content $EXPECTED_FILE $ACTUAL_FILE
}

function _mp3_get_tag_value
{
  tags=$1
  tag_field=$2
  column=$3
  actual_tag_value=$(echo "$tags" | grep $tag_field | sed 's/\s\+$//g' | awk -F"\t" "{ print \$$column }" | awk -F": " '{ print $2 }')
  echo "$actual_tag_value"
}

function _ogg_get_tag_value
{
  tags=$1
  tag_field=$2
  actual_tag_value=$(echo "$tags" | grep $tag_field | awk -F"=" '{ print $2 }')
  echo "$actual_tag_value"
}

function _check_all_current_mp3_tags
{
  if [[ $current_tags_version -eq 1 ]];then
    _run_command "id3 -R -l \"$current_file\"" "id3 command"
    tags=$command_output
  else  
    _run_command "eyeD3 -2 --no-color \"$current_file\"" "eyeD3 command"
    tags=$command_output
  fi

  _check_mp3_tags "$current_file" $current_tags_version "Artist" "$1" "$tags"
  _check_mp3_tags "$current_file" $current_tags_version "Album" "$2" "$tags"
  _check_mp3_tags "$current_file" $current_tags_version "Title" "$3" "$tags"
  _check_mp3_tags "$current_file" $current_tags_version "Year" "$4" "$tags"
  _check_mp3_tags "$current_file" $current_tags_version "Genre" "$5" "$tags"
  _check_mp3_tags "$current_file" $current_tags_version "Track" "$6" "$tags"
  _check_mp3_tags "$current_file" $current_tags_version "Comment" "$7" "$tags"
}

function _check_mp3_tags
{
  file=$1
  local tags_version=$2
  tags_field=$3
  expected_tag_value=$4
  tags=$5

  expected_value="$tags_field for $file: '$expected_tag_value' (id3v$current_tags_version)"
  actual_tag_value=""

  if [[ $tags_version -eq 1 ]];then
    case "$tags_field" in 
      Artist|Album|Title|Year|Genre|Track|Comment)
      actual_tag_value=$(_mp3_get_tag_value "$tags" "$tags_field" 1)
      ;;
      *)
        p_red "Error: unrecognized tags field '$tags_field' (for id3 tags) " 2>&1
        _fail 1 
      ;;
    esac
  else
    case "$tags_field" in 
      Artist*)
        actual_tag_value=$(_mp3_get_tag_value "$tags" "artist:" 3)
      ;;
      Album*)
        actual_tag_value=$(_mp3_get_tag_value "$tags" "album:" 1)
      ;;
      Title*)
        actual_tag_value=$(_mp3_get_tag_value "$tags" "title:" 1)
      ;;
      Year*)
        actual_tag_value=$(_mp3_get_tag_value "$tags" "year:" 3)
      ;;
      Genre*)
        actual_tag_value=$(_mp3_get_tag_value "$tags" "genre:" 3)
      ;;
      Track)
        actual_tag_value=$(_mp3_get_tag_value "$tags" "track:" 1)
      ;;
      Comment*)
        actual_tag_value=$(echo "$tags" | grep -A 1 "Comment:" | tail -n 1)
      ;;
      *)
        p_red "Error: unrecognized tags field '$tags_field' (for id3v2 tags) " 2>&1
        _fail 1 
      ;;
    esac
  fi

  actual_value="$tags_field for $file: '$actual_tag_value' (id3v$current_tags_version)"

  _check_equal_variables "$expected_value" "$actual_value"
}

function _check_ogg_tags
{
  file=$1
  tags_field=$2
  expected_tag_value=$3
  tags=$4

  expected_value="$tags_field for $file: '$expected_tag_value'"
  actual_tag_value=""

  case "$tags_field" in 
    ARTIST|ALBUM|TITLE|DATE|GENRE|TRACKNUMBER|COMMENT)
    actual_tag_value=$(_ogg_get_tag_value "$tags" "$tags_field")
    ;;
    *)
      p_red "Error: unrecognized tags field '$tags_field' (ogg) " 2>&1
      _fail 1 
    ;;
  esac

  actual_value="$tags_field for $file: '$actual_tag_value'"

  _check_equal_variables "$expected_value" "$actual_value"
}

function _run_command
{
  command=$1
  expected=$2
  get_stderr_in_stdout=$3
  ignore_exit_code=$4

  if [[ $get_stderr_in_stdout -eq 0 ]];then
    result=$(eval $command 2>$TEMP_FILE)
  else
    result=$(eval $command 2>&1)
  fi
  exit_code=$?

  if [[ $ignore_exit_code -ne 1 && $exit_code -ne 0 ]];then
    cat $TEMP_FILE > $COM_ACTUAL_FILE
    if [[ -z $expected ]];then
      echo -n > $COM_EXPECTED_FILE
    else
      echo -e "$expected" > $COM_EXPECTED_FILE
    fi
    _diff_files $COM_EXPECTED_FILE $COM_ACTUAL_FILE
    return 1
  fi

  command_output="$result"

  return $exit_code
}

function _run_check_output
{
  program=$1
  mp3splt_args=$2
  expected=$3

  _print_test_title

  if [[ $PRINT_MP3SPLT_COMMAND -eq 1 ]];then
    echo
    echo "$program $mp3splt_args"
  fi

  echo -e "$expected" > $EXPECTED_FILE

  eval "$program $mp3splt_args" > $ACTUAL_FILE 2>&1

  if [[ ! -z $expected ]];then
    check_files_content $EXPECTED_FILE $ACTUAL_FILE
  fi

  if [[ $CHECK_MP3VAL_ERRORS == 1 ]];then
    if [[ $mp3splt_args == *.mp3* ]];then
      output=$(find $OUTPUT_DIR -name "*.mp3" -exec mp3val -si '{}' \; | grep -v "Analyzing file" | grep -v "^Done!" | grep -v "No supported tags" | grep -E -v "^$")
      output_lines=$(echo -n "$output" | wc -l)
 
      if [[ $output_lines > 0 ]];then
        if [[ $FAIL_FAST -eq 0 ]]; then
          failed_tests=$(($failed_tests+1))
  
          echo
          echo "$output"
  
          _fail 1
          return
        fi
  
        echo
        echo "$output"
  
        p_red "FAILED"
        echo

        _fail 1
      fi
    fi
  fi
}

function _fail
{
  exit_status=$1

  if [[ $FAIL_FAST -eq 1 ]];then
    exit $exit_status
  fi
}


