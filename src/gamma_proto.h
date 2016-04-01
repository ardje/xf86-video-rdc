/*
 * Prototypes for gamma.c
 */
extern BOOL CompGamma(PVIDEO_CLUT ClutBuffer);
extern int VgaSetColorLookup(ScrnInfoPtr pScrn, PVIDEO_CLUT ClutBuffer, ULONG ClutBufferSize);
extern void EnableGamma(ScrnInfoPtr pScrn, BOOL Enable);
extern void SaveGammaTable(ScrnInfoPtr pScrn, PVIDEO_CLUT ClutBuffer);

#undef RDC_EXPORT
#define RDC_EXPORT
