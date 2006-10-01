#!/bin/bash

################# variables to set ############

MP3SPLT_VERSION=2.2_rc1
MP3SPLT_DOC_FILES=(AUTHORS ChangeLog COPYING INSTALL NEWS README TODO)

################# end variables to set ############

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir/..
cd $PROGRAM_DIR

SLACK_TEMP=/tmp/slack_temp

#we set the necessary flags
export CFLAGS="-O2 -march=i486 -mcpu=i486"
export LDFLAGS=""

#we create the needed directories
rm -rf $SLACK_TEMP/mp3splt/*
mkdir -p $SLACK_TEMP/mp3splt/usr/doc/mp3splt
mkdir -p $SLACK_TEMP/mp3splt/install

#we compile
./autogen.sh &&\
./configure --prefix=/usr &&\
make clean &&\
make &&\
make DESTDIR=$SLACK_TEMP/mp3splt install &&\
cp $MP3SPLT_DOC_FILES $SLACK_TEMP/mp3splt/usr/doc/mp3splt &&\
cp slackware/slack-* $SLACK_TEMP/mp3splt/install &&\
cd $SLACK_TEMP/mp3splt &&\
/sbin/makepkg -l y -c y mp3splt-${MP3SPLT_VERSION}-i486.tgz &&\
mv mp3splt-${MP3SPLT_VERSION}-i486.tgz $PROGRAM_DIR/..
