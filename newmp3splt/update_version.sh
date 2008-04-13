#!/bin/bash

#this script is used to update the version of the package

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir
cd $PROGRAM_DIR

. ./include_variables.sh "quiet_noflags"

#the program version, that we have in the $MP3SPLT_VERSION variable
VERSION=$MP3SPLT_VERSION
PROGRAM="mp3splt"

#debian changelog
if ! debchange --distribution "testing" -v $VERSION "version "$VERSION 2>/dev/null;then
    rm -f debian/.changelog.dch.swp
    debchange -r "version "$VERSION || exit 1
fi

#README
#./README:       libmp3splt version 0.3.1
sed -i "s/\s*$PROGRAM version.*/\t$PROGRAM version $VERSION/" README || exit 1
#configure.ac
#./configure.ac:AC_INIT(libmp3splt, 0.3.1, io_alex_2002@yahoo.fr)
#./configure.ac:AM_INIT_AUTOMAKE(libmp3splt, 0.3.1)
sed -i "s/AC_INIT($PROGRAM, .*,/\
AC_INIT($PROGRAM, $VERSION,/" ./configure.ac || exit 1
sed -i "s/AM_INIT_AUTOMAKE($PROGRAM, .*)/\
AM_INIT_AUTOMAKE($PROGRAM, $VERSION)/" ./configure.ac || exit 1   

#current date, we need it
DATE=$(date +%d\\/%m\\/%y)
NEW_LIBMP3SPLT_VER=${LIBMP3SPLT_VERSION//./_}

#debian control file, libmp3splt dependency
#./debian/control:Build-Depends: debhelper (>= 4.0.0), libmp3splt (= 0.3.1)
#./debian/control:Depends: ${shlibs:Depends}, libmp3splt (= 0.3.1)
sed -i "s/libmp3splt (= .*)/libmp3splt (= $LIBMP3SPLT_VERSION)/" ./debian/control || exit 1
#windows installer
#./windows/installer/win32_installer.nsi:!define VERSION "2.2.1"
sed -i "s/!define VERSION \".*\"/!define VERSION \"$VERSION\"/" ./windows/installer/win32_installer.nsi || exit 1
#configure.ac libmp3splt version check
#./configure.ac:AC_CHECK_LIB(mp3splt, mp3splt_v0_3_5,libmp3splt=yes,
#./configure.ac:        [AC_MSG_ERROR(libmp3splt version 0.3.5 needed :
sed -i "s/AC_CHECK_LIB(mp3splt, mp3splt_v.*,l/\
AC_CHECK_LIB(mp3splt, mp3splt_v$NEW_LIBMP3SPLT_VER,l/" ./configure.ac || exit 1
sed -i "s/\[AC_MSG_ERROR(libmp3splt version .* needed/\
\[AC_MSG_ERROR(libmp3splt version $LIBMP3SPLT_VERSION needed/" ./configure.ac || exit 1
#source code
#./src/mp3splt.c:#define VERSION "2.2"
sed -i "s/#define VERSION \".*\"/#define VERSION \"$VERSION\"/" ./src/mp3splt.c || exit 1
#./src/mp3splt.c:#define MP3SPLT_DATE "14/04/2006"
sed -i "s/#define MP3SPLT_DATE \".*\"/#define MP3SPLT_DATE \"$DATE\"/" ./src/mp3splt.c || exit 1

#slackware description
cd ./slackware
sed -i "s/libmp3splt version .*/libmp3splt version $LIBMP3SPLT_VERSION,/" ./slack-desc || exit 1

echo "Finished setting up $PROGRAM to version $VERSION with\
 libmp3splt version $LIBMP3SPLT_VERSION."
