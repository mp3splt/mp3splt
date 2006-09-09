# New ports collection makefile for: libmp3splt
# Date created:	the date
# Whom:		Munteanu Alexandru Ionut
#
# $FreeBSD$
#

PORTNAME=	libmp3splt
PORTVERSION=	0.4.1
CATEGORIES=	audio devel
MASTER_SITES=	${MASTER_SITE_SOURCEFORGE:=mp3splt/}
MASTER_SITE_SUBDIR=	mp3splt
PKGNAMESUFFIX=	-fbsd
DISTNAME=	${PORTNAME}-0.4_rc1

MAINTAINER=	io_alex_2002@yahoo.fr
COMMENT=	Library to split mp3 and ogg without decoding

LIB_DEPENDS=	mad.2:${PORTSDIR}/audio/libmad \
		vorbis.3:${PORTSDIR}/audio/libvorbis \
		ogg.5:${PORTSDIR}/audio/libogg \
		id3tag.0:${PORTSDIR}/audio/libid3tag

GNU_CONFIGURE=	YES
USE_GMAKE=	YES
CONFIGURE_ENV+=	LDFLAGS="${LDFLAGS} -L${LOCALBASE}/lib" \
		CFLAGS="${CFLAGS} -I${LOCALBASE}/include"

WRKSRC=		${WRKDIR}/libmp3splt-0.4_rc1

#copy documentation
DOC_DIR=	${PREFIX}/share/doc/libmp3splt/

pre-install:
	${MKDIR} ${DOC_DIR}
	${INSTALL_DATA} ${WRKSRC}/AUTHORS ${DOC_DIR}
	${INSTALL_DATA} ${WRKSRC}/ChangeLog ${DOC_DIR}
	${INSTALL_DATA} ${WRKSRC}/COPYING ${DOC_DIR}
	${INSTALL_DATA} ${WRKSRC}/INSTALL ${DOC_DIR}
	${INSTALL_DATA} ${WRKSRC}/NEWS ${DOC_DIR}
	${INSTALL_DATA} ${WRKSRC}/README ${DOC_DIR}
	${INSTALL_DATA} ${WRKSRC}/TODO ${DOC_DIR}
	${INSTALL_DATA} ${WRKSRC}/LIMITS ${DOC_DIR}

.include <bsd.port.mk>
