#!/bin/sh

aclocal $ACLOCAL_FLAGS
autoconf
automake -a -c
