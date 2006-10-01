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

#we put the flags, to find out libmp3splt headers and libraries
export CFLAGS="-I/tmp/static_tmp/libmp3splt/usr/local/include $CFLAGS"
export LDFLAGS="-L/tmp/static_tmp/libmp3splt/usr/local/lib $LDFLAGS"

#we create the /tmp directories
STATIC_DIR=/tmp/static_tmp/mp3splt-gtk
if [[ -d $STATIC_DIR ]];then mv $STATIC_DIR ${STATIC_DIR}_old;fi
mkdir -p $STATIC_DIR
mkdir -p $STATIC_DIR/usr/local/share/doc/mp3splt-gtk

#we compile
./autogen.sh && \
./configure --enable-bmp --disable-shared --enable-static && \
make clean && \
make DESTDIR=$STATIC_DIR install && \
cp "${MP3SPLT_GTK_DOC_FILES[@]}" $STATIC_DIR/usr/local/share/doc/mp3splt-gtk &&\
tar -c -z -C $STATIC_DIR -f mp3splt-gtk-${MP3SPLT_GTK_VERSION}_static_$ARCH.tar.gz . &&\
mv mp3splt-gtk*.tar.gz ../
