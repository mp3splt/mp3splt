#!/bin/bash

#we move in the current script directory
script_dir=$(readlink -f $0) || exit 1
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir/..
cd $PROGRAM_DIR

. ./include_variables.sh

if [[ $ARCH = "x86_64" ]];then
    ARCH="amd64";
fi

put_package "debian"

#we generate the debian files
./debian/generate_debian_files.sh || exit 1

DEBIAN_VERSION=$(sed -n "s/.*(\(${LIBMP3SPLT_VERSION}.*\)).*/\1/ p" ./debian/changelog)
#if we don't have the distribution file
DIST_FILE="../libmp3splt_${DEBIAN_VERSION}_${ARCH}.deb"
if [[ ! -f $DIST_FILE ]];then
    #we compile
    ./autogen.sh && \
        ./configure --prefix=/usr --with-ltdl-lib=/usr/lib --with-ltdl-include=/usr/include && \
        sed -i 's/ECHO=/echo=/g' libtool && \
        make clean && \
        make && \
        #we create the debian package
    fakeroot debian/rules binary &&\
        #we install for mp3splt and mp3splt-gtk
    make install DESTDIR=/tmp/temp || exit 1
else
    put_is_package_warning "We already have the $DIST_FILE distribution file !"
fi

