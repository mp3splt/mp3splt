#!/usr/local/bin/bash

#this file creates a openbsd package for mp3splt-gtk
#it must be run from the openbsd directory : ./do_pkg.sh
###########modify variables###########

files=(bin/mp3splt-gtk \
share/pixmaps/mp3splt-gtk.png \
share/pixmaps/mp3splt-gtk_ico.png \
share/locale/fr/LC_MESSAGES/mp3splt-gtk.mo)

doc_files=(AUTHORS COPYING ChangeLog INSTALL NEWS README TODO);

export AUTOMAKE_VERSION="1.9";
export AUTOCONF_VERSION="2.59";

###########end modify variables###########

VERSION=0.4_rc1
NAME="mp3splt-gtk"

#we compile and install the library
export CFLAGS="-I/usr/include -I/usr/local/include"
export LDFLAGS="-L/usr/lib -L/usr/local/lib"
cd .. && ./autogen.sh && ./configure && make clean \
&& make && make install
cd openbsd

#we write the file for the package
echo $"@comment Mp3splt-gtk is a GTK2 gui that uses libmp3splt to split mp3 and ogg without deconding
@comment OpenBSD package by Munteanu Alexandru Ionut <io_alex_2002@yahoo.fr>
@name $NAME-obsd-$VERSION
@arch i386
@depend audio/libmp3splt:libmp3splt-obsd-*:libmp3splt-obsd-0.4_rc1
@depend audio/bmp:bmp-*:bmp-*
@depend devel/glib2:glib2-*:glib2-*
@depend x11/gtk+2:gtk+2-*:gtk+2-*" > +CONTENTS;

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
