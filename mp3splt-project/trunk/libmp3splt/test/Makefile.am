
INCLUDES = $(CUTTER_CFLAGS) -I$(top_srcdir)/include
LIBS = $(CUTTER_LIBS) $(top_builddir)/src/libmp3splt.la
AM_LDFLAGS = -module -rpath $(libdir) -avoid-version -no-undefined

if HAS_CUTTER

noinst_LTLIBRARIES = test_splt_array.la
test_splt_array_la_SOURCES = test_splt_array.c

endif

TESTS = run-tests.sh
TESTS_ENVIRONMENT = NO_MAKE=yes CUTTER="$(CUTTER)"

echo-cutter:
	@echo $(CUTTER)

