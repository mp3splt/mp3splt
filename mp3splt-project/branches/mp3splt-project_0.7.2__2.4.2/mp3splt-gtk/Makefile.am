# General automake options.
AUTOMAKE_OPTIONS = foreign no-dependencies
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = m4 po doc src

EXTRA_DIST = mp3splt-gtk.desktop.in autogen.sh m4 debian/changelog debian/compat debian/control debian/copyright debian/dirs debian/docs debian/rules

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

# A make target that always needs to be rebuilt
FORCE:
.PHONY: FORCE

