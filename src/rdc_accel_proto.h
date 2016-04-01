/*
 * External prototypes for rdc_accel.c
 */
#ifndef _rdc_accel_h_
#define _rdc_accel_h_
extern Bool RDCAccelInit(ScreenPtr pScreen);
extern Bool RDCExaDownloadFromScreen (PixmapPtr pSrc, int x,  int y, int w,  int h, char *dst,  int dst_pitch);
extern Bool RDCExaUploadToScreen ( PixmapPtr   pDst, int x, int y, int w, int h, char *src, int src_pitch);
extern int RDCAccelMarkSync(ScreenPtr pScreen);
extern void RDCAccelWaitMarker(ScreenPtr pScreen, int marker);
#endif
/*
 * Static prototypes for rdc_accel.c
 */
#ifdef _rdc_accel_c_
RDC_STATIC Bool RDCExaPrepareCopy( PixmapPtr pSrcPixmap, PixmapPtr pDstPixmap, int xdir, int ydir, int alu, Pixel planeMask);
RDC_STATIC Bool RDCExaPrepareSolid(PixmapPtr pPixmap, int alu, Pixel planeMask, Pixel fg);
RDC_STATIC ExaDriverPtr RDCInitExa(ScreenPtr pScreen);
RDC_STATIC static void RDCSetupForSolidLine(ScrnInfoPtr pScrn, int color, int rop, unsigned int planemask);
RDC_STATIC void RDCDisableClipping(ScrnInfoPtr pScrn);
RDC_STATIC void RDCExaCopy( PixmapPtr pDstPixmap, int srcX, int srcY, int dstX, int dstY, int width, int height);
RDC_STATIC void RDCExaDoneCopy(PixmapPtr pPixmap);
RDC_STATIC void RDCExaDoneSolidCopy(PixmapPtr pPixmap);
RDC_STATIC void RDCExaSolid(PixmapPtr pPixmap, int x1, int y1, int x2, int y2);
RDC_STATIC void RDCSetClippingRectangle(ScrnInfoPtr pScrn, int left, int top, int right, int bottom);
RDC_STATIC void RDCSetupForColor8x8PatternFill(ScrnInfoPtr pScrn, int patx, int paty, int rop, unsigned int planemask, int trans_col);
RDC_STATIC void RDCSetupForCPUToScreenColorExpandFill(ScrnInfoPtr pScrn, int fg, int bg, int rop, unsigned int planemask);
RDC_STATIC void RDCSetupForDashedLine(ScrnInfoPtr pScrn, int fg, int bg, int rop, unsigned int planemask, int length, UCHAR *pattern);
RDC_STATIC void RDCSetupForMonoPatternFill(ScrnInfoPtr pScrn, int patx, int paty, int fg, int bg, int rop, unsigned int planemask);
RDC_STATIC void RDCSetupForScreenToScreenColorExpandFill(ScrnInfoPtr pScrn, int fg, int bg, int rop, unsigned int planemask);
RDC_STATIC void RDCSetupForScreenToScreenCopy(ScrnInfoPtr pScrn, int xdir, int ydir, int rop, unsigned int planemask, int trans_color);
RDC_STATIC void RDCSetupForSolidFill(ScrnInfoPtr pScrn, int color, int rop, unsigned int planemask);
RDC_STATIC void RDCSubsequentColor8x8PatternFillRect(ScrnInfoPtr pScrn, int patx, int paty, int dst_x, int dst_y, int width, int height);
RDC_STATIC void RDCSubsequentCPUToScreenColorExpandFill(ScrnInfoPtr pScrn, int dst_x, int dst_y, int width, int height, int offset);
RDC_STATIC void RDCSubsequentDashedTwoPointLine(ScrnInfoPtr pScrn, int x1, int y1, int x2, int y2, int flags, int phase);
RDC_STATIC void RDCSubsequentMonoPatternFill(ScrnInfoPtr pScrn, int patx, int paty, int dst_x, int dst_y, int width, int height);
RDC_STATIC void RDCSubsequentScreenToScreenColorExpandFill(ScrnInfoPtr pScrn, int dst_x, int dst_y, int width, int height, int src_x, int src_y, int offset);
RDC_STATIC void RDCSubsequentScreenToScreenCopy(ScrnInfoPtr pScrn, int x1, int y1, int x2, int y2, int width, int height);
RDC_STATIC void RDCSubsequentSolidFillRect(ScrnInfoPtr pScrn, int dst_x, int dst_y, int width, int height);
RDC_STATIC void RDCSubsequentSolidHorVertLine(ScrnInfoPtr pScrn, int x, int y, int len, int dir);
RDC_STATIC void RDCSubsequentSolidTwoPointLine(ScrnInfoPtr pScrn, int x1, int y1, int x2, int y2, int flags);
RDC_STATIC void RDCSync(ScrnInfoPtr pScrn);
#endif


#undef RDC_EXPORT
#define RDC_EXPORT
#undef RDC_STATIC
#define RDC_STATIC static
