#!/bin/bash

#we move in the current script directory
script_dir=$(readlink -f $0) || exit 1
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir/..
cd $PROGRAM_DIR

. ./include_variables.sh

echo
echo $'Package :\tnexenta'
echo

./debian/generate_debian_files.sh || exit 1

#if we don't have the distribution file
DIST_FILE="../mp3splt_${MP3SPLT_VERSION}_solaris-${ARCH}.deb"
if [[ ! -f $DIST_FILE ]];then
    TEMP_DIR=/tmp/temp

    #we set necessary flags
    export CFLAGS="-I/$TEMP_DIR/usr/include $CFLAGS"
    export LDFLAGS="-L/$TEMP_DIR/usr/lib $LDFLAGS"
    
    #we compile
    ./autogen.sh &&\
        ./configure --prefix=/usr --host=i386-pc-solaris2.11 &&\
        make clean &&\
        make &&\
        #we create the debian package
    fakeroot debian/rules binary || exit 1
else
    echo
    echo "We already have the $DIST_FILE distribution file !"
    echo
fi