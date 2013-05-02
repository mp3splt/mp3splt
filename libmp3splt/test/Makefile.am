
EXTRA_DIST = run-tests.sh

INCLUDES = $(CUTTER_CFLAGS) -I$(top_srcdir)/include -I$(top_srcdir)/src
LIBS = $(CUTTER_LIBS) $(top_builddir)/src/libmp3splt.la
AM_LDFLAGS = -module -rpath $(libdir) -avoid-version -no-undefined

if HAS_CUTTER

noinst_LTLIBRARIES = test_splt_array.la \
test_pair.la \
test_string_utils.la \
test_filename_regex.la \
test_socket_manager.la \
test_minimum_track_join.la \
test_splitpoints_handling.la \
test_tags_handling.la

test_splt_array_la_SOURCES = test_splt_array.c tests.h

test_pair_la_SOURCES = test_pair.c tests.h

test_string_utils_la_SOURCES = test_string_utils.c

test_filename_regex_la_SOURCES = test_filename_regex.c tests.c tests.h

test_socket_manager_la_SOURCES = test_socket_manager.c tests.h

test_minimum_track_join_la_SOURCES = test_minimum_track_join.c tests.h

test_splitpoints_handling_la_SOURCES = test_splitpoints_handling.c

test_tags_handling_la_SOURCES = test_tags_handling.c

TESTS = run-tests.sh
TESTS_ENVIRONMENT = NO_MAKE=yes CUTTER="$(CUTTER)" TESTS_DIR="$(top_builddir)/test"

echo-cutter:
	@echo $(CUTTER)

echo-tests-dir:
	@echo $(top_builddir)/test

endif

