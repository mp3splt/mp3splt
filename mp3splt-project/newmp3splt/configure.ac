#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(mp3splt, 2.3rc1, mtrotta@users.sourceforge.net)

AC_CONFIG_SRCDIR([src/mp3splt.c])
AM_INIT_AUTOMAKE(mp3splt, 2.3rc1)
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

#we check for mingw
AC_MSG_CHECKING(for Mingw)
case $host in
	*-*-mingw*)	MING=yes
			LIBS="libvorbisfile.a libmad.a libvorbis.a libogg.a libid3tag.a libz.a libmp3splt.a -lwsock32"
			;;
	*)		MING=no
			LIBS="$LIBS"
			;;
esac
AM_CONDITIONAL(MINGW, test x$MING = xyes)
AC_MSG_RESULT($MING)

#checking for libmp3splt
AC_CHECK_LIB(mp3splt, mp3splt_split,libmp3splt=yes,
        [AC_MSG_ERROR(libmp3splt needed :

Download the latest version of libmp3splt at http://mp3splt.sourceforge.net/
)])

AC_ARG_ENABLE(oggsplt_symlink,
AC_HELP_STRING([--enable-oggsplt],
[Enable symlink of oggsplt to mp3splt [[default=yes]] ]),
,enable_oggsplt_symlink=yes)
AM_CONDITIONAL(DO_SYMLINK, test x$enable_oggsplt_symlink = xyes)

#generate makefiles
AC_OUTPUT(Makefile src/Makefile doc/Makefile)

echo
echo "---------------------------------------------------------"
echo "Configuration:"
echo
echo "    install path:            ${prefix}"
echo
echo "---------------------------------------------------------"
echo

