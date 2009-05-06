plugindir=$(libdir)/libmp3splt

plugin_LTLIBRARIES = 
INCLUDES =

#ccommon_LDFLAGS = -module -export-dynamic -avoid-version
common_LDFLAGS = -L../src -L../src/.libs -L/lib -no-undefined -lm -lmp3splt

#mp3 plugin
if MP3_PLUGIN

INCLUDES += @MAD_CFLAGS@
plugin_LTLIBRARIES += libsplt_mp3.la
libsplt_mp3_la_SOURCES = mp3.c mp3.h
libsplt_mp3_la_LDFLAGS = $(common_LDFLAGS) @MAD_LIBS@

if ID3TAG
INCLUDES += @ID3_CFLAGS@
libsplt_mp3_la_LIBADD = @ID3_LIBS@
else
INCLUDES += -DNO_ID3TAG
endif

endif

#OGG plugin
if OGG_PLUGIN

INCLUDES += @OGG_CFLAGS@ @VORBIS_CFLAGS@
plugin_LTLIBRARIES += libsplt_ogg.la
libsplt_ogg_la_SOURCES = ogg.c ogg.h
libsplt_ogg_la_LDFLAGS = $(common_LDFLAGS) @VORBISFILE_LIBS@ @VORBIS_LIBS@ @OGG_LIBS@ 

endif

