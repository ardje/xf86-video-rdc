/*
 * Prototypes for rdc_mode.c
 */
extern Bool BTranslateIndexToRefreshRate(UCHAR ucRRateIndex, float *fRefreshRate);
extern Bool RDCSetMode(ScrnInfoPtr pScrn, DisplayModePtr mode);
extern char* pcConvertResolutionToString(ULONG ulResolution);
extern DisplayModePtr RDCBuildModePool(ScrnInfoPtr pScrn);
extern DisplayModePtr SearchDisplayModeRecPtr(DisplayModePtr pModePoolHead, CBIOS_ARGUMENTS *pCBiosArguments);
extern USHORT usGetVbeModeNum(ScrnInfoPtr pScrn, DisplayModePtr mode);

#undef RDC_EXPORT
#define RDC_EXPORT
