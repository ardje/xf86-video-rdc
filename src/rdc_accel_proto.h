/*
 * Prototypes for rdc_accel.c
 */
extern Bool RDCAccelInit(ScreenPtr pScreen);
extern Bool RDCExaDownloadFromScreen (PixmapPtr pSrc, int x,  int y, int w,  int h, char *dst,  int dst_pitch);
extern Bool RDCExaUploadToScreen ( PixmapPtr   pDst, int x, int y, int w, int h, char *src, int src_pitch);
extern int RDCAccelMarkSync(ScreenPtr pScreen);
extern void RDCAccelWaitMarker(ScreenPtr pScreen, int marker);

#undef RDC_EXPORT
#define RDC_EXPORT
