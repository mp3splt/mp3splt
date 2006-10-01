#!/bin/bash

################# variables to set ############

LIBMP3SPLT_VERSION=0.4_rc1
LIBMP3SPLT_DOC_FILES=(AUTHORS ChangeLog COPYING INSTALL NEWS README TODO LIMITS)

################# end variables to set ############

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir/..
cd $PROGRAM_DIR

#we set the necessary flags
export CFLAGS="-O2 -march=i486 -mcpu=i486";
export LDFLAGS="";

SLACK_TEMP=/tmp/slack_temp

#we create the needed directories
rm -rf $SLACK_TEMP/libmp3splt/*
mkdir -p $SLACK_TEMP/libmp3splt/usr/doc/libmp3splt
mkdir -p $SLACK_TEMP/libmp3splt/install

#we compile
./autogen.sh &&\
./configure --prefix=/usr &&\
make clean &&\
make &&\
#we install in /usr and we run ldconfig
make install &&\
/sbin/ldconfig &&\
make DESTDIR=$SLACK_TEMP/libmp3splt install &&\
cp $LIBMP3SPLT_DOC_FILES $SLACK_TEMP/libmp3splt/usr/doc/libmp3splt &&\
cp slackware/slack-* $SLACK_TEMP/libmp3splt/install &&\
cd $SLACK_TEMP/libmp3splt &&\
/sbin/makepkg -l y -c y libmp3splt-${LIBMP3SPLT_VERSION}-i486.tgz &&\
mv libmp3splt-${LIBMP3SPLT_VERSION}-i486.tgz $PROGRAM_DIR/..
