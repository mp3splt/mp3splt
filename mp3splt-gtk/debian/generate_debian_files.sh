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
Maintainer: Ryan Niebur <ryanryan52@gmail.com>
DM-Upload-Allowed: yes
Build-Depends: debhelper (>= 7.0), libmp3splt-dev (>= $LIBMP3SPLT_VERSION), libgtk2.0-dev, libgstreamer1.0-dev, libgstreamer-plugins-base1.0-dev, audacious-dev, gnome-doc-utils, libgnomeui-dev, rarian-compat
Standards-Version: 3.9.1
Homepage: http://mp3splt.sourceforge.net/

Package: mp3splt-gtk
Architecture: any
Depends: \${shlibs:Depends}, \${misc:Depends}, libmp3splt0 (>= $LIBMP3SPLT_VERSION), libmp3splt0-mp3 | libmp3splt-plugin
Recommends: gstreamer1.0-plugins-good, libmp3splt0-ogg
Suggests: audacious
Description: Gnome application for lossless linear editing of audio files
 This application allows to cut an audio file into several tracks.
 Since there is no decoding step involved this means that MP3 (including
 VBR) and Ogg Vorbis can be split into smaller files without
 loosing any quality. Useful for splitting albums into tracks which
 can be done either manually, using freedb.org data from the net
 or using .cue files ..." > control

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
