#!/bin/bash

#this script is included by other scripts 
#and only contains variables used in other scripts

################# variables to set ############

ARCH=${ARCH:=i386}
MP3SPLT_GTK_VERSION=0.4rc1
LIBMP3SPLT_VERSION=0.4_rc1
MP3SPLT_GTK_DOC_FILES=(AUTHORS ChangeLog COPYING INSTALL NEWS README TODO)

################# end variables to set ############

if [[ $1 != "quiet" ]];then
    echo
    echo $'Application :\tmp3splt-gtk'
    echo $'Architecture :\t'$ARCH
    echo $'Version :\t'$MP3SPLT_GTK_VERSION
fi
