# General automake options.
AUTOMAKE_OPTIONS = foreign no-dependencies
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = m4 po doc src test

EXTRA_DIST = mp3splt-gtk.desktop.in autogen.sh debian/changelog debian/compat debian/control debian/copyright debian/dirs debian/docs debian/rules

appsdir = $(datadir)/applications
apps_DATA = mp3splt-gtk.desktop

mp3splt-gtk.desktop: mp3splt-gtk.desktop.in FORCE
	sed -e "s#PREFIX/#$(datadir)/pixmaps/#" $< > $@

doc: FORCE
	$(MAKE) -C doc doc

if GNOME
EXTRA_DIST += build-aux/gnome-doc-utils.make
SUBDIRS += help
endif

CLEANFILES = mp3splt-gtk.desktop

# A make target that always needs to be rebuilt
FORCE:
.PHONY: FORCE

