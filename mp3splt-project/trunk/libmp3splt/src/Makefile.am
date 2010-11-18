includedir = $(prefix)/include/libmp3splt/

include_HEADERS = $(top_srcdir)/include/libmp3splt/mp3splt.h

INCLUDES=-DSPLT_PLUGINS_DIR=\"$(libdir)/libmp3splt\" @INCLTDL@

lib_LTLIBRARIES = libmp3splt.la

libmp3splt_la_LDFLAGS = -no-undefined -version-info 0:5:0
libmp3splt_la_CPPFLAGS = -I$(top_srcdir)/include/libmp3splt
libmp3splt_la_LIBADD = @LIBINTL@ -lm

if WIN32

libmp3splt_la_LIBADD += -lltdl 

if IS_ON_WINDOWS
libmp3splt_la_LIBADD += /lib/libz.a
else
libmp3splt_la_LIBADD += -lz
endif

libmp3splt_la_LIBADD += -lws2_32 -lintl

else
libmp3splt_la_LIBADD += @LIBLTDL@ 
endif

libmp3splt_la_SOURCES = \
  types_func.c $(top_srcdir)/include/libmp3splt/types_func.h \
  splt.c $(top_srcdir)/include/libmp3splt/splt.h  \
  mp3splt.c $(top_srcdir)/include/libmp3splt/mp3splt.h \
  cddb.c $(top_srcdir)/include/libmp3splt/cddb.h \
  checks.c $(top_srcdir)/include/libmp3splt/checks.h \
  utils.c $(top_srcdir)/include/libmp3splt/utils.h \
  plugins.c $(top_srcdir)/include/libmp3splt/plugins.h \
  win32.c $(top_srcdir)/include/libmp3splt/win32.h \
  cue.c $(top_srcdir)/include/libmp3splt/cue.h \
  cddb_cue_common.c $(top_srcdir)/include/libmp3splt/cddb_cue_common.h \
  freedb.c $(top_srcdir)/include/libmp3splt/freedb.h \
  audacity.c $(top_srcdir)/include/libmp3splt/audacity.h \
  splt_array.c $(top_srcdir)/include/libmp3splt/splt_array.h \
  string_utils.c $(top_srcdir)/include/libmp3splt/string_utils.h \
  tags_utils.c $(top_srcdir)/include/libmp3splt/tags_utils.h \
  input_output.c $(top_srcdir)/include/libmp3splt/input_output.h \
  options.c $(top_srcdir)/include/libmp3splt/options.h \
  output_format.c $(top_srcdir)/include/libmp3splt/output_format.h \
  split_points.c $(top_srcdir)/include/libmp3splt/split_points.h \
  errors.c $(top_srcdir)/include/libmp3splt/errors.h \
  freedb_utils.c $(top_srcdir)/include/libmp3splt/freedb_utils.h \
  silence_utils.c $(top_srcdir)/include/libmp3splt/silence_utils.h \
  sync_errors.c $(top_srcdir)/include/libmp3splt/sync_errors.h \
  wrap.c $(top_srcdir)/include/libmp3splt/wrap.h \
  client.c $(top_srcdir)/include/libmp3splt/client.h \
  conversions.c $(top_srcdir)/include/libmp3splt/conversions.h \
  tags_parser.c $(top_srcdir)/include/libmp3splt/tags_parser.h \
  oformat_parser.c $(top_srcdir)/include/libmp3splt/oformat_parser.h \
  pair.c $(top_srcdir)/include/libmp3splt/pair.h \
  debug.c $(top_srcdir)/include/libmp3splt/debug.h

# Define a C macro LOCALEDIR indicating where catalogs will be installed.
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

