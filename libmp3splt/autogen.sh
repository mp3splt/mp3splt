#!/bin/sh
win=$1

#autopoint check
(autopoint --version) > /dev/null 2>&1 ||
{
    echo
    echo "Error: you must have autopoint installed to compile libmp3splt !"
    echo
    exit 1
}

#autoconf check
(autoconf --version && autoheader --version) > /dev/null 2>&1 ||
{
    echo
    echo "Error: you must have autoconf installed to compile libmp3splt !"
    echo
    exit 1
}

#automake check
(aclocal --version) > /dev/null 2>&1 ||
{
    echo
    echo "Error: you must have automake installed to compile libmp3splt !"
    echo
    exit 1
}

#msgfmt check
HAS_MSGFMT=yes
(msgfmt --version) > /dev/null 2>&1 ||
{
  HAS_MSGFMT=no
}

#remove old libtool generated files
rm -f m4/{libtool,argz,ltdl,ltoptions,lt~obsolete,ltversion,ltsugar}.m4
rm -f libtool aclocal.m4 config.status configure autom4te.cache/* ltmain.sh

if ! test -z $win ;then
 WIN_ACLOCAL_FLAGS="-I /usr/share/aclocal"
fi

#we run aclocal, autoconf and automake
#info: the 'sed' is an ugly hack for libtool version compatibility because aclocal.m4 is shipped in 'make dist'
echo -n "1/6 Running autopoint... " \
&& autopoint -f && echo "done" \
&& echo -n "2/6 Running aclocal... " \
&& aclocal -I m4 $WIN_ACLOCAL_FLAGS $ACLOCAL_FLAGS && echo "done" \
&& if test "x$win" = x;then sed "s/lt_dlcaller_register/lt_dlcaller_set_data/" aclocal.m4 > libmp3splt_aclocal.m4 && mv libmp3splt_aclocal.m4 aclocal.m4;fi \
&& echo -n "3/6 Running autoheader... " \
&& autoheader && echo "done" \
&& {
 `which glibtoolize 2>&1 >/dev/null`
 if [ $? -eq 0 ]; then
   echo -n "4/6 Running glibtoolize... " 
   glibtoolize -c --force
 else
   echo -n "4/6 Running libtoolize... " 
   libtoolize -c --force
 fi
} && echo "done" \
&& echo -n "5/6 Running autoconf... " \
&& autoconf && echo "done" \
&& echo -n "6/6 Running automake... " \
&& automake -a -c && echo "done"

if test "x$HAS_MSGFMT" = xyes;then

echo -n "Formatting language files with msgfmt... " && \
{
    cd po
    for f in *.po; do
        if test -r "$f"; then
            lang=`echo $f | sed -e 's,\.po$,,'`
            msgfmt -c -o $lang.gmo $lang.po
        fi
    done
    cd ..
} && echo "done"

fi

