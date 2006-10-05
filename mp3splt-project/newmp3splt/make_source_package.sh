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

. ./include_variables.sh

echo
echo $'Package :\tsource'
echo

#set necessary flags
export CFLAGS="-I/tmp/temp/usr/include $CFLAGS"
export LDFLAGS="-L/tmp/temp/usr/lib $LDFLAGS"

#we compile
./autogen.sh && \
./configure --prefix=/usr && \
make dist &&\
mv mp3splt*.tar.gz ../ || exit 1
