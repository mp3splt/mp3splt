#!/bin/bash

################# variables to set ############

MP3SPLT_GTK_VERSION=0.4_rc1
MP3SPLT_GTK_DOC_FILES=(AUTHORS ChangeLog COPYING INSTALL NEWS README TODO)

################# end variables to set ############

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir/..
cd $PROGRAM_DIR

SLACK_TEMP=/tmp/slack_temp

#we set the necessary flags
export CFLAGS="-O2 -march=i486 -mcpu=i486";
export LDFLAGS="";

#we create the needed directories
rm -rf $SLACK_TEMP/mp3splt-gtk/*
mkdir -p $SLACK_TEMP/mp3splt-gtk/usr/doc/mp3splt-gtk
mkdir -p $SLACK_TEMP/mp3splt-gtk/install

#we compile
./autogen.sh &&\
./configure --enable-bmp --prefix=/usr &&\
make clean &&\
make &&\
fakeroot make DESTDIR=$SLACK_TEMP/mp3splt-gtk install &&\
cp $MP3SPLT_GTK_DOC_FILES $SLACK_TEMP/mp3splt-gtk/usr/doc/mp3splt-gtk &&\
cp slackware/slack-* $SLACK_TEMP/mp3splt-gtk/install &&\
cd $SLACK_TEMP/mp3splt-gtk &&\
fakeroot /sbin/makepkg -l y -c y mp3splt-gtk-${MP3SPLT_GTK_VERSION}-i486.tgz &&\
mv mp3splt-gtk-${MP3SPLT_GTK_VERSION}-i486.tgz $PROGRAM_DIR/..
