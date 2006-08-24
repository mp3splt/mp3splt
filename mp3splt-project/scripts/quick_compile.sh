#!/bin/bash

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
cd $script_dir/

cd ../libmp3splt && make clean && make && make install
cd ../newmp3splt && make clean && make && make install

