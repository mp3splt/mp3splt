EXTRA_DIST = mp3splt-gtk.1 Doxyfile.in
man_MANS = mp3splt-gtk.1
DOC_DIR=$(HTML_DIR)

if HAVE_DOXYGEN
html: doc

doc:
	cat Doxyfile.in >Doxyfile
if HAVE_GRAPHVIZ
	echo "HAVE_DOT = YES" >> Doxyfile
else
	echo "HAVE_DOT = NO" >> Doxyfile
endif
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