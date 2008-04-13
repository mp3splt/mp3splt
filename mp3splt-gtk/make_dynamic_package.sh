#!/bin/bash

#we move in the current script directory
script_dir=$(readlink -f $0) || exit 1
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir
cd $PROGRAM_DIR

. ./include_variables.sh

put_package "linux_kernel_dynamic"

#if we don't have the distribution file
DIST_FILE="../mp3splt-gtk-${MP3SPLT_GTK_VERSION}_dynamic_${ARCH}.tar.gz"
if [[ ! -f $DIST_FILE ]];then
    #we put the flags, to find out libmp3splt headers and libraries
    export CFLAGS="-I/tmp/dynamic_tmp/libmp3splt/usr/local/include $CFLAGS"
    export LDFLAGS="-L/tmp/dynamic_tmp/libmp3splt/usr/local/lib $LDFLAGS"
    
    #create the directories we need
    DYNAMIC_DIR=/tmp/dynamic_tmp/mp3splt-gtk
    DATEMV=`date +-%d_%m_%Y__%H_%M_%S`
    if [[ -e $DYNAMIC_DIR ]];then
        mv $DYNAMIC_DIR ${DYNAMIC_DIR}${DATEMV}
    fi
    mkdir -p $DYNAMIC_DIR
    mkdir -p $DYNAMIC_DIR/usr/local/share/doc/mp3splt-gtk
    
    #we compile
    ./autogen.sh && \
        ./configure --enable-bmp --enable-shared --disable-static && \
        make clean && \
        make && \
        make DESTDIR=$DYNAMIC_DIR install && \
        cp "${MP3SPLT_GTK_DOC_FILES[@]}" $DYNAMIC_DIR/usr/local/share/doc/mp3splt-gtk &&\
        tar -c -z -C $DYNAMIC_DIR -f mp3splt-gtk-${MP3SPLT_GTK_VERSION}_dynamic_$ARCH.tar.gz . &&\
        mv mp3splt-gtk*dynamic*.tar.gz ../ || exit 1
else
    put_is_package_warning "We already have the $DIST_FILE distribution file !"
fi
