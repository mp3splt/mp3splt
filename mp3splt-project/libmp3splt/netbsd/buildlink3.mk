# $NetBSD$

BUILDLINK_DEPTH:=		${BUILDLINK_DEPTH}+
LIBMP3SPLT_BUILDLINK3_MK:=	${LIBMP3SPLT_BUILDLINK3_MK}+

.if ${BUILDLINK_DEPTH} == "+"
BUILDLINK_DEPENDS+=	libmp3splt
.endif

BUILDLINK_PACKAGES:=	${BUILDLINK_PACKAGES:Nlibmp3splt}
BUILDLINK_PACKAGES+=	libmp3splt

.if ${LIBMP3SPLT_BUILDLINK3_MK} == "+"
BUILDLINK_API_DEPENDS.libmp3splt+=	libmp3splt-nbsd-0.4_rc1
BUILDLINK_PKGSRCDIR.libmp3splt?=	../../audio/libmp3splt
.endif

.include "../../audio/libmad/buildlink3.mk"
.include "../../multimedia/libogg/buildlink3.mk"
.include "../../audio/libvorbis/buildlink3.mk"
.include "../../audio/libid3tag/buildlink3.mk"

BUILDLINK_DEPTH:=		${BUILDLINK_DEPTH:S/+$//}
