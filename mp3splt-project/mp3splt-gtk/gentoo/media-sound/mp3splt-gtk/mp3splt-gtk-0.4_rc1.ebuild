# Copyright 1999-2006 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2
# $Header:  $

# THIS FILE IS NOT SUPPORTED BY GENTOO, CAN HARM AND BREAK YOUR SYSTEM FOR GOOD!
# THERE IS NO GUARANTEE OR WARRANTY OF ANY KIND USE IT ON YOUR OWN RISK!
#
# e-build by Miklos Bagi Jr. <mbjr@mbjr.hu>
# modified by Munteanu Alexandru Ionut <io_alex_2002@yahoo.fr>

DESCRIPTION="Mp3splt-gtk is a GTK2 gui that uses libmp3splt to split mp3 and ogg files."
HOMEPAGE="http://mp3splt.sourceforge.net/"
SRC_URI="mirror://sourceforge/mp3splt/${P}.tar.gz"

LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~x86 ~amd64"
#audacious should be enabled in the future sessions
#IUSE="audacious"
#audacious? ( >=media-sound/audacious-1.1.0 )
IUSE=""

DEPEND="=media-libs/libmp3splt-0.4_rc1
        >=x11-libs/gtk+-2"

src_compile() {
#if we use audacious, enable audacious support
#    if use audacious; then
#        econf --enable-bmp || die "configuration failed"
#    else
    econf || die "configuration failed"
#    fi;
	emake || die "compilation failed"
}

src_install() {
	einstall
    dodoc AUTHORS ChangeLog COPYING INSTALL NEWS README TODO
}
