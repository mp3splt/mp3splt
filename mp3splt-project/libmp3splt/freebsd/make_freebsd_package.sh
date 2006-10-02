#!/bin/bash

#this file creates a freebsd package for libmp3splt

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir
cd $PROGRAM_DIR

. ../include_variables.sh

echo
echo $'Package :\tfreebsd'
echo

NEW_LIBMP3SPLT_VERSION=${LIBMP3SPLT_VERSION%_rc*}

DATE=`date`
#we generate the Makefile
echo "# New ports collection makefile for: libmp3splt
# Date created:	$DATE
# Whom:		Munteanu Alexandru Ionut
#
# \$FreeBSD\$
#

PORTNAME=	libmp3splt
PORTVERSION=	$NEW_LIBMP3SPLT_VERSION
CATEGORIES=	audio devel
MASTER_SITES=	\${MASTER_SITE_SOURCEFORGE:=mp3splt/}
MASTER_SITE_SUBDIR=	mp3splt
PKGNAMESUFFIX=	-fbsd
DISTNAME=	\${PORTNAME}-${LIBMP3SPLT_VERSION}

MAINTAINER=	io_alex_2002@yahoo.fr
COMMENT=	Library to split mp3 and ogg without decoding

LIB_DEPENDS=	mad.2:\${PORTSDIR}/audio/libmad \\
		vorbis.3:\${PORTSDIR}/audio/libvorbis \\
		ogg.5:\${PORTSDIR}/audio/libogg \\
		id3tag.0:\${PORTSDIR}/audio/libid3tag

GNU_CONFIGURE=	YES
USE_GMAKE=	YES
CONFIGURE_ENV+=	LDFLAGS=\"\${LDFLAGS} -L\${LOCALBASE}/lib\" \\
		CFLAGS=\"\${CFLAGS} -I\${LOCALBASE}/include\"

WRKSRC=		\${WRKDIR}/libmp3splt-${LIBMP3SPLT_VERSION}

#copy documentation
DOC_DIR=	\${PREFIX}/share/doc/libmp3splt/

pre-install:
	\${MKDIR} \${DOC_DIR}" > Makefile

for doc in "${LIBMP3SPLT_DOC_FILES[@]}";do
    echo "	\${INSTALL_DATA} \${WRKSRC}/${doc} \${DOC_DIR}" >> Makefile
done

echo "
.include <bsd.port.mk>" >> Makefile

#we generate the pkg-plist
echo "@comment \$FreeBSD\$" > pkg-plist

for file in "${LIBMP3SPLT_FILES[@]}";do
    if [[ $file != "lib/libmp3splt.so.0.0" ]];then
        echo "$file" >> pkg-plist
    else
        echo "lib/libmp3splt.so" >> pkg-plist
        echo "lib/libmp3splt.so.0" >> pkg-plist
    fi
done

for doc in "${LIBMP3SPLT_DOC_FILES[@]}";do
    echo "share/doc/libmp3splt/${doc}" >> pkg-plist
done

echo "@dirrm include/libmp3splt" >> pkg-plist
echo "@dirrm share/doc/libmp3splt" >> pkg-plist

#TODO : do the real compilation

