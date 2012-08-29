
INCLUDES = $(CUTTER_CFLAGS) $(GTK_CFLAGS) -I$(top_srcdir)/src
LIBS = $(CUTTER_LIBS) $(GTK_LIBS) -lm

if AUDACIOUS
LIBS += @AUDACIOUS_LIBS@
INCLUDES += @AUDACIOUS_CFLAGS@ -I/usr/include/dbus-1.0 -I/usr/include/dbus-1.0/include
endif

if GSTREAMER
LIBS += @GST_LIBS@
INCLUDES += @GST_CFLAGS@
endif

DEFS=
if !GNOME
DEFS += -DNO_GNOME
endif
if !AUDACIOUS
DEFS += -DNO_AUDACIOUS
endif
if !GSTREAMER
DEFS += -DNO_GSTREAMER
endif

AM_LDFLAGS = -module -rpath $(libdir) -avoid-version -no-undefined

noinst_LTLIBRARIES = test_douglas_peucker.la

test_douglas_peucker_la_SOURCES = test_douglas_peucker.c tests.h \
../src/douglas_peucker.c ../src/douglas_peucker.h ../src/utilities.c ../src/utilities.h

TESTS = run-tests.sh
TESTS_ENVIRONMENT = NO_MAKE=yes CUTTER="$(CUTTER)"

echo-cutter:
	@echo $(CUTTER)

