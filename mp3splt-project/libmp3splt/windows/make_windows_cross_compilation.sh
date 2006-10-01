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

. ./libmp3splt/include_variables.sh

echo
echo $'Package :\tcross_windows'
echo

#we run autoconf and automake..
cd libmp3splt && ./autogen.sh && cd .. || exit 1

#untar and copy the required libraries
cd ../libs
tar jxf libmp3splt_mingw_required_libs.tar.bz2
cp lib/libz.a ../mp3splt-project/libmp3splt/
cp lib/libz.a ../mp3splt-project/libmp3splt/src/
cd ..

#cross compile flags
export CFLAGS="-mms-bitfields -enable-stdcall-fixup -I`pwd`/libs/include -D_WIN32 -D__MINGW32__ $CFLAGS"
export LDFLAGS="-L`pwd`/libs/lib"
export PKG_CONFIG_PATH="`pwd`/libs/lib/pkgconfig"
export PATH="`pwd`/libs/bin:$PATH"

#we compile and install libmp3splt
cd mp3splt-project/libmp3splt &&\
./configure --prefix=`pwd`/../../libs --host=mingw32 --disable-shared --disable-vorbistest\
&& make clean && make && make install
