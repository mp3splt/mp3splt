EXTRA_DIST = mp3splt-gtk.1 Doxyfile_all.in
man_MANS = mp3splt-gtk.1

if HAVE_DOXYGEN
html: doc

doc: doxyfile.temp

BUILD_DOC_DIR=$(top_builddir)/doc

doxyfile.temp:
	cat $(BUILD_DOC_DIR)/Doxyfile_all > $(BUILD_DOC_DIR)/Doxyfile
if HAVE_GRAPHVIZ
	echo "HAVE_DOT = YES" >> $(BUILD_DOC_DIR)/Doxyfile
else
	echo "HAVE_DOT = NO" >> $(BUILD_DOC_DIR)/Doxyfile
endif
	@echo "Running doxygen to generate the code documentation"
	doxygen
	echo temporary > $(BUILD_DOC_DIR)/doxyfile.temp

CLEANFILES=$(BUILD_DOC_DIR)/doxyfile.temp $(BUILD_DOC_DIR)/Doxyfile

all-local: html 

clean-local:
	-rm -f $(top_builddir)/doc/doxygen/search/*.css
	-rm -f $(top_builddir)/doc/doxygen/search/*.html
	-rm -f $(top_builddir)/doc/doxygen/search/*.js
	-rm -f $(top_builddir)/doc/doxygen/search/*.png
	-rmdir $(top_builddir)/doc/doxygen/search
	-rm -f $(top_builddir)/doc/doxygen/*.html
	-rm -f $(top_builddir)/doc/doxygen/*.png
	-rm -f $(top_builddir)/doc/doxygen/*.md5
	-rm -f $(top_builddir)/doc/doxygen/*.map
	-rm -f $(top_builddir)/doc/doxygen/*.js
	-rm -f $(top_builddir)/doc/doxygen/*.css
	-rm -f $(top_builddir)/doc/doxygen/*.svg
	-rmdir $(top_builddir)/doc/doxygen

#doxygen_target_dir=$(DESTDIR)$(docdir)/doxygen
#
#install-data-local:
#	-$(MKDIR_P) $(doxygen_target_dir)
#	-$(INSTALL_DATA) doxygen/*.html $(doxygen_target_dir)
#	-$(INSTALL_DATA) doxygen/*.png $(doxygen_target_dir)
#	-$(INSTALL_DATA) doxygen/*.md5 $(doxygen_target_dir)
#	-$(INSTALL_DATA) doxygen/*.map $(doxygen_target_dir)
#	-$(INSTALL_DATA) doxygen/*.js $(doxygen_target_dir)
#	-$(INSTALL_DATA) doxygen/*.css $(doxygen_target_dir)
#	-$(MKDIR_P) $(doxygen_target_dir)/search
#	-$(INSTALL_DATA) doxygen/search/*.css $(doxygen_target_dir)/search
#	-$(INSTALL_DATA) doxygen/search/*.html $(doxygen_target_dir)/search
#	-$(INSTALL_DATA) doxygen/search/*.js $(doxygen_target_dir)/search
#	-$(INSTALL_DATA) doxygen/search/*.png $(doxygen_target_dir)/search
#
#uninstall-local:
#	-rm $(doxygen_target_dir)/search/*.css
#	-rm $(doxygen_target_dir)/search/*.html
#	-rm $(doxygen_target_dir)/search/*.js
#	-rm $(doxygen_target_dir)/search/*.png
#	-rmdir $(doxygen_target_dir)/search
#	-rm $(doxygen_target_dir)/*.html
#	-rm $(doxygen_target_dir)/*.png
#	-rm $(doxygen_target_dir)/*.md5
#	-rm $(doxygen_target_dir)/*.map
#	-rm $(doxygen_target_dir)/*.js
#	-rm $(doxygen_target_dir)/*.css
#	-rmdir $(doxygen_target_dir)

else
doc:
	@echo "You must have 'doxygen' installed on your system to generate"
	@echo "the documentation.  Try installing it, and running"
	@echo "'configure' again."
endif

FORCE:
.PHONY: FORCE

