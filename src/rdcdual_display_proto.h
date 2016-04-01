/*
 * External prototypes for rdcdual_display.c
 */
#ifndef _rdcdual_display_h_
#define _rdcdual_display_h_
extern Bool GetDisaplyStatus(ScrnInfoPtr pScrn);
extern void rdc_crtc_init(ScrnInfoPtr pScrn, int pipe);
extern void rdc_crt_init(ScrnInfoPtr pScrn);
extern void rdc_hdmi_init(pScrn);
extern void rdc_output_commit (xf86OutputPtr output);
extern void rdc_output_prepare (xf86OutputPtr output);
#endif
/*
 * Static prototypes for rdcdual_display.c
 */
#ifdef _rdcdual_display_c_
RDC_STATIC Bool rdc_crtc_lock (xf86CrtcPtr crtc);
RDC_STATIC Bool rdc_crtc_mode_fixup(xf86CrtcPtr crtc, DisplayModePtr mode, DisplayModePtr adjusted_mode);
RDC_STATIC Bool rdc_crt_mode_fixup(xf86OutputPtr output, DisplayModePtr mode, DisplayModePtr adjusted_mode);
RDC_STATIC Bool rdc_hdmi_mode_fixup(xf86OutputPtr output, DisplayModePtr mode, DisplayModePtr adjusted_mode);
RDC_STATIC DisplayModePtr rdc_crt_get_modes (xf86OutputPtr output);
RDC_STATIC DisplayModePtr rdc_hdmi_get_modes (xf86OutputPtr output);
RDC_STATIC int rdc_crt_mode_valid(xf86OutputPtr output, DisplayModePtr pMode);
RDC_STATIC int rdc_hdmi_mode_valid(xf86OutputPtr output, DisplayModePtr pMode);
RDC_STATIC void rdc_crtc_commit (xf86CrtcPtr crtc);
RDC_STATIC void rdc_crtc_dpms(xf86CrtcPtr crtc, int mode);
RDC_STATIC void rdc_crtc_gamma_set(xf86CrtcPtr crtc, CARD16 *red, CARD16 *green, CARD16 *blue, int size);
RDC_STATIC void rdc_crtc_mode_set(xf86CrtcPtr crtc, DisplayModePtr mode, DisplayModePtr adjusted_mode, int x, int y);
RDC_STATIC void rdc_crtc_prepare (xf86CrtcPtr crtc);
RDC_STATIC void rdc_crtc_set_origin(xf86CrtcPtr crtc, int x, int y);
RDC_STATIC void rdc_crtc_unlock (xf86CrtcPtr crtc);
RDC_STATIC void rdc_crt_destroy (xf86OutputPtr output);
RDC_STATIC void rdc_crt_dpms(xf86OutputPtr output, int mode);
RDC_STATIC void rdc_crt_mode_set(xf86OutputPtr output, DisplayModePtr mode, DisplayModePtr adjusted_mode);
RDC_STATIC void rdc_crt_restore (xf86OutputPtr output);
RDC_STATIC void rdc_crt_save (xf86OutputPtr output);
RDC_STATIC void rdc_hdmi_destroy (xf86OutputPtr output);
RDC_STATIC void rdc_hdmi_dpms(xf86OutputPtr output, int mode);
RDC_STATIC void rdc_hdmi_mode_set(xf86OutputPtr output, DisplayModePtr mode, DisplayModePtr adjusted_mode);
RDC_STATIC void rdc_hdmi_restore(xf86OutputPtr output);
RDC_STATIC void rdc_hdmi_save(xf86OutputPtr output);
RDC_STATIC xf86CrtcPtr rdc_crt_get_crtc(xf86OutputPtr output);
RDC_STATIC xf86CrtcPtr rdc_hdmi_get_crtc(xf86OutputPtr output);
RDC_STATIC xf86OutputStatus rdc_crt_detect(xf86OutputPtr output);
RDC_STATIC xf86OutputStatus rdc_hdmi_detect(xf86OutputPtr output);
#endif


#undef RDC_EXPORT
#define RDC_EXPORT
#undef RDC_STATIC
#define RDC_STATIC static
