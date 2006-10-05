#!/bin/bash

#we move in the current script directory
script_dir=$(readlink -f $0) || exit 1
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir/..
cd $PROGRAM_DIR

. ./include_variables.sh

echo
echo $'Package :\tgentoo'
echo

#generate mp3splt-gtk ebuild
cd gentoo && \
echo "# Copyright 1999-2006 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# \$Header:  \$

# THIS FILE IS NOT SUPPORTED BY GENTOO, CAN HARM AND BREAK YOUR SYSTEM FOR GOOD!
# THERE IS NO GUARANTEE OR WARRANTY OF ANY KIND USE IT ON YOUR OWN RISK!
#
# e-build by Miklos Bagi Jr. <mbjr@mbjr.hu>
# modified by Munteanu Alexandru Ionut <io_alex_2002@yahoo.fr>

DESCRIPTION=\"Mp3splt-gtk is a GTK2 gui that uses libmp3splt to split mp3 and ogg files.\"
HOMEPAGE=\"http://mp3splt.sourceforge.net/\"
SRC_URI=\"mirror://sourceforge/mp3splt/\${P}.tar.gz\"

LICENSE=\"GPL-2\"
SLOT=\"0\"
KEYWORDS=\"~x86 ~amd64\"
#audacious should be enabled in the future sessions
#IUSE=\"audacious\"
#audacious? ( >=media-sound/audacious-1.1.0 )
IUSE=\"\"

DEPEND=\"=media-libs/libmp3splt-${LIBMP3SPLT_VERSION}
        >=x11-libs/gtk+-2\"

src_compile() {
#if we use audacious, enable audacious support
#    if use audacious; then
#        econf --enable-bmp || die \"configuration failed\"
#    else
    econf || die \"configuration failed\"
#    fi;
    emake || die \"compilation failed\"
}

src_install() {
	einstall
    dodoc AUTHORS ChangeLog COPYING INSTALL NEWS README TODO
}" > ./media-sound/mp3splt-gtk/mp3splt-gtk-${MP3SPLT_GTK_VERSION}.ebuild  || exit 1\
&& cd ..

#create the directories we need
GENTOO_TEMP=/tmp/gentoo_temp
DATEMV=`date +-%d_%m_%Y__%H_%M_%S`
if [[ -e $GENTOO_TEMP ]];then
    mv $GENTOO_TEMP ${GENTOO_TEMP}${DATEMV}
fi
mkdir -p $GENTOO_TEMP

#the ebuild
cp -a gentoo/* $GENTOO_TEMP
find $GENTOO_TEMP -name \".svn\" -exec rm -rf '{}' \; &>/dev/null
#digest mp3splt-gtk
#if we don't have distribution file, create it
if [[ ! -e ../mp3splt-gtk-$MP3SPLT_GTK_VERSION.tar.gz ]];then
    ./make_source_package.sh || exit 1
fi && \
cp ../mp3splt-gtk-${MP3SPLT_GTK_VERSION}.tar.gz /usr/portage/distfiles &&\
ebuild $GENTOO_TEMP/media-sound/mp3splt-gtk/mp3splt-gtk* digest &&\
tar czf ../mp3splt-gtk-${MP3SPLT_GTK_VERSION}_ebuild.tar.gz $GENTOO_TEMP/media-sound || exit 1
