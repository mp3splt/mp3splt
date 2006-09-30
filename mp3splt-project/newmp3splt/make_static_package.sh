#!/bin/bash

################# variables to set ############

ARCH=i386
MP3SPLT_VERSION=2.2_rc1
MP3SPLT_DOC_FILES=(AUTHORS ChangeLog COPYING INSTALL NEWS README TODO)

################# end variables to set ############

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir
cd $PROGRAM_DIR

#we put the flags, to find out libmp3splt headers and libraries
export CFLAGS="-I/tmp/static_tmp/libmp3splt/usr/local/include $CFLAGS"
export LDFLAGS="-L/tmp/static_tmp/libmp3splt/usr/local/lib $LDFLAGS"

#we create the /tmp directories
STATIC_DIR=/tmp/static_tmp/mp3splt
rm -rf $STATIC_DIR/*
mkdir -p $STATIC_DIR
mkdir -p $STATIC_DIR/usr/local/share/doc/mp3splt

#we compile
./autogen.sh && \
./configure --disable-shared --enable-static && \
make clean && \
make DESTDIR=$STATIC_DIR install && \
cp "${MP3SPLT_DOC_FILES[@]}" $STATIC_DIR/usr/local/share/doc/mp3splt &&\
tar -c -z -C $STATIC_DIR -f mp3splt-${MP3SPLT_VERSION}_static_$ARCH.tar.gz . &&\
mv mp3splt*.tar.gz ../
