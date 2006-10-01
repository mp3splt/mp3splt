#!/bin/bash

################# variables to set ############

MP3SPLT_VERSION=2.2_rc1

################# end variables to set ############

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir/..
cd $PROGRAM_DIR

#we set the flags to find libmp3splt
export CFLAGS="-I../libmp3splt/arch/pkg/usr/include $CFLAGS"
export LDFLAGS="-L../libmp3splt/arch/pkg/usr/lib $LDFLAGS"

#we make the distribution file if we don't have it
if [[ ! -e ../mp3splt-${MP3SPLT_VERSION}.tar.gz ]];then
    ./make_source_package.sh
fi &&\
cp ../mp3splt-${MP3SPLT_VERSION}.tar.gz ./arch &&\
cd arch && makepkg -d -c &&\
mv mp3splt*pkg.tar.gz ../.. &&\
rm -rf ./mp3splt-${MP3SPLT_VERSION}.tar.gz
