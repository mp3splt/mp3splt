#!/usr/local/bin/bash

#this file creates a openbsd package for mp3splt-gtk

################# variables to set ############

export AUTOMAKE_VERSION="1.9";
export AUTOCONF_VERSION="2.59";

################# end variables to set ############

#we move in the current script directory
script_dir=$(readlink -f $0) || exit 1
script_dir=${script_dir%\/*.sh}
cd $script_dir

. ../include_variables.sh

echo
echo $'Package :\topenbsd'
echo

VERSION=$MP3SPLT_GTK_VERSION
NAME="mp3splt-gtk"

#we compile and install the library
export CFLAGS="-I/usr/include -I/usr/local/include $CFLAGS"
export LDFLAGS="-L/usr/lib -L/usr/local/lib $LDFLAGS"
cd .. && ./autogen.sh && ./configure --enable-bmp && make clean \
&& make && make install || exit 1
cd openbsd

#we write the file for the package
echo $"@comment Mp3splt-gtk is a GTK2 gui that uses libmp3splt to split mp3 and ogg without deconding
@comment OpenBSD package by Munteanu Alexandru Ionut <io_alex_2002@yahoo.fr>
@name ${NAME}_obsd_$ARCH-${VERSION}
@arch ${ARCH}
@depend audio/libmp3splt:libmp3splt_obsd_$ARCH-*:libmp3splt_obsd_$ARCH-${LIBMP3SPLT_VERSION}
@depend audio/bmp:bmp-*:bmp-*
@depend devel/glib2:glib2-*:glib2-*
@depend x11/gtk+2:gtk+2-*:gtk+2-*" > +CONTENTS;

echo "@cwd /usr/local" >> +CONTENTS;
#we put the dist files
for a in "${MP3SPLT_GTK_FILES[@]}";
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
cp "${MP3SPLT_GTK_DOC_FILES[@]}" /usr/local/share/doc/$NAME
chown -R root:wheel /usr/local/share/doc/$NAME
cd openbsd
for a in "${MP3SPLT_GTK_DOC_FILES[@]}";
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
`pkg_create -f +CONTENTS` || exit 1

#we copy the results
mv ${NAME}_obsd_*.tgz ../.. || exit 1

#we uninstall the program
cd .. && make uninstall && rm -rf /usr/local/share/doc/$NAME
