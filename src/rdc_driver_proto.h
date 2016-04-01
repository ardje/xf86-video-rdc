/*
 * Prototypes for rdc_driver.c
 */
extern Bool RDCCloseScreen(ScreenPtr pScreen);
extern Bool RDCDriverFunc(ScrnInfoPtr pScrn, xorgDriverFuncOp op, pointer data);
extern Bool RDCGetRec(ScrnInfoPtr pScrn);
extern Bool RDCModeInit(ScrnInfoPtr pScrn, DisplayModePtr mode);
extern Bool RDCRandRGetInfo(ScrnInfoPtr pScrn, Rotation *rotations);
extern Bool RDCRandRSetConfig(ScrnInfoPtr pScrn, xorgRRConfig *config);
extern Bool RDCSaveScreen(ScreenPtr pScreen, Bool unblack);
extern Bool RDCSwitchMode(ScrnInfoPtr pScrn, DisplayModePtr mode);
extern const OptionInfoRec * RDCAvailableOptions(int chipid, int busid);
extern const OptionInfoRec *RDCAvailableOptions(int chipid, int busid);;
extern void RDCAdjustFrame(ScrnInfoPtr pScrn, int x, int y);
extern void RDCApertureInit(ScrnInfoPtr pScrn);
extern void RDCFreeRec(ScrnInfoPtr pScrn);
extern void RDCPointerMoved(ScrnInfoPtr pScrn, int x, int y);
extern void RDCProbeDDC(ScrnInfoPtr pScrn, int index);
extern void RDCRestore(ScrnInfoPtr pScrn);
extern void RDCSave(ScrnInfoPtr pScrn);
extern void RDCSetHWCaps(RDCRecPtr pRDC);
extern void TurnDirectAccessFBON(ScrnInfoPtr pScrn, Bool bTurnOn);
extern void vFillRDCModeInfo (ScrnInfoPtr pScrn);
extern void vUpdateHDMIFakeMode(ScrnInfoPtr pScrn);
extern xf86MonPtr RDCDoDDC(ScrnInfoPtr pScrn, int index);

#undef RDC_EXPORT
#define RDC_EXPORT
