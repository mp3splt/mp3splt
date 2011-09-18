if GNOME

include $(top_srcdir)/build-aux/gnome-doc-utils.make

dist-hook: doc-dist-hook
DOC_MODULE = mp3splt-gtk
DOC_ENTITIES = legal.xml
DOC_INCLUDES = 
DOC_FIGURES = figures/FreeDBSearch.png figures/mp3splt-gtk.png figures/Preferences.png figures/SplitType.png figures/Tracklist.png
DOC_LINGUAS = en de fr es_CL cs es ru_RU
EXTRA_DIST = C/figures/FreeDBSearch.png C/figures/mp3splt-gtk.png C/figures/Preferences.png C/figures/SplitType.png C/figures/Tracklist.png

else

doc:
	@echo Generation of help files is disabled since Gnome support is disabled.

endif

