#!/bin/bash

#################
#set variables MINGW_HOST 

HOST=${MINGW_HOST:-"i586-mingw32msvc"}
export CC=${HOST}"-gcc"

#################

#we move in the current script directory
script_dir=$(readlink -f $0) || exit 1
script_dir=${script_dir%\/*.sh}
cd $script_dir/../..

. ./libmp3splt/include_variables.sh || exit 1 

put_package "cross_windows"

#we run autoconf and automake..
cd libmp3splt && ./autogen.sh && cd .. || exit 1

#untar and copy the required libraries
cd ../libs
tar jxf libmp3splt_mingw_required_libs.tar.bz2 || exit 1

#hack
sed -i 's/-luuid//' lib/libltdl.la

#
cp bin/libltdl-7.dll ../trunk/ || exit 1
cp bin/libogg-0.dll ../trunk/ || exit 1
cp lib/libFLAC*.dll ../trunk/ || exit 1
cp bin/libvorbis-0.dll ../trunk/ || exit 1
cp bin/libvorbisenc-2.dll ../trunk/ || exit 1
cp bin/libvorbisfile-3.dll ../trunk/ || exit 1
cp bin/libmad-0.dll ../trunk/ || exit 1
cp bin/libid3tag.dll ../trunk/ || exit 1
cp bin/zlib1.dll ../trunk/ || exit 1
cp bin/pcre3.dll ../trunk/ || exit 1
cp bin/pcreposix3.dll ../trunk/ || exit 1
#
cp lib/libz.a ../trunk/libmp3splt/ || exit 1
cp lib/libz.a ../trunk/libmp3splt/src/ || exit 1
cd ..

#cross compile flags
export CFLAGS="-mms-bitfields -enable-stdcall-fixup -I`pwd`/libs/include -D_WIN32 -D__MINGW32__ $CFLAGS"
export LDFLAGS="-L`pwd`/libs/lib $LDFLAGS"
export PKG_CONFIG_PATH="`pwd`/libs/lib/pkgconfig"
export PATH="`pwd`/libs/bin:$PATH"

#modify pkg-config files path
TARGET=`pwd`/libs
for f in `pwd`/libs/lib/pkgconfig/*.pc ; do
  cat $f | sed s+^prefix=.*+prefix=$TARGET+ > $f.tmp
  mv $f.tmp $f
done  

make -C trunk/libmp3splt/libltdl

#we compile and install libmp3splt
cd trunk/libmp3splt &&\
    ./configure --enable-optimise --prefix=`pwd`/../../libs --host=$HOST --target=$HOST --disable-cutter --disable-oggtest --disable-vorbistest\
    && make clean && make && make install || exit 1

