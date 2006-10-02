#!/bin/bash

#this script is included by other scripts 
#and only contains variables used in other scripts

################# variables to set ############

ARCH=${ARCH:=i386}
MP3SPLT_VERSION=2.2_rc1
MP3SPLT_DOC_FILES=(AUTHORS ChangeLog COPYING INSTALL NEWS README TODO)

MP3SPLT_FILES=(bin/mp3splt)
MP3SPLT_MAN1_FILES=(mp3splt.1)

LIBMP3SPLT_VERSION=0.4_rc1

################# end variables to set ############

if [[ $1 != "quiet" ]];then
    echo
    echo $'Application :\tmp3splt'
    echo $'Architecture :\t'$ARCH
    echo $'Version :\t'$MP3SPLT_VERSION
fi
