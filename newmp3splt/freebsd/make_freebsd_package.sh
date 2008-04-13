#!/usr/local/bin/bash

#this file creates a freebsd package for mp3splt

#we move in the current script directory
script_dir=$(greadlink -f $0) || exit 1
script_dir=${script_dir%\/*.sh}
cd $script_dir

. ../include_variables.sh

put_package "freebsd"

#we change 2.2_rc1 to 2.2.r1
TEMP_MP3SPLT_VERSION=${MP3SPLT_VERSION/_/.}
NEW_MP3SPLT_VERSION=${TEMP_MP3SPLT_VERSION/rc/r}

#if we don't have the distribution file
DIST_FILE="../../mp3splt_fbsd_${ARCH}-${NEW_MP3SPLT_VERSION}.tbz"
if [[ ! -f $DIST_FILE ]];then

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
PKGNAMESUFFIX=	_fbsd_$ARCH
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
        echo "share/man/man1/${man1}" >> pkg-plist
    done

    for doc in "${MP3SPLT_DOC_FILES[@]}";do
        echo "share/doc/mp3splt/$doc" >> pkg-plist
    done

    echo "@dirrm share/doc/mp3splt" >> pkg-plist

    #we generate the pkg-descr file
    echo $MP3SPLT_DESCRIPTION > pkg-descr
    echo "
WWW: http://mp3splt.sourceforge.net
" >> pkg-descr

    #we generate the distinfo file
    echo "" > distinfo

    cd ..

    #we set the flags
    export ACLOCAL_FLAGS="-I /usr/local/share/aclocal"
    export CFLAGS="-I/usr/local/include -I/usr/include -I/usr/X11R6/include $CFLAGS"
    export LDFLAGS="-L/usr/local/lib -L/usr/lib -L/usr/X11R6/lib $LDFLAGS"

    #remove old package
    pkg_delete mp3splt_fbsd_$ARCH
    #make dist if necessary
    if [[ ! -e ../mp3splt-${MP3SPLT_VERSION}.tar.gz ]];then
        ./make_source_package.sh "netbsd" || exit 1
    fi &&\
        cp ../mp3splt-${MP3SPLT_VERSION}.tar.gz /usr/ports/distfiles/ || exit 1
    #create ports mp3splt directory
    DATEMV=`date +-%d_%m_%Y__%H_%M_%S`
    if [[ -e /usr/ports/audio/mp3splt ]];then
        #we uninstall mp3splt from a previous build
        cd /usr/ports/audio/mp3splt && make deinstall; cd -
        mv /usr/ports/audio/mp3splt /usr/ports/audio/mp3splt${DATEMV}
    fi
    mkdir -p /usr/ports/audio/mp3splt
    cp ./freebsd/* /usr/ports/audio/mp3splt &&\
        rm -f ./freebsd/pkg-descr ./freebsd/pkg-plist ./freebsd/distinfo ./freebsd/Makefile
    #we create the package
    cd /usr/ports/audio/mp3splt && make makesum && make && make install\
        && make package && cd - &&\
        mv /usr/ports/audio/mp3splt/*fbsd*.tbz ../ &&\
        #we uninstall the installed package
    cd /usr/ports/audio/mp3splt && make deinstall; cd - || exit 1
else
    put_is_package_warning "We already have the $DIST_FILE distribution file !"
fi
