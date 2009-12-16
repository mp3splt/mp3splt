#!/bin/bash

RUN_INTERNET_TESTS=1

DIFF_CMD="diff"
VIEW_DIFF_CMD="acoc diff"
USE_GRAPHIC_DIFF=1
GRAPHIC_DIFF_CMD="meld"

MP3SPLT="./mp3splt -X"
MP3_FILE="songs/La_Verue__Today.mp3"
SILENCE_MP3_FILE="songs/La_Verue__Today_silence.mp3"
NO_TAGS_MP3_FILE="songs/La_Verue__Today__no_tags.mp3"
CBR_MP3_FILE="songs/Merci_Bonsoir__Je_veux_Only_love.mp3"
WRAPPED_FILE="songs/wrapped.mp3"
SYNCERR_FILE="songs/syncerror.mp3"
CDDB_FILE="songs/test.cddb"
CUE_FILE="songs/test.cue"
AUDACITY_FILE="songs/audacity.txt"

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

auto_adjust_warning="
 +-----------------------------------------------------------------------------+
 |NOTE: When you use cddb/cue, split files might be not very precise due to:|
 |1) Who extracts CD tracks might use \"Remove silence\" option. This means that |
 |   the large mp3 file is shorter than CD Total time. Never use this option.  |
 |2) Who burns CD might add extra pause seconds between tracks.  Never do it.  |
 |3) Encoders might add some padding frames so  that  file is longer than CD.  |
 |4) There are several entries of the same cd on CDDB, find the best for yours.|
 |   Usually you can find the correct splitpoints, so good luck!  |
 +-----------------------------------------------------------------------------+
 | TRY TO ADJUST SPLITS POINT WITH -a OPTION. Read man page for more details!  |
 +-----------------------------------------------------------------------------+
"
