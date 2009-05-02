#/**********************************************************
# *
# * mp3splt-gtk - utility using libmp3splt,
# *                for mp3/ogg splitting without decoding
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
AC_INIT(mp3splt-gtk, 0.5.6, io_fx@yahoo.fr)

AC_CONFIG_SRCDIR([src/mp3splt-gtk.c])
AM_INIT_AUTOMAKE(mp3splt-gtk, 0.5.6)
AC_CONFIG_HEADER([config.h])

CFLAGS="-Wall $CFLAGS -I../libmp3splt/include/ -I../../libmp3splt/include/ \
-I../libmp3splt/include/libmp3splt/ -I../../libmp3splt/include/libmp3splt/"
LDFLAGS="$LDFLAGS -L../libmp3splt/src -L../libmp3splt/src/.libs \
-L../../libmp3splt/src/.libs/ -L../../libmp3splt/src/"

#################################################
# Check for programs
#################################################

AC_PROG_CC
#gettext
AC_CHECK_HEADERS([unistd.h])
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.13.1])

#################################################################
# Check for the type of the host
#################################################################

AC_MSG_CHECKING(the host)
AM_CONDITIONAL(NOT_WIN32,true)
case $host in
	*linux*)
		HOST="Linux-based system"
		;;
	*openbsd*)
		HOST="OpenBSD"
		LIBS="$LIBS `pkg-config --libs gtk+-2.0` -lpthread"
		CFLAGS="$CFLAGS `pkg-config --cflags gtk+-2.0`"
		;;
	*netbsd*)
		HOST="NetBSD"
		LIBS="$LIBS -D_nl_domain_bindings=libintl_nl_domain_bindings -lintl -lstdc++"
		;;
	*freebsd*)
		HOST="FreeBSD"
		LIBS="$LIBS -lpthread"
		;;
	*mingw*)
		AM_CONDITIONAL(NOT_WIN32,false)
		HOST="Mingw"
		LIBS="$LIBS -lwsock32 -lshlwapi -mwindows"
		#LIBS="$LIBS -lwsock32 -lshlwapi"
		CFLAGS="$CFLAGS -mms-bitfields -enable-stdcall-fixup"
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
# Check for libraries
#################################################

#################################################
#Check for GTK
AM_PATH_GTK_2_0(2.6.0,,AC_MSG_ERROR(mp3splt-gtk needs GTK+ >= 2.6.0))

#################################################
#Check for libmp3splt

MP3SPLT_CHECK(,[ AC_MSG_ERROR(libmp3splt version 0.5.5 needed :

Download the latest version of libmp3splt at http://mp3splt.sourceforge.net/
) ])

#################################################
#Check for audacious

AM_CONDITIONAL(AUDACIOUS,false)
audacious="no"
#checking for --disable-audacious & audacious library
AC_ARG_ENABLE(audacious,
AC_HELP_STRING([--disable-audacious], [Disable audacious player support. ]),
[AC_MSG_WARN([audacious support is disabled !])
audacious="disabled"]
,
#action if not given
[PKG_CHECK_MODULES(AUDACIOUS, [audacious >= 0.1],
  [AM_CONDITIONAL(AUDACIOUS,true) audacious="yes" ],
  [AC_MSG_ERROR([Audacious player development files are not installed.

    Install it or configure with "--disable-audacious".])])
])

#################################################
#Check for gstreamer

AM_CONDITIONAL(GSTREAMER,false)
gstreamer="no"
#checking for --disable-gstreamer & gstreamer library
AC_ARG_ENABLE(gstreamer,
AC_HELP_STRING([--disable-gstreamer], [Disable gstreamer 'internal' player support. ]),
[AC_MSG_WARN([gstreamer support is disabled !])
gstreamer="disabled"]
,
#action if not given
[PKG_CHECK_MODULES(GST, [gstreamer-0.10 >= 0.10
                         gstreamer-base-0.10 >= 0.10
                         gstreamer-plugins-base-0.10 >= 0.10],
  [AM_CONDITIONAL(GSTREAMER,true) gstreamer="yes"],
  [AC_MSG_ERROR([GStreamer >= 0.10 development files are not installed.

    Install it or configure with "--disable-gstreamer".])])
])

#################################################
# Generate Makefiles
#################################################

AC_OUTPUT(Makefile src/Makefile m4/Makefile po/Makefile.in doc/Makefile)

#################################################
# Output configuration
#################################################

echo
echo "---------------------------------------------------------"
echo "Configuration:"
echo
echo "    install path:            ${prefix}"
echo
echo " Players :"
echo
echo "    gstreamer                $gstreamer"
echo "    audacious                $audacious"
echo "    snackAmp                 yes"
echo
echo "---------------------------------------------------------"
echo

