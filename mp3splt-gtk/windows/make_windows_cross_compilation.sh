#!/bin/bash

#################
#set variables MINGW_HOST 

HOST=${MINGW_HOST:-"i686-mingw32"}
export CC=${HOST}"-gcc"

#################

#we move in the current script directory
script_dir=$(readlink -f $0) || exit 1
script_dir=${script_dir%\/*.sh}
cd $script_dir/../..

. ./mp3splt-gtk/include_variables.sh || exit 1

put_package "cross_windows"

#we run autoconf and automake..
cd mp3splt-gtk && ./autogen.sh || exit 1 && cd ..

#untar and copy the required libraries
cd ../libs
tar jxf mp3splt-gtk_mingw_required_libs.tar.bz2 || exit 1
tar jxf mp3splt-gtk_runtime.tar.bz2 || exit 1
cp -a mp3splt-gtk_runtime/*.dll ./bin || exit 1
cp lib/libmad.a lib/libvorbis.a lib/libogg.a lib/libid3tag.a lib/libz.a \
    lib/libvorbisfile.a ../trunk/mp3splt-gtk/ || exit 1
cp lib/libmad.a lib/libvorbis.a lib/libogg.a lib/libid3tag.a lib/libz.a \
    lib/libvorbisfile.a ../trunk/mp3splt-gtk/src || exit 1
cd ..

#cross compile flags
export CFLAGS=" -mms-bitfields -enable-stdcall-fixup -I`pwd`/libs/include -D_WIN32 -D__MINGW32__ $CFLAGS"
export LDFLAGS="-L`pwd`/libs/lib $LDFLAGS"
export PKG_CONFIG_PATH="`pwd`/libs/lib/pkgconfig"
export PATH="`pwd`/libs/bin:$PATH"

#modify pkg-config files path
TARGET=`pwd`/libs
for f in `pwd`/libs/lib/pkgconfig/*.pc ; do
  cat $f | sed s+^prefix=.*+prefix=$TARGET+ > $f.tmp
  mv $f.tmp $f
done  

#we compile mp3splt-gtk
cd trunk/mp3splt-gtk &&\
cp ../../libs/lib/libmp3splt.a . && cp ../../libs/lib/libmp3splt.a ./src &&\
./configure --disable-audacious --prefix=`pwd`/../../libs --host=$HOST --disable-gtktest &&\
make clean && make && make install &&\
${HOST}-strip ./src/mp3splt-gtk.exe || exit 1

