#!/bin/bash

#this scripts generates the "control" and "docs" files

#we move in the current script directory
script_dir=$(readlink -f $0) || exit 1
script_dir=${script_dir%\/*.sh}
cd $script_dir

. ../include_variables.sh "quiet_noflags" || exit 1

#generate the control file
echo "Source: mp3splt
Section: sound
Priority: optional
Maintainer: Ryan Niebur <ryanryan52@gmail.com>
DM-Upload-Allowed: yes
Build-Depends: debhelper (>= 7.0), libmp3splt-dev (>= $LIBMP3SPLT_VERSION)
Standards-Version: 3.9.1
Homepage: http://mp3splt.sourceforge.net/

Package: mp3splt
Architecture: any
Depends: \${misc:Depends}, \${shlibs:Depends}, libmp3splt0 (>= $LIBMP3SPLT_VERSION), libmp3splt0-mp3 | libmp3splt-plugin
Recommends: libmp3splt0-ogg
Description: Command line program that splits MP3 and Ogg Vorbis files without reencoding
 Used to split MP3 (VBR supported) and Ogg Vorbis
 files into smaller files without decoding. Useful for splitting albums, either
 manually, using freedb.org data, or .cue files ...
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
