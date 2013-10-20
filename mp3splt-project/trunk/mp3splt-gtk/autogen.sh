#!/bin/sh
win=$1

#autopoint check
(autopoint --version) > /dev/null 2>&1 ||
{
    echo
    echo "Error: you must have autopoint installed to compile mp3splt-gtk !"
    echo
    exit 1
}

#autoconf check
(autoconf --version && autoheader --version) > /dev/null 2>&1 ||
{
    echo
    echo "Error: you must have autoconf installed to compile mp3splt-gtk !"
    echo
    exit 1
}

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

#remove old libtool generated files
rm -f m4/{libtool,argz,ltdl,ltoptions,lt~obsolete,ltversion,ltsugar}.m4
rm -f libtool aclocal.m4 config.status configure autom4te.cache/* ltmain.sh

if test "x$win" != x;then
 WIN_ACLOCAL_FLAGS="-I /usr/share/aclocal"
fi

echo -n "Running autopoint... ";
autopoint --ver 0.13.1 -f && echo "done";
echo -n "Running aclocal... " \
&& aclocal -I m4 $WIN_ACLOCAL_FLAGS $ACLOCAL_FLAGS && echo "done" \
&& echo -n "Running autoheader... " \
&& autoheader && echo "done"

if test "x$win" = x; then {
  echo "Trying to initialize the help file builder... " 
  echo " " 
  echo -n "Running gnome-doc-prepare... " \
  && gnome-doc-prepare --force --automake \
  && echo "done" \
  && echo -n "Running aclocal again after adding the help files... " \
  && aclocal -I m4 $WIN_ACLOCAL_FLAGS $ACLOCAL_FLAGS \
  && echo "done"
}; fi

{
   `which glibtoolize 2>&1 >/dev/null`
    if [ $? -eq 0 ]; then
      echo -n "Running glibtoolize (in case we use the cutter testing framework) ... "
      glibtoolize -c --force
    else
      echo -n "Running libtoolize (in case we use the cutter testing framework) ... "
      libtoolize -c --force
    fi
} && echo "done"

echo -n "Running autoconf... " \
&& autoconf && echo "done" \
&& if test "x$win" = x; then {
  if test -e build-aux/gnome-doc-utils.make; then
    echo "Seems like we found working gnome documentation build utilities.";
  else
    echo "doc-dist-hook:" > build-aux/gnome-doc-utils.make;
  fi
}; else rm -f build-aux/gnome-doc-utils.make && touch build-aux/gnome-doc-utils.make; fi \
&& echo -n "Running automake... " \
&& automake -a -c && echo "done"

if ! test -z $win ;then
  touch build-aux/config.rpath
  automake -a -c
fi

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

