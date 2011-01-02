#/**********************************************************
# *
# * libmp3splt -- library based on mp3splt,
# *               for mp3/ogg splitting without decoding
# *
# * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
# * Copyright (c) 2005-2010 Munteanu Alexandru - io_fx@yahoo.fr
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
AC_INIT([libmp3splt],[0.6.1],[io_fx@yahoo.fr])
AC_CONFIG_AUX_DIR([libltdl/config])
AM_INIT_AUTOMAKE(libmp3splt, 0.6.1)
AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE([enable])

DEBUG_FLAGS="-g"
#OPTIMISE_FLAGS="-O2"
#WARNING_FLAGS="-Wall -Wextra -Wswitch-enum -Wswitch-default -Wfloat-equal -Wbad-function-cast -Wcast-qual -Wunreachable-code"
WARNING_FLAGS="-Wall"
CFLAGS="$DEBUG_FLAGS $WARNING_FLAGS $OPTIMISE_FLAGS $CFLAGS"

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

ifdef([LT_CONFIG_LTDL_DIR],
      [ LT_CONFIG_LTDL_DIR([libltdl]) ],
      [])

ifdef([LT_INIT],
      [ LT_INIT([win32-dll]) ],
      [ AC_PROG_LIBTOOL ])

AC_WITH_LTDL

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
    AM_CONDITIONAL(WIN32, true)
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
# helper for windows compilation
#################################################################

AM_CONDITIONAL(IS_ON_WINDOWS, false)

AC_ARG_ENABLE(win, [AC_HELP_STRING([--enable-win],[ Enable if compiling on windows with mingw. ]) ],
    [enable_win=$enableval],[enable_win="no"])

if test "x$enable_win" = xyes;then
  AM_CONDITIONAL(IS_ON_WINDOWS, true)
fi

#################################################################
# Check mp3 plugin libraries
#################################################################

#check for --disable-mp3 & mad library
AM_CONDITIONAL(MP3_PLUGIN,false)
mp3="no" 
AC_ARG_ENABLE(mp3, [AC_HELP_STRING([--disable-mp3],[ Disable mp3 plugin. ]) ], 
    [enable_mp3_plugin=$enableval],[enable_mp3_plugin="yes"])

if test "x$enable_mp3_plugin" = xyes;then
  MAD_CHECK([ AM_CONDITIONAL(MP3_PLUGIN,true) mp3="yes" ],
      [ AC_MSG_WARN([libmad not found ! Will build without mp3 support.]) ])
fi

#check for --disable-id3tag & id3tag library
AM_CONDITIONAL(ID3TAG,false)
id3="no";
AC_ARG_ENABLE(id3tag, [AC_HELP_STRING([--disable-id3tag],[ Disable id3tag support for the mp3 plugin. ]) ],
    [enable_id3tag=$enableval],[enable_id3tag="yes"])

if test "x$enable_id3tag" = xyes;then
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

if test "x$enable_ogg_plugin" = xyes;then
  XIPH_PATH_OGG(,AC_MSG_WARN([Ogg not found! Download the latest version of libogg at http://www.xiph.org.]))
  XIPH_PATH_VORBIS(AM_CONDITIONAL(OGG_PLUGIN,true) ogg="yes",
      AC_MSG_WARN(Vorbis not found! Will build without ogg/vorbis support.))
fi

#################################################################
# Check for cutter
#################################################################

AM_CONDITIONAL(HAS_CUTTER, false)

AC_ARG_ENABLE(cutter, [AC_HELP_STRING([--disable-cutter],[ Disable Cutter unit testing. ]) ],
    [enable_cutter=$enableval],[enable_cutter="yes"])

if test "x$enable_cutter" = xyes;then

AC_CHECK_CUTTER

cutter_command="no"
AC_CHECK_FILE([$CUTTER], [cutter_command="yes"])

if test "x$CUTTER" != x;then
  if test "x$cutter_command" = xyes;then
    AM_CONDITIONAL(HAS_CUTTER, true)
  fi
fi

fi

#################################################################
# Check for pcre - perl compatible regular expressions
#################################################################

AM_CONDITIONAL(PCRE, false)
pcre="no"
AC_ARG_ENABLE(pcre, AC_HELP_STRING([--disable-pcre], [Disable pcre support to set tags from input filename. ]),
    [enable_pcre=$enableval],[enable_pcre="yes"])

if test "x$enable_pcre" = xyes;then
  PKG_PROG_PKG_CONFIG
  PKG_CHECK_MODULES([PCRE], [libpcre >= 1.0],
      [AM_CONDITIONAL(PCRE, true) pcre="yes" ],
      [AC_MSG_ERROR([libpcre is not installed. It is used for setting tags from input filename.

    Install it or configure with "--disable-pcre".])])
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
echo " Other :"

echo
echo "    pcre (for setting tags from the input filename)         $pcre"

if test "x$mp3" = "xyes"; then
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
# Documentation of the code using 'make doxygen'
# (but only if doxygen is installed)
#################################################

# Check for doxygen, so we can decide if we add it to the "doc" 
# make target.
AC_PATH_PROG([DOXYGEN],[doxygen])
AC_ARG_VAR(DOXYGEN,
           [Location of the 'doxygen' program; if found we can generate documentation for the C code])
AM_CONDITIONAL(HAVE_DOXYGEN, [ test "x$DOXYGEN" != "x" ])

# Check for dot, the tool that makes the nice little caller graphs for the
# doxygen documentation.
AC_PATH_PROG([GRAPHVIZ],[dot])
AC_ARG_VAR(GRAPHVIZ,
           [Location of the 'dot' program from the graphviz suite; if found we can generate caller graphs for the documentation of the C code])
AM_CONDITIONAL(HAVE_GRAPHVIZ, [ test "x$GRAPHVIZ" != "x" ])
if test x$HAVE_DOT = "xfalse"; then
   AC_MSG_WARN([*** dot from the graphviz package not found, inheritance diagrams will not be generated in code documentation])
fi

#################################################
# Generate Makefile
#################################################

AC_CONFIG_FILES([Makefile src/Makefile po/Makefile.in plugins/Makefile test/Makefile m4/Makefile doc/Makefile])
AC_OUTPUT

