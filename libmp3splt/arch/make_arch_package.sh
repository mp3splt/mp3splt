#!/bin/bash

#we move in the current script directory
script_dir=$(readlink -f $0) || exit 1
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir/..
cd $PROGRAM_DIR

. ./include_variables.sh "noflags" || exit 1

put_package "arch"

if [[ $ARCH = "i386" ]];then
    ARCH=i686
    CFLAGS="-O2 -mtune=${ARCH} $CFLAGS"
fi

#if we don't have the distribution file
DIST_FILE="../libmp3splt-${LIBMP3SPLT_VERSION}-1_${ARCH}.pkg.tar.gz"
if [[ ! -f $DIST_FILE ]];then
    #we generate the PKGBUILD file
    cd arch && echo "# \$Id: PKGBUILD \$
# Packager: Munteanu Alexandru Ionut <io_alex_2002@yahoo.fr>
pkgname=libmp3splt
pkgver=${LIBMP3SPLT_VERSION}
pkgrel=1
pkgdesc=\"Library created from mp3splt v2.1c to split mp3 and ogg without deconding\"
arch=(i686 x86_64)
url=\"http://mp3splt.sourceforge.net\"
groups=root
depends=('libogg' 'libvorbis' 'libmad' 'libid3tag')
source=(\$pkgname-\$pkgver.tar.gz)

build() {
  cd \$startdir/src/\$pkgname-\$pkgver
  ./configure --prefix=/usr
  make || return 1
  mkdir -p \$startdir/pkg/usr/share/libmp3splt/doc
  cp ${LIBMP3SPLT_DOC_FILES[@]} \$startdir/pkg/usr/share/libmp3splt/doc/
  make prefix=\$startdir/pkg/usr install
}" > PKGBUILD && cd ..
    
    #we make the distribution file if we don't have it
    if [[ ! -e ../libmp3splt-${LIBMP3SPLT_VERSION}.tar.gz ]];then
        ./make_source_package.sh || exit 1
    fi &&\
        cd $PROGRAM_DIR &&\
        cp ../libmp3splt-${LIBMP3SPLT_VERSION}.tar.gz ./arch &&\
        cd arch && makepkg &&\
        mv libmp3splt-${LIBMP3SPLT_VERSION}-1.pkg.tar.gz \
        ../../libmp3splt-${LIBMP3SPLT_VERSION}-1_${ARCH}.pkg.tar.gz &&\
        rm -f ./libmp3splt-${LIBMP3SPLT_VERSION}.tar.gz && rm -f PKGBUILD \
        || exit 1
else
    put_is_package_warning "We already have the $DIST_FILE distribution file !"
fi
