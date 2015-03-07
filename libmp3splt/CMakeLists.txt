cmake_minimum_required(VERSION 2.8.4)
project(libmp3splt)

include_directories(include/libmp3splt)
include_directories(include)
include_directories(src)

configure_file(config.h.in config.h)

add_definitions(-DLIBMP3SPLT_VERSION="0.9.3")
add_definitions(-DSPLT_PLUGINS_DIR="/usr/lib/libmp3splt")

set(CMAKE_C_FLAGS "-fno-builtin -g -Wall")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    /usr/share/kde4/apps/cmake/modules
    ${PROJECT_SOURCE_DIR}/cmake)

set(LIBMP3SPLT_SOURCE_FILES
    config.h
    include/libmp3splt/mp3splt.h
    include/libmp3splt/version.h
    src/audacity.c
    src/audacity.h
    src/cddb.c
    src/cddb.h
    src/cddb_cue_common.c
    src/cddb_cue_common.h
    src/checks.c
    src/checks.h
    src/client.c
    src/client.h
    src/conversions.c
    src/conversions.h
    src/cue.c
    src/cue.h
    src/debug.c
    src/debug.h
    src/errors.c
    src/errors.h
    src/filename_regex.c
    src/filename_regex.h
    src/freedb.c
    src/freedb.h
    src/freedb_utils.c
    src/freedb_utils.h
    src/input_output.c
    src/input_output.h
    src/mp3splt.c
    src/oformat_parser.c
    src/oformat_parser.h
    src/options.c
    src/options.h
    src/output_format.c
    src/output_format.h
    src/pair.c
    src/pair.h
    src/plugins.c
    src/plugins.h
    src/proxy.c
    src/proxy.h
    src/silence_utils.c
    src/silence_utils.h
    src/socket_manager.c
    src/socket_manager.h
    src/split_points.c
    src/split_points.h
    src/splt.c
    src/splt.h
    src/splt_array.c
    src/splt_array.h
    src/string_utils.c
    src/string_utils.h
    src/sync_errors.c
    src/sync_errors.h
    src/tags_parser.c
    src/tags_parser.h
    src/tags_utils.c
    src/tags_utils.h
    src/types_func.c
    src/types_func.h
    src/utils.c
    src/utils.h
    src/win32.c
    src/win32.h
    src/wrap.c
    src/wrap.h
)

add_library(mp3splt SHARED ${LIBMP3SPLT_SOURCE_FILES})

#TODO: tests compilation
#    test/test_filename_regex.c
#    test/test_minimum_track_join.c
#    test/test_pair.c
#    test/test_socket_manager.c
#    test/test_splitpoints_handling.c
#    test/test_splt_array.c
#    test/test_string_utils.c
#    test/test_tags_handling.c
#    test/tests.c
#    test/tests.h

target_link_libraries(mp3splt -lm -ldl)

find_package(Libintl REQUIRED)
include_directories(${LIBINTL_INCLUDE_DIR})
target_link_libraries(mp3splt ${LIBINTL_LIBRARIES})

find_package(Gettext REQUIRED)
include_directories(${GETTEXT_INCLUDE_DIR})
target_link_libraries(mp3splt ${GETTEXT_LIBRARIES})

find_package(LTDL REQUIRED)
include_directories(${LTDL_INCLUDE_DIR})
target_link_libraries(mp3splt ${LTDL_LIBRARIES})

find_package(PCRE)
if (PCRE_FOUND)
  include_directories(${PCRE_INCLUDE_DIRS})
  target_link_libraries(mp3splt ${PCRE_LIBRARIES})
endif (PCRE_FOUND)
#TODO: define NO_PCRE if no PCRE found

add_subdirectory("plugins")

