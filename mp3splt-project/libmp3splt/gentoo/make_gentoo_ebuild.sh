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

#if we don't have the distribution file
DIST_FILE="../libmp3splt-${LIBMP3SPLT_VERSION}_ebuild.tar.gz"
if [[ ! -f $DIST_FILE ]];then
    #generate libmp3splt ebuild
    cd gentoo && \
        echo "# Copyright 1999-2006 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# \$Header:  \$

# THIS FILE IS NOT SUPPORTED BY GENTOO, CAN HARM AND BREAK YOUR SYSTEM FOR GOOD!
# THERE IS NO GUARANTEE OR WARRANTY OF ANY KIND USE IT ON YOUR OWN RISK!
#
# e-build by Miklos Bagi Jr. <mbjr@mbjr.hu>
# modified by Munteanu Alexandru Ionut <io_alex_2002@yahoo.fr>

DESCRIPTION=\"Libmp3splt is a library created from mp3splt v2.1c to split mp3 and ogg files\"
HOMEPAGE=\"http://mp3splt.sourceforge.net/\"
SRC_URI=\"mirror://sourceforge/mp3splt/\${P}.tar.gz\"

LICENSE=\"GPL-2\"
SLOT=\"0\"
KEYWORDS=\"~x86 ~amd64\"
#we can put the use tags for vorbis and id3tag if we want
IUSE=\"\"

DEPEND=\"media-libs/libmad
        media-libs/libvorbis
        media-libs/libid3tag\"

src_compile() {
        econf || die \"configure failed\"
        emake || die \"compilation failed\"
}

src_install() {
    einstall
    dodoc ${LIBMP3SPLT_DOC_FILES[@]}
}" > ./media-libs/libmp3splt/libmp3splt-${LIBMP3SPLT_VERSION}.ebuild \
    && cd ..
    
#create the directories we need
    GENTOO_TEMP=/tmp/gentoo_temp
    DATEMV=`date +-%d_%m_%Y__%H_%M_%S`
    if [[ -e $GENTOO_TEMP ]];then
        mv $GENTOO_TEMP ${GENTOO_TEMP}${DATEMV}
    fi
    mkdir -p $GENTOO_TEMP
    
#the ebuild
    cp -a gentoo/* $GENTOO_TEMP;
    rm -f ./gentoo/media-libs/libmp3splt/libmp3splt-${LIBMP3SPLT_VERSION}.ebuild
    find $GENTOO_TEMP -name \".svn\" -exec rm -rf '{}' \; &>/dev/null
#digest libmp3splt
#if we don't have distribution file, create it
    if [[ ! -e ../libmp3splt-$LIBMP3SPLT_VERSION.tar.gz ]];then
        ./make_source_package.sh || exit 1
    fi && \
        cp ../libmp3splt-${LIBMP3SPLT_VERSION}.tar.gz /usr/portage/distfiles &&\
        ebuild $GENTOO_TEMP/media-libs/libmp3splt/libmp3splt* digest &&\
        tar czf ../libmp3splt-${LIBMP3SPLT_VERSION}_ebuild.tar.gz $GENTOO_TEMP/media-libs || exit 1
else
    echo
    echo "We already have the $DIST_FILE distribution file !"
    echo
fi
