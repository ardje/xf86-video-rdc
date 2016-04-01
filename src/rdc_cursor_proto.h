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
static Bool RDCUseHWCursorARGB(ScreenPtr pScreen, CursorPtr pCurs);
static Bool RDCUseHWCursor(ScreenPtr pScreen, CursorPtr pCurs);
static void RDCFireCursor(ScrnInfoPtr pScrn);
static void RDCLoadCursorARGB_HQ(ScrnInfoPtr pScrn, CursorPtr pCurs);
static void RDCLoadCursorARGB(ScrnInfoPtr pScrn, CursorPtr pCurs);
static void RDCLoadCursorImage_HQ(ScrnInfoPtr pScrn, UCHAR *src);
static void RDCLoadCursorImage(ScrnInfoPtr pScrn, UCHAR *src);
static void RDCSetCursorColors_HQ(ScrnInfoPtr pScrn, int bg, int fg);
static void RDCSetCursorColors(ScrnInfoPtr pScrn, int bg, int fg);
static void RDCSetCursorPosition_HQ(ScrnInfoPtr pScrn, int x, int y);
static void RDCSetCursorPosition(ScrnInfoPtr pScrn, int x, int y);
static void RDCShowCursor_HQ(ScrnInfoPtr pScrn);
static void RDCShowCursor(ScrnInfoPtr pScrn);
#endif


#undef RDC_EXPORT
#define RDC_EXPORT
#undef RDC_STATIC
#define RDC_STATIC static
