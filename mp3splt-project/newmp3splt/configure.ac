#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(mp3splt, 2.2_rc1, mtrotta@users.sourceforge.net)

AC_CONFIG_SRCDIR([src/mp3splt.c])
AM_INIT_AUTOMAKE(mp3splt, 2.2_rc1)
AM_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC

# Checks for header files.

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.

#we check for mingw
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
		LIBS="libvorbisfile.a libmad.a libvorbis.a libogg.a libid3tag.a libz.a libmp3splt.a -lwsock32"
		;;
	*solaris*)
		HOST="Solaris"
		LIBS="$LIBS -lsocket"
		;;
	*)
		HOST="Other"
                LIBS=""
#		LIBS="-lmad -logg -lvorbis -lvorbisfile -lm"
		;;
esac
AC_MSG_RESULT($HOST)

#by default we do the libmp3splt test
LIBMP3SPLT_TEST=1
#checking for --disable-libmp3splt_test string
AC_ARG_ENABLE(libmp3splt_test,
AC_HELP_STRING([--disable-libmp3splt_test],
	[Disables the libmp3splt test when doing configure)]),
#action if given
LIBMP3SPLT_TEST=0
,
#action if not given
LIBMP3SPLT_TEST=1
)

#only we haven't disabled the libmp3splt test
#we check for libmp3splt
if test $LIBMP3SPLT_TEST -eq 1;then
#checking for libmp3splt
AC_CHECK_LIB(mp3splt, mp3splt_v0_4_rc1,libmp3splt=yes,
        [AC_MSG_ERROR(libmp3splt version 0.4_rc1 needed :

Download the latest version of libmp3splt at http://mp3splt.sourceforge.net/
)])
fi

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

