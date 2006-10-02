#!/bin/bash

#this file creates a freebsd package for mp3splt

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir
cd $PROGRAM_DIR

. ../include_variables.sh

echo
echo $'Package :\tfreebsd'
echo

NEW_MP3SPLT_VERSION=${MP3SPLT_VERSION%_rc*}

DATE=`date`
#we generate the Makefile
echo "# New ports collection makefile for: mp3splt
# Date created:	$DATE
# Whom:		Munteanu Alexandru Ionut
#
# \$FreeBSD\$
#

PORTNAME=	mp3splt
PORTVERSION=	${NEW_MP3SPLT_VERSION}
CATEGORIES=	audio
MASTER_SITES=	\${MASTER_SITE_SOURCEFORGE:=mp3splt/}
MASTER_SITE_SUBDIR=	mp3splt
PKGNAMESUFFIX=	-fbsd
DISTNAME=	\${PORTNAME}-${MP3SPLT_VERSION}

MAINTAINER=	io_alex_2002@yahoo.fr
COMMENT= 	Command line program to split mp3 and ogg without decoding

BUILD_DEPENDS=	libmp3splt:\${PORTSDIR}/audio/libmp3splt

GNU_CONFIGURE=	YES
USE_GMAKE=	YES
CONFIGURE_ENV+=	LDFLAGS=\"\${LDFLAGS} -L\${LOCALBASE}/lib\" \\
		CFLAGS=\"\${CFLAGS} -I\${LOCALBASE}/include\"

WRKSRC=		\${WRKDIR}/mp3splt-${MP3SPLT_VERSION}

#copy documentation
DOC_DIR=	\${PREFIX}/share/doc/mp3splt/

pre-install:
	\${MKDIR} \${DOC_DIR}" > Makefile

for doc in "${MP3SPLT_DOC_FILES[@]}";do
    echo "	\${INSTALL_DATA} \${WRKSRC}/${doc} \${DOC_DIR}" >> Makefile
done

echo "
.include <bsd.port.mk>" >> Makefile

#we generate the pkg-plist
echo "@comment \$FreeBSD\$" > pkg-plist

for file in "${MP3SPLT_FILES[@]}";do
    echo "$file" >> pkg-plist
done

for man1 in "${MP3SPLT_MAN1_FILES[@]}";do
    echo "man/man1/${man1}" >> pkg-plist
done

for doc in "${MP3SPLT_DOC_FILES[@]}";do
    echo "share/doc/mp3splt/$doc" >> pkg-plist
done

echo "@dirrm share/doc/mp3splt" >> pkg-plist

cd ..

#we set the flags
export ACLOCAL_FLAGS="-I /usr/local/share/aclocal"
export CFLAGS="-I/usr/local/include -I/usr/include -I/usr/X11R6/include"
export LDFLAGS="-L/usr/local/lib -L/usr/lib -L/usr/X11R6/lib"

#remove old package
pkg_delete mp3splt-fbsd
#make dist if necessary
if [[ ! -e ../mp3splt-${MP3SPLT_VERSION}.tar.gz ]];then
    ./make_source_package.sh
fi &&\
mv ../mp3splt-${MP3SPLT_VERSION}.tar.gz /usr/ports/distfiles/
#create ports mp3splt directory
mkdir -p /usr/ports/audio/mp3splt
rm -rf /usr/ports/audio/mp3splt/*
cp ./freebsd/* /usr/ports/audio/mp3splt &&\
#we create the package
cd /usr/ports/audio/mp3splt && make makesum && make && make install\
&& make package && cd - &&\
mv /usr/ports/audio/mp3splt/*fbsd*.tbz ../ &&\
#we uninstall the installed package
cd /usr/ports/audio/mp3splt && make deinstall && cd -
