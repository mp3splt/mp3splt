#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(mp3splt-gtk, 0.4_rc1, io_alex_2002@yahoo.fr)

AC_CONFIG_SRCDIR([src/mp3splt-gtk.c])
AM_INIT_AUTOMAKE(mp3splt-gtk, 0.4_rc1)
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
#gettext
AC_CHECK_HEADERS([unistd.h])
AM_GNU_GETTEXT([external])
AM_GNU_GETTEXT_VERSION(0.13.1)

# Checks for libraries.
AM_PATH_GTK_2_0(2.6.0,,AC_MSG_ERROR(mp3splt-gtk needs GTK+ >= 2.6.0))

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
		LIBS="libvorbisfile.a libmad.a libvorbis.a libogg.a libid3tag.a libz.a libmp3splt.a -lwsock32 -mwindows"
		;;
	*)
		HOST="Other"
		;;
esac
AC_MSG_RESULT($HOST)

#checking for libmp3splt
AC_CHECK_LIB(mp3splt, mp3splt_v0_4_rc1,libmp3splt=yes,
        [AC_MSG_ERROR(libmp3splt version 0.4_rc1 needed :

You can download it at http://mp3splt.sourceforge.net/
)])

#checking for --enable-bmp string
AC_ARG_ENABLE(bmp,
AC_HELP_STRING([--enable-bmp],
	[Enable beep media player support(default is disabled)]),
#action if given
[AC_CHECK_LIB([beep], [xmms_remote_play],[],
	    [AC_MSG_ERROR(beep media player is not installed. 
	    
 Install it or configure without "--enable-bmp" :
)])
LIBS="$LIBS -lbeep"
bmp=yes
],
#action if not given
[CFLAGS="$CFLAGS -DNO_BMP"
bmp=no
]
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
echo "    beep-media-player        $bmp"
echo "    snackAmp                 yes (default)"
echo
echo "---------------------------------------------------------"
echo

