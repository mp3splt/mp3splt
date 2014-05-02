EXTRA_DIST = run-tests.sh

INCLUDES = $(CUTTER_CFLAGS) $(GTK_CFLAGS) -I$(top_srcdir)/src
LIBS = $(CUTTER_LIBS) $(GTK_LIBS) -lm

if HAS_CUTTER

if AUDACIOUS
LIBS += @AUDACIOUS_LIBS@ @DBUS_GLIB_LIBS@
INCLUDES += @AUDACIOUS_CFLAGS@ @DBUS_GLIB_CFLAGS@
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
$(top_srcdir)/src/douglas_peucker.c $(top_srcdir)/src/douglas_peucker.h \
$(top_srcdir)/src/utilities.c $(top_srcdir)/src/utilities.h

TESTS = run-tests.sh
TESTS_ENVIRONMENT = NO_MAKE=yes CUTTER="$(CUTTER)" TESTS_DIR="$(top_builddir)/test"

echo-cutter:
	@echo $(CUTTER)

echo-tests-dir:
	@echo $(top_builddir)/test

endif

