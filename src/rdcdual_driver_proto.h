/*
 * External prototypes for rdcdual_driver.c
 */
#ifndef _rdcdual_driver_h_
#define _rdcdual_driver_h_
extern Bool RDCCursorInitDual(ScreenPtr pScreen);
extern Bool RDCSwitchModeDual(ScrnInfoPtr pScrn, DisplayModePtr mode);
extern void RDCAdjustFrameDual(ScrnInfoPtr pScrn, int x, int y);
extern void RDCInitpScrnDual(ScrnInfoPtr pScrn);
#endif
/*
 * Static prototypes for rdcdual_driver.c
 */
#ifdef _rdcdual_driver_c_
static Bool RDCCreateScreenResources(ScreenPtr pScreen);
static Bool RDCEnterVTDual(ScrnInfoPtr pScrn);
static Bool RDCPreInitDual(ScrnInfoPtr pScrn, int flags);
static Bool RDCScreenInitDual(ScreenPtr pScreen, int argc, char **argv);
static Bool rdc_user_modesetting_init(ScrnInfoPtr pScrn);
static Bool rdc_xf86crtc_resize(ScrnInfoPtr scrn, int width, int height);
static int rdc_output_clones (ScrnInfoPtr pScrn, int type_mask);
static ModeStatus RDCValidModeDual(ScrnInfoPtr pScrn, DisplayModePtr mode, Bool verbose, int flags);
static void RDCBlockHandler(ScreenPtr pScreen, pointer pTimeout, pointer pReadmask);
static void RDCFreeScreenDual(ScrnInfoPtr pScrn);
static void RDCLeaveVTDual(ScrnInfoPtr pScrn);
static void RDCPreInitCrtcConfig(ScrnInfoPtr pScrn);
static void RDCSetupOutputs(ScrnInfoPtr pScrn);
#endif


#undef RDC_EXPORT
#define RDC_EXPORT
#undef RDC_STATIC
#define RDC_STATIC static
