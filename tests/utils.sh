#!/bin/bash

#print in different colors
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
function p_red
{
  echo -n -e "\033[1;31m${@}\033[0m"
}
function p_yellow
{
  echo -n -e "\033[33m${@}\033[0m"
}

function print_test_title
{
  p_white "\tRunning '"
  p_yellow $test_name
  p_white "' ... "
}

function check_files_content
{
  expected_f=$1
  actual_f=$2

  $DIFF_CMD $expected_f $actual_f &> $TEMP_FILE
  exit_code=$?

  if [[ $exit_code -ne 0 ]];then
    p_red "FAILED"
    echo
    echo
    if [[ $exit_code -ne 2 ]];then
      $DIFF_CMD $expected_f $actual_f
    else
      cat $TEMP_FILE >> $actual_f
    fi
    echo
    if [[ $USE_GRAPHIC_DIFF -eq 1 ]];then
      $GRAPHIC_DIFF_CMD $expected_f $actual_f
    fi
    exit 1
  fi
}

function check_file_content
{
  file=$1
  echo -e "$expected" > $EXPECTED_FILE
  check_files_content $EXPECTED_FILE $file
}

function check_equal_variables
{
  expected_v=$1
  actual_v=$2

  echo -e "$expected_v" > $EXPECTED_FILE
  echo -e "$actual_v" > $ACTUAL_FILE

  check_files_content $EXPECTED_FILE $ACTUAL_FILE
}

function run_command
{
  command=$1

  result=$($command)
  exit_code=$?

  if [[ $exit_code -ne 0 ]];then
    p_red "FAILED"
    cat $TEMP_FILE > $ACTUAL_FILE
    echo -e $expected > $EXPECTED_FILE
    $DIFF_CMD $EXPECTED_FILE $ACTUAL_FILE
    echo
    if [[ $USE_GRAPHIC_DIFF -eq 1 ]];then
      $GRAPHIC_DIFF_CMD $expected_f $actual_f
    fi
    exit 1
  fi

  echo "$result"
}

function mp3_get_tag_value
{
  tags=$1
  tag_field=$2
  column=$3
  actual_tag_value=$(echo "$tags" | grep $tag_field | sed 's/\s\+$//g' | awk -F"\t" "{ print \$$column }" | awk -F": " '{ print $2 }')
  echo "$actual_tag_value"
}

function check_mp3_tags
{
  file=$1
  tags_version=$2
  tags_field=$3
  expected_tag_value=$4

  expected_value="$tags_field for $file : '$expected_tag_value'"
  actual_tag_value=""

  if [[ $tags_version -eq 1 ]];then
    id3_tags=$(run_command "id3 -R -l $file")

    case "$tags_field" in 
      Artist*)
        actual_tag_value=$(mp3_get_tag_value "$id3_tags" "Artist" 1)
      ;;
      Album*)
        actual_tag_value=$(mp3_get_tag_value "$id3_tags" "Album" 1)
      ;;
      Title*)
        actual_tag_value=$(mp3_get_tag_value "$id3_tags" "Title" 1)
      ;;
      Year*)
        actual_tag_value=$(mp3_get_tag_value "$id3_tags" "Year" 1)
      ;;
      Genre*)
        actual_tag_value=$(mp3_get_tag_value "$id3_tags" "Genre" 1)
      ;;
      Tracknumber*)
        actual_tag_value=$(mp3_get_tag_value "$id3_tags" "Track" 1)
      ;;
      Comment*)
        actual_tag_value=$(mp3_get_tag_value "$id3_tags" "Comment" 1)
      ;;
      *)
        p_red "Error: unrecognized tags field '$tags_field' (for mp3 tags) " 2>&1
        exit 1
      ;;
    esac

  else
    id3v2_tags=$(run_command "eyeD3 -2 --no-color $file")

    case "$tags_field" in 
      Artist*)
        actual_tag_value=$(mp3_get_tag_value "$id3v2_tags" "artist" 3)
      ;;
      Album*)
        actual_tag_value=$(mp3_get_tag_value "$id3v2_tags" "album" 1)
      ;;
      Title*)
        actual_tag_value=$(mp3_get_tag_value "$id3v2_tags" "title" 1)
      ;;
      Year*)
        actual_tag_value=$(mp3_get_tag_value "$id3v2_tags" "year" 3)
      ;;
      Genre*)
        actual_tag_value=$(mp3_get_tag_value "$id3v2_tags" "genre" 3)
      ;;
      Tracknumber*)
        actual_tag_value=$(mp3_get_tag_value "$id3v2_tags" "track" 1)
      ;;
      Comment*)
        actual_tag_value=$(echo "$id3v2_tags" | tail -n 1)
      ;;
      *)
        p_red "Error: unrecognized tags field '$tags_field' (for mp3 tags) " 2>&1
        exit 1
      ;;
    esac

  fi

  actual_value="$tags_field for $file : '$actual_tag_value'"

  check_equal_variables "$expected_value" "$actual_value"
}

function check_current_mp3_tags
{
  check_mp3_tags $current_file $current_tags_version "$1" "$2"
}

function check_all_current_mp3_tags
{
  check_current_mp3_tags "Artist" "$1"
  check_current_mp3_tags "Album" "$2"
  check_current_mp3_tags "Title" "$3"
  check_current_mp3_tags "Year" "$4"
  check_current_mp3_tags "Genre" "$5"
  check_current_mp3_tags "Tracknumber" "$6"
  check_current_mp3_tags "Comment" "$7"
}

function check_mp3_length
{
  file=$1
  expected_length=$2

  mp3_info=$(run_command "eyeD3 --no-color $file")
  actual_length=$(echo "$mp3_info" | grep "Time: " | awk -F"\t" '{ print $1 }' | sed 's/Time: //g' | sed 's/:/./g')

  expected_value="Length for mp3 $file : '$expected_length'"
  actual_value="Length for mp3 $file : '$actual_length'"

  check_equal_variables "$expected_value" "$actual_value"
}

function check_current_mp3_length
{
  check_mp3_length $current_file $1
}

#TODO
function check_ogg_tags
{
  file=$1
  tags_field=$2
  expected_value=$3

}

function check_current_ogg_tags
{
  check_ogg_tags $current_file $1 $2
}

#main function to run a test
function run_check_output
{
  print_test_title

  mp3splt_args=$1

  echo -e "$expected" > $EXPECTED_FILE

  $MP3SPLT $mp3splt_args > $ACTUAL_FILE 2>&1

  check_files_content $EXPECTED_FILE $ACTUAL_FILE
}

#checks the file name & size
function check_file_size
{
  file=$1
  expected_size=$2

  size=$(du -b $file 2>$TEMP_FILE | awk '{ print $1 }')

  if [[ $size -ne $expected_size || $expected_size -eq 0 ]];then
    echo -e "file '$file' size : $expected_size" > $EXPECTED_FILE

    echo -e "file '$file' size : $size" > $ACTUAL_FILE
    cat $TEMP_FILE >> $ACTUAL_FILE

    check_files_content $EXPECTED_FILE $ACTUAL_FILE
  fi
}

function check_current_file_size
{
  check_file_size $current_file $1
}

