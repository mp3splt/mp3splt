#!/bin/bash

#this script is used to update the version of the package

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir
cd $PROGRAM_DIR

. ./include_variables.sh "quiet_noflags"

#the program version, that we have in the $MP3SPLT_VERSION variable
VERSION=$MP3SPLT_GTK_VERSION
PROGRAM="mp3splt-gtk"

sed -i "1,4s/mp3splt-gtk (\(.*\))/mp3splt-gtk ($VERSION)/" ./debian/changelog

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

#configure.ac libmp3splt version check
#./configure.ac:        [AC_MSG_ERROR(libmp3splt version 0.3.5 needed :
sed -i "s/\[AC_MSG_ERROR(libmp3splt version .* needed/\
\[AC_MSG_ERROR(libmp3splt version $LIBMP3SPLT_VERSION needed/" ./configure.ac || exit 1
#source code
#./src/main_win.c:#define VERSION "0.3.1"
#./src/main_win.c:  g_snprintf(b3, 100, "-release of 27/02/06-\n%s libmp3splt...
sed -i "s/#define VERSION \".*\"/#define VERSION \"$VERSION\"/" ./src/main_win.c || exit 1
sed -i "s/release of .* libmp3splt/release of $DATE-\\\n%s libmp3splt/" ./src/main_win.c || exit 1

#slackware description
cd ./slackware
sed -i "s/libmp3splt version .*/libmp3splt version $LIBMP3SPLT_VERSION,/" ./slack-desc || exit 1

echo "Finished setting up $PROGRAM to version $VERSION with\
 libmp3splt version $LIBMP3SPLT_VERSION."
