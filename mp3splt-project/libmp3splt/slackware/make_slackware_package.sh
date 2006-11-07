#!/bin/bash

#we move in the current script directory
script_dir=$(readlink -f $0) || exit 1
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir/..
cd $PROGRAM_DIR

. ./include_variables.sh

put_package "slackware"

#if we don't have the distribution file
DIST_FILE="../libmp3splt-${LIBMP3SPLT_VERSION}-${ARCH}.tgz"
if [[ ! -f $DIST_FILE ]];then
    #we set the necessary flags
    #export CFLAGS="-O2 -march=$ARCH -mcpu=$ARCH";
    export CFLAGS="$CFLAGS";
    export LDFLAGS="$LDFLAGS";
    
    SLACK_TEMP=/tmp/slack_temp
    
    #we create the needed directories
    DATEMV=`date +-%d_%m_%Y__%H_%M_%S`
    if [[ -e $SLACK_TEMP/libmp3splt ]];then
        mv $SLACK_TEMP/libmp3splt $SLACK_TEMP/libmp3splt${DATEMV}
    fi
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
        /sbin/makepkg -l y -c y libmp3splt-${LIBMP3SPLT_VERSION}-$ARCH.tgz &&\
        mv libmp3splt-${LIBMP3SPLT_VERSION}-$ARCH.tgz $PROGRAM_DIR/.. || exit 1
else
    put_is_package_warning "We already have the $DIST_FILE distribution file !"
fi
