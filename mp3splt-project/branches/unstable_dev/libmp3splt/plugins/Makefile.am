plugindir=$(libdir)/libmp3splt

plugin_LTLIBRARIES = 

#ccommon_LDFLAGS = -module -export-dynamic -avoid-version
common_LDFLAGS = -L../src -L../src/.libs -L/lib -no-undefined -lmp3splt

#mp3 plugin
if MP3_PLUGIN

plugin_LTLIBRARIES += libsplt_mp3.la
libsplt_mp3_la_SOURCES = mp3.c mp3.h
libsplt_mp3_la_LDFLAGS = $(common_LDFLAGS)
libsplt_mp3_la_LIBADD = -lmad

if LIB_ID3TAG
libsplt_mp3_la_LIBADD += -lid3tag
endif

endif

#OGG plugin
if OGG_PLUGIN

plugin_LTLIBRARIES += libsplt_ogg.la
libsplt_ogg_la_SOURCES = ogg.c ogg.h
libsplt_ogg_la_LDFLAGS = $(common_LDFLAGS)
libsplt_ogg_la_LIBADD = -lvorbisfile -lvorbis -logg

endif

