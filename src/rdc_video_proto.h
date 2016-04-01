/*
 * External prototypes for rdc_video.c
 */
#ifndef _rdc_video_h_
#define _rdc_video_h_
extern UpdateOverlay(RDCRecPtr pRDC, VIDHWINFOPtr pVIDHwInfo, int iSrcBufIndex);
extern void RDCAllocateVPOSTMem(ScrnInfoPtr pScrn, RDCPortPrivPtr pRDCPortPriv, long width, long height, Bool bRDC_Video);
extern void RDCCopyFOURCC(RDCRecPtr  pRDC, unsigned char* pSrcStart, RDCPortPrivPtr pRDCPortPriv, unsigned long SrcPitch, unsigned long DstPitch, int FourCC, short width, short height);
extern void RDCCopyFourCCVPOST(RDCRecPtr  pRDC, RDCPortPrivPtr pRDCPortPriv, unsigned char* pSrc, long Width, long Height, Bool bRDC_Video);
extern void RDCUpdateVideo(RDCRecPtr pRDC, RDCPortPrivPtr pRDCPortPriv, BYTE bEnable, Bool bRDC_Video);
extern void RDCUpdateVID(RDCRecPtr  pRDC, RDCPortPrivPtr pRDCPortPriv, BYTE bEnable);
extern void RDCVideoInit(ScreenPtr pScreen);
extern void UpdateVPost(RDCRecPtr pRDC, VPOSTHWINFOPtr pVPHwInfo);
#endif
/*
 * Static prototypes for rdc_video.c
 */
#ifdef _rdc_video_c_
static FBLinearPtr RDCAllocateMemory( ScrnInfoPtr pScrn, FBLinearPtr PackedBuf, int size);
static int RDCGetPortAttribute( ScrnInfoPtr pScrn, Atom attribute, INT32 * value, pointer data);
static int RDCPutImage(ScrnInfoPtr pScrn, short src_x, short src_y, short drw_x, short drw_y, short src_w, short src_h, short drw_w, short drw_h, int id, unsigned char *buf, short width, short height, Bool sync, RegionPtr clipBoxes, pointer data, DrawablePtr pDraw);
static int RDCPutImageVPOST(ScrnInfoPtr pScrn, short src_x, short src_y, short drw_x, short drw_y, short src_w, short src_h, short drw_w, short drw_h, int id, unsigned char *buf, short width, short height, Bool sync, RegionPtr clipBoxes, pointer data, DrawablePtr pDraw);
static int RDCQueryImageAttributesOverlay(ScrnInfoPtr pScrn,int id, unsigned short *w, unsigned short *h, int *pitches, int *offsets);
static int RDCSetPortAttribute( ScrnInfoPtr pScrn, Atom attribute, INT32 value, pointer data);
static void RDCQueryBestSize(ScrnInfoPtr pScrn, Bool motion, short vid_w, short vid_h, short drw_w, short drw_h, unsigned int *p_w, unsigned int *p_h, pointer data);
static void RDCStopVideoPost(ScrnInfoPtr pScrn, pointer data, Bool shutdown);
static void RDCStopVideo(ScrnInfoPtr pScrn, pointer data, Bool shutdown);
static XF86VideoAdaptorPtr RDCSetupImageVideoOverlay(ScreenPtr pScreen);
#endif


#undef RDC_EXPORT
#define RDC_EXPORT
#undef RDC_STATIC
#define RDC_STATIC static
