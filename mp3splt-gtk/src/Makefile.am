
if NO_IMAGES_DIR
appicondir =
imagedir   =
else
appicondir = $(datadir)/pixmaps
imagedir   = $(datadir)/mp3splt-gtk
endif

image_DATA = backward.svg backward_inactive.svg  
image_DATA+= forward.svg  forward_inactive.svg
image_DATA+= play.svg     play_inactive.svg
image_DATA+= stop.svg     stop_inactive.svg
image_DATA+= pause.svg    pause_inactive.svg 

appicon_DATA = mp3splt-gtk.png mp3splt-gtk_ico.svg
SOURCES = $(appicon_DATA) $(image_DATA)
EXTRA_DIST = $(appicon_DATA) $(image_DATA)

bin_PROGRAMS = mp3splt-gtk

mp3splt_gtk_SOURCES = \
	splitpoints_window.c splitpoints_window.h \
	main_window.c main_window.h \
	audacious_control.c audacious_control.h \
	gstreamer_control.c gstreamer_control.h \
	player_control.c player_control.h \
	player_window.c player_window.h \
	preferences_window.c preferences_window.h \
	utilities.c utilities.h \
	split_files_window.c split_files_window.h \
	freedb_window.c freedb_window.h \
	snackamp_control.c snackamp_control.h \
	split_mode_window.c split_mode_window.h \
	mp3splt-gtk.c mp3splt-gtk.h \
	multiple_files_window.c multiple_files_window.h \
	messages_window.c messages_window.h \
	import.c import.h \
	options_manager.c options_manager.h \
	preferences_manager.c preferences_manager.h \
	widgets_helper.c widgets_helper.h \
  drawing_helper.c drawing_helper.h \
	combo_helper.c combo_helper.h \
	radio_helper.c radio_helper.h \
	export.c export.h \
	ui_manager.c ui_manager.h \
  douglas_peucker.c douglas_peucker.h \
  libmp3splt_manager.c libmp3splt_manager.h \
  external_includes.h ui_types.h all_includes.h \
  drag_and_drop.c drag_and_drop.h \
  mutex.c mutex.h

INCLUDES = @GTK_CFLAGS@ @LIBMP3SPLT_CFLAGS@

mp3splt_gtk_LDADD = @GTK_LIBS@ -lgthread-2.0 @LIBMP3SPLT_LIBS@ -lm

# Define a C macro LOCALEDIR indicating where catalogs will be installed.
localedir = $(datadir)/locale
DEFS = -DLOCALEDIR=\"$(localedir)\" @DEFS@

if !GNOME
DEFS += -DNO_GNOME
endif

if WIN32
INCLUDES += -mms-bitfields -enable-stdcall-fixup
mp3splt_gtk_LDFLAGS = -mwindows
mp3splt_gtk_LDADD += -lwsock32 -lshlwapi
else
DEFS += -DPIXMAP_PATH=\"$(appicondir)\" -DIMAGEDIR=\"$(imagedir)\"
endif

if AUDACIOUS
mp3splt_gtk_LDADD += @AUDACIOUS_LIBS@ @DBUS_GLIB_LIBS@
INCLUDES += @AUDACIOUS_CFLAGS@ @DBUS_GLIB_CFLAGS@
else
DEFS += -DNO_AUDACIOUS
endif

if GSTREAMER
mp3splt_gtk_LDADD += @GST_LIBS@
INCLUDES += @GST_CFLAGS@
else
DEFS += -DNO_GSTREAMER
endif

