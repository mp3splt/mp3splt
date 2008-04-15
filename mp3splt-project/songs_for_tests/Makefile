MP3_FILE='en_attendant_Mado__21min20_thermosta7__mme_asperge.mp3'
MP3_VBR_FILE='nat_h__demain__juste_un_peu_de_temps.mp3'
OGG_FILE='cartel__prepare_for_glory__whoa_whoa.ogg'

MP3SPLT_PROG='./mp3splt'
OUTPUT_DIR='split_output'

tests: test_mp3 test_ogg

test_mp3:
	$(MP3SPLT_PROG) -o $(OUTPUT_DIR) $(MP3_FILE) 0.2 1.3 2.4 EOF

test_ogg:
	$(MP3SPLT_PROG) -o $(OUTPUT_DIR) $(OGG_FILE) 0.2 1.3 2.4 EOF

