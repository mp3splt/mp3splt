#!/bin/bash

aclocal -I m4
autoconf
automake -a -c
