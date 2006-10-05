#!/bin/bash

#this scripts generates the "control" and "docs" files

#we move in the current script directory
script_dir=$(readlink -f $0) || exit 1
script_dir=${script_dir%\/*.sh}
cd $script_dir

. ../include_variables.sh "quiet"

#generate the control file
echo "Source: mp3splt
Section: sound
Priority: optional
Maintainer: Munteanu Alexandru Ionut (io_alex_2002@yahoo.fr)
Build-Depends: debhelper (>= 4.0.0), libmp3splt (= ${LIBMP3SPLT_VERSION})
Standards-Version: 3.6.1.1

Package: mp3splt
Architecture: any
Depends: ${shlibs:Depends}, libmp3splt (= ${LIBMP3SPLT_VERSION})
Description: Command line program that splits MP3 and Ogg Vorbis files without reencoding
 Used to split MP3 (VBR supported) and Ogg Vorbis
 files into smaller files without decoding. Useful for splitting albums, either
 manually, using freedb.org data, or .cue files ...
 .
 Homepage: http://mp3splt.sourceforge.net/
" > control

#generate the docs file
counter=1;
for doc in "${MP3SPLT_DOC_FILES[@]}";do
    if [[ $doc != "COPYING" ]];then
        if [[ $counter == 1 ]];then
            echo $doc > ./docs
            counter=2
        else
            echo $doc >> ./docs
        fi
    fi
done
