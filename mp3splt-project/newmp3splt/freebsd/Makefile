# New ports collection makefile for: mp3splt
# Date created:	the date
# Whom:		Munteanu Alexandru Ionut
#
# $FreeBSD$
#

PORTNAME=	mp3splt
PORTVERSION=	2.2.1
CATEGORIES=	audio
MASTER_SITES=	${MASTER_SITE_SOURCEFORGE:=mp3splt/}
MASTER_SITE_SUBDIR=	mp3splt
PKGNAMESUFFIX=	-fbsd
DISTNAME=	${PORTNAME}-2.2_rc1

MAINTAINER=	io_alex_2002@yahoo.fr
COMMENT= 	Command line program to split mp3 and ogg without decoding

LIB_DEPENDS=	libmp3splt.0:${PORTSDIR}/audio/libmp3splt

GNU_CONFIGURE=	YES
USE_GMAKE=	YES
CONFIGURE_ENV+=	LDFLAGS="${LDFLAGS} -L${LOCALBASE}/lib" \
		CFLAGS="${CFLAGS} -I${LOCALBASE}/include"

WRKSRC=		${WRKDIR}/mp3splt-0.4_rc1

#copy documentation
DOC_DIR=	${PREFIX}/share/doc/mp3splt/

pre-install:
	${MKDIR} ${DOC_DIR}
	${INSTALL_DATA} ${WRKSRC}/AUTHORS ${DOC_DIR}
	${INSTALL_DATA} ${WRKSRC}/ChangeLog ${DOC_DIR}
	${INSTALL_DATA} ${WRKSRC}/COPYING ${DOC_DIR}
	${INSTALL_DATA} ${WRKSRC}/INSTALL ${DOC_DIR}
	${INSTALL_DATA} ${WRKSRC}/NEWS ${DOC_DIR}
	${INSTALL_DATA} ${WRKSRC}/README ${DOC_DIR}
	${INSTALL_DATA} ${WRKSRC}/TODO ${DOC_DIR}

.include <bsd.port.mk>
