#!/bin/sh
cd libmp3splt
debuild -S
cd ..
cd newmp3splt
debuild -S
cd ..
cd mp3splt-gtk
debuild -S
cd ..
