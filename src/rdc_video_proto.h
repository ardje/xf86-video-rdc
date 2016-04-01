/*
 * Prototypes for rdc_video.c
 */
extern UpdateOverlay(RDCRecPtr pRDC, VIDHWINFOPtr pVIDHwInfo, int iSrcBufIndex);
extern void RDCAllocateVPOSTMem(ScrnInfoPtr pScrn, RDCPortPrivPtr pRDCPortPriv, long width, long height, Bool bRDC_Video);
extern void RDCCopyFOURCC(RDCRecPtr  pRDC, unsigned char* pSrcStart, RDCPortPrivPtr pRDCPortPriv, unsigned long SrcPitch, unsigned long DstPitch, int FourCC, short width, short height);
extern void RDCCopyFourCCVPOST(RDCRecPtr  pRDC, RDCPortPrivPtr pRDCPortPriv, unsigned char* pSrc, long Width, long Height, Bool bRDC_Video);
extern void RDCUpdateVideo(RDCRecPtr pRDC, RDCPortPrivPtr pRDCPortPriv, BYTE bEnable, Bool bRDC_Video);
extern void RDCUpdateVID(RDCRecPtr  pRDC, RDCPortPrivPtr pRDCPortPriv, BYTE bEnable);
extern void RDCVideoInit(ScreenPtr pScreen);
extern void UpdateVPost(RDCRecPtr pRDC, VPOSTHWINFOPtr pVPHwInfo);

#undef RDC_EXPORT
#define RDC_EXPORT
