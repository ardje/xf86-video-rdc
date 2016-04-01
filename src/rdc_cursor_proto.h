/*
 * Prototypes for rdc_cursor.c
 */
extern Bool bInitHWC(ScrnInfoPtr pScrn, RDCRecPtr pRDC);
extern Bool RDCCursorInit(ScreenPtr pScreen);
extern void RDCHideCursor_HQ(ScrnInfoPtr pScrn);
extern void RDCHideCursor(ScrnInfoPtr pScrn);

#undef RDC_EXPORT
#define RDC_EXPORT
