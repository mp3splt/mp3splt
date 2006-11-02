#!/usr/pkg/bin/bash

#this file creates a netbsd package for mp3splt

#we move in the current script directory
script_dir=$(greadlink -f $0) || exit 1
script_dir=${script_dir%\/*.sh}
cd $script_dir

unset PKG_PATH

. ../include_variables.sh

echo
echo $'Package :\tnetbsd'
echo

#if we don't have the distribution file
DIST_FILE="../../mp3splt_nbsd_${ARCH}-${MP3SPLT_VERSION}.tgz"
if [[ ! -f $DIST_FILE ]];then
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

    #we generate the distinfo file
    echo "\$NetBSD\$" > distinfo

    #we generate the DESCR file
    echo $MP3SPLT_DESCRIPTION > DESCR

    cd ..

    #we set the flags
    export CFLAGS="-I/usr/pkg/include $CFLAGS"
    export LDFLAGS="-L/usr/pkg/lib $LDFLAGS"

    #we remove possible remained files
    rm -f ../mp3splt*nbsd*.tgz
    rm -f /usr/pkgsrc/packages/All/mp3splt*nbsd*.tgz
    #we create the directory
    DATEMV=`date +-%d_%m_%Y__%H_%M_%S`
    if [[ -e /usr/pkgsrc/audio/mp3splt ]];then
        mv /usr/pkgsrc/audio/mp3splt /usr/pkgsrc/audio/mp3splt${DATEMV}
    fi
    mkdir -p /usr/pkgsrc/audio/mp3splt
    #copy netbsd files
    cp ./netbsd/* /usr/pkgsrc/audio/mp3splt/
    rm -f ./netbsd/DESCR ./netbsd/Makefile ./netbsd/distinfo \
        ./netbsd/PLIST
    #we make the distribution file if we don't have it
    if [[ ! -e ../mp3splt-${MP3SPLT_VERSION}.tar.gz ]];then
        bash ./make_source_package.sh "netbsd" || exit 1
    fi &&\
        cp ../mp3splt-${MP3SPLT_VERSION}.tar.gz /usr/pkgsrc/distfiles || exit 1

    #remove possible installed package
    pkg_delete mp3splt_nbsd_$ARCH
    
    #package creation
    cd /usr/pkgsrc/audio/mp3splt && bmake mdi && bmake package &&\
        bmake deinstall && cd - &&\
        mv /usr/pkgsrc/packages/All/mp3splt*nbsd*.tgz ../ || exit 1
else
    echo
    echo "We already have the $DIST_FILE distribution file !"
    echo
fi
