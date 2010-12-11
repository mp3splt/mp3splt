
INCLUDES = $(CUTTER_CFLAGS) -I$(top_srcdir)/include
LIBS = $(CUTTER_LIBS) $(top_builddir)/src/libmp3splt.la
AM_LDFLAGS = -module -rpath $(libdir) -avoid-version -no-undefined

if HAS_CUTTER

noinst_LTLIBRARIES = test_splt_array.la \
test_pair.la \
test_string_utils.la \
test_filename_regex.la

my_include_dir=$(top_srcdir)/include

test_splt_array_la_SOURCES = test_splt_array.c tests.h \
  $(my_include_dir)/splt_array.h

test_pair_la_SOURCES = test_pair.c tests.h \
  $(my_include_dir)/pair.h

test_string_utils_la_SOURCES = test_string_utils.c \
  $(my_include_dir)/mp3splt.h \
  $(my_include_dir)/string_utils.h

test_filename_regex_la_SOURCES = test_filename_regex.c tests.c tests.h \
  $(my_include_dir)/mp3splt.h \
  $(my_include_dir)/options.h \
  $(my_include_dir)/filename_regex.h \
  $(my_include_dir)/tags_utils.h

TESTS = run-tests.sh
TESTS_ENVIRONMENT = NO_MAKE=yes CUTTER="$(CUTTER)"

echo-cutter:
	@echo $(CUTTER)

endif

