#!/bin/bash

################# variables to set ############

LIBMP3SPLT_DIR=libmp3splt
MP3SPLT_DIR=newmp3splt
MP3SPLT_GTK_DIR=mp3splt-gtk

################## end variables to set ############

DATE_START=`date`

script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROJECT_DIR=$script_dir/..
cd $PROJECT_DIR

cd $LIBMP3SPLT_DIR/po && rm -f libmp3splt.pot && make clean
make || exit 1
cd -

cd $MP3SPLT_DIR/po && rm -f mp3splt.pot && make clean
make || exit 1
cd -

cd $MP3SPLT_GTK_DIR/po && rm -f mp3splt-gtk.pot && make clean
make || exit 1
cd -

