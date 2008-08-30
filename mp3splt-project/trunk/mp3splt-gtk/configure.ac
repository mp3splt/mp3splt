#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.56)
AC_INIT(mp3splt-gtk, 0.5, io_alex_2002@yahoo.fr)

AC_CONFIG_SRCDIR([src/mp3splt-gtk.c])
AM_INIT_AUTOMAKE(mp3splt-gtk, 0.5)
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
#gettext
AC_CHECK_HEADERS([unistd.h])
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION([0.13.1])

# Checks for libraries.
AM_PATH_GTK_2_0(2.6.0,,AC_MSG_ERROR(mp3splt-gtk needs GTK+ >= 2.6.0))

CFLAGS="$CFLAGS -I../libmp3splt/include/ -I../../libmp3splt/include/ \
-I../libmp3splt/include/libmp3splt/ -I../../libmp3splt/include/libmp3splt/"
LDFLAGS="$LDFLAGS -L../libmp3splt/src -L../libmp3splt/src/.libs \
-L../../libmp3splt/src/.libs/ -L../../libmp3splt/src/"

#we check for mingw
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
AC_CHECK_LIB(mp3splt, mp3splt_new_state,libmp3splt=yes,
        [AC_MSG_ERROR(libmp3splt version 0.5 needed :

You can download it at http://mp3splt.sourceforge.net/
)])
fi

#checking for --enable-audacious string
AC_ARG_ENABLE(audacious,
AC_HELP_STRING([--enable-audacious], [Enable Audacious player support(default is disabled)]),
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

#generate makefiles
AC_OUTPUT(Makefile src/Makefile m4/Makefile po/Makefile.in)

echo
echo "---------------------------------------------------------"
echo "Configuration:"
echo
echo "    install path:            ${prefix}"
echo
echo " Players supported :"
echo
echo "    audacious                $audacious"
echo "    snackAmp                 yes (default)"
echo
echo "---------------------------------------------------------"
echo

