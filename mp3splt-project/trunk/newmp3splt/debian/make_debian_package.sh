#!/bin/bash

#move in the current script directory
script_dir=$(readlink -f $0) || exit 1
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir/..
cd $PROGRAM_DIR

. ./include_variables.sh || exit 1

if [[ $ARCH = "x86_64" ]];then
    ARCH="amd64";
fi

put_package "debian"

./debian/generate_debian_files.sh || exit 1

DEBIAN_VERSION=$(sed -n "s/^.*(\(${MP3SPLT_VERSION}.*\)).*/\1/ p" ./debian/changelog);

DIST_FILE="../mp3splt_${DEBIAN_VERSION}_${ARCH}.deb"
if [[ ! -f $DIST_FILE ]];then
    export CFLAGS="-I/tmp/temp/usr/include $CFLAGS" &&\
    export LDFLAGS="-L/tmp/temp/usr/lib $LDFLAGS" &&\
    sudo ldconfig &&\
    ./autogen.sh && ./configure --enable-optimise --prefix=/usr && make clean && \
    debuild -i -us -uc -b -d || exit 1
else
    put_is_package_warning "We already have the $DIST_FILE distribution file !"
fi
