#!/bin/sh

autopoint -f
aclocal -I m4 $ACLOCAL_FLAGS
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
