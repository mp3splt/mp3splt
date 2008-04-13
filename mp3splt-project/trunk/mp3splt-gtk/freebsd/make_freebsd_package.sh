#!/usr/local/bin/bash

#this file creates a freebsd package for mp3splt

#we move in the current script directory
script_dir=$(greadlink -f $0) || exit 1
script_dir=${script_dir%\/*.sh}
cd $script_dir

. ../include_variables.sh

put_package "freebsd"

#we change 0.4_rc1 to 0.4.r1
TEMP_MP3SPLT_GTK_VERSION=${MP3SPLT_GTK_VERSION/_/.}
NEW_MP3SPLT_GTK_VERSION=${TEMP_MP3SPLT_GTK_VERSION/rc/r}

#if we don't have the distribution file
DIST_FILE="../../mp3splt-gtk_fbsd_${ARCH}-${NEW_MP3SPLT_GTK_VERSION}.tbz"
if [[ ! -f $DIST_FILE ]];then
    
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
PKGNAMESUFFIX=  _fbsd_$ARCH
DISTNAME=       \${PORTNAME}-${MP3SPLT_GTK_VERSION}

MAINTAINER=     io_alex_2002@yahoo.fr
COMMENT=        Command line program to split mp3 and ogg without decoding

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
    
    #we generate the pkg-descr file
    echo $MP3SPLT_GTK_DESCRIPTION > pkg-descr
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
    pkg_delete mp3splt-gtk_fbsd_$ARCH
    
    #make dist if necessary
    if [[ ! -e ../mp3splt-gtk-${MP3SPLT_GTK_VERSION}.tar.gz ]];then
        ./make_source_package.sh "netbsd" || exit 1
    fi &&\
        cp ../mp3splt-gtk-${MP3SPLT_GTK_VERSION}.tar.gz /usr/ports/distfiles/ || exit 1
    #create ports mp3splt-gtk directory
    DATEMV=`date +-%d_%m_%Y__%H_%M_%S`
    if [[ -e /usr/ports/audio/mp3splt-gtk ]];then
        #we uninstall mp3splt-gtk from a previous build
        cd /usr/ports/audio/mp3splt-gtk && make deinstall; cd -
        mv /usr/ports/audio/mp3splt-gtk /usr/ports/audio/mp3splt-gtk${DATEMV}
    fi
    mkdir -p /usr/ports/audio/mp3splt-gtk
    cp ./freebsd/* /usr/ports/audio/mp3splt-gtk
    rm -f ./freebsd/pkg-descr ./freebsd/pkg-plist ./freebsd/distinfo ./freebsd/Makefile
    #we create the package
    cd /usr/ports/audio/mp3splt-gtk && make makesum && make && make install\
        && make package && cd - &&\
        mv /usr/ports/audio/mp3splt-gtk/*fbsd*.tbz ../ &&\
        #uninstall some packages
    cd /usr/ports/audio/mp3splt-gtk && make deinstall; cd - || exit 1
else
    put_is_package_warning "We already have the $DIST_FILE distribution file !"
fi
