#!/bin/bash

#this file creates a netbsd package for mp3splt

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir
cd $PROGRAM_DIR

. ../include_variables.sh

echo
echo $'Package :\tnetbsd'
echo

#we generate the makefile
echo "# \$NetBSD\$

DISTNAME=       mp3splt-${MP3SPLT_VERSION}
PKGNAME=        mp3splt_nbsd_$ARCH-${MP3SPLT_VERSION}
CATEGORIES=     audio
MASTER_SITES=   \${MASTER_SITE_SOURCEFORGE:=mp3splt/}

MAINTAINER=     io_alex_2002@yahoo.fr
HOMEPAGE=       http://mp3splt.sourceforge.net/
COMMENT=        Command line program to split mp3 and ogg without decoding

PKG_INSTALLATION_TYPES= overwrite pkgviews

GNU_CONFIGURE=          YES

DEPENDS+=       libmp3splt_nbsd_$ARCH-${LIBMP3SPLT_VERSION}:../../audio/libmp3splt

OBJMACHINE=     YES
DOC_DIR=\${PREFIX}/share/doc/mp3splt/

#copy documentation
pre-install:
        \${INSTALL_DATA_DIR} \${DOC_DIR}" > Makefile

for doc in "${MP3SPLT_DOC_FILES[@]}";do
    echo "	\${INSTALL_DATA} \${WRKSRC}/${doc} \${DOC_DIR}" >> Makefile
done

echo "
.include \"../../audio/libmp3splt/buildlink3.mk\"

.include \"../../mk/bsd.pkg.mk\"" >> Makefile

#we generate the PLIST
echo "@comment $NetBSD$" > PLIST

for file in "${MP3SPLT_FILES[@]}";do
    echo "$file" >> PLIST
done

for file in "${MP3SPLT_MAN1_FILES[@]}";do
    echo "man/man1/$file" >> PLIST
done

for doc in "${MP3SPLT_DOC_FILES[@]}";do
    echo "share/doc/mp3splt/${doc}" >> PLIST
done

echo "@dirrm share/doc/mp3splt" >> PLIST

cd ..

#we set the flags
export CFLAGS="-I/usr/pkg/include"
export LDFLAGS="-L/usr/pkg/lib"

#we create the directory
mkdir -p /usr/pkgsrc/audio/mp3splt
#we remove possible remained files
rm -rf /usr/pkgsrc/audio/mp3splt/*
rm -f /usr/pkgsrc/packages/All/mp3splt*nbsd*.tgz
#copy netbsd files
cp ./netbsd/* /usr/pkgsrc/audio/mp3splt/
#we make the distribution file if we don't have it
if [[ ! -e ../mp3splt-${MP3SPLT_VERSION}.tar.gz ]];then
    ./make_source_package.sh
fi &&\
mv ../mp3splt-${MP3SPLT_VERSION}.tar.gz /usr/pkgsrc/distfiles
#remove possible installed package
pkg_delete mp3splt_nbsd_$ARCH

#package creation
cd /usr/pkgsrc/audio/mp3splt && bmake mdi && bmake package &&\
bmake deinstall && cd -
mv /usr/pkgsrc/packages/All/mp3splt*nbsd*.tgz ../
