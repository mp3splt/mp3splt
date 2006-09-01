#!/usr/local/bin/bash

#this file creates a openbsd package for libmp3splt
#it must be run from the openbsd directory : ./do_pkg.sh
###########modify variables###########

files=(include/libmp3splt/mp3splt.h \
include/libmp3splt/mp3splt_types.h \
lib/libmp3splt.a \
lib/libmp3splt.la \
lib/libmp3splt.so.0.0)
doc_files=(AUTHORS COPYING ChangeLog INSTALL LIMITS NEWS README TODO);

export AUTOMAKE_VERSION="1.9";
export AUTOCONF_VERSION="2.59";

###########end modify variables###########

VERSION=0.4_rc1
NAME="libmp3splt"

#we compile and install the library
export CFLAGS="-I/usr/include -I/usr/local/include"
export LDFLAGS="-L/usr/lib -L/usr/local/lib"
cd .. && ./autogen.sh && ./configure && make clean \
&& make && make install
cd openbsd

#we write the file for the package
echo $"@comment Libmp3splt is a library created from mp3splt v2.1c to split mp3 and ogg without deconding
@comment OpenBSD package by Munteanu Alexandru Ionut <io_alex_2002@yahoo.fr>
@name $NAME-obsd-$VERSION
@arch i386
@depend audio/libmad:libmad-*:libmad-*
@depend audio/libid3tag:libid3tag-*:libid3tag-*
@depend audio/libogg:libogg-*:libogg-*
@depend audio/libvorbis:libvorbis-*:libvorbis-*" > +CONTENTS;

echo "@cwd /usr/local" >> +CONTENTS;
#we put the dist files
for a in "${files[@]}";
do
  echo "$a" >> +CONTENTS;
  md5=`md5 /usr/local/$a | cut -d" " -f 4`;
  size=`ls -l /usr/local/$a | cut -d" " -f 8`;
  echo "@md5 $md5" >> +CONTENTS;
  echo "@size $size" >> +CONTENTS;
done;

#we put the documentation files
mkdir -p /usr/local/share/doc/$NAME
cd ..
cp "${doc_files[@]}" /usr/local/share/doc/$NAME
chown -R root:wheel /usr/local/share/doc/$NAME
cd openbsd
for a in "${doc_files[@]}";
do
  echo "share/doc/$NAME/$a" >> +CONTENTS;
  md5=`md5 /usr/local/share/doc/$NAME/$a | cut -d" " -f 4`;
  size=`ls -l /usr/local/share/doc/$NAME/$a | cut -d" " -f 8`;
  echo "@md5 $md5" >> +CONTENTS;
  echo "@size $size" >> +CONTENTS;
done;

echo "@exec /sbin/ldconfig -m %D/lib
@unexec /sbin/ldconfig -R" >> +CONTENTS;

#create package
`pkg_create -f +CONTENTS`

#erase +CONTENTS
echo "" > +CONTENTS;

#we uninstall the library
cd .. && make uninstall && rm -rf /usr/local/share/doc/$NAME
cd openbsd

#we copy the results
mv $NAME-obsd*.tgz ../..
