EXTRA_DIST = mp3splt.1 manual.html
man_MANS = mp3splt.1

install-data-hook:
if DO_OGG_SYMLINK
	-cd $(DESTDIR)/$(mandir)/man1 && $(LN_S) mp3splt.1 oggsplt.1
endif
if DO_FLAC_SYMLINK
	-cd $(DESTDIR)/$(mandir)/man1 && $(LN_S) mp3splt.1 flacsplt.1
endif

uninstall-hook:
if DO_OGG_SYMLINK
	if [ -L $(DESTDIR)/$(mandir)/man1/oggsplt.1 ];then \
	  echo "The $(DESTDIR)/$(mandir)/man1/oggsplt.1 symlink still exists. You may want to remove it."; \
	fi
endif
if DO_FLAC_SYMLINK
	if [ -L $(DESTDIR)/$(mandir)/man1/flacsplt.1 ];then \
	  echo "The $(DESTDIR)/$(mandir)/man1/flacsplt.1 symlink still exists. You may want to remove it."; \
	fi
endif

