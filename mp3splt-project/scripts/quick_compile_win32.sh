#!/usr/bin/env sh

cd libmp3splt && make clean && make && make install && cd ..
cd newmp3splt && make clean && make && make install && cd ..

#./newmp3splt/src/mp3splt.exe -t 1.0 song.mp3

#./newmp3splt/src/mp3splt.exe -q -d test -o @n-@a-@t-@p -g [@a=artist1,@t=title1,@p=performer,@c=comment][@a=artist2,@t=title2,@p=perfo2,@c=comment2][@a=artist3,@t=title3,@p=perfor3,@c=comment3] -t 1.0 song.mp3
#./newmp3splt/src/mp3splt.exe -q -d test -o @n-@a-@t-@p -g %[@a=artist1,@t=title1,@p=performer,@c=comment] -t 1.0 song.mp3
