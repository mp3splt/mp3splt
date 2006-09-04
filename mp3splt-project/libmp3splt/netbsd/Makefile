# $NetBSD$

DISTNAME= 	libmp3splt-0.4_rc1
PKGNAME=	libmp3splt-nbsd-0.4_rc1
CATEGORIES= 	audio devel
MASTER_SITES= 	${MASTER_SITE_SOURCEFORGE:=mp3splt/}

MAINTAINER= 	io_alex_2002@yahoo.fr
HOMEPAGE= 	http://mp3splt.sourceforge.net/
COMMENT= 	Library to split mp3 and ogg without decoding

PKG_INSTALLATION_TYPES= overwrite pkgviews

GNU_CONFIGURE=		YES
USE_LIBTOOL=		YES

DEPENDS+=	libmad-[0-9]*:../../audio/libmad
DEPENDS+=	libogg-[0-9]*:../../multimedia/libogg
DEPENDS+=	libvorbis-[0-9]*:../../audio/libvorbis
DEPENDS+=	libid3tag-[0-9]*:../../audio/libid3tag

OBJMACHINE=	YES
DOC_DIR=${PREFIX}/share/doc/libmp3splt/

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
	${INSTALL_DATA} ${WRKSRC}/LIMITS ${DOC_DIR}

.include "../../audio/libmad/buildlink3.mk"
.include "../../multimedia/libogg/buildlink3.mk"
.include "../../audio/libvorbis/buildlink3.mk"
.include "../../audio/libid3tag/buildlink3.mk"

.include "../../mk/bsd.pkg.mk"
