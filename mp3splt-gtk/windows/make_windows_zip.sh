#!/bin/sh
#we should have done '!/bin/bash', but it won't work on msys+mingw

#if the first argument of the script is not empty, then we consider a cross
#compilation; otherwise, consider a windows installation
we_dont_cross_compile=$1

#we move in the current script directory
#on unix-like env
if [[ ! -z $we_dont_cross_compile ]];then
  script_dir=$(readlink -f $0) || exit 1
  script_dir=${script_dir%\/*.sh}
  cd $script_dir
fi

. ../include_variables.sh || exit 1

ARCH="i386"

DLL_SUFFIX="-0"
if [[ -z $we_dont_cross_compile ]];then
  put_package "windows_zip"
else
  put_package "cross_windows_zip"
fi

LANGUAGES="fr_FR de_DE"

BASIC_FILES="
mp3splt-gtk/src/.libs/mp3splt-gtk.exe
libmp3splt/src/.libs/libmp3splt${DLL_SUFFIX}.dll
libltdl-7.dll
pcre3.dll
LIBMP3SPLT_LIBS_README_LICENSES_SOURCES.txt
MP3SPLT-GTK_LIBS_README_LICENSES_SOURCES.txt
mp3splt-gtk/src/mp3splt-gtk.png
mp3splt-gtk/src/mp3splt-gtk_ico.svg
mp3splt-gtk/src/backward.svg
mp3splt-gtk/src/backward_inactive.svg
mp3splt-gtk/src/forward.svg
mp3splt-gtk/src/forward_inactive.svg
mp3splt-gtk/src/pause.svg
mp3splt-gtk/src/pause_inactive.svg
mp3splt-gtk/src/play.svg
mp3splt-gtk/src/play_inactive.svg
mp3splt-gtk/src/stop.svg
mp3splt-gtk/src/stop_inactive.svg
mp3splt-gtk/windows/mp3splt-gtk.ico
"

MP3_PLUGIN_FILES="
libmad-0.dll
libid3tag.dll
libmp3splt/plugins/.libs/libsplt_mp3${DLL_SUFFIX}.dll
"

OGG_PLUGIN_FILES="
libogg-0.dll
libvorbis-0.dll
libvorbisenc-2.dll
libvorbisfile-3.dll
libmp3splt/plugins/.libs/libsplt_ogg${DLL_SUFFIX}.dll
"

FLAC_PLUGIN_FILES="
libFLAC.dll
libmp3splt/plugins/.libs/libsplt_flac${DLL_SUFFIX}.dll
"

MP3SPLT_DOC_FILES="
mp3splt-gtk/README
mp3splt-gtk/COPYING
mp3splt-gtk/ChangeLog
mp3splt-gtk/INSTALL
mp3splt-gtk/NEWS
mp3splt-gtk/TODO
mp3splt-gtk/AUTHORS
"

LIBMP3SPLT_DOC_FILES="
libmp3splt/README
libmp3splt/COPYING
libmp3splt/ChangeLog
libmp3splt/INSTALL
libmp3splt/NEWS
libmp3splt/TODO
libmp3splt/AUTHORS
"

MAIN_FILES="$BASIC_FILES $MP3_PLUGIN_FILES $OGG_PLUGIN_FILES $FLAC_PLUGIN_FILES"

TEMP_DIR="mp3splt-gtk_${MP3SPLT_GTK_VERSION}_$ARCH"

cd ../..

mkdir -p $TEMP_DIR/libmp3splt_doc
mkdir -p $TEMP_DIR/mp3splt-gtk_doc
mkdir -p $TEMP_DIR/translations

for file in "$MAIN_FILES";do
  cp $file $TEMP_DIR
done

for lib_doc_file in "$LIBMP3SPLT_DOC_FILES";do
  cp $lib_doc_file $TEMP_DIR/libmp3splt_doc
done

for doc_file in "$MP3SPLT_DOC_FILES";do
  cp $doc_file $TEMP_DIR/mp3splt-gtk_doc
done

for lang in $LANGUAGES;do
  cp -r translations/$lang/translations/$lang $TEMP_DIR/translations
done

cp -r ../libs/libmp3splt_libs_licenses $TEMP_DIR
cp -r ../libs/mp3splt-gtk_libs_licenses $TEMP_DIR

if [[ ! -z $we_dont_cross_compile ]];then
  cd ../libs &&\
  tar jxf mp3splt-gtk_runtime.tar.bz2 -C ../trunk || exit 1 &&\
  cd - &> /dev/null
fi

cp -r mp3splt-gtk_runtime/* $TEMP_DIR

rm -f $TEMP_DIR.zip
zip -r $TEMP_DIR.zip $TEMP_DIR

rm -rf mp3splt-gtk_runtime
rm -rf $TEMP_DIR

