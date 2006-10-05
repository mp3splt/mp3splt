#!/bin/bash

#we move in the current script directory
script_dir=$(readlink -f $0) || exit 1
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir/..
cd $PROGRAM_DIR

. ./include_variables.sh

echo
echo $'Package :\trpm'
echo

cd rpm &&\
echo "Summary: Mp3splt-gtk is a GTK2 gui that uses libmp3splt to split mp3 and ogg without decoding
Name: mp3splt-gtk
Version: ${MP3SPLT_GTK_VERSION}
Release: 1
Source: %{name}-%{version}.tar.gz
License: GNU GPL
Packager: Munteanu Alexandru Ionut <io_alex_2002@yahoo.fr>
Group: Productivity/Multimedia/Sound/Utilities
URL: http://mp3splt.sourceforge.net
BuildRoot: /tmp/rpm_temp/mp3splt-gtk
Requires: libmp3splt = ${LIBMP3SPLT_VERSION}, beep-media-player, glib2 >= 2.6, gtk2 >= 2.6, pkgconfig
Provides: mp3splt-gtk
%description
mp3splt-project common features:
* split mp3 and ogg files from a begin time to an end time without decoding
* split an album with splitpoints from the freedb.org server
* split an album with local .XMCD, .CDDB or .CUE file
* split files automatically with silence detection
* split files by a fixed time length
* split files created with Mp3Wrap or AlbumWrap
* split concatenated mp3 files
* support for mp3 VBR (variable bit rate)
* specify output directory for splitted files
%prep
%setup
%build
./configure --prefix=/usr --enable-bmp
make
%install
make DESTDIR=\$RPM_BUILD_ROOT install
%clean
%files
%defattr(-,root,root)
/usr/*
%doc ${MP3SPLT_GTK_DOC[@]}" > ./SPECS/mp3splt-gtk.spec || exit 1 \
&& cd ..

#we need the flags because we told libmp3splt to install in /tmp/rpm
RPM_TEMP=/tmp/rpm_temp
export CFLAGS="-I$RPM_TEMP/libmp3splt/usr/include $CFLAGS"
export LDFLAGS="-L$RPM_TEMP/libmp3splt/usr/lib $LDFLAGS"

#we make the distribution file if we don't have it
if [[ ! -e ../mp3splt-gtk-$MP3SPLT_GTK_VERSION.tar.gz ]];then
    ./make_source_package.sh || exit 1
fi && \
cp ../mp3splt-gtk-${MP3SPLT_GTK_VERSION}.tar.gz ./rpm/SOURCES &&\
echo "%_topdir $PROGRAM_DIR/rpm" > ~/.rpmmacros &&\
cd rpm &&\
rpmbuild -ba ./SPECS/mp3splt-gtk.spec &&\
rm -rf ./BUILD/* &&\
rm -rf ./SOURCES/* &&\
mv ./RPMS/$ARCH/*.rpm ../.. &&\
mv ./SRPMS/*.rpm ../.. || exit 1
