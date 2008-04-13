appicondir = $(datadir)/pixmaps
appicon_DATA = mp3splt-gtk_ico.png mp3splt-gtk.png
SOURCES = mp3splt-gtk_ico.png mp3splt-gtk.png
EXTRA_DIST = $(appicon_DATA)

bin_PROGRAMS = mp3splt-gtk

mp3splt_gtk_SOURCES = \
	tree_tab.c tree_tab.h \
	main_win.c main_win.h \
	xmms_control.c xmms_control.h \
	player.c player.h \
	player_tab.c player_tab.h \
	preferences_tab.c preferences_tab.h \
	utilities.c utilities.h \
	splitted_files.c splitted_files.h \
	freedb_tab.c freedb_tab.h \
	cddb_cue.c cddb_cue.h \
	snackamp_control.c snackamp_control.h \
	special_split.c special_split.h\
	mp3splt-gtk.c mp3splt-gtk.h \
	util.h

INCLUDES = @GTK_CFLAGS@ -DPIXMAP_PATH=\""$(datadir)/pixmaps/"\" -Wall

mp3splt_gtk_LDADD = @GTK_LIBS@ -lgthread-2.0 -lmp3splt

# Define a C macro LOCALEDIR indicating where catalogs will be installed.
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@
