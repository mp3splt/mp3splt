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
AC_INIT(mp3splt-gtk, 0.6.1, io_fx@yahoo.fr)

# tell which file by being there indicates we are run from the
# right directory
AC_CONFIG_SRCDIR([src/mp3splt-gtk.c])

# tell autoconf where to put the temporary files and scripts
# it generates. Seems to be more clean to put them into a subdir;
# One of this files is gnome-doc-utils.make, which is needed to 
# compile the help files.
AC_CONFIG_AUX_DIR([build-aux])

AM_INIT_AUTOMAKE(mp3splt-gtk, 0.6.1)
AC_CONFIG_HEADER([config.h])
AM_MAINTAINER_MODE([enable])

#DEBUG_FLAGS="-g"
OPTIMISE_FLAGS="-O2"
#WARNING_FLAGS="-Wall -Wextra -Wswitch-enum -Wswitch-default -Wfloat-equal -Wbad-function-cast -Wcast-qual -Wunreachable-code"
WARNING_FLAGS="-Wall"
CFLAGS="$DEBUG_FLAGS $WARNING_FLAGS $OPTIMISE_FLAGS $CFLAGS -I../libmp3splt/include/ -I../../libmp3splt/include/"
LDFLAGS="$LDFLAGS -L../libmp3splt/src/.libs -L../../libmp3splt/src/.libs/"

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
AM_CONDITIONAL(WIN32,false)
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
		AM_CONDITIONAL(WIN32,true)
		;;
	*solaris*)
		HOST="Solaris"
		LIBS="$LIBS -lsocket"
		;;
	*)
		HOST="Other"
		;;
esac
AC_MSG_RESULT($HOST)

#################################################
# Check for libraries
#################################################

#check for -lm
AC_CHECK_LIB(m, pow, [ haslm="yes" ] , [ AC_MSG_ERROR([libm not found !]) ] )

#check for libgnome, the library for displaying the "help"
#dialogue
#AC_CHECK_LIB(gnome-2,gnome_help_display_uri,, [ AC_MSG_ERROR([libgnome (night be part of libgnomeui) not found !]) ] )
#AM_PATH_GNOME_2_0(2.12.0,,AC_MSG_ERROR(mp3splt-gtk needs libgnome (might be part of libgnomeui >= 2.12.0))
PKG_CHECK_MODULES([GNOMEUI],
                  [libgnomeui-2.0], ,
                  [have_gnomeui=no])

#################################################
#Check for GTK
AM_PATH_GTK_2_0(2.12.0,,AC_MSG_ERROR(mp3splt-gtk needs GTK+ >= 2.12.0))

#################################################
#Check for libmp3splt

MP3SPLT_CHECK(,[ AC_MSG_ERROR(libmp3splt version 0.6.1 needed :

Download the latest version of libmp3splt at http://mp3splt.sourceforge.net/
) ])

#################################################
#Check for audacious

AM_CONDITIONAL(AUDACIOUS,false)
audacious="no"
#checking for --disable-audacious & audacious library
AC_ARG_ENABLE(audacious, AC_HELP_STRING([--disable-audacious], [Disable audacious player support. ]),
    [enable_audacious=$enableval],[enable_audacious="yes"])

if test x$enable_audacious = xyes;then
  PKG_CHECK_MODULES(AUDACIOUS, [audacious >= 0.1 audclient],
      [AM_CONDITIONAL(AUDACIOUS, true) audacious="yes" ],
      [AC_MSG_ERROR([Audacious player development files are not installed.

    Install it or configure with "--disable-audacious".])])
fi

#################################################
#Check for gstreamer

AM_CONDITIONAL(GSTREAMER,false)
gstreamer="no"
#checking for --disable-gstreamer & gstreamer library
AC_ARG_ENABLE(gstreamer, AC_HELP_STRING([--disable-gstreamer], [Disable gstreamer 'internal' player support. ]),
    [enable_gstreamer=$enableval],[enable_gstreamer="yes"])

if test x$enable_gstreamer = xyes;then
  PKG_CHECK_MODULES(GST, [gstreamer-0.10 >= 0.10
      gstreamer-base-0.10 >= 0.10
      gstreamer-plugins-base-0.10 >= 0.10],
      [AM_CONDITIONAL(GSTREAMER,true) gstreamer="yes"],
      [AC_MSG_ERROR([GStreamer >= 0.10 development files are not installed.

    Install it or configure with "--disable-gstreamer".])])
fi

#################################################
# Generate Makefiles
#################################################

AC_CONFIG_FILES(Makefile src/Makefile m4/Makefile po/Makefile.in doc/Makefile help/Makefile)

#################################################
# Define some useful constants
#################################################

# Gettext wants to know the path to the top directory and such things...
top_builddir=`pwd`
AC_SUBST(top_builddir)
AC_SUBST(PACKAGE)
AC_SUBST(LOCALEDIR)

#################################################
# Initialize the gnome help system
#################################################

# This one is thaken from the "Migrating to gnome Documentation Build
# utilities" page. No Idea, what it does.
AC_CONFIG_MACRO_DIR([m4])

# Prepare building the gnome documentation of this program
GNOME_DOC_INIT

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

