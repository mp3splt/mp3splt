appicondir = $(datadir)/pixmaps
appicon_DATA = mp3splt-gtk_ico.png mp3splt-gtk.png
SOURCES = mp3splt-gtk_ico.png mp3splt-gtk.png
EXTRA_DIST = $(appicon_DATA)

bin_PROGRAMS = mp3splt-gtk

mp3splt_gtk_SOURCES = \
	tree_tab.c tree_tab.h \
	main_win.c main_win.h \
	xmms_control.c xmms_control.h \
	gstreamer_control.c gstreamer_control.h \
	player.c player.h \
	player_tab.c player_tab.h \
	preferences_tab.c preferences_tab.h \
	utilities.c utilities.h \
	split_files.c split_files.h \
	freedb_tab.c freedb_tab.h \
	snackamp_control.c snackamp_control.h \
	special_split.c special_split.h\
	mp3splt-gtk.c mp3splt-gtk.h \
	util.h \
	multiple_files.c multiple_files.h \
	messages.c messages.h \
	import.c import.h \
	options_manager.c options_manager.h \
	preferences_manager.c preferences_manager.h \
	widgets_helper.c widgets_helper.h \
	combo_helper.c combo_helper.h \
	radio_helper.c radio_helper.h \
	export.c export.h 

INCLUDES = @GTK_CFLAGS@ @MP3SPLT_CFLAGS@

mp3splt_gtk_LDADD = @GTK_LIBS@ -lgthread-2.0 @MP3SPLT_LIBS@ -lm

if GNOME
mp3splt_gtk_LDADD += @GNOMEUI_LIBS@
INCLUDES += $(GNOMEUI_CFLAGS)
else
INCLUDES += -DNO_GNOME
endif

if WIN32
INCLUDES += -mms-bitfields -enable-stdcall-fixup
mp3splt_gtk_LDFLAGS = -mwindows
mp3splt_gtk_LDADD += -lwsock32 -lshlwapi
else
INCLUDES += -DPIXMAP_PATH=\""$(datadir)/pixmaps/"\"
endif

if AUDACIOUS
mp3splt_gtk_LDADD += @AUDACIOUS_LIBS@
INCLUDES += @AUDACIOUS_CFLAGS@ -I/usr/include/dbus-1.0 -I/usr/include/dbus-1.0/include
else
INCLUDES += -DNO_AUDACIOUS
endif

if GSTREAMER
mp3splt_gtk_LDADD += @GST_LIBS@
INCLUDES += @GST_CFLAGS@
else
INCLUDES += -DNO_GSTREAMER
endif

# Define a C macro LOCALEDIR indicating where catalogs will be installed.
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

