#!/bin/bash

#mp3splt command
M="./mp3splt -q"
#song directory
SD="songs"
#main MP3 song
MP3S="$SD/La_Verue__Today.mp3"

#print cyan color
function p_cyan
{
  echo -e "\033[36m${@}\033[0m"
}
#print blue color
function p_blue
{
  echo -e "\033[1;34m${@}\033[0m"
}
#print yellow color
function p_yellow
{
  echo -e "\033[33m${@}\033[0m"
}

function run
{
  test_name=$1
  command=$2

  echo
  p_yellow "Running test '"$test_name"' ..."
  echo

  $command

  echo
  p_yellow "Finished running test '"$test_name"'."
  echo
}

run "joke" "$M $MP3S 1.0 2.0"

exit 0

