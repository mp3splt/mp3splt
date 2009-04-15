# Alexandru Munteanu <io_fx@yahoo.fr>
# Slightly modified 'ogg.m4'

AC_DEFUN([ID3_CHECK],
[

AC_ARG_WITH(id3, [AC_HELP_STRING([--with-id3=PFX],
    [ Prefix where id3tag is installed (optional). ]) ],
  id3_prefix="$withval", id3_prefix="")
AC_ARG_WITH(id3-libraries, [AC_HELP_STRING([--with-id3-libraries=DIR],
    [ Directory where id3tag library is installed (optional). ]) ],
  id3_libraries="$withval", id3_libraries="")
AC_ARG_WITH(id3-includes, [AC_HELP_STRING([--with-id3-includes=DIR],
    [ Directory where id3tag header files are installed (optional). ]) ],
  id3_includes="$withval", id3_includes="")
AC_ARG_ENABLE(id3test, [AC_HELP_STRING([--disable-id3test],
    [ Do not try to compile and run a test id3 program. ]) ],
  , enable_id3test=yes)

if test "x$id3_libraries" != "x"; then
  ID3_LIBS="-L$id3_libraries"
elif test "x$id3_prefix" != "x"; then
  ID3_LIBS="-L$id3_prefix/lib"
elif test "x$prefix" != "xNONE"; then
  ID3_LIBS="-L$prefix/lib"
fi

ID3_LIBS="$ID3_LIBS -lid3tag"

if test "x$id3_includes" != "x"; then
  ID3_CFLAGS="-I$id3_includes"
elif test "x$id3_prefix" != "x"; then
  ID3_CFLAGS="-I$id3_prefix/include"
elif test "x$prefix" != "xNONE"; then
  ID3_CFLAGS="-I$prefix/include"
fi


AC_MSG_CHECKING(for id3tag)
no_id3=""

if test "x$enable_id3test" = "xyes" ; then
  ac_save_CFLAGS="$CFLAGS"
  ac_save_LIBS="$LIBS"
  CFLAGS="$CFLAGS $ID3_CFLAGS"
  LIBS="$LIBS $ID3_LIBS"

  rm -f conf.id3test

      AC_TRY_RUN([
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <id3tag.h>

int main ()
{
  system("touch conf.id3test");
  return 0;
}

],, no_id3=yes,[echo $ac_n "cross compiling; assumed OK... $ac_c"])

       CFLAGS="$ac_save_CFLAGS"
       LIBS="$ac_save_LIBS"
fi


if test "x$no_id3" = "x" ; then
   AC_MSG_RESULT(yes)
   ifelse([$1], , :, [$1])     
else
   AC_MSG_RESULT(no)
   if test -f conf.id3test ; then
     :
   else
     echo "*** Could not run id3tag test program, checking why..."
     CFLAGS="$CFLAGS $ID3_CFLAGS"
     LIBS="$LIBS $ID3_LIBS"
     AC_TRY_LINK([
#include <stdio.h>
#include <id3tag.h>
],     [ return 0; ],
       [ echo "*** The test program compiled, but did not run. This usually means"
       echo "*** that the run-time linker is not finding id3tag or finding the wrong"
       echo "*** version of id3tag. If it is not finding id3tag, you'll need to set your"
       echo "*** LD_LIBRARY_PATH environment variable, or edit /etc/ld.so.conf to point"
       echo "*** to the installed location  Also, make sure you have run ldconfig if that"
       echo "*** is required on your system"
       echo "***"
       echo "*** If you have an old version installed, it is best to remove it, although"
       echo "*** you may also be able to get things to work by modifying LD_LIBRARY_PATH"],
       [ echo "*** The test program failed to compile or link. See the file config.log for the"
       echo "*** exact error that occured. This usually means id3tag was incorrectly installed"
       echo "*** or that you have moved id3tag since it was installed." ])
     CFLAGS="$ac_save_CFLAGS"
     LIBS="$ac_save_LIBS"
   fi
   ID3_CFLAGS=""
   ID3_LIBS=""
   ifelse([$2], , :, [$2])
fi

AC_SUBST(ID3_CFLAGS)
AC_SUBST(ID3_LIBS)
rm -f conf.id3test
])

