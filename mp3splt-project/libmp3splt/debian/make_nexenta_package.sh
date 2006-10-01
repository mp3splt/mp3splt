#!/bin/bash

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir/..
cd $PROGRAM_DIR

. ./include_variables.sh

echo
echo $'Package :\tnexenta'
echo

TEMP_DIR=/tmp/temp
if [[ -d $TEMP_DIR ]];then mv $TEMP_DIR ${TEMP_DIR}_old;fi
mkdir -p $TEMP_DIR

#we compile
./autogen.sh &&\
./configure --prefix=/usr --host=i386-pc-solaris2.11 &&\
make clean &&\
make &&\
make install DESTDIR=$TEMP_DIR &&\
#we create the debian package
fakeroot debian/rules binary
