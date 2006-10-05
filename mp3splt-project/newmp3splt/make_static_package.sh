#!/bin/bash

#we move in the current script directory
script_dir=$(readlink -f $0) || exit 1
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
STATIC_DIR=/tmp/static_tmp/mp3splt
DATEMV=`date +-%d_%m_%Y__%H_%M_%S`
if [[ -e $STATIC_DIR ]];then
    mv $STATIC_DIR ${STATIC_DIR}${DATEMV}
fi
mkdir -p $STATIC_DIR
mkdir -p $STATIC_DIR/usr/local/share/doc/mp3splt

#we compile
./autogen.sh && \
./configure --disable-shared --enable-static && \
make clean && \
make DESTDIR=$STATIC_DIR install && \
cp "${MP3SPLT_DOC_FILES[@]}" $STATIC_DIR/usr/local/share/doc/mp3splt &&\
tar -c -z -C $STATIC_DIR -f mp3splt-${MP3SPLT_VERSION}_static_$ARCH.tar.gz . &&\
mv mp3splt*.tar.gz ../ || exit 1
