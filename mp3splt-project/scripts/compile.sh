#!/bin/bash

#program versions
LIBMP3SPLT_VERSION=0.4rc1;
MP3SPLT_VERSION=2.3rc1;
MP3SPLT_GTK_VERSION=0.4rc1;

DIST_VERSION=$LIBMP3SPLT_VERSION;

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
cd $script_dir/../

#we update versions
./scripts/update_version.sh libmp3splt $LIBMP3SPLT_VERSION $LIBMP3SPLT_VERSION
./scripts/update_version.sh mp3splt-gtk $MP3SPLT_GTK_VERSION $LIBMP3SPLT_VERSION
./scripts/update_version.sh mp3splt $MP3SPLT_VERSION $LIBMP3SPLT_VERSION

#we do the real compilation
mkdir temp
mkdir -p $DIST_VERSION
rm -rf $DIST_VERSION/*
export CFLAGS="-I`pwd`/temp/include"
export LDFLAGS="-L`pwd`/temp/lib"
make
mv ./mp3splt-gtk/mp3splt-gtk*tar.gz ./
mv ./libmp3splt/libmp3splt*tar.gz ./
rm -rf ./temp
./crosscompile_win32.sh
mv ./mp3splt-gtk*exe ./$DIST_VERSION
mv ./*.deb ./$DIST_VERSION
mv ./*.tar.gz ./$DIST_VERSION
mv ./mp3splt*exe ./$DIST_VERSION
mv ./newmp3splt/mp3splt*.tar.gz ./$DIST_VERSION
