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
AC_INIT(libmp3splt, 0.4rc1, io_alex_2002@yahoo.fr)
AM_INIT_AUTOMAKE(libmp3splt, 0.4rc1)
AM_CONFIG_HEADER(config.h)

AM_PROG_LIBTOOL

# Checks for programs.
AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S

# Checks for typedefs, structures, and compiler characteristics.
AC_C_BIGENDIAN
AC_C_CONST

#some functions
AC_FUNC_FSEEKO

#checks for libmad
AC_CHECK_LIB(mad, mad_frame_decode,
[LIBS="$LIBS -lmad"]
mp3=yes,
   [AC_MSG_ERROR(libmad needed! Download the latest version at
   http://www.mars.org/home/rob/proj/mpeg/)]
   mp3=no)

AC_CHECK_FUNCS(fseeko, [ CFLAGS="$CFLAGS -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64" ], AC_MSG_WARN(fseeko() function is missing: FILE > 2 GB SUPPORT DISABLED!))

#checks for ogg
AC_ARG_ENABLE(ogg,
AC_HELP_STRING([--disable-ogg],
[Disable Ogg Vorbis support]), [
CFLAGS="$CFLAGS -DNO_OGG"
LIBS="$LIBS -lm"]
ogg=no, [
XIPH_PATH_OGG(,AC_MSG_ERROR(Ogg needed! Download the latest version of libogg at http://www.xiph.org))
XIPH_PATH_VORBIS(,AC_MSG_ERROR(Vorbis needed! Download the latest version of libvorbis at http://www.xiph.org))
LIBS="$LIBS -lvorbisfile -lvorbis -logg"
]
ogg=yes)

#checks for ogg symlink
AC_ARG_ENABLE(oggsplt_symlink,
AC_HELP_STRING([--enable-oggsplt],
[Enable symlink of oggsplt to mp3splt [[default=yes]] ]),
,enable_oggsplt_symlink=yes)
AM_CONDITIONAL(DO_SYMLINK, test x$enable_oggsplt_symlink = xyes)

#we check for mingw, we add libz
AC_MSG_CHECKING(for Mingw)
case $host in
	*-*-mingw*)	MING=yes
			LIBS="libz.a"
			;;
	*)		MING=no
			LIBS="$LIBS"
			;;
esac
AM_CONDITIONAL(MINGW, test x$MING = xyes)
AC_MSG_RESULT($MING)

#checks for id3tag
AC_CHECK_LIB(id3tag, id3_tag_new,
[ID3_LIBS="-lid3tag"]
id3=yes,
[ID3_CFLAGS="-DNO_ID3TAG"]
id3="no")

#check for --disable-id3tag
AC_ARG_ENABLE(id3tag,
AC_HELP_STRING([--disable-id3tag],
	[Disable id3tag support (for getting id3 from the orginial mp3
	file), Default: id3tag enabled if found installed and disabled
	if not]),
#action if given
id3disabled="yes"
[CFLAGS="$CFLAGS -DNO_ID3TAG"]
,
#action if not given
[LIBS="$LIBS $ID3_LIBS"]
[CFLAGS="$CFLAGS $ID3_CFLAGS"]
id3disabled="no"
)

#generate makefiles
AC_OUTPUT(Makefile src/Makefile)

#if we don't disable id3 and have id3, then we compiled with id3
if test $id3disabled = "no" && test $id3 = "yes"; then
   with_id3="yes"
else
   with_id3="no"
fi

#we print to screen the configuration
echo
echo "---------------------------------------------------------"
echo "Configuration:"
echo
echo "    install path:            ${prefix}"
echo
echo " Formats supported :"
echo
echo "    libmad (.mp3 format)                                    $mp3"
echo "    libogg, libvorbis (.ogg format)                         $ogg"
echo
echo " Other :"
echo "    libid3tag (for getting id3 from the orginial mp3 file)  $with_id3"
if test $id3 = "no" && test $id3disabled = "no"; then
echo "      install libid3tag from http://sourceforge.net/projects/mad/"
echo "      to have this feature supported"
fi
echo
echo "---------------------------------------------------------"
echo
