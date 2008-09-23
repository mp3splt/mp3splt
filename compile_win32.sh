#!/bin/sh

#we move in the current script directory
#!readlink not in mingw
#script_dir=$(readlink -f $0)
#script_dir=${script_dir%\/*.sh}
#cd $script_dir/

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

#installer for the gui
cd mp3splt-gtk/windows/installer
cp win32_installer.nsi win32_installer.nsi_old
cat win32_installer.nsi | sed s+!define\ MP3SPLT_PATH.*+\!define\ MP3SPLT_PATH\ c:/mp3splt_mingw`pwd`/../../..+ > win32_installer.nsi2
mv win32_installer.nsi2 win32_installer.nsi
cd ../../..
#installer for the text mode
cd newmp3splt/windows/installer
cp win32_installer.nsi win32_installer.nsi_old
cat win32_installer.nsi | sed s+!define\ MP3SPLT_PATH.*+\!define\ MP3SPLT_PATH\ c:/mp3splt_mingw`pwd`/../../..+ > win32_installer.nsi2
mv win32_installer.nsi2 win32_installer.nsi
cd ../../..

make -f Makefile.win32 dist
