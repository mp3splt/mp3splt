#!/usr/bin/env bash

#compiler settings
#Debian GNU/Linux settings
export CC="i586-mingw32msvc-gcc"
export PATH=/usr/i586-mingw32msvc/bin/:$PATH

#prepare for building..
cd ../libs 
tar jxf libmp3splt_mingw_required_libs.tar.bz2
tar jxf mp3splt-gtk_mingw_required_libs.tar.bz2
tar jxf mp3splt-gtk_runtime.tar.bz2
tar jxf mp3splt-gtk_runtime.tar.bz2 -C ../mp3splt-project
cp -a mp3splt-gtk_runtime/*.dll ./bin
cp lib/libz.a ../mp3splt-project/libmp3splt/
cp lib/libz.a ../mp3splt-project/libmp3splt/src/
cp lib/libmad.a lib/libvorbis.a lib/libogg.a lib/libid3tag.a lib/libz.a \
    lib/libvorbisfile.a ../mp3splt-project/mp3splt-gtk/
cp lib/libmad.a lib/libvorbis.a lib/libogg.a lib/libid3tag.a lib/libz.a \
    lib/libvorbisfile.a ../mp3splt-project/mp3splt-gtk/src
cp lib/libmad.a lib/libvorbis.a lib/libogg.a lib/libid3tag.a lib/libz.a \
    lib/libvorbisfile.a ../mp3splt-project/newmp3splt/
cp lib/libmad.a lib/libvorbis.a lib/libogg.a lib/libid3tag.a lib/libz.a \
    lib/libvorbisfile.a ../mp3splt-project/newmp3splt/src
cd ../mp3splt-project

cd libmp3splt && ./autogen.sh && cd ..
cd mp3splt-gtk && ./autogen.sh && cd ..

export CFLAGS="-I`pwd`/../libs/include -D_WIN32 -D__MINGW32__"
export CFLAGS="-mms-bitfields -enable-stdcall-fixup $CFLAGS"
export LDFLAGS="-L`pwd`/../libs/lib"
export PKG_CONFIG_PATH="`pwd`/../libs/lib/pkgconfig"
export PATH="`pwd`/../libs/bin:$PATH"

#modify .*/devel/target/ for pkg-config
TARGET=`pwd`/../libs
for f in `pwd`/../libs/lib/pkgconfig/*.pc ; do
   if grep 'prefix=.*/devel/target.*' $f >/dev/null 2>&1 ; then
     cat $f | sed s+^prefix=.*/devel/target.*+prefix=$TARGET+ > $f.tmp
     mv $f.tmp $f
   fi
done  
#modify /target for pkg-config
TARGET=`pwd`/../libs
for f in `pwd`/../libs/lib/pkgconfig/*.pc ; do
   if grep 'prefix=/target.*' $f >/dev/null 2>&1 ; then
     cat $f | sed s+^prefix=/target.*+prefix=$TARGET+ > $f.tmp
     mv $f.tmp $f
   fi
done  

#build libmp3splt
make -f Makefile.win32 cross_libmp3splt && \
#build mp3splt-gtk
make -f Makefile.win32 cross_mp3splt-gtk && \
mv ./mp3splt-gtk/src/mp3splt-gtk ./mp3splt-gtk/src/mp3splt-gtk.exe
#build newmp3splt
make -f Makefile.win32 cross_newmp3splt
mv ./newmp3splt/src/mp3splt ./newmp3splt/src/mp3splt.exe

#create installer
cd mp3splt-gtk/other
cp win32_installer.nsi win32_installer.nsi_old
cat win32_installer.nsi | sed s+.define\ MP3SPLT_PATH.*+\!define\ MP3SPLT_PATH\ `pwd`/../..+ > win32_installer.nsi2
mv win32_installer.nsi2 win32_installer.nsi
cd ../.. && make -f Makefile.win32 cross_dist && cd ./mp3splt-gtk/other
mv win32_installer.nsi_old win32_installer.nsi
cp mp3splt-gtk*exe ../..
cd ../..
rm -rf mp3splt-gtk_runtime
