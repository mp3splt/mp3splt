includedir = $(prefix)/include/libmp3splt/

include_HEADERS = mp3splt.h mp3splt_types.h

INCLUDES=-Wall -Wswitch-enum -g -std=gnu99

#shared
lib_LTLIBRARIES = libmp3splt.la

libmp3splt_la_SOURCES = \
	splt.h  \
	splt.c \
	mp3splt.c mp3splt.h \
	mp3.c mp3.h \
	ogg.c ogg.h \
	cddb.c cddb.h \
	checks.c checks.h \
	utils.c utils.h \
	types_func.c types_func.h \
	mp3splt_types.h

