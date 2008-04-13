#!/bin/bash

#we move in the current script directory
script_dir=$(readlink -f $0) || exit 1
script_dir=${script_dir%\/*.sh}
cd $script_dir/installer

. ../../include_variables.sh

put_package "cross_windows_installer"

#we compile the locales
mv fr_locales fr_locales_old
mkdir -p fr_locales
wine `pwd`/../../../../libs/bin/msgfmt -o ./fr_locales/mp3splt-gtk.mo ../../po/fr.po || exit 1

cd ../../../../libs &&\
tar jxf mp3splt-gtk_runtime.tar.bz2 -C ../mp3splt-project || exit 1 &&\
cd ../mp3splt-project/mp3splt-gtk/windows/installer

#we put the correct installer path
cp win32_installer.nsi win32_installer.nsi_old
cat win32_installer.nsi | sed s+.define\ MP3SPLT_PATH.*+\!define\ MP3SPLT_PATH\ `pwd`/../../..+ > win32_installer.nsi2
mv win32_installer.nsi2 win32_installer.nsi

#we create the installer
make -f Makefile cross || exit 1
mv mp3splt-gtk_${MP3SPLT_GTK_VERSION}.exe ../../../mp3splt-gtk_${MP3SPLT_GTK_VERSION}_${ARCH}.exe || exit 1

#put the old installer file of mp3splt
mv win32_installer.nsi_old win32_installer.nsi

#erase the executable
rm -f ../../src/mp3splt-gtk.exe

#remove used dirs
rm -rf fr_locales && cd ../../.. && rm -rf mp3splt-gtk_runtime
