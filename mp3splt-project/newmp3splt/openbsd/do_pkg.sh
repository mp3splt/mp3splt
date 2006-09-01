#!/usr/local/bin/bash

#this file creates a openbsd package for mp3splt
#it must be run from the openbsd directory : ./do_pkg.sh
###########modify variables###########

files=(bin/mp3splt)
doc_files=(AUTHORS COPYING ChangeLog INSTALL NEWS README TODO);
man1_files=(mp3splt.1);

export AUTOMAKE_VERSION="1.9";
export AUTOCONF_VERSION="2.59";

###########end modify variables###########

VERSION=2.2_rc1
NAME="mp3splt"

#we compile and install the library
export CFLAGS="-I/usr/include -I/usr/local/include"
export LDFLAGS="-L/usr/lib -L/usr/local/lib"
cd .. && ./autogen.sh && ./configure && make clean \
&& make && make install
cd openbsd

#we write the file for the package
echo $"@comment Mp3splt is the command line program from the mp3splt-project, to split mp3 and ogg without decoding
@comment OpenBSD package by Munteanu Alexandru Ionut <io_alex_2002@yahoo.fr>
@name $NAME-obsd-$VERSION
@arch i386
@depend audio/libmp3splt:libmp3splt-obsd-*:libmp3splt-obsd-0.4_rc1" > +CONTENTS;

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

#manual page
for a in "${man1_files[@]}";
do
  echo "@man man/man1/$a" >> +CONTENTS;
  md5=`md5 /usr/local/man/man1/$a | cut -d" " -f 4`;
  size=`ls -l /usr/local/man/man1/$a | cut -d" " -f 8`;
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
