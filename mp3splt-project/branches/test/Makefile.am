AUTOMAKE_OPTIONS = nostdinc

bin_PROGRAMS = mp3splt

mp3splt_SOURCES = \
	mp3splt.c mp3splt.h \
	splt.c splt.h \
	mp3.c mp3.h \
	wrap.c wrap.h \
	cddb.c cddb.h \
	ogg.c ogg.h

mp3splt_LDADD = @OGG_LIBS@ @VORBIS_LIBS@

SUBDIRS = doc

EXTRA_DIST = mp3splt.1

man_MANS = mp3splt.1

install-exec-hook:
if DO_SYMLINK
	-cd $(DESTDIR)/$(bindir) && $(LN_S) mp3splt oggsplt
endif

uninstall-hook:
if DO_SYMLINK
	if [ -L $(DESTDIR)/$(bindir)/oggsplt ]; then \
		echo "The $(DESTDIR)/$(bindir)/mpg123 symlink still exists. You may want to remove it."\
	fi
endif

