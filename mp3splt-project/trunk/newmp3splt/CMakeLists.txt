cmake_minimum_required(VERSION 2.8.4)
project(mp3spltcli)

include_directories(../libmp3splt/include/libmp3splt)
include_directories(../libmp3splt/include)
include_directories(src)

link_directories(../libmp3splt)

configure_file(config.h.in config.h)

set(CMAKE_C_FLAGS "-fno-builtin -g -Wall")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH}
    /usr/share/kde4/apps/cmake/modules
    ${PROJECT_SOURCE_DIR}/cmake)

set(MP3SPLT_SOURCE_FILES
    config.h
    src/options_parsers.c src/options_parsers.h
    src/options_manager.c src/options_manager.h
    src/print_utils.c src/print_utils.h
    src/utils.c src/utils.h
    src/options_checker.h src/options_checker.c
    src/data_manager.c src/data_manager.h
    src/windows_utils.c src/windows_utils.h
    src/freedb.c src/freedb.h
    src/common.h
    src/mp3splt.c
)

add_executable(mp3spltcli ${MP3SPLT_SOURCE_FILES})

target_link_libraries(mp3spltcli -lm -ldl -lmp3splt)

find_package(Libintl REQUIRED)
include_directories(${LIBINTL_INCLUDE_DIR})
target_link_libraries(mp3spltcli ${LIBINTL_LIBRARIES})

find_package(Gettext REQUIRED)
include_directories(${GETTEXT_INCLUDE_DIR})
target_link_libraries(mp3spltcli ${GETTEXT_LIBRARIES})

