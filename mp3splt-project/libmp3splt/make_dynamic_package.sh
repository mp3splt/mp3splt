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

#create the directories we need
DYNAMIC_DIR=/tmp/dynamic_tmp/libmp3splt
rm -rf $DYNAMIC_DIR/*
mkdir -p $DYNAMIC_DIR
mkdir -p $DYNAMIC_DIR/usr/local/share/doc/libmp3splt

#we compile
./autogen.sh && \
./configure --enable-shared --disable-static && \
make clean && \
make && \
make DESTDIR=$DYNAMIC_DIR install && \
cp "${LIBMP3SPLT_DOC_FILES[@]}" $DYNAMIC_DIR/usr/local/share/doc/libmp3splt &&\
tar -c -z -C $DYNAMIC_DIR -f libmp3splt-${LIBMP3SPLT_VERSION}_dynamic_$ARCH.tar.gz . &&\
mv libmp3splt*dynamic*.tar.gz ../
