#/**********************************************************
# *
# * mp3splt-gtk - utility using libmp3splt,
# *                for mp3/ogg splitting without decoding
# *
# * Copyright (c) 2002-2005 M. Trotta - <mtrotta@users.sourceforge.net>
# * Copyright (c) 2005-2011 Munteanu Alexandru - io_fx@yahoo.fr
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

AC_PREREQ(2.62)
AC_INIT(mp3splt-gtk, 0.7.1, mp3splt.sf.net)

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
AM_INIT_AUTOMAKE(mp3splt-gtk, 0.7.1)

# Tell autoconf where to put the output file that contains all
# definitions our C program will need
AC_CONFIG_HEADER([config.h])
AM_MAINTAINER_MODE([enable])

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

AM_CONDITIONAL(GNOME, false)
AM_CONDITIONAL(HAVE_GNOME_DOC_UTILS, false)
gnome="no"

AC_ARG_ENABLE(gnome, AC_HELP_STRING([--disable-gnome], [Disable gnome support. ]),
    [enable_gnome=$enableval],[enable_gnome="yes"])

AM_CONDITIONAL(ENABLE_SK, true)

if test "x$enable_gnome" = xyes;then
  ifdef([GNOME_DOC_INIT],
  [
    GNOME_DOC_INIT([],
                   [
                     AM_CONDITIONAL(GNOME, true)
                     AM_CONDITIONAL(HAVE_GNOME_DOC_UTILS, true)
                     gnome="yes"
                   ],
                   [AC_MSG_ERROR([gnome-doc-utils not found.
  
      Install it or configure with "--disable-gnome".])])
  ],
  [])
fi

#################################################
#Check for GTK & audacious

AC_ARG_ENABLE(gtk3, [
 AC_HELP_STRING([--enable-gtk3],  [Compile against GTK 3.x explicitly (default: use GTK 3.x if available)])
 AC_HELP_STRING([--disable-gtk3], [Compile against GTK 2.x explicitly (default: use GTK 3.x if available)])],
     [enable_gtk3=$enableval],[enable_gtk3="auto"])

if test "x$enable_gtk3" = xauto; then
  AC_MSG_NOTICE([selecting GTK 2.x/3.x automatically])
  ifdef([AM_PATH_GTK_3_0],[enable_gtk3=yes],[enable_gtk3=no])
fi

audacious_check="audclient < 3.0"
if test "x$enable_gtk3" = xyes; then
  AC_MSG_NOTICE([GTK 3.x selected])

  ifdef([AM_PATH_GTK_3_0],
  [
    CFLAGS="$CFLAGS -DGTK_DISABLE_SINGLE_INCLUDES -DGDK_DISABLE_DEPRECATED -DGTK_DISABLE_DEPRECATED -DGSEAL_ENABLE"
    AM_PATH_GTK_3_0(3.0.0,,AC_MSG_ERROR(mp3splt-gtk needs GTK+ >= 3.0.0))
    audacious_check="audclient >= 3.0"
  ],
  [
    AC_MSG_ERROR([Cannot find GTK 3.x])
  ])
else
  AC_MSG_NOTICE([GTK 2.x selected])
  AM_PATH_GTK_2_0(2.20.0,,AC_MSG_ERROR(mp3splt-gtk needs GTK+ >= 2.20.0))
fi

#################################################
#Check for libmp3splt

MP3SPLT_CHECK(,[ AC_MSG_ERROR(libmp3splt version 0.7.1 needed :

Download the latest version of libmp3splt at http://mp3splt.sourceforge.net/
) ])

#################################################
#Check for audacious

AM_CONDITIONAL(AUDACIOUS,false)
audacious="no"
#checking for --disable-audacious & audacious library
AC_ARG_ENABLE(audacious, AC_HELP_STRING([--disable-audacious], [Disable audacious player support. ]),
    [enable_audacious=$enableval],[enable_audacious="yes"])

if test "x$enable_audacious" = xyes;then
  PKG_CHECK_MODULES([AUDACIOUS], [$audacious_check ],
      [AM_CONDITIONAL(AUDACIOUS, true) audacious="yes" ],
      [AC_MSG_ERROR([Audacious player development files for $audacious_check not found.

    Install it or configure with "--disable-audacious".
    Also check that if you want to compile with GTK 3.x, you will need audacious >= 3.0.
    For GTK 2.x, you will need audacious < 3.0.
    You can force the GTK version using --enable-gtk3 and --disable-gtk3.
]) ])
fi

