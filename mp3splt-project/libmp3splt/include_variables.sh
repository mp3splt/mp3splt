#!/bin/bash

#this script is included by other scripts 
#and only contains variables used in other scripts

################# variables to set ############

ARCH=${ARCH:=i386}
LIBMP3SPLT_VERSION=0.4_rc1
LIBMP3SPLT_DOC_FILES=(AUTHORS ChangeLog COPYING INSTALL NEWS README TODO LIMITS)

################# end variables to set ############

echo
echo $'Application :\tlibmp3splt'
echo $'Architecture :\t'$ARCH
echo $'Version :\t'$LIBMP3SPLT_VERSION
