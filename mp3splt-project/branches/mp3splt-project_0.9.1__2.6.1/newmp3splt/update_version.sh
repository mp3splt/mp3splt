#!/bin/bash

#this script is used to update the version of the package

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir
cd $PROGRAM_DIR

. ./include_variables.sh "quiet_noflags" || exit 1

#the program version, that we have in the $MP3SPLT_VERSION variable
VERSION=$MP3SPLT_VERSION
PROGRAM="mp3splt"

#debian changelog
sed -i "1,4s/mp3splt (\(.*\))/mp3splt ($VERSION)/" ./debian/changelog

#README
#./README:       libmp3splt version 0.3.1
sed -i "s/\s*$PROGRAM version.*/\t$PROGRAM version $VERSION/" README || exit 1
#configure.ac
#./configure.ac:AC_INIT(libmp3splt, 0.3.1, io_alex_2002@yahoo.fr)
sed -i "s/AC_INIT($PROGRAM, .*,/\
AC_INIT($PROGRAM, $VERSION,/" ./configure.ac || exit 1

#configure.ac libmp3splt version
sed -i "s/minimum_libmp3splt_version\],\[.*\]/minimum_libmp3splt_version],[$LIBMP3SPLT_VERSION]/" ./configure.ac || exit 1

#source code
DATE=$(date +%d\\/%m\\/%y)
#./src/common.h:#define VERSION "2.2"
sed -i "s/#define VERSION \".*\"/#define VERSION \"$VERSION\"/" ./src/common.h || exit 1
#./src/common.h:#define MP3SPLT_DATE "14/04/2006"
sed -i "s/#define MP3SPLT_DATE \".*\"/#define MP3SPLT_DATE \"$DATE\"/" ./src/common.h || exit 1

echo "Finished setting up $PROGRAM to version $VERSION with\
 libmp3splt version $LIBMP3SPLT_VERSION."

