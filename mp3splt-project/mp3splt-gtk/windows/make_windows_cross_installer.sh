#!/bin/bash

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
cd $script_dir/../../

#we compile the locales
rm -rf fr_locales
mkdir fr_locales
wine `pwd`/../libs/bin/msgfmt -o ./fr_locales/mp3splt-gtk.mo ./mp3splt-gtk/po/fr.po

cd ../libs &&\
tar jxf mp3splt-gtk_runtime.tar.bz2 -C ../mp3splt-project &&\
cd ../mp3splt-project

cd mp3splt-gtk/other
#we put the correct installer path
cp win32_installer.nsi win32_installer.nsi_old
cat win32_installer.nsi | sed s+.define\ MP3SPLT_PATH.*+\!define\ MP3SPLT_PATH\ `pwd`/../..+ > win32_installer.nsi2
mv win32_installer.nsi2 win32_installer.nsi

#we create the installer
make -f Makefile cross
mv mp3splt-gtk*exe ../..

#put the old installer file of mp3splt
mv win32_installer.nsi_old win32_installer.nsi

#remove used dirs
cd ../.. && rm -rf fr_locales && rm -rf mp3splt-gtk_runtime
