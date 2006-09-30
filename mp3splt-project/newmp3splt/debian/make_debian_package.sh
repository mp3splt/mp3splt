#!/bin/bash

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir/../
cd $PROGRAM_DIR

#set necessary flags
export CFLAGS="-I/tmp/temp/usr/include $CFLAGS"
export LDFLAGS="-L/tmp/temp/usr/lib $LDFLAGS"

#we compile
./autogen.sh && \
./configure --prefix=/usr && \
make clean && \
make && \
#we create the debian package
fakeroot debian/rules binary
