#!/bin/bash

#this scripts generates the "control" and "docs" files

#we move in the current script directory
script_dir=$(readlink -f $0) || exit 1
script_dir=${script_dir%\/*.sh}
cd $script_dir

. ../include_variables.sh "quiet_noflags" || exit 1

#generate the control file
echo "Source: libmp3splt
Priority: optional
Maintainer: Ryan Niebur <ryanryan52@gmail.com>
DM-Upload-Allowed: yes
Build-Depends: debhelper (>= 7.2), gawk, libogg-dev, libvorbis-dev, libmad0-dev, libltdl3-dev, libid3tag0-dev
Standards-Version: 3.9.1
Section: libs
Homepage: http://mp3splt.sourceforge.net/
Vcs-Git: git://git.ryan52.info/git/libmp3splt
Vcs-Browser: http://git.ryan52.info/?p=libmp3splt;a=summary

Package: libmp3splt
Section: sound
Architecture: any
Conflicts: libmp3splt
Replaces: libmp3splt
Depends: \${misc:Depends}, \${shlib:Depends}
Description:
 Mp3Splt-project is a utility to split mp3 and ogg files selecting a
 begin and an end time position, without decoding. It's very useful to
 split large mp3/ogg to make smaller files or to split entire albums to
 obtain original tracks. If you want to split an album, you can select
 split points and filenames manually or you can get them automatically
 from CDDB (internet or a local file) or from .cue files. Supports also
 automatic silence split, that can be used also to adjust cddb/cue
 splitpoints. You can extract tracks from Mp3Wrap or AlbumWrap files in
 few seconds. For mp3 files, both ID3v1 & ID3v2 tags are
 supported. Mp3splt-project is split in 3 parts: libmp3splt, mp3splt
 and mp3splt-gtk." > control

#generate the docs file
counter=1;
for doc in "${LIBMP3SPLT_DOC_FILES[@]}";do
    if [[ $doc != "COPYING" ]];then
        if [[ $counter == 1 ]];then
            echo $doc > ./docs
            counter=2
        else
            echo $doc >> ./docs
        fi
    fi
done
