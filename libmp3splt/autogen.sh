#!/bin/sh
win=$1

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

#remove old libtool generated files
rm -f m4/{libtool,argz,ltdl,ltoptions,lt~obsolete,ltversion,ltsugar}.m4
rm -f libtool aclocal.m4 config.status configure autom4te.cache/* ltmain.sh

#we run aclocal, autoconf and automake
#info: the 'sed' is an ugly hack for libtool version compatibility because aclocal.m4 is shipped in 'make dist'
echo -n "1/6 Running autopoint... " \
&& autopoint -f && echo "done" \
&& echo -n "2/6 Running aclocal... " \
&& aclocal -Im4 $ACLOCAL_FLAGS && echo "done" \
&& if [[ -z $win ]];then sed -i "s/lt_dlcaller_register/lt_dlcaller_set_data/" aclocal.m4;fi \
&& echo -n "3/6 Running autoheader... " \
&& autoheader && echo "done" \
&& echo -n "4/6 Running libtoolize... " \
&& libtoolize -c --force && echo "done" \
&& echo -n "5/6 Running autoconf... " \
&& autoconf && echo "done" \
&& echo -n "6/6 Running automake... " \
&& automake -a -c && echo "done"

