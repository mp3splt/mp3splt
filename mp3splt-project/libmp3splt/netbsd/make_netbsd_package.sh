#!/bin/bash

#this file creates a netbsd package for libmp3splt

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

DISTNAME= 	libmp3splt-${LIBMP3SPLT_VERSION}
PKGNAME=	libmp3splt-nbsd-${LIBMP3SPLT_VERSION}
CATEGORIES= 	audio devel
MASTER_SITES= 	\${MASTER_SITE_SOURCEFORGE:=mp3splt/}

MAINTAINER= 	io_alex_2002@yahoo.fr
HOMEPAGE= 	http://mp3splt.sourceforge.net/
COMMENT= 	Library to split mp3 and ogg without decoding

PKG_INSTALLATION_TYPES= overwrite pkgviews

GNU_CONFIGURE=		YES
USE_LIBTOOL=		YES

DEPENDS+=	libmad-[0-9]*:../../audio/libmad
DEPENDS+=	libogg-[0-9]*:../../multimedia/libogg
DEPENDS+=	libvorbis-[0-9]*:../../audio/libvorbis
DEPENDS+=	libid3tag-[0-9]*:../../audio/libid3tag

OBJMACHINE=	YES
DOC_DIR=\${PREFIX}/share/doc/libmp3splt/

#copy documentation
pre-install:
	\${INSTALL_DATA_DIR} \${DOC_DIR}" > Makefile

for doc in "${LIBMP3SPLT_DOC_FILES[@]}";do
    echo "	\${INSTALL_DATA} \${WRKSRC}/${doc} \${DOC_DIR}" >> Makefile
done

echo "
.include \"../../audio/libmad/buildlink3.mk\"
.include \"../../multimedia/libogg/buildlink3.mk\"
.include \"../../audio/libvorbis/buildlink3.mk\"
.include \"../../audio/libid3tag/buildlink3.mk\"

.include \"../../mk/bsd.pkg.mk" >> Makefile

#we generate the PLIST
echo "@comment \$NetBSD\$" > PLIST

for file in "${LIBMP3SPLT_FILES[@]}";do
    if [[ $file != "lib/libmp3splt.so.0.0" ]] &&
        [[ $file != "lib/libmp3splt.a" ]];then
        echo "$file" >> PLIST
    fi
done

for doc in "${LIBMP3SPLT_DOC_FILES[@]}";do
    echo "share/doc/libmp3splt/${doc}" >> PLIST
done

echo "@dirrm include/libmp3splt" >> PLIST
echo "@dirrm share/doc/libmp3splt" >> PLIST

#we generate buildlink3.mk
echo "# \$NetBSD\$

BUILDLINK_DEPTH:=		\${BUILDLINK_DEPTH}+
LIBMP3SPLT_BUILDLINK3_MK:=	\${LIBMP3SPLT_BUILDLINK3_MK}+

.if \${BUILDLINK_DEPTH} == \"+\"
BUILDLINK_DEPENDS+=	libmp3splt
.endif

BUILDLINK_PACKAGES:=	\${BUILDLINK_PACKAGES:Nlibmp3splt}
BUILDLINK_PACKAGES+=	libmp3splt

.if \${LIBMP3SPLT_BUILDLINK3_MK} == \"+\"
BUILDLINK_API_DEPENDS.libmp3splt+=	libmp3splt-nbsd-${LIBMP3SPLT_VERSION}
BUILDLINK_PKGSRCDIR.libmp3splt?=	../../audio/libmp3splt
.endif

.include \"../../audio/libmad/buildlink3.mk\"
.include \"../../multimedia/libogg/buildlink3.mk\"
.include \"../../audio/libvorbis/buildlink3.mk\"
.include \"../../audio/libid3tag/buildlink3.mk\"

BUILDLINK_DEPTH:=		\${BUILDLINK_DEPTH:S/+$//}" > buildlink3.mk

cd ..

#we set the flags
export CFLAGS="-I/usr/pkg/include"
export LDFLAGS="-L/usr/pkg/lib"

#remove possible left files
rm -rf ../*mp3splt*nbsd*.tgz
rm -f /usr/pkgsrc/packages/All/libmp3splt*nbsd*.tgz
#create libmp3splt pkgsrc dir
mkdir -p /usr/pkgsrc/audio/libmp3splt
#erase its content
rm -rf /usr/pkgsrc/audio/libmp3splt/*
#copy netbsd files
cp ./netbsd/* /usr/pkgsrc/audio/libmp3splt
#we make the distribution file if we don't have it
if [[ ! -e ../libmp3splt-${LIBMP3SPLT_VERSION}.tar.gz ]];then
    ./make_source_package.sh
fi &&\
mv ../libmp3splt-${LIBMP3SPLT_VERSION}.tar.gz /usr/pkgsrc/distfiles
#remove possible installed package
pkg_delete libmp3splt-nbsd

#package creation
cd /usr/pkgsrc/audio/libmp3splt && bmake mdi && bmake package &&\
cd - && mv /usr/pkgsrc/packages/All/*libmp3splt*nbsd*.tgz ../
