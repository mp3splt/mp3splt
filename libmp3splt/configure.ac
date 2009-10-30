#/**********************************************************
# *
# * libmp3splt -- library based on mp3splt,
# *               for mp3/ogg splitting without decoding
# *
# * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
# * Copyright (c) 2005-2009 Munteanu Alexandru - io_fx@yahoo.fr
# *
# * http://mp3splt.sourceforge.net
# *
# *********************************************************/

#/**********************************************************
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
#
# *********************************************************/

#################################################
# Basic initialisations 
#################################################

AC_PREREQ(2.56)
AC_INIT([libmp3splt],[0.5.8],[io_fx@yahoo.fr])
AM_INIT_AUTOMAKE(libmp3splt, 0.5.8)
AC_CONFIG_HEADERS([config.h])

#DEBUG_FLAGS="-g"
OPTIMISE_FLAGS="-O2"
#WARNING_FLAGS="-Wall -Wextra -Wswitch-enum -Wswitch-default -Wfloat-equal -Wbad-function-cast -Wcast-qual -Wunreachable-code"
WARNING_FLAGS="-Wall"
CFLAGS="$DEBUG_FLAGS $WARNING_FLAGS $OPTIMISE_FLAGS $CFLAGS -I../include/libmp3splt"

#################################################
# Check for programs
#################################################

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S

AC_CHECK_HEADERS([unistd.h])
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.13.1])

#################################################################
# Checks for typedefs, structures, and compiler characteristics
#################################################################

AC_C_BIGENDIAN
AC_C_CONST

#################################################################
# Enable libtool, check for libltdl and define @LIBLTDL@ and @INCLTDL@
#################################################################

#LT_INIT([dlopen])
#LTDL_INIT
AC_WITH_LTDL
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

#################################################################
# Check for the type of the host
#################################################################

AM_CONDITIONAL(WIN32,false)
AC_MSG_CHECKING(the host)
case $host in
  *linux*)
    HOST="Linux-based system"
		;;
	*openbsd*)
		HOST="OpenBSD"
		LIBS="$LIBS -lm"
		;;
	*netbsd*)
		HOST="NetBSD"
		;;
	*freebsd*)
		HOST="FreeBSD"
		;;
	*mingw*)
		HOST="Mingw"
    AM_CONDITIONAL(WIN32,true)
		;;
	*solaris*)
		HOST="Solaris"
		;;
	*)
		HOST="Other"
		;;
esac
AC_MSG_RESULT($HOST)

#################################################################
# Check for some functions
#################################################################

AC_FUNC_FSEEKO
AC_CHECK_FUNCS(fseeko, [ CFLAGS="$CFLAGS -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64" ],
               [ AC_MSG_WARN([fseeko() function is missing: FILE > 2 GB SUPPORT DISABLED!]) ])

#################################################
# Check for libraries
#################################################

#check for -lm
AC_CHECK_LIB(m, pow, [ haslm="yes" ] , [ AC_MSG_ERROR([libm not found !]) ] )

#check for -ltld
AC_CHECK_LIB(ltdl, lt_dlopen, [ hasltdl="yes" ], [ AC_MSG_ERROR([libltdl not found - check libtool installation !]) ] ) 

#################################################################
# Check mp3 plugin libraries
#################################################################

#check for --disable-mp3 & mad library
AM_CONDITIONAL(MP3_PLUGIN,false)
mp3="no" 
AC_ARG_ENABLE(mp3, [AC_HELP_STRING([--disable-mp3],[ Disable mp3 plugin. ]) ], 
    [enable_mp3_plugin=$enableval],[enable_mp3_plugin="yes"])

if test x$enable_mp3_plugin = xyes;then
  MAD_CHECK([ AM_CONDITIONAL(MP3_PLUGIN,true) mp3="yes" ],
      [ AC_MSG_WARN([libmad not found ! Will build without mp3 support.]) ])
fi

#check for --disable-id3tag & id3tag library
AM_CONDITIONAL(ID3TAG,false)
id3="no";
AC_ARG_ENABLE(id3tag, [AC_HELP_STRING([--disable-id3tag],[ Disable id3tag support for the mp3 plugin. ]) ],
    [enable_id3tag=$enableval],[enable_id3tag="yes"])

if test x$enable_id3tag = xyes;then
  ID3_CHECK([ AM_CONDITIONAL(ID3TAG,true) id3="yes"; ],
      [ AC_MSG_WARN([libid3tag not found ! Will build without id3 support.]) ])
fi


#################################################################
# Check ogg plugin libraries
#################################################################

#check for --disable-ogg & ogg libraries
AM_CONDITIONAL(OGG_PLUGIN,false)
ogg="no" 
AC_ARG_ENABLE(ogg, [AC_HELP_STRING([--disable-ogg],[ Disable Ogg Vorbis plugin. ]) ],
    [enable_ogg_plugin=$enableval],[enable_ogg_plugin="yes"])

if test x$enable_ogg_plugin = xyes;then
  XIPH_PATH_OGG(,AC_MSG_WARN([Ogg not found! Download the latest version of libogg at http://www.xiph.org.]))
  XIPH_PATH_VORBIS(AM_CONDITIONAL(OGG_PLUGIN,true) ogg="yes",
      AC_MSG_WARN(Vorbis not found! Will build without ogg/vorbis support.))
fi


#################################################
# Print the configuration to the screen
#################################################

echo
echo "---------------------------------------------------------"
echo "Configuration:"
echo
echo "    install path:            $prefix"
echo
echo " Plugins :"
echo
echo "    libmad (.mp3 format)                                    $mp3"
echo "    libogg, libvorbis (.ogg/vorbis format)                  $ogg"
echo
if test "x$mp3" = "xyes"; then
  echo " Other :"
  echo
  echo "    libid3tag (for handling id3 tags)                       $id3"
  if test "x$id3" = "xno" && test "x$with_id3" = "xyes"; then
    echo
    echo "    install libid3tag from http://sourceforge.net/projects/id3/"
    echo "    to have this feature supported"
  fi
  echo
fi
echo "---------------------------------------------------------"
echo

#################################################
# Generate Makefile
#################################################

AC_CONFIG_FILES([Makefile src/Makefile po/Makefile.in plugins/Makefile m4/Makefile])
AC_OUTPUT

