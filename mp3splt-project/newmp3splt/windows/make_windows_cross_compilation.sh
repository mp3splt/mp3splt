#!/bin/bash

################# variables to set ############

#Debian GNU/Linux settings
export CC="i586-mingw32msvc-gcc"
export PATH=/usr/i586-mingw32msvc/bin/:$PATH

################# end variables to set ############

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
cd $script_dir/../..

#we run autoconf and automake..
cd newmp3splt && ./autogen.sh && cd .. || exit 1

#untar and copy the required libraries
cd ../libs
cp lib/libmad.a lib/libvorbis.a lib/libogg.a lib/libid3tag.a lib/libz.a \
    lib/libvorbisfile.a ../mp3splt-project/newmp3splt/
cp lib/libmad.a lib/libvorbis.a lib/libogg.a lib/libid3tag.a lib/libz.a \
    lib/libvorbisfile.a ../mp3splt-project/newmp3splt/src
cd ..

#cross compile flags
export CFLAGS=" -mms-bitfields -enable-stdcall-fixup -I`pwd`/libs/include -D_WIN32 -D__MINGW32__ $CFLAGS"
export LDFLAGS="-L`pwd`/libs/lib"
export PKG_CONFIG_PATH="`pwd`/libs/lib/pkgconfig"
export PATH="`pwd`/libs/bin:$PATH"

#we compile mp3splt
cd mp3splt-project/newmp3splt &&\
cp ../../libs/lib/libmp3splt.a . &&\
cp ../../libs/lib/libmp3splt.a ./src &&\
./configure --prefix=`pwd`/../../libs --host=--mingw32 &&\
make clean && make &&\
strip ./src/mp3splt.exe
