/*
 * Prototypes for rdcdual_display.c
 */
extern Bool GetDisaplyStatus(ScrnInfoPtr pScrn);
extern void rdc_crtc_init(ScrnInfoPtr pScrn, int pipe);
extern void rdc_crt_init(ScrnInfoPtr pScrn);
extern void rdc_hdmi_init(pScrn);
extern void rdc_output_commit (xf86OutputPtr output);
extern void rdc_output_prepare (xf86OutputPtr output);

#undef RDC_EXPORT
#define RDC_EXPORT
