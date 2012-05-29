INCLUDES = $(CUTTER_CFLAGS) $(GTK_CFLAGS) -I$(top_srcdir)/src
LIBS = $(CUTTER_LIBS) $(GTK_LIBS)

AM_LDFLAGS = -module -rpath $(libdir) -avoid-version -no-undefined

if HAS_CUTTER

noinst_LTLIBRARIES = test_points_distance.la

test_points_distance_la_SOURCES = test_douglas_peucker.c tests.h \
../src/douglas_peucker.c ../src/douglas_peucker.h

TESTS = run-tests.sh
TESTS_ENVIRONMENT = NO_MAKE=yes CUTTER="$(CUTTER)"

echo-cutter:
	@echo $(CUTTER)

endif

