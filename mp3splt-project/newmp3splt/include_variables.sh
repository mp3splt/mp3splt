#!/bin/bash

#this script is included by other scripts 
#and only contains variables used in other scripts

################# variables to set ############

#build architecture
ARCH=${ARCH:=`uname -m`}
#mp3splt version
MP3SPLT_VERSION=2.2_rc1
#the libmp3splt version (if mp3splt is only compatible with this
#libmp3splt version)
LIBMP3SPLT_VERSION=0.4_rc1
#the mp3splt documentation files
MP3SPLT_DOC_FILES=(AUTHORS ChangeLog COPYING INSTALL NEWS README TODO)
#the mp3splt installed files (needed for openbsd, freebsd, ...)
MP3SPLT_FILES=(bin/mp3splt)
#mp3splt man page
MP3SPLT_MAN1_FILES=(mp3splt.1)

#we put i386 if i686 except for archlinux, where we set i686 inside
#its make_ script
if [[ $ARCH = "i486" ]] || [[ $ARCH = "i586" ]] || [[ $ARCH = "i686" ]];then
    ARCH="i386";
fi

#compilation flags
if [[ $1 != "noflags" ]];then
    if [[ $ARCH = "i386" ]];then
        CFLAGS="-O2 -march=${ARCH}"
        LDFLAGS=""
    fi
fi

################# end variables to set ############

if [[ $1 != "quiet" ]];then
    echo
    echo $'Application :\tmp3splt'
    echo $'Architecture :\t'$ARCH
    echo $'Version :\t'$MP3SPLT_VERSION
fi
