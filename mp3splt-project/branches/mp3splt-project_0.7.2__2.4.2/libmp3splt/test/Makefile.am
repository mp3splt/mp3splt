
INCLUDES = $(CUTTER_CFLAGS) -I$(top_srcdir)/include -I$(top_srcdir)/src
LIBS = $(CUTTER_LIBS) $(top_builddir)/src/libmp3splt.la
AM_LDFLAGS = -module -rpath $(libdir) -avoid-version -no-undefined

if HAS_CUTTER

noinst_LTLIBRARIES = test_splt_array.la \
test_pair.la \
test_string_utils.la \
test_filename_regex.la \
test_socket_manager.la

test_splt_array_la_SOURCES = test_splt_array.c tests.h

test_pair_la_SOURCES = test_pair.c tests.h

test_string_utils_la_SOURCES = test_string_utils.c

test_filename_regex_la_SOURCES = test_filename_regex.c tests.c tests.h

test_socket_manager_la_SOURCES = test_socket_manager.c tests.h

TESTS = run-tests.sh
TESTS_ENVIRONMENT = NO_MAKE=yes CUTTER="$(CUTTER)"

echo-cutter:
	@echo $(CUTTER)

endif

