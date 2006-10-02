# $NetBSD$

DISTNAME=       mp3splt-2.2_rc1
PKGNAME=        mp3splt-nbsd-2.2_rc1
CATEGORIES=     audio
MASTER_SITES=   ${MASTER_SITE_SOURCEFORGE:=mp3splt/}

MAINTAINER=     io_alex_2002@yahoo.fr
HOMEPAGE=       http://mp3splt.sourceforge.net/
COMMENT=        Command line program to split mp3 and ogg without
decoding

PKG_INSTALLATION_TYPES= overwrite pkgviews

GNU_CONFIGURE=          YES

DEPENDS+=       libmp3splt-nbsd-0.4_rc1:../../audio/libmp3splt

OBJMACHINE=     YES
DOC_DIR=${PREFIX}/share/doc/mp3splt/

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

.include "../../mk/bsd.pkg.mk"
