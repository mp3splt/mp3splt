
link_directories(../)

set(COMMON_PLUGIN_FILES
    silence_processors.c
    silence_processors.h
)

#mp3
set(MP3_PLUGIN_SOURCES
    mp3.c
    mp3.h
    mp3_silence.c
    mp3_silence.h
    mp3_utils.c
    mp3_utils.h
)

add_library(splt_mp3 SHARED ${MP3_PLUGIN_SOURCES} ${COMMON_PLUGIN_FILES})

target_link_libraries(splt_mp3 -lm -lmp3splt)

find_package(Mad REQUIRED)
include_directories(splt_mp3 ${MAD_INCLUDE_DIR})
target_link_libraries(splt_mp3 ${MAD_LIBRARIES})

find_package(ID3Tag REQUIRED)
include_directories(splt_mp3 ${ID3TAG_INCLUDE_DIR})
target_link_libraries(splt_mp3 ${ID3TAG_LIBRARY})
#TODO: define NO_ID3TAG if not found

#ogg
set(OGG_PLUGIN_SOURCES
    ogg.c
    ogg.h
    ogg_new_stream_handler.c
    ogg_new_stream_handler.h
    ogg_silence.c
    ogg_silence.h
    ogg_utils.c
    ogg_utils.h
)

add_library(splt_ogg SHARED ${OGG_PLUGIN_SOURCES} ${COMMON_PLUGIN_FILES})

target_link_libraries(splt_ogg -lm -lmp3splt)

find_package(OggVorbis REQUIRED)
include_directories(splt_ogg ${OGGVORBIS_INCLUDE_DIR})
target_link_libraries(splt_ogg ${OGGVORBIS_LIBRARIES})

#flac
set(FLAC_PLUGIN_SOURCES
    flac.c
    flac.h
    flac_frame_reader.c
    flac_frame_reader.h
    flac_md5_decoder.c
    flac_md5_decoder.h
    flac_metadata.c
    flac_metadata.h
    flac_metadata_utils.c
    flac_metadata_utils.h
    flac_silence.c
    flac_silence.h
    flac_tags.c
    flac_tags.h
    flac_utils.c
    flac_utils.h
    from_flac_library.c
    from_flac_library.h
    md5.c
    md5.h
)

add_library(splt_flac SHARED ${FLAC_PLUGIN_SOURCES} ${COMMON_PLUGIN_FILES})

target_link_libraries(splt_flac -lm -lmp3splt)

find_package(Flac REQUIRED)
include_directories(${FLAC_INCLUDE_DIR})
target_link_libraries(splt_flac ${FLAC_LIBRARIES})

