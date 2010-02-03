includedir = $(prefix)/include/libmp3splt/

include_HEADERS = ../include/libmp3splt/mp3splt.h

INCLUDES=-DSPLT_PLUGINS_DIR=\"$(libdir)/libmp3splt\" @INCLTDL@

lib_LTLIBRARIES = libmp3splt.la

libmp3splt_la_LDFLAGS = -no-undefined -version-info 0:5:0
libmp3splt_la_LIBADD = @LIBINTL@ -lm

if WIN32
libmp3splt_la_LIBADD += -lltdl -lz -lws2_32 -lintl
else
libmp3splt_la_LIBADD += @LIBLTDL@ 
endif

libmp3splt_la_SOURCES = \
types_func.c ../include/libmp3splt/types_func.h \
splt.c ../include/libmp3splt/splt.h  \
mp3splt.c ../include/libmp3splt/mp3splt.h \
cddb.c ../include/libmp3splt/cddb.h \
checks.c ../include/libmp3splt/checks.h \
utils.c ../include/libmp3splt/utils.h \
plugins.c ../include/libmp3splt/plugins.h \
win32.c ../include/libmp3splt/win32.h \
cue.c ../include/libmp3splt/cue.h \
cddb_cue_common.c ../include/libmp3splt/cddb_cue_common.h \
freedb.c ../include/libmp3splt/freedb.h \
audacity.c ../include/libmp3splt/audacity.h \
splt_array.c ../include/libmp3splt/splt_array.h \
string_utils.c ../include/libmp3splt/string_utils.h \
tags_utils.c ../include/libmp3splt/tags_utils.h \
io.c ../include/libmp3splt/io.h

# Define a C macro LOCALEDIR indicating where catalogs will be installed.
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

