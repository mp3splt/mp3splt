#!/bin/bash

DIFF_CMD="diff"
VIEW_DIFF_CMD="acoc diff"
USE_GRAPHIC_DIFF=0
GRAPHIC_DIFF_CMD="meld"

MP3SPLT="./mp3splt -T"
MP3_FILE="songs/La_Verue__Today.mp3"

PRINT_MP3SPLT_COMMAND=0

OUTPUT_DIR="output"
ACTUAL_FILE="/tmp/mp3splt_actual.txt"
EXPECTED_FILE="/tmp/mp3splt_expected.txt"
COM_ACTUAL_FILE="/tmp/mp3splt_com_actual.txt"
COM_EXPECTED_FILE="/tmp/mp3splt_com_expected.txt"
TEMP_FILE="/tmp/mp3splt_temp.txt"

test_name=""
current_file=""
current_tags_version=0
command_output=""

