# $NetBSD$

DISTNAME= 	mp3splt-gtk-0.4_rc1
PKGNAME=	mp3splt-gtk-nbsd-0.4_rc1
CATEGORIES= 	audio
MASTER_SITES= 	${MASTER_SITE_SOURCEFORGE:=mp3splt/}

MAINTAINER= 	io_alex_2002@yahoo.fr
HOMEPAGE= 	http://mp3splt.sourceforge.net/
COMMENT= 	Libmp3splt GTK2 gui to split mp3 and ogg without decoding

PKG_INSTALLATION_TYPES= overwrite pkgviews

CONFIGURE_ARGS+=	--enable-bmp
GNU_CONFIGURE=		YES
USE_TOOLS+=		pkg-config

DEPENDS+=	libmp3splt-nbsd-0.4_rc1:../../audio/libmp3splt
DEPENDS+=	bmp-[0-9]*:../../audio/bmp
DEPENDS+=	gtk2+>=2.6:../../x11/gtk2
DEPENDS+=	glib2>=2.6:../../devel/glib2

OBJMACHINE=	YES
DOC_DIR=${PREFIX}/share/doc/mp3splt-gtk/

#copy documentation
pre-install:
	${INSTALL_DATA_DIR} ${DOC_DIR}
	${INSTALL_DATA} ${WRKSRC}/AUTHORS ${DOC_DIR}
	${INSTALL_DATA} ${WRKSRC}/ChangeLog ${DOC_DIR}
	${INSTALL_DATA} ${WRKSRC}/COPYING ${DOC_DIR}
	${INSTALL_DATA} ${WRKSRC}/INSTALL ${DOC_DIR}
	${INSTALL_DATA} ${WRKSRC}/NEWS ${DOC_DIR}
	${INSTALL_DATA} ${WRKSRC}/README ${DOC_DIR}
	${INSTALL_DATA} ${WRKSRC}/TODO ${DOC_DIR}

.include "../../audio/libmp3splt/buildlink3.mk"
.include "../../devel/glib2/buildlink3.mk"
.include "../../x11/gtk2/buildlink3.mk"
.include "../../audio/bmp/buildlink3.mk"

.include "../../mk/bsd.pkg.mk"
