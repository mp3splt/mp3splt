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

#we make the distribution file if we don't have it
if [[ ! -e ../libmp3splt-$LIBMP3SPLT_VERSION.tar.gz ]];then
    ./make_source_package.sh
fi && \
cp ../libmp3splt-${LIBMP3SPLT_VERSION}.tar.gz ./rpm/SOURCES &&\
echo "%_topdir $PROGRAM_DIR/rpm" > ~/.rpmmacros &&\
#this also installs libmp3splt in /tmp/rpm_temp
cd rpm && rpmbuild -ba ./SPECS/libmp3splt.spec &&\
rm -rf ./BUILD/* &&\
rm -rf ./SOURCES/* &&\
mv ./RPMS/$ARCH/*.rpm ../.. &&\
mv ./SRPMS/*.rpm ../..
