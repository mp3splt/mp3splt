# General automake options.
AUTOMAKE_OPTIONS = foreign no-dependencies
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = m4 po doc src help

EXTRA_DIST = mp3splt-gtk.desktop autogen.sh build-aux/gnome-doc-utils.make m4

appsdir = $(datadir)/applications
apps_DATA = mp3splt-gtk.desktop


doc: FORCE
	$(MAKE) -C doc doc


DISTCHECK_CONFIGURE_FLAGS = --disable-scrollkeeper

# A make target that always needs to be rebuilt
FORCE:
.PHONY: FORCE