#!/usr/bin/env bash

cd ../libmp3splt && make clean && make && make install
cd ../newmp3splt && make clean && make && make install

