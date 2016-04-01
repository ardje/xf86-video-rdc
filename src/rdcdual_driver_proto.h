/*
 * Prototypes for rdcdual_driver.c
 */
extern Bool RDCSwitchModeDual(int scrnIndex, DisplayModePtr mode, int flags);
extern void RDCAdjustFrameDual(int scrnIndex, int x, int y, int flags);
extern void RDCInitpScrnDual(ScrnInfoPtr pScrn);

#undef RDC_EXPORT
#define RDC_EXPORT
