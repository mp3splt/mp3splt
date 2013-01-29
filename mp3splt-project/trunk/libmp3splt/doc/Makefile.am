SUBDIRS = examples

EXTRA_DIST = Doxyfile.in empty_footer.html libmp3splt_ico.svg

if HAVE_DOXYGEN
html: doc

doc: doxyfile.temp

api_doc: doxyfile_api.temp

doxyfile_api.temp:
	cat Doxyfile_api.in >Doxyfile
if HAVE_GRAPHVIZ
	echo "HAVE_DOT = YES" >> Doxyfile
else
	echo "HAVE_DOT = NO" >> Doxyfile
endif
	echo "STRIP_FROM_PATH = $(top_builddir)" >> Doxyfile
	@echo "Running doxygen to generate the API documentation"
	doxygen
	echo temporary > doxyfile.temp

doxyfile.temp:
	cat Doxyfile.in >Doxyfile
if HAVE_GRAPHVIZ
	echo "HAVE_DOT = YES" >> Doxyfile
else
	echo "HAVE_DOT = NO" >> Doxyfile
endif
	echo "STRIP_FROM_PATH = $(top_builddir)" >> Doxyfile
	@echo "Running doxygen to generate the code documentation"
	doxygen
	echo temporary > doxyfile.temp

CLEANFILES=doxyfile.temp doxyfile_api.temp

all-local: html 

clean-local:
	-rm -f $(top_srcdir)/doc/doxygen/*.html
	-rm -f $(top_srcdir)/doc/doxygen/*.png
	-rm -f $(top_srcdir)/doc/doxygen/*.md5
	-rm -f $(top_srcdir)/doc/doxygen/*.map
	-rm -f $(top_srcdir)/doc/doxygen/*.js
	-rm -f $(top_srcdir)/doc/doxygen/*.css
	-rm -f $(top_srcdir)/doc/doxygen/*.svg

doxygen_target_dir=$(DESTDIR)$(docdir)/doxygen

install-data-local:
	-$(MKDIR_P) $(doxygen_target_dir)
	-$(INSTALL_DATA) doxygen/*.html $(doxygen_target_dir)
	-$(INSTALL_DATA) doxygen/*.png $(doxygen_target_dir)
	-$(INSTALL_DATA) doxygen/*.md5 $(doxygen_target_dir)
	-$(INSTALL_DATA) doxygen/*.map $(doxygen_target_dir)
	-$(INSTALL_DATA) doxygen/*.js $(doxygen_target_dir)
	-$(INSTALL_DATA) doxygen/*.css $(doxygen_target_dir)
	-$(INSTALL_DATA) libmp3splt_ico.svg $(doxygen_target_dir)

uninstall-local:
	-rm $(doxygen_target_dir)/*.html
	-rm $(doxygen_target_dir)/*.png
	-rm $(doxygen_target_dir)/*.md5
	-rm $(doxygen_target_dir)/*.map
	-rm $(doxygen_target_dir)/*.js
	-rm $(doxygen_target_dir)/*.css
	-rm $(doxygen_target_dir)/libmp3splt_ico.svg
	-rmdir $(doxygen_target_dir)

else
doc:
	@echo "You must have 'doxygen' installed on your system to generate"
	@echo "the documentation.  Try installing it, and running"
	@echo "'configure' again."
endif

FORCE:
.PHONY: FORCE

