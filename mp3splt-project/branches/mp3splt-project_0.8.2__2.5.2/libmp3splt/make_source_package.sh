#!/bin/bash

#we move in the current script directory
if [[ $1 == "netbsd" ]];then
    script_dir=$(greadlink -f $0) || exit 1
else
    script_dir=$(readlink -f $0) || exit 1
fi;
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir
cd $PROGRAM_DIR

. ./include_variables.sh || exit 1

put_package "source"

#if we don't have the distribution file
DIST_FILE="../libmp3splt-${LIBMP3SPLT_VERSION}.tar.gz"
if [[ ! -f $DIST_FILE ]];then
    #create the directories we need
    TEMPDIR=/tmp/temp
    DATEMV=`date +-%d_%m_%Y__%H_%M_%S`
    if [[ -e $TEMPDIR ]];then
        mv $TEMPDIR ${TEMPDIR}${DATEMV}
    fi
    mkdir -p $TEMPDIR
    
    #we compile
    ./autogen.sh &&\
        ./configure --prefix=/usr &&\
        make clean && make distcheck && make dist && make install DESTDIR=$TEMPDIR &&\
        mv libmp3splt*.tar.gz ../ || exit 1
else
    put_is_package_warning "We already have the $DIST_FILE distribution file !"
fi
