#!/bin/bash

RUN_INTERNET_TESTS=1
RUN_PROXY_TESTS=0
FAIL_FAST=1
CHECK_MP3VAL_ERRORS=1

DIFF_CMD="diff"
VIEW_DIFF_CMD="acoc diff"
USE_GRAPHIC_DIFF=0
GRAPHIC_DIFF_CMD="meld"

MP3SPLT="./mp3splt -X"
SONGS_DIR="songs"

PROXY_HOST="localhost"
PROXY_PORT="3177"

#MP3
MP3_FILE="$SONGS_DIR/La_Verue__Today.mp3"
SILENCE_MP3_FILE="$SONGS_DIR/La_Verue__Today_silence.mp3"
NO_TAGS_MP3_FILE="$SONGS_DIR/La_Verue__Today__no_tags.mp3"
CBR_MP3="Merci_Bonsoir__Je_veux_Only_love.mp3"
CBR_MP3_FILE="$SONGS_DIR/$CBR_MP3"
WRAPPED_FILE="$SONGS_DIR/wrapped.mp3"
SYNCERR_FILE="$SONGS_DIR/syncerror.mp3"

#OGG
OGG_FILE="$SONGS_DIR/Kelly_Allyn__Whiskey_Can.ogg"
SILENCE_OGG_FILE="$SONGS_DIR/Kelly_Allyn__Whiskey_Can_silence.ogg"
MULTIPLE_LOGICAL_STREAMS_OGG_FILE="$SONGS_DIR/Kelly_Allyn__Whiskey_Can_multiple_logical_streams.ogg"
STREAM_OGG_FILE="$SONGS_DIR/Kelly_Allyn__Whiskey_Can__stream.ogg"
STREAM_OGG_SILENCE_FILE="$SONGS_DIR/Kelly_Allyn__Whiskey_Can__stream_silence.ogg"
NO_TAGS_OGG_FILE="$SONGS_DIR/Kelly_Allyn__Whiskey_Can__no_tags.ogg"

#OTHERS
CDDB_FILE="$SONGS_DIR/test.cddb"
CUE_FILE="$SONGS_DIR/test.cue"
AUDACITY_FILE="$SONGS_DIR/audacity.txt"

PRINT_MP3SPLT_COMMAND=0

OUTPUT_DIR="output"
ACTUAL_FILE="/tmp/mp3splt_actual.txt"
EXPECTED_FILE="/tmp/mp3splt_expected.txt"
COM_ACTUAL_FILE="/tmp/mp3splt_com_actual.txt"
COM_EXPECTED_FILE="/tmp/mp3splt_com_expected.txt"
TEMP_FILE="/tmp/mp3splt_temp.txt"
FAILED_TESTS_TEMP_FILE="/tmp/mp3splt_failed_tests.txt"

test_name=""
current_file=""
current_tags_version=0
command_output=""
failed_tests=0

auto_adjust_warning="
 +------------------------------------------------------------------------------+
 | NOTE: When you use cddb/cue, split files might be not very precise due to:   |
 | 1) Who extracts CD tracks might use \"Remove silence\" option. This means that |
 |    the large mp3 file is shorter than CD Total time. Never use this option.  |
 | 2) Who burns CD might add extra pause seconds between tracks.  Never do it.  |
 | 3) Encoders might add some padding frames so  that  file is longer than CD.  |
 | 4) There are several entries of the same cd on CDDB, find the best for yours.|
 |    Usually you can find the correct splitpoints, so good luck!               |
 +------------------------------------------------------------------------------+
 |  TRY TO ADJUST SPLITS POINT WITH -a OPTION. Read man page for more details!  |
 +------------------------------------------------------------------------------+
"
