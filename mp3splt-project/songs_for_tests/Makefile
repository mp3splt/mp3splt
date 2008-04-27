MP3_FILE=en_attendant_Mado__21min20_thermosta7__mme_asperge.mp3
MP3_VBR_FILE=nat_h__demain__juste_un_peu_de_temps.mp3
OGG_FILE=cartel__prepare_for_glory__whoa_whoa.ogg
DEWRAP_FILE=mp3wrap_file.mp3

ERROR_MODE_MP3_FILE=all_concat.mp3
ERROR_MODE_FILE=$(ERROR_MODE_MP3_FILE)

MP3SPLT_PROG=./mp3splt

MP3SPLT_FILE?=$(MP3_FILE)
CDDB_MP3SPLT_FILE?=$(CDDB_MP3_FILE)

CDDB_MP3_FILE=alicia_keys__songs_in_a_minor.mp3
CDDB_OGG_FILE=alicia_keys__songs_in_a_minor.ogg

CDDB_FILE=alicia_keys__songs_in_a_minor.cddb
CUE_FILE=alicia_keys__songs_in_a_minor.cue

NORMAL_SPLIT_OUTPUT_DIR=normal_split
AUTO_ADJUST_SPLIT_OUTPUT_DIR=auto_adjust_split
CUSTOM_TAGS_SPLIT_OUTPUT_DIR=custom_tags_split
TIME_SPLIT_OUTPUT_DIR=time_split
SILENCE_SPLIT_OUTPUT_DIR=silence_split
CDDB_SPLIT_OUTPUT_DIR=cddb_split
CUE_SPLIT_OUTPUT_DIR=cue_split
WRAP_SPLIT_OUTPUT_DIR=wrap_split
ERROR_MODE_SPLIT_OUTPUT_DIR=error_split

#if we set debug or not
DEBUG=
#DEBUG="-D"

all: all_tests

all_tests: install_libmp3splt clean test_mp3 test_ogg

install_libmp3splt:
	make -C ../trunk/libmp3splt install

clean:
	rm -f $(NORMAL_SPLIT_OUTPUT_DIR)/*; rm -f $(TIME_SPLIT_OUTPUT_DIR)/*; rm -f $(SILENCE_SPLIT_OUTPUT_DIR)/*
	rm -f $(WRAP_SPLIT_OUTPUT_DIR)/*; rm -f $(ERROR_MODE_SPLIT_OUTPUT_DIR)/*; rm -f $(CDDB_SPLIT_OUTPUT_DIR)/*
	rm -f $(AUTO_ADJUST_SPLIT_OUTPUT_DIR)/*;
	rm -f $(CUE_SPLIT_OUTPUT_DIR)/*

test_mp3:
	export MP3SPLT_FILE=$(MP3_FILE) && export CDDB_MP3SPLT_FILE=$(CDDB_MP3_FILE) && make tests

test_ogg:
	export MP3SPLT_FILE=$(OGG_FILE) && export CDDB_MP3SPLT_FILE=$(CDDB_OGG_FILE) && make tests

tests: normal_split time_split

#-f, -k and -p options not tested : must test them separately
#-d option is tested all over
tests:
	@#normal : also verifies the -m option
	echo && echo "normal split : " && $(MP3SPLT_PROG) -m -q $(DEBUG) -d $(NORMAL_SPLIT_OUTPUT_DIR) $(MP3SPLT_FILE) 0.2 1.3 2.4 EOF
	@#time : -t option
	echo && echo "time split : " && $(MP3SPLT_PROG) -q $(DEBUG) -d $(TIME_SPLIT_OUTPUT_DIR) -t 1.5 $(MP3SPLT_FILE)
	@#silence : -s option
	echo && echo "silence split : " && $(MP3SPLT_PROG) -q $(DEBUG) -s -d $(SILENCE_SPLIT_OUTPUT_DIR) $(CDDB_MP3SPLT_FILE)
	@#cddb : -c option
	echo && echo "cddb split : " && $(MP3SPLT_PROG) -q $(DEBUG) -d $(CDDB_SPLIT_OUTPUT_DIR) -c $(CDDB_FILE) $(CDDB_MP3SPLT_FILE)
	@#cue : -c option
	echo && echo "cue split : " && $(MP3SPLT_PROG) -q $(DEBUG) -d $(CUE_SPLIT_OUTPUT_DIR) -c $(CUE_FILE) $(CDDB_MP3SPLT_FILE)
	@#error split : -e option
	echo && echo "error split : " && $(MP3SPLT_PROG) -q $(DEBUG) -d $(ERROR_MODE_SPLIT_OUTPUT_DIR) -e $(ERROR_MODE_FILE)
	@#dewrap : -w option
	echo && echo "dewrap split : " && $(MP3SPLT_PROG) -q $(DEBUG) -d $(WRAP_SPLIT_OUTPUT_DIR) -w $(DEWRAP_FILE)
	@#list wrap files : -l option
	echo && echo "-l option list wrap : " && $(MP3SPLT_PROG) -q $(DEBUG) -l $(DEWRAP_FILE)
	@#count how many silence points we have : -i option
	echo && echo "-i option count silence : " && $(MP3SPLT_PROG) -q $(DEBUG) -i $(CDDB_MP3SPLT_FILE)
	@#verifies the -a option
	echo && echo "-a option auto adjust : " && $(MP3SPLT_PROG) -a -q $(DEBUG) -d $(AUTO_ADJUST_SPLIT_OUTPUT_DIR) $(MP3SPLT_FILE) 0.2 1.3 2.4 EOF
	@#testing the -g option
	echo && echo "-g option custom tags : " && $(MP3SPLT_PROG) -m -q $(DEBUG) -g "[@t=jimmy,@a=wife_of_jimmy,@b=big show],\
	 [@t=john,@a=wife_of_john,@b=small show],%%[@o,@a=default_artist]"\
	-d $(CUSTOM_TAGS_SPLIT_OUTPUT_DIR) $(MP3SPLT_FILE) 0.2 1.3 2.4 EOF
	@#testing the -o option
	echo && echo "-g option set output filename : " && $(MP3SPLT_PROG) -m -q $(DEBUG) -o "@a_@t_@n"\
	-d $(NORMAL_SPLIT_OUTPUT_DIR) $(MP3SPLT_FILE) 0.2 1.3 2.4 EOF
	@#testing the -g + -o option
	echo && echo "-g + - o options set output tags and filename : " && $(MP3SPLT_PROG) -m -q $(DEBUG) -g "%%[@o,@title=the_same]" -o "@n_@t_@a"\
	-d $(NORMAL_SPLIT_OUTPUT_DIR) $(MP3SPLT_FILE) 0.2 1.3 2.4 EOF
	@#testing the -n option
	echo && echo "-n option set no tags : " && $(MP3SPLT_PROG) -m -q $(DEBUG) -n -o "@a_@b_@t"\
	-d $(NORMAL_SPLIT_OUTPUT_DIR) $(MP3SPLT_FILE) 0.2 1.3 2.4 EOF
	@echo


