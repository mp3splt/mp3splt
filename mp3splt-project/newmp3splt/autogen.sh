#!/bin/sh

aclocal -I m4 $ACLOCAL_FLAGS
autoconf
automake -a -c
