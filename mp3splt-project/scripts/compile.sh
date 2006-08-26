#!/bin/bash

#program versions
LIBMP3SPLT_VERSION=0.4rc1;
MP3SPLT_VERSION=2.3rc1;
MP3SPLT_GTK_VERSION=0.4rc1;

DIST_VERSION=$LIBMP3SPLT_VERSION;

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
cd $script_dir/../

#we update versions
./scripts/update_version.sh libmp3splt $LIBMP3SPLT_VERSION $LIBMP3SPLT_VERSION
./scripts/update_version.sh mp3splt-gtk $MP3SPLT_GTK_VERSION $LIBMP3SPLT_VERSION
./scripts/update_version.sh mp3splt $MP3SPLT_VERSION $LIBMP3SPLT_VERSION

#we do the real compilation of the distribution+debian packages
mkdir -p temp
mkdir -p $DIST_VERSION
rm -rf $DIST_VERSION/*
export CFLAGS="-I`pwd`/temp/include"
export LDFLAGS="-L`pwd`/temp/lib"
make
mv ./mp3splt-gtk/mp3splt-gtk*tar.gz ./
mv ./libmp3splt/libmp3splt*tar.gz ./
mv ./newmp3splt/mp3splt*tar.gz ./
rm -rf ./temp

#we do the windows executables
./crosscompile_win32.sh

############# ebuilds ################
#we do the ebuilds with gentoo in chroot /mnt/gentoo
mkdir -p gentoo_temp

#libmp3splt ebuild
cp -a ./libmp3splt/gentoo/* ./gentoo_temp;
find ./gentoo_temp -name \".svn\" -exec rm -rf '{}' &>/dev/null \;
#digest libmp3splt
dchroot -d -c gentoo "cd `pwd`;
cp *.tar.gz /usr/portage/distfiles;
ebuild ./gentoo_temp/media-libs/libmp3splt/libmp3splt* digest;"
tar czf libmp3splt-${LIBMP3SPLT_VERSION}_ebuild.tar.gz ./gentoo_temp/media-libs;
rm -rf gentoo_temp/*;

#mp3splt-gtk ebuild
cp -a ./mp3splt-gtk/gentoo/* ./gentoo_temp;
find ./gentoo_temp -name \".svn\" -exec rm -rf '{}' &>/dev/null \;
#digest mp3splt-gtk
dchroot -d -c gentoo "cd `pwd`;
ebuild ./gentoo_temp/media-sound/mp3splt-gtk/mp3splt* digest;"
tar czf mp3splt-gtk-${MP3SPLT_GTK_VERSION}_ebuild.tar.gz ./gentoo_temp/media-sound;
rm -rf gentoo_temp/*;

#mp3splt ebuild
cp -a ./newmp3splt/gentoo/* ./gentoo_temp;
find ./gentoo_temp -name \".svn\" -exec rm -rf '{}' &>/dev/null \;
#digest mp3splt-gtk
dchroot -d -c gentoo "cd `pwd`;
ebuild ./gentoo_temp/media-sound/mp3splt/mp3splt* digest;"
tar czf mp3splt-${MP3SPLT_VERSION}_ebuild.tar.gz ./gentoo_temp/media-sound;
rm -rf gentoo_temp/*;

#end ebuilds temp directory
rm -rf gentoo_temp;
############# end ebuilds ################

#copy packages to the new directory
mv ./*.exe ./$DIST_VERSION
mv ./*.deb ./$DIST_VERSION
mv ./*.tar.gz ./$DIST_VERSION
