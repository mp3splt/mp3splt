EXTRA_DIST = mp3splt-gtk.1 Doxyfile
man_MANS = mp3splt-gtk.1
DOC_DIR=$(HTML_DIR)

if HAVE_DOXYGEN
doc:
	@echo "Running doxygen to generate the code documentation"
	doxygen
else
doc:
	@echo "You must have 'doxygen' installed on your system to generate"
	@echo "the documentation.  Try installing it, and running"
	@echo "'configure' again."
endif

# A FORCE target that 
FORCE:
.PHONY: FORCE