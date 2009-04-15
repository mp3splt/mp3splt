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

#we run aclocal, autoconf and automake
#info: the 'sed' is an ugly hack for libtool version compatibility because aclocal.m4 is shipped in 'make dist'
echo -n "1/5 Running aclocal... " \
&& aclocal -Im4 $ACLOCAL_FLAGS && echo "done" \
&& if [[ -z $win ]];then sed -i "s/lt_dlcaller_register/lt_dlcaller_set_data/" aclocal.m4;fi \
&& echo -n "2/5 Running autoheader... " \
&& autoheader && echo "done" \
&& echo -n "3/5 Running libtoolize... " \
&& libtoolize -c --force && echo "done" \
&& echo -n "4/5 Running autoconf... " \
&& autoconf && echo "done" \
&& echo -n "5/5 Running automake... " \
&& automake -a -c && echo "done"


