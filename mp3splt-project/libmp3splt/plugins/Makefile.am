plugindir=$(libdir)/libmp3splt

plugin_LTLIBRARIES = 

common_LDFLAGS = -module -export-dynamic -avoid-version

#mp3 plugin
if MP3_PLUGIN

plugin_LTLIBRARIES += splt_mp3.la
splt_mp3_la_SOURCES = mp3.c mp3.h
splt_mp3_la_LDFLAGS = $(common_LDFLAGS)
splt_mp3_la_LIBADD = -lmad

if LIB_ID3TAG
splt_mp3_la_LIBADD += -lid3tag
endif

endif

#OGG plugin
if OGG_PLUGIN

plugin_LTLIBRARIES += splt_ogg.la
splt_ogg_la_SOURCES = ogg.c ogg.h
splt_ogg_la_LDFLAGS = $(common_LDFLAGS)
splt_ogg_la_LIBADD = -lvorbisfile -lvorbis -logg

endif

