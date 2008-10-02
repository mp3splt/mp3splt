#/**********************************************************
# *
# * mp3splt-gtk - utility using libmp3splt,
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
AC_INIT(mp3splt-gtk, 0.5.1, io_fx@yahoo.fr)

AC_CONFIG_SRCDIR([src/mp3splt-gtk.c])
AM_INIT_AUTOMAKE(mp3splt-gtk, 0.5.1)
AC_CONFIG_HEADER([config.h])

CFLAGS="$CFLAGS -I../libmp3splt/include/ -I../../libmp3splt/include/ \
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
		HOST="Mingw"
		LIBS="$LIBS -lwsock32 -mwindows"
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
#checking for --disable-libmp3splt_test string
AC_ARG_ENABLE(libmp3splt_test,
AC_HELP_STRING([--disable-libmp3splt_test],
	[Disables the libmp3splt test when doing configure]),

#action if given
LIBMP3SPLT_TEST_DISABLED=1
,
#action if not given

#checking for libmp3splt
AC_CHECK_LIB(mp3splt, mp3splt_new_state,libmp3splt=yes,
        [AC_MSG_ERROR(libmp3splt version 0.5.1 needed :

You can download it at http://mp3splt.sourceforge.net/
)])

LIBMP3SPLT_TEST_DISABLED=0
)

#################################################
#Check for audacious
#checking for --enable-audacious string
AC_ARG_ENABLE(audacious,
AC_HELP_STRING([--enable-audacious], [Enable Audacious player support (default is disabled)]),
[PKG_CHECK_MODULES(AUDACIOUS, [audacious >= 0.1],
  [],
  [AC_MSG_ERROR([Audacious player development files are not installed.

    Install it or configure with "--disable-audacious".
])])
AM_CONDITIONAL(AUDACIOUS,true)
audacious=yes
]
,
#action if not given
[CFLAGS="$CFLAGS -DNO_AUDACIOUS"
AM_CONDITIONAL(AUDACIOUS,false)
audacious=no]
)

#################################################
#Check for gstreamer
#checking for --enable-gstreamer string
AC_ARG_ENABLE(gstreamer,
AC_HELP_STRING([--enable-gstreamer], [Enable GStreamer 'internal' player support (default is disabled)]),
[PKG_CHECK_MODULES(GST, [gstreamer-0.10 >= 0.10
                         gstreamer-base-0.10 >= 0.10
                         gstreamer-plugins-base-0.10 >= 0.10],
  [],
  [AC_MSG_ERROR([GStreamer >= 0.10 development files are not installed.

    Install it or configure with "--disable-gstreamer".])])
AM_CONDITIONAL(GSTREAMER,true)
gstreamer=yes
]
,
#action if not given
[
AC_MSG_WARN([gstreamer support is disabled !])
CFLAGS="$CFLAGS -DNO_GSTREAMER"
AM_CONDITIONAL(GSTREAMER,false)
gstreamer=no
])

#################################################
# Generate Makefiles
#################################################

AC_OUTPUT(Makefile src/Makefile m4/Makefile po/Makefile.in)

#################################################
# Output configuration
#################################################

echo
echo "---------------------------------------------------------"
echo "Configuration:"
echo
echo "    install path:            ${prefix}"
echo
echo " Players supported :"
echo
echo "    gstreamer                $gstreamer"
echo "    audacious                $audacious"
if test $audacious = "no"; then
  echo "        Please run configure with '--enable-audacious' to enable audacious support !"
fi
echo "    snackAmp                 yes"
echo
echo "---------------------------------------------------------"
echo

