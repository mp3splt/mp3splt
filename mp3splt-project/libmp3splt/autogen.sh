#!/bin/sh
# Run this to set up the build system: configure, makefiles, etc.
# (based on the version in enlightenment's cvs)

aclocal

autoconf

automake -a -c
