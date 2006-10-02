#!/bin/bash

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir/..
cd $PROGRAM_DIR

. ./include_variables.sh

echo
echo $'Package :\tgentoo'
echo

#generate mp3splt ebuild
cd gentoo && \
echo "# Copyright 1999-2006 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# \$Header:  \$

# THIS FILE IS NOT SUPPORTED BY GENTOO, CAN HARM AND BREAK YOUR SYSTEM FOR GOOD!
# THERE IS NO GUARANTEE OR WARRANTY OF ANY KIND USE IT ON YOUR OWN RISK!
#
# e-build by Miklos Bagi Jr. <mbjr@mbjr.hu>
# modified by Munteanu Alexandru Ionut <io_alex_2002@yahoo.fr>

DESCRIPTION=\"Mp3splt is a command line program to cut mp3 and ogg without decoding. It uses libmp3splt.\"
HOMEPAGE=\"http://mp3splt.sourceforge.net/\"
SRC_URI=\"mirror://sourceforge/mp3splt/\${P}.tar.gz\"

LICENSE=\"GPL-2\"
SLOT=\"0\"
KEYWORDS=\"~x86 ~amd64\"
#we can put the use tags for vorbis and id3tag if we want
IUSE=\"\"

DEPEND=\"=media-libs/libmp3splt-${LIBMP3SPLT_VERSION}\"

src_compile() {
        econf --enable-oggsplt || die \"configure failed\"
        emake || die \"compilation failed\"
}

src_install() {
    einstall
    dodoc ${MP3SPLT_DOC_FILES[@]}
}" > ./media-sound/mp3splt/mp3splt-${MP3SPLT_VERSION}.ebuild \
&& cd ..

#create the directories we need
GENTOO_TEMP=/tmp/gentoo_temp
if [[ -d $GENTOO_TEMP ]];then mv $GENTOO_TEMP ${GENTOO_TEMP}_old;fi
mkdir -p $GENTOO_TEMP

#the ebuild
cp -a gentoo/* $GENTOO_TEMP
find $GENTOO_TEMP -name \".svn\" -exec rm -rf '{}' \; &>/dev/null
#digest mp3splt
#if we don't have distribution file, create it
if [[ ! -e ../mp3splt-$MP3SPLT_VERSION.tar.gz ]];then
    ./make_source_package.sh
fi && \
cp ../mp3splt-${MP3SPLT_VERSION}.tar.gz /usr/portage/distfiles &&\
ebuild $GENTOO_TEMP/media-sound/mp3splt/mp3splt* digest &&\
tar czf ../mp3splt-${MP3SPLT_VERSION}_ebuild.tar.gz $GENTOO_TEMP/media-sound;
