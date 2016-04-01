/*
 * External prototypes for gamma.c
 */
#ifndef _gamma_h_
#define _gamma_h_
extern BOOL CompGamma(PVIDEO_CLUT ClutBuffer);
extern int VgaSetColorLookup(ScrnInfoPtr pScrn, PVIDEO_CLUT ClutBuffer, ULONG ClutBufferSize);
extern void EnableGamma(ScrnInfoPtr pScrn, BOOL Enable);
extern void SaveGammaTable(ScrnInfoPtr pScrn, PVIDEO_CLUT ClutBuffer);
#endif
/*
 * Static prototypes for gamma.c
 */
#ifdef _gamma_c_
#endif


#undef RDC_EXPORT
#define RDC_EXPORT
#undef RDC_STATIC
#define RDC_STATIC static
