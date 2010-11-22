#if GNOME
include $(top_srcdir)/build-aux/gnome-doc-utils.make
#make dist fails with doc-dist-hook ...
#dist-hook: doc-dist-hook

DOC_MODULE = mp3splt-gtk
DOC_ENTITIES = legal.xml
DOC_INCLUDES = 
DOC_FIGURES = 
DOC_LINGUAS = en_GB de_DE
#else
doc:
	@echo Generation of help files is disabled since Gnome support is disabled.
#endif
