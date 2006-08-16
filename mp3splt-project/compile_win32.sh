#!/usr/bin/env sh

tar jxf ../libmp3splt_mingw_required_libs.tar.bz2 -C /
cp /usr/bin/mingw32-make.exe /usr/bin/make.exe
tar jxf ../mp3splt-gtk_mingw_required_libs.tar.bz2 -C /
tar jxf ../mp3splt-gtk_runtime.tar.bz2
cp -a ./mp3splt-gtk_runtime/*.dll /usr/bin

#copy libraries (for static linking)
#mp3splt-gtk configure dir
cp /usr/lib/libz.a /usr/lib/libvorbisfile.a /usr/lib/libmad.a \
/usr/lib/libvorbis.a /usr/lib/libogg.a /usr/lib/libid3tag.a ./mp3splt-gtk
#mp3splt-gtk source dir
cp /usr/lib/libz.a /usr/lib/libvorbisfile.a /usr/lib/libmad.a \
/usr/lib/libvorbis.a /usr/lib/libogg.a /usr/lib/libid3tag.a ./mp3splt-gtk/src
#newmp3splt configure dir
cp /usr/lib/libz.a /usr/lib/libvorbisfile.a /usr/lib/libmad.a \
/usr/lib/libvorbis.a /usr/lib/libogg.a /usr/lib/libid3tag.a ./newmp3splt
#newmp3splt source dir
cp /usr/lib/libz.a /usr/lib/libvorbisfile.a /usr/lib/libmad.a \
/usr/lib/libvorbis.a /usr/lib/libogg.a /usr/lib/libid3tag.a ./newmp3splt/src
#libz for libmp3splt
cp /usr/lib/libz.a ./libmp3splt
cp /usr/lib/libz.a ./libmp3splt/src

export PKG_CONFIG_PATH="/usr/lib/pkgconfig"
export CFLAGS="-mms-bitfields -enable-stdcall-fixup $CFLAGS"

make -f Makefile.win32


#installer for the gui
cd mp3splt-gtk/other
cp win32_installer.nsi win32_installer.nsi_old
cat win32_installer.nsi | sed s+!define\ MP3SPLT_PATH.*+\!define\ MP3SPLT_PATH\ c:/mp3splt_mingw`pwd`/../..+ > win32_installer.nsi2
mv win32_installer.nsi2 win32_installer.nsi
cd ../..
#installer for the text mode
cd newmp3splt/other
cp win32_installer.nsi win32_installer.nsi_old
cat win32_installer.nsi | sed s+!define\ MP3SPLT_PATH.*+\!define\ MP3SPLT_PATH\ c:/mp3splt_mingw`pwd`/../..+ > win32_installer.nsi2
mv win32_installer.nsi2 win32_installer.nsi
cd ../..

make -f Makefile.win32 dist