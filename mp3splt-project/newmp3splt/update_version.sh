#!/bin/bash

#this script is used to update the version of the package

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir
cd $PROGRAM_DIR

. ./include_variables.sh "quiet"

#the program version, that we have in the $MP3SPLT_VERSION variable
VERSION=$MP3SPLT_VERSION
PROGRAM="mp3splt"

#debian changelog
if ! debchange --distribution "testing" -v $VERSION "version "$VERSION 2>/dev/null;then
    rm -f debian/.changelog.dch.swp
    debchange -r "version "$VERSION
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
#rpm global Version
sed -i "s/Version: .*/Version: $VERSION/" ./rpm/SPECS/$PROGRAM.spec
#arch global version
sed -i "s/pkgver=.*/pkgver=$VERSION/" ./arch/PKGBUILD

#current date, we need it
DATE=$(date +%d\\/%m\\/%y)
NEW_LIBMP3SPLT_VER=${LIBMP3SPLT_VERSION//./_}

#debian control file, libmp3splt dependency
#./debian/control:Build-Depends: debhelper (>= 4.0.0), libmp3splt (= 0.3.1)
#./debian/control:Depends: ${shlibs:Depends}, libmp3splt (= 0.3.1)
sed -i "s/libmp3splt (= .*)/libmp3splt (= $LIBMP3SPLT_VERSION)/" ./debian/control
#windows installer
#./other/win32_installer.nsi:!define VERSION "2.2.1"
sed -i "s/!define VERSION \".*\"/!define VERSION \"$VERSION\"/" ./other/win32_installer.nsi
#configure.ac libmp3splt version check
#./configure.ac:AC_CHECK_LIB(mp3splt, mp3splt_v0_3_5,libmp3splt=yes,
#./configure.ac:        [AC_MSG_ERROR(libmp3splt version 0.3.5 needed :
sed -i "s/AC_CHECK_LIB(mp3splt, mp3splt_v.*,l/\
AC_CHECK_LIB(mp3splt, mp3splt_v$NEW_LIBMP3SPLT_VER,l/" ./configure.ac
sed -i "s/\[AC_MSG_ERROR(libmp3splt version .* needed/\
\[AC_MSG_ERROR(libmp3splt version $LIBMP3SPLT_VERSION needed/" ./configure.ac
#source code
#./src/mp3splt.c:#define VERSION "2.2"
sed -i "s/#define VERSION \".*\"/#define VERSION \"$VERSION\"/" ./src/mp3splt.c
#./src/mp3splt.c:#define MP3SPLT_DATE "14/04/2006"
sed -i "s/#define MP3SPLT_DATE \".*\"/#define MP3SPLT_DATE \"$DATE\"/" ./src/mp3splt.c
#update gentoo ebuild
cd gentoo/media-sound/$PROGRAM
if [[ $SUBVERSION == 1 ]];then
    svn mv $PROGRAM* $PROGRAM-$VERSION.ebuild 2>/dev/null
    svn ci -m "updated gentoo version" &>/dev/null
else
    mv $PROGRAM* $PROGRAM-$VERSION.ebuild 2>/dev/null
fi
sed -i "s/media-libs\/libmp3splt-.*/media-libs\/libmp3splt-$LIBMP3SPLT_VERSION/" ./$PROGRAM-$VERSION.ebuild
#slackware description
cd ../../../slackware
sed -i "s/libmp3splt version .*/libmp3splt version $LIBMP3SPLT_VERSION,/" ./slack-desc
cd ..
#rpm libmp3splt Requires
sed -i "s/libmp3splt = .*/libmp3splt = $LIBMP3SPLT_VERSION/" ./rpm/SPECS/$PROGRAM.spec
#arch libmp3splt depends
sed -i "s/libmp3splt=.*'/libmp3splt=${LIBMP3SPLT_VERSION}'/" ./arch/PKGBUILD

echo "Finished setting up $PROGRAM to version $VERSION with\
 libmp3splt version $LIBMP3SPLT_VERSION."
