/*
 * External prototypes for rdc_mode.c
 */
#ifndef _rdc_mode_h_
#define _rdc_mode_h_
extern Bool BTranslateIndexToRefreshRate(UCHAR ucRRateIndex, float *fRefreshRate);
extern Bool RDCSetMode(ScrnInfoPtr pScrn, DisplayModePtr mode);
extern char* pcConvertResolutionToString(ULONG ulResolution);
extern DisplayModePtr RDCBuildModePool(ScrnInfoPtr pScrn);
extern DisplayModePtr SearchDisplayModeRecPtr(DisplayModePtr pModePoolHead, CBIOS_ARGUMENTS *pCBiosArguments);
extern USHORT usGetVbeModeNum(ScrnInfoPtr pScrn, DisplayModePtr mode);
#endif
/*
 * Static prototypes for rdc_mode.c
 */
#ifdef _rdc_mode_c_
#endif


#undef RDC_EXPORT
#define RDC_EXPORT
#undef RDC_STATIC
#define RDC_STATIC static
