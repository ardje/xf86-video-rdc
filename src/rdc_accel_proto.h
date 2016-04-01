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
static Bool RDCExaPrepareCopy( PixmapPtr pSrcPixmap, PixmapPtr pDstPixmap, int xdir, int ydir, int alu, Pixel planeMask);
static Bool RDCExaPrepareSolid(PixmapPtr pPixmap, int alu, Pixel planeMask, Pixel fg);
static ExaDriverPtr RDCInitExa(ScreenPtr pScreen);
static void RDCDisableClipping(ScrnInfoPtr pScrn);
static void RDCExaCopy( PixmapPtr pDstPixmap, int srcX, int srcY, int dstX, int dstY, int width, int height);
static void RDCExaDoneCopy(PixmapPtr pPixmap);
static void RDCExaDoneSolidCopy(PixmapPtr pPixmap);
static void RDCExaSolid(PixmapPtr pPixmap, int x1, int y1, int x2, int y2);
static void RDCSetClippingRectangle(ScrnInfoPtr pScrn, int left, int top, int right, int bottom);
static void RDCSetupForColor8x8PatternFill(ScrnInfoPtr pScrn, int patx, int paty, int rop, unsigned int planemask, int trans_col);
static void RDCSetupForCPUToScreenColorExpandFill(ScrnInfoPtr pScrn, int fg, int bg, int rop, unsigned int planemask);
static void RDCSetupForDashedLine(ScrnInfoPtr pScrn, int fg, int bg, int rop, unsigned int planemask, int length, UCHAR *pattern);
static void RDCSetupForMonoPatternFill(ScrnInfoPtr pScrn, int patx, int paty, int fg, int bg, int rop, unsigned int planemask);
static void RDCSetupForScreenToScreenColorExpandFill(ScrnInfoPtr pScrn, int fg, int bg, int rop, unsigned int planemask);
static void RDCSetupForScreenToScreenCopy(ScrnInfoPtr pScrn, int xdir, int ydir, int rop, unsigned int planemask, int trans_color);
static void RDCSetupForSolidFill(ScrnInfoPtr pScrn, int color, int rop, unsigned int planemask);
static void RDCSetupForSolidLine(ScrnInfoPtr pScrn, int color, int rop, unsigned int planemask);
static void RDCSubsequentColor8x8PatternFillRect(ScrnInfoPtr pScrn, int patx, int paty, int dst_x, int dst_y, int width, int height);
static void RDCSubsequentCPUToScreenColorExpandFill(ScrnInfoPtr pScrn, int dst_x, int dst_y, int width, int height, int offset);
static void RDCSubsequentDashedTwoPointLine(ScrnInfoPtr pScrn, int x1, int y1, int x2, int y2, int flags, int phase);
static void RDCSubsequentMonoPatternFill(ScrnInfoPtr pScrn, int patx, int paty, int dst_x, int dst_y, int width, int height);
static void RDCSubsequentScreenToScreenColorExpandFill(ScrnInfoPtr pScrn, int dst_x, int dst_y, int width, int height, int src_x, int src_y, int offset);
static void RDCSubsequentScreenToScreenCopy(ScrnInfoPtr pScrn, int x1, int y1, int x2, int y2, int width, int height);
static void RDCSubsequentSolidFillRect(ScrnInfoPtr pScrn, int dst_x, int dst_y, int width, int height);
static void RDCSubsequentSolidHorVertLine(ScrnInfoPtr pScrn, int x, int y, int len, int dir);
static void RDCSubsequentSolidTwoPointLine(ScrnInfoPtr pScrn, int x1, int y1, int x2, int y2, int flags);
static void RDCSync(ScrnInfoPtr pScrn);
#endif


#undef RDC_EXPORT
#define RDC_EXPORT
#undef RDC_STATIC
#define RDC_STATIC static
