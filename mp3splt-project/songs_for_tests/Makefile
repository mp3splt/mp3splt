MP3_FILE=en_attendant_Mado__21min20_thermosta7__mme_asperge.mp3
MP3_VBR_FILE=tanita_good_tradition.mp3
OGG_FILE=cartel__prepare_for_glory__whoa_whoa.ogg
DEWRAP_FILE=mp3wrap_file.mp3

ERROR_MODE_MP3_FILE=all_concat.mp3
ERROR_MODE_FILE=$(ERROR_MODE_MP3_FILE)

VALGRIND=
#VALGRIND=valgrind --leak-check=yes
MP3SPLT_PROG=$(VALGRIND) ./mp3splt

MP3SPLT_FILE?=$(MP3_FILE)
MP3_OR_OGG_STDIN?=m-
MP3_OGG?=mp3
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
#DEBUG=-D

#TODO: PROXY
all: all_tests

#all_tests: install_libmp3splt clean normal_tests stdin_tests stdout_tests
all_tests: install_libmp3splt clean stdin_tests

install_libmp3splt:
	make -C ../trunk/libmp3splt install

clean:
	rm -f $(NORMAL_SPLIT_OUTPUT_DIR)/*; rm -f $(TIME_SPLIT_OUTPUT_DIR)/*; rm -f $(SILENCE_SPLIT_OUTPUT_DIR)/*
	rm -f $(WRAP_SPLIT_OUTPUT_DIR)/*; rm -f $(ERROR_MODE_SPLIT_OUTPUT_DIR)/*; rm -f $(CDDB_SPLIT_OUTPUT_DIR)/*
	rm -f $(AUTO_ADJUST_SPLIT_OUTPUT_DIR)/*;
	rm -f $(CUE_SPLIT_OUTPUT_DIR)/*

normal_tests:
	export MP3SPLT_FILE=$(MP3_FILE) && export CDDB_MP3SPLT_FILE=$(CDDB_MP3_FILE) && make all_normal_tests
	export MP3SPLT_FILE=$(OGG_FILE) && export CDDB_MP3SPLT_FILE=$(CDDB_OGG_FILE) && make all_normal_tests

stdin_tests:
	export MP3SPLT_FILE=$(MP3_FILE) && export CDDB_MP3SPLT_FILE=$(CDDB_MP3_FILE) && export MP3_OR_OGG_STDIN=m- && make all_stdin_tests
	export MP3SPLT_FILE=$(OGG_FILE) && export CDDB_MP3SPLT_FILE=$(CDDB_OGG_FILE) && export MP3_OR_OGG_STDIN=o- && make all_stdin_tests

stdout_tests:
	export MP3SPLT_FILE=$(MP3_FILE) && export CDDB_MP3SPLT_FILE=$(CDDB_MP3_FILE) && export MP3_OGG=mp3 && make all_stdout_tests
	export MP3SPLT_FILE=$(OGG_FILE) && export CDDB_MP3SPLT_FILE=$(CDDB_OGG_FILE) && export MP3_OGG=ogg && make all_stdout_tests

tests: normal_split time_split

all_normal_tests:
	@#normal : also verifies the -m option
	echo && echo "normal split : " && $(MP3SPLT_PROG) -m playlist.m3u -q $(DEBUG) -d $(NORMAL_SPLIT_OUTPUT_DIR) $(MP3SPLT_FILE) 0.2 1.3 2.4 EOF
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
	echo "" echo "-a option auto adjust : " && $(MP3SPLT_PROG) -q -a -p th=-20 -d $(AUTO_ADJUST_SPLIT_OUTPUT_DIR) $(CDDB_MP3SPLT_FILE) 0.0 1.43 5.0 9.0 12.30
	@#testing the -g option
	echo && echo "-g option custom tags : " && $(MP3SPLT_PROG) -q $(DEBUG) -g "[@t=jimmy,@a=wife_of_jimmy,@b=big show], \
[@t=john,@a=wife_of_john,@b=small show],%%[@o,@a=default_artist]" \
-d $(CUSTOM_TAGS_SPLIT_OUTPUT_DIR) $(MP3SPLT_FILE) 0.2 1.3 2.4 EOF
	@#testing the -o option
	echo && echo "-o option set output filename : " && $(MP3SPLT_PROG) -q $(DEBUG) -o "@a_@t_@n" \
-d $(NORMAL_SPLIT_OUTPUT_DIR) $(MP3SPLT_FILE) 0.2 1.3 2.4 EOF
	@#testing the -g + -o option
	echo && echo "-g + - o options set output tags and filename : " && $(MP3SPLT_PROG) -q $(DEBUG) -g "%%[@o,@t=the_same]" -o "@n_@t_@a" \
-d $(NORMAL_SPLIT_OUTPUT_DIR) $(MP3SPLT_FILE) 0.2 1.3 2.4 EOF
	@#testing the -n option
	echo && echo "-n option set no tags : " && $(MP3SPLT_PROG) -q $(DEBUG) -n -o "@a_@n_@b_@t" \
-d $(NORMAL_SPLIT_OUTPUT_DIR) $(MP3SPLT_FILE) 0.2 1.3 2.4 EOF
	@echo

all_stdin_tests:
	@#normal : also verifies the -m option
	echo && echo "normal split : " && cat $(MP3SPLT_FILE) | $(MP3SPLT_PROG) -m playlist.m3u -q $(DEBUG) -d $(NORMAL_SPLIT_OUTPUT_DIR) $(MP3_OR_OGG_STDIN) 0.2 1.3 2.4 EOF
	#@#time : -t option
	#NOT WORKING! BROKEN, BROKEN
	echo && echo "time split : " && cat $(MP3SPLT_FILE) | $(MP3SPLT_PROG) -q $(DEBUG) -d $(TIME_SPLIT_OUTPUT_DIR) -t 1.5 $(MP3_OR_OGG_STDIN) 
	@#silence : -s option
	#-unavailable in old mp3splt
	#echo && echo "silence split : " && cat $(CDDB_MP3SPLT_FILE) | $(MP3SPLT_PROG) -q $(DEBUG) -d $(SILENCE_SPLIT_OUTPUT_DIR) -s $(MP3_OR_OGG_STDIN)
	@#cddb : -c option
	echo && echo "cddb split : " && cat $(CDDB_MP3SPLT_FILE) | $(MP3SPLT_PROG) -q $(DEBUG) -c $(CDDB_FILE) -d $(CDDB_SPLIT_OUTPUT_DIR) $(MP3_OR_OGG_STDIN)
	@#cue : -c option
	echo && echo "cue split : " && cat $(CDDB_MP3SPLT_FILE) | $(MP3SPLT_PROG) -q $(DEBUG) -c $(CUE_FILE) -d $(CUE_SPLIT_OUTPUT_DIR) $(MP3_OR_OGG_STDIN)
	@#error split : -e option 
	#-unavailable in old mp3splt
	#echo && echo "error split : " && cat $(ERROR_MODE_FILE) | $(MP3SPLT_PROG) -q $(DEBUG) -e -d $(ERROR_MODE_SPLIT_OUTPUT_DIR) $(MP3_OR_OGG_STDIN)
	@#dewrap : -w option
	#-unavailable in old mp3splt
	#echo && echo "dewrap split : " && cat $(DEWRAP_FILE) | $(MP3SPLT_PROG) -q $(DEBUG) -w -d $(DEWRAP_FILE) $(MP3_OR_OGG_STDIN)
	@#list wrap files : -l option
	#-unavailable in old mp3splt
	#echo && echo "-l option list wrap : " && cat $(DEWRAP_FILE) | $(MP3SPLT_PROG) -q $(DEBUG) -l $(MP3_OR_OGG_STDIN)
	@#count how many silence points we have : -i option
	#-unavailable in old mp3splt
	#echo && echo "-i option count silence : " && cat $(CDDB_MP3SPLT_FILE) | $(MP3SPLT_PROG) -q $(DEBUG) -i $(MP3_OR_OGG_STDIN)
	@#verifies the -a option
	#-unavailable in old mp3splt
	#echo "" echo "-a option auto adjust : " && cat $(CDDB_MP3SPLT_FILE) | $(MP3SPLT_PROG) -d $(AUTO_ADJUST_SPLIT_OUTPUT_DIR) -q -a -p th=-20 $(MP3_OR_OGG_STDIN) 0.0 1.43 5.0 9.0 12.30
	@#testing the -g option
	echo && echo "-g option custom tags : " && cat $(MP3SPLT_FILE) | $(MP3SPLT_PROG) -q $(DEBUG) -g "[@t=jimmy,@a=wife_of_jimmy,@b=big show], \
[@t=john,@a=wife_of_john,@b=small show],%%[@o,@a=default_artist]" \
-d $(CUSTOM_TAGS_SPLIT_OUTPUT_DIR) $(MP3_OR_OGG_STDIN) 0.2 1.3 2.4 EOF
	@#testing the -o option
	echo && echo "-o option set output filename : " && cat $(MP3SPLT_FILE) | $(MP3SPLT_PROG) -q $(DEBUG) -o "@a_@t_@n" \
-d $(NORMAL_SPLIT_OUTPUT_DIR) $(MP3_OR_OGG_STDIN) 0.2 1.3 2.4 EOF
	@#testing the -g + -o option
	echo && echo "-g + - o options set output tags and filename : " && cat $(MP3SPLT_FILE) | $(MP3SPLT_PROG) -q $(DEBUG) -g "%%[@o,@t=the_same]" -o "@n_@t_@a" \
-d $(NORMAL_SPLIT_OUTPUT_DIR) -o "@n_@t_@a" $(MP3_OR_OGG_STDIN) 0.2 1.3 2.4 EOF
	@#testing the -n option
	echo && echo "-n option set no tags : " && cat $(MP3SPLT_FILE) | $(MP3SPLT_PROG) -q $(DEBUG) -n -o "@a_@n_@b_@t" \
-d $(NORMAL_SPLIT_OUTPUT_DIR) $(MP3_OR_OGG_STDIN) 0.2 1.3 2.4 EOF
	@echo

#-o - must not work with -d
#-i option check
all_stdout_tests:
	@#normal : also verifies the -m option
	echo && echo "normal split : " && $(MP3SPLT_PROG) -m playlist.m3u -q $(DEBUG) -o - $(MP3SPLT_FILE) 0.2 1.3 2.4 EOF > $(NORMAL_SPLIT_OUTPUT_DIR)/normal_stdout.$(MP3_OGG)
	@#time : -t option
	echo && echo "time split : " && $(MP3SPLT_PROG) -q $(DEBUG) -d $(TIME_SPLIT_OUTPUT_DIR) -t 1.5 $(MP3SPLT_FILE) -o - > $(TIME_SPLIT_OUTPUT_DIR)/time_stdout.$(MP3_OGG)
	@#silence : -s option
	echo && echo "silence split : " && $(MP3SPLT_PROG) -q $(DEBUG) -s $(CDDB_MP3SPLT_FILE) -s -o - > $(SILENCE_SPLIT_OUTPUT_DIR)/silence_stdout.$(MP3_OGG)
	@#cddb : -c option
	echo && echo "cddb split : " && $(MP3SPLT_PROG) -q $(DEBUG) -c $(CDDB_FILE) $(CDDB_MP3SPLT_FILE) -o - > $(CDDB_SPLIT_OUTPUT_DIR)/cddb_stdout.$(MP3_OGG)
	@#cue : -c option
	echo && echo "cue split : " && $(MP3SPLT_PROG) -q $(DEBUG) -c $(CUE_FILE) $(CDDB_MP3SPLT_FILE) -o - > $(CUE_SPLIT_OUTPUT_DIR)/cue_stdout.$(MP3_OGG)
	@#error split : -e option
	echo && echo "error split : " && $(MP3SPLT_PROG) -q $(DEBUG) -e $(ERROR_MODE_FILE) -o - > $(ERROR_MODE_SPLIT_OUTPUT_DIR)/error_stdout.$(MP3_OGG)
	@#dewrap : -w option
	#echo && echo "dewrap split : " && $(MP3SPLT_PROG) -q $(DEBUG) -w $(DEWRAP_FILE) -o - > $(WRAP_SPLIT_OUTPUT_DIR)/wrap_stdout.$(MP3_OGG)
	@#list wrap files : -l option
	#-unavailable
	@#count how many silence points we have : -i option
	echo && echo "-i option count silence : " && $(MP3SPLT_PROG) -q $(DEBUG) -i $(CDDB_MP3SPLT_FILE) -o -
	@#verifies the -a option
	echo "" echo "-a option auto adjust : " && $(MP3SPLT_PROG) -q -a -p th=-20 $(CDDB_MP3SPLT_FILE) 0.0 1.43 5.0 9.0 12.30 -o - > $(AUTO_ADJUST_SPLIT_OUTPUT_DIR)/auto_adj_stdout.$(MP3_OGG)
	@#testing the -g option
	echo && echo "-g option custom tags : " && $(MP3SPLT_PROG) -q $(DEBUG) -g "[@t=jimmy,@a=wife_of_jimmy,@b=big show], \
[@t=john,@a=wife_of_john,@b=small show],%%[@o,@a=default_artist]" \
$(MP3SPLT_FILE) 0.2 1.3 2.4 EOF -o - > $(CUSTOM_TAGS_SPLIT_OUTPUT_DIR)/custom_tags_stdout.$(MP3_OGG)
	@#testing the -o option
	#-unavailable
	@#testing the -g + -o option
	#-unavailable
	@#testing the -n option
	echo && echo "-n option set no tags : " && $(MP3SPLT_PROG) -q $(DEBUG) -n \
$(MP3SPLT_FILE) 0.2 1.3 2.4 EOF -o - > $(NORMAL_SPLIT_OUTPUT_DIR)/no_tags_stdout.$(MP3_OGG)
	@echo

