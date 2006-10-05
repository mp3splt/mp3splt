#!/usr/pkg/bin/bash

#this file creates a netbsd package for mp3splt-gtk

#we move in the current script directory
script_dir=$(greadlink -f $0) || exit 1
script_dir=${script_dir%\/*.sh}
cd $script_dir

unset PKG_PATH

. ../include_variables.sh

echo
echo $'Package :\tnetbsd'
echo

#we generate the makefile
echo "# \$NetBSD\$

DISTNAME=       mp3splt-gtk-${MP3SPLT_GTK_VERSION}
PKGNAME=        mp3splt-gtk_nbsd_$ARCH-${MP3SPLT_GTK_VERSION}
CATEGORIES=     audio
MASTER_SITES=   \${MASTER_SITE_SOURCEFORGE:=mp3splt/}

MAINTAINER=     io_alex_2002@yahoo.fr
HOMEPAGE=       http://mp3splt.sourceforge.net/
COMMENT=        Libmp3splt GTK2 gui to split mp3 and ogg without decoding

PKG_INSTALLATION_TYPES= overwrite pkgviews

CONFIGURE_ARGS+=        --enable-bmp
GNU_CONFIGURE=          YES
USE_TOOLS+=             pkg-config

DEPENDS+=       libmp3splt_nbsd_$ARCH-${LIBMP3SPLT_VERSION}:../../audio/libmp3splt
DEPENDS+=       bmp-[0-9]*:../../audio/bmp
DEPENDS+=       gtk2+>=2.6:../../x11/gtk2
DEPENDS+=       glib2>=2.6:../../devel/glib2

OBJMACHINE=     YES
DOC_DIR=\${PREFIX}/share/doc/mp3splt-gtk/

#copy documentation
pre-install:
        \${INSTALL_DATA_DIR} \${DOC_DIR}" > Makefile

for doc in "${MP3SPLT_GTK_DOC_FILES[@]}";do
    echo "	\${INSTALL_DATA} \${WRKSRC}/${doc} \${DOC_DIR}" >> Makefile
done

echo "
.include \"../../audio/libmp3splt/buildlink3.mk\"
.include \"../../devel/glib2/buildlink3.mk\"
.include \"../../x11/gtk2/buildlink3.mk\"
.include \"../../audio/bmp/buildlink3.mk\"

.include \"../../mk/bsd.pkg.mk\"" >> Makefile

#we generate the PLIST
echo "@comment \$NetBSD\$" > PLIST

for file in "${MP3SPLT_GTK_FILES[@]}";do
    echo "$file" >> PLIST
done

for doc in "${MP3SPLT_GTK_DOC_FILES[@]}";do
    echo "share/doc/mp3splt-gtk/$doc" >> PLIST
done

echo "@dirrm share/doc/mp3splt-gtk" >> PLIST

cd ..

#we set the flags
export CFLAGS="-I/usr/pkg/include"
export LDFLAGS="-L/usr/pkg/lib"

#we remove possible remained files
rm -f ../mp3splt-gtk*nbsd*.tgz
rm -f /usr/pkgsrc/packages/All/mp3splt-gtk*nbsd*.tgz
#we create the directory
DATEMV=`date +-%d_%m_%Y__%H_%M_%S`
if [[ -e /usr/pkgsrc/audio/mp3splt-gtk ]];then
    mv /usr/pkgsrc/audio/mp3splt-gtk /usr/pkgsrc/audio/mp3splt-gtk${DATEMV}
fi
mkdir -p /usr/pkgsrc/audio/mp3splt-gtk
#copy netbsd files
cp ./netbsd/* /usr/pkgsrc/audio/mp3splt-gtk/
#we make the distribution file if we don't have it
if [[ ! -e ../mp3splt-${MP3SPLT_GTK_VERSION}.tar.gz ]];then
    bash ./make_source_package.sh "netbsd" || exit 1
fi &&\
cp ../mp3splt-gtk-${MP3SPLT_GTK_VERSION}.tar.gz /usr/pkgsrc/distfiles || exit 1

#remove possible installed package
pkg_delete mp3splt-gtk_nbsd_$ARCH

#package creation
cd /usr/pkgsrc/audio/mp3splt-gtk && bmake mdi && bmake package &&\
bmake deinstall && cd - &&\
mv /usr/pkgsrc/packages/All/mp3splt-gtk*nbsd*.tgz ../ || exit 1

