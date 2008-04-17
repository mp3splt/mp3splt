MP3_FILE=en_attendant_Mado__21min20_thermosta7__mme_asperge.mp3
MP3_VBR_FILE=nat_h__demain__juste_un_peu_de_temps.mp3
OGG_FILE=cartel__prepare_for_glory__whoa_whoa.ogg

MP3SPLT_PROG=./mp3splt

MP3SPLT_FILE?=$(MP3_FILE)

NORMAL_SPLIT_OUTPUT_DIR=normal_split
TIME_SPLIT_OUTPUT_DIR=time_split
SILENCE_SPLIT_OUTPUT_DIR=silence_split
WRAP_SPLIT_OUTPUT_DIR=wrap_split
ERROR_MODE_SPLIT_OUTPUT_DIR=error_mode_split
CDDB_SPLIT_OUTPUT_DIR=cddb_split
CUE_SPLIT_OUTPUT_DIR=cue_split

all: all_tests

all_tests: install_libmp3splt test_mp3 test_ogg

install_libmp3splt:
	make -C ../trunk/libmp3splt install

test_mp3:
	export MP3SPLT_FILE=$(MP3_FILE) && make tests

test_ogg:
	export MP3SPLT_FILE=$(OGG_FILE) && make tests

tests: normal_split time_split

tests:
	echo && $(MP3SPLT_PROG) -q -d $(NORMAL_SPLIT_OUTPUT_DIR) $(MP3SPLT_FILE) 0.2 1.3 2.4 EOF
	echo && $(MP3SPLT_PROG) -q -d $(TIME_SPLIT_OUTPUT_DIR) -t 1.5 $(MP3SPLT_FILE)
	@echo

