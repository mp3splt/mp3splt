#!/bin/bash

#this script is used to update the version of the package

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir
cd $PROGRAM_DIR

. ./include_variables.sh "quiet"

#the program version, that we have in the $LIBMP3SPLT_VERSION variable
VERSION=$LIBMP3SPLT_VERSION
PROGRAM="libmp3splt"

#debian changelog
if ! debchange --distribution "testing" -v $VERSION "version "$VERSION 2>/dev/null;then
    rm -f debian/.changelog.dch.swp
    debchange -a "version "$VERSION
fi

#README
#./README:       libmp3splt version 0.3.1
sed -i "s/\s*$PROGRAM version.*/\t$PROGRAM version $VERSION/" README
#configure.ac
#./configure.ac:AC_INIT(libmp3splt, 0.3.1, io_alex_2002@yahoo.fr)
#./configure.ac:AM_INIT_AUTOMAKE(libmp3splt, 0.3.1)
sed -i "s/AC_INIT($PROGRAM, .*,/\
AC_INIT($PROGRAM, $VERSION,/" ./configure.ac
sed -i "s/AM_INIT_AUTOMAKE($PROGRAM, .*)/\
AM_INIT_AUTOMAKE($PROGRAM, $VERSION)/" ./configure.ac    

NEW_LIBMP3SPLT_VER=${LIBMP3SPLT_VERSION//./_}

#libmp3splt source code
#./src/mp3splt_types.h:#define SPLT_PACKAGE_VERSION "0.3.1"
sed -i "s/#define SPLT_PACKAGE_VERSION \".*\"/\
#define SPLT_PACKAGE_VERSION \"$VERSION\"/" ./src/mp3splt_types.h
#./src/mp3splt.c:void mp3splt_v0_3_1
sed -i "s/void mp3splt_v.*/void mp3splt_v$NEW_LIBMP3SPLT_VER()/" ./src/mp3splt.c
#./src/Doxyfile:PROJECT_NUMBER=0.3.1
sed -i "s/PROJECT_NUMBER=.*/PROJECT_NUMBER=$VERSION/" ./src/Doxyfile

echo "Finished setting up $PROGRAM to version $VERSION."
