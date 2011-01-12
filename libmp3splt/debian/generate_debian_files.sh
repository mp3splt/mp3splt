#!/bin/bash

#this scripts generates "docs" files

#we move in the current script directory
script_dir=$(readlink -f $0) || exit 1
script_dir=${script_dir%\/*.sh}
cd $script_dir

. ../include_variables.sh "quiet_noflags" || exit 1

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
