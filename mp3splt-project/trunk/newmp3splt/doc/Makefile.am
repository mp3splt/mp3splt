EXTRA_DIST = mp3splt.1 manual.html
man_MANS = mp3splt.1

install-exec-hook:
if DO_SYMLINK
	-$(mkdir_p) $(DESTDIR)/$(man1dir)
	-cd $(DESTDIR)/$(man1dir) && $(LN_S) mp3splt.1 oggsplt.1
endif

uninstall-hook:
if DO_SYMLINK
	if [ -L $(DESTDIR)/$(man1dir)/oggsplt.1 ];then \
	  echo "The $(DESTDIR)/$(man1dir)/oggsplt.1 symlink still exists. You may want to remove it."; \
	fi
endif

