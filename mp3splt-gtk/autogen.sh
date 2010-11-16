#!/bin/sh

#autoconf check
(autoconf --version && autoheader --version) > /dev/null 2>&1 ||
{
    echo
    echo "Error: you must have autoconf installed to compile mp3splt-gtk !"
    echo
    exit 1
}

echo -n "Preparing the files for the help system with gnome-doc-prepare... ";
gnome-doc-prepare --automake && echo "done";

#automake check
(aclocal --version) > /dev/null 2>&1 ||
{
    echo
    echo "Error: you must have automake installed to compile mp3splt-gtk !"
    echo
    exit 1
}

#msgfmt check
(msgfmt --version) > /dev/null 2>&1 ||
{
    echo
    echo "Error: you must have gettext(msgfmt) installed to compile mp3splt-gtk !"
    echo
    exit 1
}

echo -n "Running autopoint... ";
autopoint -f && echo "done";
echo -n "Running aclocal... " \
&& aclocal -I m4 $ACLOCAL_FLAGS && echo "done" \
&& echo -n "Running autoheader... " \
&& autoheader && echo "done" \
&& echo -n "Running autoconf... " \
&& autoconf && echo "done" \
&& echo -n "Running automake... " \
&& automake -a -c && echo "done"

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
