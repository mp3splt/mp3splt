#!/bin/bash

#this script is included by other scripts 
#and only contains variables used in other scripts

################# variables to set ############

#build architecture
ARCH=${ARCH:=`uname -m`}
#libmp3splt version
LIBMP3SPLT_VERSION=0.4_rc1
#the libmp3splt documentation files
LIBMP3SPLT_DOC_FILES=(AUTHORS ChangeLog COPYING INSTALL NEWS README TODO LIMITS)
#the libmp3splt installed files (needed for openbsd, freebsd,...)
LIBMP3SPLT_FILES=(include/libmp3splt/mp3splt.h \
include/libmp3splt/mp3splt_types.h \
lib/libmp3splt.a \
lib/libmp3splt.la \
lib/libmp3splt.so.0.0)

#we put i386 if i686 except for archlinux, where we set i686 inside
#its make_ script
if [[ $ARCH = "i486" ]] || [[ $ARCH = "i586" ]] || [[ $ARCH = "i686" ]];then
    ARCH="i386";
fi

#compilation flags
if [[ $1 != "noflags" ]];then
    if [[ $ARCH = "i386" ]];then
        CFLAGS="-O2 -march=${ARCH} $CFLAGS"
        LDFLAGS="$LDFLAGS"
    fi
fi

################# end variables to set ############

if [[ $1 != "quiet" ]];then
    echo
    echo $'Application :\tlibmp3splt'
    echo $'Architecture :\t'$ARCH
    echo $'Version :\t'$LIBMP3SPLT_VERSION
fi
