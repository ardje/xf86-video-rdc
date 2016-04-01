/*
 * External prototypes for rdc_cursor.c
 */
#ifndef _rdc_cursor_h_
#define _rdc_cursor_h_
extern Bool bInitHWC(ScrnInfoPtr pScrn, RDCRecPtr pRDC);
extern Bool RDCCursorInit(ScreenPtr pScreen);
extern void RDCHideCursor_HQ(ScrnInfoPtr pScrn);
extern void RDCHideCursor(ScrnInfoPtr pScrn);
#endif
/*
 * Static prototypes for rdc_cursor.c
 */
#ifdef _rdc_cursor_c_
RDC_STATIC Bool RDCUseHWCursorARGB(ScreenPtr pScreen, CursorPtr pCurs);
RDC_STATIC Bool RDCUseHWCursor(ScreenPtr pScreen, CursorPtr pCurs);
RDC_STATIC void RDCFireCursor(ScrnInfoPtr pScrn);
RDC_STATIC void RDCLoadCursorARGB_HQ(ScrnInfoPtr pScrn, CursorPtr pCurs);
RDC_STATIC void RDCLoadCursorARGB(ScrnInfoPtr pScrn, CursorPtr pCurs);
RDC_STATIC void RDCLoadCursorImage_HQ(ScrnInfoPtr pScrn, UCHAR *src);
RDC_STATIC void RDCLoadCursorImage(ScrnInfoPtr pScrn, UCHAR *src);
RDC_STATIC void RDCSetCursorColors_HQ(ScrnInfoPtr pScrn, int bg, int fg);
RDC_STATIC void RDCSetCursorColors(ScrnInfoPtr pScrn, int bg, int fg);
RDC_STATIC void RDCSetCursorPosition_HQ(ScrnInfoPtr pScrn, int x, int y);
RDC_STATIC void RDCSetCursorPosition(ScrnInfoPtr pScrn, int x, int y);
RDC_STATIC void RDCShowCursor_HQ(ScrnInfoPtr pScrn);
RDC_STATIC void RDCShowCursor(ScrnInfoPtr pScrn);
#endif


#undef RDC_EXPORT
#define RDC_EXPORT
#undef RDC_STATIC
#define RDC_STATIC static
