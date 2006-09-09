# New ports collection makefile for: mp3splt-gtk
# Date created:	the date
# Whom:		Munteanu Alexandru Ionut
#
# $FreeBSD$
#

PORTNAME=	mp3splt-gtk
PORTVERSION=	0.4.1
CATEGORIES=	audio
MASTER_SITES=	${MASTER_SITE_SOURCEFORGE:=mp3splt/}
MASTER_SITE_SUBDIR=	mp3splt
PKGNAMESUFFIX=	-fbsd
DISTNAME=	${PORTNAME}-0.4_rc1

MAINTAINER=	io_alex_2002@yahoo.fr
COMMENT= 	Command line program to split mp3 and ogg without decoding

BUILD_DEPENDS=	libmp3splt:${PORTSDIR}/audio/libmp3splt
USE_GNOME=	gtk20
LIB_DEPENDS=	glib-2.0:${PORTSDIR}/devel/glib20 \
		beep.2:${PORTSDIR}/multimedia/beep-media-player

GNU_CONFIGURE=	YES
USE_GMAKE=	YES
CONFIGURE_ARGS+=--enable-bmp
CONFIGURE_ENV+=	LDFLAGS="${LDFLAGS} -L${LOCALBASE}/lib" \
		CFLAGS="${CFLAGS} -I${LOCALBASE}/include"

WRKSRC=		${WRKDIR}/mp3splt-gtk-0.4_rc1

#copy documentation
DOC_DIR=	${PREFIX}/share/doc/mp3splt-gtk/

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
