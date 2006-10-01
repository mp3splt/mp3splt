#!/bin/bash

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir/..
cd $PROGRAM_DIR

. ./include_variables.sh

echo
echo $'Package :\tgentoo'
echo

#create the directories we need
GENTOO_TEMP=/tmp/gentoo_temp
if [[ -d $GENTOO_TEMP ]];then mv $GENTOO_TEMP ${GENTOO_TEMP}_old;fi
mkdir -p $GENTOO_TEMP

#the ebuild
cp -a gentoo/* $GENTOO_TEMP
find $GENTOO_TEMP -name \".svn\" -exec rm -rf '{}' \; &>/dev/null
#digest mp3splt
#if we don't have distribution file, create it
if [[ ! -e ../mp3splt-$MP3SPLT_VERSION.tar.gz ]];then
    ./make_source_package.sh
fi && \
cp ../mp3splt-${MP3SPLT_VERSION}.tar.gz /usr/portage/distfiles &&\
ebuild $GENTOO_TEMP/media-sound/mp3splt/mp3splt* digest &&\
tar czf ../mp3splt-${MP3SPLT_VERSION}_ebuild.tar.gz $GENTOO_TEMP/media-sound;
