#!/bin/sh

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
echo -n "Running aclocal... " \
&& aclocal $ACLOCAL_FLAGS && echo "done" \
&& echo -n "Running autoheader... " \
&& autoheader && echo "done" \
&& echo -n "Running autoconf... " \
&& autoconf && echo "done" \
&& echo -n "Running automake... " && echo "done" \
&& automake -a -c
