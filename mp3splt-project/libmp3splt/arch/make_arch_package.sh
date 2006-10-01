#!/bin/bash

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir/..
cd $PROGRAM_DIR

. ./include_variables.sh

echo
echo $'Package :\tarch'
echo

if [[ $ARCH = "i386" ]];then
    ARCH=i686
fi

#we make the distribution file if we don't have it
if [[ ! -e ../libmp3splt-${LIBMP3SPLT_VERSION}.tar.gz ]];then
    ./make_source_package.sh
fi &&\
cd $PROGRAM_DIR &&\
cp ../libmp3splt-${LIBMP3SPLT_VERSION}.tar.gz ./arch &&\
cd arch && makepkg &&\
mv libmp3splt-${LIBMP3SPLT_VERSION}-1.pkg.tar.gz \
../../libmp3splt-${LIBMP3SPLT_VERSION}-1-$ARCH.pkg.tar.gz &&\
rm -rf ./libmp3splt-${LIBMP3SPLT_VERSION}.tar.gz
