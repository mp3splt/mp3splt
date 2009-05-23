#!/bin/sh

#autoconf check
(autoconf --version && autoheader --version) > /dev/null 2>&1 ||
{
    echo
    echo "Error: you must have autoconf installed to compile mp3splt !"
    echo
    exit 1
}

#automake check
(aclocal --version) > /dev/null 2>&1 ||
{
    echo
    echo "Error: you must have automake installed to compile mp3splt !"
    echo
    exit 1
}

#we run aclocal, autoconf and automake
echo -n "1/5 Running autopoint... " \
&& autopoint -f && echo "done" \
&& echo -n "2/5 Running aclocal... " \
&& aclocal -I m4 $ACLOCAL_FLAGS && echo "done" \
&& echo -n "3/5 Running autoheader... " \
&& autoheader && echo "done" \
&& echo -n "4/5 Running autoconf... " \
&& autoconf && echo "done" \
&& echo -n "5/5 Running automake... " \
&& automake -a -c && echo "done"

