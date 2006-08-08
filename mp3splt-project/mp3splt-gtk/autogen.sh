#!/bin/sh
# Run this to set up the build system: configure, makefiles, etc.
# (based on the version in enlightenment's cvs)

autopoint -f
aclocal -I m4
autoconf
automake -a -c

cd po
for f in *.po; do
  if test -r "$f"; then
    lang=`echo $f | sed -e 's,\.po$,,'`
    msgfmt -c -o $lang.gmo $lang.po
  fi
done
cd ..
