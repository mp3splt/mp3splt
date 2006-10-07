#!/usr/local/bin/bash

#this file creates a freebsd package for libmp3splt

#we move in the current script directory
script_dir=$(greadlink -f $0) || exit 1
script_dir=${script_dir%\/*.sh}
cd $script_dir

. ../include_variables.sh

echo
echo $'Package :\tfreebsd'
echo

#we change 0.4_rc1 to 0.4.r1
TEMP_LIBMP3SPLT_VERSION=${LIBMP3SPLT_VERSION/_/.}
NEW_LIBMP3SPLT_VERSION=${TEMP_LIBMP3SPLT_VERSION/rc/r}

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
PKGNAMESUFFIX=	_fbsd_$ARCH
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

#we generate the pkg-descr file
echo $LIBMP3SPLT_DESCRIPTION > pkg-descr
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
pkg_delete libmp3splt_fbsd_$ARCH
#make dist if necessary
if [[ ! -e ../libmp3splt-${LIBMP3SPLT_VERSION}.tar.gz ]];then
    ./make_source_package.sh "netbsd" || exit 1
fi &&\
cp ../libmp3splt-${LIBMP3SPLT_VERSION}.tar.gz /usr/ports/distfiles/ || exit 1
#create ports libmp3splt directory
DATEMV=`date +-%d_%m_%Y__%H_%M_%S`
if [[ -e /usr/ports/audio/libmp3splt ]];then
    #we uninstall libmp3splt from a previous build
    cd /usr/ports/audio/libmp3splt && make deinstall; cd -
    mv /usr/ports/audio/libmp3splt /usr/ports/audio/libmp3splt${DATEMV}
fi
mkdir -p /usr/ports/audio/libmp3splt
cp ./freebsd/* /usr/ports/audio/libmp3splt
rm -f ./freebsd/pkg-descr ./freebsd/pkg-plist ./freebsd/distinfo ./freebsd/Makefile
#we create the package
cd /usr/ports/audio/libmp3splt && make makesum && make && make install &&\
make package;  cd - &&\
mv /usr/ports/audio/libmp3splt/*fbsd*.tbz ../ || exit 1
