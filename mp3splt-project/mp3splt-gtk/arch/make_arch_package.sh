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

#we set the flags to find libmp3splt
export CFLAGS="-I../libmp3splt/arch/pkg/usr/include $CFLAGS"
export LDFLAGS="-L../libmp3splt/arch/pkg/usr/lib $LDFLAGS"

#we make the distribution file if we don't have it
if [[ ! -e ../mp3splt-gtk-${MP3SPLT_GTK_VERSION}.tar.gz ]];then
    ./make_source_package.sh
fi &&\
cp ../mp3splt-gtk-${MP3SPLT_GTK_VERSION}.tar.gz ./arch &&\
cd arch && makepkg -d -c &&\
mv mp3splt-gtk-${MP3SPLT_GTK_VERSION}-1.pkg.tar.gz \
../../mp3splt-gtk-${MP3SPLT_GTK_VERSION}-1_${ARCH}.pkg.tar.gz &&\
rm -rf ./mp3splt-gtk-${MP3SPLT_GTK_VERSION}.tar.gz
