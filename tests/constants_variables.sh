#!/bin/bash

DIFF_CMD="acoc diff"
USE_GRAPHIC_DIFF=0
GRAPHIC_DIFF_CMD="meld"

MP3SPLT="./mp3splt -T"
MP3_FILE="songs/La_Verue__Today.mp3"

OUTPUT_DIR="output"
ACTUAL_FILE="/tmp/mp3splt_actual.txt"
EXPECTED_FILE="/tmp/mp3splt_expected.txt"
TEMP_FILE="/tmp/mp3splt_temp.txt"
NULL_FILE="/dev/null"

expected=""
test_name=""
current_file=""
current_tags_version=0

