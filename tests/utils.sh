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

#TODO
function check_mp3_tags
{
  file=$1
  tags_version=$2
  tags_field=$3
  expected_value=$4

}

function check_current_mp3_tags
{
  check_mp3_tags $current_file $current_tags_version $1 $2
}

#TODO
function check_ogg_tags
{
  file=$1
  tags_field=$2
  expected_value=$3

}

function check_ogg_current_tags
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

