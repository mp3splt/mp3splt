#!/bin/bash

#we move in the current script directory
script_dir=$(readlink -f $0) || exit 1
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir/..
cd $PROGRAM_DIR

. ./include_variables.sh "noflags"

echo
echo $'Package :\tarch'
echo

if [[ $ARCH = "i386" ]];then
    ARCH=i686
    CFLAGS="-O2 -mtune=${ARCH} $CFLAGS"
fi

#we generate the PKGBUILD file
cd arch && echo "# \$Id: PKGBUILD \$
# Packager: Munteanu Alexandru Ionut <io_alex_2002@yahoo.fr>
pkgname=mp3splt-gtk
pkgver=${MP3SPLT_GTK_VERSION}
pkgrel=1
pkgdesc=\"Mp3splt-gtk is a GTK2 gui that uses libmp3splt to split mp3 and ogg without deconding\"
arch=(i686 x86_64)
url=\"http://mp3splt.sourceforge.net\"
groups=root
depends=('libmp3splt=${LIBMP3SPLT_VERSION}')
source=(\$pkgname-\$pkgver.tar.gz)

build() {
  cd \$startdir/src/\$pkgname-\$pkgver
  ./configure --prefix=/usr --enable-bmp
  make || return 1
  mkdir -p \$startdir/pkg/usr/share/\$pkgname/doc
  cp ${MP3SPLT_GTK_DOC_FILES[@]} \$startdir/pkg/usr/share/\$pkgname/doc/
  make prefix=\$startdir/pkg/usr install
}" > PKGBUILD && cd ..

#we set the flags to find libmp3splt
export CFLAGS="-I../libmp3splt/arch/pkg/usr/include $CFLAGS"
export LDFLAGS="-L../libmp3splt/arch/pkg/usr/lib $LDFLAGS"

#we make the distribution file if we don't have it
if [[ ! -e ../mp3splt-gtk-${MP3SPLT_GTK_VERSION}.tar.gz ]];then
    ./make_source_package.sh || exit 1
fi &&\
cp ../mp3splt-gtk-${MP3SPLT_GTK_VERSION}.tar.gz ./arch &&\
cd arch && makepkg -d -c &&\
mv mp3splt-gtk-${MP3SPLT_GTK_VERSION}-1.pkg.tar.gz \
../../mp3splt-gtk-${MP3SPLT_GTK_VERSION}-1_${ARCH}.pkg.tar.gz &&\
rm -f ./mp3splt-gtk-${MP3SPLT_GTK_VERSION}.tar.gz || exit 1
