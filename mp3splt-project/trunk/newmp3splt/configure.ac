#/**********************************************************
# *
# * mp3splt - utility using libmp3splt,
# *                for mp3/ogg splitting without decoding
# *
# * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
# * Copyright (c) 2005-2008 Munteanu Alexandru - io_fx@yahoo.fr
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
# GNU Library General Public License for more details.
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
AC_INIT(mp3splt, 2.2.1, mtrotta@users.sourceforge.net)

AC_CONFIG_SRCDIR([src/mp3splt.c])
AM_INIT_AUTOMAKE(mp3splt, 2.2.1)
AM_CONFIG_HEADER([config.h])


CFLAGS="$CFLAGS -I../libmp3splt/include/ -I../../libmp3splt/include/ \
-I../libmp3splt/include/libmp3splt/ -I../../libmp3splt/include/libmp3splt/"
LDFLAGS="$LDFLAGS -L../libmp3splt/src/.libs -L../libmp3splt/src \
-L../../libmp3splt/src/.libs/ -L../../libmp3splt/src/"

#################################################
# Check for programs
#################################################

AC_PROG_CC

#################################################################
# Check for the type of the host
#################################################################

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
		LIBS="$LIBS -lltdl -lwsock32"
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
#checking for --disable-libmp3splt_test string
AC_ARG_ENABLE(libmp3splt_test,
AC_HELP_STRING([--disable-libmp3splt_test],
	[Disables the libmp3splt test when doing configure)]),

#action if given
LIBMP3SPLT_TEST_DISABLED=1
,
#action if not given

#checking for libmp3splt
AC_CHECK_LIB(mp3splt, mp3splt_new_state,libmp3splt=yes,
        [AC_MSG_ERROR(libmp3splt version 0.5.1 needed :

Download the latest version of libmp3splt at http://mp3splt.sourceforge.net/
)])

LIBMP3SPLT_TEST_DISABLED=0
)

#################################################
#Check for --enable-oggsplt
AC_ARG_ENABLE(oggsplt_symlink,
AC_HELP_STRING([--enable-oggsplt],
[Enable symlink of oggsplt to mp3splt [[default=yes]] ]),
,enable_oggsplt_symlink=yes)
AM_CONDITIONAL(DO_SYMLINK, test x$enable_oggsplt_symlink = xyes)

#################################################
# Generate Makefiles
#################################################

AC_OUTPUT(Makefile src/Makefile doc/Makefile)

#################################################
# Output configuration
#################################################

echo
echo "---------------------------------------------------------"
echo "Configuration:"
echo
echo "    install path:            ${prefix}"
echo
echo "---------------------------------------------------------"
echo

