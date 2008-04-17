MP3_FILE=en_attendant_Mado__21min20_thermosta7__mme_asperge.mp3
MP3_VBR_FILE=nat_h__demain__juste_un_peu_de_temps.mp3
OGG_FILE=cartel__prepare_for_glory__whoa_whoa.ogg

MP3SPLT_PROG=./mp3splt

MP3SPLT_FILE?=$(MP3_FILE)
CDDB_MP3SPLT_FILE?=$(CDDB_MP3_FILE)

CDDB_MP3_FILE=alicia_keys__songs_in_a_minor.mp3
CDDB_OGG_FILE=alicia_keys__songs_in_a_minor.ogg

CDDB_FILE=alicia_keys__songs_in_a_minor.cddb
CUE_FILE=alicia_keys__songs_in_a_minor.cue

NORMAL_SPLIT_OUTPUT_DIR=normal_split
TIME_SPLIT_OUTPUT_DIR=time_split
SILENCE_SPLIT_OUTPUT_DIR=silence_split
CDDB_SPLIT_OUTPUT_DIR=cddb_split
CUE_SPLIT_OUTPUT_DIR=cue_split
WRAP_SPLIT_OUTPUT_DIR=wrap_split
ERROR_MODE_SPLIT_OUTPUT_DIR=error_mode_split

all: all_tests

all_tests: install_libmp3splt clean test_mp3 test_ogg

install_libmp3splt:
	make -C ../trunk/libmp3splt install

clean:
	rm -f $(NORMAL_SPLIT_OUTPUT_DIR)/*; rm -f $(TIME_SPLIT_OUTPUT_DIR)/*; rm -f $(SILENCE_SPLIT_OUTPUT_DIR)/*
	rm -f $(WRAP_SPLIT_OUTPUT_DIR)/*; rm -f $(ERROR_MODE_SPLIT_OUTPUT_DIR)*; rm -f $(CDDB_SPLIT_OUTPUT_DIR)/*
	rm -f $(CUE_SPLIT_OUTPUT_DIR)/*

test_mp3:
	export MP3SPLT_FILE=$(MP3_FILE) && export CDDB_MP3SPLT_FILE=$(CDDB_MP3_FILE) && make tests

test_ogg:
	export MP3SPLT_FILE=$(OGG_FILE) && export CDDB_MP3SPLT_FILE=$(CDDB_OGG_FILE) && make tests

tests: normal_split time_split

tests:
	@#normal
	echo && $(MP3SPLT_PROG) -q -d $(NORMAL_SPLIT_OUTPUT_DIR) $(MP3SPLT_FILE) 0.2 1.3 2.4 EOF
	@#time
	echo && $(MP3SPLT_PROG) -q -d $(TIME_SPLIT_OUTPUT_DIR) -t 1.5 $(MP3SPLT_FILE)
	@#silence
	echo && $(MP3SPLT_PROG) -q -s -d $(SILENCE_SPLIT_OUTPUT_DIR) $(MP3SPLT_FILE)
	@#cddb
	echo && $(MP3SPLT_PROG) -q -d $(CDDB_SPLIT_OUTPUT_DIR) -c $(CDDB_FILE) $(CDDB_MP3SPLT_FILE)
	@#cue
	echo && $(MP3SPLT_PROG) -q -d $(CUE_SPLIT_OUTPUT_DIR) -c $(CUE_FILE) $(CDDB_MP3SPLT_FILE)
	@echo

