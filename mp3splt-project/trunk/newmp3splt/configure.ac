#/**********************************************************
# *
# * mp3splt - utility using libmp3splt,
# *                for mp3/ogg splitting without decoding
# *
# * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
# * Copyright (c) 2005-2014 Munteanu Alexandru - m@ioalex.net
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
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
#
# *********************************************************/

#################################################
# Basic initialisations 
#################################################

m4_define([minimum_libmp3splt_version],[0.9.3.1518])

AC_PREREQ(2.56)
AC_INIT(mp3splt, 2.6.3.1518, mtrotta@users.sourceforge.net)

AC_CONFIG_SRCDIR([src/mp3splt.c])
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER([config.h])
AM_MAINTAINER_MODE([enable])

#################################################
# Check for programs
#################################################

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S

AC_CHECK_HEADERS([unistd.h])
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.18.1])

AC_DEFINE_UNQUOTED([LIBMP3SPLT_WITH_SONAME], "libmp3splt0", [libmp3splt library with soname])

#################################################################
# Check for functions
#################################################################

AC_CHECK_FUNCS([getline])

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
		LIBS="$LIBS -lsocket"
		;;
	*)
		HOST="Other"
		LIBS="$LIBS"
		;;
esac
AC_MSG_RESULT($HOST)

#################################################
#Check for libmp3splt

PKG_CHECK_MODULES([LIBMP3SPLT], [libmp3splt = minimum_libmp3splt_version],
[],
[ AC_MSG_ERROR(libmp3splt version minimum_libmp3splt_version needed :

Download the latest version of libmp3splt at http://mp3splt.sourceforge.net/
) ])

#################################################
#Check for --enable-oggsplt_symlink

AC_ARG_ENABLE(oggsplt_symlink,
AC_HELP_STRING([--enable-oggsplt_symlink], [Enable symlink of oggsplt to mp3splt. ]),
[enable_ogg_symlink=$enableval],[enable_ogg_symlink="no"])

if test "x$enable_ogg_symlink" = xyes;then
  AM_CONDITIONAL(DO_OGG_SYMLINK, true)
else
  AM_CONDITIONAL(DO_OGG_SYMLINK, false)
fi

#################################################
#Check for --enable-flacsplt_symlink

AC_ARG_ENABLE(flacsplt_symlink,
AC_HELP_STRING([--enable-flacsplt_symlink], [Enable symlink of flacsplt to mp3splt. ]),
[enable_flac_symlink=$enableval],[enable_flac_symlink="no"])

if test "x$enable_flac_symlink" = xyes;then
  AM_CONDITIONAL(DO_FLAC_SYMLINK, true)
else
  AM_CONDITIONAL(DO_FLAC_SYMLINK, false)
fi


#################################################################
# Check for some debugging, warnings and optimise options
#################################################################

AC_ARG_ENABLE(c-debug, [AC_HELP_STRING([--enable-c-debug],[ Enable debugging symbols. ]) ],
    [enable_c_debug=$enableval],[enable_c_debug="no"])
if test "x$enable_c_debug" = xyes;then
  CFLAGS="$CFLAGS -g -Wall"
fi

AC_ARG_ENABLE(optimise, [AC_HELP_STRING([--enable-optimise],[ Enable O3 optimise. ]) ],
    [enable_optimise=$enableval],[enable_optimise="no"])
if test "x$enable_optimise" = xyes;then
  CFLAGS="$CFLAGS -O3"
fi

AC_ARG_ENABLE(extra-warnings, [AC_HELP_STRING([--enable-extra-warnings],[ Enable extra warnings. ]) ],
    [enable_extra_warnings=$enableval],[enable_extra_warnings="no"])
if test "x$enable_extra_warnings" = xyes;then
  CFLAGS="$CFLAGS -Wall -Wextra -Wswitch-enum -Wswitch-default -Wfloat-equal -Wbad-function-cast -Wcast-qual -Wunreachable-code"
fi

#################################################
# Generate Makefiles
#################################################

AC_OUTPUT(Makefile src/Makefile po/Makefile.in doc/Makefile)

#################################################
# Output configuration
#################################################

echo
echo "---------------------------------------------------------"
echo "Configuration:"
echo
echo "    install path:            ${prefix}"
echo "    oggsplt symlink:         ${enable_ogg_symlink}"
echo "    flacsplt symlink:        ${enable_flac_symlink}"
echo
echo "---------------------------------------------------------"
echo

