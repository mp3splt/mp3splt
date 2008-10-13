#/**********************************************************
# *
# * libmp3splt -- library based on mp3splt,
# *               for mp3/ogg splitting without decoding
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
AC_INIT(libmp3splt, 0.5.2, io_fx@yahoo.fr)
AM_INIT_AUTOMAKE(libmp3splt, 0.5.2)
AM_CONFIG_HEADER(config.h)

CFLAGS="-g $CFLAGS -I../include/libmp3splt"

#################################################
# Check for programs
#################################################

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S

#################################################################
# Checks for typedefs, structures, and compiler characteristics
#################################################################

AC_C_BIGENDIAN
AC_C_CONST

#################################################################
# Enable libtool and check for libltdl and define @LIBLTDL@ and @INCLTDL@
#################################################################

AC_WITH_LTDL
AC_LIBTOOL_WIN32_DLL
AC_PROG_LIBTOOL

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
    AM_CONDITIONAL(WIN32,true)
		LIBS="-lz -lws2_32"
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
# Check mp3 plugin libraries
#################################################################

#check for --disable-mp3
AC_ARG_ENABLE(mp3,
              [ AC_HELP_STRING([--disable-mp3], [ Disable mp3 plugin. ]) ],
              [ AM_CONDITIONAL(MP3_PLUGIN,false) mp3="no" ],
              #check for -lmad
              [ AC_CHECK_LIB(mad, mad_frame_decode, [ mp3="yes"
					              AM_CONDITIONAL(MP3_PLUGIN,true) ],
                             [ AC_MSG_WARN([libmad not found ! Will build without mp3 support.]); 
			       AM_CONDITIONAL(MP3_PLUGIN,false) mp3="no" ])
               ])

#check for --disable-id3tag
AC_ARG_ENABLE(id3tag,
              [ AC_HELP_STRING([--disable-id3tag], [ Disable id3tag support for the mp3 plugin. ]) ],
              [ AM_CONDITIONAL(LIB_ID3TAG,false) id3="no" with_id3="no" CFLAGS="$CFLAGS -DNO_ID3TAG" ],
              #check for -lid3tag
              [ AC_CHECK_LIB(id3tag,
                             [id3_tag_new],
                             [ id3="yes" ],
                             [ ID3_CFLAGS="-DNO_ID3TAG"; id3="no"
			       AM_CONDITIONAL(LIB_ID3TAG,true) ])
              AM_CONDITIONAL(LIB_ID3TAG,true) CFLAGS="$CFLAGS $ID3_CFLAGS" with_id3="yes" ])

#################################################################
# Check ogg plugin libraries
#################################################################

#check for --disable-ogg
AC_ARG_ENABLE(ogg,
              [ AC_HELP_STRING([--disable-ogg], [ Disable Ogg Vorbis plugin. ]) ],
              [ AM_CONDITIONAL(OGG_PLUGIN,false) ogg="no" ],
              #check for -logg -lvorbis -lvorbisfile
              [ XIPH_PATH_OGG(,AC_MSG_ERROR(Ogg needed! Download the latest version of libogg at http://www.xiph.org))
              XIPH_PATH_VORBIS(,AC_MSG_ERROR(Vorbis needed! Download the latest version of libvorbis at http://www.xiph.org))
              AM_CONDITIONAL(OGG_PLUGIN,true) ogg="yes" ])

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
echo "    libogg, libvorbis (.ogg format)                         $ogg"
echo
if test $mp3 = "yes"; then
  echo " Other :"
  echo "    libid3tag (for getting id3 from the orginial mp3 file)  $id3"
  if test $id3 = "no" && test $with_id3 = "yes"; then
    echo "      install libid3tag from http://sourceforge.net/projects/mad/"
    echo "      to have this feature supported"
  fi
fi
echo
echo "---------------------------------------------------------"
echo

#################################################
# Generate Makefile
#################################################

AC_OUTPUT(Makefile src/Makefile plugins/Makefile)

