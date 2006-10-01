#!/bin/bash

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir
cd $PROGRAM_DIR

. ./include_variables.sh

echo
echo $'Package :\tstatic'
echo

#create the directories that we need
STATIC_DIR=/tmp/static_tmp/libmp3splt
if [[ -d $STATIC_DIR ]];then mv $STATIC_DIR ${STATIC_DIR}_old;fi
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
