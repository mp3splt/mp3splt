#!/bin/bash

#this scripts generates the "control" and "docs" files

#we move in the current script directory
script_dir=$(readlink -f $0) || exit 1
script_dir=${script_dir%\/*.sh}
cd $script_dir

. ../include_variables.sh "quiet_noflags" || exit 1

#generate the control file
echo "Source: mp3splt-gtk
Section: sound
Priority: optional
Maintainer: Munteanu Alexandru Ionut (io_alex_2002@yahoo.fr)
Build-Depends: debhelper (>= 4.0.0), libmp3splt (>= ${LIBMP3SPLT_VERSION})
Standards-Version: 3.6.1.1

Package: mp3splt-gtk
Architecture: any
Depends: \${shlibs:Depends}, libmp3splt (>= ${LIBMP3SPLT_VERSION})
Suggests: mp3splt
Description: GTK2 gui that uses libmp3splt for lossless linear editing
 of audio files (cut an audio file into several tracks.)
 Since there is no decoding step involved this means that MP3 (including
 VBR) and Ogg Vorbis can be split into smaller files without
 loosing any quality. Useful for splitting albums into tracks which
 can be done either manually, using freedb.org data from the net
 or using .cue files ...
 Homepage: http://mp3splt.sourceforge.net/
" > control

#generate the docs file
counter=1;
for doc in "${MP3SPLT_GTK_DOC_FILES[@]}";do
    if [[ $doc != "COPYING" ]];then
        if [[ $counter == 1 ]];then
            echo $doc > ./docs
            counter=2
        else
            echo $doc >> ./docs
        fi
    fi
done
