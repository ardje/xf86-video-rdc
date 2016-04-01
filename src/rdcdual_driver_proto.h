/*
 * External prototypes for rdcdual_driver.c
 */
#ifndef _rdcdual_driver_h_
#define _rdcdual_driver_h_
extern Bool RDCSwitchModeDual(int scrnIndex, DisplayModePtr mode, int flags);
extern void RDCAdjustFrameDual(int scrnIndex, int x, int y, int flags);
extern void RDCInitpScrnDual(ScrnInfoPtr pScrn);
#endif
/*
 * Static prototypes for rdcdual_driver.c
 */
#ifdef _rdcdual_driver_c_
RDC_STATIC Bool RDCCreateScreenResources(ScreenPtr pScreen);
RDC_STATIC Bool RDCEnterVTDual(int scrnIndex, int flags);
RDC_STATIC Bool RDCPreInitDual(ScrnInfoPtr pScrn, int flags);
RDC_STATIC Bool RDCScreenInitDual(int scrnIndex, ScreenPtr pScreen, int argc, char **argv);
RDC_STATIC Bool rdc_user_modesetting_init(ScrnInfoPtr pScrn);
RDC_STATIC Bool rdc_xf86crtc_resize(ScrnInfoPtr scrn, int width, int height);
RDC_STATIC int rdc_output_clones (ScrnInfoPtr pScrn, int type_mask);
RDC_STATIC ModeStatus RDCValidModeDual(int scrnIndex, DisplayModePtr mode, Bool verbose, int flags);
RDC_STATIC void RDCBlockHandler(int i, pointer blockData, pointer pTimeout, pointer pReadmask);
RDC_STATIC void RDCFreeScreenDual(int scrnIndex, int flags);
RDC_STATIC void RDCLeaveVTDual(int scrnIndex, int flags);
RDC_STATIC void RDCPreInitCrtcConfig(ScrnInfoPtr pScrn);
RDC_STATIC void RDCSetupOutputs(ScrnInfoPtr pScrn);
#endif


#undef RDC_EXPORT
#define RDC_EXPORT
#undef RDC_STATIC
#define RDC_STATIC static
