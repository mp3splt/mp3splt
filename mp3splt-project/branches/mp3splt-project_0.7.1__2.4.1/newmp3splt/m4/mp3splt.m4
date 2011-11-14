# Alexandru Munteanu <io_fx@yahoo.fr>
# Slightly modified 'ogg.m4'

AC_DEFUN([MP3SPLT_CHECK],
[

AC_ARG_WITH(mp3splt, [AC_HELP_STRING([--with-mp3splt=PFX],
    [ Prefix where libmp3splt is installed (optional). ]) ],
  mp3splt_prefix="$withval", mp3splt_prefix="")
AC_ARG_WITH(mp3splt-libraries, [AC_HELP_STRING([--with-mp3splt-libraries=DIR],
    [ Directory where libmp3splt library is installed (optional). ]) ],
  mp3splt_libraries="$withval", mp3splt_libraries="")
AC_ARG_WITH(mp3splt-includes, [AC_HELP_STRING([--with-mp3splt-includes=DIR],
    [ Directory where libmp3splt header files are installed (optional). ]) ],
  mp3splt_includes="$withval", mp3splt_includes="")
AC_ARG_ENABLE(mp3splttest, [AC_HELP_STRING([--disable-mp3splttest],
    [ Do not try to compile and run a test mp3splt program. ]) ],
  , enable_mp3splttest=yes)

if test "x$mp3splt_libraries" != "x"; then
  MP3SPLT_LIBS="-L$mp3splt_libraries"
elif test "x$mp3splt_prefix" != "x"; then
  MP3SPLT_LIBS="-L$mp3splt_prefix/lib"
elif test "x$prefix" != "xNONE"; then
  MP3SPLT_LIBS="-L$prefix/lib"
fi

MP3SPLT_LIBS="$MP3SPLT_LIBS -lmp3splt"

if test "x$mp3splt_includes" != "x"; then
  MP3SPLT_CFLAGS="-I$mp3splt_includes"
elif test "x$mp3splt_prefix" != "x"; then
  MP3SPLT_CFLAGS="-I$mp3splt_prefix/include"
elif test "x$prefix" != "xNONE"; then
  mp3splt_CFLAGS="-I$prefix/include"
fi


AC_MSG_CHECKING(for libmp3splt)
no_mp3splt=""

if test "x$enable_mp3splttest" = "xyes" ; then
  ac_save_CFLAGS="$CFLAGS"
  ac_save_LIBS="$LIBS"
  CFLAGS="$CFLAGS $MP3SPLT_CFLAGS"
  LIBS="$LIBS $MP3SPLT_LIBS"

  rm -f conf.mp3splttest

      AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libmp3splt/mp3splt.h>

int main ()
{
  system("touch conf.mp3splttest");
  return 0;
}

],, no_mp3splt=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])

       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
fi


if test "x$no_mp3splt" = "x" ; then
   AC_MSG_RESULT(yes)
   ifelse([$1], , :, [$1])     
else
   AC_MSG_RESULT(no)
   if test -f conf.mp3splttest ; then
     :
   else
     echo "*** Could not run libmp3splt test program, checking why..."
     CFLAGS="$CFLAGS $MP3SPLT_CFLAGS"
     LIBS="$LIBS $MP3SPLT_LIBS"
     AC_TRY_LINK([
#include <stdio.h>
#include <libmp3splt/mp3splt.h>
],     [ return 0; ],
       [ echo "*** The test program compiled, but did not run. This usually means"
       echo "*** that the run-time linker is not finding libmp3splt or finding the wrong"
       echo "*** version of libmp3splt. If it is not finding libmp3splt, you'll need to set your"
       echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
       echo "*** to the installed location  Also, make sure you have run ldconfig if that"
       echo "*** is required on your system"
       echo "***"
       echo "*** If you have an old version installed, it is best to remove it, although"
       echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
       [ echo "*** The test program failed to compile or link. See the file config.log for the"
       echo "*** exact error that occured. This usually means libmp3splt was incorrectly installed"
       echo "*** or that you have moved libmp3splt since it was installed." ])
     CFLAGS="$ac_save_CFLAGS"
     LIBS="$ac_save_LIBS"
   fi
   MP3SPLT_CFLAGS=""
   MP3SPLT_LIBS=""
   ifelse([$2], , :, [$2])
fi

AC_SUBST(MP3SPLT_CFLAGS)
AC_SUBST(MP3SPLT_LIBS)
rm -f conf.mp3splttest
])

