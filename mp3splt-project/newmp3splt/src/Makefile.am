bin_PROGRAMS = mp3splt

mp3splt_SOURCES = mp3splt.c

INCLUDES = -Wall -g

mp3splt_LDADD = -lmp3splt

#install-exec-hook:
#if DO_SYMLINK
#	-cd $(DESTDIR)/$(bindir) && $(LN_S) mp3splt oggsplt
#endif
#uninstall-hook:
#if DO_SYMLINK
#	if [ -L $(DESTDIR)/$(bindir)/oggsplt ]; then \
#		echo "The $(DESTDIR)/$(bindir)/oggsplt symlink still exists. You may want to remove it." \
#	fi \;
#endif
