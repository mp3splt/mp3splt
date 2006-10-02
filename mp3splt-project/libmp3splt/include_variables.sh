#!/bin/bash

#this script is included by other scripts 
#and only contains variables used in other scripts

################# variables to set ############

ARCH=${ARCH:=i386}
LIBMP3SPLT_VERSION=0.4_rc1
LIBMP3SPLT_DOC_FILES=(AUTHORS ChangeLog COPYING INSTALL NEWS README TODO LIMITS)

#the libmp3splt installed files (needed for openbsd, freebsd,...)
LIBMP3SPLT_FILES=(include/libmp3splt/mp3splt.h \
include/libmp3splt/mp3splt_types.h \
lib/libmp3splt.a \
lib/libmp3splt.la \
lib/libmp3splt.so.0.0)

#if we modify the subversion repository (the ebuild needs renaming)
SUBVERSION=${SUBVERSION:=0}

################# end variables to set ############

if [[ $1 != "quiet" ]];then
    echo
    echo $'Application :\tlibmp3splt'
    echo $'Architecture :\t'$ARCH
    echo $'Version :\t'$LIBMP3SPLT_VERSION
fi
