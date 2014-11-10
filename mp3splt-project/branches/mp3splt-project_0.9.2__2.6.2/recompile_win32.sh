#!/bin/sh

export PKG_CONFIG_PATH="/usr/lib/pkgconfig"

make -f Makefile.win32 recompile || exit 1;
make -f Makefile.win32 dist || exit 1

