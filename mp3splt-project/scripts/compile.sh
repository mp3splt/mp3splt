#!/usr/bin/env bash

mkdir temp
mkdir -p new_dist
rm -rf new_dist/*
export CFLAGS="-I`pwd`/temp/include"
export LDFLAGS="-L`pwd`/temp/lib"
make
mv ./mp3splt-gtk/mp3splt-gtk*tar.gz ./
mv ./libmp3splt/libmp3splt*tar.gz ./
rm -rf ./temp
./crosscompile_win32.sh
mv ./mp3splt-gtk*exe ./new_dist
mv ./*.deb ./new_dist
mv ./*.tar.gz ./new_dist
mv ./mp3splt*exe ./new_dist
mv ./newmp3splt/mp3splt*.tar.gz ./new_dist
