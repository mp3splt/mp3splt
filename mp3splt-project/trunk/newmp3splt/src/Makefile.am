bin_PROGRAMS = mp3splt

mp3splt_SOURCES = mp3splt.c

INCLUDES = @MP3SPLT_CFLAGS@
mp3splt_LDADD = @MP3SPLT_LIBS@ @LIBINTL@

if WIN32
  mp3splt_LDADD += -lwsock32 -lshlwapi 
endif

# Define a C macro LOCALEDIR indicating where catalogs will be installed.
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

install-exec-hook:
if DO_SYMLINK
	-cd $(DESTDIR)/$(bindir) && $(LN_S) mp3splt oggsplt
endif
uninstall-hook:
if DO_SYMLINK
	if [ -L $(DESTDIR)/$(bindir)/oggsplt ];then \
	  echo "The $(DESTDIR)/$(bindir)/oggsplt symlink still exists. You may want to remove it."; \
	fi
endif

