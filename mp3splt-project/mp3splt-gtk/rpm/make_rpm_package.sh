#!/bin/bash

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir/..
cd $PROGRAM_DIR

. ./include_variables.sh

echo
echo $'Package :\trpm'
echo

#we need the flags because we told libmp3splt to install in /tmp/rpm
RPM_TEMP=/tmp/rpm_temp
export CFLAGS="-I$RPM_TEMP/libmp3splt/usr/include $CFLAGS"
export LDFLAGS="-L$RPM_TEMP/libmp3splt/usr/lib $LDFLAGS"

#we make the distribution file if we don't have it
if [[ ! -e ../mp3splt-gtk-$MP3SPLT_GTK_VERSION.tar.gz ]];then
    ./make_source_package.sh
fi && \
cp ../mp3splt-gtk-${MP3SPLT_GTK_VERSION}.tar.gz ./rpm/SOURCES &&\
echo "%_topdir $PROGRAM_DIR/rpm" > ~/.rpmmacros &&\
cd rpm &&\
rpmbuild -ba ./SPECS/mp3splt-gtk.spec &&\
rm -rf ./BUILD/* &&\
rm -rf ./SOURCES/* &&\
mv ./RPMS/$ARCH/*.rpm ../.. &&\
mv ./SRPMS/*.rpm ../..
