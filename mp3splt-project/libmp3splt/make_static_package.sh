#!/bin/bash

################# variables to set ############

ARCH=i386;
LIBMP3SPLT_VERSION=0.4_rc1;
LIBMP3SPLT_DOC_FILES=(AUTHORS ChangeLog COPYING INSTALL NEWS README TODO LIMITS)

################# end variables to set ############

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir
cd $PROGRAM_DIR

#create the directories that we need
STATIC_DIR=/tmp/static_tmp/libmp3splt
rm -rf $STATIC_DIR/*
mkdir -p $STATIC_DIR
mkdir -p $STATIC_DIR/usr/local/share/doc/libmp3splt

#we compile
./autogen.sh && \
./configure --disable-shared --enable-static && \
make clean && \
make DESTDIR=$STATIC_DIR install && \
cp "${LIBMP3SPLT_DOC_FILES[@]}" $STATIC_DIR/usr/local/share/doc/libmp3splt &&\
tar -c -z -C $STATIC_DIR -f libmp3splt-${LIBMP3SPLT_VERSION}_static_$ARCH.tar.gz . &&\
mv libmp3splt*.tar.gz ../

#we install libmp3splt shared libs too for mp3splt and mp3splt-gtk
#configure scripts
./configure --enable-shared --enable-static && make clean && make &&\
make DESTDIR=$STATIC_DIR install
