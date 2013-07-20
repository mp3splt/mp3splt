includedir = $(prefix)/include/libmp3splt/

include_HEADERS = $(top_srcdir)/include/libmp3splt/mp3splt.h \
$(top_srcdir)/include/libmp3splt/version.h

INCLUDES=-DSPLT_PLUGINS_DIR=\"$(libdir)/libmp3splt$(SONAME)\" -DLIBMP3SPLT_VERSION=\"$(VERSION)\" @INCLTDL@

lib_LTLIBRARIES = libmp3splt.la

libmp3splt_la_LDFLAGS = -no-undefined -version-info 0:9:0
libmp3splt_la_CPPFLAGS = -I$(top_srcdir)/include/libmp3splt
libmp3splt_la_LIBADD = @LIBINTL@ -lm

if PCRE
libmp3splt_la_LIBADD += @PCRE_LIBS@ 
INCLUDES += @PCRE_CFLAGS@
else
INCLUDES += -DNO_PCRE
endif

if WIN32

libmp3splt_la_LIBADD += -lltdl 

if IS_ON_WINDOWS
libmp3splt_la_LIBADD += /lib/libz.a
else
libmp3splt_la_LIBADD += -lz
endif

libmp3splt_la_LIBADD += -lws2_32 -lintl -lshlwapi

else
libmp3splt_la_LIBADD += @LIBLTDL@ 
endif

libmp3splt_la_SOURCES = \
  types_func.c types_func.h \
  splt.c splt.h  \
  mp3splt.c $(top_srcdir)/include/libmp3splt/mp3splt.h \
  $(top_srcdir)/include/libmp3splt/version.h \
  cddb.c cddb.h \
  checks.c checks.h \
  utils.c utils.h \
  plugins.c plugins.h \
  win32.c win32.h \
  cue.c cue.h \
  cddb_cue_common.c cddb_cue_common.h \
  freedb.c freedb.h \
  audacity.c audacity.h \
  splt_array.c splt_array.h \
  string_utils.c string_utils.h \
  tags_utils.c tags_utils.h \
  input_output.c input_output.h \
  options.c options.h \
  output_format.c output_format.h \
  split_points.c split_points.h \
  errors.c errors.h \
  freedb_utils.c freedb_utils.h \
  silence_utils.c silence_utils.h \
  sync_errors.c sync_errors.h \
  wrap.c wrap.h \
  client.c client.h \
  conversions.c conversions.h \
  tags_parser.c tags_parser.h \
  oformat_parser.c oformat_parser.h \
  pair.c pair.h \
  debug.c debug.h \
  filename_regex.c filename_regex.h \
  socket_manager.c socket_manager.h \
  proxy.c proxy.h

# Define a C macro LOCALEDIR indicating where catalogs will be installed.
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

