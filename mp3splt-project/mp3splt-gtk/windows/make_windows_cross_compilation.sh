#!/bin/bash

################# variables to set ############

#Debian GNU/Linux settings
export CC="i586-mingw32msvc-gcc"
export PATH=/usr/i586-mingw32msvc/bin/:$PATH

################# end variables to set ############

#we move in the current script directory
script_dir=$(readlink -f $0) || exit 1
script_dir=${script_dir%\/*.sh}
cd $script_dir/../..

. ./mp3splt-gtk/include_variables.sh 

put_package "cross_windows"

#we run autoconf and automake..
cd mp3splt-gtk && ./autogen.sh || exit 1 && cd ..

#untar and copy the required libraries
cd ../libs
tar jxf mp3splt-gtk_mingw_required_libs.tar.bz2 || exit 1
tar jxf mp3splt-gtk_runtime.tar.bz2 || exit 1
cp -a mp3splt-gtk_runtime/*.dll ./bin || exit 1
cp lib/libmad.a lib/libvorbis.a lib/libogg.a lib/libid3tag.a lib/libz.a \
    lib/libvorbisfile.a ../mp3splt-project/mp3splt-gtk/ || exit 1
cp lib/libmad.a lib/libvorbis.a lib/libogg.a lib/libid3tag.a lib/libz.a \
    lib/libvorbisfile.a ../mp3splt-project/mp3splt-gtk/src || exit 1
cd ..

#cross compile flags
export CFLAGS=" -mms-bitfields -enable-stdcall-fixup -I`pwd`/libs/include -D_WIN32 -D__MINGW32__ $CFLAGS"
export LDFLAGS="-L`pwd`/libs/lib $LDFLAGS"
export PKG_CONFIG_PATH="`pwd`/libs/lib/pkgconfig"
export PATH="`pwd`/libs/bin:$PATH"

#modify .*/devel/target/ for pkg-config
TARGET=`pwd`/libs
for f in `pwd`/libs/lib/pkgconfig/*.pc ; do
   if grep 'prefix=.*/devel/target.*' $f >/dev/null 2>&1 ; then
     cat $f | sed s+^prefix=.*/devel/target.*+prefix=$TARGET+ > $f.tmp
     mv $f.tmp $f
   fi
done  
#modify /target for pkg-config
TARGET=`pwd`/libs
for f in `pwd`/libs/lib/pkgconfig/*.pc ; do
   if grep 'prefix=/target.*' $f >/dev/null 2>&1 ; then
     cat $f | sed s+^prefix=/target.*+prefix=$TARGET+ > $f.tmp
     mv $f.tmp $f
   fi
done

#we compile mp3splt-gtk
cd mp3splt-project/mp3splt-gtk &&\
cp ../../libs/lib/libmp3splt.a . && cp ../../libs/lib/libmp3splt.a ./src &&\
./configure --prefix=`pwd`/../../libs --host=mingw32 --disable-gtktest &&\
make clean && make &&\
strip ./src/mp3splt-gtk.exe || exit 1

#we clean the remained files
find . -type f -name "*.a" -exec rm -f {} \;
