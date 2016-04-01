/*
 * External prototypes for rdc_driver.c
 */
#ifndef _rdc_driver_h_
#define _rdc_driver_h_
extern Bool RDCCloseScreen(ScreenPtr pScreen);
extern Bool RDCDriverFunc(ScrnInfoPtr pScrn, xorgDriverFuncOp op, pointer data);
extern Bool RDCGetRec(ScrnInfoPtr pScrn);
extern Bool RDCModeInit(ScrnInfoPtr pScrn, DisplayModePtr mode);
extern Bool RDCRandRGetInfo(ScrnInfoPtr pScrn, Rotation *rotations);
extern Bool RDCRandRSetConfig(ScrnInfoPtr pScrn, xorgRRConfig *config);
extern Bool RDCSaveScreen(ScreenPtr pScreen, Bool unblack);
extern Bool RDCSwitchMode(ScrnInfoPtr pScrn, DisplayModePtr mode);
extern const OptionInfoRec * RDCAvailableOptions(int chipid, int busid);
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
#endif
/*
 * Static prototypes for rdc_driver.c
 */
#ifdef _rdc_driver_c_
RDC_STATIC Bool RDCEnterVT(ScrnInfoPtr pScrn);
RDC_STATIC Bool rdc_pci_probe (DriverPtr		driver, int		        entity_num, struct pci_device	*device, intptr_t		match_data);
RDC_STATIC Bool RDCPreInit(ScrnInfoPtr pScrn, int flags);
RDC_STATIC Bool RDCProbe(DriverPtr drv, int flags);
RDC_STATIC Bool RDCScreenInit(ScreenPtr pScreen, int argc, char **argv);
RDC_STATIC ModeStatus RDCValidMode(ScrnInfoPtr pScrn, DisplayModePtr mode, Bool verbose, int flags);
RDC_STATIC pointer RDCSetup(pointer module, pointer opts, int *errmaj, int *errmin);
RDC_STATIC void RDCFreeScreen(ScrnInfoPtr pScrn);
RDC_STATIC void RDCIdentify(int flags);
RDC_STATIC void RDCLeaveVT(ScrnInfoPtr pScrn);
#endif


#undef RDC_EXPORT
#define RDC_EXPORT
#undef RDC_STATIC
#define RDC_STATIC static
