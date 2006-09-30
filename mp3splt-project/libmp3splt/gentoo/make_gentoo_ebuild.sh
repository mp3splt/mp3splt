#!/bin/bash

################# variables to set ############

LIBMP3SPLT_VERSION=0.4_rc1;

################# end variables to set ############

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir/..
cd $PROGRAM_DIR

#create the directories we need
GENTOO_TEMP=/tmp/gentoo_temp
rm -rf $GENTOO_TEMP/*
mkdir -p $GENTOO_TEMP

#the ebuild
cp -a gentoo/* $GENTOO_TEMP;
find $GENTOO_TEMP -name \".svn\" -exec rm -rf '{}' \; &>/dev/null
#digest libmp3splt
#if we don't have distribution file, create it
if [[ ! -e ../libmp3splt-$LIBMP3SPLT_VERSION.tar.gz ]];then
    ./make_source_package.sh
fi && \
cp ../libmp3splt-${LIBMP3SPLT_VERSION}.tar.gz /usr/portage/distfiles &&\
ebuild $GENTOO_TEMP/media-libs/libmp3splt/libmp3splt* digest &&\
tar czf ../libmp3splt-${LIBMP3SPLT_VERSION}_ebuild.tar.gz $GENTOO_TEMP/media-libs;
