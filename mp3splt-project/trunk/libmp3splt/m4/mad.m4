# Alexandru Munteanu <m@ioalex.net>
# Slightly modified 'ogg.m4'

AC_DEFUN([MAD_CHECK],
[

AC_ARG_WITH(mad, [AC_HELP_STRING([--with-mad=PFX],
    [ Prefix where libmad is installed (optional). ]) ],
  mad_prefix="$withval", mad_prefix="")
AC_ARG_WITH(mad-libraries, [AC_HELP_STRING([--with-mad-libraries=DIR],
    [ Directory where libmad library is installed (optional). ]) ],
  mad_libraries="$withval", mad_libraries="")
AC_ARG_WITH(mad-includes, [AC_HELP_STRING([--with-mad-includes=DIR],
    [ Directory where libmad header files are installed (optional). ]) ],
  mad_includes="$withval", mad_includes="")
AC_ARG_ENABLE(madtest, [AC_HELP_STRING([--disable-madtest],
    [ Do not try to compile and run a test mad program. ]) ],
  , enable_madtest=yes)

if test "x$mad_libraries" != "x"; then
  MAD_LIBS="-L$mad_libraries"
elif test "x$mad_prefix" != "x"; then
  MAD_LIBS="-L$mad_prefix/lib"
elif test "x$prefix" != "xNONE"; then
  MAD_LIBS="-L$prefix/lib"
fi

MAD_LIBS="$MAD_LIBS -lmad"

if test "x$mad_includes" != "x"; then
  MAD_CFLAGS="-I$mad_includes"
elif test "x$mad_prefix" != "x"; then
  MAD_CFLAGS="-I$mad_prefix/include"
elif test "x$prefix" != "xNONE"; then
  MAD_CFLAGS="-I$prefix/include"
fi


AC_MSG_CHECKING(for Mad)
no_mad=""

if test "x$enable_madtest" = "xyes" ; then
  ac_save_CFLAGS="$CFLAGS"
  ac_save_LIBS="$LIBS"
  CFLAGS="$CFLAGS $MAD_CFLAGS"
  LIBS="$LIBS $MAD_LIBS"

  rm -f conf.madtest

      AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mad.h>

int main ()
{
  system("touch conf.madtest");
  return 0;
}

],, no_mad=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])

       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
fi


if test "x$no_mad" = "x" ; then
   AC_MSG_RESULT(yes)
   ifelse([$1], , :, [$1])     
else
   AC_MSG_RESULT(no)
   if test -f conf.madtest ; then
     :
   else
     echo "*** Could not run Mad test program, checking why..."
     CFLAGS="$CFLAGS $MAD_CFLAGS"
     LIBS="$LIBS $MAD_LIBS"
     AC_TRY_LINK([
#include <stdio.h>
#include <mad.h>
],     [ return 0; ],
       [ echo "*** The test program compiled, but did not run. This usually means"
       echo "*** that the run-time linker is not finding libmad or finding the wrong"
       echo "*** version of libmad. If it is not finding libmad, you'll need to set your"
       echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
       echo "*** to the installed location  Also, make sure you have run ldconfig if that"
       echo "*** is required on your system"
       echo "***"
       echo "*** If you have an old version installed, it is best to remove it, although"
       echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
       [ echo "*** The test program failed to compile or link. See the file config.log for the"
       echo "*** exact error that occured. This usually means libmad was incorrectly installed"
       echo "*** or that you have moved libmad since it was installed." ])
     CFLAGS="$ac_save_CFLAGS"
     LIBS="$ac_save_LIBS"
   fi
   MAD_CFLAGS=""
   MAD_LIBS=""
   ifelse([$2], , :, [$2])
fi

AC_SUBST(MAD_CFLAGS)
AC_SUBST(MAD_LIBS)
rm -f conf.madtest
])

