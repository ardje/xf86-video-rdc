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
static Bool RDCCreateScreenResources(ScreenPtr pScreen);
static Bool RDCEnterVTDual(int scrnIndex, int flags);
static Bool RDCPreInitDual(ScrnInfoPtr pScrn, int flags);
static Bool RDCScreenInitDual(int scrnIndex, ScreenPtr pScreen, int argc, char **argv);
static Bool rdc_user_modesetting_init(ScrnInfoPtr pScrn);
static Bool rdc_xf86crtc_resize(ScrnInfoPtr scrn, int width, int height);
static int rdc_output_clones (ScrnInfoPtr pScrn, int type_mask);
static ModeStatus RDCValidModeDual(int scrnIndex, DisplayModePtr mode, Bool verbose, int flags);
static void RDCBlockHandler(int i, pointer blockData, pointer pTimeout, pointer pReadmask);
static void RDCFreeScreenDual(int scrnIndex, int flags);
static void RDCLeaveVTDual(int scrnIndex, int flags);
static void RDCPreInitCrtcConfig(ScrnInfoPtr pScrn);
static void RDCSetupOutputs(ScrnInfoPtr pScrn);
#endif


#undef RDC_EXPORT
#define RDC_EXPORT
#undef RDC_STATIC
#define RDC_STATIC static
