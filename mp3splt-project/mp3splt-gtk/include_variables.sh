#!/bin/bash

#this script is included by other scripts 
#and only contains variables used in other scripts

################# variables to set ############

#build architecture
ARCH=${ARCH:=`uname -m`}
#mp3splt-gtk version
MP3SPLT_GTK_VERSION=0.4_rc1
#the libmp3splt version (if mp3splt-gtk is only compatible with this
#libmp3splt version)
LIBMP3SPLT_VERSION=0.4_rc1
#the mp3splt-gtk documentation files
MP3SPLT_GTK_DOC_FILES=(AUTHORS ChangeLog COPYING INSTALL NEWS README TODO)
#the mp3splt-gtk installed files (needed for openbsd, freebsd, ...)
MP3SPLT_GTK_FILES=(bin/mp3splt-gtk \
share/pixmaps/mp3splt-gtk.png \
share/pixmaps/mp3splt-gtk_ico.png \
share/locale/fr/LC_MESSAGES/mp3splt-gtk.mo)

#description of the package
#used for rpm, freebsd and netbsd
#slackware has separate description file
MP3SPLT_GTK_DESCRIPTION="mp3splt-project common features:
* split mp3 and ogg files from a begin time to an end time without decoding
* split an album with splitpoints from the freedb.org server
* split an album with local .XMCD, .CDDB or .CUE file
* split files automatically with silence detection
* split files by a fixed time length
* split files created with Mp3Wrap or AlbumWrap
* split concatenated mp3 files
* support for mp3 VBR (variable bit rate)
* specify output directory for splitted files"

#we put i386 if i686 except for archlinux, where we set i686 inside
#its make_ script
if [[ $ARCH = "i486" || $ARCH = "i586" || $ARCH = "i686" || $ARCH = "i86pc" ]];then
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
    echo $'Application :\tmp3splt-gtk'
    echo $'Architecture :\t'$ARCH
    echo $'Version :\t'$MP3SPLT_GTK_VERSION
fi
