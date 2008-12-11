#!/bin/bash

#we move in the current script directory
script_dir=$(readlink -f $0) || exit 1
script_dir=${script_dir%\/*.sh}
cd $script_dir/installer

. ../../include_variables.sh

put_package "cross_windows_installer"

NSIS_INSTALLER_SCRIPT="win32_cross_installer.nsi"

#we put the correct installer path
cp $NSIS_INSTALLER_SCRIPT ${NSIS_INSTALLER_SCRIPT}_old
cat $NSIS_INSTALLER_SCRIPT | sed s+.define\ MP3SPLT_PATH.*+\!define\ MP3SPLT_PATH\ `pwd`/../../..+ > ${NSIS_INSTALLER_SCRIPT}2
mv ${NSIS_INSTALLER_SCRIPT}2 $NSIS_INSTALLER_SCRIPT

#we create the installer
make -f Makefile cross || exit 1
##mv mp3splt_${MP3SPLT_VERSION}.exe ../../../mp3splt_${MP3SPLT_VERSION}_${ARCH}.exe || exit 1

#put the old installer file of mp3splt
##mv ${NSIS_INSTALLER_SCRIPT}_old $NSIS_INSTALLER_SCRIPT

