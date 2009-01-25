includedir = $(prefix)/include/libmp3splt/

include_HEADERS = ../include/libmp3splt/mp3splt.h

INCLUDES=-DSPLT_PLUGINS_DIR=\"$(libdir)/libmp3splt\" @INCLTDL@

#shared
lib_LTLIBRARIES = libmp3splt.la

libmp3splt_la_LDFLAGS = -no-undefined -version-info 0:5:0
libmp3splt_la_LIBADD = @LIBLTDL@ -lm

if WIN32
libmp3splt_la_LIBADD += -lz -lws2_32
endif

libmp3splt_la_SOURCES = \
types_func.c ../include/libmp3splt/types_func.h \
splt.c ../include/libmp3splt/splt.h  \
mp3splt.c ../include/libmp3splt/mp3splt.h \
cddb.c ../include/libmp3splt/cddb.h \
checks.c ../include/libmp3splt/checks.h \
utils.c ../include/libmp3splt/utils.h \
plugins.c ../include/libmp3splt/plugins.h

