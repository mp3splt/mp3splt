#!/bin/bash

#this file creates a netbsd package for mp3splt

#we move in the current script directory
script_dir=$(readlink -f $0)
script_dir=${script_dir%\/*.sh}
PROGRAM_DIR=$script_dir
cd $PROGRAM_DIR

. ../include_variables.sh

echo
echo $'Package :\tnetbsd'
echo

#we generate the makefile
echo "# \$NetBSD\$

DISTNAME=       mp3splt-${MP3SPLT_VERSION}
PKGNAME=        mp3splt-nbsd-${MP3SPLT_VERSION}
CATEGORIES=     audio
MASTER_SITES=   \${MASTER_SITE_SOURCEFORGE:=mp3splt/}

MAINTAINER=     io_alex_2002@yahoo.fr
HOMEPAGE=       http://mp3splt.sourceforge.net/
COMMENT=        Command line program to split mp3 and ogg without decoding

PKG_INSTALLATION_TYPES= overwrite pkgviews

GNU_CONFIGURE=          YES

DEPENDS+=       libmp3splt-nbsd-${LIBMP3SPLT_VERSION}:../../audio/libmp3splt

OBJMACHINE=     YES
DOC_DIR=\${PREFIX}/share/doc/mp3splt/

#copy documentation
pre-install:
        \${INSTALL_DATA_DIR} \${DOC_DIR}" > Makefile

for doc in "${MP3SPLT_DOC_FILES[@]}";do
    echo "	\${INSTALL_DATA} \${WRKSRC}/${doc} \${DOC_DIR}" >> Makefile
done

echo "
.include \"../../audio/libmp3splt/buildlink3.mk\"

.include \"../../mk/bsd.pkg.mk\"" >> Makefile

#we generate the PLIST
echo "@comment $NetBSD$" > PLIST

for file in "${MP3SPLT_FILES[@]}";do
    echo "$file" >> PLIST
done

for file in "${MP3SPLT_MAN1_FILES[@]}";do
    echo "man/man1/$file" >> PLIST
done

for doc in "${MP3SPLT_DOC_FILES[@]}";do
    echo "share/doc/mp3splt/${doc}" >> PLIST
done

echo "@dirrm share/doc/mp3splt" >> PLIST
