bin_PROGRAMS = mp3splt

mp3splt_SOURCES = \
	options_parsers.c options_parsers.h \
	options_manager.c options_manager.h \
	print_utils.c print_utils.h \
	utils.c utils.h \
	options_checker.h options_checker.c \
	data_manager.c data_manager.h \
	windows_utils.c windows_utils.h \
	freedb.c freedb.h \
	common.h \
	mp3splt.c

AM_CFLAGS = @LIBMP3SPLT_CFLAGS@
mp3splt_LDADD = @LIBMP3SPLT_LIBS@ @LIBINTL@

if WIN32
  mp3splt_LDADD += -lwsock32 -lshlwapi 
endif

# Define a C macro LOCALEDIR indicating where catalogs will be installed.
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

install-exec-hook:
if DO_OGG_SYMLINK
	-cd $(DESTDIR)/$(bindir) && $(LN_S) mp3splt oggsplt
endif
if DO_FLAC_SYMLINK
	-cd $(DESTDIR)/$(bindir) && $(LN_S) mp3splt flacsplt
endif

uninstall-hook:
if DO_OGG_SYMLINK
	if [ -L $(DESTDIR)/$(bindir)/oggsplt ];then \
	  echo "The $(DESTDIR)/$(bindir)/oggsplt symlink still exists. You may want to remove it."; \
	fi
endif
if DO_FLAC_SYMLINK
	if [ -L $(DESTDIR)/$(bindir)/flacsplt ];then \
	  echo "The $(DESTDIR)/$(bindir)/flacsplt symlink still exists. You may want to remove it."; \
	fi
endif

