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
AC_INIT(mp3splt-gtk, 0.6.1, mp3splt.sf.net)

# tell autoconf where to put the temporary files and scripts
# it generates. Seems to be more clean to put them into a subdir;
# One of this files is gnome-doc-utils.make, which is needed to 
# compile the help files.
AC_CONFIG_AUX_DIR([build-aux])

AC_CONFIG_MACRO_DIR([m4])

# tell which file by being there indicates we are run from the
# right directory
AC_CONFIG_SRCDIR([src/mp3splt-gtk.c])

# Initialize automake
AM_INIT_AUTOMAKE(mp3splt-gtk, 0.6.1)

# Tell autoconf where to put the output file that contains all
# definitions our C program will need
AC_CONFIG_HEADER([config.h])
AM_MAINTAINER_MODE([enable])

DEBUG_FLAGS="-g"
#OPTIMISE_FLAGS="-O2"
#WARNING_FLAGS="-Wall -Wextra -Wswitch-enum -Wswitch-default -Wfloat-equal -Wbad-function-cast -Wcast-qual -Wunreachable-code"
WARNING_FLAGS="-Wall"
CFLAGS="$DEBUG_FLAGS $WARNING_FLAGS $OPTIMISE_FLAGS $CFLAGS -I../libmp3splt/include/ -I../../libmp3splt/include/"
LDFLAGS="$LDFLAGS -L../libmp3splt/src/.libs -L../../libmp3splt/src/.libs/"

#################################################
# Check for programs
#################################################

AC_PROG_CC

# Include all m4 macros that have to do with the language C
AC_LANG_C

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

#################################################
# Initialize the gnome integration
#################################################

AM_CONDITIONAL(GNOME,false)
gnome="no"
#checking for --disable-gnome
AC_ARG_ENABLE(gnome, AC_HELP_STRING([--disable-gnome], [Disable gnome support. ]),
    [enable_gnome=$enableval],[enable_gnome="yes"])

if test x$enable_gnome = xyes;then
  #Prepare building the gnome documentation of this program
  GNOME_DOC_INIT
  #Check for libgnome, the library for displaying the "help" dialogue
  PKG_CHECK_MODULES([GNOMEUI],
                    [libgnomeui-2.0], 
                    [AM_CONDITIONAL(GNOME, true) gnome="yes" ],
      [AC_MSG_ERROR([gnomeui development files are not installed.

    Install it or configure with "--disable-gnome".])])
fi

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
# Generate Makefiles
#################################################

AC_OUTPUT(Makefile src/Makefile m4/Makefile po/Makefile.in doc/Makefile help/Makefile)

#################################################
# Define some useful constants
#################################################

# Gettext wants to know the path to the top directory and such things...
top_builddir=`pwd`
AC_SUBST(top_builddir)
AC_SUBST(PACKAGE)
AC_SUBST(LOCALEDIR)


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
echo " Other :"
echo
echo "    gnome support            $gnome"
echo
echo "---------------------------------------------------------"
echo

