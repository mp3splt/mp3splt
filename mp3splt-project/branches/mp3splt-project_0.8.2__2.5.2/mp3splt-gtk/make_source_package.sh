#!/bin/bash

#we move in the current script directory
if [[ $1 == "netbsd" ]];then
    script_dir=$(greadlink -f $0) || exit 1
else
    script_dir=$(readlink -f $0) || exit 1
fi;
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir
cd $PROGRAM_DIR

. ./include_variables.sh || exit 1

put_package "source"

#if we don't have the distribution file
DIST_FILE="../mp3splt-gtk-${MP3SPLT_GTK_VERSION}.tar.gz"
if [[ ! -f $DIST_FILE ]];then
    #set necessary flags
    export CFLAGS="-I/usr/include -I/usr/local/include -I/tmp/temp/usr/include $CFLAGS"
    export LDFLAGS="-L/usr/lib -L/usr/local/lib -L/tmp/temp/usr/lib $LDFLAGS"
    
    #we compile
    ./autogen.sh &&\
        ./configure --prefix=/usr &&\
        make clean && make distcheck && make dist &&\
        mv mp3splt-gtk*.tar.gz ../ || exit 1
else
    put_is_package_warning "We already have the $DIST_FILE distribution file !"
fi
