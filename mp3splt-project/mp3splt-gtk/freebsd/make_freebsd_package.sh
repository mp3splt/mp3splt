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

NEW_MP3SPLT_GTK_VERSION=${MP3SPLT_GTK_VERSION%_rc*}

DATE=`date`
#we generate the Makefile
echo "# New ports collection makefile for: mp3splt-gtk
# Date created: $DATE
# Whom:         Munteanu Alexandru Ionut
#
# \$FreeBSD\$
#

PORTNAME=       mp3splt-gtk
PORTVERSION=    ${NEW_MP3SPLT_GTK_VERSION}
CATEGORIES=     audio
MASTER_SITES=   \${MASTER_SITE_SOURCEFORGE:=mp3splt/}
MASTER_SITE_SUBDIR=     mp3splt
PKGNAMESUFFIX=  -fbsd
DISTNAME=       \${PORTNAME}-${MP3SPLT_GTK_VERSION}

MAINTAINER=     io_alex_2002@yahoo.fr
COMMENT=        Command line program to split mp3 and ogg without
decoding

BUILD_DEPENDS=  libmp3splt:\${PORTSDIR}/audio/libmp3splt
USE_GNOME=      gtk20
LIB_DEPENDS=    glib-2.0:\${PORTSDIR}/devel/glib20 \\
                beep.2:\${PORTSDIR}/multimedia/beep-media-player

GNU_CONFIGURE=  YES
USE_GMAKE=      YES
CONFIGURE_ARGS+=--enable-bmp
CONFIGURE_ENV+= LDFLAGS=\"\${LDFLAGS} -L\${LOCALBASE}/lib\" \\
                CFLAGS=\"\${CFLAGS} -I\${LOCALBASE}/include\"

WRKSRC=         \${WRKDIR}/mp3splt-gtk-${MP3SPLT_GTK_VERSION}

#copy documentation
DOC_DIR=        \${PREFIX}/share/doc/mp3splt-gtk/

pre-install:
        \${MKDIR} \${DOC_DIR}" > Makefile

for doc in "${MP3SPLT_GTK_DOC_FILES[@]}";do
    echo "	\${INSTALL_DATA} \${WRKSRC}/${doc} \${DOC_DIR}" >> Makefile
done

echo "
.include <bsd.port.mk>" >> Makefile

#we generate the pkg-plist
echo "@comment \$FreeBSD\$" > pkg-plist

for file in "${MP3SPLT_GTK_FILES[@]}";do
    echo "$file" >> pkg-plist
done

for doc in "${MP3SPLT_GTK_DOC_FILES[@]}";do
    echo "share/doc/mp3splt-gtk/$doc" >> pkg-plist
done

echo "@dirrm share/doc/mp3splt-gtk" >> pkg-plist
