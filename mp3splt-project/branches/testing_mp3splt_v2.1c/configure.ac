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

# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.53)
AC_INIT(mp3splt, 2.1c, mtrotta@users.sourceforge.net)
AM_INIT_AUTOMAKE($PACKAGE_NAME, $PACKAGE_VERSION)
AM_CONFIG_HEADER(config.h)

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S

# Checks for typedefs, structures, and compiler characteristics.
AC_C_BIGENDIAN
AC_C_CONST

# Checks for library functions.
AC_FUNC_MALLOC

AC_CHECK_LIB(mad, mad_frame_decode,,
                        [AC_MSG_ERROR(libmad needed! Download the latest version at http://www.mars.org/home/rob/proj/mpeg/)])

AC_CHECK_FUNCS(fseeko, [ CFLAGS="$CFLAGS -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64" ], AC_MSG_WARN(fseeko() function is missing: FILE > 2 GB SUPPORT DISABLED!))

AC_ARG_ENABLE(ogg,
AC_HELP_STRING([--disable-ogg],
[Disable Ogg Vorbis support]), [
CFLAGS="$CFLAGS -DNO_OGG"
LIBS="$LIBS -lm"], [
XIPH_PATH_OGG(,AC_MSG_ERROR(Ogg needed! Download the latest version of libogg at http://www.xiph.org))
XIPH_PATH_VORBIS(,AC_MSG_ERROR(Vorbis needed! Download the latest version of libvorbis at http://www.xiph.org))
LIBS="$LIBS -lvorbisfile"
])

AC_ARG_ENABLE(oggsplt_symlink,
AC_HELP_STRING([--enable-oggsplt],
[Enable symlink of oggsplt to mp3splt [[default=yes]] ]),
,enable_oggsplt_symlink=yes)
AM_CONDITIONAL(DO_SYMLINK, test x$enable_oggsplt_symlink = xyes)

AC_OUTPUT(Makefile doc/Makefile)
