includedir = $(prefix)/include/libmp3splt/

include_HEADERS = ../include/libmp3splt/mp3splt.h ../include/libmp3splt/mp3splt_types.h

INCLUDES=-Wall -Wswitch-enum -std=gnu99 -g

#shared
lib_LTLIBRARIES = libmp3splt.la

libmp3splt_la_SOURCES = \
	../include/libmp3splt/splt.h  \
	splt.c \
	mp3splt.c ../include/libmp3splt/mp3splt.h \
	mp3.c ../include/libmp3splt/mp3.h \
	ogg.c ../include/libmp3splt/ogg.h \
	cddb.c ../include/libmp3splt/cddb.h \
	checks.c ../include/libmp3splt/checks.h \
	utils.c ../include/libmp3splt/utils.h \
	types_func.c ../include/libmp3splt/types_func.h \
	../include/libmp3splt/mp3splt_types.h