#################################################
#Check for gstreamer

AM_CONDITIONAL(GSTREAMER,false)
gstreamer="no"
#checking for --disable-gstreamer & gstreamer library
AC_ARG_ENABLE(gstreamer, AC_HELP_STRING([--disable-gstreamer], [Disable gstreamer 'internal' player support. ]),
    [enable_gstreamer=$enableval],[enable_gstreamer="yes"])

if test "x$enable_gstreamer" = xyes;then
  PKG_CHECK_MODULES(GST, [gstreamer-0.10 >= 0.10
      gstreamer-base-0.10 >= 0.10
      gstreamer-plugins-base-0.10 >= 0.10],
      [AM_CONDITIONAL(GSTREAMER,true) gstreamer="yes"],
      [AC_MSG_ERROR([GStreamer >= 0.10 development files are not installed.

    Install it or configure with "--disable-gstreamer".])])
fi

#################################################
# Documentation of the code
# (but only if doxygen is installed)
#################################################

doxygen_doc="no" 
with_graphviz="no"
AC_ARG_ENABLE(doxygen_doc, [AC_HELP_STRING([--disable-doxygen_doc],[ Disable doxygen code documentation. ]) ], 
    [enable_doxygen_doc=$enableval],[enable_doxygen_doc="yes"])

AM_CONDITIONAL(HAVE_DOXYGEN, false)
AM_CONDITIONAL(HAVE_GRAPHVIZ, false)
if test "x$enable_doxygen_doc" = xyes;then
  AC_PATH_PROG([DOXYGEN],[doxygen])
  AC_ARG_VAR(DOXYGEN,
             [Location of the 'doxygen' program; if found we can generate documentation for the C code])
  
  if test "x$DOXYGEN" != "x"; then
   AM_CONDITIONAL(HAVE_DOXYGEN, true)
   doxygen_doc="yes"

   AC_PATH_PROG([GRAPHVIZ],[dot])
   AC_ARG_VAR(GRAPHVIZ,
              [Location of the 'dot' program from the graphviz suite; if found we can generate caller graphs for the documentation of the C code])
   if test "x$GRAPHVIZ" = "x"; then
     AC_MSG_WARN([*** dot from the graphviz package not found, inheritance diagrams will not be generated in code documentation])
   else
     AM_CONDITIONAL(HAVE_GRAPHVIZ, true)
     with_graphviz="yes"
   fi

  fi
fi

#################################################
# 
#################################################

AM_CONDITIONAL(NO_IMAGES_DIR, false)
AC_ARG_WITH([no_images_dir],
    [AS_HELP_STRING([--with-no_images_dir],
      [take images from the directory where the application is run])],
    [AM_CONDITIONAL(NO_IMAGES_DIR, true)],
    [])

#################################################################
# Check for some debugging, warnings and optimise options
#################################################################

AC_ARG_ENABLE(c-debug, [AC_HELP_STRING([--enable-c-debug],[ Enable debugging symbols. ]) ],
    [enable_c_debug=$enableval],[enable_c_debug="no"])
if test "x$enable_c_debug" = xyes;then
  CFLAGS="$CFLAGS -g"
fi

AC_ARG_ENABLE(optimise, [AC_HELP_STRING([--enable-optimise],[ Enable O2 optimise. ]) ],
    [enable_optimise=$enableval],[enable_optimise="no"])
if test "x$enable_optimise" = xyes;then
  CFLAGS="$CFLAGS -O2"
fi

AC_ARG_ENABLE(extra-warnings, [AC_HELP_STRING([--enable-extra-warnings],[ Enable extra warnings. ]) ],
    [enable_extra_warnings=$enableval],[enable_extra_warnings="no"])
if test "x$enable_extra_warnings" = xyes;then
  CFLAGS="$CFLAGS -Wall -Wextra -Wswitch-enum -Wswitch-default -Wfloat-equal -Wbad-function-cast -Wcast-qual -Wunreachable-code"
fi

#################################################
# Generate Makefiles
#################################################

AC_CONFIG_FILES([Makefile src/Makefile m4/Makefile po/Makefile.in doc/Makefile help/Makefile])

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
echo "    doxygen documentation    $doxygen_doc"
echo "        with graphviz        $with_graphviz"
echo "---------------------------------------------------------"
echo

# Tell autoconf to start outputting whatever it has to.
AC_OUTPUT
