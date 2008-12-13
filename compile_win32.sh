#!/bin/sh

echo "unarchiving libmp3splt required libs ..."
tar jxf ../libmp3splt_mingw_required_libs.tar.bz2 -C /
cp /usr/bin/mingw32-make.exe /usr/bin/make.exe
echo "unarchiving mp3splt-gtk required libs ..."
tar jxf ../mp3splt-gtk_mingw_required_libs.tar.bz2 -C /
echo "unarchiving mp3splt-gtk runtime ..."
tar jxf ../mp3splt-gtk_runtime.tar.bz2
cp -a ./mp3splt-gtk_runtime/*.dll /usr/bin
mkdir -p /lib/.libs
cp /lib/libvorbis* /lib/libmad* /lib/libid3tag* /lib/.libs
cp /bin/libvorbis* /bin/libmad* /bin/libid3tag* /lib/.libs

export PKG_CONFIG_PATH="/usr/lib/pkgconfig"

make -f Makefile.win32 || exit 1;
make -f Makefile.win32 dist || exit 1
