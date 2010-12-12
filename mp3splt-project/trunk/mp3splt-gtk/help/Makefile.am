#if HAVE_GNOME_DOC_UTILS
include $(top_srcdir)/build-aux/gnome-doc-utils.make

# Include the help files in the tarball
dist-hook: doc-dist-hook

# Either an empty dependency or a dummy doc-dist-hook, depending
# on what make needs.
doc-dist-hook: en_GB

DOC_MODULE = mp3splt-gtk
DOC_ENTITIES = legal.xml
DOC_INCLUDES = 
#DOC_FIGURES = figures/FreeDBSearch.png figures/mp3splt-gtk.png figures/Preferences.png figures/SplitType.png figures/Tracklist.png
DOC_LINGUAS = en_GB de_DE
EXTRA_DIST = $(DOC_FIGURES)
#else
doc:
	@echo Generation of help files is disabled since Gnome support is disabled.
#endif
