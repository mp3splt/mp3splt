#!/bin/sh

SUBVERSION=0;

#print usage and exit function
function print_usage_exit_error()
{
    echo "Usage: $0 PROGRAM VERSION LIBMP3SPLT_VERSION"
    echo $'\tPROGRAM can be : libmp3splt, mp3splt or mp3splt-gtk'
    exit 1;
}

#check number of arguments
if [[ $# != 3 ]];
then
    print_usage_exit_error;
fi;

#we check the $PROGRAM argument
if [[ $1 == "libmp3splt" ]] || [[ $1 == "mp3splt" ]] || 
    [[ $1 == "mp3splt-gtk" ]];then
    #we get the program and the version
    PROGRAM=$1;
    VERSION=$2;
    LIBMP3SPLT_VERSION=$3;
    
    #we move in the current script directory
    script_dir=$(readlink -f $0)
    script_dir=${script_dir%\/*.sh}
    cd $script_dir/
    
    #common changes
    #we do the debian changelog
    if [[ $1 == "mp3splt" ]]; then
        cd ../newmp3splt
    else
        cd ../$PROGRAM
    fi;
    if ! debchange --distribution "testing" -v $VERSION "version "$VERSION 2>/dev/null;then
        rm -f debian/.changelog.dch.swp;
        debchange -r "version "$VERSION;
    fi
    
    #README
    #./README:       libmp3splt version 0.3.1
    sed -i "s/\s*$PROGRAM version.*/\t$PROGRAM version $VERSION/" README;
    #configure.ac
    #./configure.ac:AC_INIT(libmp3splt, 0.3.1, io_alex_2002@yahoo.fr)
    #./configure.ac:AM_INIT_AUTOMAKE(libmp3splt, 0.3.1)
    sed -i "s/AC_INIT($PROGRAM, .*,/\
AC_INIT($PROGRAM, $VERSION,/" ./configure.ac;
    sed -i "s/AM_INIT_AUTOMAKE($PROGRAM, .*)/\
AM_INIT_AUTOMAKE($PROGRAM, $VERSION)/" ./configure.ac;    
    
    #current date, we need it
    DATE=$(date +%d\\/%m\\/%y);
    NEW_LIBMP3SPLT_VER=${LIBMP3SPLT_VERSION//./_}
    
    case $PROGRAM in
        #libmp3splt settings
        "libmp3splt") 
            #libmp3splt source code
            #./src/mp3splt_types.h:#define SPLT_PACKAGE_VERSION "0.3.1"
            sed -i "s/#define SPLT_PACKAGE_VERSION \".*\"/\
#define SPLT_PACKAGE_VERSION \"$VERSION\"/" ./src/mp3splt_types.h;
            #./src/mp3splt.c:void mp3splt_v0_3_1
            sed -i "s/void mp3splt_v.*/void mp3splt_v$NEWVER()/" ./src/mp3splt.c;
            #./src/Doxyfile:PROJECT_NUMBER=0.3.1
            sed -i "s/PROJECT_NUMBER=.*/PROJECT_NUMBER=$VERSION/" ./src/Doxyfile;
            #update gentoo ebuild
            cd gentoo/media-libs/$PROGRAM
            if [[ $SUBVERSION == 1 ]];then
                svn mv $PROGRAM* $PROGRAM-$VERSION.ebuild 2>/dev/null
                svn ci -m "updated gentoo version" &>/dev/null
            else
                mv $PROGRAM* $PROGRAM-$VERSION.ebuild 2>/dev/null
            fi;
            ;;
        #mp3splt settings
        "mp3splt")
            #debian control file, libmp3splt dependency
            #./debian/control:Build-Depends: debhelper (>= 4.0.0), libmp3splt (= 0.3.1)
            #./debian/control:Depends: ${shlibs:Depends}, libmp3splt (= 0.3.1)
            sed -i "s/libmp3splt (= .*)/libmp3splt (= $LIBMP3SPLT_VERSION)/" ./debian/control;
            #windows installer
            #./other/win32_installer.nsi:!define VERSION "2.2.1"
            sed -i "s/!define VERSION \".*\"/!define VERSION \"$VERSION\"/" ./other/win32_installer.nsi;
            #source code
            #./src/mp3splt.c:#define VERSION "2.2"
            sed -i "s/#define VERSION \".*\"/#define VERSION \"$VERSION\"/" ./src/mp3splt.c;
            #./src/mp3splt.c:#define MP3SPLT_DATE "14/04/2006"
            sed -i "s/#define MP3SPLT_DATE \".*\"/#define MP3SPLT_DATE \"$DATE\"/" ./src/mp3splt.c;
            #update gentoo ebuild
            cd gentoo/media-sound/$PROGRAM
            if [[ $SUBVERSION == 1 ]];then
                svn mv $PROGRAM* $PROGRAM-$VERSION.ebuild 2>/dev/null
                svn ci -m "updated gentoo version" &>/dev/null
            else
                mv $PROGRAM* $PROGRAM-$VERSION.ebuild 2>/dev/null
            fi;
            sed -i "s/media-libs\/libmp3splt-.*/media-libs\/libmp3splt-$LIBMP3SPLT_VERSION/" ./$PROGRAM-$VERSION.ebuild;
            ;;
        #mp3splt-gtk settings
        "mp3splt-gtk")
            #windows installer
            #./other/win32_installer.nsi:!define VERSION "0.3.1"
            sed -i "s/!define VERSION \".*\"/!define VERSION \"$VERSION\"/" ./other/win32_installer.nsi;
            #debian control file, libmp3splt dependency
            #./debian/control:Build-Depends: debhelper (>= 4.0.0), libmp3splt (= 0.3.1), beep-media-player-dev(>= 0.9.7-1)
            #./debian/control:Depends: ${shlibs:Depends}, libmp3splt (= 0.3.1), beep-media-player(>= 0.9.7-1)
            sed -i "s/libmp3splt (= .*)/libmp3splt (= $LIBMP3SPLT_VERSION)/" ./debian/control;
            #configure.ac libmp3splt version check
            #./configure.ac:AC_CHECK_LIB(mp3splt, mp3splt_v0_3_5,libmp3splt=yes,
            #./configure.ac:        [AC_MSG_ERROR(libmp3splt version 0.3.5 needed :
            sed -i "s/AC_CHECK_LIB(mp3splt, mp3splt_v.*,/\
AC_CHECK_LIB(mp3splt, mp3splt_v$NEW_LIBMP3SPLT_VER,/" ./configure.ac;
            sed -i "s/\[AC_MSG_ERROR(libmp3splt version .* needed/\
\[AC_MSG_ERROR(libmp3splt version $LIBMP3SPLT_VERSION needed/" ./configure.ac;
            #source code
            #./src/main_win.c:#define VERSION "0.3.1"
            #./src/main_win.c:  g_snprintf(b3, 100, "-release of 27/02/06-\n%s libmp3splt...
            sed -i "s/#define VERSION \".*\"/#define VERSION \"$VERSION\"/" ./src/main_win.c;
            sed -i "s/release of .* libmp3splt/release of $DATE-\\\n%s libmp3splt/" ./src/main_win.c;
            #update gentoo ebuild
            cd gentoo/media-sound/$PROGRAM
            if [[ $SUBVERSION == 1 ]];then
                svn mv $PROGRAM* $PROGRAM-$VERSION.ebuild 2>/dev/null
                svn ci -m "updated gentoo version" &>/dev/null
            else
                mv $PROGRAM* $PROGRAM-$VERSION.ebuild 2>/dev/null
            fi;
            sed -i "s/media-libs\/libmp3splt-.*\"/media-libs\/libmp3splt-$LIBMP3SPLT_VERSION\"/" ./$PROGRAM-$VERSION.ebuild;
            ;;
    esac    
else
    print_usage_exit_error;
fi;

echo "Finished setting up version $PROGRAM for version $VERSION with libmp3splt version $LIBMP3SPLT_VERSION";

#dont forget to modify the ChangeLog, News
