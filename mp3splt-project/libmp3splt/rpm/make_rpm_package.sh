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

#generate rpm specs
cd rpm &&\
echo "Summary: Library created from mp3splt v2.1c to split mp3 and ogg without deconding
Name: libmp3splt
Version: ${LIBMP3SPLT_VERSION}
Release: 1
Source: %{name}-%{version}.tar.gz
License: GNU GPL
Packager: Munteanu Alexandru Ionut <io_alex_2002@yahoo.fr>
Group: Productivity/Multimedia/Sound/Utilities
URL: http://mp3splt.sourceforge.net
BuildRoot: /tmp/rpm_temp/libmp3splt
Requires: libmad libvorbis libogg libid3tag
Provides: libmp3splt
%description
${LIBMP3SPLT_DESCRIPTION}
%prep
%setup
%build
./configure --prefix=/usr
make
%install
make DESTDIR=\$RPM_BUILD_ROOT install
%files
%defattr(-,root,root)
/usr/lib/*
/usr/include/*
%doc ${LIBMP3SPLT_DOC_FILES[@]}
" > ./SPECS/libmp3splt.spec \
&& cd ..

#we make the distribution file if we don't have it
if [[ ! -e ../libmp3splt-$LIBMP3SPLT_VERSION.tar.gz ]];then
    ./make_source_package.sh || exit 1
fi && \
cp ../libmp3splt-${LIBMP3SPLT_VERSION}.tar.gz ./rpm/SOURCES &&\
echo "%_topdir $PROGRAM_DIR/rpm" > ~/.rpmmacros &&\
#this also installs libmp3splt in /tmp/rpm_temp
cd rpm && rpmbuild -ba ./SPECS/libmp3splt.spec &&\
rm -rf ./BUILD/* &&\
rm -rf ./SOURCES/* &&\
rm -f ./SPECS/libmp3splt.spec &&\
mv ./RPMS/$ARCH/*.rpm ../.. &&\
mv ./SRPMS/*.rpm ../.. || exit 1
