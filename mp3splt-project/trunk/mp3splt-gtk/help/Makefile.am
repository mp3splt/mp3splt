#if GNOME
include $(top_srcdir)/build-aux/gnome-doc-utils.make

# Include the help files in the tarball
dist-hook: doc-dist-hook

# Make can now decide itself if the following line is
# an additional (empty) dependency or the rule for making
# doc-dist-hook on computers that don't have gnome.
doc-dist-hook:

DOC_MODULE = mp3splt-gtk
DOC_ENTITIES = legal.xml
DOC_INCLUDES = 
DOC_FIGURES = 
DOC_LINGUAS = en_GB de_DE
#else
doc:
	@echo Generation of help files is disabled since Gnome support is disabled.
#endif
