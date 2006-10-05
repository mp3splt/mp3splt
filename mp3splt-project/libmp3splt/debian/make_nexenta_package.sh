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

#we generate the debian files
./debian/generate_debian_files.sh || exit 1

TEMP_DIR=/tmp/temp
DATEMV=`date +-%d_%m_%Y__%H_%M_%S`
if [[ -e $TEMP_DIR ]];then
    mv $TEMP_DIR ${TEMP_DIR}${DATEMV}
fi
mkdir -p $TEMP_DIR

#we compile
./autogen.sh &&\
./configure --prefix=/usr --host=i386-pc-solaris2.11 &&\
make clean &&\
make &&\
make install DESTDIR=$TEMP_DIR &&\
#we create the debian package
fakeroot debian/rules binary || exit 1
