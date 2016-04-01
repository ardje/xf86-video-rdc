/*
 * Prototype for CInt10.c
 */

/*
 * Prototype for HDMI.c
 */

/*
 * Prototype for TV.c
 */

/*
 * Prototype for gamma.c
 */

/*
 * Prototype for rdc_2dtool.c
 */
extern Bool bCREnable2D(RDCRecPtr pRDC);
extern Bool bCREnableCMDQ(RDCRecPtr pRDC);
extern Bool bCRInitCMDQInfo(ScrnInfoPtr pScrn, RDCRecPtr pRDC);
extern Bool bEnable2D(RDCRecPtr pRDC);
extern Bool bEnableCMDQ(RDCRecPtr pRDC);
extern Bool bInitCMDQInfo(ScrnInfoPtr pScrn, RDCRecPtr pRDC);
extern int FireCRCMDQ(int iDev, unsigned int *pCmd, unsigned int size);
extern UCHAR *pjRequestCMDQ( RDCRecPtr pRDC, ULONG   ulDataLen);
extern unsigned int GetFBIFCaps(int iDev);
extern void vCRDisable2D(RDCRecPtr pRDC);
extern void vCRWaitEngIdle(RDCRecPtr pRDC);
extern void vDisable2D(RDCRecPtr pRDC);
extern void vWaitEngIdle(RDCRecPtr pRDC);

/*
 * Prototype for rdc_accel.c
 */

/*
 * Prototype for rdc_cursor.c
 */

/*
 * Prototype for rdc_driver.c
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

/*
 * Prototype for rdc_extension.c
 */

/*
 * Prototype for rdc_mode.c
 */

/*
 * Prototype for rdc_tool.c
 */

/*
 * Prototype for rdc_vgatool.c
 */

/*
 * Prototype for rdc_video.c
 */

/*
 * Prototype for rdcdual_display.c
 */

/*
 * Prototype for rdcdual_driver.c
 */

/*
 * Prototype for vidinit.c
 */

#define RDC_EXPORT
